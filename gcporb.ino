//
// gcporb.ino
// joshiemoore 2022
//
// A sketch for ESP-32
// This sketch queries the GCP index at regular intervals and sets a
// connected LED strip to the same color as displayed on the GCP Dot
// website. This can be used to make GCP Dot mood lighting.
// http://www.gcpdot.com/gcpdot/
//
// This program is Free Software, licensed under the terms of GPLv3.
// https://www.gnu.org/licenses/gpl-3.0.txt
//

#include <HttpClient.h>
#include <TinyXML.h>
#include <FastLED.h>
#include <WiFi.h>

//
// User configuration
//
// Set this to your wifi SSID
#define WIFI_SSID "YOUR_SSID"
// Set this to your wifi password
#define WIFI_PASS "YOUR_PASSWORD"
// Set this to the ESP32 pin you connected the LED strip's data pin to
#define LED_PIN 2
// Set this to the number of LEDs on your LED strip
#define NUM_LEDS 14
//
// End of user configuration
//

CRGB leds[NUM_LEDS];

// GCP dot info
#define GCP_UPDATE_INTERVAL 30
#define GCP_HOST "www.gcpdot.com"
#define GCP_PATH "/gcpdot/gcpindex.php?current=1"
unsigned long last_gcp_update = 0;
float cur_gcp_index = -1;
int last_server_time = 0;
bool update_index = false;

// XML 
TinyXML xml;
uint8_t xml_buffer[4096];

// color data retrieved from gcpdot.com/gcpdot.js
#define NUM_COLORS 13
typedef struct _GCP_THRESH {
  float tail;
  CRGB color;
} GCP_THRESH;

GCP_THRESH COLOR_THRESHES[] = {
  { .tail = 0.00,   .color = CRGB(0xFF, 0x00, 0x64) },
  { .tail = 0.03,   .color = CRGB(0x84, 0x06, 0x07) },
  { .tail = 0.05,   .color = CRGB(0xC9, 0x5E, 0x00) },
  { .tail = 0.08,   .color = CRGB(0xC6, 0x90, 0x00) },
  { .tail = 0.15,   .color = CRGB(0xC6, 0xC3, 0x00) },
  { .tail = 0.23,   .color = CRGB(0xB0, 0xCC, 0x00) },
  { .tail = 0.30,   .color = CRGB(0x88, 0xC2, 0x00) },
  { .tail = 0.40,   .color = CRGB(0x00, 0xA7, 0x00) },
  { .tail = 0.90,   .color = CRGB(0x00, 0xB5, 0xC9) },
  { .tail = 0.9125, .color = CRGB(0x21, 0xBC, 0xF1) },
  { .tail = 0.93,   .color = CRGB(0x07, 0x86, 0xE1) },
  { .tail = 0.96,   .color = CRGB(0x00, 0x00, 0xFF) },
  { .tail = 1.00,   .color = CRGB(0xFF, 0xFF, 0xFF) }
};

// XML tag handler
void xml_callback(uint8_t status_flags, char* tag_name,
  uint16_t tag_name_len, char* tag_data, uint16_t data_len)
{
  if (tag_data == NULL) return;

  if (!strcmp(tag_name, "t"))
  {
    int new_time = atoi(tag_data);
    if (new_time > last_server_time)
    {
      last_server_time = new_time;
      update_index = true;
    }
  }
  if (!strcmp(tag_name, "/gcpstats/ss/s") && update_index)
  {
    cur_gcp_index = atof(tag_data);
  }
}

// retrieve current GCP index
void get_gcp_index()
{
  WiFiClient wc;
  HttpClient http(wc);
  int err = http.get(GCP_HOST, GCP_PATH);

  if (err != 0)
  {
    Serial.print("HTTP GET failed with error code: ");
    Serial.println(err);
    return;
  }
  
  err = http.responseStatusCode();
  if (err < 0)
  {
    Serial.print("responseStatusCode() failed with error: ");
    Serial.println(err);
    return;
  }

  if (err != 200)
  {
    Serial.print("HTTP GET returned error response: ");
    Serial.println(err);
    return;
  }

  // Parse GCP dot response
  while (http.connected() || http.available() > 0)
  {
    if (http.available() > 0)
    {
      xml.processChar(http.read());
    }
    else
    {
      delay(250);
    }
  }

  http.stop();
}

// return GCP dot color based on GCP index
CRGB gcp_index_to_color(float index)
{
  if (index == -1)
  {
    return CRGB(255, 255, 255);
  }

  for (int i = 0; i < NUM_COLORS - 1; i++)
  {
    if (index >= COLOR_THRESHES[i].tail && index < COLOR_THRESHES[i + 1].tail)
    {
      return COLOR_THRESHES[i].color;
    }
  }

  Serial.print("gcp_index_to_color() Invalid color index:");
  Serial.println(index);
  return CRGB(255, 255, 255);
}

// Set all LEDs in the strip to a solid color
void set_color(CRGB color)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = color;
  }
  FastLED.show();
}

// connect to wifi
void wifi_connect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Connecting to WiFi network %s", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);

  // init LED control
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  set_color(CRGB(255, 255, 255));

  // init XML parser
  xml.init((uint8_t *) xml_buffer, sizeof(xml_buffer), &xml_callback);

  // connect to wifi
  wifi_connect();

  // get initial
  get_gcp_index();
}

void loop() {
  if (update_index)
  {
    CRGB new_color = gcp_index_to_color(cur_gcp_index);
    set_color(new_color);
    update_index = false;
    Serial.println(cur_gcp_index);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wifi connection lost, reconnecting...");
    wifi_connect();
  }

  // refresh the index at regular intervals
  unsigned long local_time = millis();
  if (local_time - last_gcp_update > GCP_UPDATE_INTERVAL * 1000)
  {
    last_gcp_update = local_time;
    get_gcp_index();
  }
}
