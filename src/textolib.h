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
*  Waveshare_SIM7600.h by Kaloha
*  Copyright (C) Waveshare     April 27 2018
*  http://www.waveshare.com / http://www.waveshare.net
*/

#ifndef textolib_h
#define textolib_h

#include <stdlib.h>
#include <string.h>
#include <arduino.h>

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <wiring.h>
#endif


#include <SoftwareSerial.h>

#define RECORD_LENGTH 330

struct MenuItem {
  char* label;
  int id, action, size;
};

struct Message {
  int index;
  char* body;
  char* timestamp;
  char* sender_number;
};

/* texto Class */
class textolib {

public:
  int userkey;
  bool highSpeed;
  

  textolib();
  ~textolib();

  // SIM query
  void PowerOn(int PowerKey);

  // Check network status
  void Initialize(int timeout=2000);

  // Phone calls
  void PhoneCall(const char* PhoneNumber);

  // SMS sending and receiving message 
  bool SendingShortMessage(const char* PhoneNumber,const char* Message);

  // FTP download file to Module EFS or uploading EFS file to FTP
  void ConfigureFTP(const char* FTPServer,const char* FTPUserName,const char* FTPPassWord);
  void UploadToFTP(const char* FileName);
  void DownloadFromFTP(const char* FileName);

  // GPS positoning
  bool GPSPositioning();

  //TCP and UDP communication
//  bool PDPSetting(const char* APN);
//  bool TCPClientCM(const char* ServerIP,const char* Port,const char* Message,const char* MessageSize);  //TCP Client Command Mode
//  bool UDPServerCM(const char* ServerIP,const char* Port,const char* Message,const char* MessageSize);  //UDP Client Command Mode
//  bool TCPServerCM(const char* ServerIP,const char* Port,const char* Message,const char* MessageSize);  //TCP Client Command Mode

  // Other functions.
  uint8_t getMessages(const char* group, const char* expected_answer, Message** messagebuffer, unsigned int messagecount, unsigned int timeout);
  uint8_t getATcommandResponse(const char* ATcommand, const char* expected_answer, char* responseBuffer, unsigned int bufferLen, unsigned int timeout);
  uint8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout);
  uint8_t sendATcommand2(const char* ATcommand, const char* expected_answer1, const char* expected_answer2, unsigned int timeout);
};

extern textolib texto;

#endif

