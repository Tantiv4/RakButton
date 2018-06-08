
#include "WiFi.h"
#include <HttpClient.h>
#include "wifi_drv.h"
#include <WiFiClient.h>
#include <GTimer.h>
#include <FlashMemory.h>

#define LED1  0
#define LED2  1
#define LED3  2
#define LED4  3
#define RED   0
#define GREEN 1
#define BLUE  2
#define OFF   3
#define RE_WIFI_CONFIG_ENABLE 0
void led_off();
void led_ctrl(uint8_t led_num, uint8_t rgb);
void trigger_action();
char t4_server[] = "api-dev.tantiv4.com";    /// Name of the server we want to connect to
char path[256];
String url = "https://api-dev.tantiv4.com/api/v1/rak/ifttt/trigger";
/*------------------------------------------------------------------------------------------------------------------*/
unsigned char test_ca_cert[] = \
                               "-----BEGIN CERTIFICATE-----\n" \
                               "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
                               "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
                               "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
                               "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
                               "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
                               "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
                               "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
                               "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
                               "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
                               "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
                               "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
                               "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
                               "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
                               "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
                               "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
                               "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
                               "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
                               "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
                               "-----END CERTIFICATE-----\n"; //For the usage of verifying client

unsigned char test_client_cert[] = \
                                   "-----BEGIN CERTIFICATE-----\n" \
                                   "MIID6DCCAtCgAwIBAgIJAMZHIhoPSFHuMA0GCSqGSIb3DQEBCwUAMIGIMQswCQYD\n" \
                                   "VQQGEwJJTjELMAkGA1UECAwCQVAxEjAQBgNVBAcMCUh5ZGVyYWJhZDEQMA4GA1UE\n" \
                                   "CgwHVGFudGl2NDERMA8GA1UECwwIRmlybXdhcmUxDjAMBgNVBAMMBUFzaG9rMSMw\n" \
                                   "IQYJKoZIhvcNAQkBFhRjLmFzaG9rNDE0QGdtYWlsLmNvbTAeFw0xODA0MTExMDUw\n" \
                                   "MDNaFw0xOTA0MTExMDUwMDNaMIGIMQswCQYDVQQGEwJJTjELMAkGA1UECAwCQVAx\n" \
                                   "EjAQBgNVBAcMCUh5ZGVyYWJhZDEQMA4GA1UECgwHVGFudGl2NDERMA8GA1UECwwI\n" \
                                   "RmlybXdhcmUxDjAMBgNVBAMMBUFzaG9rMSMwIQYJKoZIhvcNAQkBFhRjLmFzaG9r\n" \
                                   "NDE0QGdtYWlsLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKsl\n" \
                                   "JeZO1zQ72WgM+1rxl6vFjq78mtaA8Dj/bI1XfuUUq4IGyC8Ronwb+6PPbK+qDzz8\n" \
                                   "kyKaVSwWuCeShh/3psaBZE4m2g+VEstCgoZMZ0Ae13G40JJBGGgWTXdYA1tIJJs3\n" \
                                   "kJPcyWYEDdHGnPI8tilfAB0SbwpuufgcCF8bQOPLl427JvsKoLcwmr5W75/JYdPN\n" \
                                   "VrkIuDz+olzQs9svCJMMMfpsqn61R40IgVitm6nOEdlMaOsefCb4+YG0EBbOoiad\n" \
                                   "GkNBU+nqTaiV8O8Aa/YeRwueUawl6M+DVKvd9V3tIgcSN+jmJdmOW/9kFQ3qmr5k\n" \
                                   "N567fOH7V6f0HQb0BUMCAwEAAaNTMFEwHQYDVR0OBBYEFK3Wx8ROOUBhmrSvr8qa\n" \
                                   "7SADem00MB8GA1UdIwQYMBaAFK3Wx8ROOUBhmrSvr8qa7SADem00MA8GA1UdEwEB\n" \
                                   "/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAA7pES2cK8Mp2cYVlso4dLE5it+h\n" \
                                   "q4L/6Hj0E5PzzuUgtUnxd3aVyfz1y+0SnDXGQcq4y0e8q2focUntnKW45skEULR8\n" \
                                   "KjoIm+aOCZRnKvnOnoCkFkfCOHzsuGK14HpRvUUQsamaA7RyuOTr54Ozo5g6TFoF\n" \
                                   "HLhL1at+Iz0pK+MsmXD0FKnWLSQ/LhALdqDsiZtSFMWjQB+AtI/PeoSFTNEBrgeC\n" \
                                   "kziC9EGu5HwMZG7cDnIcgTk8YmHR95YBBqZDCQbgl32gwfmYnei1NpAkr9QYBT2k\n" \
                                   "FzI8OZrD7vP0gxTW10h+aHkkHpRovUad5+wh5MmMsmyhnJDUclrBNDcSHrs=\n" \
                                   "-----END CERTIFICATE-----\n";  //For the usage of verifying client
unsigned char test_client_key[] = \
                                  "-----BEGIN PRIVATE KEY-----\n" \
                                  "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCrJSXmTtc0O9lo\n" \
                                  "DPta8ZerxY6u/JrWgPA4/2yNV37lFKuCBsgvEaJ8G/ujz2yvqg88/JMimlUsFrgn\n" \
                                  "koYf96bGgWROJtoPlRLLQoKGTGdAHtdxuNCSQRhoFk13WANbSCSbN5CT3MlmBA3R\n" \
                                  "xpzyPLYpXwAdEm8Kbrn4HAhfG0Djy5eNuyb7CqC3MJq+Vu+fyWHTzVa5CLg8/qJc\n" \
                                  "0LPbLwiTDDH6bKp+tUeNCIFYrZupzhHZTGjrHnwm+PmBtBAWzqImnRpDQVPp6k2o\n" \
                                  "lfDvAGv2HkcLnlGsJejPg1Sr3fVd7SIHEjfo5iXZjlv/ZBUN6pq+ZDeeu3zh+1en\n" \
                                  "9B0G9AVDAgMBAAECggEAIuovMy7+ZF+ZWMrdNI54/MgO9c/+3qfrWhVx1dv1vVJc\n" \
                                  "MuijSkb0uREUYzTb3XJ6LVzxv2tw2M/BvgD8b+UcFv8qsW7nqaol7WWiLzfDEKe6\n" \
                                  "GC82DcBN9F4oa6J+/M5Y+Xl6e0/dnnxRPWyhdCHCn+oLoeKy2JQCSIWfh5eNw2re\n" \
                                  "C6ZPsf+EMi7qdI3MFZGJ6suNa/OXgLLiEuo9Z+AqyPOFEONXcWmBAYqyrAokWcZH\n" \
                                  "SXrXE1+JIsa7ZUIsfj+AGM9FHwneN2CAM3s44sLnhOFG9kszxgI3b0LTC5bul+OJ\n" \
                                  "/Ftef8B6XLgV4JKCIKEx9iW3992+D6slsuNMsNhJ0QKBgQDYCHowsdCRdiEo3bBy\n" \
                                  "Vf7z9FOZDEG9uaqcw07nhvJWw/OgHpegcc//BB82ekdGaq32r/nWHV/xGIcBj4/H\n" \
                                  "alu13XRDZnu8PCaMcvqLcVxI0OXJ47w6X+UkBpOAknWuJczDQJrrifZ8DQxdWZRn\n" \
                                  "loeWEQaA6kJEKj67A5rx1EocvwKBgQDKzrwD84nS3oG+BH1FqIAvhdZ8i45tgaLH\n" \
                                  "fVDtkQ6m1dR7Qc2aj2LEXBy312BQMXr/U2fuZodGMlRwpqIClhW+0vS4t/OjERZd\n" \
                                  "TISuQEJALUkcn4M72JptXj4sb+pcnDnqSOj6lxiFgsgtbmQ1lQozmc25M7t5BpnG\n" \
                                  "nL8xoO8EfQKBgAUm9psQspO1iIbr+phPerwguJmu1CaNNgd1FP2qlz8Af1/zOtSF\n" \
                                  "wlsnowLumaKRSsLWYZP8sVwTpKyh2uD7pWJJ/Mrat9q+aIDzAQywUG4gJUrSVpvh\n" \
                                  "y5o3We84DvxvRFnnwYoaiTleKkxTpZlu2p72mvwaGih8BXD6XgikcnwLAoGBALkB\n" \
                                  "LfJMz4VVBPXaEozg0U6IwMiMZjju8RcZ8iFFGiCHH3Xw6SX9dxO5E1mBV5LQEGuq\n" \
                                  "Ij0PuocGkpDQ8Sl60AtrVWLMK6ytgcFKdB6g786rgO5jdMu7I+Rw/DZ9QUWKHiri\n" \
                                  "p+UwwGXKj+EhurhGZS/9NfWkg3g71Tug8ajMLj3hAoGAbIdoyp0NMHG/DXoqhjcb\n" \
                                  "iuv9qO/yZBRsJ+PCGMeH66AnXiY5OW4rTD2CkwT1dhfKUVEI6OPPXmwueRtSqnWe\n" \
                                  "/cowL22BAOyJDWd0WCRlL5AFQoTPFAFf+491AqRYVCiYVyhVMzcfGtBL8MCVsjP9\n" \
                                  "EfDJhlvUh+aHtqoXwuS2mvE=\n" \
                                  "-----END PRIVATE KEY-----\n";
/*------------------------------------------------------------------------------------------------------------------*/
WiFiSSLClient client;
//URL information
const char pth1[] = "/api/v1/rak/ifttt/trigger?device_id=";
const char pth2[] = "&&button_id=";
const char pth3[] = "HTTP/1.0";
const char device_id[] = "Enter Device-ID";// Enter Device-ID for to connect Tantiv4 server.
int btn_no, count;
/*--------------------------------------------------------------------------------------------------------------------*/
typedef enum {
  WIFI_AP_MODE,
  WIFI_STATION_MODE,
  WIFI_MODE_ERROR
}
status_t;
/*--------------------------------------------------------------------------------------------------------------------*/
/* power enable */
int pwr_en = 15;
/* leds */
int led1_r = 25;
int led1_g = 24;
int led1_b = 19;
int led2_r = 0;
int led2_g = 2;
int led2_b = 6;
int led4_r = 12;
int led4_g = 11;
int led4_b = 13;
int led3_r = 22;
int led3_g = 21;
int led3_b = 1;
/* keys */
int key1 = 23;
int key2 = 14;
int key3 = 10;
int key4 = 20;
/*------------------------------------------------------------------------------------------------------------------*/
#if RE_WIFI_CONFIG_ENABLE
void start_AP_mode(char *default_ssid , char *default_password , char *channel);
void start_station_mode(char *connect_ssid, char *connect_password);
void get_ssid_password_from_flash(void);
void reset_wifi_config(void);
void disconnect_wifi(void);
status_t button_press_detection(void);
status_t wifi_config(void);
#else
void start_station_mode(char *connect_ssid, char *connect_password);
status_t button_press_detection(void);
#endif
/*------------------------------------------------------------------------------------------------------------------*/
#if RE_WIFI_CONFIG_ENABLE
char ssid_default[] = "RDB";  //Set the AP's SSID
char password_default[] = "test1234";     //Set the AP's password
char channel_no[] = "1";         //Set the AP's channel
char buffer[128];
char *ssid_get;
char *password_get;
static char ssid_connect[33];
static char password_connect[65];
#else
static char ssid_connect[32] = "yourNetwork"; //  your network SSID (name)
static char password_connect[64] = "password";    // your network password (use for WPA, or use as key for WEP)
#endif
int status = WL_IDLE_STATUS;     // the Wifi radio's status
#if RE_WIFI_CONFIG_ENABLE
volatile int mode = 3;
char i, j, len1, len2;
WiFiServer server(6789);
WiFiClient client_web;
#else
volatile int mode = 1;
#endif
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  // pinMode(13, OUTPUT);
  pinMode(pwr_en, OUTPUT);
  digitalWrite(pwr_en, 1);

  pinMode(led2_r, OUTPUT);
  pinMode(led2_g, OUTPUT);
  pinMode(led2_b, OUTPUT);

  pinMode(led1_b, OUTPUT);
  pinMode(led3_r, OUTPUT);
  pinMode(led3_g, OUTPUT);
  pinMode(led3_b, OUTPUT);
  pinMode(led4_b, OUTPUT);

  pinMode(key2, INPUT_PULLUP);
  pinMode(key3, INPUT_PULLUP);
  pinMode(key4, INPUT_PULLUP);

#if 1
  /*
       Pin D21-D25 can not be used as digital IO ,when in debug mode(Enable JTAG).
       D21-D25 can be used as digital IO when in factory mode.(Disable JTAG)
  */
  //D21-D25
  pinMode(led4_g, OUTPUT);
  pinMode(led4_r, OUTPUT);
  pinMode(led1_g, OUTPUT);
  pinMode(led1_r, OUTPUT);
  pinMode(key1, INPUT_PULLUP);
#endif

  led_off();


  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }
}
/*----------------------------------------------------------------------------------------------*/
status_t wifi_status;
void loop()
{
  led_off();
#if RE_WIFI_CONFIG_ENABLE
  if ((mode == 3) && (status != WL_CONNECTED))
  {
    FlashMemory.read();
    if (FlashMemory.buf[0] == 0xFF)
    {
      start_AP_mode(ssid_default , password_default , channel_no);
    }
    else {
      get_ssid_password_from_flash();
      wifi_status = WIFI_STATION_MODE;
      start_station_mode(ssid_connect, password_connect);
      Serial.println("Connected to wifi");
    }
    printWifiData();
    printCurrentNet();
    server.begin();                           // start the web server on port 6789
    printWifiStatus();
  }

  if (mode == 1)
  {
    button_press_detection();
  }

  if ((status != WL_CONNECTED))
  {
    if (wifi_status == WIFI_AP_MODE)
    {
      start_AP_mode(ssid_default , password_default , channel_no);
    }
    else if (wifi_status == WIFI_STATION_MODE)
    {
      start_station_mode(ssid_connect, password_connect);
    }
    else if (wifi_status == WIFI_MODE_ERROR)
    {
      mode = 0;
    }
    else
    {

    }
    if (status == WL_CONNECTED) {
      printWifiStatus();
    }
  }

  if (mode == 0)
  {
    wifi_config();
  }
#else
  if (status != WL_CONNECTED)
  {
    start_station_mode(ssid_connect, password_connect);
  }
  button_press_detection();
#endif
}
/*-------------------------------------------------------------------------------------*/
status_t button_press_detection(void)
{
#if RE_WIFI_CONFIG_ENABLE
  if (digitalRead(key2) == 0) {
    Serial.println("short press detected");
    count = 10;
    delay(50);
    while (count && (digitalRead(key2) == 0)) {
      Serial.println("holding");
      Serial.println(count);
      --count;
      delay(1000);
    }
    if (count)
    {
      Serial.println("short press");
      led_ctrl(LED2, BLUE);
      delay(500);
      Serial.print("key2");
      btn_no = 2;
      trigger_action();
    }
    else
    {
      disconnect_wifi();
      reset_wifi_config();
      status = WL_DISCONNECTED;
      client.stop();
      client_web.stop();
      Serial.println("long press detected reset the device");
      wifi_status = WIFI_AP_MODE;
      //start_AP_mode(ssid_default , password_default , channel_no);
      //AP MODE already started:
      Serial.println("AP mode already started");
    }
  }
#else
  if (digitalRead(key2) == 0) {
    delay(50);
    if (digitalRead(key2) == 0) {
      led_ctrl(LED2, BLUE);
      delay(500);
      Serial.print("key2");
      btn_no = 2;
      trigger_action();
    }
  }
#endif
  if (digitalRead(key1) == 0) {
    delay(50);
    if (digitalRead(key1) == 0) {
      led_ctrl(LED1, BLUE);
      delay(500);
      Serial.print("key1");
      btn_no = 1;
      trigger_action();
    }
  }
  if (digitalRead(key3) == 0) {
    delay(50);
    if (digitalRead(key3) == 0) {
      led_ctrl(LED3, BLUE);
      delay(500);
      Serial.print("key3");
      btn_no = 3;
      trigger_action();
    }
  }
  if (digitalRead(key4) == 0) {
    delay(50);
    if (digitalRead(key4) == 0) {
      led_ctrl(LED4, BLUE);
      delay(500);
      Serial.print("key4");
      btn_no = 4;
      trigger_action();
    }
  }
  // delay(10);
}
/*-----------------------------------------------------------------------------------*/
#if RE_WIFI_CONFIG_ENABLE
status_t wifi_config(void)
{
  // listen for incoming clients
  client_web = server.available();
  if (client_web) {
    Serial.println("Client connected");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    if (client_web.connected()) {
      while (client_web.available()) {
        char c = client_web.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply

        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response");
          // send a standard http response header
          client_web.println("HTTP/1.1 200 OK");
          client_web.println("Content-Length: 2048 - 167");
          client_web.println("Content-type: text/html");//\r\nExpires: Sun, 31 Dec 2028 23:59:59 GMT\r\nPragma: no-cache\r\n\r\n");
          client_web.println();
          client_web.println("<!DOCTYPE html>\r\n");
          client_web.println("<html>\r\n<head>\r\n");
          client_web.println("<link rel=\"icon\" href=\"data:;base64,=\">\r\n");
          client_web.println("<meta charset=\"UTF-8\">\r\n");
          client_web.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n");
          client_web.println("<title>RDB Wi-fi Setup</title>\r\n</head>\r\n");
          client_web.println("<body style=\"background-color:#0d6faa; color:#fff;    text-align: center;padding: 10px;\">\r\n");
          client_web.println("<br><br><br><br><br><br><br>\r\n");
          client_web.println("<h1>Select the Network</h1>\r\n");
          client_web.println("<br><br>\r\n");
          client_web.println("<form class=\"formVal\" action=\"http://192.168.1.1:6789\" method=\"POST\">\r\n<div>\r\n");
          client_web.println("</select>\r\n<br><br>\r\n");
          client_web.println("</div>\r\n<div>\r\n");
          client_web.println("<label>SSID:</label>\r\n");
          client_web.println("<input type=\"ssid\" name='ssid' id='ssid'>\r\n");
          client_web.println("</select>\r\n<br><br>\r\n");
          client_web.println("</div>\r\n<div>\r\n");
          client_web.println("<label>Password:</label>\r\n");
          client_web.println("<input type=\"password\" name='password' id='password'>\r\n");
          client_web.println("</div><br>\r\n");
          client_web.println("<div>\r\n<button type=\"submit\" value=\"Submit\" onclick=\"myFunction()\">Pair</button>\r\n</div>\r\n</form>\r\n");
          client_web.println("<div>\r\n<footer>\r\n<p>Copyright 2018 Tantiv4 Inc</p>\r\n</footer>\r\n</div>\r\n");
          client_web.println("<script type=\"text/javascript\">\r\nfunction myFunction()\r\n{\r\n");
          client_web.println("var xmlHttp = new XMLHttpRequest();\r\n");
          client_web.println( "xmlHttp.open(\"post\", \"http://192.168.1.1:6789\");\r\n");
          client_web.println("xmlHttp.setRequestHeader('Content-Type', 'application/json');\r\n");
          client_web.println("xmlHttp.setRequestHeader(\"Cache-Control\", \"max-age=0\");\r\n");
          client_web.println("xmlHttp.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\r\n");
          client_web.println("xmlHttp.send(JSON.stringify({\r\nssid:document.getElementById(\"ssid\").value,\r\npassword:document.getElementById(\"password\").value\r\n}))\r\n\r\n}\r\n</script>\r\n");
          client_web.println( "</body>\r\n</html>\r\n");

          client_web.println();
          //    Here is where the POST data is.
          Serial.println("Post data: ");
          int ctr = 0;
          while (client_web.available()) {
            int c = client_web.read();
            Serial.write(c);
            buffer[ctr] = c;
            ctr++;
          }
          Serial.println("Buffer1: ");
          Serial.print(buffer);
          delay(1);
          client_web.stop();
          if (buffer != NULL)
          {
            ssid_get = strstr(buffer, "{\"ssid\":");
            if (ssid_get == NULL)
            {
              Serial.println("ssid_connect is missing\n");
              memset(buffer , 0, sizeof(buffer));
              currentLineIsBlank = true;
              client_web.stop();
              return WIFI_MODE_ERROR;;
            }
            ssid_get += strlen("{\"ssid\":");
            sscanf(ssid_get, "\"%[^\"]s" , ssid_connect);
            password_get = strstr(buffer, "\"password\":");
            if (password_get == NULL)
            {
              Serial.println("password_connect is missing\n");
              memset(buffer , 0, sizeof(buffer));
              currentLineIsBlank = true;
              //client.flush();
              client_web.stop();
              return WIFI_MODE_ERROR;
            }
            password_get += strlen("\"password\":");
            sscanf(password_get, "\"%[^\"]s", password_connect);
            Serial.println("ssid_connect");
            Serial.println(ssid_connect);
            Serial.println("password_connect");
            Serial.println(password_connect);
            reset_wifi_config();
            FlashMemory.buf[0] = strlen(ssid_connect);
            memcpy(FlashMemory.buf + 1, ssid_connect, strlen(ssid_connect));
            FlashMemory.buf[strlen(ssid_connect) + 1] = strlen(password_connect);
            memcpy((FlashMemory.buf + 2 + strlen(ssid_connect)), password_connect, strlen(password_connect));
            FlashMemory.update();
            status = WL_DISCONNECTED;
            wifi_status = WIFI_STATION_MODE;
            server.available().stop();
          }
          memset(buffer , 0, sizeof(buffer));
          currentLineIsBlank = true;
          client_web.stop();
        }
        else if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client_web.stop();
  }
}
#endif
/*-----------------------------------------------------------------------------------*/
void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);

  // print your gateway address:
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(gateway);
  Serial.println();
}
/*-----------------------------------------------------------------------------------*/
void printCurrentNet() {
  // print the SSID of the AP:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of AP:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[0], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.println(bssid[5], HEX);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
/*-----------------------------------------------------------------------------------*/
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  all_leds_blink(mode);
  if (!mode)
  {
    Serial.print("To see this page in action, open a browser to http://");
  }
  Serial.println(ip);
}
/*-----------------------------------------------------------------------------------*/
void led_off() {
  led_ctrl(LED1, OFF);
  led_ctrl(LED2, OFF);
  led_ctrl(LED3, OFF);
  led_ctrl(LED4, OFF);
}
/*-----------------------------------------------------------------------------------*/
void trigger_action()
{
  if (status == WL_CONNECTED)
  {
   if (!client.connected())
    {

    if (client.connect(t4_server, 443, test_ca_cert, test_client_cert, test_client_key)) {
      Serial.println("connected to server");
    }
    else
    {
      led_ctrl(btn_no - 1, RED);
      delay(1000);
      Serial.println("connected to server failed");
      Serial.println();
      Serial.println("disconnecting from server.");
      delay(1);
      client.stop();
      return;
    }

    Serial.println("btn_no = ");
    Serial.println(btn_no);
    memset(path, 0, sizeof(path));
    sprintf(path, "%s%s%s%d%s", "{\"device_id\":\"", device_id, "\",\"button_id\":\"", btn_no, "\"}");
    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: " + t4_server + "\r\n" +
                 "Connection: close\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + strlen(path) + "\r\n" +
                 "\r\n" + // This is the extra CR+LF pair to signify the start of a body
                 path + "\n");
    String line;
    while (client.connected())
    {
      if (client.available())
      {
        line = client.readStringUntil('}');
        Serial.println(line);
      }
    }
    if (line.indexOf("\"status\":\"success\"") > 0)
    {
      led_ctrl(btn_no - 1 , GREEN);
      Serial.println("Success");
    }
    else {
      led_ctrl(btn_no - 1, RED);
      Serial.println("failed");
    }
   }
    client.stop();
    delay(1000);
  }
  else
  {
    Serial.println("***Please check the wifi connection**");
  }

}

/*-----------------------------------------------------------------------------------*/
void led_ctrl(uint8_t led_num, uint8_t rgb)
{
  switch (led_num) {
    case LED1:
      if (rgb == RED) {
        digitalWrite(led1_r, 0);
        digitalWrite(led1_g, 1);
        digitalWrite(led1_b, 1);
      }
      else if (rgb == GREEN) {
        digitalWrite(led1_r, 1);
        digitalWrite(led1_g, 0);
        digitalWrite(led1_b, 1);
      }
      else if (rgb == BLUE) {
        digitalWrite(led1_r, 1);
        digitalWrite(led1_g, 1);
        digitalWrite(led1_b, 0);
      }
      else if (rgb == OFF) {
        digitalWrite(led1_r, 1);
        digitalWrite(led1_g, 1);
        digitalWrite(led1_b, 1);
      }
      break;

    case LED2:
      if (rgb == RED) {
        digitalWrite(led2_r, 0);
        digitalWrite(led2_g, 1);
        digitalWrite(led2_b, 1);
      }
      else if (rgb == GREEN) {
        digitalWrite(led2_r, 1);
        digitalWrite(led2_g, 0);
        digitalWrite(led2_b, 1);
      }
      else if (rgb == BLUE) {
        digitalWrite(led2_r, 1);
        digitalWrite(led2_g, 1);
        digitalWrite(led2_b, 0);
      }
      else if (rgb == OFF) {
        digitalWrite(led2_r, 1);
        digitalWrite(led2_g, 1);
        digitalWrite(led2_b, 1);
      }
      break;

    case LED3:
      if (rgb == RED) {
        digitalWrite(led3_r, 0);
        digitalWrite(led3_g, 1);
        digitalWrite(led3_b, 1);
      }
      else if (rgb == GREEN) {
        digitalWrite(led3_r, 1);
        digitalWrite(led3_g, 0);
        digitalWrite(led3_b, 1);
      }
      else if (rgb == BLUE) {
        digitalWrite(led3_r, 1);
        digitalWrite(led3_g, 1);
        digitalWrite(led3_b, 0);
      }
      else if (rgb == OFF) {
        digitalWrite(led3_r, 1);
        digitalWrite(led3_g, 1);
        digitalWrite(led3_b, 1);
      }
      break;

    case LED4:
      if (rgb == RED) {
        digitalWrite(led4_r, 0);
        digitalWrite(led4_g, 1);
        digitalWrite(led4_b, 1);
      }
      else if (rgb == GREEN) {
        digitalWrite(led4_r, 1);
        digitalWrite(led4_g, 0);
        digitalWrite(led4_b, 1);
      }
      else if (rgb == BLUE) {
        digitalWrite(led4_r, 1);
        digitalWrite(led4_g, 1);
        digitalWrite(led4_b, 0);
      }
      else if (rgb == OFF) {
        digitalWrite(led4_r, 1);
        digitalWrite(led4_g, 1);
        digitalWrite(led4_b, 1);
      }
      break;
    default:
      break;
  }
}
/*-----------------------------------------------------------------------------------*/
#if RE_WIFI_CONFIG_ENABLE
void start_AP_mode(char *default_ssid , char *default_password , char *channel)
{
  mode = 0;
  // attempt to start AP:
  if (status != WL_CONNECTED) {
    Serial.print("Attempting to start AP with SSID: ");
    Serial.println(ssid_default);
    status = WiFi.apbegin(default_ssid, default_password, channel);
  }
}
#endif
/*------------------------------------------------------------------------------------*/
void start_station_mode(char *connect_ssid, char *connect_password)
{
  mode = 1;
  if ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid_connect);
    status = WiFi.begin(connect_ssid, connect_password);
  }
#if !RE_WIFI_CONFIG_ENABLE
  if (status == WL_CONNECTED) {
    printWifiStatus();
  }
#endif
}
/*-----------------------------------------------------------------------------------*/
#if RE_WIFI_CONFIG_ENABLE
void get_ssid_password_from_flash(void)
{
  len1 = FlashMemory.buf[0];
  Serial.print("lenssid = ");
  Serial.println(FlashMemory.buf[0]);
  memcpy(ssid_connect, FlashMemory.buf + 1, len1);
  Serial.println(ssid_connect);
  len2 = FlashMemory.buf[len1 + 1];
  Serial.print("lenpassword = ");
  Serial.println(FlashMemory.buf[len1 + 1]);
  memcpy(password_connect, FlashMemory.buf + len1 + 2, len2);
  Serial.println("flash SSID");
  Serial.print(ssid_connect);
  Serial.println("flash PASSWORD");
  Serial.print(password_connect);
}
/*-----------------------------------------------------------------------------------*/
void reset_wifi_config(void)
{
  memset(FlashMemory.buf, 0xFF, (sizeof(ssid_connect) + sizeof(password_connect)));
  FlashMemory.update();
}
/*-----------------------------------------------------------------------------------*/
void disconnect_wifi(void)
{
  while (WiFi.disconnect() != WL_DISCONNECTED) {
    delay(1000);
  }
  while (WiFi.status() != WL_DISCONNECTED)
  {
    delay(50);
  }
}
#endif
/*-----------------------------------------------------------------------------------*/
void all_leds_blink(uint8_t led_status)
{
  if (led_status)
  {
    led_ctrl(LED1, GREEN);
    led_ctrl(LED2, GREEN);
    led_ctrl(LED3, GREEN);
    led_ctrl(LED4, GREEN);
  }
  else
  {
    led_ctrl(LED1, BLUE);
    led_ctrl(LED2, BLUE);
    led_ctrl(LED3, BLUE);
    led_ctrl(LED4, BLUE);
  }
  delay(1000);
  led_off();
}
