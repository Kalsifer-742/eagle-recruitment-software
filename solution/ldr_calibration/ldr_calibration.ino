const int light_sensor_pin = A5;
const unsigned long light_sensor_period = 1000;

void read_light_sensor(void)
{
  int analog_value = analogRead(light_sensor_pin);
  
  float voltage = analog_value * (5.0 / 1023.0);

  Serial.print("Brightness: ");
  Serial.print(analog_value);
  Serial.print(" - ");
  Serial.println(voltage);
}

void setup() 
{
  Serial.begin(9600);

  HardwareTimer *ligh_sensor_timer = new HardwareTimer(TIM6);
  ligh_sensor_timer->setOverflow(1000000, MICROSEC_FORMAT); 
  ligh_sensor_timer->attachInterrupt(read_light_sensor);
  ligh_sensor_timer->resume();
}

void loop() 
{

}