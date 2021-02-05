#include <LiquidCrystal.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 
IPAddress ip(192, 168, 178, 141);    
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int stufe;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Starte...");
  Serial.begin(9600);      // initialize serial communication
  //pinMode(9, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  lcd.clear();
  lcd.print("Kein Status");
}


void loop() {
  delay(300);
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Status: <a href=\"/E\">SEHR WICHTIG</a> (Stufe 3)<br>");
            client.print("Status: <a href=\"/H\">WICHTIG</a> (Stufe 2)<br>");
            client.print("Status: <a href=\"/L\">ANKLOPFEN</a> (Stufe 1)<br>");
            client.print("Status: <a href=\"/N\">Nichts</a> (Stufe 0)<br>");
            client.print("Aktuelle Stufe: " + String(stufe));

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
            stufe = 2;
            lcd.clear();
            lcd.print("Stufe 2");
            lcd.setCursor(0,1);
            lcd.print("Wichtig!");
            delay(500);
        }
        if (currentLine.endsWith("GET /L")) {
            stufe = 1;
            lcd.clear();
            lcd.print("Stufe 1");
            lcd.setCursor(0,1);
            lcd.print("Anklopfen");
            delay(500);
        }
        if (currentLine.endsWith("GET /E")) {
            stufe = 3;
            lcd.clear();
            lcd.print("Stufe 3");
            lcd.setCursor(0,1);
            lcd.print("!!!WICHTIG!!!");
            delay(500);
        }
        if (currentLine.endsWith("GET /N")) {
            stufe = 0;
            lcd.clear();
            lcd.print("Stufe 0");
            lcd.setCursor(0,1);
            lcd.print("Nichts");
            delay(500);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  Serial.println("Stufe: " + String(stufe));
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
