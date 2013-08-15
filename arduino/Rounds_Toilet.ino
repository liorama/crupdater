const int LED = 13;
const int DOOR = 12;
const int MOTION = 8;

int previous_door_status = 0;
int previous_motion_status= 0;


void setup() {
  pinMode (LED,OUTPUT);
  pinMode (MOTION, INPUT);
  pinMode (DOOR, INPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("I'm up!!!");
}


void loop () {
  int door  = digitalRead(DOOR);
  int motion = digitalRead(MOTION);
  if (door != previous_door_status || (motion != previous_motion_status && motion == HIGH)) {
    notify(door, motion);
    int status = isOccupide(door, motion);
    digitalWrite(LED, status);
    previous_motion_status = motion;
    previous_door_status = door;
 }
//  delay(100);
}

void notify(int door, int motion ) {
  char msg[20] ;
  sprintf(msg, "Door:%d, motion: %d" , door, motion);
  Serial.println(msg);
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


