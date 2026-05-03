Import("env")
import os

config_path = os.path.join(env["PROJECT_DIR"], "config.env")
defines = []

if not os.path.isfile(config_path):
    print("config.env not found - using built-in defaults")
else:
    with open(config_path, "r", encoding="utf-8") as f:
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
    if value.isdigit() or (value.startswith("-") and value[1:].isdigit()):
        env.Append(CPPDEFINES=[(key, int(value))])
    else:
        env.Append(CPPDEFINES=[(key, value)])

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

print("config.env loaded: {} key(s), HARDWARE_PROFILE={}, TUBE_TYPE={}".format(
    len(defines), hardware_profile, selected_tube
))
