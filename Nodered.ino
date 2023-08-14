
#define ESP32_RTOS

#include <esp_system.h>
#include "OTA.h"
#include "Definiciones.h"

#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

#include <HardwareSerial.h>
// HardwareSerial BTserial(2);  // Crea una instancia de HardwareSerial asociada al puerto 2 pin: 16RX 17TX
HardwareSerial BTserial(1);  // Crea una instancia de HardwareSerial asociada al puerto 1 pin:  9RX 10RX

#include <ESP32Time.h>
//#include <TelnetStream.h>       

#include "Comunicaciones.h"
#include "keepWiFiAlive.h"
#include "Ordenes_Nodered.h"

// Create an NTPClient to get the time
const char *ntpServer = "pool.ntp.org";
const long  gmtOffset_sec       = 3600;
       int  daylightOffset_sec  = 3600;

ESP32Time rtc;
struct tm timeinfo;

//·································································································································
//·································································································································

void setup() {

    WiFi.disconnect(true);
    delay(100);
    WiFi_conect();
    // OTA ESP32 
    setupOTA(Device, ssid, password); 

    //comunicaciones();
    
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    reconnect();

    Serial.begin(9600);     // Inicia el puerto serie 0
    // Serial.begin(9600, SERIAL_8N1, 16, 17);  // Inicia el puerto serie 2 con los pines GPIO17 (RX2) y GPIO16 (TX2)
    BTserial.begin(9600, SERIAL_8N1, 18, 19);  // Inicia el puerto serie 2 con los pines GPIO17 (RX2) y GPIO16 (TX2)

    // actualiza la hora de internet
    Actualiza_WebTime();
    time_mqtt_live=time(NULL);
    now = time(NULL);

    // Comprueba la conexión Wifi.Y RECONECTA si se ha perdido.
    xTaskCreatePinnedToCore(keepWiFiAlive, "keepWiFiAlive", 1023, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(delayX, "delayX", 1000, NULL, 0, NULL, 0);

}
//·································································································································
//·································································································································

void loop() {

    ArduinoOTA.handle();
    if (!client.connected()) {reconnect();}
    ordenes_Nodered();
    now = time(NULL);
    
    if (Serial.available()) {
        // captura datos desde arduino pro-mini
        String data = Serial.readString(); 
        // Bypass datos serie a BT HC06     
        BTserial.print(data);    
        // repite datos serie a Nodered
        for (int i = 0; i < sizeof(StatusON) / sizeof(StatusON[0]); i++) {
            if (data == StatusON[i]) {
                Topic = Devices[i];
                send_Nodered(Devices[i], "true");
                Serial.println(Devices[i] + (String) "-true");
            } else if (data == StatusOFF[i]) {
                Topic = Devices[i];
                send_Nodered(Devices[i], "false");    // deja pasar la cadena al puerto serie 1 al arduino pro mini
                Serial.println(Devices[i] + (String) "-false");
            }
        }
        data="";
    }

    // captura y reenvia datos entre el BT:BTserial y el arduino pro-mini y renvia a nodered desde BT:BTserial
    if (BTserial.available()) {
        String data = BTserial.readString();  // Lee un byte del puerto serie 2
        Serial.print(data);          // Imprime el byte en el puerto serie 0
    }

    // envia estado del dispositivo a node red
    static int statustime=time(NULL);
    //Serial.println(statustime);
    if (now-statustime > nodered_ciclo  || Update_Node_Red) {
        Update_Node_Red=false;
        statustime=time(NULL);
        status_device();
    }

    // actualiza cada 24h timeweb
    static int cycle24h;
    if(now-cycle24h > 24*60*60) {
        cycle24h=now;
        Actualiza_WebTime();  // UNA VEZ AL DIA
    }

    delay(100);
    client.loop();
}
//###########################################################################################

// manda a node-red el estado del sistema
void status_device() {

    client.publish( (TopicDevice+"/RSSI").c_str() , String ( WiFi.RSSI()).c_str() );
    client.publish( (TopicDevice+"/IP").c_str() , String ( WiFi.localIP().toString()).c_str() );
    client.publish( (TopicDevice+"/MAC").c_str() , String ( WiFi.macAddress()).c_str() );
    TelnetStream.print( WiFi.RSSI());
    TelnetStream.print( "-");
    TelnetStream.print( WiFi.localIP().toString());
    TelnetStream.print( "-");
    TelnetStream.println(WiFi.macAddress());
    TelnetStream.println(rtc.getTime()    );
    //    Serial.println(rtc.getTime() );
}


// publica datos a nodered
void send_Nodered(String topic, String value) {
    client.publish( (topic).c_str(), String ( value).c_str() );
}


// matiene conectado a la conexion WIFI y nodered
void delayX(void * parameter){
    for(;;){
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        flag_delayX=true;
    }
}



// actualiza tiempo desde internet
void Actualiza_WebTime() {
    
    // Obtener el último domingo de marzo a las 02:00 horario de verano
    if (timeinfo.tm_mon >= 2 && timeinfo.tm_wday == 0 && timeinfo.tm_mday >= 25 && timeinfo.tm_hour >= 10) {
        // El último domingo de marzo a las 02:00 se ha pasado, se cambia al horario de verano
        Serial.println("Cambiando al horario de verano");
        TelnetStream.println("Cambiando al horario de verano");
        daylightOffset_sec = 3600; // Ajuste horario de verano para España (1 hora)
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }

    // Obtener el último domingo de octubre a las 03:00 horario de invierno
    if (timeinfo.tm_mon >= 9 && timeinfo.tm_wday == 0 && timeinfo.tm_mday >= 25 && timeinfo.tm_hour >= 3) {
        // El último domingo de octubre a las 03:00 se ha pasado, se cambia al horario de invierno
        Serial.println("Cambiando al horario de invierno");
        TelnetStream.println("Cambiando al horario de verano");
        daylightOffset_sec = 0; // Ajuste horario de invierno para España (0 horas)
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }

    // actualiza la hora de internet
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(&timeinfo)) { Serial.println("Error al obtener la hora"); return; }
    if(getLocalTime(&timeinfo)) { rtc.setTimeStruct(timeinfo ); }
}


