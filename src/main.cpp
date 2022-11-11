// 2021-2022 spacerabbit#1487
// VER 22.11.11
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "HT_SSD1306Wire.h"
#include "GPS_Air530Z.h" // Enable this for board version 1.1
//#include "GPS_Air530.h" // Enable this for board version 1.0 and 1.0_1
//#define DEBUG // Enable/Disable debug output over the serial console

/* Choose Device */
#define DEVICE_A
//#define DEVICE_B

#ifdef DEVICE_A
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif

#ifdef DEVICE_B
#define DS18B20_SENSOR
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif


#define MIN_DIST              20        // Minimum distance in meters from the last sent location before we can send again. A hex is about 340m, divide by this value to get the pings per hex.
#define VBAT_CORRECTION       1.005     // Edit this for calibrating your battery voltage
#define GPS_READ_RATE         900       // How often to read GPS
#define SLEEPING_UPDATE_RATE  600000    // Update every 20min when sleeping 
#define MAX_GPS_WAIT          300000    // Max time to wait for GPS before going to sleep
#define AUTO_SLEEP_TIMER      300000    // If no movement for this amount of time, the device will go to sleep. Comment out if you don't want this feature. 
#define NONSTOP_UPDATE_RATE   10000     // NonStop Mode Update rate
#define MENU_IDLE_TIMEOUT     10000     // Auto exit the menu if no button pressed in this amount of ms
#define TIMEZONE_OFFSET       1         // Offset to UTC


#ifdef DS18B20_SENSOR_HTU
#include <Wire.h>
#include "SparkFunHTU21D.h"
HTU21D myHumidity;
#endif

#ifdef DS18B20_SENSOR
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS GPIO11
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#endif

extern SSD1306Wire            display;    // Defined in LoRaWan_APP.cpp
extern uint8_t                isDispayOn; // Defined in LoRaWan_APP.cpp
#ifdef GPS_Air530_H
Air530Class                   GPS;
#endif
#ifdef GPS_Air530Z_H
Air530ZClass                  GPS;
#endif

// Commend out/uncomment this line to disable/enable the auto sleep/wake up by vibration sensor feature
#define VIBR_SENSOR           GPIO5     // Change the pin where the sensor is connected if different
// Comment out/uncomment this line to disable/enable the functionality  where the vibration sensor wakes the device from "deep" sleep (VIBR_SENSOR must be enabled)
#define VIBR_WAKE_FROM_SLEEP
// If put to sleeep from the menu, this will disable the wake up by vibration and only allow it to work when auto sleep was activated in some way (like stopped for too long)
#define MENU_SLEEP_DISABLE_VIBR_WAKEUP



/* ABP para*/
uint8_t nwkSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint32_t devAddr =  ( uint32_t )0x00000000;

#if defined( REGION_EU868 )
/*LoraWan channelsmask, default channels 0-7*/ 
uint16_t userChannelsMask[6] = { 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };
int8_t dataRates[6] = {0, 1, 2, 3, 4, 5};
#else
uint16_t userChannelsMask[6] = { 0xFF00,0x0000,0x0000,0x0000,0x0000,0x0000 };
int8_t dataRates[4] = {0, 1, 2, 3};
#endif

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
/* Start with non-zero value, for the first transmission with previously stored JOIN,
 * but it will be changed later depending on the mode */
uint32_t appTxDutyCycle = NONSTOP_UPDATE_RATE;

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

#define PORT_DEFAULT        2
#define PORT_HDOPHIGH       3
#define PORT_LASTLOC        4
#define PORT_NEVERGOTGPS    5
#define PORT_NOGPS          6
#define PORT_SLEEPNOMOVE    7
#define PORT_SLEEPNOGPS     8
#define PORT_SLEEPLOWBATT   9
#define PORT_SLEEPDL        10
#define PORT_DEEPSLEEP      11
#define PORT_DLCONFIRM      12

bool prepareTxFrame(uint8_t port);

/* Application port */
uint8_t appPort = PORT_DEFAULT;

uint8_t confirmedNbTrials = 4;

const uint8_t rabbit_logo_bmp[] PROGMEM = {
  0x00, 0x00, 0xF7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x08, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x08, 0x80, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xF8, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xF8, 0x03, 0xFC, 0x01, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x88, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xF0, 0x87, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x08, 0x80, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x1F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xF0, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xFE, 0xE1, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 
  0xF8, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xC7, 0xFF, 0xFF, 0xFF, 
  0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x0F, 0x00, 0xE0, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xE0, 
  0xFF, 0x01, 0xF0, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x00, 0x00, 0x00, 
  0x70, 0x07, 0x22, 0xF0, 0xFF, 0xFF, 0xF8, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF7, 0x0F, 0x00, 0x00, 0x88, 0x08, 0x14, 0xF0, 0xC1, 0xFF, 0xFF, 0xBF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0x3F, 0x00, 0x00, 0x88, 0x08, 0x08, 0xE0, 
  0x1F, 0xF8, 0x7F, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0x00, 0x00, 
  0x88, 0x88, 0xFF, 0xC0, 0xFF, 0xF3, 0xBF, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xEF, 0xFF, 0x00, 0x00, 0x88, 0x08, 0x00, 0x80, 0xFF, 0xFF, 0x9F, 0x33, 
  0xF8, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0x01, 0x00, 0xF8, 0x0F, 0x00, 0x00, 
  0xFE, 0xFF, 0xDF, 0xB7, 0xFB, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0x01, 0x00, 
  0x00, 0x80, 0x80, 0x00, 0x00, 0xE0, 0xDF, 0xB6, 0xF7, 0xFF, 0xFF, 0xFF, 
  0xDF, 0xFF, 0x01, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xFF, 0x1F, 0xBC, 0xB7, 
  0xF7, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0x01, 0x00, 0x70, 0x87, 0x80, 0xF0, 
  0xFF, 0xFF, 0x0F, 0x38, 0xF0, 0xFF, 0xFF, 0xC0, 0xEF, 0xCF, 0x00, 0x00, 
  0x88, 0x88, 0x80, 0xF8, 0xF9, 0xFF, 0xE7, 0xBB, 0xFB, 0xFF, 0x7F, 0x1E, 
  0xEF, 0x1F, 0x00, 0x00, 0x88, 0x88, 0xFF, 0xF8, 0x03, 0xFE, 0x77, 0xBB, 
  0xF7, 0xFF, 0x3F, 0x7F, 0xEC, 0x3F, 0x00, 0x00, 0x88, 0x08, 0x00, 0xF8, 
  0xFF, 0xF0, 0xF7, 0xBB, 0xF7, 0xFF, 0xBF, 0xFF, 0xE1, 0x7F, 0x00, 0x00, 
  0x88, 0x08, 0x00, 0xF0, 0xFF, 0xFF, 0xE7, 0x3B, 0xF0, 0xFF, 0x3F, 0xFF, 
  0xC7, 0x7F, 0x00, 0x00, 0xF8, 0x0F, 0xFF, 0xC0, 0xFF, 0xFF, 0xEF, 0x99, 
  0xFF, 0xFF, 0x7F, 0xFE, 0x87, 0x3F, 0x00, 0x00, 0x00, 0x80, 0x08, 0x00, 
  0xFE, 0xFF, 0x9F, 0xDC, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0x3E, 0x00, 0x00, 
  0x00, 0x80, 0x08, 0x00, 0x00, 0xFE, 0x3F, 0xCF, 0xFF, 0xFF, 0xFF, 0xF9, 
  0x03, 0x18, 0x00, 0x00, 0xF0, 0x8F, 0x08, 0x00, 0x00, 0x00, 0xFE, 0xE7, 
  0xFF, 0xFF, 0x1F, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x88, 0x00, 0xFF, 0x00, 
  0x00, 0x00, 0xFE, 0xF7, 0xFF, 0xFF, 0xCF, 0xF3, 0x01, 0x00, 0x00, 0x00, 
  0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 
  0x01, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 
  0xFF, 0xFF, 0xEF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x88, 0x00, 0xF7, 0x00, 
  0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0x00, 0x00, 0x00, 0x00, 
  0xF0, 0x8F, 0x08, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x08, 0x00, 0x00, 0x00, 0x00, 0xFC, 
  0xF9, 0xFF, 0x3F, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x08, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x38, 0x00, 0x00, 0x00, 0x00, 
  0x70, 0x8F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x03, 
  0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xFE, 0xC1, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x88, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x88, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x8F, 0xFF, 0x00, 
  0x00, 0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

bool      sleepMode               = false;
bool      loopingInSend           = false;
bool      menuMode                = false;
bool      screenOffMode           = true; // Enable normal operation with the screen off - for more battery saving
uint32_t  lastScreenPrint         = 0;
uint32_t  joinStart               = 0;
uint32_t  gpsSearchStart          = 0;
uint32_t  lastValidGPS            = 0;
uint32_t  lastValidSend           = 0;
uint32_t  lastSend                = 0;
int       currentMenu             = 0;
bool      sleepActivatedFromMenu  = false;
bool      gpsTimerSet             = false;
enum eDeviceState_LoraWan stateAfterMenu;

bool      sendLastLoc         = false;
bool      lastLocSet          = false;
uint32_t  last_lat            = 0;
uint32_t  last_lon            = 0;
double    last_send_lat       = 0;
double    last_send_lon       = 0;
uint32_t  min_dist_moved      = MIN_DIST;
uint32_t  dist_moved          = UINT32_MAX;
uint8_t   currentDRidx        = 0;

bool      nonStopMode         = false;            ///////////////////////////////////////////////////
bool      gps_debug           = false;
bool      sendNoGPS           = true;
bool      sendReasonToSleep   = false;
bool      sleepNow            = false;
int       nonStopUpdateRate   = NONSTOP_UPDATE_RATE;
int       sleepingUpdateRate  = SLEEPING_UPDATE_RATE;

#ifdef MAX_GPS_WAIT
int       maxGPSwait          = MAX_GPS_WAIT;
#endif

#ifdef MAX_GPS_WAIT
int       autoSleepTimer          = AUTO_SLEEP_TIMER;
#endif

int       reasonToSleep       = 0;
int       sendNoGPScount      = 1;
int       sendHighHDOPcount   = 1;
int       sendRate            = 30000;
float     requiredHdop        = 3.7;
bool      mustStartGPS            = false;
bool      mustCycleGPS            = false;
bool      mustToggleSleepMode     = false;
bool      mustToggleScreenMode    = false;
int       spreadingFactor       = 12;

bool      confirmDLreceived = false;
bool      sendConfirmDownLink = false;

#define MENU_CNT 5

char* menu[MENU_CNT] = {"Screen OFF", "Sleep", "NonStop Mode", "Set SF12", "Set SF7"};  //    "Next DR", SET_SF7,  //, DEBUG_INFO, "Debug Info"

enum eMenuEntries
{
  SCREEN_OFF,
  SLEEP,
  NONSTOP_MODE,
  SET_SF12,
  SET_SF7
};

void userKey();

// Timer to schedule wake ups for GPS read before going to sleep 
static TimerEvent_t GPSCycleTimer;
// Timer to auto close the menu after certain period of inactivity
static TimerEvent_t menuIdleTimeout;

void LoRaWANsend(void)
{
  lastSend = millis();
  LoRaWAN.send();
}

int32_t fracPart(double val, int n)
{
  return (int32_t)abs(((val - (int32_t)(val)) * pow(10, n)));
}

// RGB LED power on
void VextON(void)
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

// RGB LED power off
void VextOFF(void) 
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}

// Call this from other display methods (assumes the display is initialized and awake)
void displayBatteryLevel()
{
  uint16_t batteryVoltage;
  char str[30];  
  int index;
  
  detachInterrupt(USER_KEY); // reading battery voltage is messing up with the pin and driving it down, which simulates a long press for our interrupt handler 

  batteryVoltage = getBatteryVoltage();
  float_t batV = ((float_t)batteryVoltage * VBAT_CORRECTION)/1000;  // Multiply by the appropriate value for your own device to adjust the measured value after calibration
  index = sprintf(str, "%d.%02dV", (int)batV, fracPart(batV, 2));       
    
  str[index] = 0;

  attachInterrupt(USER_KEY, userKey, FALLING);  // Attach again after voltage reading is done

  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, str);
}

void displayGPSInfo()
{
  char str[30];
  display.clear();
  display.setFont(ArialMT_Plain_10);

  int gpsHour = GPS.time.hour() + TIMEZONE_OFFSET;
  if (gpsHour > 23) gpsHour = gpsHour - 24;
  
  int last_send = (millis() - lastValidSend)/1000;
  if (last_send > 999) last_send = 999;

  int distance_meters = dist_moved;
  if (distance_meters > 999) distance_meters = 999;


display.setTextAlignment(TEXT_ALIGN_RIGHT);

  displayBatteryLevel();

  int index = sprintf(str, "%dm", (int)GPS.altitude.meters());
  str[index] = 0;
  display.drawString(128, 16, str);

  index = sprintf(str, "SF%d", spreadingFactor);
  str[index] = 0;
  display.drawString(128, 32, str);

  index = sprintf(str, "%d/%d", distance_meters, min_dist_moved);
  str[index] = 0;
  display.drawString(128, 48, str);



display.setTextAlignment(TEXT_ALIGN_CENTER);
  
  index = sprintf(str, "%02ds", last_send);
  str[index] = 0;
  display.drawString(70, 0, str);

  index = sprintf(str, "%d.%d", (int)GPS.location.lng(), fracPart(GPS.location.lng(), 4));
  str[index] = 0;
  display.drawString(70, 16, str);

  index = sprintf(str, "%d.%d dop", (int)GPS.hdop.hdop(), fracPart(GPS.hdop.hdop(), 1));
  str[index] = 0;
  display.drawString(70, 32, str); 

display.setFont(ArialMT_Plain_16);
  index = sprintf(str, "%03d", (int)GPS.speed.kmph());
  str[index] = 0;
  display.drawString(70, 48, str);
display.setFont(ArialMT_Plain_10);

display.setTextAlignment(TEXT_ALIGN_LEFT);

  index = sprintf(str, "%02d:%02d:%02d", gpsHour, GPS.time.minute(), GPS.time.second());
  str[index] = 0;
  display.drawString(0, 0, str);

  index = sprintf(str, "%d.%d", (int)GPS.location.lat(), fracPart(GPS.location.lat(), 4));
  str[index] = 0;
  display.drawString(0, 16, str);   

  index = sprintf(str, "%d sat", (int)GPS.satellites.value());
  str[index] = 0;
  display.drawString(0, 32, str); 

  if (nonStopMode == true)
  {
    index = sprintf(str, "Nonstop");
  }
  else
  {
    display.setFont(ArialMT_Plain_16);

    if (GPS.course.deg() < 23) index = sprintf(str, "- N -");
    else if (GPS.course.deg() < 68) index = sprintf(str, "N - E");
    else if (GPS.course.deg() < 113) index = sprintf(str, "- E -");
    else if (GPS.course.deg() < 158) index = sprintf(str, "S - E");
    else if (GPS.course.deg() < 203) index = sprintf(str, "- S -");
    else if (GPS.course.deg() < 248) index = sprintf(str, "S - W");
    else if (GPS.course.deg() < 293) index = sprintf(str, "- W -");
    else if (GPS.course.deg() < 338) index = sprintf(str, "N - W");
    else index = sprintf(str, "- N -");

    //index = sprintf(str, "%d", (int)GPS.course.deg());
  }
  str[index] = 0;
  display.drawString(0, 48, str); 
  

  display.display();
}

#ifdef DEBUG
void printGPSInfo()
{
  Serial.print("Date/Time: ");
  if (GPS.date.isValid())
  {
    Serial.printf("%d/%02d/%02d", GPS.date.year(), GPS.date.day(), GPS.date.month());
  }
  else
  {
    Serial.print("INVALID");
  }

  if (GPS.time.isValid())
  {
    Serial.printf(" %02d:%02d:%02d.%02d", GPS.time.hour(), GPS.time.minute(), GPS.time.second(), GPS.time.centisecond());
  }
  else
  {
    Serial.print(" INVALID");
  }
  Serial.println();
  
  Serial.print("LAT: ");
  Serial.print(GPS.location.lat(), 6);
  Serial.print(", LON: ");
  Serial.print(GPS.location.lng(), 6);
  Serial.print(", ALT: ");
  Serial.print(GPS.altitude.meters());

  Serial.println(); 
  
  Serial.print("SATS: ");
  Serial.print(GPS.satellites.value());
  Serial.print(", HDOP: ");
  Serial.print(GPS.hdop.hdop());
  Serial.print(", AGE: ");
  Serial.print(GPS.location.age());
  Serial.print(", COURSE: ");
  Serial.print(GPS.course.deg());
  Serial.print(", SPEED: ");
  Serial.println(GPS.speed.kmph());
  Serial.println();
}
#endif

int8_t loraDataRate()
{
  MibRequestConfirm_t mibReq;
  LoRaMacStatus_t status;
  int8_t ret = -1;
  
  mibReq.Type = MIB_CHANNELS_DATARATE;
  status = LoRaMacMibGetRequestConfirm( &mibReq );
  if (status == LORAMAC_STATUS_OK)
  {
    ret = mibReq.Param.ChannelsDatarate;
  }

  return ret;
}

void displayLogoAndMsg(String msg, uint32_t wait_ms)
{
  if (!screenOffMode) {
    VextON();
    if (!isDispayOn)
    {
      display.wakeup(); 
      isDispayOn = 1;
    }
    display.clear();
    display.drawXbm(0, 0, 128, 48, rabbit_logo_bmp);
    //displayBatteryLevel();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 54, msg);
    #ifdef DEBUG
    Serial.println(msg);
    #endif
    display.display();

    if (wait_ms)
    {
      delay(wait_ms);
    }
  }
}

void displayGPSWaitWithCounter()
{  
  char str[30];
  int index;

  if (((millis() - lastScreenPrint) > 1000) && (millis() - gpsSearchStart > 2000))
  {
    int fix_wait_secs = (millis() - gpsSearchStart) / 1000;
    index = sprintf(str, "GPS FIX WAIT  %d", fix_wait_secs);
    str[index] = 0;
    displayLogoAndMsg(str, false);
    lastScreenPrint = millis();
  }  
}

void displayJoinTimer()
{
  char str[30];
  int index; 

  if ((millis() - lastScreenPrint) > 1000)
  {
    int join_wait_secs = (millis() - joinStart) / 1000;
    index = sprintf(str, "JOIN LORAWAN  %d", join_wait_secs);
    str[index] = 0;

    displayLogoAndMsg(str, false);
    lastScreenPrint = millis();
    display.setFont(ArialMT_Plain_16);
  }
}

void displayGPSInfoEverySecond()
{
  //if (((millis() - lastScreenPrint) > 1000) && GPS.time.isValid() && GPS.time.isUpdated())
  if (((millis() - lastScreenPrint) > 1000) && GPS.time.isValid())
  {            
    #ifdef DEBUG
    printGPSInfo();
    if (screenOffMode)
    {
      delay(15);
    }
    #endif
    if (!screenOffMode)
    {
      if (!isDispayOn)
      {
        display.wakeup();
        isDispayOn = 1;
      }    
      displayGPSInfo();
    }
    lastScreenPrint = millis();
  }
}

void displayMenu()
{
  int prev; 
  int next; 
  String currentOption = menu[currentMenu]; 
  currentOption.toUpperCase();

  prev = currentMenu - 1;

  if (prev < 0)
  {
    prev = MENU_CNT - 1;
  }

  next = currentMenu + 1;

  if (next >= MENU_CNT)
  {
    next = 0;
  }

  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.clear();
  
  display.drawString(64, 0, menu[prev]);   
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, (display.getHeight() - 16) / 2, currentOption); 
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, display.getHeight() - 10, menu[next]); 
  displayBatteryLevel();
  display.display();
}

/*void displayDebugInfo()
{
  char str[30];
  int index; 

  display.clear();  
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  index = sprintf(str,"%s: %um", "Update every", min_dist_moved);
  str[index] = 0;
  display.drawString(0, 0, str);  
  index = sprintf(str,"%s: %u", "loopingInSend", loopingInSend);
  str[index] = 0; 
  display.drawString(0, 10, str);  
  index = sprintf(str,"%s: %u", "LoRaJoined", IsLoRaMacNetworkJoined);
  str[index] = 0; 
  display.drawString(0, 20, str);  
  //index = sprintf(str,"%s: %i", "DR", loraDataRate());
  index = sprintf(str,"%s: %i", "DR", currentDRidx);
  str[index] = 0; 
  display.drawString(0, 30, str);    
  index = sprintf(str,"%s: %i", "Send No GPS", sendNoGPS);
  str[index] = 0; 
  display.drawString(0, 40, str);    
  index = sprintf(str,"%s: %i", "NonStop Mode", nonStopMode);   /////////////////////////////////////////////////
  str[index] = 0;
  display.drawString(0, 50, str);    
  display.display();

  delay(6000);    
}*/

void startGPS()
{
  detachInterrupt(USER_KEY); // User key press during GPS baud rate detection crashes the device, so we disable the interrupt for the duration of that process
  GPS.begin(115200); // If you are sure that you have selected the right include directive for your GPS chip, you can use GPS.begin(115200) here. 
  // Air530Z code has setmode(MODE_GPS_BEIDOU_GLONASS) call in begin(), but for Air530 we will need to set it ourselves
  #ifdef GPS_Air530_H
  GPS.setmode(MODE_GPS_GLONASS); //Enable dual mode - GLONASS and GPS   
  #endif
  //GPS.setNMEA(NMEA_GGA); // decrease the amount of unnecessary data the GPS sends, NMEA_RMC has most of what we need, except altitude, NMEA_GGA has altitude but does not have date and speed
  GPS.setNMEA(NMEA_RMC | NMEA_GGA); // decrease the amount of unnecessary data the GPS sends, NMEA_RMC has most of what we need, except altitude, NMEA_GGA has altitude but does not have date and speed
  attachInterrupt(USER_KEY, userKey, FALLING); // enable back the user key press monitoring
  gpsSearchStart = millis();
  mustStartGPS = false;
}

void cycleGPS()
{
  uint32_t cycleGPStimer;
  #define maxBuff 1000
  char gpsBuff[maxBuff];
  int gpsBuffPtr = 0;

  mustCycleGPS = false;

  // read the location and speed to clear the updated flags
  GPS.location.rawLat(); 

  cycleGPStimer = millis();  

  while (millis() - cycleGPStimer < GPS_READ_RATE * 2)
  {
    while (GPS.available() > 0)
    {
      gpsBuff[gpsBuffPtr] = GPS.read();
      GPS.encode(gpsBuff[gpsBuffPtr]);
      gpsBuffPtr++;
      if (gpsBuffPtr >= maxBuff)
      {
        gpsBuffPtr = sprintf(gpsBuff,"++OF++"); // indicator for overflow
      }
      gpsBuff[gpsBuffPtr] = 0;
    }

    if (GPS.location.isUpdated())
    {      
      break;
    } 
    else
    {
      if (loopingInSend && !screenOffMode)
      {
        displayGPSWaitWithCounter();
      }
    }
  }

  if (GPS.location.age() < GPS_READ_RATE * 2)
  {    
    dist_moved = GPS.distanceBetween(last_send_lat, last_send_lon, GPS.location.lat(), GPS.location.lng());

    if (GPS.hdop.hdop() <= requiredHdop) // store the last lat/long 
    {
      last_lat    = ((GPS.location.lat() + 90) / 180.0) * 16777215;
      last_lon    = ((GPS.location.lng() + 180) / 360.0) * 16777215;
      lastLocSet  = true;
    }

    if (deviceState != DEVICE_STATE_SEND)
    {
      // if we moved more than min_dist_moved, then send
      if ( (dist_moved >= min_dist_moved && ((millis() - lastSend) > 9000)) || (nonStopMode && ((millis() - lastSend) > nonStopUpdateRate)) )
      {
        deviceState = DEVICE_STATE_SEND;
      }
    }
  }

  if (gps_debug)
  {
    Serial.println(gpsBuff);
  }
}

void stopGPS()
{
  GPS.end();
}

void switchModeToSleep()
{
    sleepMode = true;
  if (!screenOffMode)
  {
    if (!isDispayOn)
    {
      display.wakeup();
      isDispayOn = 1;
    } 
    displayLogoAndMsg("LOW POWER MODE", 1000);
    display.sleep();
    isDispayOn = 0;
  }

/*
  if (!screenOffMode)
  {
    displayLogoAndMsg("LOW POWER MODE", 1000);
 screenOffMode = true;       
  VextOFF();
  display.sleep();
  //display.stop();
  isDispayOn = 0;   
  }*/


  #ifdef DEBUG
  else
  {
    Serial.println("Going to sleep...");
  }
  #endif
  mustCycleGPS = false;
  stopGPS();
  //Radio.Sleep();      
  deviceState = DEVICE_STATE_CYCLE;  
}

void switchModeOutOfSleep()
{
    sleepMode = false;
  sleepActivatedFromMenu = false;
  if (!screenOffMode)
  {
    if (!isDispayOn)
    {
      display.wakeup();
      isDispayOn = 1;
    }
    displayLogoAndMsg("DEVICE ACTIVE", 1000);
    display.clear();
    display.display();
  }

  lastValidSend = millis();
  sleepNow = false;
  sendReasonToSleep = false;
  lastValidGPS = millis();
  requiredHdop = 3.7;
  sendNoGPScount = 1;
  sendHighHDOPcount = 1;

  #ifdef DEBUG
  Serial.println("Waking Up...");
  #endif
  //startGPS(); - Apparently we can't do this anymore in libraries v1.4.0 because GPSSerial.available() does not return anything when we are inside an interrupt handler (as we would be if we call this from USR key press)
  mustStartGPS = true;  
mustCycleGPS = false;
    deviceState = DEVICE_STATE_SEND;

  loopingInSend = false;
}

void switchScrenOffMode()
{
  displayLogoAndMsg("SCREEN OFF", 1000);
  screenOffMode = true;       
  VextOFF();
  display.stop();
  isDispayOn = 0;   
}

void switchScreenOnMode()
{
  screenOffMode = false;  
  VextON();
  display.init();
  isDispayOn = 1;
  displayLogoAndMsg("SCREEN ON", 1000);  
  display.clear();
  display.display();
}

void autoSleepIfNoGPS()
{
  #ifdef MAX_GPS_WAIT
  if (!nonStopMode && millis() - lastValidGPS > maxGPSwait)   ///////////////////////////////////////////
  {
    sendReasonToSleep = true;
    reasonToSleep = 1;
    if ((millis() - lastSend) < 9000) deviceState = DEVICE_STATE_CYCLE;
    else deviceState = DEVICE_STATE_SEND;
  }
  #endif
}

void autoSleepIfLowBatt()
{
  detachInterrupt(USER_KEY); // reading battery voltage is messing up with the pin and driving it down, which simulates a long press for our interrupt handler 
  uint16_t batteryVoltageMV = getBatteryVoltage();
  if (batteryVoltageMV < 3300)
  {
    sendReasonToSleep = true;
    reasonToSleep = 2;
    sleepActivatedFromMenu = true;
    if ((millis() - lastSend) < 9000) deviceState = DEVICE_STATE_CYCLE;
    else deviceState = DEVICE_STATE_SEND;
  }
  attachInterrupt(USER_KEY, userKey, FALLING);  // Attach again after voltage reading is done 
}

void autoSleepOnDownLink()
{
  if (sleepNow == true)
  {
  sendReasonToSleep = true;
  reasonToSleep = 3;
  if ((millis() - lastSend) < 9000) deviceState = DEVICE_STATE_CYCLE;
  else deviceState = DEVICE_STATE_SEND;
  }
}

void confirmOnDownLink()
{
  if (confirmDLreceived == true)
  {
  sendConfirmDownLink = true;
  if ((millis() - lastSend) < 9000) deviceState = DEVICE_STATE_CYCLE;
  else deviceState = DEVICE_STATE_SEND;
  }
}

static void OnGPSCycleTimerEvent()
{
  TimerStop(&GPSCycleTimer);

  if (!loopingInSend)
  {
    mustCycleGPS = true;
  }
  gpsTimerSet = false; 
}

bool prepareTxFrame(uint8_t port)
{
  appPort = port;

  uint32_t  lat, lon;
  int       alt, hdop, sats;

  #ifdef DS18B20_SENSOR_HTU
  digitalWrite(GPIO10, HIGH);
  Wire.begin();
  myHumidity.begin();

  //Serial.println(myHumidity.readTemperature());
  //Serial.println(myHumidity.readHumidity());

  int temperature = myHumidity.readTemperature() * 5;

  Wire.end();
  digitalWrite(GPIO10, LOW);

  if (!screenOffMode) {
    display.init();
  }
  #endif

  #ifdef DS18B20_SENSOR
  VextON();
  sensors.begin();
  sensors.requestTemperatures();

  #ifdef DEBUG
  Serial.println(sensors.getTempCByIndex(0));
  #endif

  int temperature = sensors.getTempCByIndex(0) * 5;
  if (screenOffMode) {
    VextOFF();
  }
  #endif

  unsigned char *puc;
  bool      ret = false;
  
  appDataSize = 0;

  detachInterrupt(USER_KEY); // reading battery voltage is messing up with the pin and driving it down, which simulates a long press for our interrupt handler 

  uint16_t batteryVoltage = ((float_t)((float_t)((float_t)getBatteryVoltage() * VBAT_CORRECTION)  / 10) + .5);  

  if (port == PORT_DEFAULT && GPS.location.isValid())
  {
    last_send_lat = GPS.location.lat(); // store the value for distance calculation
    last_send_lon = GPS.location.lng(); // store the value for distance calculation
  }

  switch (port)
  {

    case PORT_DEFAULT ... PORT_HDOPHIGH:

      if (GPS.location.isValid())
      {
        lat     = ((GPS.location.lat() + 90) / 180.0) * 16777215;
        lon     = ((GPS.location.lng() + 180) / 360.0) * 16777215;

        alt     = (uint16_t)GPS.altitude.meters();
        sats    = GPS.satellites.value();
        hdop    = GPS.hdop.hdop() * 10;

        puc = (unsigned char *)(&lat);
        appData[appDataSize++] = puc[2];
        appData[appDataSize++] = puc[1];
        appData[appDataSize++] = puc[0];

        puc = (unsigned char *)(&lon);
        appData[appDataSize++] = puc[2];
        appData[appDataSize++] = puc[1];
        appData[appDataSize++] = puc[0];

        puc = (unsigned char *)(&alt);
        appData[appDataSize++] = puc[1];
        appData[appDataSize++] = puc[0];

        appData[appDataSize++] = (uint8_t)((batteryVoltage - 200));
        appData[appDataSize++] = (uint8_t)(sats);
        appData[appDataSize++] = (uint8_t)(hdop);

        #ifdef DS18B20_SENSOR
        appData[appDataSize++] = (uint8_t)(temperature);
        #endif

        ret = true;
      }
      break; 


    case PORT_LASTLOC:

      puc = (unsigned char *)(&last_lat);
      appData[appDataSize++] = puc[2];
      appData[appDataSize++] = puc[1];
      appData[appDataSize++] = puc[0];

      puc = (unsigned char *)(&last_lon);
      appData[appDataSize++] = puc[2];
      appData[appDataSize++] = puc[1];
      appData[appDataSize++] = puc[0];

      appData[appDataSize++] = (uint8_t)((batteryVoltage-200));

      #ifdef DS18B20_SENSOR
      appData[appDataSize++] = (uint8_t)(temperature);
      #endif

      ret = true;
      break;      


    case PORT_NEVERGOTGPS ... PORT_DEEPSLEEP:

      appData[appDataSize++] = (uint8_t)((batteryVoltage-200));

      #ifdef DS18B20_SENSOR
      appData[appDataSize++] = (uint8_t)(temperature);
      #endif

      ret = true;
      break;         

    case PORT_DLCONFIRM:

      if (nonStopMode) appData[appDataSize++] = (uint8_t)(nonStopUpdateRate / 1000);
      else appData[appDataSize++] = (uint8_t)(0);
      appData[appDataSize++] = (uint8_t)(min_dist_moved);
      appData[appDataSize++] = (uint8_t)(screenOffMode);
      appData[appDataSize++] = (uint8_t)(autoSleepTimer / 10000);
      appData[appDataSize++] = (uint8_t)(maxGPSwait / 10000);
      appData[appDataSize++] = (uint8_t)(sleepingUpdateRate / 10000);
      appData[appDataSize++] = (uint8_t)(sleepActivatedFromMenu);

      ret = true;
      break;
  }

  attachInterrupt(USER_KEY, userKey, FALLING);  // Attach again after voltage reading is done 

  return ret;
}

#ifdef VIBR_SENSOR
void vibration(void)
{
  detachInterrupt(VIBR_SENSOR);
  
  if (!menuMode) // Ignore vibrations while in the menu
  {
    if (sleepMode)
    {
      #ifdef VIBR_WAKE_FROM_SLEEP     
      #ifdef MENU_SLEEP_DISABLE_VIBR_WAKEUP // If menu sleep overwrites the "vibration wake up from sleeep", then add the IF statement to not wake up when sleep was initiated from the menu
      if (!sleepActivatedFromMenu)
      #endif  
      mustToggleSleepMode = true;
      #endif
    }
  }
}

void setVibrAutoWakeUp()
{
  bool setupVibr = false; // Default operation is - don't wait for vibration

  // Except if we have VIBR_WAKE_FROM_SLEEP enabled, in which case we need to attach in sleep mode
  #ifdef VIBR_WAKE_FROM_SLEEP 
    // But if MENU_SLEEP_DISABLE_VIBR_WAKEUP enabled, only if sleep was not activated from the menu
    #ifdef MENU_SLEEP_DISABLE_VIBR_WAKEUP
    if (!sleepActivatedFromMenu)
    #endif
      setupVibr = setupVibr | sleepMode;  
  #endif
  
  if (setupVibr)
  {
    attachInterrupt(VIBR_SENSOR, vibration, FALLING);
  }  
}
#endif

void autoSleepIfNoMovement()
{
  #ifdef AUTO_SLEEP_TIMER
  if (millis() - lastSend > autoSleepTimer)
  {
    sendReasonToSleep = true;
    reasonToSleep = 0;
    if ((millis() - lastSend) < 9000) deviceState = DEVICE_STATE_CYCLE;
    else deviceState = DEVICE_STATE_SEND;
  }
  #endif
}

static void OnMenuIdleTimeout()
{
  TimerStop(&menuIdleTimeout);

  if (menuMode)
  {
    menuMode = false;
    deviceState = DEVICE_STATE_SEND;
    if (!screenOffMode)
    {
      display.clear();
      display.display();
    }
  }
}

void executeMenu(void)
{
  TimerStop(&menuIdleTimeout);
  
  switch (currentMenu)
  {
    case SCREEN_OFF:
      mustToggleScreenMode = true;
      deviceState = DEVICE_STATE_CYCLE;
      menuMode = false;      
      break;

    case SLEEP:
      mustToggleSleepMode = true;    
      menuMode = false;
      sleepActivatedFromMenu = true;
      break;

    case NONSTOP_MODE:                                        /////////////////////////////////////////////
      nonStopMode = !nonStopMode;
      deviceState = DEVICE_STATE_CYCLE;
      menuMode = false;
      break;


    /*case DEBUG_INFO:
      displayDebugInfo();
      deviceState = DEVICE_STATE_CYCLE;
      menuMode = false;
      break;

      GPS.end(); 
      delay(1000);

      deviceState = DEVICE_STATE_INIT;
      menuMode = false;
      break;  */

    case SET_SF12:
      deviceState = DEVICE_STATE_SEND;
      LoRaWAN.setDataRateForNoADR(0);
      spreadingFactor = 12;
      menuMode = false;
      break;

    case SET_SF7:
      deviceState = DEVICE_STATE_SEND;
      LoRaWAN.setDataRateForNoADR(5);
      spreadingFactor = 7;
      menuMode = false;
      break;
      
    default:
      menuMode = false;
      deviceState = stateAfterMenu;
      break;
  }
  if (!screenOffMode)
  {
    display.clear();
    display.display();
  }
}

void userKey(void)
{
  delay(10);
  if (digitalRead(USER_KEY) == LOW)
  {
    uint16_t keyDownTime = 0;
    while (digitalRead(USER_KEY) == LOW)
    {
      delay(1);
      keyDownTime++;
      if (keyDownTime >= 1000)
        break;
    }

    if (keyDownTime < 700)
    {
      if (sleepMode)
      {        
        if (screenOffMode)
        {
          screenOffMode = false;  
          VextON();
          display.init();
          isDispayOn = 1;
        }
        mustToggleSleepMode = true;
      }
      else if (screenOffMode)
      {
        mustToggleScreenMode = true;
      }
      else
      {
        if (menuMode)
        {
          currentMenu++;
          if (currentMenu >= MENU_CNT)
          {
            currentMenu = 0;
          }
        }
        else
        {
          menuMode = true;
          //currentMenu = 0;
          stateAfterMenu = DEVICE_STATE_CYCLE;  
          deviceState = DEVICE_STATE_SLEEP;
        }        
        TimerSetValue(&menuIdleTimeout, MENU_IDLE_TIMEOUT);
        TimerStart(&menuIdleTimeout);
      }
    }
    else
    {
      if (menuMode)
      {
        executeMenu();
      }
    }
  }
}

void handleStateChangesOnLoopStart()
{
  if (mustStartGPS)
  {
    startGPS();
  }

  if (mustCycleGPS && !sleepMode && !menuMode)
  {
    cycleGPS();
  }

  if (mustToggleScreenMode)
  {
    if (screenOffMode)
    {
      switchScreenOnMode();
    }
    else
    {
      switchScrenOffMode();
    }
    mustToggleScreenMode = false;
  }

  if (mustToggleSleepMode)
  {
    if (sleepMode)
    {
      switchModeOutOfSleep();
    }
    else
    {
      switchModeToSleep();
    }
    mustToggleSleepMode = false;
  }
}

void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
  #ifdef DEBUG
  Serial.printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
  Serial.print("+REV DATA:");
  for(uint8_t i=0;i<mcpsIndication->BufferSize;i++)
  {
    Serial.printf("%02X",mcpsIndication->Buffer[i]);
  }
  Serial.println();
  #endif

  uint8_t cmd = mcpsIndication->Buffer[0];

  if (mcpsIndication->Port == 2) //Sleep mode
  {
    if (cmd == 0)
    {
      sleepNow = true;
      sleepActivatedFromMenu = false;
    }
    else if (cmd == 1)
    {
      if (sleepMode) { mustToggleSleepMode = true; }
      sleepActivatedFromMenu = false;
    }
    else if (cmd == 2)
    {
      sleepNow = true;
      sleepActivatedFromMenu = true;
    }
  }
  else
  {
    confirmDLreceived = true;
  }

  if (mcpsIndication->Port == 3) //NonStop
  {
    if (cmd == 0)
    {
      nonStopMode = false;
    }
    else
    {
      nonStopMode = true;
      nonStopUpdateRate = cmd * 1000;
    }
    
    sendNoGPScount = 1;
    sendHighHDOPcount = 1;
  }	

  if (mcpsIndication->Port == 4) //Distance
  {
    min_dist_moved = cmd;
  }

  if (mcpsIndication->Port == 5) //SF
  {
    LoRaWAN.setDataRateForNoADR(cmd);

    spreadingFactor = -(cmd-12);
  }

  if (mcpsIndication->Port == 6) //Screen
  {
    if (cmd == 0)
    {
      if (!screenOffMode) { mustToggleScreenMode = true; }
    }
    else if (cmd == 1)
    {
      if (screenOffMode) { mustToggleScreenMode = true; }
    }
  }

  if (mcpsIndication->Port == 7) //Autosleep
  {
    autoSleepTimer = cmd * 10000;
  }	

  if (mcpsIndication->Port == 8) //GPS wait
  {
    maxGPSwait = cmd * 10000;
  }	

  if (mcpsIndication->Port == 9) //Sleep update
  {
    sleepingUpdateRate = cmd * 10000;
  }	
}



// Use AT+GPSDBG=1 to enable GPS debug output
bool checkUserAt(char * cmd, char * content)
{
  if (strcmp(cmd, "GPSDBG")==0)
  {
    gps_debug = (atoi(content) == 1);
    Serial.printf("+GPSDBG=%d", gps_debug);
    Serial.println();
    return true;
  }
  return false;
}

void setup() 
{
  boardInitMcu();

  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  #if(AT_SUPPORT)
  enableAt();
  #endif

  //pinMode(GPIO10, OUTPUT);
  //digitalWrite(GPIO10, LOW);

  #ifdef DS18B20_SENSOR_x
  sensors.begin();
  #endif

  if (!screenOffMode) {
    VextON();
    // Display branding image. If we don't want that - the following 2 lines can be removed  
    display.init(); // displayMcuInit() will init the display, but if we want to show our logo before that, we need to init ourselves.   
    isDispayOn = 1;
  }

  deviceState = DEVICE_STATE_INIT;
  
  /* This will switch deviceState to DEVICE_STATE_SLEEP and schedule a SEND timer which will 
    switch to DEVICE_STATE_SEND if saved network info exists and no new JOIN is necessary */
  //LoRaWAN.ifskipjoin(); 
  
  if (deviceState != DEVICE_STATE_INIT)
  {
    /* This messes up with LoRaWAN.init() so it can't be called before it, 
      but if we are not going to call LoRaWAN.init(), then we have to do it here. */
    startGPS(); 
  }
  //Setup user button - this must be after LoRaWAN.ifskipjoin(), because the button is used there to cancel stored settings load and initiate a new join
  pinMode(USER_KEY, INPUT);
  attachInterrupt(USER_KEY, userKey, FALLING);  

  #ifdef VIBR_SENSOR
  pinMode(VIBR_SENSOR, INPUT);
  #endif

  TimerInit(&GPSCycleTimer, OnGPSCycleTimerEvent);
  TimerInit(&menuIdleTimeout, OnMenuIdleTimeout);
}

void loop()
{
handleStateChangesOnLoopStart();

  switch (deviceState)
  {
    case DEVICE_STATE_INIT:
    {
      #if(AT_SUPPORT)
      getDevParam();
      #endif

      #ifdef DEBUG
      Serial.println("INIT");
      #endif

      printDevParam();
      LoRaWAN.init(loraWanClass, loraWanRegion);
      LoRaWAN.setDataRateForNoADR(0); // Set DR_0       
      //LoRaMacSetTestDutyCycleOn(false);
      
      mustStartGPS = true;

      deviceState = DEVICE_STATE_JOIN;
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      #ifdef DEBUG
      Serial.println("JOIN");
      #endif

      LoRaWAN.join();
      joinStart = millis();
      lastScreenPrint = joinStart;

      //startGPS();
      
      break;
    }
    case DEVICE_STATE_SEND:
    {
      #ifdef DEBUG
      Serial.println("DEVICE_STATE_SEND");
      #endif

      autoSleepIfLowBatt();
      autoSleepOnDownLink();
      confirmOnDownLink();

      if (menuMode) // User pressed the button while we were waiting for the next send timer
      {
        stateAfterMenu = deviceState; // If while waiting inside the menu, a Cycle timer ended and sent us here, after exiting the menu, don't waste time going back to Cycle, go directly to Send
        deviceState = DEVICE_STATE_SLEEP;
      }
      else if (sleepMode && !sendConfirmDownLink)
      {
        deviceState = DEVICE_STATE_CYCLE; // Send to Cycle so it could setup a sleep timer if not done yet

              sendRate = 30000;
              
              if ((millis() - lastSend) > sendRate) {
                appPort = PORT_DEEPSLEEP;
                if (prepareTxFrame(appPort)) LoRaWANsend();
                appPort = PORT_DEFAULT;
              }
              
      }
      else 
      {  

        if (sendConfirmDownLink)                        ///////////////////////////////////////////////////////////////////////////
        {
          appPort = PORT_DLCONFIRM;
          displayLogoAndMsg("CONFIRM DOWNLINK", 1000);

          if (prepareTxFrame(appPort)) LoRaWANsend();
          sendConfirmDownLink = false;
          confirmDLreceived = false;
        }
        else if (sendLastLoc)
        {
          if (lastLocSet) appPort = PORT_LASTLOC;
          else            appPort = PORT_NEVERGOTGPS;
          displayLogoAndMsg("SEND LAST LOCATION", false);
          if (prepareTxFrame(appPort)) LoRaWANsend();
          sendLastLoc = false;
          appPort = PORT_DEFAULT;
        }
        else if (sendReasonToSleep)                        ///////////////////////////////////////////////////////////////////////////
        {
          if      (reasonToSleep == 1) appPort = PORT_SLEEPNOGPS;
          else if (reasonToSleep == 2) appPort = PORT_SLEEPLOWBATT;
          else if (reasonToSleep == 3) appPort = PORT_SLEEPDL;
          else                         appPort = PORT_SLEEPNOMOVE;
          displayLogoAndMsg("SEND DEVICE STATUS", 1000);
          sleepNow = false;
          if (prepareTxFrame(appPort)) LoRaWANsend();
          sendReasonToSleep = false;
          //sendLastLoc = trackerMode;
          //LoRaWAN.cycle(SLEEPING_UPDATE_RATE);
          //switchModeToSleep();
          mustToggleSleepMode = true;
        }

        //if (!sleepMode) // if switchModeToSleep() was clled, we don't need any of this to execute
        //{
          if (!loopingInSend) // We are just getting here from some other state
          {
            loopingInSend = true; // We may be staying here for a while, but we want to reset the below variables only once when we enter.
            /* Reset both these variables. The goal is to skip the first unnecessary display of the GPS Fix Wait screen 
              and only show it if there was more than 1s without GPS fix and correctly display the time passed on it */
            gpsSearchStart = lastScreenPrint = millis(); 
          }
          
          //cycleGPS(); // Read anything queued in the GPS Serial buffer, parse it and populate the internal variables with the latest GPS data
          if (!mustToggleSleepMode && !sendLastLoc) {
          if (GPS.location.age() < GPS_READ_RATE) 
          {
            if (GPS.hdop.hdop() <= requiredHdop) // SEND COORDINATES
            {
              if (GPS.hdop.hdop() <= 1.7) requiredHdop = 2.4;
              if (GPS.hdop.hdop() <= 0.7) requiredHdop = 1.4;
              lastValidGPS = millis();
              sendNoGPScount = 0;
              sendHighHDOPcount = 0;
              appPort = PORT_DEFAULT;
                if (prepareTxFrame(appPort)) // Don't send bad data (the method will return false if GPS coordinates are 0)
                {
                  //if (!menuMode && !sleepMode) // In case user pressed the button while prepareTxFrame() was running
                  if (!menuMode) // In case user pressed the button while prepareTxFrame() was running
                  { 
                    if ((millis() - lastSend) > 9000) {
                      lastValidSend = millis();
                      displayLogoAndMsg("SEND COORDINATES", 1000);
                      LoRaWANsend();
                    }
                  }
                }
            }
            else // HIGH HDOP
            {
              if      (nonStopMode)            sendRate = nonStopUpdateRate;
              else if (sendHighHDOPcount > 0)  sendRate = 30000 * sendHighHDOPcount;
              else                             sendRate = 10000;

              if ((millis() - lastSend) > sendRate) {
                sendHighHDOPcount++;
                displayLogoAndMsg("SEND HDOP HIGH", 1000);
                if (prepareTxFrame(PORT_HDOPHIGH)) LoRaWANsend();
              }
              autoSleepIfNoGPS(); // If the wait for GPS is too long, automatically go to sleep
            }

            deviceState = DEVICE_STATE_CYCLE; // Schedule next send
          }   
          else // NO GPS
          {
            if (sendNoGPS)
            {
              if      (nonStopMode)         sendRate = nonStopUpdateRate;
              else if (sendNoGPScount > 0)  sendRate = 30000 * sendNoGPScount;
              else                          sendRate = 10000;

              if ((millis() - lastSend) > sendRate) {
                sendNoGPScount++;
                displayLogoAndMsg("SEND NO GPS", 1000);
                if (prepareTxFrame(PORT_NOGPS)) LoRaWANsend();
              }
            }
            autoSleepIfNoGPS(); // If the wait for GPS is too long, automatically go to sleep
          }   
        //}
        //cycleGPS();
        mustCycleGPS = true;
      }
    }
      break;
    }
    case DEVICE_STATE_CYCLE:
    {
      #ifdef DEBUG
      Serial.println("CYCLE");
      #endif

      loopingInSend = false;
      if (menuMode)
      {
        stateAfterMenu = deviceState; // This may not be necessary, because most of the menu options exist to Cycle anyway, but feels like the right thing to do
      }
      else
      {
        // Schedule next packet transmission
        if (nonStopMode)
        {
          LoRaWAN.cycle(nonStopUpdateRate);
        }
        else if (sleepMode)
        {
            LoRaWAN.cycle(sleepingUpdateRate);
            sendLastLoc = true; // After wake up, if tracker mode enabled - send the last known location before waiting for GPS 
        }
        #ifdef VIBR_SENSOR
        setVibrAutoWakeUp();
        #endif
      }

      if (!gpsTimerSet) 
      {
        // Schedule a wakeup for GPS read before going to sleep
        TimerSetValue(&GPSCycleTimer, GPS_READ_RATE);
        TimerStart(&GPSCycleTimer);
        gpsTimerSet = true;          
      }

      deviceState = DEVICE_STATE_SLEEP;
      break;
    }
    case DEVICE_STATE_SLEEP:
    {
      #ifdef DEBUG
      Serial.print("Z");
      #endif

      loopingInSend = false;
      if (menuMode)
      {
        if (!isDispayOn)
        {
          display.wakeup();
          isDispayOn = 1;
        }
        displayMenu();
      }
      else if (!IsLoRaMacNetworkJoined)
      {
        if (!screenOffMode)
        {
          displayJoinTimer(); // When not joined yet, it will display the seconds passed, so the user knows it is doing something
        }
      }
      else if (!sleepMode) // When not in sleep mode - display the current GPS every second
      {
        autoSleepIfNoMovement();

        if (!sleepMode) // checking if autoSleepIfNoMovement() changed it
        {
          displayGPSInfoEverySecond();

          if (!gpsTimerSet && LoRaMacState == LORAMAC_IDLE) 
          {
            // Schedule a wakeup for GPS read before going to sleep
            TimerSetValue(&GPSCycleTimer, GPS_READ_RATE);
            TimerStart(&GPSCycleTimer);
            gpsTimerSet = true;          
          }
        }
      }
      else // going to deep sleep, no need to keep the display on
      {
        if (!screenOffMode && isDispayOn) // only if screen on mode (otherwise the dispaly object is not initialized and calling methods from it will cause a crash)
        {
          display.sleep();
          VextOFF();
          isDispayOn = 0;
        }
      }

      if (deviceState == DEVICE_STATE_SLEEP) // because the exit from the menu may change it to Cycle or Send and we don't want to go to sleep without having scheduled a wakeup
      {

        LoRaWAN.sleep();
      } 
      break;
    }
    default:
    {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
}
