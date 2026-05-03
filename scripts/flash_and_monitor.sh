#!/usr/bin/env bash
# =============================================================================
# NixiTubeClock — Flash & Monitor Script
# =============================================================================
# Builds firmware, flashes it to the ESP32, and captures serial debug output.
# Called by the flash-and-monitor skill each time firmware needs to be updated.
#
# Usage:
#   ./scripts/flash_and_monitor.sh [OPTIONS]
#
# Options:
#   --build-only        Only build; do not flash or open monitor
#   --no-monitor        Build and flash; skip serial monitor
#   --monitor-only      Skip build/flash; open serial monitor only
#   --port <dev>        Override upload/monitor port (default: /dev/ttyUSB0)
#   --log <file>        Tee serial output to a file (default: no log file)
#   --lines <n>         Number of serial lines to capture then exit (default: live)
#   -h, --help          Show this help message
#
# Examples:
#   ./scripts/flash_and_monitor.sh
#   ./scripts/flash_and_monitor.sh --port /dev/ttyUSB1
#   ./scripts/flash_and_monitor.sh --build-only
#   ./scripts/flash_and_monitor.sh --no-monitor
#   ./scripts/flash_and_monitor.sh --monitor-only --log debug.log
#   ./scripts/flash_and_monitor.sh --lines 50 --log boot.log
# =============================================================================

set -euo pipefail

# -----------------------------------------------------------------------
# Defaults
# -----------------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PIO="$HOME/.platformio/penv/bin/platformio"
ENV="esp-wrover-kit"
PORT="/dev/ttyUSB0"
BAUD=115200
LOG_FILE=""
LINES=0          # 0 = live (no limit)
MODE="full"      # full | build-only | no-monitor | monitor-only

# -----------------------------------------------------------------------
# Argument parsing
# -----------------------------------------------------------------------
while [[ $# -gt 0 ]]; do
  case "$1" in
    --build-only)    MODE="build-only";   shift ;;
    --no-monitor)    MODE="no-monitor";   shift ;;
    --monitor-only)  MODE="monitor-only"; shift ;;
    --port)          PORT="$2";           shift 2 ;;
    --log)           LOG_FILE="$2";       shift 2 ;;
    --lines)         LINES="$2";          shift 2 ;;
    -h|--help)
      sed -n '4,30p' "$0"   # print the header comment block
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
# Print a section banner to visually separate build stages
banner() {
  echo ""
  echo "========================================================"
  echo "  $1"
  echo "========================================================"
}

# Confirm that config.env exists before doing anything
check_config() {
  local cfg="$PROJECT_ROOT/config.env"
  if [[ ! -f "$cfg" ]]; then
    echo "[ERROR] config.env not found at: $cfg"
    echo "        Copy config.env.example and fill in your settings:"
    echo "          cp config.env.example config.env"
    exit 1
  fi

  # Extract and display key settings so the user can confirm they look right
  banner "Current config.env settings"
  grep -E "^(HARDWARE_PROFILE|TUBE_TYPE|FORCE_PROVISIONING|PROVISION_AP_NAME|WIFI_SYNC_SECONDS|CITY)=" "$cfg" || true
  echo ""
}

# Check whether the upload port actually exists
check_port() {
  if [[ ! -e "$PORT" ]]; then
    echo "[WARN]  Port $PORT not found."
    echo "        Connected serial devices:"
    "$PIO" device list 2>/dev/null | grep -E "^/dev|Description" | head -20 || true
    echo ""
    echo "        Re-run with --port /dev/ttyUSBx to override."
    exit 1
  fi
}

# -----------------------------------------------------------------------
# Step 1: Build
# -----------------------------------------------------------------------
do_build() {
  banner "Building firmware (env: $ENV)"
  cd "$PROJECT_ROOT"
  "$PIO" run -e "$ENV"
}

# -----------------------------------------------------------------------
# Step 2: Flash
# -----------------------------------------------------------------------
do_flash() {
  banner "Flashing firmware to $PORT"
  echo ""
  echo "  *** MANUAL BOOT MODE REQUIRED ***"
  echo ""
  echo "  Your PCB has no auto-reset circuit, so you must enter"
  echo "  download mode manually before flashing:"
  echo ""
  echo "    1. Hold down the  BOOT  button"
  echo "    2. Press and release the  RST  button (keep BOOT held)"
  echo "    3. Release the  BOOT  button"
  echo "    4. The ESP32 is now in download mode (no output on serial)"
  echo ""
  read -rp "  Press ENTER here once the ESP32 is in download mode... "
  echo ""
  cd "$PROJECT_ROOT"
  # --before no_reset: skip esptool's auto-reset handshake (requires platformio.ini flag too)
  # --after hard_reset: reboot into normal firmware after flashing
  "$PIO" run -e "$ENV" -t upload --upload-port "$PORT"
}

# -----------------------------------------------------------------------
# Step 3: Serial monitor
# -----------------------------------------------------------------------
do_monitor() {
  banner "Opening serial monitor — $PORT @ ${BAUD} baud  (Ctrl+C to exit)"

  local monitor_cmd=("$PIO" device monitor --baud "$BAUD" --port "$PORT")

  if [[ -n "$LOG_FILE" ]]; then
    echo "[INFO]  Logging output to: $LOG_FILE"
    if [[ "$LINES" -gt 0 ]]; then
      # Capture a fixed number of lines, tee to file and stdout
      "${monitor_cmd[@]}" 2>&1 | head -n "$LINES" | tee "$LOG_FILE"
    else
      # Live output, also written to log file
      "${monitor_cmd[@]}" 2>&1 | tee "$LOG_FILE"
    fi
  else
    if [[ "$LINES" -gt 0 ]]; then
      "${monitor_cmd[@]}" 2>&1 | head -n "$LINES"
    else
      "${monitor_cmd[@]}"
    fi
  fi
}

# -----------------------------------------------------------------------
# Main execution
# -----------------------------------------------------------------------
check_config

case "$MODE" in
  build-only)
    do_build
    banner "Build complete — firmware NOT flashed"
    ;;

  no-monitor)
    check_port
    do_build
    do_flash
    banner "Flash complete — serial monitor NOT opened"
    ;;

  monitor-only)
    check_port
    do_monitor
    ;;

  full)
    check_port
    do_build
    do_flash
    # Firmware has a 3-second boot delay so the monitor connects in time.
    # Wait here only briefly since the monitor itself needs a moment to open.
    echo ""
    echo "[INFO]  Opening monitor — firmware will print output in ~3 s..."
    sleep 1
    do_monitor
    ;;
esac
