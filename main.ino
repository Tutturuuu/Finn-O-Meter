#include <ArduinoHttpClient.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 
IPAddress ip(192, 168, 178, 141);    
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int BUTTON_PIN = 7; // Arduino pin connected to button's pin
const int LED_PIN    = 9; // Arduino pin connected to LED's pin
const char serverName[] = "api.pushover.net";  // server name
int port = 80;
const String user = "um6nsiyaqft7uwq4xewcpbyznrn8cz";
const String token = "anviwaox7yr3kinzgae57xnf9euidh";
int ledState = LOW;     // the current state of LED
int lastButtonState;    // the previous state of button
int currentButtonState; // the current state of button
int stufe;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverName, port);
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(LED_PIN, OUTPUT);          // set arduino pin to output mode
  lcd.begin(16, 2);
  Serial.begin(9600);
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
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
    lcd.print("Finn-O-Meter");
    lcd.setCursor(0,1);
    lcd.print(ip);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(7000);
  lcd.clear();
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  lcd.print("Kein Status");
  currentButtonState = digitalRead(BUTTON_PIN);
}


void loop() {
  ledState = HIGH;
  digitalWrite(LED_PIN, ledState);
  lastButtonState    = currentButtonState;      // save the last state
  currentButtonState = digitalRead(BUTTON_PIN); // read new state

  if(lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("The button is pressed");
    pushMsg(token, user, "Bitte Kommen!");
    // toggle state of LED
    ledState = HIGH;
    digitalWrite(LED_PIN, ledState);
    delay(500);
    ledState = LOW;
    // control LED arccoding to the toggled state
    digitalWrite(LED_PIN, ledState); 
  }
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
            client.print("<a href=\"/E\">Bitte Leise!!!!</a> (Stufe 3/3)<br>");
            client.print("<a href=\"/H\">In Meeting</a>      (Stufe 2/3)<br>");
            client.print("<a href=\"/L\">Beschaeftigt</a>    (Stufe 1/3)<br>");
            client.print("<a href=\"/N\">Abrufbar</a>        (Stufe 0/3)<br>");
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
            lcd.print("In Meeting");
            delay(500);
        }
        if (currentLine.endsWith("GET /L")) {
            stufe = 1;
            lcd.clear();
            lcd.print("Stufe 1");
            lcd.setCursor(0,1);
            lcd.print("Beschaeftigt");
            delay(500);
        }
        if (currentLine.endsWith("GET /E")) {
            stufe = 3;
            lcd.clear();
            lcd.print("Stufe 3");
            lcd.setCursor(0,1);
            lcd.print("Bitte Leise!!!");
            delay(500);
        }
        if (currentLine.endsWith("GET /N")) {
            stufe = 0;
            lcd.clear();
            lcd.print("Stufe 0");
            lcd.setCursor(0,1);
            lcd.print("Abrufbar");
            delay(500);
        }
        if (currentLine.endsWith("GET /A")) {
            stufe = 99;
            lcd.clear();
            lcd.print("Finn-O-Meter");
            lcd.setCursor(0,1);
            lcd.print("Ich komm' schon!");
            delay(500);
        }
        if (currentLine.endsWith("GET /S")) {
            stufe = 99;
            lcd.clear();
            lcd.print("Finn-O-Meter");
            lcd.setCursor(0,1);
            lcd.print("Im Voicechat");
            delay(500);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  Serial.println("Stufe: " + String(stufe));
  ledState = HIGH;
  digitalWrite(LED_PIN, ledState);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

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
void pushMsg(String token, String user, String message) {
  String httpRequestData = "token=" + token + "&user=" + user + "&message=" + message + "&priority=2&retry=60&expire=3600&callback=https://schule.finnlab.de/E";
  Serial.println("making POST request");
  String contentType = "application/x-www-form-urlencoded";
  String postData = httpRequestData;

  client.post("/1/messages.json", contentType, postData);


  int statusCode = client.responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  String response = client.responseBody();
  Serial.print("Response: ");
  Serial.println(response);

  //Serial.println("Wait 1 second");
  delay(1000);
  }
