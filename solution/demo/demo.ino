const int light_sensor_pin = A5;
unsigned long start_time; 
unsigned long current_time;
const unsigned long light_sensor_period = 200;
const unsigned long voltage_period = 350;

void print_reading(int value)
{
  Serial.print("Analog reading: ");
  Serial.print(value);   // the raw analog reading

  // We'll have a few threshholds, qualitatively determined
  if (value < 10) {
    Serial.println(" - Dark");
  } else if (value < 200) {
    Serial.println(" - Dim");
  } else if (value < 500) {
    Serial.println(" - Light");
  } else if (value < 800) {
    Serial.println(" - Bright");
  } else {
    Serial.println(" - Very bright");
  }
}

void setup() {
  Serial.begin(9600);
  start_time = millis();
}

void loop() {
    current_time = millis();
  if (current_time - start_time >= light_sensor_period)  //test whether the period has elapsed
  {
    int analog_value = analogRead(light_sensor_pin); //value between 0 and 1023
    print_reading(analog_value);
    start_time = current_time;
  }
}
