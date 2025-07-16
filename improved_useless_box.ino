const int toggleSwitch = 7; 
const int limitSwitch = 8;
const int pin1 = 4;
const int pin2 = 5;

unsigned long before = 0;
unsigned long actionDelay = 200;

enum State { IDLE, FAKEOUT, FORWARD, BACKWARD, PEEK, PAUSE, SLEEPY};

State state = IDLE;

void setup() {
  Serial.begin(115200);
  pinMode(toggleSwitch, INPUT_PULLUP);
  pinMode(limitSwitch, INPUT_PULLUP);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  motor(0);
}

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
          state = FAKEOUT;
          motor(1);
          actionDelay = random(150, 250);
        } else if (r >= 4 && r < 9) {
          state = FORWARD;
          motor(1);
        } else {
          state = SLEEPY;
          actionDelay = random(300, 1000);
        }
        before = now;
      } else if (!toggle && !limit) {
        motor(0);
      } 
      break;

    case FAKEOUT:
      if (now - before >= actionDelay) {
        motor(-1);
        state = BACKWARD;
        before = now;
      }
      break;

    case FORWARD:
      if (!toggle) {
        motor(-1);
        state = BACKWARD;
        before = now;
      }
      break;

    case BACKWARD:
      if (limit) {
        int r = random(0, 10);
        if (r < 3) {
          motor(1);
          state = PEEK;
          actionDelay = random(100, 250);
        } else {
          motor(0);
          state = IDLE;
        }
        before = now;
      }
      break;

    case PEEK:
      if (now - before >= actionDelay) {
        motor(0);
        actionDelay = random(1000, 1400);
        state = PAUSE;
        before = now;
      }
      break;

    case PAUSE:
      if (now - before >= actionDelay) {
        motor(-1);
        state = BACKWARD;
        before = now;
      }
      break;

    case SLEEPY:
      if (now - before >= actionDelay) {
        state = IDLE;
      }
      break;

  }
}
