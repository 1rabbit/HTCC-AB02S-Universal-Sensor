// 2021-2022 spacerabbit#1487
// VER 22.12.22

//#define FRIDGE
//#define CORNINO
//#define LIZARD
//#define SQUIRREL
//#define BALLON3
//#define SLOTH
//#define DOLPHIN
//#define SDB
//#define THERMOSTAT1
//#define THERMOSTAT2
#define SOLARSUPERCAP

//#define DEBUG


#include "LoRaWan_APP.h"
#include "Arduino.h"

#define DEVICE_RESTART_PERIOD 7 //Days
#define TX_DUTY_CYCLE  6
#define BME280_CALIBRATION_T  1
#define BME280_CALIBRATION_H  1
#define BME280_CALIBRATION_P  1
#define DHT22_CALIBRATION  1
#define DHT22_CALIBRATION_H  1
#define DS18B20_CALIBRATION  1
#define DS18B20_CALIBRATION2 1
#define ADC_VOLTAGE_FACTOR 1
#define APPLICATION_PORT 20

// 0x[0-9a-fA-F][0-9a-fA-F] 
// 0x00

#ifdef FRIDGE
#define SOLAR_LITHIUM
#define DS18B20_SENSOR
#define DS18B20_SENSOR2
#define ONE_WIRE_BUS GPIO10
bool noGPS = true;
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif

#ifdef CORNINO
#define SOLAR_12V
#define ADC_VOLTAGE_FACTOR 3.7593
#define DS18B20_SENSOR
#define ONE_WIRE_BUS GPIO10
bool noGPS = true;
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif  

#ifdef LIZARD
#define SOLAR_SUPERCAP
#define AIR530_GPS
#define SOLAR_SUPERCAP_PWR
#define ADC_VOLTAGE_FACTOR 1.7380

#define DS18B20_SENSOR
#define DS18B20_SENSOR2
#define ONE_WIRE_BUS GPIO10

#define APPLICATION_PORT 20

bool noGPS = true;
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif  

#ifdef SQUIRREL
#define SOLAR_SUPERCAP
#define AIR530_GPS
//#define LOW_POWER_START
#define SOLAR_SUPERCAP_PWR
#define ADC_VOLTAGE_FACTOR 1.7637
#define DHT22_SENSOR
#define DHTPIN GPIO5 
#define DS18B20_SENSOR
#define DS18B20_SENSOR2
#define ONE_WIRE_BUS GPIO11

bool noGPS = true;
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif  

#ifdef BALLON3
#define SOLAR_SUPERCAP
#define DS18B20_SENSOR
#define ONE_WIRE_BUS GPIO10
#define AIR530_GPS
#define AIR530_TRACKING

bool noGPS = false;
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif  

#ifdef SLOTH
#define LITHIUM_HIGH_CAPACITY
#define DHT22_SENSOR
#define DHTPIN GPIO5 
//#define DS18B20_SENSOR
//#define DS18B20_SENSOR2
//#define ONE_WIRE_BUS GPIO11

bool noGPS = false;
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif  

#ifdef DOLPHIN
#define LITHIUM_HIGH_CAPACITY
#define ADC_VOLTAGE_FACTOR 1.181

#define DS18B20_CALIBRATION  0.9654
#define DS18B20_CALIBRATION2 1
#define DS18B20_SENSOR
#define DS18B20_SENSOR2
#define ONE_WIRE_BUS GPIO11

#define DHT22_CALIBRATION  1.0045
#define DHT22_CALIBRATION_H  1
#define DHT22_SENSOR
#define DHTPIN GPIO5

uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif  


#ifdef SDB
#define TX_DUTY_CYCLE 30
#define LITHIUM_HIGH_CAPACITY
#define ADC_VOLTAGE_FACTOR 1.189

#define BME280_SENSOR
#define BME280_CALIBRATION_H  1
#define BME280_CALIBRATION_P  1.018090

uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif  


#ifdef THERMOSTAT1
#define THERMOSTAT
#define THERMOSTAT_DEFAULT_SET 2300
#define LITHIUM_HIGH_CAPACITY
#define ADC_VOLTAGE_FACTOR 1.180

#define DHT22_CALIBRATION  1.004
#define DHT22_CALIBRATION_H  1.107
#define DHT22_SENSOR
#define DHTPIN GPIO11

#define DEVICE_RESTART_PERIOD 1

#define APPLICATION_PORT 40

uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif


#ifdef THERMOSTAT2
#define THERMOSTAT
#define THERMOSTAT_DEFAULT_SET 1950
#define LITHIUM_HIGH_CAPACITY
#define ADC_VOLTAGE_FACTOR 1.009
#define BATT_READ_DIRECT

#define DHT22_CALIBRATION  1
#define DHT22_CALIBRATION_H  1
#define DHT22_SENSOR
#define DHTPIN GPIO11

#define DEVICE_RESTART_PERIOD 1

#define APPLICATION_PORT 40

uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif



#ifdef SOLARSUPERCAP
#define TX_DUTY_CYCLE 30
#define SOLAR_SUPERCAP
#define ADC_VOLTAGE_FACTOR 1.7543

#define BME280_SENSOR
#define BME280_CALIBRATION_T  0.9843
#define BME280_CALIBRATION_H  1
#define BME280_CALIBRATION_P  1.019277

uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif  





#ifdef AIR530_GPS
//#include "GPS_Air530.h" // Enable this for board version 1.0 and 1.0_1
#include "GPS_Air530Z.h" // Enable this for board version 1.1
Air530ZClass GPS;
#endif

#ifdef BME280_SENSOR
#include "Seeed_BME280.h"
#include <Wire.h>
BME280 bme280;
#endif

#ifdef DHT22_SENSOR
#include <Adafruit_Sensor.h>
#include <DHT.h>
DHT dht(DHTPIN, DHT22);
#endif

#ifdef DS18B20_SENSOR
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#endif

#ifdef LOW_POWER_START
static TimerEvent_t sleep;
static TimerEvent_t wakeUp;
uint8_t lowpower=1;
#endif

uint8_t nwkSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint32_t devAddr =  ( uint32_t )0x00000000;
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t  loraWanClass = LORAWAN_CLASS;
bool overTheAirActivation = LORAWAN_NETMODE;
bool loraWanAdr = LORAWAN_ADR;
bool keepNet = LORAWAN_NET_RESERVE;
bool isTxConfirmed = LORAWAN_UPLINKMODE;
uint8_t confirmedNbTrials = 4;
bool sleepMode = false;

int voltageBattMV = 0;
int voltageBattADC = 0;
bool startedGPS = false;
int temperatureCentieme0 = 0;
int temperatureCentieme = 0;
int temperatureCentieme2 = 0;
int humidityCentieme0 = 0;
int pressurePa = 0;
int deviceRestartPeriod = DEVICE_RESTART_PERIOD;
#ifdef THERMOSTAT
int thermostatTemperatureCenti = THERMOSTAT_DEFAULT_SET;
bool heatingOrder = false;
#endif

#ifdef DEBUG
uint32_t appTxDutyCycle = 1;  
#else
uint32_t appTxDutyCycle = TX_DUTY_CYCLE;  
#endif


uint8_t appPort = APPLICATION_PORT;
int frameCount = 1;
int cycleCount = 999;
int cycleInterval = 1;


/////////// HANDLE DOWNLINKS ////////////
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

  frameCount = 0; //Auto-restart Reset

  uint8_t cmd = mcpsIndication->Buffer[0];

  if (mcpsIndication->Port == 3) 
  {
    appTxDutyCycle = cmd;
  }

  if (mcpsIndication->Port == 5) 
  {
    LoRaWAN.setDataRateForNoADR(cmd);
  }

  if (mcpsIndication->Port == 199) 
  {
    deviceRestartPeriod = cmd;
  }

//cycleInterval

  #ifdef THERMOSTAT
  if (mcpsIndication->Port == 40) 
  {
    thermostatTemperatureCenti = cmd * 10;
  }
  #endif


}



static void prepareTxFrame()
{

  unsigned char *puc;
  appDataSize = 0;

  #ifdef BME280_SENSOR
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(10);
  if(bme280.init()){
    delay(100);
    temperatureCentieme0 = bme280.getTemperature() * 100 * BME280_CALIBRATION_T;
    humidityCentieme0 = bme280.getHumidity() * 100 * BME280_CALIBRATION_H;
    pressurePa = bme280.getPressure() * BME280_CALIBRATION_P;
  }
  Wire.end();
  digitalWrite(Vext, HIGH);
  #endif

  #ifdef DS18B20_SENSOR
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(50);
  sensors.begin();
  sensors.setResolution(12);
  sensors.requestTemperatures();
  temperatureCentieme = sensors.getTempCByIndex(0) * 100 * DS18B20_CALIBRATION;
  #ifdef DS18B20_SENSOR2
  temperatureCentieme2 = sensors.getTempCByIndex(1) * 100 * DS18B20_CALIBRATION2;
  #endif
  pinMode(ONE_WIRE_BUS, OUTPUT);
  digitalWrite(ONE_WIRE_BUS, LOW);
  digitalWrite(Vext, HIGH);
  #endif

  #ifdef DHT22_SENSOR
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(50);
  #ifdef THERMOSTAT
  delay(1200);
  #endif
  dht.begin();
  temperatureCentieme0 = dht.readTemperature() * 100 * DHT22_CALIBRATION;
  humidityCentieme0 = dht.readHumidity() * 100 * DHT22_CALIBRATION_H;
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, LOW);
  digitalWrite(Vext, HIGH);
  #endif

  #ifdef THERMOSTAT
  if (thermostatTemperatureCenti > temperatureCentieme0)
  {
    pinMode(GPIO5, OUTPUT);
    digitalWrite(GPIO5, HIGH);
    heatingOrder = true;
  }
  else
  {
    pinMode(GPIO5, OUTPUT);
    digitalWrite(GPIO5, LOW);
    heatingOrder = false;
  }
  #endif

  #ifdef SOIL_SENSOR
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(1000);
    voltageBattADC = analogRead(ADC3);
  digitalWrite(Vext, HIGH);
  #endif

  #ifdef AIR530_TRACKING
  if (startedGPS == true && noGPS == false)
  {
    appPort = 21;
      
    uint32_t  lat, lon;
    int       alt, sats;
    
    uint32_t start = millis();
    while( (millis()-start) < 5000 )
    {
      while (GPS.available() > 0) { GPS.encode(GPS.read()); }
      if (GPS.location.age() < 2000 ) { break; }
    }
    if(GPS.location.age() < 2000)
    {
      start = millis();
      while( (millis()-start) < 5000 ) //gps stabilize
      {
        while (GPS.available() > 0) { GPS.encode(GPS.read()); }
      }
    }
  
    lat     = ((GPS.location.lat() + 90) / 180.0) * 16777215;
    lon     = ((GPS.location.lng() + 180) / 360.0) * 16777215;
    alt     = GPS.altitude.meters();
    sats    = GPS.satellites.value();
  
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
  
    appData[appDataSize++] = (uint8_t)sats;
  
    int voltageBattEncodedSmall = ( voltageBattMV - 1500 ) / 20;
    appData[appDataSize++] = (uint8_t)(voltageBattEncodedSmall);
  
    #ifdef DS18B20_SENSOR
      int temperatureEncodedSmall = ( (temperatureCentieme / 100) + 55 ) * 2;
      appData[appDataSize++] = (uint8_t)(temperatureEncodedSmall);
    #endif
  }

  else
  {
    appPort = 20;
  
    appData[appDataSize++] = (uint8_t)(voltageBattMV >> 8);
    appData[appDataSize++] = (uint8_t)(voltageBattMV);

    #ifdef DS18B20_SENSOR
      int temperatureEncodedLarge = (temperatureCentieme) + 10000;
      appData[appDataSize++] = (uint8_t)(temperatureEncodedLarge >> 8);
      appData[appDataSize++] = (uint8_t)(temperatureEncodedLarge);
    #endif
  }
  #endif

    appPort = APPLICATION_PORT;

    appData[appDataSize++] = (uint8_t)(voltageBattMV >> 8);
    appData[appDataSize++] = (uint8_t)(voltageBattMV);

    #ifdef SOIL_SENSOR
    //appPort = 23;
    appData[appDataSize++] = (uint8_t)(voltageBattADC >> 8);
    appData[appDataSize++] = (uint8_t)(voltageBattADC);
    #endif

    #if defined(DHT22_SENSOR) || defined(BME280_SENSOR)
      if (temperatureCentieme0 == 2147483647) appPort = 99;
      if (humidityCentieme0 == 2147483647) appPort = 99;
    
      appData[appDataSize++] = (uint8_t)((temperatureCentieme0 + 10000) >> 8);
      appData[appDataSize++] = (uint8_t)((temperatureCentieme0 + 10000));

      appData[appDataSize++] = (uint8_t)(humidityCentieme0 >> 8);
      appData[appDataSize++] = (uint8_t)(humidityCentieme0);
    #endif

    #ifdef BME280_SENSOR
      //appPort = 24;
      appData[appDataSize++] = (uint8_t)((pressurePa / 10) >> 8);
      appData[appDataSize++] = (uint8_t)((pressurePa / 10));
    #endif

    #ifdef DS18B20_SENSOR
      //appPort = 20;
      if (temperatureCentieme == 0) appPort = 99;
      appData[appDataSize++] = (uint8_t)((temperatureCentieme + 10000) >> 8);
      appData[appDataSize++] = (uint8_t)((temperatureCentieme + 10000));
    #endif

    #ifdef DS18B20_SENSOR2
      if (temperatureCentieme2 == 0) appPort = 99;
      appData[appDataSize++] = (uint8_t)((temperatureCentieme2 + 10000) >> 8);
      appData[appDataSize++] = (uint8_t)((temperatureCentieme2 + 10000));
    #endif



    #ifdef THERMOSTAT
      //appPort = 40;
      appData[appDataSize++] = (uint8_t)(thermostatTemperatureCenti / 10);
      appData[appDataSize++] = (uint8_t)(heatingOrder);
    #endif


}

#ifdef LOW_POWER_START
void onSleep()
{
  #ifdef DEBUG
  Serial.println("LOW POWER   ");
#endif
  lowpower=1;
  TimerSetValue( &wakeUp, 60000 );
  TimerStart( &wakeUp );
}
void onWakeUp()
{
  #ifdef DEBUG
  Serial.println("WAKING UP");
  #endif
  lowpower=0;
}
#endif

void setup()
{
  boardInitMcu();

  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  #ifdef LOW_POWER_START
  TimerInit( &sleep, onSleep );
  TimerInit( &wakeUp, onWakeUp );
  onSleep();
  #endif
}

void loop()
{
  #ifdef SOLAR_SUPERCAP
  voltageBattADC = analogRead(ADC3);
  voltageBattMV = voltageBattADC / ADC_VOLTAGE_FACTOR; // 0-7v.20k10k:1.74-1.77  10k10k:1.171

  int analogReadX = 0;
  for(int i=0;i<50;i++) analogReadX+=analogRead(ADC3);
	voltageBattMV = ( analogReadX / 50 ) * ADC_VOLTAGE_FACTOR;


  //uint32_t appTxDutyCycle = 60000;  
  //uint32_t appTxDutyCycle = 10000;  
  //cycleInterval = 1;
  #endif

  #ifdef SOLAR_SUPERCAP_PWR
  if (voltageBattMV < 2000) { appTxDutyCycle = 15; cycleInterval = 4; }
  if (voltageBattMV > 2000) { appTxDutyCycle = 15; cycleInterval = 2; }
  if (voltageBattMV > 3000) { appTxDutyCycle = 12; cycleInterval = 1; }
  if (voltageBattMV > 5500) { appTxDutyCycle = 1; cycleInterval = 6; }

  #ifdef AIR530_GPS
    if (voltageBattMV <= 5500 && startedGPS == true) // 3000
    {
      GPS.end(); 
      startedGPS = false;
      LoRaWAN.setDataRateForNoADR(5);
    }
    if (voltageBattMV >= 6000 && startedGPS == false) // 4000
    {
      GPS.begin(9600);
      GPS.setNMEA(NMEA_GGA);
      startedGPS = true;
      LoRaWAN.setDataRateForNoADR(0);
    }
  #endif
  #endif

  #ifdef LOW_POWER_START
  if(lowpower){
    lowPowerHandler();
  }
  #endif
  
  #ifdef SOLAR_LITHIUM
  voltageBattMV = getBatteryVoltage();
  
  if (voltageBattMV < 3300) { cycleInterval = 20; }
  if (voltageBattMV > 3500) { cycleInterval = 10; }
  if (voltageBattMV > 3700) { cycleInterval = 2; }
  #endif
    
  #ifdef SOLAR_12V
  int analogReadX = 0;
  for(int i=0;i<100;i++) analogReadX+=analogRead(ADC3);
	voltageBattMV = ( analogReadX / 100 ) * ADC_VOLTAGE_FACTOR;
  #endif

  #ifdef LITHIUM_HIGH_CAPACITY
    #ifdef BATT_READ_DIRECT
      voltageBattMV = getBatteryVoltage() * ADC_VOLTAGE_FACTOR;
    #else
      int analogReadX = 0;
      for(int i=0;i<50;i++) analogReadX+=analogRead(ADC1);
      voltageBattMV = ( analogReadX / 50 ) * ADC_VOLTAGE_FACTOR;
    #endif

    cycleInterval = 1;

    if (voltageBattMV < 3300) { cycleInterval = 10; }
    if (voltageBattMV < 3400) { cycleInterval = 5; }
    else { cycleInterval = 1; }
  #endif
  
  #ifdef LITHIUM_KEEPER
  if (voltageBattMV < 3850 && startedGPS == true)
  {
    GPS.end(); 
    startedGPS = false;
  }
  if (voltageBattMV > 3950 && startedGPS == false)
  {
    GPS.begin(9600);
    GPS.setNMEA(NMEA_GGA);
    startedGPS = true;
  }
  #endif








  
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
      #ifdef DEBUG
      Serial.println("INIT");
      #endif
      
      LoRaWAN.init(loraWanClass,loraWanRegion);
      
      deviceState = DEVICE_STATE_JOIN;
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      #ifdef DEBUG
      Serial.println("JOIN");
      #endif
      
      LoRaWAN.join();
      break;
    }
    case DEVICE_STATE_SEND:
    {
      int cyclesPerDay = (8640 / ((appTxDutyCycle) * cycleInterval));

      frameCount++;
      if (frameCount > (cyclesPerDay * deviceRestartPeriod)) CySoftwareReset();
  

      cycleCount++;

      #ifdef DEBUG
      Serial.println(" ");   

      Serial.print(cyclesPerDay);
      Serial.println(" cyclesPerDay");   

      Serial.print(voltageBattMV);
      Serial.println(" voltageBattMV");

      Serial.print(getBatteryVoltage());
      Serial.println(" getBatteryVoltage");

      Serial.print(analogRead(ADC1));
      Serial.println(" analogRead(ADC1)");

      Serial.print(analogRead(ADC2));
      Serial.println(" analogRead(ADC2)");

      Serial.print(analogRead(ADC3));
      Serial.println(" analogRead(ADC3)");
      
      Serial.print(cycleCount);
      Serial.println(" cycleCount");

      Serial.print(cycleInterval);
      Serial.println(" cycleInterval");

      #ifdef DS18B20_SENSOR
      Serial.print(temperatureCentieme);
      Serial.println(" temperatureCentieme");
      #endif
      #ifdef DS18B20_SENSOR2
      Serial.print(temperatureCentieme2);
      Serial.println(" temperatureCentieme2");
      #endif
      
      #if defined(DHT22_SENSOR) || defined(BME280_SENSOR)
      Serial.print(temperatureCentieme0);
      Serial.println(" temperatureCentieme0");

      Serial.print(humidityCentieme0);
      Serial.println(" humidityCentieme0");
      #endif

      #ifdef BME280_SENSOR
      Serial.print(pressurePa);
      Serial.println(" pressurePa");
      #endif
  



      #endif
      if (cycleCount >= cycleInterval)
      {
      #ifdef DEBUG
      Serial.println("PREPARETXFRAME");
      #endif
      cycleCount = 0;
      
      prepareTxFrame();

      #ifdef DEBUG
      Serial.println("SEND");
      #endif

      LoRaWAN.send();
      
      deviceState = DEVICE_STATE_CYCLE;
      break;
      }
      else
      {
      #ifdef DEBUG
      Serial.println("WAITSEND");
      #endif
      }
    }
    case DEVICE_STATE_CYCLE:
    {
      #ifdef DEBUG
      Serial.println("CYCLE");
      #endif
      
      // Schedule next packet transmission
      if (!sleepMode)
      {
         LoRaWAN.cycle(appTxDutyCycle * 10000);
      }
      deviceState = DEVICE_STATE_SLEEP;
      break;
    }
    case DEVICE_STATE_SLEEP:
    {
      #ifdef DEBUG
      Serial.print("Z");
      #endif
      
      LoRaWAN.sleep();   
      break;
    }
    default:
    {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
}