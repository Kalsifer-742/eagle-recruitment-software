const int light_sensor_pin = A0;
const int voltage_pin = A5;
const int undervoltage_led_pin = D2;
const int overvoltage_led_pin = D4;
const int emergency_button = PC13;
HardwareTimer *light_sensor_check_timer = new HardwareTimer(TIM6);
HardwareTimer *voltage_check_timer = new HardwareTimer(TIM7);

typedef enum {
  RUNNING_STATE,
  DANGER_STATE,
  WAITING_STATE
} State;

struct current_state_t
{
  State state;
  void (*function)(void);
} current_state;

typedef enum {
  VOLTAGE_IN_RANGE,
  UNDERVOLTAGE,
  OVERVOLTAGE,
  EMERGENCY_BUTTON_PRESSED
} Event;

typedef enum {
  POWER_OFF_LEDS,
  TURN_ON_UNDERVOLTAGE_LED,
  TURN_ON_OVERVOLTAGE_LED,
  PAUSE_TIMERS,
  RESUME_TIMERS,
  NONE
} Action;

typedef struct {
    State next_state;
    Action action;
} Transition;

// transition_table[current_state][event]
Transition transition_table[3][4] = 
{
  //RUNNING_STATE
  {{RUNNING_STATE, NONE}, {DANGER_STATE, TURN_ON_UNDERVOLTAGE_LED}, {DANGER_STATE, TURN_ON_OVERVOLTAGE_LED}, {WAITING_STATE, PAUSE_TIMERS}},
  //DANGER_STATE
  {{RUNNING_STATE, POWER_OFF_LEDS}, {DANGER_STATE, TURN_ON_UNDERVOLTAGE_LED}, {DANGER_STATE, TURN_ON_OVERVOLTAGE_LED}, {WAITING_STATE, PAUSE_TIMERS}},
  //WAITING_STATE
  {{WAITING_STATE, NONE}, {WAITING_STATE, NONE}, {WAITING_STATE, NONE}, {RUNNING_STATE, RESUME_TIMERS}}
};

void handle_danger_to_running_state_transition(void)
{
  digitalWrite(undervoltage_led_pin, LOW);
  digitalWrite(overvoltage_led_pin, LOW);
  Serial.println("leds_status: undervoltage_led is OFF");
  Serial.println("leds_status: overvoltage_led is OFF");
  return;
}

void handle_undervoltage_led(void)
{
  digitalWrite(overvoltage_led_pin, LOW);
  Serial.println("leds_status: overvoltage_led is OFF");
  digitalWrite(undervoltage_led_pin, HIGH);
  Serial.println("leds_status: undervoltage_led is ON");
  return;
}

void handle_overvoltage_led(void)
{
  digitalWrite(undervoltage_led_pin, LOW);
  Serial.println("leds_status: undervoltage_led is OFF");
  digitalWrite(overvoltage_led_pin, HIGH);
  Serial.println("leds_status: overvoltage_led is ON");
  return;
}

void handle_to_waiting_state_transition(void)
{
  light_sensor_check_timer->pause();
  voltage_check_timer->pause();
  return;
}

void handle_waiting_to_running_state_transition()
{
  light_sensor_check_timer->resume();
  voltage_check_timer->resume();
  return;
}

void transitions_handler(Event event)
{
  Transition transition = transition_table[current_state.state][event];
  //handle also this with a function pointer ?
  switch(transition.action)
  {
    case POWER_OFF_LEDS:
      handle_danger_to_running_state_transition();
      break;
    case TURN_ON_UNDERVOLTAGE_LED:
      handle_undervoltage_led();
      break;
    case TURN_ON_OVERVOLTAGE_LED:
      handle_overvoltage_led();
      break;
    case PAUSE_TIMERS:
      handle_to_waiting_state_transition();
      break;
    case RESUME_TIMERS:
      handle_waiting_to_running_state_transition();
      break;
    case NONE:
      break;
  }

  current_state.state = transition.next_state;
  switch(current_state.state)
  {
    case RUNNING_STATE:
      current_state.function = running_state;
      break;
    case DANGER_STATE:
      current_state.function = danger_state;
      break;
    case WAITING_STATE:
      current_state.function = waiting_state;
      break;
  }

  return;
}

void running_state(void)
{
  //timers running
}

//if I could create multiple states it would be easy to identify which LED is on
void danger_state(void)
{
  
}

void waiting_state(void)
{
  Serial.println("Board in waiting state - please press the emergency button");
  delay(500);
}

void ms_to_hhmmssms(unsigned long& ms, int& hours, int& minutes, int& seconds)
{
  seconds = ms / 1000;
  ms = ms % 1000;

  minutes = seconds / 60;
  seconds = seconds % 60;

  hours = minutes / 60;
  minutes = minutes % 60;
}

//maybe create a dedicated function for printing
void print_timestamp()
{
  unsigned long ms = millis();
  int h, m, s;
  ms_to_hhmmssms(ms, h, m, s);
  Serial.print(h);
  Serial.print(":");
  Serial.print(m);
  Serial.print(":");
  Serial.print(s);
  Serial.print(":");
  Serial.print(ms);
}

//The only impossible value is below 0 lux but it can't happen with this approach
void light_sensor_timer_interrupt_handler(void)
{
  int analog_value = analogRead(light_sensor_pin);
  float voltage = analog_value * (5.0 / 1023.0);
  //int lux = exp(-0.5852985121078413 * voltage + 5.546777762224811);
  int lux = 123.8957 + (65237400000 - 123.8957) / (1 + pow((voltage / 0.00003897886), 1.778511)); //more accurate in the higher range
  print_timestamp();
  Serial.print(" brightness: ~ ");

  /*int lux = 0;
  if (voltage > 4.30) {
    lux = 50;
    Serial.print("<");
  } else if (voltage > 0.90) {
    lux = 500;
  } else {
    lux = 1200;
    Serial.print(">");
  }
  */
  
  Serial.print(lux);
  Serial.println("Lux");
  //Serial.println(voltage);
}

void voltage_timer_interrupt_handler(void)
{
  int sensor_value = analogRead(voltage_pin);
  float voltage = sensor_value * (5.0 / 1023.0);
  print_timestamp();
  Serial.print(" voltage: ");
  Serial.print(voltage);
  Serial.println("V");
  if (voltage < 1.8)
  {
    //Event e = UNDERVOLTAGE;
    transitions_handler(UNDERVOLTAGE);
  }
  else if (voltage > 2.7) 
  {
    //Event e = OVERVOLTAGE;
    transitions_handler(OVERVOLTAGE);
  }
  else 
  {
    //Event e = VOLTAGE_IN_RANGE;
    transitions_handler(VOLTAGE_IN_RANGE);
  }
}

void button_interrupt_handler(void)
{
  //Event e = EMERGENCY_BUTTON_PRESSED;
  transitions_handler(EMERGENCY_BUTTON_PRESSED);
}

void setup() 
{
  Serial.begin(9600);
  pinMode(undervoltage_led_pin, OUTPUT);
  pinMode(overvoltage_led_pin, OUTPUT);
  pinMode(emergency_button, INPUT);
  attachInterrupt(digitalPinToInterrupt(emergency_button), button_interrupt_handler, HIGH);

  light_sensor_check_timer->setOverflow(1000000, MICROSEC_FORMAT); //200000
  light_sensor_check_timer->attachInterrupt(light_sensor_timer_interrupt_handler);
  voltage_check_timer->setOverflow(1000000, MICROSEC_FORMAT); //350000
  voltage_check_timer->attachInterrupt(voltage_timer_interrupt_handler);
  light_sensor_check_timer->resume();
  voltage_check_timer->resume();

  current_state = {RUNNING_STATE, running_state};
}

void loop() 
{
  current_state.function();
  //Serial.println(current_state.state);
  //delay(500);
}