/*
 * //Súbor podpory diakritiky pre LCD displeje
 *  súbor naimportovať do  hlavného sketch-u
 * */

#ifndef LCDDIAKRITIKA_H
#define LCDDIAKRITIKA_H

#include <avr/pgmspace.h> 
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd; //instancia LCD displeja definovana v 


/* Definicia znakov s diakritikou
   koncovky: i -dlzen
             v -makcen
             c -dvojbodka
*/

const byte char_yi[8] PROGMEM = { 0x02, 0x04, 0x11, 0x11, 0x0A, 0x04, 0x08, 0x00 }; // "y" s dlznom
const byte char_cv[8] PROGMEM = { 0x0A, 0x04, 0x0E, 0x11, 0x10, 0x11, 0x0E, 0x00 }; // "c" s makcenom
const byte char_sv[8] PROGMEM = { 0x0A, 0x04, 0x0E, 0x10, 0x0E, 0x01, 0x0E, 0x00 }; // "s" s makcenom
const byte char_Sv[8] PROGMEM = { 0x0A, 0x04, 0x0F, 0x10, 0x0E, 0x01, 0x1E, 0x00 }; // "S" s makcenom
const byte char_lv[8] PROGMEM = { 0x0B, 0x09, 0x0A, 0x08, 0x08, 0x08, 0x06, 0x00 }; // "l" s makcenom
const byte char_ac[8] PROGMEM = { 0x0A, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00 }; // "a" prehlasovane
const byte char_ei[8] PROGMEM = { 0x02, 0x04, 0x0E, 0x11, 0x17, 0x10, 0x0E, 0x00 }; // "e" s dlznom
const byte char_zv[8] PROGMEM = { 0x0A, 0x04, 0x1F, 0x02, 0x04, 0x08, 0x1F, 0x00 }; // "z" s makcenom

/*normal progress bar chars*/
const byte char_empty_bar[8] PROGMEM = { 0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00 };     // prazdny obdlznik   
const byte char_bar_1column[8] PROGMEM = { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00 };   // jedna zvisla ciarka zlava
const byte char_bar_2column[8] PROGMEM = { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 };   // dve zvisle ciarky zlava
const byte char_bar_3column[8] PROGMEM = { 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x00 };   // tri zvisle ciarky zlava
const byte char_bar_4column[8] PROGMEM = { 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x00 };   // styri zvisle ciarky zlava
const byte char_entire_bar[8] PROGMEM = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00 };           // plny obdlznik (pat zvislych ciarok)
const byte char_dot[8] PROGMEM = { 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00 };           // bodka


/*precision progress bar chars*/

const byte char_p_empty[8] PROGMEM = { 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00 };  //&0   // prazdny stred progressbaru   0

const byte char_p_left1[8] PROGMEM = { 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00 };  //&1   // jedna ciarka vlavo 1  
const byte char_p_left2[8] PROGMEM = { 0x1F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00 };  //&2   // dve ciarky vlavo   2 
const byte char_p_left3[8] PROGMEM = { 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00 };  //&3   // tri ciarky vlavo   3
const byte char_p_left4[8] PROGMEM = { 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00 };  //&4   // styri ciarky vlavo 4  
const byte char_p_bar[8]   PROGMEM = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00 };  //&5   // pat ciarok - plny bar 5

const byte char_p_right0[8] PROGMEM = { 0x1F, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1F, 0x00 };  //&6   // pravy koniec progressbaru 0
const byte char_p_right1[8] PROGMEM = { 0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00 };  //&6     // pravy koniec progressbaru 1
const byte char_p_right2[8] PROGMEM = { 0x1F, 0x19, 0x19, 0x19, 0x19, 0x19, 0x1F, 0x00 };  //&6     // pravy koniec progressbaru 2
const byte char_p_right3[8] PROGMEM = { 0x1F, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1F, 0x00 };  //&6     // pravy koniec progressbaru 3

//UP DOWN mode chars
const byte char_p_UpDown[8] PROGMEM = { 0x04, 0x0E, 0x1F, 0x00, 0x1F, 0x0E, 0x04, 0x00 }; //&7 //UP+DOWN
const byte char_p_Up[8] PROGMEM = { 0x04, 0x0E, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 };     //&7 //UP
const byte char_p_Down[8] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x0E, 0x04, 0x00 };   //&7 //DOWN


/// @brief Ulozenie bitovej mapy znaku do pamate displeja
/// @param number cislo znaku 0 - 7
/// @param array  bitova mapa znaku - 8bytov - jednotlive riadky zhora nadol 
void LCDsetCharacter(uint8_t number, const byte *array) {
 byte temp[8];
 if (number >= 8) return;
 for (uint8_t i = 0; i < 8; i++)
  temp[i] = pgm_read_byte_near(array + i); 
  lcd.createChar(number, temp);
};

/// @brief Vypis textu na LCD
/// @param txt Text so zastupnymi znakmi: &0 - znamena uzivatelsky znak 0
///        Ktory bol vytvoreny volanim LCDsetCharacter(0, array)
///        Zastupne znaky su aj &1, &2, ... , &7 
void LCDprintText(const __FlashStringHelper* txt) {
  char c = 0;
  uint8_t i = 0;
  byte* t = (byte*)(txt);
  while ((c = pgm_read_byte_near(t + i)) > 0) {
    if (c == '&') {
      i ++;
      c = pgm_read_byte_near(t + i);
       if (c == '&') lcd.print('&');
       if (c >= '0' && c <= '7') lcd.write((byte)(c - '0'));
       if (c == 0) return; //koniec retazca
      } else {
        lcd.write(c);
      }
      i ++;
  }
}

#endif