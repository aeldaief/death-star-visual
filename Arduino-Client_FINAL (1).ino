/*
 * FINAL CODE FOR SENSOR
 * -- Includes both ultrasonic sensor reading
 * -- Determines whether it should initiate sequence
 * -- Send request to server (Alex E's RPi) which inititaes actuator
 * 
 * Author: Jake Rocco
 * Date Created: 1 December 2020
 * 
 */


#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "DTD 2";        // your network SSID (name)
char pass[] = "Tuftsd15";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiClient client; // intialize wifi client object
IPAddress server(192,168,0,64); // IP address of Alex E's pi

// define pins for echo and trig pins on sensor
const int trigPin = 7;
const int echoPin = 6; 
// defining threshold distance to then send request
const int threshold = 10; 

// initialize variables
long duration; 
int distance; 


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 

  // initalize pins... trigPin as OUTPUT and echoPin as INPUT
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // check for the wifi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);   // print the network name (SSID)
    // connect to WPA?WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection
    delay(10000);
  }

  Serial.println("Connected to wifi");
  printWifiStatus();
  Serial.println("\nStarting connection to server...");
}

void loop() {
  // clears trigPin
  digitalWrite(trigPin, LOW);
  delay(2);
  // sets trigPin on HIGH for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delay(10); 
  digitalWrite(trigPin, LOW); // turn off trigpin
  // read echoPin, which returns how long it takes sound wave to travel there and back
  duration = pulseIn(echoPin, HIGH);
  Serial.print("Duration (microseconds): ");
  Serial.println(duration);
  // calculate distance from duration variable
  // multiple by .034 to convert m/s to cm/us -> (microseconds)
  distance = duration * 0.034 / 2; 
  Serial.print("Distance (cm): "); 
  Serial.println(distance); 

  delay(500); 

  if (distance <= threshold) {
    if (client.connect(server, 5000)) {
      Serial.println("on"); 

      // make a http request
      client.println("GET /pinon HTTP/1.1"); 
      client.println("Host: 192.168.0.64");
      client.println("Connection: close");
      client.println();

      delay(30000);
    }
  } else {
    if (client.connect(server, 5000)) {
      Serial.println("off"); 

      // make a http request
      client.println("GET /pinoff HTTP/1.1"); 
      client.println("Host: 192.168.0.64");
      client.println("Connection: close");
      client.println();   

      delay(1000); 
    }
  } 
}



void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println("dBm");
}
