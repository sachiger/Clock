/*
 * clock.cpp library for system clock for IoT applications
 * Created by Sachi Gerlitz
 * 
 * 01-II-2025 Ver 2.1   [getVersion]
 * 01-IX-2024 ver 0.2   [redefine begin()]
 * 28-VII-2024 ver 0.1
 *
 * in this file
 *  constructor:  Clock
 *  methods:      begin; MasterClock; computeDaychange; setDateFromDayOfYear;
 *                ComputeDayOfYear; IsThisLeapYear; ComputeDayOfWeek;
 *                StartStopwatch; ElapseStopwatch; PrintElapsed; getVersion;
 */

#include "Arduino.h"
#include "Clock.h"

// **************************************************************************************** //
Clock::Clock( TimePack T) {
  _LocT = T;
}     // end of Clock 

// **************************************************************************************** //
TimePack Clock::begin(TimePack T, uint8_t Multiplier){
  TimePack _LocT=T;
  _LocT.speed = Multiplier;               // {1,10,50,100}
  switch  (_LocT.speed) {
    case  1:
      _LocT.OneSecondMicro = 1000000L;     // run clock at x1 speed
      break;
    case  10:
      _LocT.OneSecondMicro = 100000L;      // run clock at x10 speed
      break;
    case  50:
      _LocT.OneSecondMicro = 20000L;       // run clock at x50 speed
      break;
    case  100:
      _LocT.OneSecondMicro = 10000L;       // run clock at x100 speed
      break;
    default:
      _LocT.OneSecondMicro = 1000000L;    // run clock at x1 speed
      break;
  }
  // set defaults
  _LocT.clockHour  = 23;
  _LocT.clockMin   = 30;
  _LocT.clockSec   = 0;
  _LocT.hourEdge   = false;
  _LocT.minEdge    = false;
  _LocT.secEdge    = false;
  _LocT.secFlip    = false;
  _LocT.previousMillis = 0;
  _LocT.clockYear      = 23;
  _LocT.clockMonth     = 2;
  _LocT.clockDay       = 28;
  _LocT.clockWeekDay   = 6;
  _LocT.speed          = 1;
  _LocT.NTPbeginOnce   = true;
  _LocT.IsTimeSet      = false;
  return  _LocT;
}     // end of begin 

// **************************************************************************************** //
TimePack Clock::MasterClock(TimePack T){
  /*
   * Procedure to count time and to set execution slot and edge flags
   */
  //static const char Mname[] PROGMEM = "MasterClock:";
  unsigned  long  currentMillis;
  TimePack _LocT=T;
  currentMillis = micros();
      
  // time count - second based
  //****************************
  if (currentMillis - _LocT.previousSecond >= _LocT.OneSecondMicro) {  // one second edge
    _LocT.previousSecond = _LocT.previousSecond + _LocT.OneSecondMicro ;
    _LocT.clockSec++;
    _LocT.secEdge  = true;
    _LocT.secFlip  = !_LocT.secFlip;
    _LocT.minEdge = false; _LocT.hourEdge = false;
  } else { 
    _LocT.secEdge = false; 
    _LocT.minEdge = false; 
    _LocT.hourEdge = false; 
  } // end of one second

  // increment counters
  //********************
  if ( _LocT.clockSec==60 ) {      // 1 min edge
     _LocT.clockSec  = 0;
     _LocT.clockMin++;
     _LocT.minEdge = true;
  }
    
  if ( _LocT.clockMin==60 ) {      // 1 hour edge
    _LocT.clockMin = 0;
    _LocT.clockHour++;
    _LocT.hourEdge = true;
  }
    
  if ( _LocT.clockHour==24 ) {     // 1 day edge
      _LocT.clockHour = 0;
      _LocT = computeDaychange(_LocT);  // advance date and week day
  }
  
  return  _LocT;

}      // end of MasterClock

// **************************************************************************************** //
TimePack  Clock::computeDaychange(TimePack T){
    /*
     * method to manualy advance date at 00:00:00
     * it compute the sequencial # of the day, advance and recompute the day from the new sequencial number
     */
    TimePack _LocT=T;
    uint16_t  dayNum;
    bool      leap = IsThisLeapYear(_LocT.clockYear+2000);
    dayNum = ComputeDayOfYear(_LocT.clockDay,_LocT.clockMonth,leap)+1;    // poit to tomorrow
    if ( (dayNum > 366) && leap ) { dayNum = 0; _LocT.clockYear++; } 
    else if ( dayNum > 365 )      { dayNum = 0; _LocT.clockYear++; } 
    _LocT = setDateFromDayOfYear(_LocT, dayNum);
    return  _LocT;
}     // end of computeDaychange

// **************************************************************************************** //
TimePack Clock::setDateFromDayOfYear(TimePack T, uint16_t dayNum) {
  /*
   * method to set the clock's month,day and week from <DayNum> {1-366} sequencial day # of the year
   * <clockDay>     {1,..,31}
   * <clockMonth>   {1,..,12}
   * <clockWeekDay> {0,..,6}
   */
  //static const char Mname[] PROGMEM = "ComputeDateFromDayOfYear:";
  static const uint16_t monthsTab   [13] = { 0, 31, 59, 90, 120,151,181,212,243,273,304,334,365 };
  //                                         31 28  31  30  31  30  31  31  30  31  30  31
  static const uint16_t monthsTabLeap[13] = { 0, 31, 60, 91, 121,152,182,213,244,274,305,335,366 };
  //                                          31 29  31  30  31  30  31  31  30  31  30  31
  //                                         Jan feb mar apr may jun Jul aug sep oct nov dec
  TimePack _LocT = T;
  _LocT.clockDay   = 31;
  _LocT.clockMonth = 12;
  for (uint8_t ii=1;ii<13;ii++) {                             // search month
    if ( IsThisLeapYear(_LocT.clockYear+2000) ) {             // for leap year
        if (monthsTabLeap[ii] >= dayNum){                     // month found
            _LocT.clockMonth = ii;
            _LocT.clockDay   = dayNum - monthsTabLeap[ii-1];
            break;
        }
    } else {                                                  // for regular year
        if ( monthsTab[ii] >= dayNum ) {                      // month found
            _LocT.clockMonth = ii;
            _LocT.clockDay   = dayNum - monthsTab[ii-1];
            break;
        }
    }   // end leap
  }     // end month search

  _LocT.clockWeekDay = ComputeDayOfWeek (_LocT.clockYear+2000,_LocT.clockMonth,_LocT.clockDay);
  return  _LocT;

}     // end of setDateFromDayOfYear

// **************************************************************************************** //
uint16_t Clock::ComputeDayOfYear(uint8_t dayN, uint8_t monthN, bool lap) {
  /*
   * method to compute the day (1-366) of the year by the date
   * <dayN> {1,31}
   * <monthN> {1,12}
   */
  //static const char Mname[] PROGMEM = "ComputeDayOfYear:";
  static const uint16_t monthsTab[12] =   {  0, 31, 59, 90, 120,151,181,212,243,273,304,334 };
  //                                         31 28  31  30  31  30  31  31  30  31  30  31
  //                                         Jan feb mar APr may jun Jul aug sep oct nov dec
  uint16_t number=1;
  if ( (dayN>0) && (dayN<32) && (monthN>0) && (monthN<13) ) {
    number = monthsTab[monthN-1] + uint16_t(dayN);
    if ( lap && (monthN>2) ) number++;            // correct all months after Feb    
  } else {
    number=0;                                     // error on day or on month
  }
  return  number;
}     // end of ComputeDayOfYear

// **************************************************************************************** //
bool  Clock::IsThisLeapYear(uint16_t year){
  /*
   * method to compute if a year <year> is a leap year {2000...}
   * returns true/false
   *
   * The algorithm to determine if a year is a leap year is as follows: 
   * Every year that is exactly divisible by four is a leap year, 
   * except for years that are exactly divisible by 100, but these centurial years are leap years, 
   * if they are exactly divisible by 400.
   */
  if ( year%100 == 0 )  return true;
  if ( year%4   == 0 )  return true;
  return  false;
}     // end of IsThisLeapYear

// **************************************************************************************** //
uint8_t Clock::ComputeDayOfWeek (uint16_t MyYear, uint8_t MyMonth, uint8_t MyDay){
  /*
   * method to compute day of the week from the date
   * returns the day number: Sunday=0
   */
  uint8_t day_of_week;
  
  day_of_week = (MyDay + int(2.6 * (((MyMonth + 12 - 3) % 12) + 1) - 0.2) - 40 + 
              (MyMonth < 3 ? MyYear-1 : MyYear) + int((MyMonth < 3 ? MyYear-1 : MyYear)/4) + 5) % 7;
  day_of_week--;
  if ( day_of_week==255 ) day_of_week = 6;

  return  day_of_week;
}   // end of ComputeDayOfWeek

// **************************************************************************************** //
unsigned long Clock::StartStopwatch ( ) {
  /*
   * method to measure elapsed time - start the count
   * returns start time signature
   */
  unsigned long localCurrentMillis = millis();
  return localCurrentMillis;  
}   // end of StartStopwatch

// **************************************************************************************** //
unsigned long Clock::ElapseStopwatch (unsigned long previousMillisIn) {
  /*
   * method to measure elapse time in mS
   * Returns the elapse time signature
   */
  unsigned long ElapseMillis = millis()-previousMillisIn;
  return ElapseMillis;    
}       // end of ElapseStopwatch

// **************************************************************************************** //
void Clock::PrintElapsed (unsigned long previousMillisIn, char  *TimeMessage) {
  /*
   * method to print elapse time with message
   */
  Serial.print(TimeMessage); Serial.print(F(" elapsed time="));
  Serial.print( ElapseStopwatch (previousMillisIn) ) ;
  Serial.print(F("mS. "));
}       // end of PrintElapsed 

// **************************************************************************************** //
const   char* Clock::getVersion() {
    /*
     * method to return the lib's version
     */
    return  Clock_Version;
}   // end of getVersion

// **************************************************************************************** //
