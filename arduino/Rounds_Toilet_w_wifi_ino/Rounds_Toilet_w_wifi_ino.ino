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
char ssid[] = "Rounds-3";     //  Rounds network SSID (name) 
char pass[] = "igetaround3";    //  network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char serverAddress[] = "office.gixoo.com";
int serverPort = 3000;
unsigned long lastAttemptTime = 0;     // last time you connected to the server, in milliseconds

WiFiClient client;


////// status flags ////////
int previous_door_status = -1;
int previous_motion_status= -1;

void setup() {
	pinMode (MOTION, INPUT);
	pinMode (DOOR, INPUT);
	pinMode (SD_CARD, OUTPUT);
	if (Serial){
		Serial.begin(9600);
	}
	// while(!Serial && Serial);
	setPinsMode(redPins, OUTPUT);
	setPinsMode(greenPins, OUTPUT);
	setPinsMode(bluePins, OUTPUT);
	setLedsColor(0,0,0);
	digitalWrite(SD_CARD, HIGH);  // Disable SD Card slot on the wifi shield(As it's not in use. Needed by the shield's library)
	Serial && Serial.println("I'm up!!!");
	delay(1000); // I found this helps the arduino find the shield more often...
	// check for the presence of the shield:
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial && Serial.println("WiFi shield not present"); 
		while(WiFi.status() == WL_NO_SHIELD){
			setLedsColor(255,0,0);
			delay(500);
			setLedsColor(0,0,0);
			delay(400);
		};
	} 
	// Try connecting until it works:
	setLedsColor(255,0, 225); //turn pink on to indicate wifi shield ok
	while (status != WL_CONNECTED) { 
		Serial && Serial.print("Attempting to connect to SSID: ");
		Serial && Serial.println(ssid);
		status = WiFi.begin(ssid, pass);
		delay(1000);
	} 
	// printNetworkInfo(); // When connected print Network Info
	setLedsColor(0,255,0); //turn green on to indicate connected to wifi
}

void connectToServer(){
	Serial && Serial.print("Connecting to server.");
	while(!client.connect(serverAddress, serverPort)){
		Serial && Serial.print(".");
		delay(50);
	}
	Serial && Serial.println("Connected!");
}

void loop () {
	int door  = digitalRead(DOOR);
	delay(2500); // For a situation that both sensors get triggered when a person steps out of the room (this will prevent the status changing to occupied)
	int motion = digitalRead(MOTION);
	Serial && Serial.println("Door: " + String(door) + " Motion: " + String(motion));
	if (door != previous_door_status || (motion != previous_motion_status && motion == HIGH)) {
		notify(door, motion);
		int status = isOccupide(door, motion);
		setStatus(status);
		previous_motion_status = motion;
		previous_door_status = door;
	}
}


void notify(int door, int motion ) {
	char queryString[19] ;
	sprintf(queryString, "d=%d&m=%d&t=%lu" , door, motion, millis());
	Serial && Serial.println("-----");
	while(!client.connected()){
		connectToServer();
	}
	Serial && Serial.println("Making a request to update sensors...");
	// String outBuffer = "POST / HTTP/1.1\r\n"; 
	String outBuffer = "GET /?"  + String(queryString) + " HTTP/1.1\r\n";  // Using GET cause it's faster here, should be switched to post
	// outBuffer +=  "Host: " + String(serverAddress) + "\r\n";
	outBuffer +=  "Connection: close\r\n";
	// outBuffer +=  "Content-Type: application/x-www-form-urlencoded;\r\n";
	// outBuffer +=  "Content-Length: " + String(strlen(queryString)) + "\r\n";
	// outBuffer +=  "\r\n" + String(queryString);
	client.println(outBuffer);
	client.println();
	Serial && Serial.println(outBuffer);
	// while(client.connected()){
		// if(client.available()){
			// readResponse();
		// }
	// }
	Serial && Serial.println("-----");
	client.flush();
	client.stop();

}


// this was meant for reading the server response for updating status and checking for if the request had errors which caused the server not to be updated
// void readResponse(){
	// Serial && Serial.print("Read:");

	// String response;
	// char charRead = NULL;
	// while (client.available()) {
		// charRead = client.read();
		// if(charRead != (char) - 1) {
			// response += charRead;
		// }
		// delay(150);
	// }
	// Serial && Serial.println(response);
 // }

String printNetworkInfo () {
	byte mac[6];
	byte encryption = WiFi.encryptionType();
	IPAddress ip = WiFi.localIP();
	Serial && Serial.print("Wifi Connected! \nEncryption Type:");
	Serial && Serial.println(encryption,HEX);
	Serial && Serial.print("ip Address is:");
	Serial && Serial.println(ip);
	WiFi.macAddress(mac);
	String macAddress = "MAC: " + String(mac[5],HEX) + ":" + String(mac[4],HEX) +  ":" +String(mac[3],HEX) +  ":" +String(mac[2],HEX) +  ":" +String(mac[2],HEX) +  ":" +String(mac[1],HEX) +  ":" +String(mac[0],HEX);
	Serial && Serial.println(macAddress);
	return macAddress;
}

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
		// Serial && Serial.println("Setting pin " + String(pins[i]) + " to value " + String(value));
		analogWrite(pins[i], value);
	} 
}

void setPinsMode(int pins[], int value){
	int i;
	int l;
	for(i = 0, l = sizeof(pins) / sizeof(*pins); i < l; i++){
		// Serial && Serial.println("Setting pin " + String(pins[i]) + " to mode " + String(value));
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



