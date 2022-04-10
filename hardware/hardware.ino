#include <SoftwareSerial.h>

#define DEBUG true

SoftwareSerial esp8266(3, 2);

const int longRelay = 10;
const int shortRelay = 11;
const int LedIndicator = 9;

bool Heated = false;
bool found = false;

String SSID = "LightCore 2.5Ghz";
String PASSWORD = "avgust14martin";
String host = "192.168.88.241";
String path = "/esp_test/esp_test";
String port = "80";

int Count = 0;
int CountTime = 0;

//StaticJsonBuffer<200> jsonBuffer;
//JsonObject &root = jsonBuffer.createObject();

void setup()
{
  Serial.begin(115200);
  esp8266.begin(115200);
  sendCommand("AT+RST\r\n", 5, "OK");
  sendCommand("AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"\r\n", 20, "WIFI CONNECTED");
  sendCommand("AT+CWMODE=3\r\n", 5, "OK");
  sendCommand("AT+CIPMUX=1\r\n", 5, "OK");
  sendCommand("AT+CIPSERVER=1,80\r\n", 20, "OK");

  pinMode(longRelay, OUTPUT);
  pinMode(shortRelay, OUTPUT);
  pinMode(LedIndicator, OUTPUT);
  digitalWrite(longRelay, HIGH);
  digitalWrite(shortRelay, HIGH);
  digitalWrite(LedIndicator, HIGH);
  delay(1000);
  digitalWrite(longRelay, LOW);
  digitalWrite(shortRelay, LOW);
  digitalWrite(LedIndicator, LOW);
}

void loop()
{
  if (esp8266.available())
  {
    String res = "" +
                 String("HTTP/1.1 200 OK\r\n") +
                 "Content-Type: none\r\n" +
                 "Content-Length: 0\r\n" +
                 "Connection: close\r\n\r\n";

    if (esp8266.find("+IPD,"))
    {
      delay(1000);
      int connectionId = esp8266.read() - 48;
      esp8266.find("command=");
      int command = (esp8266.read());
      delay(500);
      Serial.println("COMMAND: " + command);
      Serial.println("CONNID: " + connectionId);
      if (command == '1')
      {
        // digitalWrite(RELAY_PIN, LOW);
        // delay(1500);
        // digitalWrite(RELAY_PIN, HIGH);
        // String cipSend = "AT+CIPSEND=" + String(connectionId) + "," + String(res.length());
        // sendCommandToesp(cipSend, 4, ">");
        // sendData(res);
        // String closeCommand = "AT+CIPCLOSE=";
        // closeCommand += connectionId;
        // closeCommand += "\r\n";
        // sendCommandToesp(closeCommand, 5, "OK");

        command = (esp8266.read());
        if (command == '1')
        {
          digitalWrite(longRelay, HIGH);
          digitalWrite(LedIndicator, HIGH);
          delay(1500);
          digitalWrite(longRelay, LOW);
          digitalWrite(LedIndicator, LOW);
          String cipSend = "AT+CIPSEND=" + String(connectionId) + "," + String(res.length()) + "\r\n";
          sendCommand(cipSend, 4, ">");
          sendData(res);
          String closeCommand = "AT+CIPCLOSE=";
          closeCommand += connectionId;
          closeCommand += "\r\n";
          sendCommand(closeCommand, 5, "OK");
          Heated = true;
        }
        else if (command == '2')
        {
          digitalWrite(shortRelay, HIGH);
          digitalWrite(LedIndicator, HIGH);
          delay(1500);
          digitalWrite(shortRelay, LOW);
          digitalWrite(LedIndicator, LOW);
          String cipSend = "AT+CIPSEND=" + String(connectionId) + "," + String(res.length()) + "\r\n";
          sendCommand(cipSend, 4, ">");
          sendData(res);
          String closeCommand = "AT+CIPCLOSE=";
          closeCommand += connectionId;
          closeCommand += "\r\n";
          sendCommand(closeCommand, 5, "OK");
          Heated = true;
        }
      }
    }
  }
}
void sendCommand(String command, int delay, char reply[])
{
  Serial.print(Count);
  Serial.print(" at command => ");
  Serial.println(command);
  while (CountTime < delay)
  {
    esp8266.print(command);
    if (esp8266.find(reply))
    {
      found = true;
      break;
    }
    CountTime++;
  }

  if (found)
  {
    Serial.println("Command sent");
    Count++;
    CountTime = 0;
  }
  else
  {
    Serial.println("Command not sent");
    Count = 0;
    CountTime = 0;
    
  }

  found = false;
}

void sendData(String Request)
{
  Serial.println(Request);
  esp8266.println(Request);
  delay(1500);
  Count++;
}
