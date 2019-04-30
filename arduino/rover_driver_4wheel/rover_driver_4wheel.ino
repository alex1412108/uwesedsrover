/*

*/
#include <avr/wdt.h>


char INPUTS[19]; 
enum INPUT_FIELDS {
  INPUT_A,
  INPUT_B,
  INPUT_X,
  INPUT_Y,
  INPUT_RB,
  INPUT_LB,
  INPUT_MENU,
  INPUT_LT,
  INPUT_RT,
  INPUT_UP,
  INPUT_RIGHT,
  INPUT_DOWN,
  INPUT_LEFT,
  INPUT_LS,
  INPUT_RS,
  INPUT_LX,
  INPUT_LY,
  INPUT_RX,
  INPUT_RY
};
boolean NEW_INPUT = false;


//Motor PWM
const int MOTOR_RF_0 = 7;  // PWM 7
const int MOTOR_RF_1 = 6;  // PWM 8

//const int MOTOR_RM_0 = 24;  // DIG 4 
//const int MOTOR_RM_1 = 26;  // DIG 3 

const int MOTOR_RB_0 = 12;  // PWM 2
const int MOTOR_RB_1 = 13;  // PWM 1

const int MOTOR_LB_0 = 8;  // PWM 6
const int MOTOR_LB_1 = 9;  // PWM 5

//const int MOTOR_LM_0 = 13;  // DIG 1
//const int MOTOR_LM_1 = 12;  // DIG 2

const int MOTOR_LF_0 = 11;  // PWM 3
const int MOTOR_LF_1 = 10;  // PWM 4

//Servo PWM
const int SERVO_RF = 5;  // PWM 9
const int SERVO_RB = 3;  // PWM 11
const int SERVO_LB = 2;  // PWM 12
const int SERVO_LF = 4;  // PWM 10
const int SCOOP = 45;  // PWM 13

//Motor board settings
const int MOTOR_DECAY = 36;
const int MOTOR_SLEEP = 34;
const int MOTOR_RESET = 32;

const int SHDN6V = 23;  // Digital 6
const int SHDN12V = 22;  // Digital 5

//Fault monitoring
const int MOTOR_FAULT1 = 37;
const int MOTOR_FAULT2 = 35;
const int MOTOR_FAULT3 = 33;


void setup() {
  //wdt_enable(WDTO_2S);
  Serial.begin(9600);
  
  // Configure end effectors.
  pinMode(MOTOR_RF_0, OUTPUT);
  pinMode(MOTOR_RF_1, OUTPUT);
  //pinMode(MOTOR_RM_0, OUTPUT);
  //pinMode(MOTOR_RM_1, OUTPUT);
  pinMode(MOTOR_RB_0, OUTPUT);
  pinMode(MOTOR_RB_1, OUTPUT);
  pinMode(MOTOR_LB_0, OUTPUT);
  pinMode(MOTOR_LB_1, OUTPUT);
  //pinMode(MOTOR_LM_0, OUTPUT);
  //pinMode(MOTOR_LM_1, OUTPUT);
  pinMode(MOTOR_LF_0, OUTPUT);
  pinMode(MOTOR_LF_1, OUTPUT);
  
  pinMode(SERVO_RF, OUTPUT);
  pinMode(SERVO_RB, OUTPUT);
  pinMode(SERVO_LB, OUTPUT);
  pinMode(SERVO_LF, OUTPUT);
  pinMode(SCOOP, OUTPUT);
  
  // Configure status light.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}


void loop() {
  /*
  if (Serial.available() == 19) {
    Serial.readBytes(INPUTS, 19);
    //wdt_reset();
  } */
  
  recvWithStartEndMarkers();
  
  if (NEW_INPUT == true) {
    if (INPUTS[INPUT_A] == char(1)) {
      digitalWrite(LED_BUILTIN, HIGH);  
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    // Update motor signals.
    control_motor_pwm(MOTOR_RF_0, MOTOR_RF_1, INPUTS[INPUT_RY]);
    control_motor_pwm(MOTOR_RB_0, MOTOR_RB_1, INPUTS[INPUT_RY]);
    control_motor_pwm(MOTOR_LF_0, MOTOR_LF_1, INPUTS[INPUT_LY]);
    control_motor_pwm(MOTOR_LB_0, MOTOR_LB_1, INPUTS[INPUT_LY]);
  
    // Update scoop signal.
    //
    NEW_INPUT = false;
  }
}


void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    static int numChars = 19;
    char startMarker = char(255);
    char endMarker = char(254);
    char rc;
 
    while (Serial.available() > 0 && NEW_INPUT == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                INPUTS[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                recvInProgress = false;
                ndx = 0;
                NEW_INPUT = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}


void read_inputs() {
  if (Serial.available() >= 19) {
    Serial.readBytes(INPUTS, 19);
    //wdt_reset();
  }
}


void set_status_light() {
  if (INPUTS[INPUT_A] == char(1)) {
    digitalWrite(LED_BUILTIN, HIGH);  
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}


void control_motor_pwm(int pin0, int pin1, char value) {
  if (value > 137) {
    analogWrite(pin0, ((int)value - 127) * 2 - 1);
    analogWrite(pin1, 0);
  } else if (value < 117) {
    analogWrite(pin0, 0);
    analogWrite(pin1, 255 - ((int)value * 2));
  } else {
    analogWrite(pin0, 0);
    analogWrite(pin1, 0);
  }
}


void control_motor(int pin0, int pin1, char value) {
  if (value > 137) {
    digitalWrite(pin0, HIGH);
    digitalWrite(pin1, LOW);
  } else if (value < 117) {
    digitalWrite(pin0, LOW);
    digitalWrite(pin1, HIGH);
  } else {
    digitalWrite(pin0, LOW);
    digitalWrite(pin1, LOW);
  }
}