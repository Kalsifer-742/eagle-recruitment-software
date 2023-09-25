const int light_sensor_pin = A5;
const int voltage_pin = A0;
const int undervoltage_led_pin = D2;
const int overvoltage_led_pin = D4;
const int emergency_button = PC13;
bool waiting = false;

HardwareTimer *light_sensor_timer = new HardwareTimer(TIM6);
HardwareTimer *voltage_check_timer = new HardwareTimer(TIM7);

enum States {
  danger_state,
  waiting_state,
  running_state
};

//volatile ?
void wait_state(void)
{
  waiting = !waiting;
}

void read_light_sensor(void)
{
  int analog_value = analogRead(light_sensor_pin);
  int voltage = analog_value * (5.0 / 1023.0);
  int lux = exp(-0.5852985121078413 * voltage + 5.546777762224811);
  Serial.print("Brightness: ");
  Serial.print(lux);
  Serial.println(" lx"); //lux
}

void read_voltage(void)
{
  int sensor_value = analogRead(voltage_pin);
  float voltage = sensor_value * (5.0 / 1023.0);
  if (voltage < 1.8) 
  {
    digitalWrite(undervoltage_led_pin, HIGH);
  }
  else if (voltage > 2.7) 
  {
    digitalWrite(overvoltage_led_pin, HIGH);
  }
  else 
  {
    digitalWrite(undervoltage_led_pin, LOW);
    digitalWrite(overvoltage_led_pin, LOW);
  }
  Serial.print("Voltage: ");
  Serial.println(voltage);
}

void setup() 
{
  Serial.begin(9600);
  pinMode(undervoltage_led_pin, OUTPUT);
  pinMode(overvoltage_led_pin, OUTPUT);
  pinMode(emergency_button, INPUT);
  attachInterrupt(digitalPinToInterrupt(emergency_button), wait_state, HIGH);

  light_sensor_timer->setOverflow(1000000, MICROSEC_FORMAT); 
  light_sensor_timer->attachInterrupt(read_light_sensor);
  voltage_check_timer->setOverflow(500000, MICROSEC_FORMAT);
  voltage_check_timer->attachInterrupt(read_voltage);

  light_sensor_timer->resume();
  voltage_check_timer->resume();
}

void loop() 
{
  if (waiting) {
    light_sensor_timer->pause();
    voltage_check_timer->pause();
    while (true) {
      if (!waiting) {
        break;
      }
      Serial.println("Board in waiting state - please press the emergency button");
      delay(500);
    }
    light_sensor_timer->resume();
    voltage_check_timer->resume();
  }
}