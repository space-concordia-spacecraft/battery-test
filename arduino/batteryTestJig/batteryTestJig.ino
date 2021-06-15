#include <SPI.h>

// Pins
#define PIN_BUZZER 3

#define PIN_CELL_A_DIR 9
#define PIN_CELL_A_CHARGE 8

#define PIN_CELL_B_DIR 4
#define PIN_CELL_B_CHARGE 5

#define PIN_BATT_SEL 6;

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

/** 
 *  Function to get voltage depending on a value
 */
float getVolts(uint16_t value){  
  return (((float)value/4096)*5);  
}

/**
 * Function to read SPI data depending on the channel (0-7)
 */
int readSPI(int chan) {
  digitalWrite(SPI_ADC_CS, LOW);
  int hi = SPI.transfer( chan << 3 );
  int lo = SPI.transfer( 0 );
  digitalWrite(SPI_ADC_CS, HIGH);
  
  return getVolts(((hi << 8) | lo) & 0xFFF);
}


/**
 * Function to charge/discharge cell B
 */
void chargeCellA(bool charge) {
  if(charge) {
    digitalWrite(PIN_CELL_A_CHARGE, HIGH);
    digitalWrite(PIN_CELL_A_DIR, LOW);
  } else {
    digitalWrite(PIN_CELL_A_CHARGE, LOW);
    digitalWrite(PIN_CELL_A_DIR, HIGH);
    digitalWrite(PIN_BATT_SEL, HIGH);
  }
}

/**
 * Function to charge/discharge cell B
 */
void chargeCellB(bool charge) {
  if(charge) {
    digitalWrite(PIN_CELL_B_CHARGE, HIGH);
    digitalWrite(PIN_CELL_B_DIR, LOW);
  } else {
    digitalWrite(PIN_CELL_B_CHARGE, LOW);
    digitalWrite(PIN_CELL_B_DIR, HIGH);
    digitalWrite(PIN_BATT_SEL, LOW);
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

  SPI.begin();
  SPI.setBitOrder(MSBFIRST); // this could be MSBFIRST or LSBFIRST
  SPI.setDataMode(SPI_MODE2);  // Mode=2 CPOL=1, CPHA=0
}

void loop() {

  // Puts all data into a string
  channelData = "";
  channelData = readSPI(CHAN_A_TM) + "," 
              + readSPI(CHAN_A_IM) + "," 
              + readSPI(CHAN_A_VM) + ","
              + readSPI(CHAN_B_TM) + ","
              + readSPI(CHAN_B_IM) + ","
              + readSPI(CHAN_B_VM) + ","
              + readSPI(CHAN_VREF) + ","
              + readSPI(CHAN_JIG_TM);

  // Serial prints it so that the computer can read it
  Serial.println(channelData);

  // If there is a command available, parse it
  if(Serial.available()){
    command = Serial.readStringUntil('\n');
         
    if(command.equals("charge_a")){
      chargeCellA(true);
    }
    else if(command.equals("charge_b")){
      chargeCellB(true);
    }
  }
}
