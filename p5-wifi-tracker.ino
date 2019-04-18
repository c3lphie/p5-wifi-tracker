// Custom SparkFunESP8266WiFi library needed
// because at firmware is updated
// Use this: https://github.com/mads256c/SparkFun_ESP8266_AT_Arduino_Library

#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>


SoftwareSerial soft{2, 3};

static inline void AddDevice(const char *const mac, const char *const ip, const char *const gateway)
{
  char buffer[150] = {0};

  sprintf(buffer, "GET /api/adddevice.php?name=helloworld&type=0&mac=%s&ip=%s HTTP/1.1\r\nHost: %s\r\n\r\n", mac, ip, gateway);

  esp8266.tcpSend(0, buffer, strlen(buffer));
}

static inline void AddTrackedDevice(const char *const mac, const char *const targetmac, const char *const sigstrength, const char *const gateway)
{
  char buffer[150] = {0};

  sprintf(buffer, "GET /api/managetrackedinfo.php?mac=%s&mactarget=%s&signalstrength=%s HTTP/1.1\r\nHOST: %s\r\n\r\n", mac, targetmac, sigstrength, gateway);

  esp8266.tcpSend(0, buffer, strlen(buffer));
}
static char mac[18] = {0};
static char ip[16] = {0};
static char gateway[16] = {0};

void setup()
{
  soft.begin(9600);

  // Connect to esp8266 @ 115200 with hardware serial
  while (!esp8266.begin(115200, ESP8266_HARDWARE_SERIAL))
  {
    delay(1000);
  }

  delay(1000);

  // Set wifi adapter to station mode.
  esp8266.setMode(ESP8266_MODE_STA);

  delay(1000);

  // Connect to CentralHub
  while (esp8266.connect("CentralHub", "mynamejeff123") < 0)
  {
    delay(1000);
  }


  delay(1000);

  // Get ip and gateway
  IPAddress localIp = esp8266.localIP();
  IPAddress gatewayIp = localIp;
  gatewayIp[3] = 1;

  delay(1000);

  // Get mac
  esp8266.localMAC(mac);

  delay(1000);

  // Convert ips to strings
  sprintf(ip, "%d.%d.%d.%d", localIp[0], localIp[1], localIp[2], localIp[3]);
  sprintf(gateway, "%d.%d.%d.%d", gatewayIp[0], gatewayIp[1], gatewayIp[2], gatewayIp[3]);
}

// Read a char from the software serial buffer.
static inline char Get()
{
  int c = soft.read();
  while (c == -1)
  {
    c = soft.read();
    yield();
  }

  return (char)c;
}

void loop()
{
  // Connect to the webserver.
  esp8266.tcpConnect(0, gateway, 80, 0);

  delay(500);

  // Add or update this device in the database.
  AddDevice(mac, ip, gateway);

  delay(500);

  // Close the connection.
  esp8266.close(0);

  delay(500);

  // Don't timeout
  soft.setTimeout(2147483647);
  // Ignore the first as it could be corrupted
  soft.readStringUntil('\n');

  // Read mac and signal strength
  String p = soft.readStringUntil('\n');
  char buffer[50] = {0};
  strcpy(buffer, p.c_str());
  buffer[17] = 0;

  // Remove newline characters
  for (uint8_t i = 0; i < 6; i++)
  {
    if (buffer[17 + i] == '\r' || buffer[17 + i] == '\n') buffer[17 + i] = 0;
  }

  // Connect to the webserver.
  esp8266.tcpConnect(0, gateway, 80, 0);

  delay(500);

  
  AddTrackedDevice(mac, buffer, buffer + 18, gateway);

  delay(500);

  // Close the connection.
  esp8266.close(0);

  
  delay(500);
}
