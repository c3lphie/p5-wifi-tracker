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

static inline void AddTrackedDevice(const char *const mac, const char *const targetmac, const char*const sigstrength, const char*const gateway)
{
  char buffer[150] = {0};

  sprintf(buffer, "GET /api/managetrackedinfo.php?mac=%s&mactarget=%s&signalstrength=%s HTTP/1.1\r\nHOST: %s\r\n\r\n", mac, targetmac, sigstrength, gateway);

  esp8266.tcpSend(0, buffer, strlen(buffer));
}
char mac[18] = {0};
char ipa[16] = {0};
char gatewaya[16] = {0};

void setup()
{
  soft.begin(9600);
  soft.println("Begin");

  while (esp8266.begin(115200, ESP8266_HARDWARE_SERIAL) != true)
  {
    soft.println("Error");
    delay(1000);
  }

  delay(1000);

  esp8266.setMode(ESP8266_MODE_STA);

  delay(1000);

  while (esp8266.connect("CentralHub", "mynamejeff123") < 0)
  {
    soft.println("Connecting");
    delay(1000);
  }


  delay(1000);

  soft.println("Connected");

  IPAddress ip = esp8266.localIP();
  IPAddress gateway = ip;

  gateway[3] = 1;

  delay(1000);



  esp8266.localMAC(mac);

  soft.println(mac);

  delay(1000);

  if (esp8266.ping(gateway) > 0)
  {
    soft.println("We online bois");
  }

  delay(1000);


  sprintf(ipa, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  sprintf(gatewaya, "%d.%d.%d.%d", gateway[0], gateway[1], gateway[2], gateway[3]);

  //AddTrackedDevice(mac, "AA:BB:CC:DD:EE:FF", "10", gatewaya);

  soft.println(ip);
  soft.println(gateway);
}

static inline char Get()
{
  int c = soft.read();
  while (c == -1)
  {
    c = soft.read();
  }

  return (char)c;
}

void loop()
{
  soft.setTimeout(2147483647);
  soft.readStringUntil('\n');
  
  String p = soft.readStringUntil('\n');
  char buffer[50] = {0};
  strcpy(buffer, p.c_str());
  buffer[17] = 0;

  for (uint8_t i = 0; i < 6; i++)
  {
    if (buffer[17 + i] == '\r' || buffer[17 + i] == '\n') buffer[17 + i] = 0;
  }

  if (esp8266.tcpConnect(0, gatewaya, 80, 0) == 1)
  {
  }

  delay(500);

  AddTrackedDevice(mac, buffer, buffer + 18, gatewaya);

  delay(500);

  esp8266.close(0);

  delay(500);

  if (esp8266.tcpConnect(0, gatewaya, 80, 0) == 1)
  {
  }

  delay(500);

  AddDevice(mac, ipa, gatewaya);AT

  delay(500);
  esp8266.close(0);

  delay(500);
}
