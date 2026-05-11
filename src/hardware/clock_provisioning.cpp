// Clock Provisioning Module for WiFi Configuration via Captive Portal
// This module provides a web-based setup interface that allows users to:
// - Configure WiFi credentials (SSID & password)
// - Set their timezone/city for time API
// - Validate configurations before saving
// Features include DNS spoofing (captive portal) and HTTP redirects to auto-open setup page

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <clock_provisioning.h>
#include <nixiDriver.h>
#include <clock_variant_config.h>

// Default AP (Access Point) name that appears when clock needs WiFi configuration
#ifndef PROVISION_AP_NAME
#define PROVISION_AP_NAME "NixiClockSetup"
#endif

// Default WiFi SSID (overridden by provisioning portal)
#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif

// Default WiFi password (overridden by provisioning portal)
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

// Default city for timezone API (overridden by provisioning portal)
#ifndef CITY
#define CITY ""
#endif

// Base URL for time API (worldtimeapi.org)
// IMPORTANT: Users must enter timezone in format "Continent/City" (e.g., "Europe/Berlin", "America/New_York", "Asia/Tokyo")
// The city parameter will be appended to this base URL in buildTimeApiUrlForCity()
// Supported continents: Africa, America, Asia, Atlantic, Australia, Europe, Indian, Pacific
#ifndef TIME_API_BASE
#define TIME_API_BASE "https://timeapi.io/api/Time/current/zone?timeZone="
#endif

#ifndef WIFI_USE_STATIC_IP
#define WIFI_USE_STATIC_IP 0
#endif

#ifndef WIFI_STATIC_IP
#define WIFI_STATIC_IP ""
#endif

#ifndef WIFI_STATIC_GATEWAY
#define WIFI_STATIC_GATEWAY ""
#endif

#ifndef WIFI_STATIC_SUBNET
#define WIFI_STATIC_SUBNET ""
#endif

#ifndef WIFI_STATIC_DNS1
#define WIFI_STATIC_DNS1 ""
#endif

#ifndef WIFI_STATIC_DNS2
#define WIFI_STATIC_DNS2 ""
#endif

// NVS (Non-Volatile Storage) namespace for saving persistent clock config
static const char *PREF_NAMESPACE = "clockcfg";
static const char *PREF_PROVISION_DONE_KEY = "prov_done";

static void renderProvisioningIndicator(nixiDriver &display, bool visible)
{
  // Blink 0000 while captive portal is active to make AP mode obvious on-device.
  if (visible)
  {
    display.writeSegment(0, SEGMENT_1);
    display.writeSegment(0, SEGMENT_2);
    display.writeSegment(0, SEGMENT_3);
    display.writeSegment(0, SEGMENT_4);
  }
  else
  {
    display.off();
  }
}

// Sanitize city name for use in API URL
// Trims whitespace and replaces spaces with underscores (e.g., "New York" -> "New_York")
static String sanitizeCity(const String &city)
{
  String out = city;
  out.trim();          // Remove leading/trailing whitespace
  out.replace(" ", "_"); // Convert spaces to underscores for URL compatibility
  return out;
}

// Save WiFi credentials and city to non-volatile storage (NVS/flash)
// Data persists across reboots
static void saveClockConfig(const ClockConfig &config)
{
  Preferences prefs;
  prefs.begin(PREF_NAMESPACE, false); // false = read-write mode
  prefs.putString("ssid", config.ssid);
  prefs.putString("pass", config.password);
  prefs.putString("city", sanitizeCity(config.city));
  // Mark provisioning as completed.
  prefs.putBool(PREF_PROVISION_DONE_KEY, true);
  prefs.end(); // Close and commit to flash
}

// Load WiFi credentials and city from non-volatile storage (NVS/flash)
// Falls back to compile-time defaults if nothing saved
void loadClockConfig(ClockConfig &config)
{
  Preferences prefs;
  // Use read-write open so first boot does not log nvs_open NOT_FOUND for missing namespace.
  prefs.begin(PREF_NAMESPACE, false);
  config.ssid = prefs.getString("ssid", "");       // Empty string if not found
  config.password = prefs.getString("pass", "");   // Empty string if not found
  config.city = prefs.getString("city", "");       // Empty string if not found
  prefs.end();

  // Apply compile-time defaults if nothing was loaded from NVS
  if (config.ssid.length() == 0)
  {
    config.ssid = WIFI_SSID;
  }
  if (config.password.length() == 0)
  {
    config.password = WIFI_PASSWORD;
  }
  if (config.city.length() == 0)
  {
    config.city = sanitizeCity(CITY);
  }
}

bool applyStaNetworkConfig()
{
#if WIFI_USE_STATIC_IP
  IPAddress ip;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns1;
  IPAddress dns2;

  const bool valid =
      ip.fromString(WIFI_STATIC_IP) &&
      gateway.fromString(WIFI_STATIC_GATEWAY) &&
      subnet.fromString(WIFI_STATIC_SUBNET) &&
      dns1.fromString(WIFI_STATIC_DNS1);

  if (!valid)
  {
    Serial.println("[NET] Static IP enabled but config is invalid. Check WIFI_STATIC_* in config.env");
    return false;
  }

  const String dns2Str = WIFI_STATIC_DNS2;
  const bool hasDns2 = dns2Str.length() > 0 && dns2.fromString(dns2Str);

  bool ok;
  if (hasDns2)
    ok = WiFi.config(ip, gateway, subnet, dns1, dns2);
  else
    ok = WiFi.config(ip, gateway, subnet, dns1);

  if (!ok)
  {
    Serial.println("[NET] Failed to apply static IP config");
    return false;
  }

  Serial.printf("[NET] Static IP: %s  GW: %s  SN: %s\n",
                ip.toString().c_str(), gateway.toString().c_str(), subnet.toString().c_str());
#else
  // Ensure DHCP mode when static IP is disabled.
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
#endif
  return true;
}

// Attempt to connect to WiFi using saved credentials
// Returns true if connection successful, false if timeout
bool connectConfiguredWifi(const ClockConfig &config, uint32_t timeoutMs)
{
  // Already connected? Just return success
  if (WiFi.status() == WL_CONNECTED)
  {
    return true;
  }

  // Switch to Station (STA) mode and connect with provided credentials
  WiFi.mode(WIFI_STA);
  if (!applyStaNetworkConfig())
  {
    return false;
  }
  WiFi.begin(config.ssid.c_str(), config.password.c_str());

  // Poll connection status for up to timeoutMs milliseconds
  const unsigned long started = millis();
  while (millis() - started < timeoutMs)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true; // Success!
    }
    delay(250); // Wait before next status check
  }

  return false; // Timeout - connection failed
}

// Build complete URL for time API request
// IMPORTANT: The city input MUST include continent prefix in format "Continent/City"
// Examples:
//   buildTimeApiUrlForCity("Europe/Berlin") -> "https://worldtimeapi.org/api/timezone/Europe/Berlin.json"
//   buildTimeApiUrlForCity("America/New_York") -> "https://worldtimeapi.org/api/timezone/America/New_York.json"
//   buildTimeApiUrlForCity("Asia/Tokyo") -> "https://worldtimeapi.org/api/timezone/Asia/Tokyo.json"
//   buildTimeApiUrlForCity("Australia/Sydney") -> "https://worldtimeapi.org/api/timezone/Australia/Sydney.json"
// List of all supported timezones: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
//   buildTimeApiUrlForCity("Europe/Berlin") -> "https://timeapi.io/api/Time/current/zone?timeZone=Europe/Berlin"
//   buildTimeApiUrlForCity("America/New_York") -> "https://timeapi.io/api/Time/current/zone?timeZone=America/New_York"
String buildTimeApiUrlForCity(const String &city)
{
  String safeCity = sanitizeCity(city); // Replace spaces with underscores
  String url = TIME_API_BASE;
  if (url.endsWith("="))
  {
    url += safeCity;
    return url;
  }
  if (!url.endsWith("/"))
  {
    url += "/"; // Ensure base URL ends with /
  }
  url += safeCity;
  if (url.indexOf("worldtimeapi.org") >= 0)
  {
    url += ".json";
  }
  return url;
}

// Decide whether the provisioning portal should run on this boot.
//
// Reflash detection: every new build has a unique __DATE__ __TIME__ stamp.
// When a new firmware is detected, prov_done is cleared so the portal opens
// automatically, regardless of what was saved before.
bool shouldForceProvisioning()
{
  // Step 1: detect new firmware via build fingerprint.
  // FW_BUILD_NONCE is injected by extra_scripts/read_config_env.py on every
  // build invocation, making reflash detection robust even if sources did not
  // change between uploads.
#ifdef FW_BUILD_NONCE
  static const char *currentFwId = FW_BUILD_NONCE;
#else
  static const char *currentFwId = __DATE__ " " __TIME__;
#endif

  Preferences prefs;
  prefs.begin(PREF_NAMESPACE, false); // read-write to update fw_id when needed
  const String storedFwId = prefs.getString("fw_id", "");

  if (storedFwId != currentFwId)
  {
    // New firmware detected: store the new ID and clear the provisioning flag
    // so the setup portal opens on this boot.
    Serial.printf("[PROV] New firmware (prev: '%s') -> reset prov_done\n",
                  storedFwId.c_str());
    prefs.putString("fw_id", currentFwId);
    prefs.putBool(PREF_PROVISION_DONE_KEY, false);
    prefs.end();
    return true; // always portal on fresh flash
  }

  // Step 2: not a new firmware, do not force portal here.
  // Runtime connection-failure logic in profile code decides fallback AP behavior.
  prefs.end();
  return false;
}

// Simple DNS server for captive portal
// This server spoofs DNS responses by answering all DNS queries with the AP's IP address
// When user's device connects to the WiFi hotspot, it tries to detect internet connectivity
// by sending DNS queries (e.g., for captive portal detection). This server intercepts those
// queries and redirects them back to the clock's web server, triggering the setup page.
class CaptiveDNSServer
{
private:
  WiFiUDP udp;           // UDP socket for DNS communication
  uint16_t port;         // DNS port (always 53)
  IPAddress respondIP;   // AP IP address to respond with
  bool running;          // Is DNS server active?

public:
  CaptiveDNSServer() : port(53), running(false) {}

  // Start DNS server on the specified IP
  void start(IPAddress apIP)
  {
    respondIP = apIP;
    udp.begin(port);
    running = true;
    Serial.println("DNS server started on port 53");
  }

  // Stop DNS server
  void stop()
  {
    running = false;
    udp.stop();
  }

  // Handle incoming DNS requests (call this in main loop)
  void handleRequest()
  {
    if (!running)
      return;

    // Check if there's a DNS query waiting
    int packetSize = udp.parsePacket();
    if (packetSize <= 0)
      return; // No packet, nothing to do

    // Read the DNS query packet
    uint8_t buffer[512];
    int len = udp.read(buffer, 512);
    if (len < 12)
      return; // DNS header is at least 12 bytes; if smaller, it's invalid

    // Build DNS response: copy the query and add our response
    uint8_t response[512];
    memcpy(response, buffer, len); // Copy query as-is

    // Set response flags:
    // bit 15 (byte 2, bit 7) = QR (0=query, 1=response)
    // bits 11-13 (byte 3, bits 3-5) = RCODE (0=no error)
    response[2] |= 0x80; // Set QR bit to mark as response
    response[3] &= 0x0F; // Clear other flags, keep only RCODE=0

    // Append answer section to response
    int answerOffset = len;

    // Add one A (address) record with the AP's IP
    // Format: compressed name pointer, type=A, class=IN, TTL, RDLEN, RDATA(IP)
    response[answerOffset++] = 0xC0; // Pointer (compression): points to name at offset 12 (query section)
    response[answerOffset++] = 0x0C; // Offset 12 in the DNS packet
    response[answerOffset++] = 0x00; // Type field high byte (0x0001 = type A)
    response[answerOffset++] = 0x01; // Type field low byte (A record)
    response[answerOffset++] = 0x00; // Class field high byte (0x0001 = class IN)
    response[answerOffset++] = 0x01; // Class field low byte (IN = internet)
    response[answerOffset++] = 0x00; // TTL field: 0x00000000 (4 bytes, high byte first)
    response[answerOffset++] = 0x00;
    response[answerOffset++] = 0x00;
    response[answerOffset++] = 0x00;
    response[answerOffset++] = 0x00; // RDLENGTH: 4 bytes (IPv4 address is 4 bytes)
    response[answerOffset++] = 0x04;

    // Append the AP IP address (4 bytes)
    response[answerOffset++] = respondIP[0];
    response[answerOffset++] = respondIP[1];
    response[answerOffset++] = respondIP[2];
    response[answerOffset++] = respondIP[3];

    // Update ANCOUNT (answer count) in response header
    response[6] = 0x00; // ANCOUNT high byte
    response[7] = 0x01; // ANCOUNT low byte (1 answer)

    // Send response back to the client
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(response, answerOffset);
    udp.endPacket();
  }
};

// Global DNS server instance
static CaptiveDNSServer dnsServer;
static ClockConfig pendingConfig;
static String pendingFormattedTime;
static bool hasPendingConfig = false;

// Convert API datetime into a compact user-facing format: YYYY-MM-DD HH:MM:SS
static String formatDateTimeForDisplay(const String &raw)
{
  if (raw.length() >= 19 && raw[4] == '-' && raw[7] == '-' && raw[10] == 'T')
  {
    return raw.substring(0, 10) + " " + raw.substring(11, 19);
  }
  return raw;
}

// Fetch the current API time for the given config and return both raw and formatted values.
static bool fetchApiTimeForConfig(const ClockConfig &config, String &rawTime, String &formattedTime, int &httpCodeOut)
{
  HTTPClient http;
  WiFiClientSecure secureClient;
  String apiUrl = buildTimeApiUrlForCity(config.city);

  secureClient.setInsecure();
  http.begin(secureClient, apiUrl);
  httpCodeOut = http.GET();

  if (httpCodeOut != 200)
  {
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    return false;
  }

  const char *datetime = doc["datetime"];
  if (datetime == nullptr)
  {
    datetime = doc["dateTime"];
  }
  if (datetime == nullptr)
  {
    return false;
  }

  rawTime = String(datetime);
  formattedTime = formatDateTimeForDisplay(rawTime);
  return true;
}

// Main provisioning portal function
// Creates WiFi hotspot with web server for configuration
// Shows setup form, validates inputs, tests WiFi/API connection, then reboots
// This function blocks until configuration is complete
void runProvisioningPortalUntilConfigured()
{
  static nixiDriver provisioningDisplay(4, 5, 2, CLOCK_IS_NUMITRON);

  // Setup WiFi in AP (Access Point) mode - create a hotspot
  WiFi.mode(WIFI_AP);
  WiFi.softAP(PROVISION_AP_NAME); // Create open AP (no password)
  IPAddress apIP = WiFi.softAPIP(); // Usually 192.168.4.1

  // Print provisioning info to serial monitor
  Serial.println("Entering provisioning mode");
  Serial.print("AP SSID: ");
  Serial.println(PROVISION_AP_NAME);
  Serial.print("AP IP: ");
  Serial.println(apIP);

  // Start DNS server for captive portal (intercepts all DNS queries)
  dnsServer.start(apIP);

  // Create web server on port 80 (HTTP)
  WebServer server(80);

  // Route 1: Root path (/) - Redirect to /save to show setup form
  // Status 302 = Found (temporary redirect)
  server.on("/", HTTP_GET, [&server]() {
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  // Route 2: Setup form (/save GET) - Display the HTML configuration form
  // Users see this form when they first connect to the NixiClock WiFi hotspot
  server.on("/save", HTTP_GET, [&server]() {
    // Build HTML form for user input with helpful instructions and examples
    String html;
    html += "<!doctype html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>NixiClock Setup</title></head><body style='font-family:sans-serif;max-width:480px;margin:24px auto;padding:0 12px;'>";
    html += "<h2>NixiClock WiFi Setup</h2>";
    html += "<p>Configure your clock to connect to your WiFi network and set your timezone.</p>";
#if WIFI_USE_STATIC_IP
    html += "<div style='background:#e3f2fd;padding:12px;border-radius:4px;margin-bottom:20px;border-left:4px solid #2196F3;'>";
    html += "<strong>📍 Clock Network Address:</strong><br>";
    html += "After setup, access your clock at:<br>";
    html += "<code style='background:#f5f5f5;padding:4px 6px;font-size:14px;font-weight:bold;font-family:monospace;'>" + String(WIFI_STATIC_IP) + "</code>";
    html += "</div>";
#endif
    html += "<form method='POST' action='/save'>";
    html += "<label><strong>WiFi SSID</strong> (your router/network name)</label><br>";
    html += "<input name='ssid' placeholder='e.g., MyHomeWiFi' style='width:100%;padding:8px;margin-bottom:10px;box-sizing:border-box;' required><br>";
    html += "<label><strong>WiFi Password</strong></label><br>";
    html += "<input type='password' name='password' placeholder='Your WiFi password' style='width:100%;padding:8px;margin-bottom:10px;box-sizing:border-box;' required><br>";
    html += "<label><strong>Timezone</strong> (format: Continent/City)</label><br>";
    html += "<input name='city' placeholder='e.g., Europe/Berlin' style='width:100%;padding:8px;margin-bottom:14px;box-sizing:border-box;' required><br>";
    html += "<button type='submit' style='padding:10px 16px;width:100%;cursor:pointer;background:#4CAF50;color:white;border:none;border-radius:4px;font-size:16px;font-weight:bold;'>Save & Test Connection</button>";
    html += "</form>";
    html += "<div style='background:#f0f0f0;padding:12px;border-radius:4px;margin-top:20px;font-size:12px;'>";
    html += "<strong>Timezone Examples:</strong><br>";
    html += "Europe: Europe/Berlin, Europe/London, Europe/Paris<br>";
    html += "America: America/New_York, America/Chicago, America/Los_Angeles<br>";
    html += "Asia: Asia/Tokyo, Asia/Shanghai, Asia/Dubai<br>";
    html += "Australia: Australia/Sydney, Australia/Melbourne<br>";
    html += "<a href='https://en.wikipedia.org/wiki/List_of_tz_database_time_zones' target='_blank' style='color:#0066cc;'>View full timezone list →</a>";
    html += "</div>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // Route 3: Form submission (/save POST) - Validate, test WiFi/API, and save configuration
  // This is the core of the setup process - it validates user input and performs real tests:
  // 1. Validates form fields are filled (SSID, password, timezone)
  // 2. Attempts to connect to the user's WiFi router using provided credentials
  // 3. Tests the time API using the specified timezone
  // 4. Displays success/failure results with actual time from API
  // 5. Only saves configuration if BOTH WiFi and API tests pass
  server.on("/save", HTTP_POST, [&server]() {
    // Extract form data from HTTP POST request
    ClockConfig cfg;
    cfg.ssid = server.arg("ssid");       // Get WiFi SSID from form field named "ssid"
    cfg.password = server.arg("password"); // Get WiFi password from form field named "password"
    cfg.city = server.arg("city");       // Get timezone from form field named "city" (e.g., "Europe/Berlin")
    cfg.city.trim();                     // Remove leading/trailing whitespace from timezone input

    // Basic validation: ensure all fields are filled
    // This prevents saving empty or incomplete configurations
    if (!cfg.isValid())
    {
      server.send(400, "text/plain", "Error: Please fill in valid values:\n- WiFi SSID (your network name)\n- WiFi Password\n- Timezone in Continent/City format (example: Europe/Berlin)");
      return;  // Stop processing and ask user to fill in all fields
    }

    // Test WiFi connection with provided credentials
    Serial.println("Testing WiFi connection...");
    WiFi.mode(WIFI_AP_STA); // Enable both AP and STA (station) modes
    if (!applyStaNetworkConfig())
    {
      server.send(400, "text/plain", "Error: Static IP config invalid. Check WIFI_STATIC_* in config.env");
      return;
    }
    WiFi.begin(cfg.ssid.c_str(), cfg.password.c_str()); // Connect to user's router

    // Status messages to display on confirmation page
    String wifiStatus = "[FAIL] Failed to connect to WiFi";
    String timeStatus = "[SKIP] Skipped (WiFi not available)";
    String currentTime = "";
    String formattedTime = "";
    bool wifiOk = false; // WiFi connection success flag
    bool timeOk = false; // API call success flag

    // Wait up to 8 seconds for WiFi to connect
    uint32_t wifiStart = millis();
    while (millis() - wifiStart < 8000)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        wifiOk = true;
        wifiStatus = "[OK] Connected to WiFi: " + cfg.ssid;
        Serial.print("Connected to ");
        Serial.println(cfg.ssid);
        break; // Connection successful, exit loop
      }
      delay(500); // Check every 500ms
    }

    // If WiFi connection worked, test the time API
    if (wifiOk)
    {
      Serial.println("Testing API call...");
      HTTPClient http;
      WiFiClientSecure secureClient;
      String apiUrl = buildTimeApiUrlForCity(cfg.city); // Build URL with city name

      secureClient.setInsecure(); // Keep setup simple on embedded targets; avoids CA bundle management.
      secureClient.setTimeout(5000);
      http.setConnectTimeout(5000);
      http.setTimeout(7000);
      http.begin(secureClient, apiUrl);
      int httpCode = http.GET(); // Make HTTP request to time API

      // Check if API returned success (HTTP 200)
      if (httpCode == 200)
      {
        String payload = http.getString(); // Get response body
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, payload); // Parse JSON

        // If JSON parsing successful, extract datetime field
        if (!error)
        {
          const char *datetime = doc["datetime"];
          if (datetime == nullptr)
          {
            datetime = doc["dateTime"];
          }
          if (datetime)
          {
            // API returned valid datetime
            currentTime = String(datetime);
            formattedTime = formatDateTimeForDisplay(currentTime);
            timeOk = true;
            timeStatus = "[OK] Time API working for " + cfg.city;
            Serial.print("Current time from API: ");
            Serial.println(currentTime);
          }
          else
          {
            timeStatus = "[FAIL] API response missing datetime field";
          }
        }
        else
        {
          timeStatus = "[FAIL] Failed to parse API response";
        }
      }
      else
      {
        // API error (wrong city, no internet, etc.)
        timeStatus = "[FAIL] API error (HTTP " + String(httpCode) + ")";
      }

      http.end(); // Close HTTP connection
    }

    // Build HTML confirmation page showing test results
    String html;
    html += "<!doctype html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>Setup Result</title>";
    html += "<style>"; // CSS styling for result page
    html += "body { font-family:sans-serif; max-width:480px; margin:24px auto; padding:0 12px; }";
    html += ".ok { color:#155724; background:#d4edda; padding:12px; border-radius:4px; margin:10px 0; }"; // Green for success
    html += ".fail { color:#721c24; background:#f8d7da; padding:12px; border-radius:4px; margin:10px 0; }"; // Red for error
    html += ".skip { color:#856404; background:#fff3cd; padding:12px; border-radius:4px; margin:10px 0; }"; // Yellow for skipped
    html += ".time-display { background:#f0f0f0; padding:16px; border-radius:8px; margin:10px 0; text-align:center; }";
    html += ".time-main { font-size:28px; font-weight:700; letter-spacing:1px; font-family:monospace; }";
    html += ".time-meta { font-size:14px; color:#444; margin-top:6px; }";
    html += ".action-btn { padding:12px 16px; width:100%; cursor:pointer; background:#2e7d32; color:white; border:none; border-radius:6px; font-size:16px; font-weight:bold; }";
    html += "</style>";
    html += "</head><body>";
    html += "<h2>Setup Confirmation</h2>";

    // Show WiFi connection result
    html += (wifiOk ? "<div class='ok'>" : "<div class='fail'>");
    html += wifiStatus;
    html += "</div>";

    // Show API/time test result
    if (wifiOk)
    {
      html += (timeOk ? "<div class='ok'>" : "<div class='fail'>"); // Color depends on success
    }
    else
    {
      html += "<div class='skip'>"; // Skip API test if no WiFi
    }
    html += timeStatus;
    html += "</div>";

    // Display the current time received from API (if available)
    if (!formattedTime.isEmpty())
    {
      html += "<p><strong>Detected Clock Time</strong></p>";
      html += "<div class='time-display'>";
      html += "<div class='time-main'>" + formattedTime + "</div>";
      html += "<div class='time-meta'>Timezone: " + sanitizeCity(cfg.city) + "</div>";
      html += "</div>";
    }

    // If all tests passed: ask user to confirm time before applying settings
    if (wifiOk && timeOk)
    {
      pendingConfig = cfg;
      pendingConfig.city = sanitizeCity(cfg.city);
      pendingFormattedTime = formattedTime;
      hasPendingConfig = true;

      html += "<p style='color:green;font-size:18px;'><strong>[OK] All checks passed!</strong></p>";
      html += "<p>Please confirm the detected time. Settings are applied only after confirmation.</p>";
      html += "<form method='POST' action='/apply'>";
      html += "<button type='submit' class='action-btn'>Time Is Correct - Apply Settings</button>";
      html += "</form>";
      server.send(200, "text/html", html);
    }
    else
    {
      // At least one test failed - configuration NOT saved
      // User must correct their input and try again
      html += "<p style='color:red;font-size:18px;'><strong>[FAIL] Setup Failed</strong></p>";
      html += "<p>Please check your entries:</p>";
      html += "<ul>";
      if (!wifiOk)
      {
        html += "<li><strong>WiFi Connection Failed:</strong> Check SSID and password are correct</li>";
      }
      if (!timeOk)
      {
        html += "<li><strong>Time API Failed:</strong> Use format like 'Europe/Berlin' not just 'Berlin'</li>";
      }
      html += "</ul>";
      html += "<p><a href='/save' style='color:#4CAF50;text-decoration:none;font-weight:bold;'>Go Back and Try Again</a></p>";
      server.send(200, "text/html", html);
      WiFi.mode(WIFI_AP); // Return to AP-only mode (stop attempting STA connection)
    }
  });

  // Route 4: Apply configuration after explicit user confirmation
  // This route saves config, shuts down setup AP, switches to station mode, and reboots.
  server.on("/apply", HTTP_POST, [&server]() {
    if (!hasPendingConfig)
    {
      server.send(400, "text/plain", "No pending setup. Please run Save and Test first.");
      return;
    }

    // Save config to NVS (also sets prov_done=true so AP won't reopen after reboot)
    saveClockConfig(pendingConfig);
    hasPendingConfig = false;

    Serial.println("[PROV] Config saved, rebooting to connect to saved WiFi...");
    Serial.flush();

    // Send confirmation page WHILE the AP is still alive so the browser can receive it.
    // After the reboot the AP is gone automatically - no manual teardown needed.
    String html;
    html += "<!doctype html><html><head><meta charset='utf-8'>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>Setup Complete</title></head>";
    html += "<body style='font-family:sans-serif;max-width:480px;margin:24px auto;padding:0 12px;'>";
    html += "<h2>Setup Complete</h2>";
    html += "<p style='color:green;font-size:1.1em;'><strong>[OK]</strong> Settings saved successfully.</p>";
    html += "<p>Confirmed time: <strong>" + pendingFormattedTime + "</strong></p>";
    html += "<p>The clock is now rebooting and will connect directly to <strong>" + pendingConfig.ssid + "</strong>.</p>";
    html += "<p>The <strong>NixiClockSetup</strong> hotspot will disappear in a moment.</p>";
    
#if ENABLE_OTA
    html += "<div style='background:#f0f8ff;border:1px solid #87ceeb;border-radius:4px;padding:12px;margin:12px 0;'>";
    html += "<h3 style='margin-top:0;color:#0066cc;'>OTA Update Available</h3>";
  html += "<p>After the clock connects to WiFi, OTA (Over-The-Air) firmware updates will stay <strong>always available</strong>.</p>";
    html += "<p><strong>Clock IP Address:</strong> <code style='background:#e8f5ff;padding:2px 4px;font-family:monospace;'>" + String(OTA_IP) + "</code></p>";
    html += "<p>Use this command to update wirelessly:</p>";
    html += "<code style='background:#fff;border:1px solid #ccc;padding:6px;border-radius:3px;display:block;font-size:0.85em;overflow-x:auto;'>./scripts/ota_flash_monitor.sh --ip " + String(OTA_IP) + "</code>";
    html += "</div>";
#endif

    html += "<p style='color:#888;'>Rebooting in <strong><span id='sec'>3</span></strong> seconds...</p>";
    html += "<script>var s=3;setInterval(function(){if(s>0){s--;document.getElementById('sec').textContent=s;}},1000);</script>";
    html += "</body></html>";
    server.sendHeader("Connection", "close");
    server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
    server.send(200, "text/html", html);
    server.handleClient(); // ensure response bytes are flushed to the browser

    delay(500); // give browser time to render the page before we vanish
    ESP.restart(); // reboot tears down AP; prov_done=true prevents it reopening
  });

  // Routes 5-10: Intercept OS-specific captive portal detection
  // When devices connect to new WiFi, they automatically probe for internet connectivity
  // We intercept these probe requests and redirect to our setup page to auto-open the portal
  // This makes WiFi setup seamless - the login form opens automatically without user clicking!
  // Different operating systems use different detection methods:

  server.on("/generate_204", HTTP_GET, [&server]() {
    // Android captive portal detection endpoint
    // Android devices expect 204 No Content, but redirect instead to show our setup page
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  server.on("/hotspot-detect.html", HTTP_GET, [&server]() {
    // Apple iOS/macOS captive portal detection endpoint
    // iPhone, iPad, and Mac probe this URL to detect login portals
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  server.on("/connectivity-check.html", HTTP_GET, [&server]() {
    // Windows 10/11 captive portal detection (version 1)
    // Windows devices probe this URL for network connectivity
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  server.on("/ncsi.txt", HTTP_GET, [&server]() {
    // Windows NCSI (Network Connectivity Status Indicator) version 1
    // Older Windows versions use this endpoint
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  server.on("/fwlink/", HTTP_GET, [&server]() {
    // Windows NCSI version 2
    // Newer Windows versions use this endpoint for connectivity checks
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  // Route 10: Catch-all for any other request (fallback captive portal redirect)
  // Any HTTP request to any URL on the AP gets redirected to the setup page
  // This ensures maximum compatibility with different devices and browsers
  server.onNotFound([&server]() {
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  // Start the web server (listen for incoming HTTP requests)
  server.begin();

  // Main provisioning loop - continues indefinitely until configuration is saved and device reboots
  server.begin();

  unsigned long lastHeartbeat = 0;
  unsigned long lastBlinkToggle = 0;
  bool blinkVisible = false;
  for (;;)
  {
    server.handleClient();
    dnsServer.handleRequest();

    if (millis() - lastBlinkToggle >= 500)
    {
      lastBlinkToggle = millis();
      blinkVisible = !blinkVisible;
      renderProvisioningIndicator(provisioningDisplay, blinkVisible);
    }

    // Print a heartbeat every 5s so serial monitor confirms the portal is alive
    if (millis() - lastHeartbeat >= 5000)
    {
      lastHeartbeat = millis();
      Serial.printf("[PROV] Portal running — connect to '%s' WiFi and open http://192.168.4.1/save\n",
                    PROVISION_AP_NAME);
    }
    delay(5);
  }
}
