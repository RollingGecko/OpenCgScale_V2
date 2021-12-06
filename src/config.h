#ifndef CONFIG_H
#define CONFIG_H

//Dummies

//#define SCALE_DUMMY

//Server 
const char *ssid = "MyESP32AP";
const char *password = "testpassword";
IPAddress local_IP(192,168,0,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

//PIN-Settings
//WEMOS Lolin32

#define SCALEDATAPIN_MAIN		13
#define SCALECLKPIN_MAIN		15
#define SCALEDATAPIN_LEFT		2
#define SCALECLKPIN_LEFT		0
#define SCALEDATAPIN_RIGHT		04
#define SCALECLKPIN_RIGHT		16

#define LASER_PIN				12
#define BUTTON_PIN				14
#define LED_PIN					27
#define VOLTAGE_ACCU_PIN		26

#define SCALEGAIN_MAIN			128
#define SCALEGAIN_LEFT			128
#define SCALEGAIN_RIGHT			128

//StandardScaleMultiplier

#define SCALEMULTIPLIER_FRONT   207.00
#define SCALEMULTIPLIER_RIGHT   210.61
#define SCALEMULTIPLIER_LEFT    199.73



#endif // CONFIG_H
