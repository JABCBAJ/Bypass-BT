//-----------------------------------------------------------------------------------------------
// ACTIVA O DESACTIVA DISPOSITIVOS SEGUN ORDENES DEsE NODE-RED
//-----------------------------------------------------------------------------------------------

bool ordenes_Nodered() {

    // solo para test
    if(receive_Topic!="") {

        // // TelnetStream.print("<<< topic  : ");
        // // TelnetStream.println(receive_Topic);
        // // TelnetStream.print("<<< payload: ");
        // // TelnetStream.println(receive_Payload);

        // habilitar/deshabilitar termo
        
        for (int i = 0; i < sizeof(Devices) / sizeof(Devices[0]); i++) {
            if (receive_Topic==Devices[i]) {
                Serial.println(Devices[i]);
                if (receive_Payload==OrdenesON[i]) {
                    Serial.print(receive_Payload);
                }
                if (receive_Payload==OrdenesOFF[i]) {
                    Serial.print(receive_Payload);
                }
            }   
        }
        
        // 
        if (receive_Topic==TopicDevice+"/Mqtt_Live") {
            if (receive_Payload=="true") {
                time_mqtt_live=now;
                flag_mqtt_ok=true;
            }
        }
        // // reinicio total ESP32 desde Nodered
        // if (receive_Topic==TopicDevice+"/reset") { 
        //     if (receive_Payload=="true")  {
        //         delay(2000);
        //         ESP.restart(); 
        //     }
        // }

        // borra buffer mqtt
        receive_Topic=""; receive_Payload="";
    }
}

