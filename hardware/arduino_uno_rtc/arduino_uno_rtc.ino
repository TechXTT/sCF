// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int LED = 10;
bool coffeeDays[50][7];
int coffeeHours[50];
int coffeeMinutes[50];
int coffeeCounter = 0;
void setup()
{
    coffeeDays[0][4] = 1;
    coffeeHours[0] = 18;
    coffeeMinutes[0] = 43;
    coffeeCounter++;
    Serial.begin(57600);
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
    }
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
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

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    //
    //  Serial.print(" since midnight 1/1/1970 = ");
    //  Serial.print(now.unixtime());
    //  Serial.print("s = ");
    //  Serial.print(now.unixtime() / 86400L);
    //  Serial.println("d");
    for (int i = 0; i < coffeeCounter; i++)
    {
        if (coffeeDays[now.dayOfTheWeek()] && (now.hour() == coffeeHours[i] && now.minute() == coffeeMinutes[i]))
        {
            digitalWrite(LED, HIGH);
            delay(1000);
            digitalWrite(LED, LOW);
            Serial.println("It worked?");
            delay(20000);

            break;
        }
    }
    //  if(( 0 < now.dayOfTheWeek() < 6) && (now.hour() == 16 && now.minute() == 5-1)){
    //    digitalWrite(LED, HIGH);
    //    delay(1000);
    //    digitalWrite(LED, LOW);
    //    Serial.println("It worked?");
    //  }else{
    //    Serial.println("wtf come on");
    //    Serial.print(now.hour(), DEC);
    //    Serial.print(':');
    //    Serial.print(now.minute(), DEC);
    //    Serial.println();
    //  }

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
    delay(30000);
}