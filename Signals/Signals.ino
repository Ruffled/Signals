/*
  Signals

    We are going to attempt to implement running lamps
    and turn signals (and flashers) on single filament
    bulbs using PWM for two level of brightness.
*/

// Here are the PWM outputs
const int left = 9;
const int right = 10;

// Here are the input GPIO
const int left_signal = 2;
const int right_signal = 3;
const int running_lamps = 4;
const int flashers = 5;

// the setup routine runs once when you press reset:
void setup() {
  // declare PWM as outputs
  pinMode(left, OUTPUT);
  pinMode(right, OUTPUT);

  // declare switch states as inputs
  pinMode(left_signal, INPUT);
  pinMode(right_signal, INPUT);
  pinMode(running_lamps, INPUT);
  pinMode(flashers, INPUT);

  // set up our serial for debug
  Serial.begin(115200);
}

// the state of each switch
int running_lamps_active = 0;
int flashers_active = 0;
int left_signal_active = 0;
int right_signal_active = 0;

const int counts_per_blink = 7;
const int delay_per_loop = 100; // milliseconds

// Our lamp levels run 0 through 255
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
  // These static variables only get initialized once
  static int loop_count = 0;
  static int current_left = 0;
  static int current_right = 0;

  int signal_change = 0;
  int state = 0;

  // With no input state changes, we reset the loop_count
  // every counts_per_blink passes, setting our effective
  // signal and flasher blink rate.
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
  // If there was a signal change, force to 0 now.
  // This will force an immediate change instead of delaying
  // for current loop_count to reach its limit.
  if (signal_change) {
    loop_count = 0;   // changes effective immediately
    Serial.write("Signal change detected\n");
  }

  // If loop_count is 0, we need to adjust the
  // current left and right states based on the
  // state of the flasher switch and signal lever.
  if(!loop_count) {
    if (flashers_active) {
      // We simulate flashers using both signals
      // Make sure they match!
      current_left ^= 1;
      current_right = current_left;
    }
    else {
      // No flashers, but check for signals
      if (!left_signal_active) {
        current_left = 0;
      }
      else {
        current_left ^= 1;
      }
        
      if (!right_signal_active) {
        current_right = 0;
      }
      else {
        current_right ^= 1;
      }
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

  // after all of the above, we finally set
  // the actual active level of the bulb
  analogWrite(left, left_level);
  analogWrite(right, right_level);

  loop_count++;

  delay(100);
}

// set vim:sw=2:ts=2:et
