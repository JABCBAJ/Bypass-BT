// Dispositivo remoto en la red, a de coincidir con el topico en nodered "device/#"
// String TopicDevice      = "xxxxxx";  // porton prototype
// const char* Device      = "XXXXXX";  // PORTON PROTOTIPO
String TopicDevice      = "prototype";  // porton prototype
const char* Device      = "PROTOTIPO";  // PORTON PROTOTIPO

const char* ssid        = "VIRUSLAN";
const char* password    = "1Q2W3E4R5T6Y7U8I9O0P$";
const char* hostname     = "AstralClient";

#define Wifi_Reconect 100

#define mqttServer    "192.168.1.59"
#define mqttPort      1883
#define clientID      "AstralClient"

int nodered_ciclo = 10;

time_t now, time_mqtt_live, lastime1;

// desde Mqtt
String  receive_Payload;
String  receive_Topic;
String  Topic;

bool    Update_Node_Red    =false;
bool    flag_delayX        =false;
bool    flag_mqtt_ok       =true;

String  Devices[]      = {TopicDevice+"/MOTOR",   TopicDevice+"/CLORO",   TopicDevice+"/FOCO"};
String  OrdenesOFF[]   = {"A373528", "B373528", "C373528"};
String  OrdenesON[]    = {"a373528", "b373528", "c373528"};
String  StatusOFF[]    = {"<Imgs00:0", "<Imgs01:0", "<Imgs02:0"};   
String  StatusON[]     = {"<Imgs00:1", "<Imgs01:1", "<Imgs02:1"};



String Comand_Prog  = {"@373528"};              // Comand Encript for program alarms mode
String Comand_Reloj = {"T373528"};              // Comand Encript for time display mode
String Comand_Timer = {"L373528"};              // Comand Encript for timer mode temporizador 15 minutos

String StatusTimer  = {"<Imgs16:1\n\n"};        // status temporizador 15 minutos
// <TtoS01:  temporizador 15 minutos
// TtoS01: Apagado del foco