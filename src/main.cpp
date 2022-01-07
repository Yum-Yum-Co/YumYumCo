#include <Arduino.h>
#include <songBank.h>

// -------------------- PINS -----------------
#define CAR_POWER_PIN 2 
#define AC_POWER_SWITCH_PIN 5
#define BUZZER 6
#define AC_RELAY_PIN 7
 
// ------------------ STATE CONSTANTS -----------------
#define IDLE 0
#define SWITCH_CONFIRM 1
#define SWITCH_CHANGE 2
#define AC_RELAY_INVERT 3
#define AC_ON 1
#define AC_OFF 2
#define CLEANUP 10
#define CAR_OFF 0
#define CAR_ON 1

// ----------------- CONSTANTS --------------------
#define LOOP_TIME 0
#define ONE_SECOND 1000
#define ONE_MINUTE 60000  // 60,000 milliseconds = one minute

// ------------------ STATE VARIABLES ---------------- 
int carState; // the state of the Car's power
int stateAC;  // the state of the AC State Machine
int timerACState;   // the state of the AC Timer State Machine
bool powerAC;   // keeps track of the state of the A/C power (on / off)
 
// ------------------- TIMERS -----------------------
long switchConfirmTimer;
long timerAC;
long mainTimer;

// --------------------- HELPER FUNCTIONS -------------------------

/**
 * @brief Controls the logic for playing music on the Buzzer.
 *        If the AC power is on, then a song will play from the song bank.
 *        Nothing plays if the AC power is turned off.
 * 
 */
void playBuzzer(){
  if(powerAC){
    int size = sizeof(susMogiusgtusMelody) / sizeof(int); // Get the length of your array/melody

    if(random(1,4) == 3){  // 1/20 chance that rere mogustgistus plays
      playSong(BUZZER, rereSusMogiusgtusMelody, susMogiusgtusRhythm, size, susMogiusgtusBPM);
    }
    else{
      playSong(BUZZER, susMogiusgtusMelody, susMogiusgtusRhythm, size, susMogiusgtusBPM);
    }
  }
  else{
    noTone(BUZZER);
  }
}

// ---------------------- STATE MACHINES ---------------------------

/**
 * @brief State machine for the timer controlling the air conditioning.
 *        AC turns on for one minute and then turns off for 3 minutes.
 */
void timerStateMachine(){
  // Serial.print("timerState: ");
  // Serial.print(timerACState);
  // Serial.print(" | timerAC: ");
  // Serial.print(millis() - timerAC);
  // Serial.println("");

  switch(timerACState){
    case IDLE:
      timerAC = millis();
      timerACState = AC_ON;
      digitalWrite(AC_RELAY_PIN, HIGH);
      break;

    case AC_ON:
      if(millis() - timerAC > ONE_MINUTE){
        timerACState = AC_OFF;
        digitalWrite(AC_RELAY_PIN, LOW);
        timerAC = millis();
      }
      break;
    
    case AC_OFF:
      if(millis() - timerAC > ONE_MINUTE*3){
        timerACState = AC_ON;
        digitalWrite(AC_RELAY_PIN, HIGH);
        timerAC = millis();
      }
      break;
  }
}

/**
 * @brief The state machine for the AC power switch. Controls the logic surrounding on/off functions for the AC system
 * 
 * @param switchReadingAC digital reading of the AC_POWER_SWITCH_PIN. Either [true = on] or [false = off]
 */
void stateMachineAC(bool switchReadingAC){
  switch(stateAC){
    case IDLE:
      if (switchReadingAC != powerAC){
        stateAC = SWITCH_CONFIRM;   // need to confirm that switch reading is actually a flip and not just noise
        switchConfirmTimer = millis();
      }
      break;

      case SWITCH_CONFIRM:
        if (millis() - switchConfirmTimer > ONE_SECOND*0.5){  // wait for half a second in order to ignore noise in signal
          if (switchReadingAC != powerAC){
            stateAC = SWITCH_CHANGE;  // if after 0.5 seconds the switch is still fliped, then its confirmed
          }
          else{
            stateAC = IDLE;   // switch not flipped after 0.5 seconds, false alarm
          }
        }
        break;
      
      case SWITCH_CHANGE:
        powerAC = !powerAC;
        playBuzzer();
        stateAC = IDLE;
        break;
  }
}

// ----------------------- LOOPS ------------------------------------

/**
 * @brief Setup loop runs once during power-on of Arduino. Runs main loop after this one is done. 
 * The loop sings the among us theme.
 */
void setup() {
  // among us song
  Serial.begin(9600);
  pinMode(CAR_POWER_PIN, INPUT);
  pinMode(AC_POWER_SWITCH_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(AC_RELAY_PIN, OUTPUT);

  //digitalWrite(AC_RELAY_PIN, digitalRead(AC_POWER_SWITCH_PIN));   // sets relay to original power mode when car starts up

  stateAC = IDLE;
  timerACState = IDLE;
  carState = digitalRead(CAR_POWER_PIN);
  mainTimer = millis();
}

/**
 * @brief This loop  runs infinitely during Arduino's operation. If car is not powered, then all output pins will be set to LOW.
 *        Once car is powered again then it pretty much resets and all states will be acquired from reading signals on input pins.
 * 
 */
void loop() {
  bool carPower = digitalRead(CAR_POWER_PIN);

  switch(carState){
    case CAR_OFF:
      carState = carPower;
      break;

    case CAR_ON:
      // Serial.print("AC STATE: ");
      // Serial.print(stateAC);
      // Serial.print(" | AC SWITCH: ");
      // Serial.print(digitalRead(AC_POWER_SWITCH_PIN));
      // Serial.print(" | powerAC: ");
      // Serial.print(powerAC);
      // Serial.println("");

      stateMachineAC(digitalRead(AC_POWER_SWITCH_PIN));

      if(powerAC){
        timerStateMachine();
      }
      else{
        timerACState = IDLE;
        digitalWrite(AC_RELAY_PIN, LOW);
      }

      if (carPower == CAR_OFF){
        carState = CLEANUP;
      }
      break;
    
    case CLEANUP:
      digitalWrite(AC_RELAY_PIN, LOW);
      carState = CAR_OFF;
      powerAC = false;
      break;
      
  }
  while(millis() - mainTimer < LOOP_TIME){
    }
  mainTimer = millis();
}