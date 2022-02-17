/* FILE:    HCuOLED_Model_Railway_Arrivals_Display
   DATE:    24/01/22
   VERSION: 0.1
   AUTHOR: Chris Sharp
   
REVISIONS:

24/01/22 Created version 0.1


This is an example of how create a railway station arrivals board for a OO scale
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


// defining train status values as words to improve code readabilty
#define onTime 0
#define cancelled 1
#define arrived 2
#define noPlatform 3
#define delayed 4
#define bus 5

const byte NUMBER_OF_ARRIVALS = 6;
byte arrivalRows = 5;
byte nextArrival;

/* clock stuff */
byte h=10;    // the hour value the clock will start at
byte m=25;    // the minute value the clock will start at
byte s=45;    // the second value the clock will start at

/* Create an instance of the library for the display */
HCuOLED HCuOLED(SSD1306, CS_DI, DC_DI, RST_DI);

/* struct to contain the arival data for each of the train services */
struct TrainArrival
{
  byte minutes;
  byte platform;
  char *origin;
  byte state;  //  onTime, cancelled, delayed, noPlatform, arrived, bus
  byte hours;

  /* setup the hours value so it is in the next 60 minutes */
  void setup()
  {
    if (minutes > m)  hours = h;
    else              hours = h+1;
    if (hours==24)    hours=0;
  }
  
  void update()
  {
    byte randNumber;
    randNumber = random(10);      // random number to determine a trains status
    if (m == minutes-1)           // if train near arrival time randomly set it to arrived
    {
      if (state == onTime)
      { 
        if (randNumber < 5)
        {
          state = arrived;
        }
      }
    }
    if (state == noPlatform) state = onTime;    // ensure teh next arrival has a platform number

    /*  when a train has passed its arrival time set its state for the next service*/
    if (m == minutes+1) 
    {
      state = onTime;
      if(randNumber >= 6) state = noPlatform;
      if(randNumber == cancelled) state = cancelled;
      if(randNumber == bus) state = bus;
      if(randNumber == delayed) state = delayed;

      hours += 1;                           // increment hour once train has arrived
      if(hours == 24) hours = 0;
  
      nextArrival += 1;                     // increment next arrival once train has arrived
      if (nextArrival == 6) nextArrival = 0;
    }
  }
};


/* create the train arivals for each train: Minutes, Platform, Origin, State */
TrainArrival arrivals[NUMBER_OF_ARRIVALS] = {
  {13, 1, "Amblethorpe", delayed},
  {17, 4, "Blyth"},
  {26, 2, "Newcastle", cancelled},
  {44, 1, "Amblethorpe"},
  {54, 2, "Sheffield", noPlatform},
  {56, 3, "Nottingham", bus},
};



/* building an arrivals row of text in the buffer */
void displayArrival(byte row)
{
  const byte ROW_HEIGHT = 7;                  // set the height of a row
  byte train;
  train = row + nextArrival;                 // set the train which will appear on this row
  if (train >= NUMBER_OF_ARRIVALS)           // ensure that train is not greater than the number of arivals
  {
      train = train - NUMBER_OF_ARRIVALS;
  }
  row = row * ROW_HEIGHT;                     // set the vetical position of the row on the screen
  row = row + 30;                             // and push the row 30 pixels down to leave room for the clock
  HCuOLED.SetFont(sharpsharp_5pt);            // set the font 
  HCuOLED.Cursor(0, 22);                      // set text poistion for x and y axis 0, 0 is top left
  HCuOLED.Print("Arrivals");                  // Print the word arrivals to the buffer
  
  HCuOLED.Cursor(0, row);
  HCuOLED.Print(arrivals[train].origin);      // print the trains origin in the first column 

  HCuOLED.Cursor(64, row);
  if (arrivals[train].hours>9)                // check hours is a two digit value
  {
    HCuOLED.Print(arrivals[train].hours);    // print hours value into the display buffer
  } else if (arrivals[train].hours==0)       // a value of 0 will leave a blank space so check for it
  {
    HCuOLED.Print("00");                      // when hours is 0 print text rather than a blank space
  } else {
    HCuOLED.Print("0");                       // when hours is a single digit value print a leading zero
    HCuOLED.Cursor(68, row);                  // move cursor 4 pixels across to clear the leading zero
    HCuOLED.Print(arrivals[train].hours);
  }
  
  HCuOLED.Cursor(72, row);
  HCuOLED.Print(":");                         // print a colon to seperate hours and minutes

  HCuOLED.Cursor(74, row);
  if (arrivals[train].minutes>9)              // same process for minutes as hours to create leading zeros
  {
    HCuOLED.Print(arrivals[train].minutes);
  } else if (arrivals[train].minutes==0)
  {
    HCuOLED.Print("00");
  } else {
    HCuOLED.Print("0");
    HCuOLED.Cursor(79, row);
    HCuOLED.Print(arrivals[train].minutes);
  }
  
  /* if the train is not cancelled then print something in the platform column */
  if (arrivals[train].state != cancelled)     
  {
    HCuOLED.Cursor(88, row); 
    if (arrivals[train].state == noPlatform)
    {
      HCuOLED.Print("-");                      // print a dash if the platorm is unknown
    } else if (arrivals[train].state == bus)
    {
      HCuOLED.Cursor(85, row);
      HCuOLED.Print("Bus");                     // print bus for a replacement bus service
    } else
    {
      HCuOLED.Print(arrivals[train].platform);  // print platform number
    }
  }

  /* display the train state in the status column */
  if (arrivals[train].state == cancelled)
  {
    HCuOLED.Cursor(95, row);
    HCuOLED.Print("Cancelled");
  } else if (arrivals[train].state == arrived)
  {
    HCuOLED.Cursor(104, row);
    HCuOLED.Print("Arrived");
  } else if (arrivals[train].state == delayed)
  {
    HCuOLED.Cursor(100, row);
    HCuOLED.Print("Delayed");
  } else
  {
    HCuOLED.Cursor(100, row);
    HCuOLED.Print("On Time");
  }
}
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
  
   /* setup all of the arrivals */
  for (int i = 0; i < NUMBER_OF_ARRIVALS; i++)
  {
    arrivals[i].setup();
  }
  
  /* find the train that is due to arriva next */
  for (int i = 0; i < NUMBER_OF_ARRIVALS; i++)
  {
    if (arrivals[i].minutes > m) 
    {
      nextArrival = i;
      break;
    }  
  }
}
void loop() 
{ 
  HCuOLED.ClearBuffer();       // clear the buffer so it can be refilled
  for (int i = 0; i < arrivalRows; i++)
  {
    displayArrival(i);
  }
  displayClock();
  HCuOLED.Refresh();           // load the contents of the display buffer to the screen

  s=s+1;
  if(s==60)
  {
    s=0;
    m=m+1;
    arrivals[nextArrival].update();   //call update each minute
  }
  if(m==60)
  {
    m=0;
    h=h+1;
  }
  if(h==24) h=0;
 
 delay(1000);
}
