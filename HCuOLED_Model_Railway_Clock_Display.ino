/* FILE:    HCuOLED_Model_Railway_Clock_Display
   DATE:    21/01/22
   VERSION: 0.1
   AUTHOR: Chris Sharp
   
REVISIONS:

21/01/22 Created version 0.1


This is an example of how create a railway station clock for a OO scale
model railway with an uOLED displays using the HCuOLED 
library with a SH1106 based OLED display module such as:

Hobby Components 0.96" uOLED displays (HCMODU0050 & HCMODU0052)


This sketch will require one of the above displays. To use this example
connect your uOLED displays to the following pins of your 
Arduino:


MODULE1..........ARDUINO
GND.............GND
VCC.............3.3V
D0 (CLK)........D13 
D1 (DATA).......D11
RST (RESET).....D8
DC..............D9
CS (SS).........D10


You may copy, alter and reuse this code in any way you like, but please leave
reference to HobbyComponents.com in your comments if you redistribute this code.
This software may not be used directly for the purpose of promoting products that
directly compete with Hobby Components Ltd's own range of products.

THIS SOFTWARE IS PROVIDED "AS IS". HOBBY COMPONENTS MAKES NO WARRANTIES, 
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ACCURACY OR
LACK OF NEGLIGENCE. HOBBY COMPONENTS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE
FOR ANY DAMAGES INCLUDING, BUT NOT LIMITED TO, SPECIAL, INCIDENTAL OR 
CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER. */

#include "HCuOLED.h"
#include "SPI.h"

 // These connect to the CS, DC, and RST pins on the display.

#define CS_DI 10
#define DC_DI 9
#define RST_DI 8

/* clock stuff */
byte h=10;    // the hour value the clock will start at
byte m=23;    // the minute value the clock will start at
byte s=45;    // the second value the clock will start at

/* Create an instance of the library for the display */
HCuOLED HCuOLED(SSD1306, CS_DI, DC_DI, RST_DI);


/* building a clock text in the buffer */
void displayClock()
{
  byte col = 42;      // position of left edge of the clock from left side of screen in pixels
  byte row = 2;       // position of top edge of teh clock from the top of screen in pxels

  HCuOLED.Rect(col-2, row-2, col+47, row+14, OUTLINE);  // draw am outline rectangle to create a clock border
  HCuOLED.SetFont(MedProp_11pt);                        // set font for hours and minutes
  
  /* display hours   */
  HCuOLED.Cursor(col, row);                             // set coorindates for the hours value
  if (h>9)                                              // check hours is a two digit value
  {
    HCuOLED.Print(h);                                   // print hours value into the display buffer
  } else if (h==0)                                      // a value of 0 will leave a blank space so check for it
  {
    HCuOLED.Print("00");                                // when hours is 0 print text rather than a blank space
  } else {
    HCuOLED.Print("0");                                 // when hours is a single digit value print a leading zero
    HCuOLED.Cursor(col+7, row);                         // move cursor 7 pixels across to clear the leading zero
    HCuOLED.Print(h);
  }
  HCuOLED.Cursor(col+16, row);                          // print a colon between hours and minutes
  HCuOLED.Print(":");
  
  /* display minutes */
  HCuOLED.Cursor(col+20, row);
  if (m>9)                    // same process for minutes as hours to ensure leading zeros are displayed
  {
    HCuOLED.Print(m);     
  } else if (m==0)
  {
    HCuOLED.Print("00");
  } else {
    HCuOLED.Print("0");
    HCuOLED.Cursor(col+27, row);
    HCuOLED.Print(m);
  }

  /* display seconds */
  HCuOLED.SetFont(sharpsharp_6pt);  // set smaller font for seconds
  HCuOLED.Cursor(col+37, row+6);
  if (s>9)                          // same process for seconds as hours to ensure leading zeros are displayed
  {
    HCuOLED.Print(s);
  } else if (s==0)
  {
    HCuOLED.Print("00");
  } else {
    HCuOLED.Print("0");
    HCuOLED.Cursor(col+41, row+6);
    HCuOLED.Print(s);
  }
}

void setup() 
{
  /* Reset the display */
  HCuOLED.Reset();
}
void loop() 
{ 
  HCuOLED.ClearBuffer();    // clear the buufer proir to building the clock for display
  displayClock();
  HCuOLED.Refresh();        //load the contents of the display buffer to the screen

  /*varibles for seconds minutes and hours updated to create the clock */
  s=s+1;
  if(s==60)
  {
    s=0;
    m=m+1; 
  }
  if(m==60)
  {
    m=0;
    h=h+1;
  }
  if(h==24) h=0;
 
 delay(1000);     // delay in millseconds. 1000 is a second. Reduce to create a fast clock.
}
