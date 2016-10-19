/*
 EL3CTRIC_GUITAR - bluetooth - arduino - midi-out
 ANdrey Stepanov, 2014
 */
 
#include <SoftwareSerial.h>
#include <stdio.h>

 int note = 0; // "нота", которую мы получаем с датчика расстояния EV3
 int MIDInote = 0; // MIDI-нота, которую мы отправим в MIDI-OUT
 int instrument = 25; // начальный MIDI-инструмент (гитара)

 int tune = 57; // "строй" гитары, по умолчанию А = 57
 int tune_overdrived = 45; // "строй" гитары, по умолчанию А = 45
 

// изначально выключены перегруз и игра аккордами
 boolean chord = false, overdrive = false;

 int velocity = 127;// громкость midi-ноты, от 0 (тише) до 127 (громче)
 
 int noteON = 144;// 144 = 10010000 in binary, MIDI-команда "note on"
 int noteOFF = 128;// 128 = 10000000 in binary, MIDI-команда "note off"

// к 11 пину подключаем MIDI-OUT
SoftwareSerial mySerial(10, 11); // RX, TX 

void setup()  
{
  // открываем порт к ПК для отладки, в нормальном режиме закоментирован:
  //Serial.begin(9600);
  //Serial.println("EL3CTRIC_GUITAR midi-out");

  // Открываем MIDI-OUT:
  mySerial.begin(31250);
  // Немного ждем
  delay(100);
  // устанавливаем начальный инструмент в гитару
  programChange(0xc0, instrument);
  // Еще немного ждем
  delay(100);
  // В качестве теста соединения отправляем несколько нот на MIDI-OUT
  for (int i=0; i <= 3; i++)
    {
    MIDImessage(noteON, 50, velocity);
    delay(500);
    MIDImessage(noteOFF, 50, velocity);
    delay(500);
    }

  // Открываем Bluetooth соединение, пины 0 и 1
  // блютуз перед заливкой скетча - отключать от пинов
  Serial.begin(38400);

}

// Процедура играет MIDI-ноту заданной высоты и громкости
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  mySerial.write(command);// нота
  mySerial.write(MIDInote);// высота
  mySerial.write(MIDIvelocity);// громкость
}


// Процедура выбирает инструмент
void programChange(char cmd, char data1) {
  // cmd = cmd | char(midiChannel);
  mySerial.write(cmd);
  mySerial.write(data1);
}



void loop() 
{
  
  String inString = "";
  char command = ' ';
  int i=0;
  
  while (Serial.available()) // если что-то пришло по блютузу
    {
    char inChar = Serial.read();
    if (inChar == ';') { // начало того, что нам нужно начинается с точки с запятой
      delay(1);
      inChar = Serial.read(); // далее идут две цифры - расстояние ("нота")
      if (isDigit(inChar)) inString += (char)inChar; 
      delay(1);
      inChar = Serial.read();
      if (isDigit(inChar)) inString += (char)inChar; 
      command = Serial.read(); // третий символ - команда:
                               // n - ноту сыграть
                               // m - ноту заглушить
                               // b - "бэнд" (диез)
                               // o - октаой выше
                               // i - смена инструмента
                               // c - аккорд (квинта)
                               // f - clean/overdrive педаль
      i++; // помечаем, что что-то получили
      break;      
      }   
    delay(1);  
    }
  
 // играем ноту
 if (command == 'n' && i > 0 && !chord) {
    
    note = inString.toInt();
    
    if (note <=75 && note >= 63) MIDInote = tune;
    if (note <=62 && note >= 51) MIDInote = tune + 3;
    if (note <=50 && note >= 39) MIDInote = tune + 5;
    if (note <=38 && note >= 27) MIDInote = tune + 7;
    if (note <=28 && note >= 15) MIDInote = tune + 10 ;
    if (note <=14 && note >= 2)  MIDInote = tune + 12;
    
    MIDImessage(noteON, MIDInote, velocity);
  }

 if (command == 'b' && i > 0 && !chord) {
    
    note = inString.toInt();
    
    if (note <=75 && note >= 63) MIDInote = tune;
    if (note <=62 && note >= 51) MIDInote = tune + 3;
    if (note <=50 && note >= 39) MIDInote = tune + 5 + 1;
    if (note <=38 && note >= 27) MIDInote = tune + 7;
    if (note <=28 && note >= 15) MIDInote = tune + 10 ;
    if (note <=14 && note >= 2)  MIDInote = tune + 12;
    
    MIDImessage(noteON, MIDInote, velocity);
  }

// октавой выше
 if (command == 'o' && i > 0 && !chord) {
    
    note = inString.toInt();
    
    if (note <=75 && note >= 63) MIDInote = tune + 12;
    if (note <=62 && note >= 51) MIDInote = tune + 12 + 3;
    if (note <=50 && note >= 39) MIDInote = tune + 12 + 5;
    if (note <=38 && note >= 27) MIDInote = tune + 12 + 7;
    if (note <=28 && note >= 15) MIDInote = tune + 12 + 10 ;
    if (note <=14 && note >= 2)  MIDInote = tune + 12 + 12;
    
    MIDImessage(noteON, MIDInote, velocity);
  }

// аккорд (квинта)
 if (command == 'n' && i > 0 && chord) {
    
    note = inString.toInt();
    
    if (note <=75 && note >= 63) MIDInote = tune_overdrived;
    if (note <=62 && note >= 51) MIDInote = tune_overdrived + 3;
    if (note <=50 && note >= 39) MIDInote = tune_overdrived + 5;
    if (note <=38 && note >= 27) MIDInote = tune_overdrived + 7;
    if (note <=28 && note >= 15) MIDInote = tune_overdrived + 10 ;
    if (note <=14 && note >= 2)  MIDInote = tune_overdrived + 12;
    
    MIDImessage(noteON, MIDInote, velocity);
    MIDImessage(noteON, MIDInote + 7, velocity);
    MIDImessage(noteON, MIDInote + 12, velocity);
  }

// аккорды октавой выше 
 if (command == 'o' && i > 0 && chord) {
    
    note = inString.toInt();
    
    if (note <=75 && note >= 63) MIDInote = tune_overdrived + 12;
    if (note <=62 && note >= 51) MIDInote = tune_overdrived + 12 + 3;
    if (note <=50 && note >= 39) MIDInote = tune_overdrived + 12 + 5;
    if (note <=38 && note >= 27) MIDInote = tune_overdrived + 12 + 7;
    if (note <=28 && note >= 15) MIDInote = tune_overdrived + 12 + 10 ;
    if (note <=14 && note >= 2)  MIDInote = tune_overdrived + 12 + 12;
    
    MIDImessage(noteON, MIDInote, velocity);
    MIDImessage(noteON, MIDInote + 7, velocity);
    MIDImessage(noteON, MIDInote + 12, velocity);
  }


// заглушаем ноту
 if (command == 'm' && i > 0) {
    
    MIDImessage(noteOFF, MIDInote, velocity);
    MIDImessage(noteOFF, MIDInote + 7, velocity);
    MIDImessage(noteOFF, MIDInote + 12, velocity);
  }

 // смена инструмента
 if (command == 'i' && i > 0) {
    
    note = inString.toInt();
    
    if (note == 81 && instrument > 0) instrument--;
    if (note == 82 && instrument < 127) instrument++;
    
    programChange(0xc0, instrument);
  }

 // clean/overdrive педаль и игра аккордами
 if ((command == 'f' && i > 0) || (command == 'c' && i > 0)) {
    
    overdrive = !overdrive;
    chord = !chord;
   
    if (overdrive) programChange(0xc0, 29);
    else programChange(0xc0, 25);    
  }

}


