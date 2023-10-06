#include <Arduino.h>
#include <songBank.h>

// -------------------- PINS -----------------
#define CAR_POWER_PIN 12
#define AC_POWER_SWITCH_PIN 5
#define BUZZER 9
#define AC_RELAY_PIN 13

// ------------------ STATE CONSTANTS -----------------
#define IDLE 0
#define SWITCH_CONFIRM 1
#define SWITCH_CHANGE 2
#define AC_RELAY_INVERT 3
#define AC_ON_LONG 1
#define AC_ON_SHORT 11
#define AC_OFF 2
#define CLEANUP 10
#define CAR_OFF 0
#define CAR_ON 1

// ----------------- CONSTANTS --------------------
#define LOOP_TIME 0
#define ONE_SECOND 1000
#define ONE_MINUTE 60000      // 60,000 milliseconds = one minute
#define AC_ON_TIME_SHORT 2.5  // On time (minutes)
#define AC_ON_TIME_LONG 7.5   // On time (minutes)
#define AC_OFF_TIME 0.5       // Off time (minutes)

// ------------------ STATE VARIABLES ----------------
int carState;     // the state of the Car's power
int stateAC;      // the state of the AC State Machine
int timerACState; // the state of the AC Timer State Machine
bool switchAC;    // keeps track of the state of the A/C control switch (on / off)

// ------------------- TIMERS -----------------------
long switchConfirmTimer;
long timerAC;
long mainTimer;

// --------------------- HELPER FUNCTIONS -------------------------
/**
 * @brief Add to the end of loop function for debugging.
 *        Remove during installation so TX LED doesn't draw power.
 */
void printToSerial()
{
  Serial.print("SWITCH STATE MACHINE: ");
  Serial.print(stateAC);
  Serial.print(" | SWITCH (PULLDOWN): ");
  Serial.print(digitalRead(AC_POWER_SWITCH_PIN));
  Serial.print(" | switchAC bool: ");
  Serial.print(switchAC);
  Serial.print(" | CarState: ");
  Serial.print(carState);
  Serial.print(" | timerACState: ");
  Serial.print(timerACState);
  Serial.println("");
  mainTimer = millis();
}

/**
 * @brief Controls the logic for playing music on the Buzzer.
 *        If the AC power is on, then a song will play from the song bank.
 *        Nothing plays if the AC power is turned off.
 * 
 */
void playBuzzer()
{
  randomSeed(millis());
  int songSelection = random(3);  // Random number 0 to 2
  int mogustusRere = random(1,10);  // 1/10 chance that rere plays
  int* melody;
  float* rhythm;
  int bpm;
  int size;

  switch (songSelection)
  {
    case MOGUSTGUTUS:
      size = sizeof(susMogiusgtusMelody) / sizeof(int);
      rhythm = susMogiusgtusRhythm;
      bpm = susMogiusgtusBPM;
      
      if (mogustusRere) {
        melody = rereSusMogiusgtusMelody;
      }
      else {
        melody = susMogiusgtusMelody;
      }
      break;
    
    case STEREO_LOVE:
      size = sizeof(stereoLoveMelody) / sizeof(int);
      melody = stereoLoveMelody;
      rhythm = stereoLoveRhythm;
      bpm = stereoLoveBPM;
      break;

    case BETTER_OFF_ALONE:
      size = sizeof(betterOffAloneMelody) / sizeof(int);
      melody = betterOffAloneMelody;
      rhythm = betterOffAloneRhythm;
      bpm = betterOffAloneBPM;
      break;

  }
  playSong(BUZZER, melody, rhythm, size, bpm);
}

// ---------------------- STATE MACHINES ---------------------------

/**
 * @brief State machine for the timer controlling the air conditioning.
 *        AC turns on for AC_ON_TIME_SHORT or AC_ON_TIME_LONG depending on switch state,
 *        then turns off for AC_OFF_TIME.
 */
void timerStateMachine()
{
  switch (timerACState)
  {
  case IDLE:
    timerAC = millis();
    timerACState = AC_ON_LONG;
    digitalWrite(AC_RELAY_PIN, HIGH);
    break;

  case AC_ON_SHORT:
    digitalWrite(AC_RELAY_PIN, HIGH);
    if (millis() - timerAC > ONE_MINUTE * AC_ON_TIME_SHORT)
    {
      timerACState = AC_OFF;
      timerAC = millis();
    }
    break;

  case AC_ON_LONG:
    digitalWrite(AC_RELAY_PIN, HIGH);
    if (millis() - timerAC > ONE_MINUTE * AC_ON_TIME_LONG)
    {
      timerACState = AC_OFF;
      timerAC = millis();
    }
    break;

  case AC_OFF:
    digitalWrite(AC_RELAY_PIN, LOW);
    if ((millis() - timerAC > ONE_MINUTE * AC_OFF_TIME) && switchAC) {
      timerACState = AC_ON_SHORT;
      timerAC = millis();
    }
    else if ((millis() - timerAC > ONE_MINUTE * AC_OFF_TIME) && !switchAC) {
      timerACState = AC_ON_LONG;
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
void stateMachineAC(bool switchReadingAC)
{
  switch (stateAC)
  {
  case IDLE:
    if (switchReadingAC != switchAC)
    {
      stateAC = SWITCH_CONFIRM; // need to confirm that switch reading is actually a flip and not just noise
      switchConfirmTimer = millis();
    }
    break;

  case SWITCH_CONFIRM:
    if (millis() - switchConfirmTimer > ONE_SECOND * 0.5)
    { // wait for half a second in order to ignore noise in signal
      if (switchReadingAC != switchAC)
      {
        stateAC = SWITCH_CHANGE; // if after 0.5 seconds the switch is still fliped, then its confirmed
      }
      else
      {
        stateAC = IDLE; // switch not flipped after 0.5 seconds, false alarm
      }
    }
    break;

  case SWITCH_CHANGE:
    switchAC = !switchAC;
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
void setup()
{
  // among us song
  Serial.begin(9600);
  pinMode(CAR_POWER_PIN, INPUT);
  pinMode(AC_POWER_SWITCH_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(AC_RELAY_PIN, OUTPUT);

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
void loop()
{
  digitalWrite(LED_BUILTIN, LOW); // Doesn't work in setup for some reason
  bool carPower = digitalRead(CAR_POWER_PIN);

  switch (carState)
  {
  case CAR_OFF:
    carState = carPower;
    break;

  case CAR_ON:
    stateMachineAC(digitalRead(AC_POWER_SWITCH_PIN));
    timerStateMachine();

    if (carPower == CAR_OFF)
    {
      carState = CLEANUP;
    }
    break;

  case CLEANUP:
    delay(250);   // So you can hear the relay click after car turns off
    digitalWrite(AC_RELAY_PIN, LOW);
    carState = CAR_OFF;
    switchAC = false;
    timerACState = IDLE;
    break;
  }
}
