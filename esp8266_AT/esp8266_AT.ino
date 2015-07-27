#include <SoftwareSerial.h>
 
#define DEBUG true
 
SoftwareSerial esp8266(3,4); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

String IP;
String ssid = "worktong514";
String pass = "1234567890";
int connectionId;
/*
String Webpage_1 = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
                   "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
                   "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><title>ESP8266</title><style type=\"text/css\">"
                   "#HeadTable {font-family: \"Comic Sans MS\", cursive;font-size: 24px;font-style: italic;font-weight: bolder;color: #FFF;}"
                   "button {font-family: \"Comic Sans MS\", cursive;font-size: 18px;background-color: #FF6;color: #39F;}";

String Webpage_2 = "</style></head><body bgcolor=\"#FFFFCC\">&nbsp;<table width=\"80%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\"><tr>"
                   "<td width=\"10%\" bgcolor=\"#00CCFF\">&nbsp;</td><td width=\"90%\" height=\"80\" bgcolor=\"#00CCFF\" id=\"HeadTable\">ESP8266 Smart Control</td></tr></table>"
                   "<table width=\"80%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\"><tr>"
                   "<td height=\"80\" bgcolor=\"#00FF99\" align=\"center\">&nbsp;<button onClick=\"window.location='/status/1';\">ON</button></td></tr>"
                   "<tr><td height=\"80\" bgcolor=\"#00FF99\" align=\"center\">&nbsp;<button onClick=\"window.location='/status/0';\" >OFF</button></td></tr></table>"
                   "<input type=\"button\" onClick=\"window.location='/status/1';\" value=\"ON\" /><input type=\"button\" onClick=\"window.location='/status/0';\" value=\"OFF\" />"
                   "</body></html>";
*/

String Webpage_A = "<title>ESP8266</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                   "<h1>GPIO16 Control</h1><button onClick=\"window.location='/status/1';\">ON</button>&nbsp;&nbsp;"
                   "<button onClick=\"window.location='/status/0';\">OFF</button>";

void setup()
{
  Serial.begin(19200);
  esp8266.begin(19200); // your esp's baud rate might be different

  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CWJAP=\"" + ssid + "\",\"" + pass + "\"\r\n",6000,DEBUG);
  IP = sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address 
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}
 
void loop()
{
  if(esp8266.available()) // check if the esp is sending a message 
  {
    /*
    while(esp8266.available())
    {
      // The esp has data so display its output to the serial window 
      char c = esp8266.read(); // read the next character.
      Serial.write(c);
    } */
    
    if(esp8266.find("+IPD,"))
    {
     delay(200);
 
     connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
                                           
     esp8266.find("status/"); // advance cursor to "pin="
     
     //int pinNumber = (esp8266.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
     //pinNumber += (esp8266.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
    
     int control_led = esp8266.read()-48;
     if(control_led < 0) control_led = 0;
     Serial.println(":status/" + String(control_led));
     if(control_led) digitalWrite(13,HIGH);
     else digitalWrite(13,LOW);     
     
     //digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin    
     //String webpage = "<h1>Hello</h1><h2>World!</h2><button>LED1</button><button>LED2</button>IP: " + IP;
     /*
     sendWebpage("<title>ESP8266</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                 "<h1>GPIO16 Control</h1><button onClick=\"window.location='/status/1';\">ON</button>&nbsp;&nbsp;"
                 "<button onClick=\"window.location='/status/0';\">OFF</button>");   
      */
      sendWebpage(Webpage_A);
      //sendWebpage(Webpage_1);
      //sendWebpage(Webpage_2);

     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendData(closeCommand,100,DEBUG);
    }
  }
}

void sendWebpage(String command)
{
  String webpage = command;               
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend +=webpage.length();
  cipSend +="\r\n";
  sendData(cipSend,100,DEBUG);
  sendData(webpage,500,DEBUG);
}
 
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
