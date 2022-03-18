// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

#define DEBUG true

RTC_DS1307 rtc;
SoftwareSerial esp8266(2, 3);

const int longRelay = 10;
const int shortRelay = 11;
const int LedIndicator = 9;
bool Heated = false;

// char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool coffeeDays[50][7];
int coffeeHours[50];
int coffeeMinutes[50];
int coffeeCounter = 0;

void setup()
{
    // coffeeDays[0][4] = 1;
    // coffeeHours[0] = 18;
    // coffeeMinutes[0] = 43;
    // coffeeCounter++;
    Serial.begin(115200);
    esp8266.begin(115200);

    pinMode(LedIndicator, OUTPUT);
    pinMode(longRelay, OUTPUT);
    pinMode(shortRelay, OUTPUT);
    digitalWrite(LedIndicator, LOW);
    digitalWrite(longRelay, LOW);
    digitalWrite(shortRelay, LOW);
    setupESP();
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
}

void loop()
{
    DateTime now = rtc.now();

    // Serial.print(now.year(), DEC);
    // Serial.print('/');
    // Serial.print(now.month(), DEC);
    // Serial.print('/');
    // Serial.print(now.day(), DEC);
    // Serial.print(" (");
    // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // Serial.print(") ");
    // Serial.print(now.hour(), DEC);
    // Serial.print(':');
    // Serial.print(now.minute(), DEC);
    // Serial.print(':');
    // Serial.print(now.second(), DEC);
    // Serial.println();

    if (esp8266.available())
    {
        char connectionId = '0';
        esp8266.find("command=");
        char command = (esp8266.read());
        Serial.println("\ncommand: ");
        Serial.println(command);
        Serial.println("\n");
        digitalWrite(LedIndicator, HIGH);
        String content = "";
        if (command == '1' && !Heated)
        {
            command = (esp8266.read());

            if (command == '1')
            {
                Serial.println("\ncommand: ");
                Serial.println(command);
                Serial.println("\n");
                digitalWrite(longRelay, HIGH);
                delay(2000);
                digitalWrite(longRelay, LOW);
                content = "heating";
            }
            else if (command == '2')
            {
                Serial.println("\ncommand: ");
                Serial.println(command);
                Serial.println("\n");
                digitalWrite(shortRelay, HIGH);
                delay(2000);
                digitalWrite(shortRelay, LOW);
                content = "heating";
            }
        }
        else if (command == '2')
        {
            int command_int = (esp8266.read()) - 48;
            for (int i = 0; i < command_int; i++)
            {
                command_int = (esp8266.read()) - 48;
                coffeeDays[coffeeCounter][command_int] = true;
            }

            command_int = (esp8266.read()) - 48;
            if (command_int == 0)
            {
                command_int = (esp8266.read()) - 48;
                coffeeHours[coffeeCounter] = command_int;
            }
            else
            {
                command_int *= 10;
                command_int += (esp8266.read()) - 48;
                coffeeHours[coffeeCounter] = command_int;
            }

            command_int = (esp8266.read()) - 48;
            if (command_int == 0)
            {
                command_int = (esp8266.read()) - 48;
                coffeeMinutes[coffeeCounter] = command_int;
            }
            else
            {
                command_int *= 10;
                command_int += (esp8266.read()) - 48;
                coffeeMinutes[coffeeCounter] = command_int;
            }
        }
        Serial.println(content);
        sendHTTPResponse(connectionId, content);
        // make close command
        String closeCommand = "AT+CIPCLOSE=";
        closeCommand += connectionId;
        closeCommand += "\r\n";
        sendCommand(closeCommand, 1000, DEBUG);
        digitalWrite(LedIndicator, LOW);
        delay(25000);
        Heated = true;
    }

    for (int i = 0; i < coffeeCounter; i++)
    {
        if (coffeeDays[now.dayOfTheWeek()] && (now.hour() == coffeeHours[i] && now.minute() == coffeeMinutes[i]))
        {
            digitalWrite(LedIndicator, HIGH);
            delay(1000);
            digitalWrite(LedIndicator, LOW);
            Serial.println("It worked?");
            delay(20000);

            break;
        }
    }

    // calculate a date which is 7 days and 30 seconds into the future
    //  DateTime future (now + TimeSpan(7, 12, 30, 6));
    //
    //  Serial.print(" now + 7d + 30s: ");
    //  Serial.print(future.year(), DEC);
    //  Serial.print('/');
    //  Serial.print(future.month(), DEC);
    //  Serial.print('/');
    //  Serial.print(future.day(), DEC);
    //  Serial.print(' ');
    //  Serial.print(future.hour(), DEC);
    //  Serial.print(':');
    //  Serial.print(future.minute(), DEC);
    //  Serial.print(':');
    //  Serial.print(future.second(), DEC);
    //  Serial.println();
    //
    //  Serial.println();
    // delay(30000);
}

void setupESP()
{
    sendCommand("AT+RST\r\n", 2000, DEBUG);
    sendCommand("AT+CWJAP=\"Hacked device\",\"drowpass\"\r\n", 6000, DEBUG);
    sendCommand("AT+CIPSTA=\"170.20.10.5\"\r\n", 1000, DEBUG); // get ip address
    sendCommand("AT+CIPSTA?\r\n", 1000, DEBUG);
    sendCommand("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
    sendCommand("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);
    Serial.println("Server Ready");
}

/*
   Name: sendData
   Description: Function used to send data to ESP8266.
   Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
   Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    int dataSize = command.length();
    char data[dataSize];
    command.toCharArray(data, dataSize);
    esp8266.write(data, dataSize); // send the read character to the esp8266
    if (debug)
    {
        Serial.println("\r\n====== HTTP Response From Arduino ======");
        Serial.write(data, dataSize);
        Serial.println("\r\n========================================");
    }
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (esp8266.available())
        {
            // The esp has data so display its output to the serial window
            char c = esp8266.read(); // read the next character.
            response += c;
        }
    }
    if (debug)
    {
        Serial.print(response);
    }
    return response;
}

/*
   Name: sendHTTPResponse
   Description: Function that sends HTTP 200, HTML UTF-8 response
*/
void sendHTTPResponse(char connectionId, String content)
{
    // build HTTP response
    Serial.print(content);
    String httpResponse;
    String httpHeader;
    // HTTP Header
    httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n";
    httpHeader += "Content-Length: ";
    httpHeader += content.length();
    httpHeader += "\r\n";
    httpHeader += "Connection: close\r\n";
    httpResponse = httpHeader + "\r\n "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
    sendCIPData(connectionId, httpResponse);
}

/*
   Name: sendCIPDATA
   Description: sends a CIPSEND=<connectionId>,<data> command

*/
void sendCIPData(char connectionId, String data)
{
    String test = "testing";
    String cipSend = "AT+CIPSEND=0,30\r\n";
    sendCommand(cipSend, 1000, DEBUG);
    esp8266.println(data);
    esp8266.println("\r\n");
    sendData(data, 1000, DEBUG);
}

/*
   Name: sendCommand
   Description: Function used to send data to ESP8266.
   Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
   Returns: The response from the esp8266 (if there is a reponse)
*/
String sendCommand(String command, const int timeout, boolean debug)
{
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (esp8266.available())
        {
            // The esp has data so display its output to the serial window
            char c = esp8266.read(); // read the next character.
            response += c;
        }
    }
    if (debug)
    {
        Serial.print(response);
    }
    return response;
}
