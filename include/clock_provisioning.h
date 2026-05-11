#ifndef CLOCK_PROVISIONING_H
#define CLOCK_PROVISIONING_H

#include <Arduino.h>

struct ClockConfig
{
  String ssid;
  String password;
  String city;

  bool isValid() const
  {
    const bool tzLooksValid = city.indexOf('/') > 0 && !city.endsWith("/");
    return ssid.length() > 0 &&
           password.length() > 0 &&
           city.length() > 0 &&
           tzLooksValid &&
           ssid != "YOUR_WIFI_SSID" &&
           password != "YOUR_WIFI_PASSWORD";
  }
};

void loadClockConfig(ClockConfig &config);
bool applyStaNetworkConfig();
bool connectConfiguredWifi(const ClockConfig &config, uint32_t timeoutMs);
String buildTimeApiUrlForCity(const String &city);
bool shouldForceProvisioning();
void runProvisioningPortalUntilConfigured();

#endif
