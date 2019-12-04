// LinkIt One sketch for MQTT Demo

#include <LWiFi.h>
#include <LWiFiClient.h>
#include <PubSubClient.h>
#include <Wire.h>


#define I2CAddress 0x70
#define READINTERVAL  2000
#define PUBLISHINTERVAL 30000


///*
//	Modify to your WIFI Access Credentials.
//*/
#define WIFI_AP "TalTech"
#define WIFI_PASSWORD "" //"AALO402AB"
#define WIFI_AUTH LWIFI_OPEN  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define TOKEN "jHA7KcA99O4eAIdGHV2Z"


long crc;
int R0Value;
int VOCvalue;
int CO2value = 400;
int CO2_without_modify;
int VOC_without_modify;
byte data[10];
int i;
int k = 0;
long lastRead = 0;
char msg[50];
int value = 0;

///*
//	Modify to your MQTT broker - Select only one
//*/
char mqttBroker[] = "demo.thingsboard.io";
// byte mqttBroker[] = boker address; // modify to your local broker

LWiFiClient wifiClient;

PubSubClient client( wifiClient );

unsigned long lastSend;

void setup()
{
    Serial.begin( 115200 );
    delay( 10000 );
    Serial.println("board starting...");
    Wire.begin();
    pinMode(13, OUTPUT);

    InitLWiFi();

    Serial.println("SGX Sensortech VT-89");
    VZ89TEReadData(0x0D); //date code & revision
    Serial.print("date code & revision  ");  Serial.print(data[2]); Serial.print("-"); Serial.print(data[1]); Serial.print("-"); Serial.print(data[0]); Serial.print(", ");
    Serial.print("Revision: "); Serial.println(data[3]);
    Serial.print("R0 Calibration Value  ");
    VZ89TEReadData(0x10); //R0 Calibration Value
    R0Value = ((data[1] & 0x3F ) << 8) | data[0];
    Serial.print(R0Value); Serial.println(" kohm");
  
   client.setServer( mqttBroker, 1883 );
   lastSend = 0;
   Serial.println("end setup");
}

void loop()
{
  if( !client.connected() ) {
    Serial.println("reconnecting...");
    reconnect();
  }

  if( millis()-lastSend > 5000 ) {	// Send an update only after 5 seconds
    sendAnalogData();
    lastSend = millis();
  }

  client.loop();
}

void sendAnalogData() {
  // Read data to send
  long now = millis();

	lastRead = now;
	VZ89TEReadData(0x0C); //status
	CO2_without_modify = CO2value;
	VOC_without_modify = VOCvalue;

  // The relation of these two lines exists in the datasheet
	VOCvalue = 0.25*((data[0] - 13) * (1000 / 229))+0.75*VOCvalue;
	CO2value = 0.25*((data[1] - 13) * (1600 / 229) + 400)+0.75*CO2value;

  // Just debug messages
  Serial.println( "Sending analog data" );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"CO2Level\":"; payload += CO2value;  payload += ", ";
  payload += "\"VOCLevel\":"; payload += VOCvalue;
  payload += "}";
 
  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/attributes", attributes );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
  Serial.println("End of send analog data func");
}

void VZ89TEReadData(long request)
{

  crc = request;
  crc = (byte)(crc + (crc / 0x100));
  crc = 0xFF - crc;

  // Initiate Comms to device, initiate measure and read bytes of data
  Wire.beginTransmission(I2CAddress);
  Wire.write(request);  Wire.write(0);  Wire.write(0);  Wire.write(0);  Wire.write(0);  Wire.write(crc);
  Wire.endTransmission();
  delay(100);
  Wire.requestFrom(I2CAddress, 7);
  for (i = 0; i < 7; i++)
    {
      data[i] = Wire.read();
    }

  crc = data[0] + data[1] + data[2] + data[3] + data[4] + data[5];
  crc = (byte)(crc + (crc / 0x100));
  crc = 0xFF - crc;
}

void InitLWiFi()
{
  LWiFi.begin();
  // Keep retrying until connected to AP
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD))) {
    delay(1000);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  Serial.println("Reconnect function");
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker ...");

    // Attempt to connect
    if ( client.connect("LinkIt One Device", TOKEN, NULL) ) {	// Better use some random name
      Serial.println("Successfully connected ...");
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
