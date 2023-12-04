/// Vyvoj samostatnej triedy LCDShowTime 
/// 


#include <Arduino.h>
#include  "LCDShowTime.h"

/// Inicializacia instancie
void LCDShowTime::begin(uint8_t x0, uint8_t y0,  uint8_t textLength) {
  instanceOn = true;
  
  x = x0;
  y = y0;
  
  text_length = textLength;
  text = NULL;

  
  status = 0xF3; // Zobrazenie dvojbodky aj casu; neblikaj
  time = 0; //zobrazovany cas
}

void LCDShowTime::setText(char* txt) {
  text = txt;
}

void LCDShowTime::printText(bool show) {
  char c;
  if (!instanceOn) return;
  c = show ? ' ' : 0;
  if (text == NULL) c = 0;
  _lc->setCursor(x, y);
  for (uint8_t i = 0; i < text_length; i++) {
    if (c)  c = *(text + i);
    if (c) _lc->print(c); else _lc->print(' ');
  }
}

/// @brief Vykresli alebo vymaze casovy udaj a dvojbodku na displeji
///        Podla nastavenia status VISIBLE
///        Instancia musi byt aktivna: instanceOn = true;
void LCDShowTime::print() {
  if (!instanceOn) return; 
  uint8_t value;
  //korekcia suradnic, vzhladom na text   
  uint8_t x = this -> x + text_length;
    
    //obsluha zobrazenia casu
    if(status & SHOWTIME_TIME_VISIBLE) {
        _lc->setCursor(x, y);
        value = time / 60;
        if (value < 10) _lc->print('0');
        _lc->print(value, DEC);
        
        _lc->setCursor(x + 3, y);
        value = time % 60;
         if (value < 10) _lc->print('0');
        _lc->print(value, DEC);
      } else { //hide min sec
        _lc->setCursor(x, y); 
        _lc->print(F("  "));
        _lc->setCursor(x + 3 , y); 
        _lc->print(F("  "));
      }

       
      // obsluha zobrazenia dvojbodky
      _lc->setCursor(x + 2, y);
      if (status & SHOWTIME_COLON_VISIBLE) { //show colon
        _lc->print(':');
      } else { //hide colon
        _lc->print(' ');
      }
}


void LCDShowTime::blinkColon(bool blink){
  if(blink) status |= SHOWTIME_COLON_BLINK; //dvojbodka sa zobrazi podla stavu blikania
  else status &= ~ SHOWTIME_COLON_BLINK;    //dvojbodka sa zobrazi podla SHOW
}

void LCDShowTime::blinkTime(bool blink) {
  if(blink) 
    status |= SHOWTIME_TIME_BLINK;

  else status &= ~ SHOWTIME_TIME_BLINK;
} 

void LCDShowTime::blinkAll(bool blink) {
  blinkTime(blink);
  blinkColon(blink);
}



void LCDShowTime::showColon(bool show){
  if(show) status |= SHOWTIME_COLON_SHOW;
  else status &= ~ SHOWTIME_COLON_SHOW;
}

void LCDShowTime::show(bool showAll){
  showColon(showAll);
  showTime(showAll);
}

void LCDShowTime::showTime(bool show) {
  if(show) status |= SHOWTIME_TIME_SHOW;
  else status &= ~ SHOWTIME_TIME_SHOW;
}  

void LCDShowTime::setTime(uint16_t seconds) {
  time = seconds;
  status |= SHOWTIME_TIME_UPDATE;
}
 
void LCDShowTime::update() {
  // updatnutie zobrazenia Colon
  if (!instanceOn) return; //instancia je neaktivna
  
  bool print_nowC = false;
  if (status & SHOWTIME_COLON_SHOW) {
    if (status & SHOWTIME_COLON_BLINK) {
      if (blink_status & SHOWTIME_COLON_VISIBLE) {
          print_nowC = true;
      }
    } else {
          print_nowC = true;
    }
  }

  if (print_nowC != bool(status & SHOWTIME_COLON_VISIBLE)) {
    status ^= SHOWTIME_COLON_VISIBLE;
    status |= SHOWTIME_COLON_UPDATE;
  }

  
  bool print_nowT = false;
  if (status & SHOWTIME_TIME_SHOW) {
    if (status & SHOWTIME_TIME_BLINK) {
      if (blink_status & SHOWTIME_TIME_VISIBLE) {
          print_nowT = true;
      }
    } else {
          print_nowT = true;
    }
  }

  if (print_nowT != bool(status & SHOWTIME_TIME_VISIBLE)) {
    status ^= SHOWTIME_TIME_VISIBLE;
    status |= SHOWTIME_TIME_UPDATE;
  }

  //ak sa nieco zmenilo, vypis
  print();
}


// staticka metoda!!!

/// @brief Hodinovy impulz pre zmenu statusu zobrazenia casu a dvojbodky
///        Po nom treba instancie update() -ovat
void LCDShowTime::clock() {

  //statusy viditelnosti pri blikani sa nastavia podla timera a casov blikania
  timer ++;
  if (timer == 0) timer = 1;
  if (timer % blinkColonTime == 0) {
    blink_status ^= SHOWTIME_COLON_VISIBLE;
    //blink_status |= SHOWTIME_COLON_UPDATE;
  }

  if (timer % blinkTimeTime == 0) {
    blink_status ^= SHOWTIME_TIME_VISIBLE;
    //blink_status |= SHOWTIME_TIME_UPDATE;
  }
};




uint8_t LCDShowTime::blinkTimeTime = 5;    //cas synchronizovaneho blikania Time
uint8_t LCDShowTime::blinkColonTime = 5; //cas synchronizovaneho blikania Dvojbodky
uint8_t LCDShowTime::blink_status = 0x00;   //status blikania mask 
uint8_t LCDShowTime::timer = 0;  //timer na blikanie (pocet 100ms impulzov clock)

