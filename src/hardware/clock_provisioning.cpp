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
#include <ArduinoJson.h>
#include <clock_provisioning.h>

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
#define TIME_API_BASE "http://worldtimeapi.org/api/timezone"
#endif

// Force provisioning portal on every boot (1=yes, 0=only if config invalid)
#ifndef FORCE_PROVISIONING
#define FORCE_PROVISIONING 0
#endif

// NVS (Non-Volatile Storage) namespace for saving persistent clock config
static const char *PREF_NAMESPACE = "clockcfg";

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
  prefs.end(); // Close and commit to flash
}

// Load WiFi credentials and city from non-volatile storage (NVS/flash)
// Falls back to compile-time defaults if nothing saved
void loadClockConfig(ClockConfig &config)
{
  Preferences prefs;
  prefs.begin(PREF_NAMESPACE, true); // true = read-only mode
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
//   buildTimeApiUrlForCity("Europe/Berlin") -> "http://worldtimeapi.org/api/timezone/Europe/Berlin.json"
//   buildTimeApiUrlForCity("America/New_York") -> "http://worldtimeapi.org/api/timezone/America/New_York.json"
//   buildTimeApiUrlForCity("Asia/Tokyo") -> "http://worldtimeapi.org/api/timezone/Asia/Tokyo.json"
//   buildTimeApiUrlForCity("Australia/Sydney") -> "http://worldtimeapi.org/api/timezone/Australia/Sydney.json"
// List of all supported timezones: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
String buildTimeApiUrlForCity(const String &city)
{
  String safeCity = sanitizeCity(city); // Replace spaces with underscores
  String url = TIME_API_BASE;
  if (!url.endsWith("/"))
  {
    url += "/"; // Ensure base URL ends with /
  }
  url += safeCity;
  url += ".json";
  return url;
}

// Check if provisioning portal should always be shown (for testing/debugging)
bool shouldForceProvisioning()
{
  return FORCE_PROVISIONING == 1; // 1 = yes, 0 = only show if config invalid
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

// Main provisioning portal function
// Creates WiFi hotspot with web server for configuration
// Shows setup form, validates inputs, tests WiFi/API connection, then reboots
// This function blocks until configuration is complete
void runProvisioningPortalUntilConfigured()
{
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
  // Route 2: Setup form (/save GET) - Display the HTML configuration form
  // Users see this form when they first connect to the NixiClock WiFi hotspot
  server.on("/save", HTTP_GET, [&server]() {
    // Build HTML form for user input with helpful instructions and examples
    String html;
    html += "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>NixiClock Setup</title></head><body style='font-family:sans-serif;max-width:480px;margin:24px auto;padding:0 12px;'>";
    html += "<h2>🌍 NixiClock WiFi Setup</h2>";
    html += "<p>Configure your clock to connect to your WiFi network and set your timezone.</p>";
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
    html += "<strong>📍 Timezone Examples:</strong><br>";
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
      server.send(400, "text/plain", "Error: Please fill in all fields:\n- WiFi SSID (your network name)\n- WiFi Password\n- Timezone (format: Europe/Berlin)");
      return;  // Stop processing and ask user to fill in all fields
    }

    // Test WiFi connection with provided credentials
    Serial.println("Testing WiFi connection...");
    WiFi.mode(WIFI_AP_STA); // Enable both AP and STA (station) modes
    WiFi.begin(cfg.ssid.c_str(), cfg.password.c_str()); // Connect to user's router

    // Status messages to display on confirmation page
    String wifiStatus = "❌ Failed to connect to WiFi";
    String timeStatus = "⏳ Skipped (WiFi not available)";
    String currentTime = "";
    bool wifiOk = false; // WiFi connection success flag
    bool timeOk = false; // API call success flag

    // Wait up to 10 seconds for WiFi to connect
    uint32_t wifiStart = millis();
    while (millis() - wifiStart < 10000)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        wifiOk = true;
        wifiStatus = "✅ Connected to WiFi: " + cfg.ssid;
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
      String apiUrl = buildTimeApiUrlForCity(cfg.city); // Build URL with city name

      http.begin(apiUrl);
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
          if (datetime)
          {
            // API returned valid datetime
            currentTime = String(datetime);
            timeOk = true;
            timeStatus = "✅ Time API working for " + cfg.city;
            Serial.print("Current time from API: ");
            Serial.println(currentTime);
          }
          else
          {
            timeStatus = "❌ API response missing datetime field";
          }
        }
        else
        {
          timeStatus = "❌ Failed to parse API response";
        }
      }
      else
      {
        // API error (wrong city, no internet, etc.)
        timeStatus = "❌ API error (HTTP " + String(httpCode) + ")";
      }

      http.end(); // Close HTTP connection
    }

    // Build HTML confirmation page showing test results
    String html;
    html += "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<meta http-equiv='refresh' content='5;url=/'>";
    html += "<title>Setup Result</title>";
    html += "<style>"; // CSS styling for result page
    html += "body { font-family:sans-serif; max-width:480px; margin:24px auto; padding:0 12px; }";
    html += ".ok { color:#155724; background:#d4edda; padding:12px; border-radius:4px; margin:10px 0; }"; // Green for success
    html += ".fail { color:#721c24; background:#f8d7da; padding:12px; border-radius:4px; margin:10px 0; }"; // Red for error
    html += ".skip { color:#856404; background:#fff3cd; padding:12px; border-radius:4px; margin:10px 0; }"; // Yellow for skipped
    html += ".time-display { background:#f0f0f0; padding:12px; border-radius:4px; font-family:monospace; margin:10px 0; }"; // Monospace for time display
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
    if (!currentTime.isEmpty())
    {
      html += "<p><strong>Current Time from API:</strong></p>";
      html += "<div class='time-display'>" + currentTime + "</div>";
    }

    // If all tests passed: save config and reboot
    if (wifiOk && timeOk)
    {
      // Success! All tests passed - configuration is valid
      html += "<p style='color:green;font-size:18px;'><strong>✅ All checks passed!</strong></p>";
      html += "<p>Your WiFi and timezone settings are valid. Saving to flash memory and rebooting...</p>";
      server.send(200, "text/html", html);
      delay(1000); // Give user time to read success message before reboot

      // Save WiFi SSID, password, and timezone to non-volatile flash storage (NVS)
      // These settings will persist across reboots, power cycles, and factory resets
      saveClockConfig(cfg);
      delay(500);
      // Reboot to apply new settings and start normal clock operation
      ESP.restart();
    }
    else
    {
      // At least one test failed - configuration NOT saved
      // User must correct their input and try again
      html += "<p style='color:red;font-size:18px;'><strong>❌ Setup Failed</strong></p>";
      html += "<p>Please check your entries:</p>";
      html += "<ul>";
      if (!wifiOk)
      {
        html += "<li>❌ <strong>WiFi Connection Failed:</strong> Check SSID and password are correct</li>";
      }
      if (!timeOk)
      {
        html += "<li>❌ <strong>Time API Failed:</strong> Use format like 'Europe/Berlin' not just 'Berlin'</li>";
      }
      html += "</ul>";
      html += "<p><a href='/save' style='color:#4CAF50;text-decoration:none;font-weight:bold;'>← Go Back & Try Again</a></p>";
      html += "<p><small>Page will refresh in 5 seconds...</small></p>";
      server.send(200, "text/html", html);
      WiFi.mode(WIFI_AP); // Return to AP-only mode (stop attempting STA connection)
    }
  });

  // Routes 4-9: Intercept OS-specific captive portal detection
  // When devices connect to new WiFi, they probe for internet by making specific requests
  // We intercept these probes and redirect to our setup page to auto-open the portal

  server.on("/generate_204", HTTP_GET, [&server]() {
    // Android captive portal detection - expects 204 No Content, but redirect to /save instead
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  server.on("/hotspot-detect.html", HTTP_GET, [&server]() {
    // Apple iOS/macOS captive portal detection
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  server.on("/connectivity-check.html", HTTP_GET, [&server]() {
    // Windows 10/11 captive portal detection (version 1)
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  server.on("/ncsi.txt", HTTP_GET, [&server]() {
    // Windows NCSI (Network Connectivity Status Indicator)
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  server.on("/fwlink/", HTTP_GET, [&server]() {
    // Windows NCSI version 2
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  // Route 10: Catch-all for any other request (fallback captive portal redirect)
  // Ensures any HTTP request gets redirected to setup page
  server.onNotFound([&server]() {
    server.sendHeader("Location", "http://192.168.4.1/save");
    server.send(302, "text/plain", "");
  });

  // Start the web server (listen for HTTP requests)
  server.begin();

  // Main provisioning loop - continues until configuration is saved and device reboots
  // This is an infinite loop that never returns
  for (;;)
  {
    // Process incoming HTTP requests (form GET, POST, captive portal detection, etc.)
    server.handleClient();
    // Process incoming DNS requests (DNS spoofing for captive portal)
    dnsServer.handleRequest();
    delay(5); // Small delay to allow other tasks to run
  }
}
