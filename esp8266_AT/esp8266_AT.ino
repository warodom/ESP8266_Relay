#include <SoftwareSerial.h>
 
#define DEBUG true
 
SoftwareSerial esp8266(3,4); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

String IP,IP_A,IP_B;
char IP_Buffer;
String ssid = "Pi_AP";
String pass = "Raspberry";
int connectionId,Count_loop,Split;

void setup()
{
  Serial.begin(19200);
  esp8266.begin(19200); // your esp's baud rate might be different

  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);

  pinMode(7,OUTPUT);
  digitalWrite(7,LOW);
  
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  //sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CWJAP=\"" + ssid + "\",\"" + pass + "\"\r\n",4000,DEBUG);
  IP = sendData("AT+CIFSR\r\n",100,DEBUG); // get ip address 
  sendData("AT+CIPMUX=1\r\n",100,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",100,DEBUG); // turn on server on port 80
  for(Count_loop = 24; Count_loop < (IP.length() - 4); Count_loop++) {
    //if(IP[Count_loop] != " ") {
      IP_A = IP_A + IP[Count_loop];
    //}
  }
  Serial.print("IP_A : " + IP_A);
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
       if(control_led) {digitalWrite(13,HIGH); digitalWrite(7,HIGH);}
       else {digitalWrite(13,LOW); digitalWrite(7,LOW);}    
       
       sendWebpage("<!DOCTYPE html>");
       sendWebpage("<html><head><meta name=\"viewport\"content=\"width=device-width,initial-seale=1\"><title>ESP8266</title></head>");
       sendWebpage("<body bgcolor=\"#FFFFCC\">&nbsp;<table width=\"80%\" border=\"0\"align=\"center\"cellpadding=\"0\"cellspacing=\"0\">");
       sendWebpage("<tr><td width=\"10%\"bgcolor=\"#00CCFF\"></td><td width=\"90%\"height=\"80\"bgcolor=\"#00CCFF\"><h1>ESP8266 Smart Control</h1></td></tr></table>");
       sendWebpage("<table width=\"80%\"border=\"0\"align=\"center\"cellpadding=\"0\"cellspacing=\"0\">");
       sendWebpage("<tr><td height=\"80\"bgcolor=\"#00FF99\"align=\"center\">&nbsp;");
       sendWebpage("<button style=\"width:200px;height:50px\"onClick=\"window.location='/status/1';\"><h3>ON</h3></button></td></tr>");
       sendWebpage("<tr><td height=\"80\"bgcolor=\"#00FF99\"align=\"center\">&nbsp;");
       sendWebpage("<button style=\"width:200px;height:50px\"onClick=\"window.location='/status/0';\"><h3>OFF</h3></button></td></tr></table>");
       sendWebpage("</body></html>");
     }
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendData(closeCommand,50,DEBUG);
    //}
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
  sendData(cipSend,60,DEBUG);
  sendData(webpage,200,0);
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
