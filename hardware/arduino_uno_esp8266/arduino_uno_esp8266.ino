// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <SoftwareSerial.h>

#define DEBUG true

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
}

void loop()
{

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
        Serial.println("send HTTP Request");
        sendHTTPResponse(connectionId);
        // close connection
        sendCommand("AT+CIPCLOSE=0\r\n", 1000, DEBUG);

        digitalWrite(LedIndicator, LOW);
        //  delay(25000);
        Heated = true;
    }

    //    for (int i = 0; i < coffeeCounter; i++)
    //    {
    //        if (coffeeDays[now.dayOfTheWeek()] && (now.hour() == coffeeHours[i] && now.minute() == coffeeMinutes[i]))
    //        {
    //            digitalWrite(LedIndicator, HIGH);
    //            delay(1000);
    //            digitalWrite(LedIndicator, LOW);
    //            Serial.println("It worked?");
    //            delay(20000);
    //
    //            break;
    //        }
    //    }

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
    sendCommand("AT+CWJAP=\"LightCore 2.5Ghz\",\"avgust14martin\"\r\n", 3000, DEBUG);
    sendCommand("AT+CIPSTA=\"192.168.88.200\"\r\n", 1000, DEBUG); // get ip address
    sendCommand("AT+CIFSR\r\n", 1000, DEBUG);
    sendCommand("AT+CWMODE=3\r\n", 1000, DEBUG);
    sendCommand("AT+CIPMUX=1\r\n", 1000, DEBUG);       // configure for multiple connections
    sendCommand("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // set server
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
    Serial.println("Sending: " + command);
    esp8266.println(command);
    if (debug)
    {
        Serial.print("Command: ");
        Serial.println(command);
    }
    unsigned long start = millis();
    while (millis() - start < timeout)
    {
        if (esp8266.available())
        {
            char c = esp8266.read();
            response += c;
        }
    }
    if (debug)
    {
        Serial.println(response);
    }
    return response;

    // String response = "";
    // int dataSize = command.length();
    // char data[dataSize];
    // command.toCharArray(data, dataSize);
    // esp8266.write(data, dataSize); // send the read character to the esp8266
    // if (debug)
    // {
    //     Serial.println("\r\n====== HTTP Response From Arduino ======");
    //     Serial.write(data, dataSize);
    //     Serial.println("\r\n========================================");
    // }
    // long int time = millis();
    // while ((time + timeout) > millis())
    // {
    //     while (esp8266.available())
    //     {
    //         // The esp has data so display its output to the serial window
    //         char c = esp8266.read(); // read the next character.
    //         response += c;
    //     }
    // }
    // if (debug)
    // {
    //     Serial.print(response);
    // }
    // return response;
}

/*
   Name: sendHTTPResponse
   Description: Function that sends HTTP 200, HTML UTF-8 response
*/
void sendHTTPResponse(char connectionId)
{
    // build HTTP response
    String body;
    String httpResponse;
    String httpHeader;
    // HTTP Body
    body = "<!DOCTYPE html>\r\n";
    body += "<html>\r\n";
    body += "<head>\r\n";
    body += "<meta charset=\"UTF-8\">\r\n";
    body += "<title>ESP8266</title>\r\n";
    body += "</head>\r\n";
    body += "<body>\r\n";
    body += "<h1>ESP8266</h1>\r\n";
    body += "<p>Connection ID: ";
    body += connectionId;
    body += "</p>\r\n";
    body += "<p>Heating: ";
    body += Heated;
    body += "</p>\r\n";
    body += "</body>\r\n";
    body += "</html>\r\n";
    // HTTP Header
    httpHeader = "HTTP/1.1 200 OK\r\n";
    httpHeader += "Content-Type: text/html; charset=UTF-8\r\n";
    httpHeader += "Connection: close\r\n";
    httpHeader += "Content-Length: ";
    httpHeader += body.length();
    httpHeader += "\r\n\r\n";
    // HTTP Response
    httpResponse = httpHeader + body;
    // send the response
    Serial.println("send CIP Data");
    sendCIPData(connectionId, httpResponse);
}

/*
   Name: sendCIPDATA
   Description: sends a CIPSEND=<connectionId>,<data> command

*/
void sendCIPData(char connectionId, String data)
{
    String cipSend = "AT+CIPSEND=";
    cipSend += connectionId;
    cipSend += ",";
    cipSend += data.length();
    cipSend += "\r\n";
    Serial.println("CIPSEND Command");
    sendCommand(cipSend, 1000, DEBUG);
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
    char c;
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (esp8266.available())
        {
            // The esp has data so display its output to the serial window
            c = esp8266.read(); // read the next character.
            response += c;
        }
        if (c == '>')
        {
            Serial.println("> found");
            break;
        }
    }
    if (debug)
    {
        Serial.println("Response: " + response);
    }
    return response;
}
