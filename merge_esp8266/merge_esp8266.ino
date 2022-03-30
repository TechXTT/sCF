
#include <SoftwareSerial.h>
#include <Wire.h>

int LED1 = 9;
int LED2 = 10;
int LED3 = 11;

const char SSID_ESP[] = "LightCore 2.5Ghz";
const char SSID_KEY[] = "avgust14martin";
const char *host = "smartembeddedcoffee.000webhostapp.com";
String DB_id = "99999";
String DB_password = "Esp8266DataBase!";
String location_url = "/TX.php?id=";

String url = "";
String URL_withPacket = "";
unsigned long multiplier[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

const char CWMODE = '1';
const char CIPMUX = '1';

boolean setup_ESP();
boolean read_until_ESP(const char keyword1[], int keysize, int timeout_val, byte mode);
void timeout_start();
boolean timeout_check(int timeout_ms);
void serial_dump_ESP();
boolean connect_ESP();
void connect_webhost();
unsigned long timeout_start_val;
char scratch_data_from_ESP[20]; // first byte is the length of bytes
char payload[200];
int payload_size = 0, counter = 0;
char ip_address[16];

// bool sent_bool_2 = 0;
// bool sent_bool_3 = 0;

bool received_bool_1 = 0;
bool received_bool_2 = 0;
// bool received_bool_3 = 0;
// bool received_bool_4 = 0;
// bool received_bool_5 = 0;
// int received_nr_1 = 0;
// int received_nr_2 = 0;
// int received_nr_3 = 0;
// int received_nr_4 = 0;
// int received_nr_5 = 0;
// String received_text = "";

char t1_from_ESP[5]; // For time from web
char d1_from_ESP[2]; // For received_bool_1
char d2_from_ESP[2]; // For received_bool_2
// char d3_from_ESP[2];    // For received_bool_3
// char d4_from_ESP[2];    // For received_bool_4
// char d5_from_ESP[2];    // For received_bool_5
// char d9_from_ESP[6];    // For received_nr_1
// char d10_from_ESP[6];   // For received_nr_2
// char d11_from_ESP[6];   // For received_nr_3
// char d12_from_ESP[6];   // For received_nr_4
// char d13_from_ESP[6];   // For received_nr_5
// char d14_from_ESP[300]; // For received_text

const char keyword_OK[] = "OK";
const char keyword_Ready[] = "ready";
const char keyword_no_change[] = "no change";
const char keyword_blank[] = "#&";
const char keyword_ip[] = "192.";
const char keyword_rn[] = "\r\n";
const char keyword_quote[] = "\"";
const char keyword_carrot[] = ">";
const char keyword_sendok[] = "SEND OK";
const char keyword_linkdisc[] = "Unlink";

const char keyword_t1[] = "t1";
const char keyword_b1[] = "b1";
const char keyword_b2[] = "b2";
// const char keyword_b3[] = "b3";
// const char keyword_b4[] = "b4";
// const char keyword_b5[] = "b5";
// const char keyword_n1[] = "n1";
// const char keyword_n2[] = "n2";
// const char keyword_n3[] = "n3";
// const char keyword_n4[] = "n4";
// const char keyword_n5[] = "n5";
// const char keyword_n6[] = "n6";
const char keyword_doublehash[] = "##";

SoftwareSerial ESP8266(2, 3); // rx tx

void setup()
{

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  ESP8266.begin(115200);
  Serial.begin(115200);

  delay(2000);
  setup_ESP();
}

void loop()
{
  send_to_server_5();
  digitalWrite(LED1, received_bool_1);
  digitalWrite(LED2, received_bool_2);
  //digitalWrite(LED3, received_bool_2 || received_bool_1);

  delay(1000);
}

boolean setup_ESP()
{ // returns a '1' if successful

  ESP8266.print("AT\r\n"); // Send just 'AT' to make sure the ESP is responding
  // this read_until_... function is used to find a keyword in the ESP response - more on this later and in the function itself
  if (read_until_ESP(keyword_OK, sizeof(keyword_OK), 5000, 0)) // go look for keyword "OK" with a 5sec timeout
    Serial.println("ESP CHECK OK");
  else
    Serial.println("ESP CHECK FAILED");
  serial_dump_ESP(); // this just reads everything in the buffer and what's still coming from the ESP

  ESP8266.print("AT+RST\r\n");                                       // Give it a reset - who knows what condition it was in, better to start fresh
  if (read_until_ESP(keyword_Ready, sizeof(keyword_Ready), 5000, 0)) // go look for keyword "Ready" - takes a few seconds longer to complete
    Serial.println("ESP RESET OK");                                // depneding on the FW version on the ESP, sometimes the Ready is with a lowercase r - ready
  else
    Serial.println("ESP RESET FAILED");
  serial_dump_ESP();

  ESP8266.print("AT+CWMODE="); // set the CWMODE
  ESP8266.print(CWMODE);       // just send what is set in the constant
  ESP8266.print("\r\n");
  if (read_until_ESP(keyword_OK, sizeof(keyword_OK), 5000, 0)) // go look for keyword "OK"
    Serial.println("ESP CWMODE SET");
  else
    Serial.println("ESP CWMODE SET FAILED"); // probably going to fail, since a 'no change' is returned if already set - would be nice to check for two words
  serial_dump_ESP();

  // Here's where the SSID and PW are set
  ESP8266.print("AT+CWJAP=\""); // set the SSID AT+CWJAP="SSID","PW"
  ESP8266.print(SSID_ESP);      // from constant
  ESP8266.print("\",\"");
  ESP8266.print(SSID_KEY); // form constant
  ESP8266.print("\"\r\n");
  if (read_until_ESP(keyword_OK, sizeof(keyword_OK), 10000, 0)) // go look for keyword "OK"
    Serial.println("ESP SSID SET OK");
  else
    Serial.println("ESP SSID SET FAILED");
  serial_dump_ESP();

  // This checks for and stores the IP address
  Serial.println("CHECKING FOR AN IP ADDRESS");
  ESP8266.print("AT+CIPSTA=\"192.168.88.213\"\r\n");
  if (read_until_ESP(keyword_OK, sizeof(keyword_OK), 10000, 0)) // go look for keyword "OK"
    Serial.println("ESP IP SET");
  else
    Serial.println("ESP IP SET FAILED");
  serial_dump_ESP();
  ESP8266.print("AT+CIFSR\r\n"); // command to retrieve IP address from ESP
  if (read_until_ESP(keyword_rn, sizeof(keyword_rn), 10000, 0))
  { // look for first \r\n after AT+CIFSR echo - note mode is '0', the ip address is right after this
    if (read_until_ESP(keyword_rn, sizeof(keyword_rn), 1000, 1))
    { // look for second \r\n, and store everything it receives, mode='1'
      // store the ip adress in its variable, ip_address[]
      for (int i = 1; i <= (scratch_data_from_ESP[0] - sizeof(keyword_rn) + 1); i++) // that i<=... is going to take some explaining, see next lines
        ip_address[i] = scratch_data_from_ESP[i];                                  // fill up ip_address with the scratch data received
      ip_address[0] = (scratch_data_from_ESP[0] - sizeof(keyword_rn) + 1);           // store the length of ip_address in [0], same thing as before
      Serial.print("IP ADDRESS = ");                                                 // print it off to verify
      for (int i = 1; i <= ip_address[0]; i++)                                       // send out the ip address
        Serial.print(ip_address[i]);
      Serial.println("");
    }
  }
  else
    Serial.print("IP ADDRESS FAIL");
  serial_dump_ESP();

  ESP8266.print("AT+CIPMUX="); // set the CIPMUX
  ESP8266.print(CIPMUX);       // from constant
  ESP8266.print("\r\n");
  if (read_until_ESP(keyword_OK, sizeof(keyword_OK), 5000, 0)) // go look for keyword "OK" or "no change
    Serial.println("ESP CIPMUX SET");
  else
    Serial.println("ESP CIPMUX SET FAILED");
  serial_dump_ESP();
}

// This function goes and finds a keyword received from the ESP
//  const char keyword1[] = the keyword that this function will look for and return '1' when found
//  int key_size = size of that keyword - sizeof()
//  int timeout_val - timeout if the keword is not found (in milliseconds 5000=5sec)
//  byte mode - if '1', this function will save every byte received to 'scratch_data_from_ESP[]' with length of 'scratch_length' - useful for parsing between keywords
boolean read_until_ESP(const char keyword1[], int key_size, int timeout_val, byte mode)
{
  timeout_start_val = millis(); // for the timeout
  char data_in[20];             // this is the buffer - if keyword is longer than 20, then increase this
  int scratch_length = 1;       // the length of the scratch data array
  key_size--;                   // since we're going to get an extra charachter from the sizeof()

  // FILL UP THE BUFFER
  for (byte i = 0; i < key_size; i++)
  { // we only need a buffer as long as the keyword

    // timing control
    while (!ESP8266.available())
    { // wait until a new byte is sent down from the ESP - good way to keep in lock-step with the serial port
      if ((millis() - timeout_start_val) > timeout_val)
      { // if nothing happens within the timeout period, get out of here
        Serial.println("timeout 1");
        return 0; // this will end the function
      }             // timeout
    }                 // while !avail

    data_in[i] = ESP8266.read(); // save the byte to the buffer 'data_in[]
    if (mode == 1)
    { // this will save all of the data to the scratch_data_from
      scratch_data_from_ESP[scratch_length] = data_in[i]; // starts at 1
      scratch_data_from_ESP[0] = scratch_length;          // [0] is used to hold the length of the array
      scratch_length++;                                   // increment the length
    }                                                       // mode 1

  } // for i

  // THE BUFFER IS FULL, SO START ROLLING NEW DATA IN AND OLD DATA OUT
  while (1)
  { // stay in here until the keyword found or a timeout occurs

    // run through the entire buffer and look for the keyword
    // this check is here, just in case the first thing out of the ESP was the keyword, meaning the buffer was actually filled with the keyword
    for (byte i = 0; i < key_size; i++)
    {

      if (keyword1[i] != data_in[i]) // if it doesn't match, break out of the search now
        break;                     // get outta here
      if (i == (key_size - 1))
      { // we got all the way through the keyword without breaking, must be a match!
        return 1; // return a 1 and get outta here!
      }             // if
    }                 // for byte i

    // start rolling the buffer
    for (byte i = 0; i < (key_size - 1); i++)
    { // keysize-1 because everthing is shifted over - see next line
      data_in[i] = data_in[i + 1]; // so the data at 0 becomes the data at 1, and so on.... the last value is where we'll put the new data
    }                                // for

    // timing control
    while (!ESP8266.available())
    { // same thing as done in the buffer
      if ((millis() - timeout_start_val) > timeout_val)
      {
        Serial.println("timeout 2");
        return 0;
      } // timeout
    }     // while !avail

    data_in[key_size - 1] = ESP8266.read(); // save the new data in the last position in the buffer
    // Serial.write(data_in[key_size-1]);

    if (mode == 1)
    { // continue to save everything if this is set
      scratch_data_from_ESP[scratch_length] = data_in[key_size - 1];
      scratch_data_from_ESP[0] = scratch_length;
      scratch_length++;

      // Serial.write(scratch_data_from_ESP[0]);
    } // mode 1

    //  JUST FOR DEBUGGING
    // if(ESP8266.overflow())


  } // while 1

} // read until ESP

boolean read_ESP(int timeout_val) {
  long int time = millis();
  String response = "";
  char c;
  int scratch_length = 1;
  while ((time + timeout_val) > millis()) {
    while (ESP8266.available()) {
      c = ESP8266.read();
      response += c;                                 // increment the length
      delay(1);
    }
  }
  Serial.println("read_ESP response: ");
  Serial.println(response);
  Serial.println("");
}

// pretty simple function - read everything out of the serial buffer and whats coming and get rid of it
void serial_dump_ESP()
{
  char temp;

  while (ESP8266.available())
  {
    temp = ESP8266.read();
    delay(1); // could play around with this value if buffer overflows are occuring
  }             // while
  //    Serial.println("DUMPED");

} // serial dump

void send_to_server_5()
{
  // we have changing variable here, so we need to first build up our URL packet
  /*URL_withPacket = URL_webhost;//pull in the base URL
    URL_withPacket += String(unit_id);//unit id value
    URL_withPacket += "&sensor=";//unit id 1
    URL_withPacket += String(sensor_value);//sensor value
    URL_withPacket += payload_closer;*/
  url = location_url;
  url += DB_id;
  url += "&pw=";
  url += DB_password;

  URL_withPacket = "";

  URL_withPacket += "GET ";
  URL_withPacket += url;
  URL_withPacket += " HTTP/1.1\r\n";
  URL_withPacket += "Host: ";
  URL_withPacket += host;
  URL_withPacket += "\r\n";
  URL_withPacket += "Connection: close\r\n\r\n";

  /*
    URL_withPacket += "Host: www.electronoobs.com\r\n";
    URL_withPacket += "Connection: close\r\n\r\n";
  */
  /// This builds out the payload URL - not really needed here, but is very handy when adding different arrays to the payload
  counter = 0; // keeps track of the payload size
  payload_size = 0;
  for (int i = 0; i < URL_withPacket.length(); i++)
  { // using a string this time, so use .length()
    payload[i] = URL_withPacket[i]; // build up the payload
    payload_size++;                                     // increment the counter
  }                                                  // for int
  for (int i = 0; i < payload_size; i++) //print the payload to the ESP
    Serial.print(payload[i]);
  Serial.println();
  if (connect_ESP())
  { // this calls 'connect_ESP()' and expects a '1' back if successful
    // nice, we're in and ready to look for data
    // first up, we need to parse the returned data  _t1123##_d15##_d210##
    Serial.println("FOUND DATA1");
    // Serial.println("connected ESP");

    if (read_until_ESP(keyword_t1, sizeof(keyword_t1), 5000, 1))
    { // go find t1 then stop
      Serial.println("FOUND DATA2");
      if (read_until_ESP(keyword_doublehash, sizeof(keyword_doublehash), 5000, 1))
      { // our data is next, so change mode to '1' and stop at ##
        // got our data, so quickly store it away in d1
        for (int i = 1; i <= (scratch_data_from_ESP[0] - sizeof(keyword_doublehash) + 1); i++) // go see 'setup' and how this was done with the ip address for more info
          t1_from_ESP[i] = scratch_data_from_ESP[i];
        t1_from_ESP[0] = (scratch_data_from_ESP[0] - sizeof(keyword_doublehash) + 1);
        Serial.println("FOUND DATA3");
        // we still have more data to get out of this stream, now we want d1
        if (read_until_ESP(keyword_b1, sizeof(keyword_b1), 5000, 0))
        { // same as before - first d1
          Serial.println("FOUND DATA4");
          if (read_until_ESP(keyword_doublehash, sizeof(keyword_doublehash), 5000, 1))
          { // now ## and mode=1
            for (int i = 1; i <= (scratch_data_from_ESP[0] - sizeof(keyword_doublehash) + 1); i++)
              d1_from_ESP[i] = scratch_data_from_ESP[i];
            d1_from_ESP[0] = (scratch_data_from_ESP[0] - sizeof(keyword_doublehash) + 1);
            Serial.println("FOUND DATA5 ");
            if (read_until_ESP(keyword_b2, sizeof(keyword_b2), 5000, 0))
            { // same as before - first d1
              if (read_until_ESP(keyword_doublehash, sizeof(keyword_doublehash), 5000, 1))
              { // now ## and mode=1
                for (int i = 1; i <= (scratch_data_from_ESP[0] - sizeof(keyword_doublehash) + 1); i++)
                  d2_from_ESP[i] = scratch_data_from_ESP[i];
                d2_from_ESP[0] = (scratch_data_from_ESP[0] - sizeof(keyword_doublehash) + 1);

                // now that we have our data, go wait for the connection to disconnect
                //- the ESP will eventually return 'Unlink'
                // delay(10);
                Serial.println("FOUND DATA & DISCONNECTED");
                serial_dump_ESP(); // now we can clear out the buffer and read whatever is still there
                // let's see how the data looks

                Serial.println("");
                Serial.print("Time ");
                if (t1_from_ESP[0] > 3)
                {
                  Serial.print(t1_from_ESP[1]);
                  Serial.print(t1_from_ESP[2]);
                  Serial.print(":");
                  Serial.print(t1_from_ESP[3]);
                  Serial.println(t1_from_ESP[4]);
                }
                else
                {
                  Serial.print(t1_from_ESP[1]);
                  Serial.print(":");
                  Serial.print(t1_from_ESP[2]);
                  Serial.println(t1_from_ESP[3]);
                }

                Serial.print("RECEIVED_BOOL_1 = "); // print out LED data and convert to integer
                received_bool_1 = 0;
                for (int i = 1; i <= d1_from_ESP[0]; i++)
                {
                  // Serial.print(d12_from_ESP[i]);
                  received_bool_1 = received_bool_1 + ((d1_from_ESP[i] - 48) * multiplier[d1_from_ESP[0] - i]);
                }
                Serial.println(received_bool_1);

                Serial.print("RECEIVED_BOOL_2 = "); // print out LED data and convert to integer
                received_bool_2 = 0;
                for (int i = 1; i <= d2_from_ESP[0]; i++)
                {
                  // Serial.print(d12_from_ESP[i]);
                  received_bool_2 = received_bool_2 + ((d2_from_ESP[i] - 48) * multiplier[d2_from_ESP[0] - i]);
                }
                Serial.println(received_bool_2);

                Serial.println("");
                // that's it!!
              } //##
            }     // b2
          }         //##
        }             // b1
      }                 //##
    } else {               // t1
    }
    /*
      //Serial.println("connected ESP");
      //we still have more data to get out of this stream, now we want d1
      if(read_until_ESP(keyword_b6,sizeof(keyword_b6),5000,0)){//same as before - first d1
      if(read_until_ESP(keyword_doublehash,sizeof(keyword_doublehash),5000,1)){//now ## and mode=1
      for(int i=1; i<=(scratch_data_from_ESP[0]-sizeof(keyword_doublehash)+1); i++)
      d6_from_ESP[i] = scratch_data_from_ESP[i];
      d6_from_ESP[0] = (scratch_data_from_ESP[0]-sizeof(keyword_doublehash)+1);

      if(read_until_ESP(keyword_b7,sizeof(keyword_b7),5000,0)){//same as before - first d1
      if(read_until_ESP(keyword_doublehash,sizeof(keyword_doublehash),5000,1)){//now ## and mode=1
      for(int i=1; i<=(scratch_data_from_ESP[0]-sizeof(keyword_doublehash)+1); i++)
      d7_from_ESP[i] = scratch_data_from_ESP[i];
      d7_from_ESP[0] = (scratch_data_from_ESP[0]-sizeof(keyword_doublehash)+1);

      if(read_until_ESP(keyword_b8,sizeof(keyword_b8),5000,0)){//same as before - first d1
      if(read_until_ESP(keyword_doublehash,sizeof(keyword_doublehash),5000,1)){//now ## and mode=1
      for(int i=1; i<=(scratch_data_from_ESP[0]-sizeof(keyword_doublehash)+1); i++)
      d8_from_ESP[i] = scratch_data_from_ESP[i];
      d8_from_ESP[0] = (scratch_data_from_ESP[0]-sizeof(keyword_doublehash)+1);



      //now that we have our data, go wait for the connection to disconnect
      //- the ESP will eventually return 'Unlink'
      //delay(10);
      Serial.println("FOUND DATA & DISCONNECTED");
      serial_dump_ESP();//now we can clear out the buffer and read whatever is still there
      //let's see how the data looks

      Serial.print("SENT_BOOL_1 = ");//print out LED data and convert to integer
      sent_bool_1_feedback = 0;
      for(int i=1; i<=d6_from_ESP[0]; i++){
      //Serial.print(d12_from_ESP[i]);
      sent_bool_1_feedback = sent_bool_1_feedback + ((d6_from_ESP[i] - 48) * multiplier[d6_from_ESP[0] - i]);
      }
      Serial.println(sent_bool_1_feedback);

      Serial.print("SENT_BOOL_2 = ");//print out LED data and convert to integer
      sent_bool_2_feedback = 0;
      for(int i=1; i<=d7_from_ESP[0]; i++){
      //Serial.print(d12_from_ESP[i]);
      sent_bool_2_feedback = sent_bool_2_feedback + ((d7_from_ESP[i] - 48) * multiplier[d7_from_ESP[0] - i]);
      }
      Serial.println(sent_bool_1_feedback);

      Serial.print("SENT_BOOL_3 = ");//print out LED data and convert to integer
      sent_bool_3_feedback = 0;
      for(int i=1; i<=d8_from_ESP[0]; i++){
      //Serial.print(d12_from_ESP[i]);
      sent_bool_3_feedback = sent_bool_3_feedback + ((d8_from_ESP[i] - 48) * multiplier[d8_from_ESP[0] - i]);
      }
      Serial.println(sent_bool_3_feedback);
      Serial.println("");
      //that's it!!
      }//##
      }//b8
      }//##
      }//b7
      }//##
      }//b6
    */

  } // connect ESP
} // connect web host

boolean connect_ESP()
{ // returns 1 if successful or 0 if not

  Serial.println("CONNECTING");
  // or 443 para HTTPS
  // enshare.000webhostapp.co
  ESP8266.print("AT+CIPSTART=0,\"TCP\",\"smartembeddedcoffee.000webhostapp.com\",80\r\n"); // connect to your web server

  // read_until_ESP(keyword,size of the keyword,timeout in ms, data save 0-no 1-yes 'more on this later')
  if (read_until_ESP(keyword_OK, sizeof(keyword_OK), 5000, 0))
  { // go look for 'OK' and come back
    serial_dump_ESP();              // get rid of whatever else is coming
    Serial.println("CONNECTED");    // yay, connected
    ESP8266.print("AT+CIPSEND=0,"); // send AT+CIPSEND=0, size of payload
    ESP8266.print(payload_size);    // the payload size
    serial_dump_ESP();              // everything is echoed back, so get rid of it
    ESP8266.print("\r\n\r\n");      // cap off that command with a carriage return and new line feed

    if (read_until_ESP(keyword_carrot, sizeof(keyword_carrot), 5000, 0))
    { // go wait for the '>' character, ESP ready for the payload
      Serial.println("READY TO SEND");

      Serial.println(payload_size);

      for (int i = 0; i < payload_size; i++)
      { // print the payload to the ESP
        ESP8266.print(payload[i]);

        Serial.print(payload[i]);
      }
      Serial.println("");

      /*
        Serial.println("");
        Serial.println(payload_size);
        Serial.println("");
      */

      if (read_until_ESP(keyword_sendok, sizeof(keyword_sendok), 5000, 1))
      { // go wait for 'SEND OK'
        Serial.println("SENT"); // yay, it was sent
        read_ESP(5000);
        return 1;               // get out of here, data is about to fly out of the ESP
      }                           // got the SEND OK
      else                        // SEND OK
        Serial.println("FAILED TO SEND");

    } // got the back carrot >
    else
      Serial.println("FAILED TO GET >");

  } // First OK
  else
  {
    Serial.println("FAILED TO CONNECT"); // something went wrong
    //setup_ESP();                         // optional, this will go setup the module and attempt to repair itself - connect to SSID, set the CIPMUX, etc...
  }

} // VOID CONNECT FUNCTION
