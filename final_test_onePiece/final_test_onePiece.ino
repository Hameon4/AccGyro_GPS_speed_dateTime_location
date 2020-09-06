#include <ThingerWiFiNINA.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Arduino_LSM6DS3.h>

#define _DISABLE_TLS_
#define USERNAME "hamalsheraifi4"
#define DEVICE_ID "arduinoRev2"
#define DEVICE_CREDENTIAL "first_credential"

#define SSID "SSID_NAME"
#define SSID_PASSWORD "SSID_PASSWORD"

ThingerWiFiNINA thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

TinyGPS gps;
SoftwareSerial ss(4, 3);

float xA, yA, zA;
float xG, yG, zG;

void setup() {
  // configure wifi network
  thing.add_wifi(SSID, SSID_PASSWORD);

  Serial.begin(9600);
  ss.begin(9600);

  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  Serial.println("Acceleration in G's\t\tGyroscope in degrees/second");
  Serial.println("X\tY\tZ\t\tX\tY\tZ");

  thing["Acc_Gyro_Info"] >> [](pson &out) {
    out["xA"] = xA;
    out["yA"] = yA;
    out["zA"] = zA;
    out["xG"] = xG;
    out["yG"] = yG;
    out["zG"] = zG;
  };
  
  thing["deviceInfo"] >> [](pson & out) {
    out["lat"] = Latitude();
    out["lon"] = Longitude();
    out["date"] = Date();
    out["spd"] = speed();
  };
}

void loop() {
  thing.handle();

  //Run the loop only when thier is valid data
  if (Latitude() != 1000 && Longitude() != 1000) {
    Serial.println("The location");
    Serial.print(Latitude());
    Serial.print(" ,,,,,,, ");
    Serial.println(Longitude());
    Serial.println("The date and time");
    Serial.println(Date());
    Serial.println("The speed");
    Serial.println(speed());

  }
  else
    Serial.println("No Signal");

  smartdelay(2000);
}

//Function for getting the accelerometer and gyroscope values
static float AccandGyro(){
    IMU.readAcceleration(xA, yA, zA); 
    IMU.readGyroscope(xG, yG, zG);
    return xA, yA, zA, xG, yG, zG;
}

//Delay function
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    //check is signal is available
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

//Optional (extra spaces)
static void spaces(int n) {
  //create spaces
  for (int i = 0; i < 5; i++)
    Serial.print(" ");
}

//Latitude Function
static float Latitude() {
  //print the latitude
  float flat, flon;
  gps.f_get_position(&flat, &flon);
  return flat;
}

//Longitude Function
static float Longitude() {
  //print the Longitude
  float flat, flon;
  gps.f_get_position(&flat, &flon);
  return flon;
}

//Function for getting the date and time
static String Date() {
  //return string of date and time
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  byte convHour = hour + 4;
  if (convHour >= 24)
    convHour = convHour - 24;
  return "Date: " + String(day) + "/" + String(month) + "/" + String(year) + " Time: " + String(convHour) + ":" + String(minute) + ":" + String(second);
}

//Function for getting the speed
static float speed() {
  float speed = 0;
  speed = gps.f_speed_kmph();
  if (speed <= 0)
    return 0;
  else
    return speed;
}
