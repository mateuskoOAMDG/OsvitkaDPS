/*!
 *  @brief Osvitka 1.0
 *  @version 1.0.0 build: 05
 *  @date 2023-04-14
 *  @copyright (c) mateusko O.A.M.D.G
 */

// pokusna verzia na ucenie sa z githubom

 /*  
  *  PAsivny buzzer na D5
  */



#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "EEPROM.h"
#include "RealButton.h"
#include "LCDShowTime.h"
#include "LCDDiakritika.h"


#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define VERSION_BUILD 5
#define VERSION_DATE "2023-04-17"

/*************** PINOUT *******************/

#define PIN_START   2   //tlacidlo na zobrazenie aktivneho time (show(true))
#define PIN_STOP    A3  //tlacidlo na skrytiem a.t. (show(false))
#define PIN_MINUS   A2  //tlacidlo na odpocitavanie nastaveneho casu a.t.(setTime())
#define PIN_PLUS    A1  //tlacidlo na pripocitavanie nastaveneho casu a.t.(setTime())

#define PIN_BUZZER  5   //buzzer pasivny
#define PIN_P1      6   //Predvolba 1
#define PIN_P2      7   //Predvolba 2
#define PIN_P3      8   //Predvolba 3
#define PIN_P4      9   //Predvolba 4
#define PIN_P5_UD   10  //Predvolba 5 (v SL) ; UP/DOWN SELECT (v DL)
#define PIN_LIGHT_UP   11 //osvitka horna
#define PIN_LIGHT_DOWN 12 // osvitka dolna
#define PIN_LED         3 //indikacna dioda
#define PIN_JUMPER_DOWN_LIGHT A0 // verzia so aj so spodnou osvitkou LOW - U+D, HIGH - U, treba pullup 10k

 
/*************** CONSTANTS ***************/
// Mody, v ktorych sa moze nachadzat osvitka

#define MODE_START  1 // prebieha osvit
#define MODE_STOP   2 // v stave necinnosti
#define MODE_PAUSE  3 // osvit pozastaveny (osvitka vypnuta, cas sa pozastavil)
#define MODE_DONE   4 // stav po uspesnom ukonceni osvitu
#define MODE_INIT   5 // inicializacia osvitky

#define MODE_LIGHT_UP     (0x01) //horny osvit aktivny
#define MODE_LIGHT_DOWN   (0x02) //dolny osvit aktivny
#define MODE_LIGHT_BOTH   MODE_LIGHT_UP | MODE_LIGHT_DOWN
#define MODE_LIGHT_OFF    (0x00)

// status indikacnej led-ky 
#define LED_STATUS_OFF 0
#define LED_STATUS_ON  1 
#define LED_STATUS_BLINK 2
#define LED_STATUS_ACTUAL_ON 4 //b bite c. 2 sa uchovava aktualny stav led - ky

// suradnice stavoveho riadka
#define STATUS_LINE_X 0
#define STATUS_LINE_Y 1

// suradnice vypisu statusu
#define STATUS_PRINT_X 9
#define STATUS_PRINT_Y 0
#define STATUS_PRINT_L 6 //dlzka vypisu statusu


// progress bar
#define PROGRESS_BAR_X 7
#define PROGRESS_BAR_Y 1
#define MAX_PRESET 5 //pocet PRESET pamati s nastavenim casu

// up-down osvit icon
#define UPDOWN_X 15
#define UPDOWN_Y 0

// #define PROGRESS_BAR_L 8 pocet stvorcekov progressbaru = 8

//16 bitova verzia millis()  
#define mil16 (uint16_t(millis()))

#define BUZZER_FREQUENCY  2000
#define BUZZER_TONE_DURATION       100
#define BUZZER_PAUSE_DURATION       200
#define BUZZER_COUNT_START     3
#define BUZZER_COUNT_PAUSE     1
#define BUZZER_COUNT_STOP      3
#define BUZZER_COUNT_DONE      5


//********* Instancia lcd displeja  **********//

// instancia dipleja LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// instancie zobrazenia casu
LCDShowTime tTime(lcd);   // nastaveny cas osvitu
LCDShowTime tActual(lcd); // aktualny cas osvitu (ubehnuty/zostavajuci)

// Instancie harwerovych tlacidiel so softwerovym debouncingom
// Vsetky instancie maju nastavene defaultne hodnoty:
// pinMode = INPUT_PULL_UP,
// down = LOW, -ked je tlacitko stlacene digitalRead(pin) je LOW
// debouncetime = 20 (20ms)
RealButton btnStart(PIN_START);     // tlacitko START
RealButton btnStop(PIN_STOP);       // tlacitko PAUSE/STOP
RealButton btnPlus(PIN_PLUS);       // zvysenie casu
RealButton btnMinus(PIN_MINUS);     // znizenie casu
RealButton btnP1(PIN_P1);           // predvolba 1 nacitanie/ulozenie
RealButton btnP2(PIN_P2);           // predvolba 2 nacitanie/ulozenie   
RealButton btnP3(PIN_P3);           // predvolba 3 nacitanie/ulozenie
RealButton btnP4(PIN_P4);           // predvolba 4 nacitanie/ulozenie
RealButton btnP5_UD(PIN_P5_UD);        // predvolba 5 nacitanie/ulozenie | up/down select
//RealButton btnUpDown(PIN_UP_DOWN);  // prepinanie Horneho/Dolneho a obidnoch osvitov


/// @brief mod prace osvitky MODE_START | MODE_STOP | MODE_PAUSE | MODE_DONE
uint8_t mode;                 // mod, v ktorom sa prave nachadza osvitka MODE_...
uint8_t mode_old = 0;         //predosly mod (pouzova sa v loop na zistenie zmeny modu)
uint8_t up_down_mode;         //UP - 0x01, DOWN - 0x02, UP+DOWN - 0x03
bool down_light_version;        // true - verzia so spodnym osvitom - nastavuje sa pri inicializacii osvitky jumperom

// globalne premenne suvisiace s casom
uint16_t set_time;            // nastaveny cas v sekundach
uint16_t actual_time;         // aktualny cas pocas osvitu v 100ms
uint16_t actual_time_sec = 0; // aktualny cas osvitu v sec (neaktualizuje sa automaticky!)
bool remain_time = false;             // true - zobrazuje sa zostavajuci cas | false - ubehnuty cas

            // zobrazenie true - zostavajuceho | false - ubehnuteho casu


// predvolby 1 - MAX_PRESET
uint16_t preset[MAX_PRESET];        // casy timerov, predvolba 1 - MAX_PRESET
uint8_t preset_no = 0;
uint8_t  updown_status[MAX_PRESET]; // nastavenie U/D osvitu, predvolba 1 - MAX_PRESET


uint8_t progress_level;         //aktualne zobrazeny pocet stvorcekov progressbaru

uint8_t light;                  // osvitka svieti - true | nesvieti - false; U resp D je nastavene v up_down_mode

uint8_t led_status = false;        //momentalny stav svietenia led-ky
uint8_t led_timer = 0;

bool longButton = false;        // ci je  dlho stlaceny klaves P1-P5
uint16_t timer100ms = millis(); //timer riadiaci blikanie a cas - hlavny 100 ms timer!

uint8_t buzzer_n = 0; //počítadlo pipnuti pre buzzer
uint16_t buzzer_timer;
bool buzzer_playing = false;
/// @brief Uvitacia obrazovka
void inviteScreen() {
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F(" Osvitka "));
  if (down_light_version)lcd.print("DL "); else lcd.print("SL "); 
  lcd.print(VERSION_MAJOR, DEC);
  lcd.print('.');
  lcd.print(VERSION_MINOR, DEC);
  lcd.setCursor(0, 1);
  lcd.print(F("*2023 O.A.M.D.G*"));
  delay(2000);

}

/// @brief Vrati hodnotu casu a U/D statusu z EEPROM pamate
/// @param n cislo pamate
/// @param time cas v sekundach
/// @return true - uspesne nacitanie z eeprom | false - chyba
bool loadTimeFromEEPROM(uint8_t n, uint16_t &time, uint8_t& status) {
  uint16_t verify16;
  uint8_t  verify8;
  if (n > MAX_PRESET) return false;

  //nacitanie z eeprom
  EEPROM.get(n * 8 + 0, time);
  EEPROM.get(n * 8 + 2, verify16);
  if (verify16 != ~time) return false;

  EEPROM.get(n * 8 + 4, status);
  EEPROM.get(n * 8 + 6, verify8);
   if (verify8 != uint8_t(~status)) return false;
 
  
  return true;
}

/// @brief Inicializacia premennych programu
void initProgram() {
  mode_old = MODE_INIT;
  mode = MODE_STOP;
  set_time = 0;
  actual_time = 0;
  remain_time = false;

  //inicializacia instancii zobrazenia casov:

  // tTime - instancia LCDShowTime zobrazujuca nastaveny cas
  tTime.begin(2, 0, 1); 
  tTime.show(true);
  tTime.setTime(0);
  
  // tActual - instancia LCDShowTime zobrazujuca ubehnuty cas
  tActual.begin(0, 1, 1); 
  tActual.show(false);
  tActual.off();
  tActual.setTime(0);
  tActual.setText("+"); 

  LCDShowTime::blinkTimeTime = 2;
  LCDShowTime::blinkColonTime  = 5;
 
  light = false;  //osvitka vypnuta
  up_down_mode = MODE_LIGHT_BOTH;

  // nacitanie casov a statusu z EEPROM
  for (uint8_t i = 0; i < MAX_PRESET; i++)  
    if (!loadTimeFromEEPROM(i, preset[i], updown_status[i])) { preset[i] = 0; updown_status[i] = MODE_LIGHT_UP; } 
}

// Vypis informacie o verzii firmware
void versionInfo() {
  lcd.clear();
  lcd.home();
  //lcd.print(F("FW: "));
  lcd.print(VERSION_MAJOR, DEC);
  lcd.print('.');
  lcd.print(VERSION_MINOR, DEC);
  lcd.print('.');
  lcd.print(VERSION_PATCH, DEC);
  lcd.print(F(" BUILD: "));
  lcd.print(VERSION_BUILD, DEC);

  lcd.setCursor(0,1);
  lcd.print(F("DATE: ")); lcd.print(F(VERSION_DATE));
  while(!btnStop.onClick())btnStop.update();

  lcd.clear();
  lcd.home();
  lcd.print(F("FW: (c) mateusko"));
  lcd.setCursor(0,1);
  lcd.print(F("    O.A.M.D.G   "));
  while(!btnStop.onClick())btnStop.update();
  repaint();
}

// ********** Status Line **** nechce sa mi robit triedu; bude to takto******** //

uint16_t status_line_time = 0; //po akej dobe sa ma vymazat text statusline
                              //v 100ms ; 0 = nemazat
uint16_t  status_line_timer; //timer pre status line

/// @brief Vymazanie Status Line
void statusLine_clear() {
  lcd.setCursor(0, 1);
  for (uint8_t i = 0; i < 16; i++) lcd.print(' ');
  status_line_time = 0;
}

/// @brief Vypisanie textu do Status Line
/// @param text Text (Musi byt v PROGMEM!)
/// @param x    Pociatocna x- suradnica vypisu
/// @param clear true - pred vypisom vymaz Status Line
/// @param time cas v ms zobrazenia status line max 30 sec.
void statusLine_text(const __FlashStringHelper* text, uint8_t x = 0, bool clear = false, uint8_t time = 0) {
  if (clear) statusLine_clear();
  lcd.setCursor(x + STATUS_LINE_X, STATUS_LINE_Y);
  LCDprintText(text);
  
  status_line_timer = mil16;
  status_line_time = time;
}

/// @brief Updatnutie Status LIne, kvoli automatickemu zmazaniu po nastavenom case
///        volat v cca 100 ms intervaloch
void statusLine_update() {
  if (status_line_time) {
    if (mil16 - status_line_timer > status_line_time) {
      statusLine_clear();
      status_line_time = 0;
    }
  }
}

/*************************** MODE print ***************************/

/// @brief Vypisanie Mode
/// @param nmode 
void modePrint(uint8_t nmode) {
  //vymazanie stareho statusu
  lcd.setCursor(STATUS_PRINT_X, STATUS_PRINT_Y);
  for (int i = 0; i < STATUS_PRINT_L; i++) lcd.print(' ');
  // vypis statusu
  lcd.setCursor(STATUS_PRINT_X, STATUS_PRINT_Y);
  switch (nmode) {
    case MODE_START:
      LCDprintText(F("Osvit"));
      break;
    case MODE_PAUSE:
      LCDprintText(F("Pauza"));
      break;
    case MODE_STOP:
      LCDprintText(F("Stop"));
      break;
    case MODE_DONE:
      LCDprintText(F("Stop"));
      break;
  }
}

/********************* UP-DOWN print *********************/

 
/// @brief Vypis ikony UP-DOWN podla up_down_mode 
void upDownPrint() {

  bool blank = false;
  if (down_light_version) {
    switch (up_down_mode) {
      case MODE_LIGHT_UP:
        LCDsetCharacter(7, char_p_Up);
        break;  
      case MODE_LIGHT_DOWN:
        LCDsetCharacter(7, char_p_Down);
        break;  
      case MODE_LIGHT_BOTH:
        LCDsetCharacter(7, char_p_UpDown);
        break; 
      default:
        blank = true;
        break; 
    }

  } else  blank = true; //verzia HW jen s dolnym osvitom 
  lcd.setCursor(UPDOWN_X, UPDOWN_Y);

  if (blank) 
    lcd.print(' ');
  else 
    lcd.write(7);
}

/****************** PRESET NUMBER print ******************/

/// @brief Zobrazi cislo predvolby v rohu
/// @param 1 .. MAX_PRESET
void printPreset() {
  lcd.setCursor(0, 0); 
  lcd.print(F("P"));
  if (preset_no == 0 || preset_no > MAX_PRESET) {
    lcd.print(F("-"));
  } else {
    lcd.print(preset_no, DEC);
  }
}

void printProgressBar2(uint16_t actual, uint16_t max_set) {
  // LEFT   ... MIDS ...   RIGHT 
  uint8_t columns = map(actual, 0, max_set, 0, 38); //38 maximalny pocet stlpcov progressbaru
  uint8_t i, j;
  lcd.setCursor(PROGRESS_BAR_X, PROGRESS_BAR_Y);
  //lcd.print("         ");
   lcd.setCursor(PROGRESS_BAR_X, PROGRESS_BAR_Y);
  //vykreslenie plnych stvorcekov 
  j = (columns + 1) / 5; //pocet plnych stvorcekov
  for(i = 0; i < j; i++) lcd.write(5); 
  
  if (columns < (38 - 4)) {
    //vykreslenie neuplneho stvorceka (vpravo od plnych)
    i = (columns + 1) % 5;
    lcd.write(i); //neuplny stvorcek
  
  }

  //vykreslenie zbytku prazdnych stvorcekov po predposledny (6)
  for (i = j + 1; i < 7; i++) lcd.write(0);
 
  //vykreslenie posledneho neuplneho stvorceka
  switch (38 - columns) {
    case 0: 
      LCDsetCharacter(6, char_p_bar);
      break;
    case 1:
      LCDsetCharacter(6, char_p_right3);
      break;
    case 2:
      LCDsetCharacter(6, char_p_right2);
      break;
    case 3:
      LCDsetCharacter(6, char_p_right1);
      break;
    default:
      LCDsetCharacter(6, char_p_right0);
      break;
  }
  lcd.setCursor(PROGRESS_BAR_X + 7,1);
  lcd.write(6);
                                                               

}
  



/*********************  Diakritika ***********************/

/// @brief Nastavuje znaky s diakritikou pre displej
void setDiakrit() {
  LCDsetCharacter(0, char_yi); // &0 - ý
  LCDsetCharacter(1, char_cv); // &1 - č
  LCDsetCharacter(2, char_sv); // &2 - š
  LCDsetCharacter(3, char_zv); // &3 - ž
  LCDsetCharacter(4, char_lv); // &4 - ľ
  LCDsetCharacter(5, char_ac); // &5 - ä
  LCDsetCharacter(6, char_ei); // &6 - é
  //LCDsetCharacter(7, ...); // rezervovane
}


void setDiakrit2() {
  LCDsetCharacter(0, char_p_empty);
  LCDsetCharacter(1, char_p_left1);
  LCDsetCharacter(2, char_p_left2);
  LCDsetCharacter(3, char_p_left3);
  LCDsetCharacter(4, char_p_left4);
  LCDsetCharacter(5, char_p_bar); // &6 - bodka pre progress bar
  LCDsetCharacter(6, char_p_bar);  // &6 - plný stvorcek pre progressbar
} 

void repaint() {
  lcd.clear();
  lcd.home();
  printPreset();
  modePrint(mode);
  statusLine_clear();
}

void buzzerStart(uint8_t n) {
  
  if ((n > 0) && (buzzer_n == 0)) {
    buzzer_n = n;
    buzzer_timer = mil16;
    buzzer_playing = true;
    tone(PIN_BUZZER, BUZZER_FREQUENCY, BUZZER_TONE_DURATION);
  } else {
    noTone(PIN_BUZZER);
    buzzer_playing = false;
    buzzer_n = 0;
  }
  
}

/// @brief Zapne pipanie; nutne volat kazdych cca 100ms buzzerUpdate()!

inline void beep(uint8_t n = 3) {
  buzzerStart(n);
}

void buzzerUpdate() {
  if (buzzer_n > 0) {
    if (buzzer_playing) {
      if ( (mil16 - buzzer_timer) > BUZZER_TONE_DURATION) {
        buzzer_timer = mil16;
        buzzer_playing = false;
        buzzer_n --;
      }
    } else {
      if ( (mil16 - buzzer_timer) > BUZZER_PAUSE_DURATION) {
        buzzer_timer = mil16;
        buzzer_playing = true;
      }
    }
  } 
  else return; //buzzer off
  
  if (buzzer_playing) 
    tone(PIN_BUZZER, BUZZER_FREQUENCY, BUZZER_TONE_DURATION);
  else
    noTone(PIN_BUZZER);
}



void setup() {
  
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LIGHT_UP, OUTPUT);
  pinMode(PIN_LIGHT_DOWN, OUTPUT);
  pinMode(PIN_JUMPER_DOWN_LIGHT, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LIGHT_UP, LOW);
  digitalWrite(PIN_LIGHT_DOWN, LOW);


  lcd.begin();
  lcd.clear();
  
  //zistenie verzie hardweru jumperom - len vrchny osvit - false; aj spodny osvit - true
  //verzia pre 328P - NANO, UNO etc...
  // ak je pripojeny jumper tak je LOW - oba osvity
  down_light_version = analogRead(PIN_JUMPER_DOWN_LIGHT) < (1024*2/5);
  
  inviteScreen();
  
  setDiakrit();
  
 
  
  //inicializacia timerov
  initProgram();

  lcd.clear();
  tTime.print();
  statusLine_text(F(">> Pripraven&0 <<"), 0, true); 
  beep(1);
  upDownPrint();
  preset_no = 0;
  printPreset();
}

void upDownChange() {
  up_down_mode ++;
  if (up_down_mode > 3) up_down_mode = 1;
  upDownPrint();
}



/// @brief Obsluha dlheho stlacenia klavesu + a -
void handleLongPress(RealButton &btn, bool plus) {
  preset_no = 0;
  printPreset();

  
  //pocas nastavovania blikaju oba casy 
  tTime.blinkColon(false);
  tTime.blinkTime(false);
  tActual.blinkColon(false);
  tActual.blinkTime(false);

  uint16_t add = 1; // po kolko sa pridava cas
  uint16_t pressTime = 10; //casova medzera na opakovanu udalost zvysenia/znizenia casu pri dlhsom podrzani tlacitka
  uint16_t timer = mil16; //timer prepínania add
  uint16_t timer2 = mil16; //timer rychlosti reagovania na podrzane tlacitko
  
  bool first = true;
  

  while(btn.pressed() ) {

    //nastavenie kroku pridavania/odoberania casu  
    
    if ((mil16 - timer > 5000u)  && (add == 10)) {
      if (set_time % 600 == 0) {
        add = 60;
      }
    }
    
    if ((mil16 - timer > 1000u) && (add == 1)) {
      if (set_time % 30 == 0) {
        add = 10;
      }
    }

    // generovanie opakovanych pulzov pri dlhom stlaceni tlacitka
    if (mil16 - timer2 > pressTime) {
      if (first) {
        pressTime = 500; // po prvom add sa caka 500ms
        first = false;
      } else {
        pressTime = 200; //ostatne add idu po 150ms
      }

      timer2 = mil16;

      //pridanie/ ubratie hodnoty timera a osetrenie hranic
      if (plus) {
        if (set_time + add > 60 * 60) set_time = 60 * 60;
        else set_time += add;
      }  else {
        if (add > set_time) set_time = 0; 
        else set_time -= add;
      }

      tTime.setTime(set_time); //nastavenie zobrazovaneho casu

      // Zobrazenie zostavajuceho casu
      
       actual_time_sec = actual_time / 10;
       if (actual_time_sec > set_time) {
          actual_time_sec = set_time;
          actual_time = set_time * 10;
       } 

       

       if (remain_time) { //zostavajuceho 
          tActual.setText("-");
          tActual.printText(true);
          tActual.setTime(set_time - actual_time_sec);
        } else {  //ubehnuteho
          tActual.setText("+");  
          tActual.printText(true);
          tActual.setTime(actual_time_sec);
        }
    } 

    //kazdych 100ms sa prekresli time na displeji
    if(mil16 - timer100ms > 100) {      
      timer100ms = mil16;
      timer100ms += 100;
      
      //hodinovy impulz
      LCDShowTime::clock();

      //prekreslenie vsetkych timerov

      tTime.update();
      tActual.update();
      
    }
    btn.update();
  }
  tTime.blinkTime(true) ;
  tActual.blinkTime(true);
  timer100ms = mil16; 
}

/* Vykonne funkcie, ako odozvy na udalosti z tlacitok */

/// @brief Zastavenie timera
void stop() {
  tTime.blinkAll(false);
  tActual.blinkAll(false);
  tActual.show(false);
  tActual.off();

  actual_time = 0;
  mode = MODE_STOP;
  statusLine_clear();
  statusLine_text(F("Osvit zru&2en&0!"), 0, true);
  modePrint(mode);
   //zhasnutie ledky
  led_status = LED_STATUS_OFF;

  //vypnutie osvitu 
  light = false;
  beep(BUZZER_COUNT_STOP);
}

/// @brief Ukoncenie osvitu po vyprsani casu
void done() {
  tTime.blinkAll(false);
  tActual.blinkAll(false);
  tActual.show(false);
  tActual.off();
  actual_time = 0;
  beep(BUZZER_COUNT_DONE);
  statusLine_text(F("Osvit ukon&1en&0."),0,true);

  mode = MODE_DONE;
  modePrint(mode);

  //zhasnutie ledky
  led_status = LED_STATUS_OFF;

  //vypnutie osvitu 
  light = false;

}


/// @brief START - Spustenie timera (v mode STOP)
void start() {
  if (set_time == 0) {
    statusLine_text(F("  Nastav &1as!"), 0, true, 20);
    return;
  }

  actual_time = 0; //ubehnuty cas == 0
  mode = MODE_START;
  statusLine_clear();
  modePrint(mode);
  
  //rozsvietenie ledky
  led_status = LED_STATUS_BLINK | LED_STATUS_ACTUAL_ON;

  //zapnutie osvitu podla nastavenia
  light = true;

  progress_level = 0xFF; //aby sa prekreslilo
  printProgressBar2(0, set_time);


  tActual.on();
  tActual.show(true);
  tActual.setTime(0);  
  tActual.blinkTime(false);
  tActual.blinkColon(true);

  tTime.blinkAll(false);
  beep(BUZZER_COUNT_START);
}

/// @brief Docasne zastavenie osvitu
void pause() {
  mode = MODE_PAUSE;
  modePrint(MODE_PAUSE);
  tActual.blinkTime(true);
  tActual.blinkColon(false);
  tTime.blinkAll(false);
  
  //zhasnutie ledky
  led_status = LED_STATUS_ON;

  //vypnutie osvitu 
  light = false;
  beep(BUZZER_COUNT_PAUSE);
}

/// @brief Pokracovanie v osvite po pauze
void cont() {
  mode = MODE_START;
  modePrint(MODE_START); 
  tActual.blinkTime(false);
  tActual.blinkColon(true);
  tTime.blinkAll(false);
  
  //rozsvietenie ledky
  led_status = LED_STATUS_BLINK;

  //zapnutie osvitu podla nastavenia
  light = true; 
  beep(BUZZER_COUNT_PAUSE);
}





// vyvolanie casu z pamati EEPROM
void loadTime(uint8_t n) {
  uint16_t time; uint8_t status;
  bool ok;
  if (n >= MAX_PRESET) return;
  ok = loadTimeFromEEPROM(n, time, status);
  if (!ok) {
    statusLine_text(F("Chyba EEPROM!"), 0, true);
    set_time = 0;
    actual_time = 0;
    status = 0x01; //horny osvit
    return;
  }
  
  //ok
  set_time = time;
  up_down_mode = status;

  preset[n] = time;
  updown_status[n] = status;

  actual_time = 0;
  

  tTime.blinkAll(false);
  tTime.setTime(set_time);

  tActual.setTime(0);
  tActual.blinkAll(false);
  tActual.show(false);
  

  tActual.off();

  preset_no = n + 1;
  printPreset();
  statusLine_text(F("Predvo&4ba #"), 0, true); lcd.print(n + 1); lcd.print('#');
  upDownPrint();
  modePrint(0);
}

void saveTime(uint8_t n) {
  
  if (n > MAX_PRESET) return;
  if ((preset[n] == set_time) && (up_down_mode == updown_status[n])) {
     statusLine_text(F("&1as je ulo&3en&0"));
     return; //rovnaky cas je v pamati, neulozi sa
  }
  preset[n] = set_time;
  actual_time = 0;
  tTime.blinkAll(false);
  tTime.setTime(set_time);

  tActual.setTime(0);
  tActual.blinkAll(false);
  tActual.show(false);

  EEPROM.put(n * 8 + 0, set_time);
  EEPROM.put(n * 8 + 2, ~set_time); //kontrola

  EEPROM.put(n * 8 + 4, up_down_mode);
  EEPROM.put(n * 8 + 6, uint8_t(~up_down_mode));
  preset_no = n + 1;
  printPreset();
 
  tActual.off();
  statusLine_text(F("Ulo&3en&6 do #"), 0, true); lcd.print(n + 1); lcd.print('#');

  modePrint(0);
}



void loop() {
  
  //obsluha tlacitok podla mode
  switch (mode) {
    case MODE_STOP:

      if(btnStop.pressed()) {
        if(btnP1.onDouble())
           versionInfo();
      }

      //zvysenie nastaveneho casu
      else if(btnPlus.pressed()) {
        handleLongPress(btnPlus, true);
      }

      //znizenie nastaveneho casu 
      else if(btnMinus.pressed()) {
        handleLongPress(btnMinus, false);
      }
      
      //spustenie osvitu
      else if(btnStart.onClick()) {
        start();
      }
      
      else if (btnP1.onClick()) loadTime(0);
      else if (btnP2.onClick()) loadTime(1);
      else if (btnP3.onClick()) loadTime(2);
      else if (btnP4.onClick()) loadTime(3);
      else if (btnP5_UD.onClick())  {
        if (down_light_version) upDownChange(); else loadTime(4);
      }
      
      else if (btnP1.onLong()) saveTime(0); 
      else if (btnP2.onLong()) saveTime(1); 
      else if (btnP3.onLong()) saveTime(2);
      else if (btnP4.onLong()) saveTime(3);   
      else if (!down_light_version && btnP5_UD.onLong()) saveTime(4);
      
    
      break;
    
    case MODE_START:
      
      //pozastavenie osvitu PAUSE 
      if(btnStop.onClick()) {
        pause();
      } 

      //nastavenie zobrazenie zostavajuceho casu
      else if (btnMinus.onClick()) {
        remain_time = true;
        tActual.setText("-");
        tActual.printText(true);
      } 

      //nastavenie zobrazenia ubehnuteho casu
      else if (btnPlus.onClick()) {
        remain_time = false;
        tActual.setText("+");
        tActual.printText(true);
      } 

      break;

    case MODE_PAUSE:
      //pokracovanie v osvite
      if (btnStart.onClick()) cont();

      //zastavenie osvitu pri dlhom stlaceni stop
      else if(btnStop.onLong()) stop();
      

      else if(btnPlus.pressed())
        handleLongPress(btnPlus, true);

      else if(btnMinus.pressed())
        handleLongPress(btnMinus, false);

      
      break;

    case MODE_DONE:
      mode = MODE_STOP;
      break;
  }

  //END OF switch
   
  // Nasledujuca cast sa spusta vzdy pri zmene modu

  //ak sa prepne mode, resetnu sa tlacitka a nastavi sa spravna diakritika
  if (mode != mode_old) {
    
    switch(mode) {
      case MODE_START:
        //if (mode_old == MODE_STOP) beep(BUZZER_COUNT_START);
        //else if (mode_old == MODE_PAUSE) beep(BUZZER_COUNT_PAUSE);
        setDiakrit2();
        break;
      case MODE_PAUSE:
        //beep(BUZZER_COUNT_PAUSE);
        break;
      
      case MODE_STOP: 
        //if (mode_old == MODE_PAUSE)  
          //beep(BUZZER_COUNT_STOP);
       //else if (mode_old == MODE_START)
          //beep(BUZZER_COUNT_STOP); 
          setDiakrit();
        break;
      case MODE_DONE:
        //beep(BUZZER_COUNT_DONE);
        setDiakrit();
        break;
      default:
        setDiakrit();
    }

    btnStart.reset();
    btnStop.reset();
    btnPlus.reset();
    btnMinus.reset();
    btnP1.reset(); 
    btnP2.reset();
    btnP3.reset();
    btnP4.reset();
    btnP5_UD.reset(); 
    mode_old = mode;
  }
  buzzerUpdate();

  //Nasledujuca cast programu sa spusta kazduch 100 ms, zabezpecuje
  //korektne meranie casu v 100ms intervaloch

  //clock signal 100ms for timer repaint
  while ((mil16 - timer100ms) > 100u) {
    timer100ms += 100;
    led_timer ++;
    LCDShowTime::clock(); 

    

    //aktualizacia zapnutia/vypnutia osvitiek
    if (down_light_version) {
      digitalWrite(PIN_LIGHT_UP, ((up_down_mode & MODE_LIGHT_UP) && light) ? HIGH : LOW);
      digitalWrite(PIN_LIGHT_DOWN, ((up_down_mode & MODE_LIGHT_DOWN) && light) ? HIGH : LOW);
    } else {
      digitalWrite(PIN_LIGHT_UP, light ? HIGH : LOW);
    }

    // blikanie/ svietenie led - ky
    
    if (led_status & LED_STATUS_BLINK) {
      digitalWrite(PIN_LED, (led_status & LED_STATUS_ACTUAL_ON) ? HIGH : LOW ); 
      led_status ^= LED_STATUS_ACTUAL_ON;
    } else {
       digitalWrite(PIN_LED, (led_status & LED_STATUS_ON) ? HIGH : LOW ); 
    }   
   

    if(mode == MODE_START) {
      actual_time ++; //zvys cas o 1s
      actual_time_sec = actual_time / 10;
      
      //repaint progressbar

      printProgressBar2(actual_time, set_time * 10);
      // ukoncienie
      if (set_time <= actual_time_sec) { //done
        done();
      }
      
      //zobrazenie aktualneho casu
  
        if (remain_time) { //zostavajuceho 
          tActual.setText("-");
          tActual.printText(true);
          tActual.setTime(set_time - actual_time_sec);
        } else {  //ubehnuteho
          tActual.setText("+");  
          tActual.printText(true);
          tActual.setTime(actual_time_sec);
        }
      }

    tTime.update();
    tActual.update();  
  }
  
  //aktualizacia stavu tlacidiel
  btnStart.update();
  btnStop.update();
  btnPlus.update();
  btnMinus.update();
  btnP1.update(); 
  btnP2.update();
  btnP3.update();
  btnP4.update();
  btnP5_UD.update();
 
}

