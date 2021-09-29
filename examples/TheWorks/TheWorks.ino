/*
 * Included libs...
 */
#include <iFlightGPSLib.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
/*
 * Defining these for ease...
 */
#define DEBUG true
#define VERSION "1.1.0"
/* Constant variables for rx/tx 
 * pins, LCD pins, Arduino and
 * GPS module baud rates.
 */
const int RX = 8, TX = 9;
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2, btn = 7;
const uint32_t GB = 9600, AB = 115200;
/* 
 * For the push button
 * scroll menu.
 */
int btn_ctr = 1;
int buttonState = 0;
/* 
 * Declaring variables of class types.
 */
iFlightGPSLib GPS;
SoftwareSerial GPSMS(RX, TX);
LiquidCrystal LCD( RS, EN, D4, D5, D6, D7 );
/*
 * For stats that update every 5 seconds. 
 */
unsigned long last = 0UL;
/* 
 * Initiation... 
 */
void setup()
{
  pinMode(btn, INPUT);
  Serial.begin( AB );
  while ( !Serial ) {
    ;
  }
  LCD.begin(16, 2);
  GPSMS.begin( GB );
  if ( DEBUG ) {
    Serial.print( F( "Using iFlightGPSLib version: " ) ); Serial.println( iFlightGPSLib::libraryVersion() );
    Serial.println();
  }
}
/*
 * Main loop...
 */
void loop()
{
  buttonState = digitalRead(btn);
  if ( buttonState == HIGH ) {
    btn_ctr++;  
  }
  /*
   * Switch statement for button scroll menu.
   */
  switch (btn_ctr) {
  case 1:
    Serial.println("Option 1 selected...");
    LCD.clear();
    while ( btn_ctr == 1 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
        if (GPS.location.isUpdated())
        {
          lcd_display_loc();
        }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
  case 2:
    Serial.println("Option 2 selected...");
    LCD.clear();
    while ( btn_ctr == 2 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.date.isUpdated())
          {
            lcd_display_date();
          }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
  case 3:
    Serial.println("Option 3 selected...");
    LCD.clear();
    while ( btn_ctr == 3 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.time.isUpdated())
          {
            lcd_display_time(); 
          }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
  case 4:
    Serial.println("Option 4 selected...");
    LCD.clear();
    while ( btn_ctr == 4 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.speed.isUpdated())
          {
            lcd_display_spd();  
          }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
  case 5:
    Serial.println("Option 5 selected...");
    LCD.clear();
    while ( btn_ctr == 5 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.course.isUpdated())
          {
            lcd_display_course();  
          }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
  case 6:
    Serial.println("Option 6 selected...");
    LCD.clear();
    while ( btn_ctr == 6 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.altitude.isUpdated())
          {
            lcd_display_altitude();  
          }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
  case 7:
    Serial.println("Option 7 selected...");
    LCD.clear();
    while ( btn_ctr == 7 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.satellites.isUpdated())
          {
            lcd_display_satellites();  
          }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
    case 8:
    Serial.println("Option 8 selected...");
    LCD.clear();
    while ( btn_ctr == 8 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.hdop.isUpdated())
          {
            lcd_display_hdop();  
          }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
    case 9:
    Serial.println("Option 9 selected...");
    LCD.clear();
    while ( btn_ctr == 9 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (millis() - last > 5000)
          {
            _diag();  
          }
      buttonState = digitalRead(btn);
      delay(100);
      if ( buttonState == HIGH ) {
        btn_ctr++;  
      }
    }
    buttonState = 0;
    break;
  default:
    Serial.println("Default option selected, returning to option 1...");
    btn_ctr = 1;
    buttonState = 0;
    break;
  }
}
/*
 * LCD control function. Not used as of yet...
 */
void _write( String text, int cursln1, int cursln2, bool clrlcd, bool scroll )
{
  if ( scroll ) {
   LCD.autoscroll();  
  }
  LCD.setCursor( cursln1, cursln2 );
  LCD.print( text );
  if ( scroll ) {
    LCD.noAutoscroll();
  }
  if ( clrlcd ) {
    LCD.clear();
  }
}
/*
 * Display location on LCD.
 */
String lcd_display_loc()
{
  String str =
    "LOCATION   Fix Age=" +
    String(GPS.location.age()) +
    "ms Raw Lat=" + 
    String(GPS.location.rawLat().negative ? "-" : "+") +
    String(GPS.location.rawLat().deg) +
    "[+" + 
    String(GPS.location.rawLat().billionths) + 
    " billionths],  Raw Long=" + 
    String(GPS.location.rawLng().negative ? "-" : "+") +
    String(GPS.location.rawLng().deg) +
    "[+" +
    String(GPS.location.rawLng().billionths) +
    " billionths],  Lat=" +
    String(GPS.location.lat(), 6) +
    " Long=" + 
    String(GPS.location.lng(), 6);
  if ( DEBUG ) { Serial.println( str ); }
  return str;
}
/*
 * Display date on LCD.
 */
String lcd_display_date()
{
  String str =
    "DATE       Fix Age=" +
    String(GPS.date.age()) + 
    "ms Raw=" +
    String(GPS.date.value()) +
    " Year=" +
    String(GPS.date.year()) + 
    " Month=" +
    String(GPS.date.month()) +
    " Day=" +
    String(GPS.date.day());
  if ( DEBUG ) { Serial.println( str ); }
  return str;
}
/*
 * Display time on LCD.
 */
String lcd_display_time()
{
  String str =
    "TIME       Fix Age=" +
    String(GPS.time.age()) +
    "ms Raw=" + 
    String(GPS.time.value()) +
    " Hour=" +
    String(GPS.time.hour()) + 
    " Minute=" + 
    String(GPS.time.minute()) +
    " Second=" + 
    String(GPS.time.second()) +
    " Hundredths=" + 
    String(GPS.time.centisecond());
  if ( DEBUG ) { Serial.println( str ); }
  return str;
}
/*
 * Display spd on LCD.
 */ 
String lcd_display_spd()
{
  String str =
    "SPEED      Fix Age=" + 
    String(GPS.speed.age()) +
    "ms Raw=" +
    String(GPS.speed.value()) +
    " Knots=" + 
    String(GPS.speed.knots()) +
    " MPH=" +
    String(GPS.speed.mph()) +
    F(" m/s=") +
    String(GPS.speed.mps()) +
    " km/h=" +
    String(GPS.speed.kmph());
  if ( DEBUG ) { Serial.println( str ); }
  return str;
}
/*
 * Display course on LCD.
 */ 
String lcd_display_course()
{
  String str =
    "COURSE     Fix Age=" +
    String(GPS.course.age()) +
    "ms Raw=" + 
    String(GPS.course.value()) +
    " Deg=" +
    String(GPS.course.deg());
  if ( DEBUG ) { Serial.println( str ); }
  return str;
}
/*
 * Display altitude on LCD.
 */ 
String lcd_display_altitude()
{
  String str =
    "ALTITUDE   Fix Age=" +
    String(GPS.altitude.age()) +
    "ms Raw=" +
    String(GPS.altitude.value()) +
    " Meters=" +
    String(GPS.altitude.meters()) +
    " Miles=" +
    String(GPS.altitude.miles()) +
    " KM=" +
    String(GPS.altitude.kilometers()) +
    " Feet=" +
    String(GPS.altitude.feet());
  if ( DEBUG ) { Serial.println( str ); }
  return str;
}
/*
 * Display satellites on LCD.
 */ 
String lcd_display_satellites()
{
  String str =
    "SATELLITES Fix Age=" +
    String(GPS.satellites.age()) +
    "ms Value=" +
    String(GPS.satellites.value());
  if ( DEBUG ) { Serial.println( str ); }
  return str;
}
/*
 * Display HDOP on LCD.
 */ 
String lcd_display_hdop()
{
  String str =
    "HDOP       Fix Age=" +
    String(GPS.hdop.age()) +
    "ms raw=" +
    String(GPS.hdop.value()) +
    " hdop=" +
    String(GPS.hdop.hdop());
  if ( DEBUG ) { Serial.println( str ); }
  return str;
}
/*
 * Display diagnostics on LCD.
 */ 
String lcd_display_diag()
{
  String str =
    "DIAGS      Chars=" +
    String(GPS.charsProcessed()) +
    " Sentences-with-Fix=" +
    String(GPS.sentencesWithFix()) +
    " Failed-checksum=" +
    String(GPS.failedChecksum()) +
    " Passed-checksum=" + 
    String(GPS.passedChecksum());
  if ( DEBUG ) { Serial.println(); Serial.println( str ); }
  return str;
}
/*
 * Simple diagnostics function.
 */ 
void _diag()
{
  lcd_display_diag();
  if (GPS.charsProcessed() < 10)
    if ( DEBUG ) { Serial.println(F("WARNING: No GPS data.  Check RX and TX wiring.")); }
    last = millis();
    if ( DEBUG ) { Serial.println(); }
}
