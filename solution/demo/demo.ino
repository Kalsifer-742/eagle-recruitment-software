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

// Transition table: [currentState][event]
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
  return;
}

void handle_undervoltage_led(void)
{
  digitalWrite(overvoltage_led_pin, LOW);
  digitalWrite(undervoltage_led_pin, HIGH);
  return;
}

void handle_overvoltage_led(void)
{
  digitalWrite(undervoltage_led_pin, LOW);
  digitalWrite(overvoltage_led_pin, HIGH);
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
  //handle this also with a function pointer ?
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

void danger_state(void)
{
  //nothing to do
}

void waiting_state(void)
{
  Serial.println("Board in waiting state - please press the emergency button");
  delay(500);
}

void light_sensor_timer_interrupt_handler(void)
{
  //add timestamps ?
  int analog_value = analogRead(light_sensor_pin);
  int voltage = analog_value * (5.0 / 1023.0);
  int lux = exp(-0.5852985121078413 * voltage + 5.546777762224811);
  Serial.print("Brightness: ");
  Serial.print(lux);
  Serial.println(" [Lux]");
}

void voltage_timer_interrupt_handler(void)
{
  int sensor_value = analogRead(voltage_pin);
  float voltage = sensor_value * (5.0 / 1023.0);
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" [V]");

  if (voltage < 1.8)
  {
    Event e = UNDERVOLTAGE; //can i skip this ?
    transitions_handler(e);
  }
  else if (voltage > 2.7) 
  {
    Event e = OVERVOLTAGE;
    transitions_handler(e);
  }
  else 
  {
    Event e = VOLTAGE_IN_RANGE;
    transitions_handler(e);
  }
}

//volatile ?
void button_interrupt_handler(void)
{
  Event e = EMERGENCY_BUTTON_PRESSED;
  transitions_handler(e);
}

void setup() 
{
  Serial.begin(9600);
  pinMode(undervoltage_led_pin, OUTPUT);
  pinMode(overvoltage_led_pin, OUTPUT);
  pinMode(emergency_button, INPUT);
  attachInterrupt(digitalPinToInterrupt(emergency_button), button_interrupt_handler, HIGH);

  //change timings to correct ones
  light_sensor_check_timer->setOverflow(1000000, MICROSEC_FORMAT); 
  light_sensor_check_timer->attachInterrupt(light_sensor_timer_interrupt_handler);
  voltage_check_timer->setOverflow(500000, MICROSEC_FORMAT);
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