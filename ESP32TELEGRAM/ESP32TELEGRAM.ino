#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
// Wifi network station credentials
#define WIFI_SSID "PC8158"
#define WIFI_PASSWORD "12345678"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6373937960:AAGkD4zyun-Y093AiL5kkXqn0-HzpulOpMM"
const unsigned long BOT_MTBS = 1000; // mean time between scan messages
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done
bool bandera = true;
bool bandera1 = true;
int nivelBT = 15;
String chat_id = "5282728879";

void setup(){
    
    Serial.begin(115200);
    Serial.println();
// attempt to connect to Wifi network:
    Serial.print("Connecting to Wifi SSID ");
    Serial.print(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
      while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(500);
      }
    Serial.print("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Retrieving time: ");
    configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
    time_t now = time(nullptr);
      while (now < 24 * 3600){
        Serial.print(".");
        delay(100);
        now = time(nullptr);
      }
    Serial.println(now);
}

void loop(){

  if (nivelBT > 10 ){
      if (bandera==true){
      String welcome = "Se ha decretado alerta ROJA!!. Siga las recomendaciones de las autoridades locales";
      bot.sendMessage(chat_id, welcome, "Markdown");
      bandera=false;
    }
  }

  if (nivelBT > 10 && nivelBT < 15 ){
      if (bandera1==true){
      String welcome = "Se ha decretado alerta ROJA!!. Siga las recomendaciones de las autoridades locales";
      bot.sendMessage(chat_id, welcome, "Markdown");
      bandera1=false;
    }
  }

   if (nivelBT > 5 && nivelBT <= 10 ){
      if (bandera==true){
      String welcome = "Se ha decretado alerta AMARILLA. Siga las recomendaciones de las autoridades locales";
      bot.sendMessage(chat_id, welcome, "Markdown");
      bandera=false;
    }
  }


  if (millis() - bot_lasttime > BOT_MTBS){
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    bot_lasttime = millis();
  }
}