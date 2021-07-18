#include <WiFi.h>
#include <M5Stack.h>
#include <HTTPClient.h>
#include <ESP8266Audio.h>
#include "urlencode.h"

// WiFi settings
// define the macros below
// WIFI_SSID: WiFi SSID
// WIFI_PASS: WiFi Password
#include "credentials.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceHTTPStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2S *out;
WiFiClientSecure *wifi_client;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  Serial.println("Connecting to WiFi");

  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting");
    delay(1000);
  }

  Serial.println("WiFi Connected");

  wifi_client = new WiFiClientSecure();
  wifi_client->setCACert(ROOT_CA);

  HTTPClient http_client;

  char encoded[1024];
  url_encode("こんにちはこんにちは。いかがでしょうか？", encoded);
  char url[1024];
  sprintf(url, "https://asia-northeast1-paparobot.cloudfunctions.net/paparobot-tts?text=%s", encoded);
  Serial.println(url);

  // if (http_client.begin(wifi_client, url)) {
  //   http_client.setAuthorization(TTS_USER, TTS_PASS);
  //   Serial.println(http_client.GET());
  // }

  audioLogger = &Serial;

  file = new AudioFileSourceHTTPStream();
  file->setClient(wifi_client);
  file->setAuthorization(TTS_USER, TTS_PASS);
  file->RegisterStatusCB(StatusCallback, (void*)"HTTP");
  file->open(url);
  file->RegisterMetadataCB(MDCallback, (void*)"ICY");
  buff = new AudioFileSourceBuffer(file, 2048);
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
  out = new AudioOutputI2S(0, 1);
  out->SetOutputModeMono(true);
  out->SetGain(0.6);
  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  mp3->begin(buff, out);

}

void loop() {
  static int lastms = 0;

  if (mp3->isRunning()) {
    if (millis()-lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
     }
    if (!mp3->loop()) mp3->stop();
  } else {
    Serial.printf("MP3 done\n");
    delay(1000);
  }}
