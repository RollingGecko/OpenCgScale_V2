#ifndef CONFIG_H
#define CONFIG_H

//Dummies

#define SCALE_DUMMY

//Server 
const char *ssid = "MyESP32AP";
const char *password = "testpassword";
IPAddress local_IP(192,168,0,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

//StandardScaleMultiplier

#define SCALEMULTIPLIER_FRONT  500
#define SCALEMULTIPLIER_RIGHT  500
#define SCALEMULTIPLIER_LEFT  500

#endif // CONFIG_H
