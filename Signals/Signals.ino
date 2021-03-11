/*
  Signals

    We are going to attempt to implement running lamps
    and turn signals (and flashers) on single filament
    bulbs using PWM for two level of brightness.
*/

const int left = 9;           // the PWM pin for the left side lamps
const int right = 10;         // the PWM pin for the right side lamps

const int left_signal = 2;
const int right_signal = 3;
const int running_lamps = 4;
const int flashers = 5;

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  pinMode(left, OUTPUT);
  pinMode(right, OUTPUT);

  pinMode(left_signal, INPUT);
  pinMode(right_signal, INPUT);
  pinMode(running_lamps, INPUT);
  pinMode(flashers, INPUT);

  Serial.begin(115200);
}

int running_lamps_active = 0;
int flashers_active = 0;
int left_signal_active = 0;
int right_signal_active = 0;

const int counts_per_blink = 5;
const int delay_per_loop = 100; // milliseconds

int loop_count = 0;
int current_left = 0;
int current_right = 0;
int signal_change = 0;
int state = 0;

const int blink_level = 255;
const int running_level = 128;
int left_level;
int right_level;

void show_level(void)
{
  char * left_msg = " off ";
  char * right_msg = "off\n";

  if (left_level == blink_level) {
    left_msg = " blink ";
  }
  else if (left_level == running_level) {
    left_msg = " dim ";
  }


  if (right_level == blink_level) {
    right_msg = "blink\n";
  }
  else if (right_level == running_level) {
    right_msg = "dim\n";
  }

  Serial.print(running_lamps_active, DEC);
  Serial.print(flashers_active, DEC);
  Serial.print(left_signal_active, DEC);
  Serial.print(right_signal_active, DEC);
  Serial.write(left_msg);
  Serial.write(right_msg);

}

// the loop routine runs over and over again forever:
void loop() {
  signal_change = 0;

  if (loop_count >= counts_per_blink) {
    loop_count = 0;
  }

  // Get each of the input switch states, and annotate
  // any changes in state for flashers, left, or right.
  // We don't track changed states on running lamps
  running_lamps_active = digitalRead(running_lamps);

  state = digitalRead(flashers);
  if (flashers_active != state) {
    signal_change = 1;
  }
  flashers_active = state;

  // Only bother with the actual turn signals if we are
  // not running the flashers. They share filaments.
  if (!flashers_active) {
    state = digitalRead(left_signal);
    if (left_signal_active != state) {
      signal_change = 1;
    }
    left_signal_active = state;

    state = digitalRead(right_signal);
    if (right_signal_active != state) {
      signal_change = 1;
    }
    right_signal_active = state;
  }

  // Knock over any existing turn signal states if the
  // flashers are active.
  if (flashers_active) {
    left_signal_active = 0;
    right_signal_active = 0;
  }

  // If our switches are set up properly, this will never
  // happen.
  if (left_signal_active && right_signal_active) {
    Serial.write("Error, both signals active\n");
  }

  // We update actual lamp status only on loop_count == 0.
  // If there was a signal change, note that here.
  if (signal_change) {
    loop_count = 0;   // changes effective immediately
    Serial.write("Signal change detected\n");
  }

  if (flashers_active) {
    // We simulate flashers using both signals
    // Make sure they match!
    if (!loop_count) {
      current_left ^= 1;
      current_right = current_left;
    }
  }
  else {
    if (!left_signal_active) {
      current_left = 0;
    }
    else if (!loop_count) {
      current_left ^= 1;
    }
      
    if (!right_signal_active) {
      current_right = 0;
    }
    else if (!loop_count) {
      current_right ^= 1;
    }
  }

  // Default the lamps to running_lamps
  // If signals are active, it will be
  // overridden below.
  if(running_lamps_active) {
    left_level = running_level;
    right_level = running_level;
  }
  else {
    left_level = 0;
    right_level = 0;
  }

  if (left_signal_active || flashers_active)
  {
    if (current_left) {
      left_level = blink_level;
    }
    else {
      left_level = 0;
    }
  }

  if (right_signal_active || flashers_active)
  {
    if (current_right) {
      right_level = blink_level;
    }
    else {
      right_level = 0;
    }
  }

  if (!loop_count) {
    show_level();
  }

  analogWrite(left, left_level);
  analogWrite(right, right_level);

  loop_count++;

  delay(100);
}

// set vim:sw=2:ts=2:et
