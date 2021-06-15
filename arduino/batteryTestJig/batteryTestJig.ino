#include <SPI.h>

// Pins
#define PIN_BUZZER 3

#define PIN_CELL_A_DIR 9
#define PIN_CELL_A_CHARGE 8

#define PIN_CELL_B_DIR 4
#define PIN_CELL_B_CHARGE 5

// SPI
#define SPI_ADC_CS 10
#define SPI_MISO 12
#define SPI_MOSI 11

// Channels
#define CHAN_A_TM 0
#define CHAN_A_IM 1
#define CHAN_A_VM 2
#define CHAN_B_TM 3
#define CHAN_B_IM 4
#define CHAN_B_VM 5
#define CHAN_VREF 6
#define CHAN_JIG_TM 7

unsigned long currentMillis;
String channelData;

// Charge, Discharge, Charge, Discharge, Charge
int cellAChargeCount[];
int cellBChargeCount[];

boolean cellACharging;
boolean cellBCharging;

/** 
 *  getVolts()
 *  transforms value into volts
 *  returns - voltage
 */
float getVolts(uint16_t value){  
  return (((float)value/4096)*5);  
}

int readSPI(int chan) {
  digitalWrite(SPI_ADC_CS, LOW);
  int hi = SPI.transfer( chan << 3 );
  int lo = SPI.transfer( 0 );
  digitalWrite(SPI_ADC_CS, HIGH);
  
  return getVolts(((hi << 8) | lo) & 0xFFF);
}

void setup() {
  Serial.begin(9600);
  pinMode(SPI_ADC_CS, OUTPUT); // we use this for SS pin
  
  pinMode(CELL_B_CHARGE, OUTPUT);
  pinMode(CELL_A_CHARGE, OUTPUT);

  digitalWrite(CELL_B_CHARGE, HIGH);
  digitalWrite(CELL_A_CHARGE, HIGH);
 
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE2);  // Mode=2 CPOL=1, CPHA=0

  cellAChargeCount = [2];
  cellBChargeCount = [2];
}

void loop() {
  
  int val = readSPI(CHAN_A_TM);
  channelData = "";

  channelData = readSPI(CHAN_A_TM) + "," 
              + readSPI(CHAN_A_IM) + "," 
              + readSPI(CHAN_A_VM) + ","
              + readSPI(CHAN_B_TM) + ","
              + readSPI(CHAN_B_IM) + ","
              + readSPI(CHAN_B_VM) + ","
              + readSPI(CHAN_VREF) + ","
              + readSPI(CHAN_JIG_TM);
  
  Serial.println(channelData);

  if(Serial.available()){
    command = Serial.readStringUntil('\n');
         
    if(command.equals("start")){
      currentMillis = millis();
      initialize();
    }
    else if(command.equals("stop")){
      stop_charging();
    }
    else if(command.equals("pause"){
      
    }
  }
}

void initialize() {
  cellACharging = true;
  cellBCharging = false;

  // {Charge count, Discharge count}
  cellAChargeCount = {0,0};
  cellBChargeCount = {0,0};
}

void stop_charging() {
  
}
