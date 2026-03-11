#include <Arduino.h>

//int myFunction(int, int);

void setup() {

Serial.begin(115200);


}

void loop() {
  Serial.println("Hello, world!");
}

// Built in library: functions
void deubgOutputSTR(String str)
{
    Serial.print("[DEBUG] ");
    Serial.println(str);
}

void deubgOutputINT(String sensor, int num)
{
    Serial.print("[DEBUG ");
    Serial.print(sensor);
    Serial.print(": ");
    Serial.println(num);
}

void infoOutput(String str)
{
    Serial.print("[INFO] ");
    Serial.println(str);
}