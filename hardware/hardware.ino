#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <Wire.h>

RTC_DS1307 rtc;
SoftwareSerial esp8266(3, 2);

const int longRelay = 10;
const int shortRelay = 11;
const int LedIndicator = 9;

int timerBool = 0;
DateTime timer;
String sCFState = "Cold";

StaticJsonBuffer<200> jsonBuffer;
JsonObject &root = jsonBuffer.createObject();
void setup()
{

  Serial.begin(115200);
  esp8266.begin(115200);
  sendCommand("AT+RST\r\n", 5, "OK");
  sendCommand("AT+CWJAP=\"LightCore 2.5Ghz\",\"avgust14martin\"\r\n", 20, "WIFI CONNECTED");
  sendCommand("AT+CWMODE=3\r\n", 5, "OK");
  sendCommand("AT+CIPMUX=1\r\n", 5, "OK");
  sendCommand("AT+CIPSERVER=1,80\r\n", 20, "OK");

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
  }

  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  pinMode(longRelay, OUTPUT);
  pinMode(shortRelay, OUTPUT);
  pinMode(LedIndicator, OUTPUT);

  digitalWrite(longRelay, HIGH);
  digitalWrite(shortRelay, HIGH);
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

        command = (esp8266.read());
        if (command == '1')
        {
          digitalWrite(shortRelay, LOW);
          digitalWrite(LedIndicator, LOW);
          delay(1500);
          digitalWrite(shortRelay, HIGH);
          digitalWrite(LedIndicator, HIGH);
          UpdateSCFState(1);
        }
        else if (command == '2')
        {
          digitalWrite(longRelay, LOW);
          digitalWrite(LedIndicator, LOW);
          delay(1500);
          digitalWrite(longRelay, HIGH);
          digitalWrite(LedIndicator, HIGH);

          UpdateSCFState(1);
        }
        else if (command == '3')
        {
          digitalWrite(shortRelay, LOW);
          digitalWrite(longRelay, LOW);
          digitalWrite(LedIndicator, LOW);
          delay(1500);
          digitalWrite(shortRelay, HIGH);
          digitalWrite(longRelay, HIGH);
          digitalWrite(LedIndicator, HIGH);
          UpdateSCFState(0);
        }
        sendState(connectionId);
        Serial.println("button: " + command);
      }
      else if (command == '2')
      {
        delay(1000);
        sendState(connectionId);
        Serial.println("Get state");
      }
      else if (command == '3')
      {
        delay(1000);
        UpdateSCFState(0);
        sendState(connectionId);
        Serial.println("update state");
      }
    }
  }

  if (timerBool == 1)
  {
    if (rtc.now() >= timer)
    {
      UpdateSCFState(0);
      timerBool = 0;
    }
  }
}

void UpdateSCFState(int request)
{
  if (sCFState == "Cold" && request)
  {
    sCFState = "Hot";
    timer = DateTime(rtc.now() + TimeSpan(0, 0, 30, 0));
    timerBool = 1;
  }
  else if (sCFState == "Hot" && !request)
  {
    sCFState = "Cold";
  }
}

void sendState(int connectionId)
{
  root["sCFState"] = sCFState;
  String data;
  root.printTo(data);

  String request = "" + String("HTTP/1.1 200 OK\r\n") +
                   "Connection: close\r\n" +
                   "Content-Length: " + data.length() + "\r\n" +
                   "Content-Type: application/json\r\n" +
                   "\r\n" + data;

  String cipSend = "AT+CIPSEND=" + String(connectionId) + "," + String(request.length()) + "\r\n";
  sendCommand(cipSend, 4, ">");
  sendData(request);
  String closeCommand = "AT+CIPCLOSE=";
  closeCommand += connectionId;
  closeCommand += "\r\n";
  sendCommand(closeCommand, 5, "OK");
}

void sendCommand(String command, int delay, char reply[])
{
  int CountTime = 0;
  Serial.print("AT command => ");
  Serial.println(command);
  while (CountTime < delay)
  {
    esp8266.print(command);
    if (esp8266.find(reply))
    {
      Serial.println("Command sent");
      break;
    }
    CountTime++;
  }
}

void sendData(String Request)
{
  Serial.println(Request);
  esp8266.println(Request);
  delay(1500);
}
