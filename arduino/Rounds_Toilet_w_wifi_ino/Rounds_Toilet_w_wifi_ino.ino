#include <WiFi.h>

// const int LED = 13;
const int MOTION = 8;
const int DOOR = 2;
const int  SD_CARD = 4;
int redPins[] = {9};
int greenPins[] = {5};
int bluePins[] = {6};



//const String updatePath = "/update/sensors";

////// network setup ///////
char ssid[] = "Rounds-4";     //  Rounds network SSID (name) 
char pass[] = "igetaround4";    //  network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char serverAddress[] = "office.gixoo.com";
int serverPort = 3000;
unsigned long lastAttemptTime = 0;     // last time you connected to the server, in milliseconds

WiFiClient client;


////// status flags ////////
int previous_door_status = -1;
int previous_motion_status= -1;

void setup() {
	// pinMode (LED, OUTPUT);
	pinMode (MOTION, INPUT);
	pinMode (DOOR, INPUT);
	pinMode (SD_CARD, OUTPUT);
	Serial.begin(9600);
	while(!Serial);
	setPinsMode(redPins, OUTPUT);
	setPinsMode(greenPins, OUTPUT);
	setPinsMode(bluePins, OUTPUT);
	setLedsColor(0,0,0);
	digitalWrite(SD_CARD, HIGH);  // Disable SD Card slot on the wifi shield(As it's not in use. Needed by the shield's library)
	Serial.println("I'm up!!!");
	delay(1000); // I found this helps the arduino find the shield more often...
	// check for the presence of the shield:
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("WiFi shield not present"); 
		while(true);
	} 
	// Try connecting until it works:
	setLedsColor(255,0, 225); //turn pink on to indicate wifi shield ok
	while (status != WL_CONNECTED) { 
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		status = WiFi.begin(ssid, pass);
		delay(1000);
	} 
	// When connected print Network Info
	printNetworkInfo();
	// setLedsColor(255,255,10);//turn yellow on to indicate connected to wifi but not to server
	// connectToServer();
	setLedsColor(0,255,0);//turn green on to indicate connected to wifi but not to server
}

void connectToServer(){
	Serial.print("Connecting to server...");
	while(!client.connect(serverAddress, serverPort)){
		delay(1000);
	}
	Serial.println("Connected!");
}

void loop () {
	int door  = digitalRead(DOOR);
	int motion = digitalRead(MOTION);
	if (door != previous_door_status || (motion != previous_motion_status && motion == HIGH)) {
		notify(door, motion);
		int status = isOccupide(door, motion);
		setStatus(status);
		previous_motion_status = motion;
		previous_door_status = door;
	}
	//  delay(100);
}


void notify(int door, int motion ) {
	char queryString[29] ;
	sprintf(queryString, "doorSensor=%d&motionSensor=%d" , door, motion);
	Serial.println("-----");
	while(!client.connected()){
		connectToServer();
		// setLedsColor(255,255,0);//turn green on to indicate connected to wifi but not to server
	}
	Serial.println("Making a POST request to update sensors...");
	String outBuffer = "POST /update/sensors HTTP/1.1\r\n"; 
	outBuffer +=  "Host: " + String(serverAddress) + "\r\n";
	outBuffer +=  "Connection: Keep-Alive\r\n";
	outBuffer +=  "Content-Type: application/x-www-form-urlencoded;\r\n";
	outBuffer +=  "Content-Length: " + String(strlen(queryString)) + "\r\n";
	outBuffer +=  "\r\n" + String(queryString);
	Serial.println(outBuffer);
	client.println(outBuffer);
	readResponse();
	Serial.println("-----");
}

void readResponse(){
	Serial.println("Available to read: " + String(client.available()));
	while(client.available() > 0) {
		char c = client.read();
		Serial.print("Read: " + c);
	}
	client.stop();
 }

String printNetworkInfo () {
	byte mac[6];
	byte encryption = WiFi.encryptionType();
	IPAddress ip = WiFi.localIP();
	Serial.print("Wifi Connected! \nEncryption Type:");
	Serial.println(encryption,HEX);
	Serial.print("ip Address is:");
	Serial.println(ip);
	WiFi.macAddress(mac);
	String macAddress = "MAC: " + String(mac[5],HEX) + ":" + String(mac[4],HEX) +  ":" +String(mac[3],HEX) +  ":" +String(mac[2],HEX) +  ":" +String(mac[2],HEX) +  ":" +String(mac[1],HEX) +  ":" +String(mac[0],HEX);
	Serial.println(macAddress);
	return macAddress;
}

// void ledBlink (int ledPin, int times) {
	 // Serial.println("Blinking " + String(times) + " times");
	// for (int i = 0; i < times; i++){
		// Serial.println(i);
		// digitalWrite(LED, HIGH);
		// delay(500);
		// digitalWrite(LED, LOW);
		// delay(600);
	// }
// }

void setStatus(int status){
	switch(status){
		case LOW:
			setLedsColor(0,0,0);
			break;
		case HIGH:
			setLedsColor(255,0,0);
			break;
		case 2:
			setLedsColor(0,0,255);
			break;
	}
}

void setLedsColor(int red, int green, int blue)
{
		setPinsValue(redPins, red);
		setPinsValue(greenPins, green);
		setPinsValue(bluePins, blue);
}

void setPinsValue(int pins[], int value){
	int i;
	int l;
	for(i = 0, l = sizeof(pins) / sizeof(*pins); i < l; i++){
		// Serial.println("Setting pin " + String(pins[i]) + " to value " + String(value));
		analogWrite(pins[i], value);
	} 
}

void setPinsMode(int pins[], int value){
	int i;
	int l;
	for(i = 0, l = sizeof(pins) / sizeof(*pins); i < l; i++){
		// Serial.println("Setting pin " + String(pins[i]) + " to mode " + String(value));
		pinMode(pins[i], value);
	} 
}

boolean isOccupide(int door, int motion) {
	int status;
	if (motion == HIGH){
		status = door == HIGH ? HIGH : LOW;
	}
	if(door == LOW){
		status = LOW;
	}
	return status;  
}



