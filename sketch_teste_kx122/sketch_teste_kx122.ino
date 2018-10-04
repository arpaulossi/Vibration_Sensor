#define CNTL1 0x18
#define INC1 0x1C
#define INC4 0x1F
#define CNTL2 0x19
#define ODCNTL 0x1B
#define WHO_AM_I 0x0F
#define COTR 0x0C
#define AXIS_X 0x06
#define AXIS_Y 0x08
#define AXIS_Z 0x0A
#define R_READ 0x80
#define csKx 5
#define csLora 15

#include <SPI.h>

static const int spiClk = 1000000; // 1MHz
SPIClass * vspi = NULL;
SPIClass * hspi = NULL;

int16_t DataX, DataY, DataZ = 0; // Variaveis para guardar valores raw

void writeTwoBytes(int address, int dataW){  // Escreve dois bytes via SPI, utilizado para gravar dados nos registradores
  byte buf[2];
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(csKx, LOW);
  buf[0]=address;
  buf[1]=dataW;
  vspi->writeBytes(buf, 2);
  digitalWrite(csKx, HIGH);
  vspi->endTransaction();
}

int getByte(int address){ // Lê um byte de um registrador
  byte buf_w[2];
  byte buf_r[2];
  uint32_t BUF_SIZE = 2;
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(csKx, LOW);
  buf_w[0] = address | R_READ;
  vspi->transferBytes(buf_w, buf_r, BUF_SIZE);
  digitalWrite(csKx, HIGH);
  vspi->endTransaction();
  return buf_r[1];
}

int16_t getRawData(int axis){ // Retorna dado raw de um eixo
  return ((int16_t)((getByte(axis + 1) << 8) | (getByte(axis))));
}

void softwareResetKx122(){ // Executa Software Reset do KX122
  delay(50);
  writeTwoBytes(0x7F, 0x00);
  writeTwoBytes(CNTL2, 0x00);
  writeTwoBytes(CNTL2, 0x80);
  delay(150);
  byte who = getByte(WHO_AM_I);
  if(who == 0x1B){
    byte noErr = getByte(COTR);
    if(noErr != 0x55){
      Serial.println("Software Reset is failed, reboot device...");
      while(1);
    }else{
      Serial.println("Software Reaset complete!");
    }
  }else{
    Serial.println("Error ocurred in Software Reset, reboot device...");
    while(1);
  }
  delay(50);
}

void initKx122(){ // Executa inicialização do KX122
  Serial.println("Initializing KX122...");
  writeTwoBytes(CNTL1, 0x60);
  writeTwoBytes(INC1, 0x38);
  writeTwoBytes(INC4, 0x10);
  writeTwoBytes(ODCNTL, 0x02);
  writeTwoBytes(CNTL1, 0xE0);
  delay(30);
  Serial.println("Full BOOT!");
}

void setup() {
  vspi = new SPIClass(VSPI);
  hspi = new SPIClass(HSPI);

  vspi->begin();
  hspi->begin();
  
  pinMode(csKx, OUTPUT);
  pinMode(csLora, OUTPUT);

  Serial.begin(115200);
  delay(10);
  Serial.println("Teste de leitura KX122");

  softwareResetKx122();
  initKx122();
  delay(50);
}

void loop() {
  DataX = getRawData(AXIS_X); // Lê o dado raw para o eixo X
  DataY = getRawData(AXIS_Y); // Lê o dado raw para o eixo Y
  DataZ = getRawData(AXIS_Z); // Lê o dado raw para o eixo Z

  // Manda os dados para o monitor serial
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
