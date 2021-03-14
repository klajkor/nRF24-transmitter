/*
* Wiring, Arduino Uno/Nano
* MOSI: 11
* MISO: 12
* SCK: 13
* CE: 7
* CSN: 8
* 
*/
#include <Arduino.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define OLED_I2C_ADDR 0x3C /* OLED module I2C address */

#define PIN_NRF24L01_CE 7
#define PIN_NRF24L01_CSN 8
#define PIPE_NUMBER 0
#define NRF24L01_COMM_CHANNEL 124

//Set up OLED display
SSD1306AsciiWire oled_display;

//create an RF24 object
RF24 radio(PIN_NRF24L01_CE, PIN_NRF24L01_CSN);

int pin_Status_Led = LED_BUILTIN;
unsigned long packet_counter = 1;

//address through which two modules communicate.
uint8_t pipe_address[6] = "N0001";

char transmit_str[12];

void Gpio_Init(void);
void Ssd1306_Oled_Init(void);
void Nrf24l01_Transmitter_Init(void);
void Nrf24l01_Cont_Transmit(void);

void setup() {
  Serial.begin(115200);
  Gpio_Init();
  Ssd1306_Oled_Init();
  Nrf24l01_Transmitter_Init();
}

void loop() {
  Nrf24l01_Cont_Transmit();
  
}

void Gpio_Init(void) {
  pinMode(PIN_NRF24L01_CE, OUTPUT);
  pinMode(PIN_NRF24L01_CSN, OUTPUT);
  pinMode(pin_Status_Led, OUTPUT);
  digitalWrite(pin_Status_Led, LOW);
}

void Ssd1306_Oled_Init(void) {
  Wire.begin();
  oled_display.begin(&Adafruit128x32, OLED_I2C_ADDR);
  oled_display.clear();
  oled_display.setFont(fixed_bold10x15);
  oled_display.setRow(0);
  oled_display.println(F("nRF24L01"));
  oled_display.println(F("Transmitter"));
  delay(3000);
  
}

void Nrf24l01_Transmitter_Init(void) {
  char *tempPointer;
  //set the address
  digitalWrite(pin_Status_Led, HIGH);
  oled_display.clear();
  oled_display.print(F("Pipe: "));
  tempPointer=(char *)&pipe_address[0];
  oled_display.println(tempPointer);
  Serial.print(F("Pipe: "));
  Serial.println(tempPointer);
  radio.begin();
  //radio.setChannel(NRF24L01_COMM_CHANNEL);
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setCRCLength(RF24_CRC_8);
  radio.disableDynamicPayloads();
  radio.setRetries(10,15);
  radio.openWritingPipe(pipe_address);
  oled_display.print(F("Open "));
  radio.stopListening();
  digitalWrite(pin_Status_Led, LOW);
  oled_display.print(F("Stop"));
  Serial.println(F("Radio init done"));
  delay(1000);
}

void Nrf24l01_Cont_Transmit(void) {
  //Read the data if available in buffer
  //oled_display.clear();
  oled_display.setCol(0);
  oled_display.setRow(0);
  oled_display.println(F("Sending    "));
  Serial.print(F("Sending: "));
  
  //unsigned long start_time = micros();
  ultoa(packet_counter,transmit_str,10);
  Serial.print(transmit_str);
  Serial.print(F(" "));
  if (!radio.write( &transmit_str, sizeof(transmit_str) )){
    oled_display.print(F("Failed    "));
    Serial.println(F("Failed"));
  }
  else {
    Serial.println(F("Sent"));
    oled_display.setCol(0);
    oled_display.setRow(0);
    oled_display.println(F("Sent      "));
    oled_display.print(F("          "));
    oled_display.setCol(0);
    oled_display.print(transmit_str);
    packet_counter++;
  }
  delay(5000);
}