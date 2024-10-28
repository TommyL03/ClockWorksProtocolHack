#include <SD.h>

// Pin Declarations for Device 1
int dataIn1 = 2; //G35
int clockIn1 = 3; //G32

// Pin Declarations for Device 2
int dataIn2 = 4; //G23
int clockIn2 = 5; //G25

// Pin Declarations for Device 3
int dataIn3 = 6;
int clockIn3 = 7;

// Variables for Device 1
int clock1 = 1;
int lastClock1 = 1;

long data1 = 0;
unsigned long newSample1 = 0;
unsigned long oldSample1 = 0;
int bit1 = 0;
int bits1 = 0;
long latest1 = 0;

// Variables for Device 2
int clock2 = 1;
int lastClock2 = 1;
long data2 = 0;
unsigned long newSample2 = 0;
unsigned long oldSample2 = 0;
int bit2 = 0;
int bits2 = 0;
long latest2 = 0;

// Variables for Device 3
int clock3 = 1;
int lastClock3 = 1;
long data3 = 0;
unsigned long newSample3 = 0;
unsigned long oldSample3 = 0;
int bit3 = 0;
int bits3 = 0;
long latest3 = 0;

unsigned long lastWriteTime = 0;
float voltage = 0.0;

// Control Pin Variables
int controlPin = A0;        // Use analog pin A0 for DAC output
int dacValue_1_5V = 465;    // Calculated value for 1.5V output

void setup()
{
  // Pin Set Up for Device 1
  pinMode(dataIn1, INPUT);
  pinMode(clockIn1, INPUT);

  // Pin Set Up for Device 2
  pinMode(dataIn2, INPUT);
  pinMode(clockIn2, INPUT);

  // Pin Set Up for Device 3
  pinMode(dataIn3, INPUT);
  pinMode(clockIn3, INPUT);

  // Set up control pin for DAC output
  analogWriteResolution(10); // 10-bit DAC
  analogWrite(controlPin, 0); // Initialize at 0V

  Serial.begin(115200);

  // SD card initialization
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    while (1); // halt execution
  }

  // Create a new file
  SD.remove("datalog.csv"); // Remove existing file if any
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time(ms),Device1,Device2,Device3,Voltage"); // Write CSV header
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.csv");
    while (1); // halt execution
  }
}

void loop()
{
  // Processing Device 1
  lastClock1 = clock1;
  clock1 = digitalRead(clockIn1);

  if (lastClock1 == 0 && clock1 == 1)
  {
    oldSample1 = newSample1;
    newSample1 = micros();

    if ((newSample1 - oldSample1) > 40000)
    {
      latest1 = data1;
      //Serial.print("Device 1: ");
      //Serial.print(latest1);
      //Serial.println(",");
      data1 = 0;
      bits1 = 0;
    }
    bits1++;
    data1 += (digitalRead(dataIn1) && digitalRead(dataIn1)) << (bits1 - 1);
  }

  // Processing Device 2
  lastClock2 = clock2;
  clock2 = digitalRead(clockIn2);

  if (lastClock2 == 0 && clock2 == 1)
  {
    oldSample2 = newSample2;
    newSample2 = micros();

    if ((newSample2 - oldSample2) > 40000)
    {
      latest2 = data2;
      //Serial.print("Device 2: ");
      //Serial.print(latest2);
      //Serial.println(",");
      data2 = 0;
      bits2 = 0;
    }
    bits2++;
    data2 += (digitalRead(dataIn2) && digitalRead(dataIn2)) << (bits2 - 1);
  }

  // Processing Device 3
  lastClock3 = clock3;
  clock3 = digitalRead(clockIn3);

  if (lastClock3 == 0 && clock3 == 1)
  {
    oldSample3 = newSample3;
    newSample3 = micros();

    if ((newSample3 - oldSample3) > 40000)
    {
      latest3 = data3;
      //Serial.print("Device 3: ");
      //Serial.print(latest3);
      //Serial.println(",");

      data3 = 0;
      bits3 = 0;
    }
    bits3++;
    data3 += (digitalRead(dataIn3) && digitalRead(dataIn3)) << (bits3 - 1);
  }

  // Read voltage from pin A1
  int sensorValue = analogRead(A1);   // Read the input on analog pin A1
  voltage = sensorValue * (3.3 / 1023.0);  // Convert the analog reading to voltage (assuming 3.3V reference)

  //Serial.print("Voltage: ");
  //Serial.println(voltage);  // Print the voltage to Serial Monitor

  // Write to SD card once every 20 seconds
  if (millis() - lastWriteTime >= 20000) {  // Changed from 1000 to 20000 milliseconds
    lastWriteTime = millis();

    // Open the file
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      // Write data to file
      dataFile.print(millis());
      dataFile.print(",");
      dataFile.print(latest1);
      dataFile.print(",");
      dataFile.print(latest2);
      dataFile.print(",");
      dataFile.print(latest3);
      dataFile.print(",");
      dataFile.println(voltage);  // Write voltage to SD card

      dataFile.close();
    } else {
      Serial.println("Error opening datalog.csv");
    }

    // Reset all variables to their initial state
    resetVariables();
  }

  // Toggle control pin (A0) to 1.5V, 0V, 1.5V, 0V once every 10 seconds
  static unsigned long lastToggleTime = 0;
  if (millis() - lastToggleTime >= 10000) {
    lastToggleTime = millis();

    // Sequence: 1.5V, 0V, 1.5V, 0V with 1-second delays
    analogWrite(controlPin, dacValue_1_5V); // Set to 1.5V
    delay(1000); // Wait 1 second
    analogWrite(controlPin, 0); // Set to 0V
    delay(1000); // Wait 1 second
    analogWrite(controlPin, dacValue_1_5V); // Set to 1.5V
    delay(1000); // Wait 1 second
    analogWrite(controlPin, 0); // Set to 0V
  }
}

void resetVariables() {
  // Variables for Device 1
  data1 = 0;
  bits1 = 0;
  latest1 = 0;

  // Variables for Device 2
  data2 = 0;
  bits2 = 0;
  latest2 = 0;

  // Variables for Device 3
  data3 = 0;
  bits3 = 0;
  latest3 = 0;
}
