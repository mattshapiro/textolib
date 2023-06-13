/**
*  @filename   :   textolib.h
*  @brief      :   Communication functionality & data structures for Texto
*  @author     :   Matt Shapiro
*
*  Copyright (C) Matt Shapiro 5/16/2023
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documnetation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to  whom the Software is
* furished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*  Waveshare SIM7600 modem functoinality copied from:
*  Waveshare_SIM7600.cpp by Kaloha
*  Copyright (C) Waveshare     April 27 2018
*  http://www.waveshare.com / http://www.waveshare.net
*/

#include "textolib.h"

textolib::textolib(){
}

textolib::~textolib(){
}

SoftwareSerial SoftSerial(PB3,PB4);

/**************************Power on textolib**************************/
void textolib::PowerOn(int PowerKey){
   uint8_t answer = 0;
  SoftSerial.begin(9600);

  // checks if the module is started
  answer = sendATcommand("AT", "OK", 2000);
  if (answer == 0)
  {
    Serial.print("Starting up...\n");
    
    pinMode(PowerKey, OUTPUT);
    // power on pulse
    digitalWrite(PowerKey, HIGH);
    delay(500);
    digitalWrite(PowerKey, LOW);
  }

  delay(1500);
}

void textolib::Initialize(int timeout) {
    long start = millis();
    while ((millis() - start < timeout) && (sendATcommand("AT+IPREX=9600", "OK", 500)) == 0)
        delay(500);
    start = millis();
    while ((millis() - start < timeout) && ((sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || sendATcommand("AT+CREG?", "+CREG: 0,5", 500))) == 0)
        delay(500);
}

/**************************Phone Calls**************************
void textolib::PhoneCall(const char* PhoneNumber) {
  char aux_str[30];

  sprintf(aux_str, "ATD%s;", PhoneNumber);
  sendATcommand(aux_str, "OK", 10000);

  delay(20000);

  SoftSerial.println("AT+CHUP");            // disconnects the existing call
  Serial.print("Call disconnected\n");
}*/

/**************************SMS sending and receiving message **************************/
//SMS sending short message
bool textolib::SendingShortMessage(const char* PhoneNumber,const char* Message){
  uint8_t answer = 0;
  char aux_string[30];

  Serial.print("Setting SMS mode...\n");
  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  Serial.print("Sending Short Message\n");
    
  sprintf(aux_string,"AT+CMGS=\"%s\"", PhoneNumber);

  answer = sendATcommand(aux_string, ">", 3000);    // send the SMS number
  if (answer == 1)
  {
      SoftSerial.println(Message);
      SoftSerial.write(0x1A);
      answer = sendATcommand("", "OK", 20000);
      if (answer == 1)
      {
          Serial.print("Sent successfully \n"); 
          return true;   
      }
      else
      {
          Serial.print("error \n");
          return false;
      }
  }
  else
  {
 //     Serial.print(answer);
      Serial.print(" error.\n");
      return false;
  }
}

//SMS receiving short message
/*
bool textolib::ReceivingShortMessage(){
  uint8_t answer = 0;
  int i = 0;
  char RecMessage[200];

  Serial.print("Setting SMS mode...\n");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  sendATcommand("AT+CPMS=\"SM\",\"SM\",\"SM\"", "OK", 1000);    // selects the memory

    answer = sendATcommand("AT+CMGR=1", "+CMGR:", 2000);    // reads the first SMS

  if (answer == 1)
    {
        answer = 0;
        while(SoftSerial.available() == 0);
        // this loop reads the data of the SMS
        do{
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            if(SoftSerial.available() > 0){    
                RecMessage[i] = SoftSerial.read();
                i++;
                // check if the desired answer (OK) is in the response of the module
                if (strstr(RecMessage, "OK") != NULL)    
                {
                    answer = 1;
                }
            }
        }while(answer == 0);    // Waits for the asnwer with time out
        
 //       RecMessage[i] = '\0';
        
        Serial.print(RecMessage); 
        Serial.print("\n");      
        
    }
    else
    {
        Serial.print(answer);
        Serial.print(" error.\n");
    return false;
    }

  return true;
}*/

/**************************FTP download file to Module EFS , uploading EFS file to FTP**************************/
void textolib::ConfigureFTP(const char* FTPServer,const char* FTPUserName,const char* FTPPassWord){
    char aux_str[50];
  
    // sets the paremeters for the FTP server
  sendATcommand("AT+CFTPPORT=21", "OK", 2000);
  sendATcommand("AT+CFTPMODE=1", "OK", 2000);
  sendATcommand("AT+CFTPTYPE=A", "OK", 2000);

//  Serial.print(aux_str,"AT+CFTPSERV=\"%s\"", FTPServer);

    sprintf(aux_str,"AT+CFTPSERV=\"%s\"", FTPServer);
    sendATcommand(aux_str, "OK", 2000);
    
    sprintf(aux_str, "AT+CFTPUN=\"%s\"", FTPUserName);
    sendATcommand(aux_str, "OK", 2000);
    sprintf(aux_str, "AT+CFTPPW=\"%s\"", FTPPassWord);
    sendATcommand(aux_str, "OK", 2000);
}

void textolib::UploadToFTP(const char* FileName){
  char aux_str[50];

  Serial.print("Upload file to FTP...\n");
  sprintf(aux_str, "AT+CFTPPUTFILE=\"%s\",0", FileName);
  sendATcommand(aux_str, "OK", 2000);
}

void textolib::DownloadFromFTP(const char* FileName){
  char aux_str[50];
  
  Serial.print("Download file from FTP...\n");
  sprintf(aux_str, "AT+CFTPGETFILE=\"%s\",0", FileName);
    sendATcommand(aux_str, "OK", 2000);
}

/**************************GPS positoning**************************/
bool textolib::GPSPositioning(){

    uint8_t answer = 0;
    bool RecNull = true;
    int i = 0;
    char RecMessage[200];
    char LatDD[3],LatMM[10],LogDD[4],LogMM[10],DdMmYy[7] ,UTCTime[7];
    int DayMonthYear;
    float Lat = 0;
    float Log = 0;
    
    memset(RecMessage, '\0', 200);    // Initialize the string
    memset(LatDD, '\0', 3);    // Initialize the string
    memset(LatMM, '\0', 10);    // Initialize the string
    memset(LogDD, '\0', 4);    // Initialize the string
    memset(LogMM, '\0', 10);    // Initialize the string
    memset(DdMmYy, '\0', 7);    // Initialize the string
    memset(UTCTime, '\0', 7);    // Initialize the string

    Serial.print("Start GPS session...\n");
    sendATcommand("AT+CGPS=1,1", "OK", 1000);    // start GPS session, standalone mode

    delay(2000);

    while(RecNull)
    {
        answer = sendATcommand("AT+CGPSINFO", "+CGPSINFO: ", 1000);    // start GPS session, standalone mode

        if (answer == 1)
        {
            answer = 0;
            while(SoftSerial.available() == 0);
            // this loop reads the data of the GPS
            do{
                // if there are data in the UART input buffer, reads it and checks for the asnwer
                if(SoftSerial.available() > 0){    
                    RecMessage[i] = SoftSerial.read();
                    i++;
                    // check if the desired answer (OK) is in the response of the module
                    if (strstr(RecMessage, "OK") != NULL)    
                    {
                        answer = 1;
                    }
                }
            }while(answer == 0);    // Waits for the asnwer with time out
            
            RecMessage[i] = '\0';
            Serial.print(RecMessage);
            Serial.print("\n");

            if (strstr(RecMessage, ",,,,,,,,") != NULL) 
            {
                memset(RecMessage, '\0', 200);    // Initialize the string
                i = 0;
                answer = 0;
                delay(1000);
            }
            else
            {
                RecNull = false;
                sendATcommand("AT+CGPS=0", "OK:", 1000);
            } 
        }
        else
        {
            Serial.print("error \n");
            return false;
        }
        delay(2000);

    }

    strncpy(LatDD,RecMessage,2);
    LatDD[2] = '\0';
//    Serial.print("LatDD:");
//    Serial.print(LatDD);
    
    strncpy(LatMM,RecMessage+2,9);
    LatMM[9] = '\0';
//    Serial.print(" LatMM:");
//    Serial.print(LatMM);
    
    Lat = atoi(LatDD) + (atof(LatMM)/60);
    if(RecMessage[12] == 'N')
    {
        Serial.print("Latitude is ");    
        Serial.print(Lat);  
        Serial.print(" N\n");   
      }
    else if(RecMessage[12] == 'S')
    {
        Serial.print("Latitude is ");    
        Serial.print(Lat);
        Serial.print(" S\n");    
      }
    else
        return false;

    strncpy(LogDD,RecMessage+14,3);
    LogDD[3] = '\0';
//    Serial.print("LogDD:");
//    Serial.print(LogDD);
    
    strncpy(LogMM,RecMessage+17,9);
    LogMM[9] = '\0';
//    Serial.print("LogMM:");
//    Serial.print(LogMM);
    
    Log = atoi(LogDD) + (atof(LogMM)/60);
    if(RecMessage[27] == 'E')
    {
        Serial.print("Longitude is "); 
        Serial.print(Log);
        Serial.print(" E\n");
      }
    else if(RecMessage[27] == 'W')
    {
        Serial.print("Latitude is ");
        Serial.print(Lat);
        Serial.print(" W\n");      
      }
    else
        return false;

    strncpy(DdMmYy,RecMessage+29,6);
    DdMmYy[6] = '\0';
    Serial.print("Day Month Year is ");
    Serial.print(DdMmYy);
    Serial.print("\n");
    
    strncpy(UTCTime,RecMessage+36,6);
    UTCTime[6] = '\0';
    Serial.print("UTC time is ");
    Serial.print(UTCTime);
    Serial.print("\n");
    
    return true;
}

/**************************Other functions**************************/
Message parseMessages(char* in) {
    if(in != NULL) {

    }
}

// populate list of messages based on response of AT+CGML="ALL"
// this will probably leak if called more than once...
uint8_t textolib::getMessages(const char* group, const char* expected_answer, Message** messagebuffer, unsigned int messagecount, unsigned int timeout) {
    
    int previous,
        answer = 0,
        x = 0, msgindex = 0;

    char * in;
    char next;

    in = (char*)malloc(RECORD_LENGTH * sizeof(char));

    if(messagebuffer == NULL) {
        messagebuffer = (Message**)malloc(sizeof(Message) * messagecount); // allocate message list
    }

    while( SoftSerial.available() > 0) SoftSerial.read();    // Clean the input buffer

    SoftSerial.println("AT+CMGL=\"ALL\"");    // Send the AT command    
    
    previous = millis();
    
    do{
        if(SoftSerial.available() > 0){    
            next = SoftSerial.read();
            if(strstr(in, "+CMGL") && x > 12) {
                Message *msg = new Message();
                msg->index = msgindex;
                msg->body = (char*)malloc(RECORD_LENGTH * sizeof(char));
                strcpy(msg->body, in);
                x = 0;
                messagebuffer[msgindex] = msg;
                msgindex++;
                Serial.print("new record [");
                Serial.print(msg->index);
                Serial.print("] = ");
                Serial.print(msg->body);
            } else {
                x++;
            }
            in[x] = next;
            in[x+1] = '\0';
            if((answer == 0) && (strstr(in, expected_answer) != NULL)) {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }while(millis() - previous < timeout);

    Serial.println("Message dump");
    for(int i = 0; i < msgindex; i++) {
        Serial.print("Message ");
        Serial.print(messagebuffer[i]->index);
        Serial.print("= ");
        Serial.println(messagebuffer[i]->body);
    }
    
    free(in);

    return answer;
}

// copy response string into passed char buffer & return answer = 1 if (respnse == expected_answer)
uint8_t textolib::getATcommandResponse(const char* ATcommand, const char* expected_answer, char* responseBuffer, unsigned int bufferLen, unsigned int timeout) {
    uint8_t x=0,  answer=0;
    unsigned long previous;
    char in = NULL;

    memset(responseBuffer, '\0', bufferLen);    // Initialize the string

    while( SoftSerial.available() > 0) SoftSerial.read();    // Clean the input buffer

    SoftSerial.println(ATcommand);    // Send the AT command    

    x = 0;
    previous = millis();
 
 // this loop waits for the answer
    do{
        if(SoftSerial.available() > 0){    
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            in = SoftSerial.read();
            if(x < bufferLen) {
                responseBuffer[x++] = in;
                if(strstr(responseBuffer, expected_answer) != NULL) {
                    answer = 1;
                }
            }
        }
         // Waits for the asnwer with time out
    }while((in != '\0') && ((millis() - previous) < timeout));
    Serial.print("message response: ");
    Serial.println(responseBuffer);
    return answer;
}
uint8_t textolib::sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout) {

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;
    bool ascii = true;
    char ch[2];

    memset(response, '\0', 100);    // Initialize the string
    
    while( SoftSerial.available() > 0) SoftSerial.read();    // Clean the input buffer
    //int tes = freeMemory();
    SoftSerial.println(ATcommand);    // Send the AT command 

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(SoftSerial.available() > 0){    
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = SoftSerial.read(); 
            if(ascii) {
                ascii = isAscii(response[x]);
            }     
            //Serial.println(response[x]);
            x++;
            // check if the desired answer  is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
                response[x] = '\0';
                x++;
            }
        }
         // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));
   
    Serial.print("response =");
    Serial.print(response);
    Serial.print(" \ answer =");
    itoa(answer,ch,10);
    Serial.print(ch);
    Serial.print("\n");  

    return answer;
}

uint8_t textolib::sendATcommand2(const char* ATcommand, const char* expected_answer1, const char* expected_answer2, unsigned int timeout){
  uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);

    while( SoftSerial.available() > 0) SoftSerial.read();    // Clean the input buffer

    SoftSerial.println(ATcommand);    // Send the AT command 

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(SoftSerial.available() != 0){    
            response[x] = SoftSerial.read();
            Serial.print(response[x]);
            x++;
            // check if the desired answer 1  is in the response of the module
            if (strstr(response, expected_answer1) != NULL)    
            {
                Serial.print("\n");
                answer = 1;
            }
            // check if the desired answer 2 is in the response of the module
            else if (strstr(response, expected_answer2) != NULL)    
            {
                Serial.print("\n");
                answer = 2;
            }
        }
    }
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;

}

textolib texto = textolib();



