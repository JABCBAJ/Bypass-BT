
// matiene conectado a la conexion WIFI y nodered
void keepWiFiAlive(void * parameter){
    
    for(;;){

        vTaskDelay(3000 / portTICK_PERIOD_MS);

        if(now-time_mqtt_live > 30) { flag_mqtt_ok=false; }
        if(WiFi.status()!=WL_CONNECTED || flag_mqtt_ok==false) {
            WiFi.disconnect(true); WiFi_conect();
            flag_mqtt_ok=true;
        }

    }
}

