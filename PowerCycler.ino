/*

  This example connects to a encrypted WiFi network (WPA/WPA2).

  Then it prints the  MAC address of the board,

  the IP address obtained, and other network details.

  Then it continuously pings given host specified by IP Address or name.

  Circuit:

  * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and UNO WiFi Rev.2)

  created 13 July 2010

  by dlf (Metodo2 srl)

  modified 09 June 2016

  by Petar Georgiev

*/
#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;          // your network SSID (name)
char pass[] = SECRET_PASS;          // your network password (use for WPA, or use as key for WEP)
int pingInterval = 5000;//30000;           // interval between pings in ms
int bootDuration = 10000;//300000;          // time for the modem to boot up in ms
int powerOffDuration = 10000;//60000;       // duration of power off in ms
int failureThreshold = 4;           // number of failed pings before power cycling the modem
int modemRelayOut = 8;              // modem power relay module output pin #

int status = WL_IDLE_STATUS;        // the WiFi radio's status
int failedPings = 0;

// Specify IP address or hostname

String hostName = "www.google.com";
int pingResult;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(modemRelayOut, OUTPUT);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");

    // don't continue
    while (true);
  }
  
  digitalWrite(LEDB, HIGH);

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  digitalWrite(LEDB, LOW);
  digitalWrite(LEDG, HIGH);
  printCurrentNet();
  printWiFiData();
}

void loop() {
  Serial.print("Pinging ");
  Serial.print(hostName);
  Serial.print(": ");

  pingResult = WiFi.ping(hostName);
  if (pingResult >= 0) {
    failedPings = 0;
    Serial.print("SUCCESS! RTT = ");
    Serial.print(pingResult);
    Serial.println(" ms");
  } else {
    failedPings++;
    Serial.print("FAILED ");
    Serial.print(failedPings);
    Serial.print("! Error code: ");
    Serial.println(pingResult);

    if (failedPings == failureThreshold) {
      failedPings = 0;

      Serial.println("Power off!");
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDR, HIGH);
      digitalWrite(modemRelayOut, HIGH);
      delay(powerOffDuration);

      Serial.println("Power on! Booting...");
      digitalWrite(LEDR, LOW);
      digitalWrite(modemRelayOut, LOW);

      delay(bootDuration);
      digitalWrite(LEDG, HIGH);
    }
  }

  delay(pingInterval);
}

void printWiFiData() {

  // print your board's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP address : ");

  Serial.println(ip);

  Serial.print("Subnet mask: ");

  Serial.println((IPAddress)WiFi.subnetMask());

  Serial.print("Gateway IP : ");

  Serial.println((IPAddress)WiFi.gatewayIP());

  // print your MAC address:

  byte mac[6];

  WiFi.macAddress(mac);

  Serial.print("MAC address: ");

  printMacAddress(mac);
}

void printCurrentNet() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:

  byte bssid[6];

  WiFi.BSSID(bssid);

  Serial.print("BSSID: ");

  printMacAddress(bssid);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI): ");

  Serial.println(rssi);

  // print the encryption type:

  byte encryption = WiFi.encryptionType();

  Serial.print("Encryption Type: ");

  Serial.println(encryption, HEX);

  Serial.println();
}

void printMacAddress(byte mac[]) {

  for (int i = 5; i >= 0; i--) {

    if (mac[i] < 16) {

      Serial.print("0");

    }

    Serial.print(mac[i], HEX);

    if (i > 0) {

      Serial.print(":");

    }

  }

  Serial.println();
}
