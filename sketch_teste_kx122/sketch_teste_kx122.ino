#define CNTL1 0x18
#define INC1 0x1C
#define INC4 0x1F
#define ODCNTL 0x1B
#define XOUT_L 0x06
#define XOUT_H 0x07
#define YOUT_L 0x08
#define YOUT_H 0x09
#define ZOUT_L 0x0A
#define ZOUT_H 0x0B
#define MEAS 0x80

#include <SPI.h>

static const int spiClk = 1000000;
SPIClass * vspi = NULL;
SPIClass * hspi = NULL;

int16_t DataX, DataY, DataZ = 0;

void initKx122(){
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(5, LOW);
  delay(15);
  vspi->transfer(CNTL1);
  vspi->transfer(0x40);
  digitalWrite(5, HIGH);
  delay(15);
  
  digitalWrite(5, LOW);
  vspi->transfer(ODCNTL);
  vspi->transfer(0x02);
  digitalWrite(5, HIGH);
  delay(15);

  digitalWrite(5, LOW);
  vspi->transfer(CNTL1);
  vspi->transfer(0xC0);
  digitalWrite(5, HIGH);
  vspi->endTransaction();
}

int16_t readAxis(char axis){
  int8_t outAxis_L, outAxis_H;
  int16_t outAxisData;

  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE1));
  digitalWrite(5, LOW);
  delay(15);
  if(axis == 'X'){
    vspi->transfer(XOUT_L | B10000000);
    outAxis_L = vspi->transfer(MEAS);
    vspi->transfer(XOUT_H | B10000000);
    outAxis_H = vspi->transfer(MEAS);
  }
  else if(axis == 'Y'){
    vspi->transfer(YOUT_L | B10000000);
    outAxis_L = vspi->transfer(MEAS);
    vspi->transfer(YOUT_H | B10000000);
    outAxis_H = vspi->transfer(MEAS);
  }
  else if(axis == 'Z'){
    vspi->transfer(ZOUT_L | B10000000);
    outAxis_L = vspi->transfer(MEAS);
    vspi->transfer(ZOUT_H | B10000000);
    outAxis_H = vspi->transfer(MEAS);
  }

  digitalWrite(5, HIGH);
  vspi->endTransaction();
  delay(15);

  outAxisData = outAxis_H << 8 | outAxis_L;
  return outAxisData;
  
}

void setup() {
  vspi = new SPIClass(VSPI);
  hspi = new SPIClass(HSPI);

  vspi->begin();
  hspi->begin();

  pinMode(5, OUTPUT);
  pinMode(15, OUTPUT);

  Serial.begin(115200);

  initKx122();
  Serial.println("Teste de leitura KX122");
}

void loop() {
  DataX = readAxis('X');
  DataY = readAxis('Y');
  DataZ = readAxis('Z');
  
  Serial.print("X: ");
  Serial.print(DataX);
  Serial.print("\t");
  Serial.print("Y: ");
  Serial.print(DataY);
  Serial.print("\t");
  Serial.print("Z: ");
  Serial.println(DataZ);
  delay(500);
}
