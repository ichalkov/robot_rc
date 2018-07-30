#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

const int FORWARD = 1;
const int BACK = 0;
const int LEFT = 0;
const int RIGHT = 1;

//первый двигатель
int enA = 6;
int in1 = 7;
int in2 = 8;

// второй двигатель
int enB = 3;
int in3 = 5;
int in4 = 4;

RF24 radio(9,10); // "создать" модуль на пинах 9 и 10 Для Уно

byte address[][6] = {"1Node","2Node","3Node","4Node","5Node","6Node"};  //возможные номера труб
byte pipeNo;
int recieved_data[3];


void setup(){
  Serial.begin(9600); //открываем порт для связи с ПК

  // инициализируем все пины для управления двигателями
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  radio.begin();              //активировать модуль
  radio.setAutoAck(1);        //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0,15);     //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();   //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);   //размер пакета, в байтах

  radio.openReadingPipe(1,address[0]);      //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  
  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
}

//Движение вперед - назад
void go(int dir, int sp)
{
  //вперед
  if (dir == FORWARD) {
    // запуск двигателя A
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    // запуск двигателя B
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }

  //назад
  if (dir == BACK) {
    // запуск двигателя A
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    // запуск двигателя B
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);  
  }
  // устанавливаем скорость 
  analogWrite(enA, sp);
  analogWrite(enB, sp); 
}

void turn(int dir) {
  //влево
  if (dir == LEFT) {
    analogWrite(enA, 0);
  }

  //вправо
  if (dir == RIGHT) {
    analogWrite(enB, 0);
  }
}

void loop() {                         
    while( radio.available(&pipeNo)){    // слушаем эфир со всех труб
      radio.read( &recieved_data, sizeof(recieved_data) );         // чиатем входящий сигнал
      int xVal = recieved_data[0];
      int yVal = recieved_data[1];
      int sp = 150;

      Serial.println(yVal);
      // UP - DOWN
      //if (yVal == 526) {
      //  go(FORWARD, 0);
      //}
      go(FORWARD, 0);
  
      if (yVal<520) {
        //sp = map(yVal,503,0,0,255);
        go(FORWARD, sp);
      }
      if (yVal>530) {
        //sp = map(yVal,503,1023,0,255);
        go(BACK, sp);
      }

      // LEFT - RIGHT 
      if (xVal<100) {
        turn(LEFT);
      }
      if (xVal>800) {
        turn(RIGHT);
      }
     
   }
}
