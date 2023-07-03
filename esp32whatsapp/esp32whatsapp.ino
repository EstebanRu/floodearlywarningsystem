//Enviar un mensaje de WhatsApp desde ESP32
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "JuanRodrigo";
const char* password = "";
//COLOCAMOS EL TOKEN QUE NOS ENTREGA META
String token="EAAJFWZCI4qnkBAOBELxaIl2w2nM2SYho9vNLTxsGBIrXMMUFOnLuCpJtO10cXZAqJW4tvZCPNcLYkWKZA5jtPYTvXDsWJGYyZAepteSXPd4EaWIkopAQscZAwT8Dk9HnxC6fHqbGY7XNZCYCae2lZAlJtZA4SPOwJHQpEWBnoaawRA7A7db3JsDxcShLDG0DxZAeMvSuz5WpRPzAZDZD";
//COLOCAMOS LA URL A DONDE SE ENVIAN LOS MENSAJES DE WHATSAPP
String servidor = "https://graph.facebook.com/v17.0/104822046002864/messages";
//CREAMOS UNA JSON DONDE SE COLOCA EL NUMERO DE TELEFONO Y EL MENSAJE
String payload = "{\"messaging_product\":\"whatsapp\",\"to\":\"573215934095\",\"type\":\"text\",\"text\": {\"body\": \"HI\"}}";

float media = 0;

void setup() {
  Serial.begin(9600);
  //COLOCAMOS USUARIO Y CONTRASEÑA DE NUESTRA RED WIFI
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Se ha conectado al wifi con la ip: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  //SI SE DETECTA NIVEL PELIGROSO
  if (media >= 14) {

    if(WiFi.status()== WL_CONNECTED){
      //INICIAMOS EL OBJETO HTTP QUE POSTERIORMENTE ENVIARA EL MENSAJE
      HTTPClient http;
      //COLOCAMOS LA URL DEL SERVIDOR A DONDE SE ENVIARA EL MENSAJE
      http.begin(servidor.c_str());
      //COLOCAMOS LA CABECERA DONDE INDICAMOS QUE SERA TIPO JSON
      http.addHeader("Content-Type", "application/json"); 
      //AGREGAMOS EL TOKEN EN LA CABECERA DE LOS DATOS A ENVIAR
      http.addHeader("Authorization", token);    
      //ENVIAMOS LOS DATOS VIA POST
      int httpPostCode = http.POST(payload);
      //SI SE LOGRARON ENVIAR LOS DATOS
      if (httpPostCode > 0) {
        //RECIBIMOS LA RESPUESTA QUE NOS ENTREGA META
        int httpResponseCode = http.GET();
        //SI HAY RESPUESTA LA MOSTRAMOS
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      }
      http.end();
    }
    else {
      Serial.println("WiFi Desconectado");
    }
  }
  //SI NO SE DETECTA PELIGRO
  else{
    Serial.println("No hay peligro");
  }
}


