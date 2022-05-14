/* Código de Controle da Estação Meteorológica

Links de Referência:
Datalogger-->https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-modulo-leitor-de-micro-sd-card
TEMT6000-->https://how2electronics.com/temt6000-ambient-light-sensor-arduino-measure-light-intensity/#Source_Code_Measuring_Illuminance
GUVA-S12SD-->https://www.arduinoecia.com.br/monte-um-medidor-de-indice-uv-com-arduino/
DS1302-->https://www.usinainfo.com.br/blog/arduino-rtc-ds1302-projeto-verificando-o-tempo/
*/
// Inclusão das Bibliotecas
#include <dht.h> 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <SPI.h>
#include <SD.h>
#include <virtuabotixRTC.h>

 

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE); //ENDEREÇO DO I2C E DEMAIS INFORMAÇÕES
//PINOS DO RTC
#define clk 5
#define dat 6
#define rst 7

// VARIAVEIS AUXILIARES
#define segundo 15
#define minuto 4
#define hora 18
#define diasemana 6
#define diadomes 13
#define mes 05
#define ano 2021

//DECLARANDO OBJETO e VARIAVEL
#define dht11 2
dht DHT11;
int temperatura=0x00, 
      umidade=0x00;

float luz; // Declarando Variavel

int indiceUV; Declarando Variavel
const int chipSelect = 4; //Pino de conexão do pino CS do modulo leitor de MicroSD

virtuabotixRTC meuRTC(clk, dat, rst); // Declarando objeto rtc

void setup() {
 lcd.begin (16,2); //SETA A QUANTIDADE DE COLUNAS(16) E O NÚMERO DE LINHAS(2) DO DISPLAY
 lcd.setBacklight(HIGH); //LIGA O BACKLIGHT (LUZ DE FUNDO)
 //AVISA SOBRE AS CONDIÇÕES DO CARTAO NO DISPLAY
 lcd.print("Iniciando SDCard");
 delay(1500);
 lcd.clear();
   if (!SD.begin(chipSelect))
  {
    lcd.print("Falha!");
    delay(1500);
    lcd.clear();
    lcd.print("Verifique as conexoes");
    delay(1500);
    lcd.clear();
    return;
  }
  lcd.print("Cartao iniciado corretamente !");
delay(2500);
lcd.clear();



//Definindo as portas Analógicas utilizadas
int pinoUV = A0;
int pinoLUZ = A1;

pinMode(pinoLUZ, INPUT);

// meuRTC.setDS1302Time(segundo, minuto, hora, diasemana,diadomes,mes,ano); AJUSTE PARA PRIMEIRO FUNCIONAMENTO DO RTC


}

void loop() {
// Solicitando a leitura do relógio RTC
meuRTC.updateTime();

// Solicitando Leitura do DHT11
DHT11.read11(dht11);
temperatura = DHT11.temperature;
umidade = DHT11.humidity;

// Solicitando Leitura do TEMT6000 e Conversões
float tensao = analogRead(A1)*5/1024.0; // converte o valor em tensão de saída
float corrente = tensao / 10000.0; // converte em corrente de saida
float microamperes=corrente*1000000; // converte a corrente em micro Amperes
float luz = microamperes*2.0; // conversão p/ lux

//Solicitando Leitura do GUVA-S12SD
int tsaida = (analogRead(A1)*5/1024.0)*1000; //Tensao de saida em milli volts 

//Estrutura condicional para atribuição do indice UV em função do intervalo de tensão de saída
  if (tsaida <= 50) {
    indiceUV = 0;
  } else if (tsaida > 50 && tsaida <= 227) {
    indiceUV = 1;
  } else if (tsaida > 227 && tsaida <= 318) {
    indiceUV = 2;
  } else if (tsaida > 318 && tsaida <= 408) {
    indiceUV = 3;
  } else if (tsaida > 408 && tsaida <= 503) {
    indiceUV = 4;
  } else if (tsaida > 503 && tsaida <= 606) {
    indiceUV = 5;
  } else if (tsaida > 606 && tsaida <= 696) {
    indiceUV = 6;
  } else if (tsaida > 696 && tsaida <= 795) {
    indiceUV = 7;
  } else if (tsaida > 795 && tsaida <= 881) {
    indiceUV = 8;
  } else if (tsaida > 881 && tsaida <= 976) {
    indiceUV = 9;
  } else if (tsaida > 976 && tsaida <= 1079) {
    indiceUV = 10;
  } else {
    indiceUV = 11;
  }
// Exibindo os dados no Display LCD (16x2 c/ I2C)
lcd.setCursor(0,0); //SETA A POSIÇÃO DO CURSOR
lcd.print("T="); //IMPRIME O TEXTO NO DISPLAY LCD
lcd.setCursor(2,0); //SETA A POSIÇÃO DO CURSOR
lcd.print(temperatura); //IMPRIME O TEXTO NO DISPLAY LCD
lcd.print("C");
lcd.setCursor(11,0);
lcd.print("UV=");
lcd.print(indiceUV);
lcd.setCursor(0,1); //SETA A POSIÇÃO DO CURSOR
lcd.print("UR="); //IMPRIME O TEXTO NO DISPLAY LCD  
lcd.print(umidade);  
lcd.print("%");  
lcd.setCursor(7,1);
lcd.print("Luz=");
lcd.print(luz,1);   
delay(1000); // EXPOSIÇÃO DOS DADOS A CADA 1 SEGUNDOS
lcd.clear();

//Estrutura de Armazenamento
File dataFile = SD.open("medidas.txt", FILE_WRITE);
  //Grava as informacoes no arquivo
  if (dataFile)
  {
    if (meuRTC.hours <10) lcd.print("0");
    dataFile.print(meuRTC.hours);
    dataFile.print(":");
    if (meuRTC.minutes <10) lcd.print("0");
    dataFile.print(meuRTC.minutes);
    dataFile.print(":");
    if (meuRTC.seconds <10) lcd.print("0");
    dataFile.print(meuRTC.seconds);
    dataFile.print(",");
    dataFile.print(temperatura);
    dataFile.print(",");
    dataFile.print(umidade);
    dataFile.print(",");
    dataFile.print(indiceUV);
    dataFile.print(",");
    dataFile.println(luz);
    dataFile.close(); //Fecha o arquivo
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Armazenado!");
    delay(1000); // AGUARDA 1 SEGUNDOS PARA REINCIAR AS MEDIDAS. AJUSTE CONFORME A NECESSIDADE
    lcd.clear();
  }
  //Exibe um erro se nao conseguir abrir o arquivo
  else
  {
    lcd.print("Erro!");
    delay(1000);
    lcd.clear();
  }

}


  
  
  
