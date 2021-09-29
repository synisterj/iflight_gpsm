/*
 * Included libs...
 */
#include <iFlightGPSM.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
/*
 * Defining these for ease...
 */
#define DEBUG true
#define APP_VERSION "1.2.4"
#define LIB_VERSION iFlightGPSLib::libVersion()
#define LIB_AUTHOR iFlightGPSLib::libAuthor()
/* Constant variables for rx/tx 
 * pins, LCD pins, Arduino and
 * GPS module baud rates.
 */
const int RX = 8, TX = 9;
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2, btn = 7;
const uint32_t GB = 9600, AB = 115200;
/*
 * For stats that update every 5 seconds. 
 */
unsigned long last = 0UL;
/* 
 * For the push button
 * scroll menu's position.
 */
int btn_pos = 1;
int btn_state = 0;
/* 
 * Declaring variables of class types.
 */
iFlightGPSLib GPS;
SoftwareSerial GPSMS(RX, TX);
LiquidCrystal LCD( RS, EN, D4, D5, D6, D7 );
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
  pause(1500);
  if ( DEBUG ) { Serial.print( F( "DEBUG: Using application version: " ) ); Serial.println( APP_VERSION ); }
  delay( 100 );
  if ( DEBUG ) { Serial.print( F( "DEBUG: Using iFlightGPSM library version: " ) ); Serial.println( LIB_VERSION ); }
  delay( 100 );
  if ( DEBUG ) { Serial.print( F( "DEBUG: iFlightGPSM library author: " ) ); Serial.println( LIB_AUTHOR ); }
  /*
   * Display my custom iFlightGPSLib version.
   */
  LCD.clear();
  lcd_write("Using Application", 0, 0, false, false);
  lcd_write("Version: " + String( APP_VERSION ), 0, 1, false, false);
  delay( 1000 );
  LCD.clear();
  lcd_write("Using iFlightGPSLib", 0, 0, false, false);
  lcd_write("Ver: " + String( LIB_VERSION ) + " by " + String(LIB_AUTHOR), 0, 1, false, false);
  delay( 1000 );
  LCD.clear();
}
/*
 * Experimental.
 */
void pause(unsigned long millisecs)
{
  unsigned long ms = millis() + millisecs;
  while (millisecs <= ms){
    millisecs++;
    delay(1);
  }
}
/*
 * Main loop...
 */
void loop()
{
  btn_state = digitalRead(btn);
  if ( btn_state == HIGH ) {
    btn_pos++;  
  }
  /*
   * Switch statement for button scroll menu.
   */
  switch ( btn_pos ) {
  case 1: // If position = 1
    if ( DEBUG ) { Serial.println( "DEBUG: #1 Location" ); } // If DEBUG is true print this to the serial monitor.
    LCD.clear(); // Clear the LCD.
    lcd_write( "#1 Location", 0, 0, false, false ); // Write to the LCD.
    while ( btn_pos == 1 ) { // While the button position remains at 1...
      while ( GPSMS.available() > 0 ) // Check if GPSM serial is available.
        GPS.encode( GPSMS.read() ); // If and whille available, read the data.
        if ( GPS.location.isUpdated() ) // If data has been updated...
        {
          lcd_write( get_loc(), 0, 1, false, true ); // Write it to the LCD and make it scroll.
        }
      btn_state = digitalRead( btn ); // Check our button state(HIGH 5v 10kohm) to see if it's been pressed.
      delay( 300 ); // Give a little delay to read the button if pressed.
      if ( btn_state == HIGH ) { // If the button has been pressed.
        btn_pos++; // Increment our button position.
      }
    }
    btn_state = 0; // Return our button state to 0.
    break; // Break out of the case or button position.
  case 2:
    if ( DEBUG ) { Serial.println("DEBUG: #2 Date"); }
    LCD.clear();
    lcd_write("#2 Date", 0, 0, false, false);
    while ( btn_pos == 2 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.date.isUpdated())
          {
            lcd_write( get_date(), 0, 1, false, true ); // Write it to the LCD and make it scroll.
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
  case 3:
    if ( DEBUG ) { Serial.println("DEBUG: #3 Time"); }
    LCD.clear();
    lcd_write("#3 Time", 0, 0, false, false);
    while ( btn_pos == 3 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.time.isUpdated())
          {
            lcd_write( get_time(), 0, 0, false, true ); 
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
  case 4:
    if ( DEBUG ) { Serial.println("DEBUG: #4 Speed"); }
    LCD.clear();
    lcd_write("#4 Speed", 0, 0, false, false);
    while ( btn_pos == 4 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.speed.isUpdated())
          {
            lcd_write( get_spd(), 0, 1, false, true );  
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
  case 5:
    if ( DEBUG ) { Serial.println("DEBUG: #5 Course"); }
    LCD.clear();
    lcd_write("#5 Course", 0, 0, false, false);
    while ( btn_pos == 5 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.course.isUpdated())
          {
            lcd_write( get_course(), 0, 1, false, true );  
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
  case 6:
    if ( DEBUG ) { Serial.println("DEBUG: #6 Altitude"); }
    LCD.clear();
    lcd_write("#6 Altitude", 0, 0, false, false);
    while ( btn_pos == 6 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.altitude.isUpdated())
          {
            lcd_write( get_altitude(), 0, 1, false, true );  
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
  case 7:
    if ( DEBUG ) { Serial.println("DEBUG: #7 Satellites"); }
    LCD.clear();
    lcd_write("#7 Satellites", 0, 0, false, false);
    while ( btn_pos == 7 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.satellites.isUpdated())
          {
            lcd_write( get_satellites(), 0, 1, false, true );  
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
    case 8:
    if ( DEBUG ) { Serial.println("DEBUG: #8 HDOP"); }
    LCD.clear();
    lcd_write("#8 HDOP", 0, 0, false, false);
    while ( btn_pos == 8 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (GPS.hdop.isUpdated())
          {
            lcd_write(get_hdop(), 0, 1, false, true );  
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
    case 9:
    if ( DEBUG ) { Serial.println("DEBUG: #9 Diagnostics"); }
    LCD.clear();
    lcd_write("#9 Diagnostics", 0, 0, false, false);
    while ( btn_pos == 9 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (millis() - last > 5000)
          {
            lcd_write(get_diag(), 0, 1, false, true );  
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
    case 10:
    if ( DEBUG ) { Serial.println("DEBUG: #10 Fail Packets"); }
    LCD.clear();
    lcd_write("#10 Fail Packets", 0, 0, false, false);
    while ( btn_pos == 10 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (millis() - last > 5000)
          {
            lcd_write(get_fchksum(), 0, 1, false, true );  
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
    case 11:
    if ( DEBUG ) { Serial.println("DEBUG: #11 Pass Packets"); }
    LCD.clear();
    lcd_write("#11 Pass Packets", 0, 0, false, false);
    while ( btn_pos == 11 ) {
      while (GPSMS.available() > 0)
        GPS.encode(GPSMS.read());
          if (millis() - last > 5000)
          {
            lcd_write(get_pchksum(), 0, 1, false, true );  
          }
      btn_state = digitalRead(btn);
      delay(300);
      if ( btn_state == HIGH ) {
        btn_pos++;  
      }
    }
    btn_state = 0;
    break;
  default:
    if ( DEBUG ) { Serial.println("Default option selected, returning to option 1..."); }
    lcd_write("Returning to #1", 0, 0, false, true );
    btn_pos = 1;
    btn_state = 0;
    break;
  }
}
/*
 * LCD control function.
 */
void lcd_write( String text, int cursln1, int cursln2, bool clrlcd, bool scroll )
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
String get_loc()
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
  if ( DEBUG ) { Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Display date on LCD.
 */
String get_date()
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
  if ( DEBUG ) { Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Display time on LCD.
 */
String get_time()
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
  if ( DEBUG ) { Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Display spd on LCD.
 */ 
String get_spd()
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
  if ( DEBUG ) { Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Display course on LCD.
 */ 
String get_course()
{
  String str =
    "COURSE     Fix Age=" +
    String(GPS.course.age()) +
    "ms Raw=" + 
    String(GPS.course.value()) +
    " Deg=" +
    String(GPS.course.deg());
  if ( DEBUG ) { Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Display altitude on LCD.
 */ 
String get_altitude()
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
  if ( DEBUG ) { Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Display satellites on LCD.
 */ 
String get_satellites()
{
  String str =
    "SATELLITES Fix Age=" +
    String(GPS.satellites.age()) +
    "ms Value=" +
    String(GPS.satellites.value());
  if ( DEBUG ) { Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Display HDOP on LCD.
 */ 
String get_hdop()
{
  String str =
    "HDOP       Fix Age=" +
    String(GPS.hdop.age()) +
    "ms raw=" +
    String(GPS.hdop.value()) +
    " hdop=" +
    String(GPS.hdop.hdop());
  if ( DEBUG ) { Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Display diagnostics on LCD.
 */ 
String get_diag()
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
  if ( DEBUG ) { Serial.println(); Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Get fail checksums and return as string.
 */ 
String get_fchksum()
{
  String str =
    "Failed-checksum=" +
    String(GPS.failedChecksum());
  if ( DEBUG ) { Serial.println(); Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Get pass checksums and return as string.
 */ 
String get_pchksum()
{
  String str =
    " Passed-checksum=" + 
    String(GPS.passedChecksum());
  if ( DEBUG ) { Serial.println(); Serial.println( "DEBUG: " + str ); Serial.println(); }
  return str;
}
/*
 * Simple diagnostics function.
 */ 
void diag()
{
  get_diag();
  if (GPS.charsProcessed() < 10)
    if ( DEBUG ) { Serial.println(F("WARNING: No GPS data.  Check RX and TX wiring.")); }
    last = millis();
    if ( DEBUG ) { Serial.println(); }
}
