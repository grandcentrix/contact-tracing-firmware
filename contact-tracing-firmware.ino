/*
grandcentrix GmbH Corona Contact Tracing

Please be aware that this code needs some changes in order to be useful for you.
Search for the following strings that you all need to change:
* YOUR_UPDATE_HOST
* YOUR_UPDATE_PATH
* YOUR_FLOW_APP_HOST (2x)
* YOUR_FLOW_APP_PATH
*/

const short VERSION = 1;

#define USE_TLS
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12

#include <Arduino.h>
#include <ETH.h>
#include <WiFiUdp.h>
#include <HTTPUpdate.h>

#include <HardwareSerial.h>
HardwareSerial ESPSerial2(2);

#ifdef USE_TLS
#include <WiFiClientSecure.h>

// https://letsencrypt.org/certs/trustid-x3-root.pem.txt
const char *root_ca_le =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n"
    "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
    "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n"
    "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n"
    "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"
    "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n"
    "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n"
    "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n"
    "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n"
    "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n"
    "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n"
    "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n"
    "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n"
    "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n"
    "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n"
    "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n"
    "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n"
    "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n"
    "-----END CERTIFICATE-----\n";

// Azure: Baltimore CyberTrust Root
const char *root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
    "-----END CERTIFICATE-----\n";
#endif

#include <ArduinoHttpClient.h>
#include <HTTPUpdate.h>
#include <time.h>
void handleSketchDownload();
#include <analogWrite.h>

TaskHandle_t LEDTask;

static bool eth_connected = false;
long now = millis();
long lastTrigger = 0;
long lastFallingEdge = 0;

long lastPing = 0;
long report = 0;
const int led = 4;
bool ledstate = false;

const int led_red = 13;
const int led_green = 14;
const int led_blue = 15;

bool led_pause = false;

String cardid;

// Color arrays
int black[3] = {0, 0, 0};
int white[3] = {100, 100, 100};
int red[3] = {100, 0, 0};
int green[3] = {0, 100, 0};
int blue[3] = {0, 0, 100};
int yellow[3] = {40, 95, 0};
int dimWhite[3] = {30, 30, 30};
int cyberpunk0[3] = {0, 100, 62};
int cyberpunk1[3] = {0, 72, 100};
int cyberpunk2[3] = {0, 12, 100};
int cyberpunk3[3] = {75, 0, 100};
int cyberpunk4[3] = {84, 100, 100};
int cyberpunk5[3] = {100, 7, 31};
int wait = 2; // internal crossFade delay; increase for slower fades

void set_led_off()
{
  analogWrite(led_red, 0);
  analogWrite(led_green, 0);
  analogWrite(led_blue, 0);
}

void set_led_white()
{
  analogWrite(led_red, 255);
  analogWrite(led_green, 255);
  analogWrite(led_blue, 255);
}

void set_led_red()
{
  analogWrite(led_red, 255);
  analogWrite(led_green, 0);
  analogWrite(led_blue, 0);
}

void set_led_yellow()
{
  analogWrite(led_red, 100);
  analogWrite(led_green, 20);
  analogWrite(led_blue, 0);
}

void set_led_green()
{
  analogWrite(led_red, 0);
  analogWrite(led_green, 255);
  analogWrite(led_blue, 0);
}

void set_led_blue()
{
  analogWrite(led_red, 0);
  analogWrite(led_green, 0);
  analogWrite(led_blue, 255);
}

// Set time via NTP, as required for x.509 validation
void setClock()
{
  configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // UTC

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    yield();
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }

  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_ETH_START:
    Serial.println("ETH Started");
    ETH.setHostname("corona-tracer");
    break;
  case SYSTEM_EVENT_ETH_CONNECTED:
    Serial.println("ETH Connected");
    break;
  case SYSTEM_EVENT_ETH_GOT_IP:
    Serial.print("ETH MAC: ");
    Serial.print(ETH.macAddress());
    Serial.print(", IPv4: ");
    Serial.print(ETH.localIP());
    if (ETH.fullDuplex())
    {
      Serial.print(", FULL_DUPLEX");
    }
    Serial.print(", ");
    Serial.print(ETH.linkSpeed());
    Serial.println("Mbps");
    eth_connected = true;
    setClock();
    handleSketchDownload();
    start_led_task();
    break;
  case SYSTEM_EVENT_ETH_DISCONNECTED:
    Serial.println("ETH Disconnected");
    eth_connected = false;
    stop_led_task();
    set_led_red();
    break;
  case SYSTEM_EVENT_ETH_STOP:
    Serial.println("ETH Stopped");
    eth_connected = false;
    stop_led_task();
    set_led_red();
    break;
  default:
    break;
  }
}

void handleSketchDownload()
{
  set_led_blue();
  httpUpdate.setLedPin(led_blue, HIGH);
#ifdef USE_TLS
  WiFiClientSecure client;
  client.setCACert(root_ca_le);
  client.setTimeout(12000 / 1000);
  const int port = 443;
#else
  WiFiClient client;
  const int port = 80;
#endif

  const char *PATH = "/YOUR_UPDATE_PATH/update-v%d.bin?mac=%s";
  char buff[64];
  snprintf(buff, sizeof(buff), PATH, VERSION + 1, ETH.macAddress().c_str());
  t_httpUpdate_return ret = httpUpdate.update(client, "YOUR_UPDATE_HOST", port, buff);

  switch (ret)
  {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  }
}

void LEDTaskCode(void *pvParameters)
{
  for (;;)
  {
    crossFade(cyberpunk1);
    crossFade(cyberpunk5);
    crossFade(cyberpunk2);
    crossFade(cyberpunk3);
    crossFade(cyberpunk5);
    crossFade(cyberpunk2);
    crossFade(cyberpunk1);
    crossFade(cyberpunk5);
    crossFade(cyberpunk3);
    crossFade(cyberpunk2);
    crossFade(cyberpunk5);
  }
}

void start_led_task()
{
  xTaskCreatePinnedToCore(
      LEDTaskCode, /* Task function. */
      "Task1",     /* name of task. */
      10000,       /* Stack size of task */
      NULL,        /* parameter of the task */
      1,           /* priority of the task */
      &LEDTask,    /* Task handle to keep track of created task */
      0);          /* pin task to core 0 */
}

void stop_led_task()
{
  vTaskDelete(LEDTask);
}

void setup()
{
  Serial.begin(115200);
  // ID-12 card reader connected to IO16(RXD)
  ESPSerial2.begin(9600, SERIAL_8N1, 16, 17);

  Serial.print("Sketch version ");
  Serial.println(VERSION);

  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
  set_led_red();

  WiFi.onEvent(WiFiEvent);
  ETH.begin();
}

void getID()
{
  byte bytesread = 0;
  byte i = 0;
  byte val = 0;
  byte tempbyte = 0;

  cardid = "";

  while (bytesread < 12)
  {
    if (ESPSerial2.available() > 0)
    {
      val = ESPSerial2.read();

      if ((val == 0x0D) || (val == 0x0A) || (val == 0x03) ||
          (val == 0x02))
      {
        break;
      }

      cardid += (char)val;
      bytesread++;
    }
  }
  bytesread = 0;
}

bool push()
{
#ifdef USE_TLS
  WiFiClientSecure client;
  client.setCACert(root_ca);
  client.setTimeout(6000 / 1000);
  const int port = 443;
#else
  WiFiClient client;
  const int port = 80;
#endif

  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, 80, "%Y-%m-%dT%H:%M:%SZ", timeinfo);

  if (!client.connect("YOUR_FLOW_APP_HOST.logic.azure.com", port))
  {
    Serial.println("connection failed");
    return false;
  }
  client.print("POST /workflows/YOUR_FLOW_APP_PATH HTTP/1.1\r\n");
  client.print("Host: YOUR_FLOW_APP_HOST.logic.azure.com\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print("Content-Length: 95\r\n");
  client.print("Connection: close\r\n\r\n");
  client.print("{\"Location\":\"");
  client.print(ETH.macAddress());
  client.print("\",\"Timestamp\":\"");
  client.print(buffer);
  client.print("\",\"Identifier\":\"");
  client.print(cardid);
  client.print("\"}\r\n");

  while (client.connected() && !client.available())
    ;

  bool success = false;
  String line;

  while (client.available())
  {
    line = client.readStringUntil('\n');
    if (line.startsWith("HTTP/1."))
    {
      if (line.substring(9, 12).toInt() == 202)
      {
        success = true;
      }
    }
  }

  Serial.println("closing connection\n");
  client.stop();
  return success;
}

void loop()
{
  char val;
  if (ESPSerial2.available() > 0)
  {
    if ((val = ESPSerial2.read()) == 2)
    {
      getID();
      Serial.println(cardid);
      led_pause = true;
      set_led_yellow();
      if (push())
      {
        for (int i = 0; i < 4; i++)
        {
          set_led_green();
          delay(50);
          set_led_off();
          delay(50);
        }
      }
      else
      {
        for (int i = 0; i < 10; i++)
        {
          set_led_red();
          delay(200);
          set_led_off();
          delay(200);
        }
      }
      led_pause = false;
    }
  }
}

// LED fading code
int redVal = black[0];
int grnVal = black[1];
int bluVal = black[2];

int hold = 0;       // Optional hold when a color is complete, before the next crossFade
int DEBUG = 0;      // DEBUG counter; if set to 1, will write values back via serial
int loopCount = 60; // How often should DEBUG report?
int repeat = 0;     // How many times should we loop before stopping? (0 for no stop)
int j = 0;          // Loop counter for repeat

int prevR = redVal;
int prevG = grnVal;
int prevB = bluVal;

int calculateStep(int prevValue, int endValue)
{
  int step = endValue - prevValue; // What's the overall gap?
  if (step)
  {                     // If its non-zero,
    step = 1020 / step; //   divide by 1020
  }
  return step;
}

int calculateVal(int step, int val, int i)
{

  if ((step) && i % step == 0)
  {
    if (step > 0)
    {
      val += 1;
    }
    else if (step < 0)
    {
      val -= 1;
    }
  }

  if (val > 255)
  {
    val = 255;
  }
  else if (val < 0)
  {
    val = 0;
  }
  return val;
}

void crossFade(int color[3])
{
  // Convert to 0-255
  const int factor = 50; // set this to 255 for full brightness
  int R = (color[0] * factor) / 100;
  int G = (color[1] * factor) / 100;
  int B = (color[2] * factor) / 100;

  int stepR = calculateStep(prevR, R);
  int stepG = calculateStep(prevG, G);
  int stepB = calculateStep(prevB, B);

  for (int i = 0; i <= 1020; i++)
  {
    while (led_pause)
    {
      delay(1);
    }

    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);

    analogWrite(led_red, redVal);
    analogWrite(led_green, grnVal);
    analogWrite(led_blue, bluVal);

    delay(wait);

    if (DEBUG)
    { // If we want serial output, print it at the
      if (i == 0 or i % loopCount == 0)
      { // beginning, and every loopCount times
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(redVal);
        Serial.print(" / ");
        Serial.print(grnVal);
        Serial.print(" / ");
        Serial.println(bluVal);
      }
      DEBUG += 1;
    }
  }

  prevR = redVal;
  prevG = grnVal;
  prevB = bluVal;
  delay(hold);
}
