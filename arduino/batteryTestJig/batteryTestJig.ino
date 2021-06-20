#include <SPI.h>

// Pins
#define PIN_BUZZER 3

#define PIN_CELL_A_DIR 9
#define PIN_CELL_A_CHARGE 8

#define PIN_CELL_B_DIR 4
#define PIN_CELL_B_CHARGE 5

#define PIN_BATT_SEL 6

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
#define CHAN_VREF A1
#define CHAN_JIG_TM 7

#define CHARGE 0
#define DISCHARGE 1
#define SLEEP 2

float vref = 5.0f;

/** 
 * Function to get voltage depending on a value
 */
float getVolts(uint16_t value){  
  return (((float)value/4096)*vref);  
}

/**
 * Function to read SPI data depending on the channel (0-7)
 */
float readSPI(int chan) {
  digitalWrite(SPI_ADC_CS, LOW);
  int hi = SPI.transfer( chan << 3 );
  int lo = SPI.transfer( 0 );
  digitalWrite(SPI_ADC_CS, HIGH);
  
  return getVolts(( (hi << 8) | lo ) << 1);
}


/**
 * Function to charge/discharge cell A
 */
void changeCellAState(int state) {
  if(state == CHARGE) {
    digitalWrite(PIN_CELL_A_CHARGE, HIGH);
    digitalWrite(PIN_CELL_A_DIR, LOW);    
  } else if(state == DISCHARGE) {
    digitalWrite(PIN_CELL_A_CHARGE, LOW);
    digitalWrite(PIN_CELL_A_DIR, HIGH);
    digitalWrite(PIN_BATT_SEL, HIGH);
  } else {
    digitalWrite(PIN_CELL_A_DIR, LOW);
    digitalWrite(PIN_CELL_A_CHARGE, LOW);
  }
}

/**
 * Function to charge/discharge cell B
 */
void changeCellBState(int state) {
  if(state == CHARGE) {
    digitalWrite(PIN_CELL_B_CHARGE, HIGH);
    digitalWrite(PIN_CELL_B_DIR, LOW);
  } else if (state == DISCHARGE) {
    digitalWrite(PIN_CELL_B_CHARGE, LOW);
    digitalWrite(PIN_CELL_B_DIR, HIGH);
    digitalWrite(PIN_BATT_SEL, LOW);
  } else {
    digitalWrite(PIN_CELL_B_DIR, LOW);
    digitalWrite(PIN_CELL_B_CHARGE, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(SPI_ADC_CS, OUTPUT); // use this for SPI

  // setup pinmodes for charge pins
  pinMode(PIN_CELL_A_CHARGE, OUTPUT);
  pinMode(PIN_CELL_A_DIR, OUTPUT);
  pinMode(PIN_CELL_B_CHARGE, OUTPUT);
  pinMode(PIN_CELL_B_DIR, OUTPUT);
  pinMode(PIN_BATT_SEL, OUTPUT);

  pinMode(PIN_BUZZER, OUTPUT);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST); // this could be MSBFIRST or LSBFIRST
  SPI.setDataMode(SPI_MODE2);  // Mode=2 CPOL=1, CPHA=0

  analogReference(INTERNAL2V5);
}

void loop() {

  vref = analogRead(CHAN_VREF)*(3.0607 * 2.5 / 1024.0);
  
  for (int i=0; i<8; i++) {
    int chan = (i == 7) ? 0 : i+1;
    float val = (i == 6) ? vref : readSPI(chan);
    Serial.print(val);
    if(i != 7)
      Serial.print(",");
  }
  Serial.println(";");
  
  // If there is a command available, parse it
  if(Serial.available()){
    String command = Serial.readStringUntil('\n');
    
    if(command.equals("charge_a")){
      changeCellAState(CHARGE);
    } else if(command.equals("charge_b")){
      changeCellBState(CHARGE);
    } else if(command.equals("discharge_a")){
      changeCellAState(DISCHARGE);
    } else if(command.equals("discharge_b")){
      changeCellBState(DISCHARGE);
    } else if(command.equals("idle_a")) {
      changeCellAState(SLEEP);
    } else if(command.equals("idle_b")) {
      changeCellBState(SLEEP);
    }
  }
}

/*
    SS – digital 10
    MOSI – digital 11
    MISO – digital 12
    SCK – digital 13
*/

//
//#include "SPI.h" // necessary library
////int SS=10; // using digital pin 10 for SPI slave select
//
//void setup()
//{
//  Serial.begin(9600);
//  
//  pinMode(10, OUTPUT); // we use this for SS pin
//
//  pinMode(PIN_CELL_A_CHARGE, OUTPUT);
//  pinMode(PIN_CELL_A_DIR, OUTPUT);
//
//  digitalWrite(PIN_CELL_A_CHARGE, HIGH);
//  digitalWrite(PIN_CELL_A_DIR, LOW);
//  
//  SPI.begin();
//  SPI.setBitOrder(MSBFIRST);
//  SPI.setDataMode(SPI_MODE2);  // Mode=2 CPOL=1, CPHA=0
//}
//
///*
//** the value associated with chan will be returned 
//** during the next call to this function.
//*/
//int readADC(int chan)
//{
//  digitalWrite(10, LOW);
//  int hi = SPI.transfer( chan << 3 );
//  int lo = SPI.transfer( 0 );
//  digitalWrite(10, HIGH);
//  
//  return ((hi << 8) | lo) << 1;
//}
//
//void loop()
//{ 
//  for (int i=0; i<8; i++) {
//    int chan = (i == 7) ? 0 : i+1;
//    float val = getVolts(readADC(i));
//    Serial.print(val);
//    Serial.print("\t");
//  }
//  Serial.println("");
//  
//}
