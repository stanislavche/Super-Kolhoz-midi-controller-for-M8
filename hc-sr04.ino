#include <HCSR04.h>
#include <MIDI.h>
#define btnPin 2

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

byte current_value;
byte previous_value;
double new_value;
float data;
double difference;
const int channel = 15;
const int controller = 7;
int pushButton_pin = 2;
bool turnOFFHCSR04Flag = false;
int old_poten[] = {0,1,2,3,4,5};
byte analogVal = 0;

UltraSonicDistanceSensor sensor(0, 1);

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(57600);
  Serial.println("MIDI Input Test");
  pinMode(btnPin, INPUT_PULLUP);
}

void loop () {  
  uint8_t btn = digitalRead(pushButton_pin);
  // When button LOW we turn on HCSR04
  if (btn == LOW)
  {
    turnOFFHCSR04Flag = true;
    // Serial.println("Button ON");
    data = sensor.measureDistanceCm();
    if (data >= 3.0 && data <= 40.0) {
      if (data > 32.0) {
        current_value = 127;
      } else {
        current_value = map(data * 2, 6.0, 64.0, 0, 127);
      }
      
      difference = current_value - previous_value;
      
      if (current_value != previous_value  ) {
        previous_value = current_value;
        if (difference >= -20 && difference <= 20) {
          Serial.println(current_value);
          sendNoteControll(controller, current_value, channel);
        }
      }
    }
  } else {
    // When button HIGH send off note once for HCSR04
    // Serial.println("Button OFF");
    if (turnOFFHCSR04Flag == true) {
      turnOFFHCSR04Flag = false;
      switchOffController(controller, channel);
    }
  }

  // Analog read
  for(int i = 0; i < 6; i ++) {
    int poten = analogVal = analogRead(i) / 8; // read value of a potentiometer
    // (which is analog input 0 - 5)
    // depending on which for() loop we are at

    if (abs(poten - old_poten[i]) >= 2){
      old_poten[i] = poten;  // save the changed value
       // let's send a control change message
      sendNoteControll(i, (127 - analogVal), channel);
    }
  }

  delay(10);
}

void switchOffController(int cont, int ch){
  usbMIDI.sendNoteOff(cont, 0, ch);
  MIDI.sendNoteOff(cont, 0, ch);
}
void sendNoteControll(int cont, int val, int ch) {
  usbMIDI.sendControlChange(cont, val, ch);
  MIDI.sendControlChange(cont, val, ch);
}
