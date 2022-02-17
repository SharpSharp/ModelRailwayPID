/* FILE:    HCuOLED_Model_Railway_Detailed_Departures
   DATE:    26/01/22
   VERSION: 0.1
   AUTHOR: Chris Sharp
   
REVISIONS:

26/01/22 Created version 0.1


This is an example of how create a railway station departures board for a OO scale
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


const byte NUMBER_OF_DEPARTURES = 6;
byte departureRows = 3;
byte nextDeparture;

/* clock stuff */
byte h=13;    // the hour value the clock will start at
byte m=12;    // the minute value the clock will start at
byte s=48;    // the second value the clock will start at

unsigned long nextSecond;

/* Create an instance of the library for the display */
HCuOLED HCuOLED(SSD1306, CS_DI, DC_DI, RST_DI);


/* struct to contain the depature data for each of the train services */
struct TrainDeparture
{
  byte minutes;
  byte platform;
  char *destination;
  char *stops;                // stations train will stop at
  byte scroll;                // number of pixels the stops scoll when requires
  int xPos;                   // position of scrolling text
  byte hours;
  unsigned long scrollReset;  // control pause on scrolling
  unsigned long scrollWait;   // control speed of scrolling
  
  /* setup the hours value so it is in the next 60 minutes */
  void setup()
  {
    if (minutes > m)  hours = h;
    else              hours = h+1;
    if (hours==24)    hours=0;
  }

  /* increment the hours and next departure values when the train has passed its depature time*/
  void update()
  {
    if (minutes == m) 
    {
      hours += 1;
      if(hours == 24) hours = 0;
      nextDeparture += 1;
      if (nextDeparture == 6) nextDeparture = 0;
    }
  }
  /* control the position of scrolling text for trains with many station stops */
  void scrollStops()
  {
    if (millis() > scrollWait)          // pause until the scrollWait time is reached
    {        
      scrollWait = millis() + 100;      // set the scrollWait time to 100 milliseconds ahead
      if (xPos > -scroll)               // if the text position hasn't scrolled far enough
      {
        xPos -= 1;                      // move the the text position one pixel left
        scrollReset = millis() + 2000;  // set the scrollReset time 2000 milliseconds ahead
      } else                            // when the scrolling has readed its limit
      {
        if (millis() > scrollReset)     // pause until the scrollReset time is reached
        {
          xPos = 0;                     // reset the text position back to 0 
          scrollWait = millis() + 2000; // set the scrollWait time to 2000 milliseconds ahead
        }
      }
    } 
  }
};


/* create the train depatures for each train: Minutes, Platform, Destination, Stops, Scroll */
TrainDeparture departures[NUMBER_OF_DEPARTURES] = {
  {11, 1, "Sheffield", "Chesterfield and Sheffield"},
  {14, 3, "Nottingham", "Hucknall, Bullwell and Nottingham"},
  {28, 1, "Amblethorpe", "Barnmouth, Amblethorpe"},
  {40, 2, "Newcastle", "Sheffield, Leeds, York, Darlington and Newcastle", 80},
  {49, 4, "Blyth", "Ledston and Blyth"},
  {58, 1, "Amblethorpe", "Barnmouth, Amblethorpe"}
};

/* building a depatures row of text in the buffer */
void displayDeparture(byte row)
{
  const byte ROW_HEIGHT = 22;                 // set the height of a row for each train
  byte train;
  train = row + nextDeparture;                // set the train which will appear on this row
  if (train >= NUMBER_OF_DEPARTURES)          // ensure that train is not greater than the number of depatures
  {
      train = train - NUMBER_OF_DEPARTURES;
  }
  row = row * ROW_HEIGHT;                     // set the vetical position of the row on the screen
  HCuOLED.SetFont(sharpsharp_6pt);            // set the font 

  HCuOLED.Cursor(0, row);                     // set text poistion for x and y axis 0, 0 is top left
  if (departures[train].hours>9)              // check hours is a two digit value
  {
    HCuOLED.Print(departures[train].hours);   // print hours value into the display buffer
  } else if (departures[train].hours==0)      // a value of 0 will leave a blank space so check for it
  {
    HCuOLED.Print("00");                      // when hours is 0 print text rather than a blank space
  } else {
    HCuOLED.Print("0");                       // when hours is a single digit value print a leading zero
    HCuOLED.Cursor(4, row);                   // move cursor 4 pixels across to clear the leading zero
    HCuOLED.Print(departures[train].hours);
  }

  HCuOLED.Cursor(9, row);
  HCuOLED.Print(":");                         // print a colon to seperate hours and minutes
  
  HCuOLED.Cursor(11, row);
  if (departures[train].minutes>9)            // same process for minutes as hours to create leading zeros
  {
    HCuOLED.Print(departures[train].minutes);
  } else if (departures[train].minutes==0)
  {
    HCuOLED.Print("00");
  } else {
    HCuOLED.Print("0");
    HCuOLED.Cursor(16, row);
    HCuOLED.Print(departures[train].minutes);
  }

  HCuOLED.Cursor(24, row);
  HCuOLED.Print(departures[train].destination);
  
  HCuOLED.Cursor(91, row); 
  HCuOLED.Print(departures[train].platform);

  HCuOLED.Cursor(100, row);
  HCuOLED.Print("On Time");                   // print On Time at the end of the row

  HCuOLED.SetFont(sharpsharp_5pt);            // set the font smaller
  HCuOLED.Cursor(0,(row+8));                  // print below the main line of text
  HCuOLED.Print("calling at");

  if (departures[train].scroll > 0)           // if the train has a scroll value the postion of the text needs updating 
  {
    departures[train].scrollStops();          // the postion of the text is updating 
  }
  HCuOLED.Cursor(departures[train].xPos, row+14); // print below calling at
  HCuOLED.Print(departures[train].stops);
}

void setup() 
{
  /* Reset the display */
  HCuOLED.Reset();
   /* setup all of the depatures */
  for (int i = 0; i < NUMBER_OF_DEPARTURES; i++)
  {
    departures[i].setup();
  }
  /* find the train that is due to depart next */
  for (int i = 0; i < NUMBER_OF_DEPARTURES; i++)
  {
    if (departures[i].minutes > m) 
    {
      nextDeparture = i;
      break;         // when a train's minutes is greater than m break out of the for loop
    }  
  }
}
void loop() 
{ 
  HCuOLED.ClearBuffer();          // clear the buffer so it can be refilled
  for (int i = 0; i < departureRows; i++)
  {
    displayDeparture(i);
  }
  HCuOLED.Refresh();              // load the contents of the display buffer to the screen

  /*varibles for seconds minutes and hours updated to create the clock */
  if (millis() >= nextSecond)     // wait for a second to pass
  {
    s=s+1;
    nextSecond = millis() + 1000;  // create another one second pause
  }
  
  if(s==60)
  {
    s=0;
    m=m+1;
    departures[nextDeparture].update();   //call update each minute
  }
  if(m==60)
  {
    m=0;
    h=h+1;
  }
  if(h==24) h=0;
 
}
