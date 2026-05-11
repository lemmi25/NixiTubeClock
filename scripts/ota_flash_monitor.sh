#!/usr/bin/env bash
# =============================================================================
# NixiTubeClock — OTA Flash & Monitor Script
# =============================================================================
# Builds firmware and uploads it wirelessly via espota (ArduinoOTA).
# No BOOT/RST button needed — the clock must already be running and on WiFi.
#
# Pre-requisites:
#   - ENABLE_OTA=1 in config.env
#   - Clock is powered on, connected to WiFi, and within the OTA window
#     (first OTA_WINDOW_SECONDS seconds after boot)
#   - Use either: --ip NixiClock.local (mDNS), or find IP from serial log / router
#
# Usage:
#   ./scripts/ota_flash_monitor.sh [OPTIONS]
#
# Options:
#   --ip <addr>         Override OTA_IP from config.env
#   --password <pw>     Override OTA_PASSWORD from config.env
#   --port <n>          Override OTA_PORT from config.env (default 3232)
#   --serial <dev>      Serial device for debug monitor (default /dev/ttyUSB0)
#   --baud <n>          Serial baud rate (default 115200)
#   --build-only        Only build; do not flash
#   --no-monitor        Build and flash; skip serial monitor after upload
#   --log <file>        Tee serial output to a file
#   -h, --help          Show this help message
#
# Examples:
#   ./scripts/ota_flash_monitor.sh
#   ./scripts/ota_flash_monitor.sh --ip NixiClock.local
#   ./scripts/ota_flash_monitor.sh --ip 192.168.1.42
#   ./scripts/ota_flash_monitor.sh --ip 192.168.1.42 --no-monitor
#   ./scripts/ota_flash_monitor.sh --build-only
# =============================================================================

set -euo pipefail

# -----------------------------------------------------------------------
# Defaults (overridden by config.env, then CLI flags)
# -----------------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PIO="$HOME/.platformio/penv/bin/platformio"
ENV="esp-wrover-kit-ota"
CONFIG="$PROJECT_ROOT/config.env"

OTA_IP=""
OTA_PASSWORD=""
OTA_PORT="3232"
SERIAL_PORT="/dev/ttyUSB0"
BAUD=115200
LOG_FILE=""
MODE="full"   # full | build-only | no-monitor

# -----------------------------------------------------------------------
# Load defaults from config.env
# -----------------------------------------------------------------------
if [[ -f "$CONFIG" ]]; then
  _read_cfg() { grep -E "^$1=" "$CONFIG" 2>/dev/null | tail -1 | cut -d= -f2- | tr -d '[:space:]'; }
  OTA_IP="$(_read_cfg OTA_IP)"
  OTA_PASSWORD="$(_read_cfg OTA_PASSWORD)"
  OTA_PORT="$(_read_cfg OTA_PORT)"
  BAUD="$(_read_cfg "monitor_speed" || echo 115200)"
  # Fall back to 115200 if monitor_speed not in config.env
  [[ -z "$BAUD" ]] && BAUD=115200
fi

# -----------------------------------------------------------------------
# Argument parsing (CLI overrides config.env)
# -----------------------------------------------------------------------
while [[ $# -gt 0 ]]; do
  case "$1" in
    --ip)          OTA_IP="$2";       shift 2 ;;
    --password)    OTA_PASSWORD="$2"; shift 2 ;;
    --port)        OTA_PORT="$2";     shift 2 ;;
    --serial)      SERIAL_PORT="$2";  shift 2 ;;
    --baud)        BAUD="$2";         shift 2 ;;
    --log)         LOG_FILE="$2";     shift 2 ;;
    --build-only)  MODE="build-only"; shift ;;
    --no-monitor)  MODE="no-monitor"; shift ;;
    -h|--help)
      sed -n '4,42p' "$0"
      exit 0
      ;;
    *)
      echo "[ERROR] Unknown option: $1"
      echo "        Run with --help for usage."
      exit 1
      ;;
  esac
done

# -----------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------
banner() {
  echo ""
  echo "========================================================"
  echo "  $1"
  echo "========================================================"
}

# -----------------------------------------------------------------------
# Validation
# -----------------------------------------------------------------------
validate() {
  banner "OTA flash settings"
  echo "  Target IP   : ${OTA_IP:-<not set>}"
  echo "  OTA port    : $OTA_PORT"
  echo "  Password    : ${OTA_PASSWORD:+***set***}${OTA_PASSWORD:-<none>}"
  echo "  PIO env     : $ENV"
  echo "  Serial port : $SERIAL_PORT @ ${BAUD} baud"
  echo ""

  if [[ "$MODE" != "build-only" && -z "$OTA_IP" ]]; then
    echo "[ERROR] OTA target IP/hostname not specified."
    echo ""
    echo "        Provide one of these:"
    echo "          ./scripts/ota_flash_monitor.sh --ip NixiClock.local"
    echo "          ./scripts/ota_flash_monitor.sh --ip 192.168.10.XX"
    echo ""
    echo "        To find your clock's IP:"
    echo "          1. Open serial monitor: platformio device monitor"
    echo "          2. Look for: [OTA] Ready at 192.168.x.x for 180s"
    echo "          3. Use that IP: --ip 192.168.x.x"
    echo ""
    echo "        Or try mDNS hostname (easiest): --ip NixiClock.local"
    exit 1
  fi

  if [[ "$MODE" != "build-only" ]]; then
    echo "[INFO]  Pinging $OTA_IP ..."
    if ! ping -c 1 -W 2 "$OTA_IP" &>/dev/null; then
      echo "[WARN]  $OTA_IP did not respond to ping."
      echo "        Make sure the clock is powered on and within the OTA window."
      echo "        Continuing anyway — espota will give a clearer error if unreachable."
      echo ""
    else
      echo "[OK]    $OTA_IP is reachable."
    fi
  fi
}

# -----------------------------------------------------------------------
# Step 1: Build
# -----------------------------------------------------------------------
do_build() {
  banner "Building firmware (env: $ENV)"
  cd "$PROJECT_ROOT"
  "$PIO" run -e "$ENV" -t clean
  "$PIO" run -e "$ENV"
}

# -----------------------------------------------------------------------
# Step 2: OTA upload
# -----------------------------------------------------------------------
do_ota_upload() {
  banner "Uploading via OTA to $OTA_IP:$OTA_PORT"
  cd "$PROJECT_ROOT"

  # Find espota.py (comes with PlatformIO)
  local espota_path=$(python3 -c "import platformio; print(platformio.__file__)" 2>/dev/null | xargs dirname)
  espota_path="$HOME/.platformio/packages/framework-arduinoespressif32/tools/espota.py"

  if [[ ! -f "$espota_path" ]]; then
    # Try alternative location
    espota_path=$(find "$HOME/.platformio" -name "espota.py" -type f | head -1)
  fi

  if [[ ! -f "$espota_path" ]]; then
    echo "[ERROR] espota.py not found. PlatformIO tools may not be installed correctly."
    exit 1
  fi

  local fw_bin=".pio/build/esp-wrover-kit-ota/firmware.bin"
  
  if [[ ! -f "$fw_bin" ]]; then
    echo "[ERROR] Firmware binary not found: $fw_bin"
    exit 1
  fi

  # Call espota.py directly with proper argument separation
  python3 "$espota_path" -i "$OTA_IP" -p "$OTA_PORT" -f "$fw_bin" ${OTA_PASSWORD:+-a "$OTA_PASSWORD"}

  echo ""
  echo "[OTA]   Upload complete. The clock will reboot automatically."
  echo "        Opening serial monitor in 5 seconds..."
  sleep 5
}

# -----------------------------------------------------------------------
# Step 3: Serial monitor (shows debug output after OTA reboot)
# -----------------------------------------------------------------------
do_monitor() {
  banner "Serial monitor — $SERIAL_PORT @ ${BAUD} baud  (Ctrl+C to exit)"

  if [[ ! -e "$SERIAL_PORT" ]]; then
    echo "[WARN]  Serial port $SERIAL_PORT not found — skipping monitor."
    echo "        Use --serial /dev/ttyUSBx to specify the correct port."
    return
  fi

  local monitor_cmd=("$PIO" device monitor --baud "$BAUD" --port "$SERIAL_PORT")

  if [[ -n "$LOG_FILE" ]]; then
    echo "[INFO]  Logging output to: $LOG_FILE"
    "${monitor_cmd[@]}" 2>&1 | tee "$LOG_FILE"
  else
    "${monitor_cmd[@]}"
  fi
}

# -----------------------------------------------------------------------
# Main
# -----------------------------------------------------------------------
validate

case "$MODE" in
  build-only)
    do_build
    banner "Build complete — firmware NOT uploaded"
    ;;
  no-monitor)
    do_build
    do_ota_upload
    banner "OTA upload complete — serial monitor NOT opened"
    ;;
  full)
    do_build
    do_ota_upload
    do_monitor
    ;;
esac
