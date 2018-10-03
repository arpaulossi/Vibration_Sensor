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
#define R_READ B10000000

#include <SPI.h>
#include <LoRa.h>

static const int spiClk = 1600000;
SPIClass * vspi = NULL;
SPIClass * hspi = NULL;

int16_t DataX, DataY, DataZ = 0;

void initKx122(){
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(5, LOW);
  delay(15);
  vspi->transfer(CNTL1);
  vspi->transfer(0x60);
  digitalWrite(5, HIGH);
  delay(15);

  digitalWrite(5, LOW);
  vspi->transfer(INC1);
  vspi->transfer(0x38);
  digitalWrite(5, HIGH);
  delay(15);

  digitalWrite(5, LOW);
  vspi->transfer(INC4);
  vspi->transfer(0x10);
  digitalWrite(5, HIGH);
  delay(15);
  
  digitalWrite(5, LOW);
  vspi->transfer(ODCNTL);
  vspi->transfer(0x07);
  digitalWrite(5, HIGH);
  delay(15);

  digitalWrite(5, LOW);
  vspi->transfer(CNTL1);
  vspi->transfer(0xE0);
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
    vspi->transfer(XOUT_L | R_READ);
    outAxis_L = vspi->transfer(0x00);
    vspi->transfer(XOUT_H | R_READ);
    outAxis_H = vspi->transfer(0x00);
  }
  else if(axis == 'Y'){
    vspi->transfer(YOUT_L | R_READ);
    outAxis_L = vspi->transfer(0x00);
    vspi->transfer(YOUT_H | R_READ);
    outAxis_H = vspi->transfer(0x00);
  }
  else if(axis == 'Z'){
    vspi->transfer(ZOUT_L | R_READ);
    outAxis_L = vspi->transfer(0x00);
    vspi->transfer(ZOUT_H | R_READ);
    outAxis_H = vspi->transfer(0x00);
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
  LoRa.begin(915E6);

  initKx122();
  Serial.println("Teste de leitura KX122");
}

void loop() {
  DataX = readAxis('X');
  DataY = readAxis('Y');
  DataZ = readAxis('Z');

  LoRa.beginPacket();
  LoRa.print(DataX);
  LoRa.print(DataY);
  LoRa.print(DataZ);
  LoRa.endPacket();
  
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
