/* 
 * Improved Useless Box
 */

const int toggleSwitch = 7; 
const int limitSwitch = 8;
const int pin1 = 4;
const int pin2 = 5;

// Timing variables
unsigned long before = 0;
unsigned long actionDelay = 200;

// State machine definitions
enum State { IDLE, FAKEOUT, FORWARD, BACKWARD, PEEK, PAUSE, SLEEPY};

// Starts in idle state
State state = IDLE;

// Initialize pins
void setup() {
  Serial.begin(115200);
  pinMode(toggleSwitch, INPUT_PULLUP);
  pinMode(limitSwitch, INPUT_PULLUP);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  motor(0);
}

// Helper function for motor control 
// direction -1 is reverse, 0 is stop, 1 is forwards
void motor(int direction) {
  if (direction > 0) {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  } else if (direction < 0) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  } else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}


void loop() {
  unsigned long now = millis();
  bool toggle = digitalRead(toggleSwitch) == HIGH;
  bool limit = digitalRead(limitSwitch) == HIGH;

  switch (state) {

    case IDLE:
      if (toggle) {
        int r = random(0, 10);
        if (r < 4) {
          // 40% chance of "faking out"
          state = FAKEOUT;
          motor(1);
          actionDelay = random(150, 250);
        } else if (r >= 4 && r < 9) {
          // 50% chance of default forward motion
          state = FORWARD;
          motor(1);
        } else {
          // 10% chance the box "sleeps" before reponsing to toggle
          state = SLEEPY;
          actionDelay = random(300, 1000);
        }
        before = now;
      } else if (!toggle && !limit) {
        // ensure motor is stopped in idle state
        motor(0);
      } 
      break;

    case FAKEOUT:
      // Brief forward motion then reverse (don't actually press switch)
      if (now - before >= actionDelay) {
        motor(-1);
        state = BACKWARD;
        before = now;
      }
      break;

    case FORWARD:
      // Move forard until toggle is flipped off
      if (!toggle) {
        motor(-1);
        state = BACKWARD;
        before = now;
      }
      break;

    case BACKWARD:
      // Move back until limit is pressed
      if (limit) {
        int r = random(0, 10);
        if (r < 3) {
          // 30% chance to peek forward again
          motor(1);
          state = PEEK;
          actionDelay = random(100, 250);
        } else {
          // 70% chance of returning to idle state
          motor(0);
          state = IDLE;
        }
        before = now;
      }
      break;

    case PEEK:
      // Brief forward motion before pausing
      if (now - before >= actionDelay) {
        motor(0);
        actionDelay = random(1000, 1400);
        state = PAUSE;
        before = now;
      }
      break;

    case PAUSE:
      // Pause (during peak) before going back into the box
      if (now - before >= actionDelay) {
        motor(-1);
        state = BACKWARD;
        before = now;
      }
      break;

    case SLEEPY:
      // Extended delay (with motor off) before returning to idle state
      if (now - before >= actionDelay) {
        state = IDLE;
      }
      break;

  }
}