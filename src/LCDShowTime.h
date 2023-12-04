/*!  @brief Trieda LCDShowTime
 *   @file  LCDShowTime.h
 *   @version 0.0.1 beta
 *   @copyright mateusko O.A.M.D.G
 *   @date 2023-04-10
 *   
 */

#ifndef LCDSHOWTIME_H
#define LCDSHOWTIME_H

#include <LiquidCrystal_I2C.h>



/// Class LCDShowTime
/// Zobrazuje čas na displeji s možnosťou blikania



//status constants
#define SHOWTIME_COLON_SHOW    0x01 //zapnute zobrazenie casu
#define SHOWTIME_TIME_SHOW     0x02 //zapnute zobrazenie dvojbodky
#define SHOWTIME_TIME_BLINK    0x04 //zapnute blikanie casu
#define SHOWTIME_COLON_BLINK   0x08 //zapnute blikanie dvojbodky
#define SHOWTIME_TIME_VISIBLE  0x10 //cas je momentalne zobrazeny
#define SHOWTIME_COLON_VISIBLE 0x20 //dvojbodka je momentalne zobrazena
#define SHOWTIME_COLON_UPDATE  0x40 //priznak, ze sa ma prekreslit
#define SHOWTIME_TIME_UPDATE   0x80 //priznak, ze sa ma prekreslit


//************* Class LCDShowTime ****************/
// Trieda zobrazenia casoveho udaju, s moznostou blikania casu a/alebo dvojbodky

class LCDShowTime {
  
  public:
  
  static uint8_t blinkTimeTime;    //cas synchronizovaneho blikania Time
  static uint8_t blinkColonTime; //cas synchronizovaneho blikania Dvojbodky
  static uint8_t timer;  //timer na blikanie (pocet 100ms impulzov clock())
  
  protected:

  static uint8_t blink_status;   //status blikania mask 
  
  public:

  /// @brief Hodinovy impulz pre obsluhu blikania, volat kazdych (cca) 100 ms
  static void clock(); //hodinove impulzy na blikania 100ms !
  
  /// @brief Konstruktor
  /// @param lcd INstancia LCD displeja, kam sa bude zobrazovat
  LCDShowTime(LiquidCrystal_I2C& lcd) {
    _lc = &lcd;
  };
  
   
  

  ///// @brief Inicializacia instancie 
  /// @param x0 stlpec (0-15)
  /// @param y0 riadok (0-1),
  /// @param textLength (0-10) - max. dlzka textu pre casovym udajom
  void begin(uint8_t x0 = 0, uint8_t y0 = 0, uint8_t textLength = 0);


  // Metody on() a off() zapinaju a vypinaju reagovanie instancie 
  // na update() 

  /// @brief Zapnutie (Aktivacia - bez parametra alebo true, alebo deaktivacia false)
  /// @param value Zapnute - true | vypnute - false
  inline void on(bool value = true) {instanceOn = value;};
  inline void off() { instanceOn = false; }
  
  ///// @brief Nastavenie uvodneho textu, v metode begin treba nastavit max. dzku textu
  ///          text treba vykreslit volanim printText(true)
  /// @param txt smernik na zaciatok textu 
  void setText(char* txt);

  /// @brief Zobrazenie / zmazanie uvodneho textu na displeji
  /// @param show true - zobraz | false vymaz miesto, kde ma byt text
  void printText(bool show);
  
  /// @brief Nastavenie casu v sekundach, ktory sa ma zobrazit
  /// @param seconds  cas v sekundach 
  void setTime(uint16_t seconds); //nastavenie casu v sekundach


  
  /// @brief Vypis casu na displej, funkcia je volana funkciou clock()
  ///        Nemala by sa vola priamo
  void print(); 

  /// @brief Nastavenie blikania dvojbodky
  /// @param blink true - blika | false - neblika
  void blinkColon(bool blink);

  /// @brief Nastavenie blikania casoveho udaju
  /// @param blink true - blika | false - neblika
  void blinkTime(bool blink);

  /// @brief bliká cas aj colon, zacne synchronizovane casom blinkTimeTime
  /// @param blink 
  void blinkAll(bool blink);

  /// @brief Nastavenie zobrazenia/skrytia casoveho udaju - miesto na displeji je pradne
  /// @param show true - zobrazene | false - nezobrazene
  void showTime(bool show);

  /// @brief Nastavenie zobrazenia/skrytia dvojbodky
  /// @param show true - zobrazene | false - nezobrazene
  void showColon(bool show);

  /// @brief Nastavenie zobrazenia celeho casu (cas+dvojbodka)
  /// @param showAll true - zobrazene | false - nezobrazene
  void show(bool showAll);

  /// @brief Updatnutie - prekreslenie udajov na displeji, odporuca sa volat zaroven po static clock() pre vsetky instancie
  void update();


  // "status" udrzuje informaciu o aktualnom stave zobrazenia casu a dvojbodky, v pripade vynimocnej potreby
  // mozno tento status precitat, alebo apisat novy status.
  // Za normalnych okolnosti netreba tieto metody pouzivat

  /// @brief Vrati hodnotu statusu
  /// @return hodnota statusu
  inline uint8_t getStatus() {return status; };

  /// @brief Vrati povodnu hodnotu statusu a nastavi novu
  /// @return povodna hodnota statusu
  inline uint8_t setStatus(uint8_t newStatus) {
    uint8_t previosStatus = status;
    status = newStatus;
    return previosStatus;
  }

  LiquidCrystal_I2C* _lc;


  uint8_t x;  //suradnica x 
  uint8_t y;   //suradnica y
  uint8_t text_length; //dlzka uvodneho textu

  uint16_t time; //zobrazeny cas v sekundach
    
  bool instanceOn;
  char* text;
  
  uint8_t status; //status zobrazenia SHOW|BLINK|VISIBLE|UPDATE pre TIME a pre COLON
  
};


#endif