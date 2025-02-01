/*
 * clock.h library for system clock for IoT applications
 * Created by Sachi Gerlitz
 * 
 * 01-II-2025 Ver 2.1   [getVersion]
 * 01-IX-2024 ver 0.2   [redefine begin()]
 *
 */
#ifndef Clock_h
  #define Clock_h

  #ifndef Clock_Version
    #define Clock_Version "2.0.0"
  #endif  //Clock_Version
  
  #include  "Arduino.h"

  struct  TimePack {
    uint8_t       clockHour;
    uint8_t       clockMin;
    uint8_t       clockSec;
    bool          hourEdge;
    bool          minEdge;
    bool          secEdge;
    bool          secFlip;
    bool          NTPbeginOnce;       // set for startup, to configure time zone once
    bool          IsTimeSet;          // set if network time was obtained and updated
    unsigned long previousMillis;  
    unsigned long previousSecond;
    unsigned long OneSecondMicro;
    uint8_t       clockYear;
    uint8_t       clockMonth;
    uint8_t       clockDay;
    uint8_t       clockWeekDay;
    uint8_t       speed;              // should be 1, for debugging {1,10,50,100}
  };

  class Clock 
  {
    public:
      Clock(TimePack T);											// constructor
      TimePack  begin(TimePack T, uint8_t Multiplier);
      TimePack  MasterClock ( TimePack T );
      TimePack  computeDaychange ( TimePack T );
      TimePack  setDateFromDayOfYear ( TimePack T,uint16_t DayNum );
      uint16_t  ComputeDayOfYear ( uint8_t dayN, uint8_t monthN, bool lap ); 
      bool      IsThisLeapYear(uint16_t year);
      uint8_t   ComputeDayOfWeek (uint16_t Year, uint8_t Month, uint8_t Day);
      unsigned long StartStopwatch(void); 
      unsigned long ElapseStopwatch(unsigned long Prev); 
      void PrintElapsed(unsigned long Prev, char  *TimeMessage);
      const   char* getVersion();
    private:
      TimePack  _LocT;

  };

#endif   //Clock_h
