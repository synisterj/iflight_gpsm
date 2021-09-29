#ifndef __iFlightGPSLib_h
#define __iFlightGPSLib_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#define printByte(args) write(args);
#else
#include "WProgram.h"
#define printByte(args) print(args,BYTE);
#endif

#include <limits.h>

#define _GPS_LIB_NAME "iFlightGPSLib"
#define _GPS_LIB_VERSION "1.1.2"
#define _GPS_LIB_AUTHOR "J. Edelbrock"
#define _GPS_LIB_AUTHOR_EMAIL "synroot@synisterj.com"
#define _GPS_LIB_AUTHOR_SITE "https://www.synisterj.com"
#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001
#define _GPS_FEET_PER_METER 3.2808399
#define _GPS_MAX_FIELD_SIZE 15

struct RawDegrees
{
   uint16_t deg;
   uint32_t billionths;
   bool negative;
public:
   RawDegrees() : deg(0), billionths(0), negative(false)
   {}
};

struct iFlightGPSLocation
{
   friend class iFlightGPSLib;
public:
   bool isValid() const    { return valid; }
   bool isUpdated() const  { return updated; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   const RawDegrees &rawLat()     { updated = false; return rawLatData; }
   const RawDegrees &rawLng()     { updated = false; return rawLngData; }
   double lat();
   double lng();

   iFlightGPSLocation() : valid(false), updated(false)
   {}

private:
   bool valid, updated;
   RawDegrees rawLatData, rawLngData, rawNewLatData, rawNewLngData;
   uint32_t lastCommitTime;
   void commit();
   void setLatitude(const char *term);
   void setLongitude(const char *term);
};

struct iFlightGPSDate
{
   friend class iFlightGPSLib;
public:
   bool isValid() const       { return valid; }
   bool isUpdated() const     { return updated; }
   uint32_t age() const       { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }

   uint32_t value()           { updated = false; return date; }
   uint16_t year();
   uint8_t month();
   uint8_t day();

   iFlightGPSDate() : valid(false), updated(false), date(0)
   {}

private:
   bool valid, updated;
   uint32_t date, newDate;
   uint32_t lastCommitTime;
   void commit();
   void setDate(const char *term);
};

struct iFlightGPSTime
{
   friend class iFlightGPSLib;
public:
   bool isValid() const       { return valid; }
   bool isUpdated() const     { return updated; }
   uint32_t age() const       { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }

   uint32_t value()           { updated = false; return time; }
   uint8_t hour();
   uint8_t minute();
   uint8_t second();
   uint8_t centisecond();

   iFlightGPSTime() : valid(false), updated(false), time(0)
   {}

private:
   bool valid, updated;
   uint32_t time, newTime;
   uint32_t lastCommitTime;
   void commit();
   void setTime(const char *term);
};

struct iFlightGPSDecimal
{
   friend class iFlightGPSLib;
public:
   bool isValid() const    { return valid; }
   bool isUpdated() const  { return updated; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   int32_t value()         { updated = false; return val; }

   iFlightGPSDecimal() : valid(false), updated(false), val(0)
   {}

private:
   bool valid, updated;
   uint32_t lastCommitTime;
   int32_t val, newval;
   void commit();
   void set(const char *term);
};

struct iFlightGPSInteger
{
   friend class iFlightGPSLib;
public:
   bool isValid() const    { return valid; }
   bool isUpdated() const  { return updated; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   uint32_t value()        { updated = false; return val; }

   iFlightGPSInteger() : valid(false), updated(false), val(0)
   {}

private:
   bool valid, updated;
   uint32_t lastCommitTime;
   uint32_t val, newval;
   void commit();
   void set(const char *term);
};

struct iFlightGPSSpeed : iFlightGPSDecimal
{
   double knots()    { return value() / 100.0; }
   double mph()      { return _GPS_MPH_PER_KNOT * value() / 100.0; }
   double mps()      { return _GPS_MPS_PER_KNOT * value() / 100.0; }
   double kmph()     { return _GPS_KMPH_PER_KNOT * value() / 100.0; }
};

struct iFlightGPSCourse : public iFlightGPSDecimal
{
   double deg()      { return value() / 100.0; }
};

struct iFlightGPSAltitude : iFlightGPSDecimal
{
   double meters()       { return value() / 100.0; }
   double miles()        { return _GPS_MILES_PER_METER * value() / 100.0; }
   double kilometers()   { return _GPS_KM_PER_METER * value() / 100.0; }
   double feet()         { return _GPS_FEET_PER_METER * value() / 100.0; }
};

struct iFlightGPSHDOP : iFlightGPSDecimal
{
   double hdop() { return value() / 100.0; }
};

class iFlightGPSLib;
class iFlightGPSLibCustom
{
public:
   iFlightGPSLibCustom() {};
   iFlightGPSLibCustom(iFlightGPSLib &gps, const char *sentenceName, int termNumber);
   void begin(iFlightGPSLib &gps, const char *_sentenceName, int _termNumber);

   bool isUpdated() const  { return updated; }
   bool isValid() const    { return valid; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   const char *value()     { updated = false; return buffer; }

private:
   void commit();
   void set(const char *term);

   char stagingBuffer[_GPS_MAX_FIELD_SIZE + 1];
   char buffer[_GPS_MAX_FIELD_SIZE + 1];
   unsigned long lastCommitTime;
   bool valid, updated;
   const char *sentenceName;
   int termNumber;
   friend class iFlightGPSLib;
   iFlightGPSLibCustom *next;
};

class iFlightGPSLib
{
public:
  iFlightGPSLib();
  bool encode(char c);
  iFlightGPSLib &operator << (char c) {encode(c); return *this;}

  iFlightGPSLocation location;
  iFlightGPSDate date;
  iFlightGPSTime time;
  iFlightGPSSpeed speed;
  iFlightGPSCourse course;
  iFlightGPSAltitude altitude;
  iFlightGPSInteger satellites;
  iFlightGPSHDOP hdop;

  // Library information.
  static const char *libName() { return _GPS_LIB_NAME; }
  static const char *libVersion() { return _GPS_LIB_VERSION; }
  static const char *libAuthor() { return _GPS_LIB_AUTHOR; }
  static const char *libEmail() { return _GPS_LIB_AUTHOR_EMAIL; }
  static const char *libSite() { return _GPS_LIB_AUTHOR_SITE; }

  static double distanceBetween(double lat1, double long1, double lat2, double long2);
  static double courseTo(double lat1, double long1, double lat2, double long2);
  static const char *cardinal(double course);

  static int32_t parseDecimal(const char *term);
  static void parseDegrees(const char *term, RawDegrees &deg);

  uint32_t charsProcessed()   const { return encodedCharCount; }
  uint32_t sentencesWithFix() const { return sentencesWithFixCount; }
  uint32_t failedChecksum()   const { return failedChecksumCount; }
  uint32_t passedChecksum()   const { return passedChecksumCount; }

private:
  enum {GPS_SENTENCE_GPGGA, GPS_SENTENCE_GPRMC, GPS_SENTENCE_OTHER};

  // parsing state variables
  uint8_t parity;
  bool isChecksumTerm;
  char term[_GPS_MAX_FIELD_SIZE];
  uint8_t curSentenceType;
  uint8_t curTermNumber;
  uint8_t curTermOffset;
  bool sentenceHasFix;

  // custom element support
  friend class iFlightGPSLibCustom;
  iFlightGPSLibCustom *customElts;
  iFlightGPSLibCustom *customCandidates;
  void insertCustom(iFlightGPSLibCustom *pElt, const char *sentenceName, int index);

  // statistics
  uint32_t encodedCharCount;
  uint32_t sentencesWithFixCount;
  uint32_t failedChecksumCount;
  uint32_t passedChecksumCount;

  // internal utilities
  int fromHex(char a);
  bool endOfTermHandler();
};

#endif // def(__iFlightGPSLib_h)
