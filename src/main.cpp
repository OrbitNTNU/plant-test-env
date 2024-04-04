/* 
#include <Arduino.h>


const int temperaturePin = 0;

void setup()
{
  Serial.begin(9600);
}
float getVoltage(int pin);

void loop()
{
  float voltage, degreesC, degreesF;
  voltage = getVoltage(temperaturePin);

  degreesC = (voltage - 0.5) * 100.0;
  degreesF = degreesC * (9.0/5.0) + 32.0;

  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("  deg C: ");
  Serial.print(degreesC);
  Serial.print("  deg F: ");
  Serial.println(degreesF);

  delay(1000); 
}


float getVoltage(int pin)
{
  return (analogRead(pin) * 0.004882814);
}
*/

///
#include <Arduino.h>
#include <httplib.h>
#include <iostream>
#include <libserialport.h>
#include <string>
#include <thread>

const int temperaturePin = 0; // Make sure this pin number is correct for your setup

// Global variable to hold temperature data
float temperatureCelsius = 0.0;

float getVoltage(int pin) {
    return (analogRead(pin) * 0.004882814);
}

void setup() {
    Serial.begin(9600);
}

void readFromSerial() {
     if (Serial.available() > 0) {
        // Read the incoming line from the serial port
        String line = Serial.readStringUntil('\n');

        // Check if the line starts with the expected prefix
        String prefix = "Temp:";
        if (line.startsWith(prefix)) {
            // Remove the prefix from the line to isolate the temperature value
            String tempString = line.substring(prefix.length());

            // Convert the temperature string to a float value
            float temperature = tempString.toFloat();

            // Update the global temperature variable
            temperatureCelsius = temperature;

            // Print the temperature to the serial monitor for debugging
            Serial.print("Temperature read from serial: ");
            Serial.println(temperatureCelsius);
        }
    }
}

void loop() {
    // Read temperature from the sensor
    float voltage = getVoltage(temperaturePin);
    temperatureCelsius = (voltage - 0.5) * 100.0;

    // Print to Serial (will be removed or commented out later)
    Serial.print("voltage: ");
    Serial.print(voltage);
    Serial.print(" - deg C: ");
    Serial.println(temperatureCelsius);

    delay(1000);
}

int main() {
    // Set up the Arduino (this calls the setup function)
    init();

    // Launch the Arduino loop() in a separate thread
    std::thread arduinoThread([]{
        while (true) {
            loop();
        }
    });
    
    // Launch the serial reading in a separate thread
    std::thread serialThread(readFromSerial);
    
    // Detach threads
    arduinoThread.detach();
    serialThread.detach();

    using namespace httplib;
    
    // Create a server and set an endpoint for Prometheus to scrape
    Server svr;
    svr.Get("/metrics", [](const Request&, Response& res) {
        res.set_content(std::to_string(temperatureCelsius), "text/plain");
    });

    // Listen on all interfaces on port 8000
    svr.listen("0.0.0.0", 8000);
    
    return 0;
}


