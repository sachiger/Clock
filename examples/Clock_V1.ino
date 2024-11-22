/*
 * program to test Clock lib
 *
 * V1 11-V-2024
 */

#include <Clock.h>
TimePack  SysClock;                 // clock data
Clock     RunClock(SysClock);       // clock instance

#define   BAUDRATE        115200
char      daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
unsigned long StartTime;

void setup()
{
  Serial.begin(BAUDRATE);     // Serial monitor setup
  delay(500);
  
  Serial.print("\n\n\nProgram started\n"); 

  SysClock.clockHour  = 23;
  SysClock.clockMin   = 30;
  SysClock.clockSec   = 0;
  SysClock.hourEdge   = false;
  SysClock.minEdge    = false;
  SysClock.secEdge    = false;
  SysClock.secFlip    = false;
  SysClock.previousMillis = 0;
  SysClock.clockYear      = 23;
  SysClock.clockMonth     = 2;
  SysClock.clockDay       = 28;
  SysClock.clockWeekDay   = 6;
  SysClock.speed          = 1;
  SysClock = RunClock.begin(SysClock);

  StartTime = RunClock.StartStopwatch();
}

void loop()
{
  SysClock = RunClock.MasterClock(SysClock);
  if ( SysClock.secEdge ) {
    Serial.print("\n time "); 
    Serial.print(SysClock.clockHour); Serial.print(":");
    Serial.print(SysClock.clockMin); Serial.print(":");
    Serial.print(SysClock.clockSec); Serial.print(" ");
    Serial.print(SysClock.clockDay); Serial.print("/");
    Serial.print(SysClock.clockMonth); Serial.print("/20");
    Serial.print(SysClock.clockYear); Serial.print(" day of week ");
    Serial.print(daysOfTheWeek[SysClock.clockWeekDay]); Serial.print(" ");
    
    RunClock.PrintElapsed(StartTime, "period");
    StartTime = RunClock.StartStopwatch();

    SysClock.secEdge = false;
  }
}