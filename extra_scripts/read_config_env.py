Import("env")
import os
import time as _time

project_dir = env["PROJECT_DIR"]
config_path = os.path.join(project_dir, "config.env")
legacy_env_path = os.path.join(project_dir, ".env")
defines = []


def _as_cpp_string_literal(value: str) -> str:
    escaped = value.replace("\\", "\\\\").replace('"', '\\"')
    return '\\"{}\\"'.format(escaped)

active_config_path = config_path
if not os.path.isfile(active_config_path) and os.path.isfile(legacy_env_path):
    active_config_path = legacy_env_path

if not os.path.isfile(active_config_path):
    print("config.env/.env not found - using built-in defaults")
else:
    with open(active_config_path, "r", encoding="utf-8") as f:
        for raw_line in f:
            line = raw_line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            key, _, value = line.partition("=")
            key = key.strip()
            value = value.strip()
            if key:
                defines.append((key, value))

# Generic defines from config.env
for key, value in defines:
    lowered_value = value.strip().lower()
    if lowered_value == "true":
        env.Append(CPPDEFINES=[(key, 1)])
    elif lowered_value == "false":
        env.Append(CPPDEFINES=[(key, 0)])
    elif value.isdigit() or (value.startswith("-") and value[1:].isdigit()):
        env.Append(CPPDEFINES=[(key, int(value))])
    else:
        # Non-numeric values must be emitted as C string literals.
        env.Append(CPPDEFINES=[(key, _as_cpp_string_literal(value))])

config_map = {k: v for k, v in defines}

# Hardware profile selector
hardware_profile = config_map.get("HARDWARE_PROFILE", "MASTER_NO_RTC").strip().upper()
if hardware_profile == "MASTER_NO_RTC":
    env.Append(CPPDEFINES=[("HARDWARE_MASTER_NO_RTC", 1)])
elif hardware_profile == "RTC_LOCAL":
    env.Append(CPPDEFINES=[("HARDWARE_RTC_LOCAL", 1)])
else:
    print("Unknown HARDWARE_PROFILE='{}' (valid: MASTER_NO_RTC, RTC_LOCAL)".format(hardware_profile))

# Tube selector
selected_tube = config_map.get("TUBE_TYPE", "ZM1000").strip().upper()
if selected_tube == "ZM1000":
    env.Append(CPPDEFINES=[("TUBE_TYPE_ZM1000", 1)])
elif selected_tube == "IN4":
    env.Append(CPPDEFINES=[("TUBE_TYPE_IN4", 1)])
elif selected_tube == "DA2000":
    env.Append(CPPDEFINES=[("TUBE_TYPE_DA2000", 1)])
else:
    print("Unknown TUBE_TYPE='{}' (valid: ZM1000, IN4, DA2000)".format(selected_tube))

print("{} loaded: {} key(s), HARDWARE_PROFILE={}, TUBE_TYPE={}".format(
    os.path.basename(active_config_path), len(defines), hardware_profile, selected_tube
))

# Unique build nonce for reliable "new firmware" detection in NVS.
# This changes on every build invocation, even when source files are unchanged.
build_nonce = str(_time.time_ns())
env.Append(CPPDEFINES=[("FW_BUILD_NONCE", _as_cpp_string_literal(build_nonce))])
print("FW_BUILD_NONCE={}".format(build_nonce))

# Touch clock_provisioning.cpp on every build so __DATE__ __TIME__ is always
# fresh. This guarantees each new flash produces a unique firmware ID in NVS,
# which automatically triggers the provisioning portal after every reflash.
# This keeps legacy __DATE__/__TIME__ fingerprints fresh as a fallback.
prov_src = os.path.join(project_dir, "src", "hardware", "clock_provisioning.cpp")
if os.path.isfile(prov_src):
    _time.sleep(0)  # no-op; touch via utime
    now = _time.time()
    os.utime(prov_src, (now, now))
    print("Touched clock_provisioning.cpp -> fresh fw_id on this flash")
