/* FILE:    HCuOLED_Model_Railway_Station_Departure_Baord
   DATE:    31/01/22
   VERSION: 0.1
   AUTHOR: Chris Sharp
   
REVISIONS:

31/01/22 Created version 0.1


This is an example of how create a railway station departures board for a OO scale
model railway with an uOLED displays using the HCuOLED 
library with a SH1106 based OLED display module such as:

Hobby Components 0.96" uOLED displays (HCMODU0050 & HCMODU0052)


This sketch will require three of the above displays. To use this example
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


MODULE2..........ARDUINO
GND.............GND
VCC.............3.3V
D0 (CLK)........D13
D1 (DATA).......D11
RST (RESET).....D5
DC..............D6
CS (SS).........D7


MODULE3..........ARDUINO
GND.............GND
VCC.............3.3V
D0 (CLK)........D13
D1 (DATA).......D11
RST (RESET).....D2
DC..............D3
CS (SS).........D4


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
/* Module 1 digital pins */
#define CS_DI1 10
#define DC_DI1 9
#define RST_DI1 8

/* Module 2 digital pins */
#define CS_DI2 7
#define DC_DI2 6 
#define RST_DI2 5

/* Module 3 digital pins */
#define CS_DI3 4 
#define DC_DI3 3 
#define RST_DI3 2

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

const byte NUMBER_OF_DEPARTURES = 6;
byte departureRows = 3;
byte nextDeparture;

/* clock stuff */
byte h=13;    // the hour value the clock will start at
byte m=12;    // the minute value the clock will start at
byte s=48;    // the second value the clock will start at

unsigned long nextSecond;

/* Create an instance of the library for each display */
HCuOLED HCuOLED1(SSD1306, CS_DI1, DC_DI1, RST_DI1);
HCuOLED HCuOLED2(SSD1306, CS_DI2, DC_DI2, RST_DI2);
HCuOLED HCuOLED3(SSD1306, CS_DI3, DC_DI3, RST_DI3);

/* struct to contain the depature data for each of the train services */
struct TrainDeparture
{
  byte minutes;
  byte platform;
  char *destination;
  char *stops;
  byte scroll;
  int xPos;
  byte hours;
  unsigned long scrollReset;
  unsigned long scrollWait;

  
  /* increment the hours and next departure values when the train has passed its depature time*/
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

/* create the train depatures for each train: Minutes, Platform, Destination, Stops, Scroll */
TrainDeparture departures[NUMBER_OF_DEPARTURES] = {
  {11, 1, "Sheffield", "Chesterfield and Sheffield"},
  {14, 3, "Nottingham", "Hucknall, Bullwell and Nottingham"},
  {28, 1, "Amblethorpe", "Barnmouth, Amblethorpe"},
  {40, 2, "Newcastle", "Sheffield, Leeds, York, Darlington and Newcastle", 80},
  {49, 4, "Blyth", "Ledston and Blyth"},
  {58, 1, "Amblethorpe", "Barnmouth, Amblethorpe"}
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

/* building a depatures row of text in the buffer */
void displayDeparture(byte row, int screen)
{
  const byte ROW_HEIGHT = 22;                 // set the height of a row for each train
  byte train;
  train = row + nextDeparture;                // set the train which will appear on this row
  if (screen == 2) train += 3;                // reset train if it on screen 2
  if (train >= NUMBER_OF_DEPARTURES)          // ensure that train is not greater than the number of depatures 
  {
      train = train - NUMBER_OF_DEPARTURES;
  }
  row = row * ROW_HEIGHT;                     // set the vetical position of the row on the screen
  HCuOLED1.SetFont(sharpsharp_6pt);           // set the font

  HCuOLED1.Cursor(0, row);                    // set text poistion for x and y axis 0, 0 is top left
  if (departures[train].hours>9)              // check hours is a two digit value
  {
    HCuOLED1.Print(departures[train].hours);  // print hours value into the display buffer
  } else if (departures[train].hours==0)      // a value of 0 will leave a blank space so check for it
  {
    HCuOLED1.Print("00");                     // when hours is 0 print text rather than a blank space
  } else {
    HCuOLED1.Print("0");                      // when hours is a single digit value print a leading zero
    HCuOLED1.Cursor(4, row);                  // move cursor 4 pixels across to clear the leading zero
    HCuOLED1.Print(departures[train].hours);
  }

  HCuOLED1.Cursor(9, row);
  HCuOLED1.Print(":");                        // print a colon to seperate hours and minutes
  
  HCuOLED1.Cursor(11, row);
  if (departures[train].minutes>9)           // same process for minutes as hours to create leading zeros
  {
    HCuOLED1.Print(departures[train].minutes);
  } else if (departures[train].minutes==0)
  {
    HCuOLED1.Print("00");
  } else {
    HCuOLED1.Print("0");
    HCuOLED1.Cursor(16, row);
    HCuOLED1.Print(departures[train].minutes);
  }

  HCuOLED1.Cursor(24, row);
  HCuOLED1.Print(departures[train].destination);
  
  HCuOLED1.Cursor(91, row); 
  HCuOLED1.Print(departures[train].platform);

  HCuOLED1.Cursor(100, row);
  HCuOLED1.Print("On Time");                  // print On Time at the end of the row

  HCuOLED1.SetFont(sharpsharp_5pt);           // set the font smaller
  HCuOLED1.Cursor(0,(row+8));                 // print below the main line of text
  HCuOLED1.Print("calling at");

  if (departures[train].scroll > 0)           // if the train has a scroll value the postion of the text needs updating
  {
    departures[train].scrollStops();          // the postion of the text is updating 
  }
  HCuOLED1.Cursor(departures[train].xPos, row+14); // print below calling at
  HCuOLED1.Print(departures[train].stops);
  
}
void displayArrival(byte row)
{
  const byte ROW_HEIGHT = 7;                 // set the height of a row
  byte train;
  train = row + nextArrival;               // set the train which will appear on this row
  if (train >= NUMBER_OF_ARRIVALS)          // ensure that train is not greater than the number of arivals
  {
      train = train - NUMBER_OF_ARRIVALS;
  }
  row = row * ROW_HEIGHT;                     // set the vetical position of the row on the screen
  row = row + 30;                             // and push the row 30 pixels down to leave room for the clock
  HCuOLED3.SetFont(sharpsharp_5pt);            // set the font 
  HCuOLED3.Cursor(0, 22);                      // set text poistion for x and y axis 0, 0 is top left
  HCuOLED3.Print("Arrivals");                  // Print the word arrivals to the buffer
  
  HCuOLED3.Cursor(0, row);
  HCuOLED3.Print(arrivals[train].origin);      // print the trains origin in the first column 

  HCuOLED3.Cursor(64, row);
  if (arrivals[train].hours>9)                // check hours is a two digit value
  {
    HCuOLED3.Print(arrivals[train].hours);    // print hours value into the display buffer
  } else if (arrivals[train].hours==0)       // a value of 0 will leave a blank space so check for it
  {
    HCuOLED3.Print("00");                      // when hours is 0 print text rather than a blank space
  } else {
    HCuOLED3.Print("0");                       // when hours is a single digit value print a leading zero
    HCuOLED3.Cursor(68, row);                  // move cursor 4 pixels across to clear the leading zero
    HCuOLED3.Print(arrivals[train].hours);
  }
  
  HCuOLED3.Cursor(72, row);
  HCuOLED3.Print(":");                         // print a colon to seperate hours and minutes

  HCuOLED3.Cursor(74, row);
  if (arrivals[train].minutes>9)              // same process for minutes as hours to create leading zeros
  {
    HCuOLED3.Print(arrivals[train].minutes);
  } else if (arrivals[train].minutes==0)
  {
    HCuOLED3.Print("00");
  } else {
    HCuOLED3.Print("0");
    HCuOLED3.Cursor(79, row);
    HCuOLED3.Print(arrivals[train].minutes);
  }
  
  /* if the train is not cancelled then print something in the platform column */
  if (arrivals[train].state != cancelled)
  {
    HCuOLED3.Cursor(88, row); 
    if (arrivals[train].state == noPlatform)
    {
      HCuOLED3.Print("-");                       // print a dash if the platorm is unknown
    } else if (arrivals[train].state == bus)
    {
      HCuOLED3.Cursor(85, row);
      HCuOLED3.Print("Bus");                     // print bus for a replacement bus service
    } else
    {
      HCuOLED3.Print(arrivals[train].platform);  // print platform number
    }
  }

  /* display the train state in the status column */
  if (arrivals[train].state == cancelled)
  {
    HCuOLED3.Cursor(95, row);
    HCuOLED3.Print("Cancelled");
  } else if (arrivals[train].state == arrived)
  {
    HCuOLED3.Cursor(104, row);
    HCuOLED3.Print("Arrived");
  } else if (arrivals[train].state == delayed)
  {
    HCuOLED3.Cursor(100, row);
    HCuOLED3.Print("Delayed");
  } else
  {
    HCuOLED3.Cursor(100, row);
    HCuOLED3.Print("On Time");
  }
}
/* building a clock text in the buffer */
void displayClock()
{
  byte col = 42;      // position of left edge of the clock from left side of screen in pixels
  byte row = 2;       // position of top edge of teh clock from the top of screen in pxels

  HCuOLED3.Rect(col-2, row-2, col+47, row+14, OUTLINE);  // draw am outline rectangle to create a clock border
  HCuOLED3.SetFont(MedProp_11pt);                        // set font for hours and minutes
  
  /* display hours   */
  HCuOLED3.Cursor(col, row);                             // set coorindates for the hours value
  if (h>9)                                              // check hours is a two digit value
  {
    HCuOLED3.Print(h);                                   // print hours value into the display buffer
  } else if (h==0)                                      // a value of 0 will leave a blank space so check for it
  {
    HCuOLED3.Print("00");                                // when hours is 0 print text rather than a blank space
  } else {
    HCuOLED3.Print("0");                                 // when hours is a single digit value print a leading zero
    HCuOLED3.Cursor(col+7, row);                         // move cursor 7 pixels across to clear the leading zero
    HCuOLED3.Print(h);
  }
  HCuOLED3.Cursor(col+16, row);                          // print a colon between hours and minutes
  HCuOLED3.Print(":");
  
  /* display minutes */
  HCuOLED3.Cursor(col+20, row);
  if (m>9)                          // same process for minutes as hours to ensure leading zeros are displayed
  {
    HCuOLED3.Print(m);     
  } else if (m==0)
  {
    HCuOLED3.Print("00");
  } else {
    HCuOLED3.Print("0");
    HCuOLED3.Cursor(col+27, row);
    HCuOLED3.Print(m);
  }

  /* display seconds */
  HCuOLED3.SetFont(sharpsharp_6pt); // set smaller font for seconds
  HCuOLED3.Cursor(col+37, row+6);
  if (s>9)                          // same process for seconds as hours to ensure leading zeros are displayed
  {
    HCuOLED3.Print(s);
  } else if (s==0)
  {
    HCuOLED3.Print("00");
  } else {
    HCuOLED3.Print("0");
    HCuOLED3.Cursor(col+41, row+6);
    HCuOLED3.Print(s);
  }
}
void setup() 
{
  /* Reset the display */
  /* uncomment the flip lines to run the screen up side down */
  HCuOLED1.Reset();
  //HCuOLED1.Flip_V();
  //HCuOLED1.Flip_H();
  
  HCuOLED2.Reset();
  //HCuOLED2.Flip_V();
  //HCuOLED2.Flip_H();
  
  HCuOLED3.Reset();
  //HCuOLED3.Flip_V();
  //HCuOLED3.Flip_H();


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
      break;         // when a train's departure minutes is greater than m break out of the for loop
    }  
  }
  
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
      break;         // when a train's arival minutes is greater than m break out of the for loop
    }  
  }
}
void loop() 
{ 
  for (int screen = 1; screen <= 2; screen++) // create drapture boards for screens 1 and 2
  {  
    HCuOLED1.ClearBuffer();                   // clear the buffer so it can be refilled
    for (int i = 0; i < departureRows; i++)
    {
      displayDeparture(i, screen);
    }
    if (screen == 1)  HCuOLED1.Refresh();     // load the contents of the display buffer to  screen 1
    else              HCuOLED2.Refresh();     // load the contents of the display buffer to  screen 2
  }


  HCuOLED3.ClearBuffer();                     // clear the buffer so it can be refilled
  for (int i = 0; i < arrivalRows; i++)
  {
    displayArrival(i);
  }
  displayClock();
  HCuOLED3.Refresh();                         // load the contents of the display buffer to  screen 2

  /*varibles for seconds minutes and hours updated to create the clock */
  if (millis() >= nextSecond)                 // wait for a second to pass
  {
    s=s+1;
    nextSecond = millis() + 1000;             // create another one second pause
  }
  
  if(s==60)
  {
    s=0;
    m=m+1;
    departures[nextDeparture].update();       //call departure update each minute
    arrivals[nextArrival].update();           //call arrival update each minute
  }
  if(m==60)
  {
    m=0;
    h=h+1;
  }
  if(h==24) h=0;
 
}
