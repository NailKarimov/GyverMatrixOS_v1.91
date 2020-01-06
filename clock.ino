// режим часов

// ****************** НАСТРОЙКИ ЧАСОВ *****************
#define OVERLAY_CLOCK 0     // часы на фоне всех эффектов и игр. Жрёт SRAM память!
#define CLOCK_ORIENT 0      // 0 горизонтальные, 1 вертикальные
#define CLOCK_X 0           // позиция часов по X (начало координат - левый нижний угол)
#define CLOCK_Y 0           // позиция часов по Y (начало координат - левый нижний угол)
#define WALKING_CLOCK 1     // часы бегают по экрану в clockRoutine()
#define COLOR_MODE 2        // Режим цвета часов
//                          0 - заданные ниже цвета
//                          1 - радужная смена (каждая цифра)
//                          2 - радужная смена (часы, точки, минуты)
#define SEC_COLOR CRGB::White     // цвет минут
#define MIN_COLOR CRGB::White     // цвет минут
#define HOUR_COLOR CRGB::White    // цвет часов
#define DOT_COLOR CRGB::Red       // цвет точек

#define HUE_STEP 5          // шаг цвета часов в режиме радужной смены
#define HUE_GAP 30          // шаг цвета между цифрами в режиме радужной смены

// ****************** ДЛЯ РАЗРАБОТЧИКОВ ****************
byte clockHue;
#if (OVERLAY_CLOCK == 1 && CLOCK_ORIENT == 0)
CRGB overlayLEDs[75];
#elif (OVERLAY_CLOCK == 1 && CLOCK_ORIENT == 1)
CRGB overlayLEDs[70];
#endif

#if (USE_CLOCK == 1)
CRGB clockLED[7] = {CRGB::White, CRGB::White, CRGB::Red, CRGB::White, CRGB::White, CRGB::White, CRGB::White};

void clockColor() {
  if (COLOR_MODE == 0) {
    clockLED[0] = MIN_COLOR;
    clockLED[1] = MIN_COLOR;
    clockLED[2] = DOT_COLOR;
    clockLED[3] = HOUR_COLOR;
    clockLED[4] = HOUR_COLOR;
    clockLED[5] = SEC_COLOR;
    clockLED[6] = SEC_COLOR;
  } else if (COLOR_MODE == 1) {
    for (byte i = 0; i < 7; i++) clockLED[i] = CHSV(clockHue + HUE_GAP * i, 255, 255);
    clockLED[2] = CHSV(clockHue + 128 + HUE_GAP * 1, 255, 255); // точки делаем другой цвет
  } else if (COLOR_MODE == 2) {
    clockLED[0] = CHSV(clockHue + HUE_GAP * 0, 255, 255);
    clockLED[1] = CHSV(clockHue + HUE_GAP * 0, 255, 255);
    clockLED[2] = CHSV(clockHue + 128 + HUE_GAP * 1, 255, 255); // точки делаем другой цвет
    clockLED[3] = CHSV(clockHue + HUE_GAP * 2, 255, 255);
    clockLED[4] = CHSV(clockHue + HUE_GAP * 2, 255, 255);
    clockLED[5] = CHSV(clockHue + HUE_GAP * 2, 255, 255);
    clockLED[6] = CHSV(clockHue + HUE_GAP * 2, 255, 255);
  }
}

// нарисовать часы
void drawClockNewYear(byte hrs, byte mins, byte secs, boolean dots, byte X, byte Y) {
  
 if (hrs < 9) drawDigit3x5(hrs % 10, X, Y, clockLED[0]);
  
    drawPixelXY(X + 4, Y + 1, clockLED[2]);
    drawPixelXY(X + 4, Y + 3, clockLED[2]);
    
    drawPixelXY(X + 11, Y + 1, clockLED[2]);
    drawPixelXY(X + 11, Y + 3, clockLED[2]);

  drawDigit3x5(mins / 10, X + 5, Y, clockLED[3]);
  drawDigit3x5(mins % 10, X + 8, Y, clockLED[4]);
  
  drawDigit3x5(secs / 10, X + 12, Y, clockLED[5]);
  drawDigit3x5(secs % 10, X + 15, Y, clockLED[6]);
}

// нарисовать часы
void drawClock(byte hrs, byte mins, boolean dots, byte X, byte Y) {
#if (CLOCK_ORIENT == 0)
  if (hrs > 9) drawDigit3x5(hrs / 10, X, Y, clockLED[0]);
  drawDigit3x5(hrs % 10, X + 4, Y, clockLED[1]);
  if (dots) {
    drawPixelXY(X + 7, Y + 1, clockLED[2]);
    drawPixelXY(X + 7, Y + 3, clockLED[2]);
  } else {
    if (modeCode == 1) {
      drawPixelXY(X + 7, Y + 1, 0);
      drawPixelXY(X + 7, Y + 3, 0);
    }
  }
  drawDigit3x5(mins / 10, X + 8, Y, clockLED[3]);
  drawDigit3x5(mins % 10, X + 12, Y, clockLED[4]);
#else
  if (hrs > 9) drawDigit3x5(hrs / 10, X, Y + 5, clockLED[0]);
  drawDigit3x5(hrs % 10, X + 4, Y + 5, clockLED[1]);

  drawDigit3x5(mins / 10, X, Y, clockLED[3]);
  drawDigit3x5(mins % 10, X + 4, Y, clockLED[4]);
#endif
}

void clockRoutine() {
  if (loadingFlag) {
#if (MCU_TYPE == 0 || MCU_TYPE == 1)
Serial.begin(9600);
    DateTime now = rtc.now();
    secs = now.second();
    mins = now.minute();
    hrs = now.hour();
#endif
    loadingFlag = false;
    modeCode = 1;
  }
  
  FastLED.clear();
  if (!clockSet) {
    clockTicker();
    drawClock(hrs, mins, dotFlag, CLOCK_X, CLOCK_Y);
  } else {
    if (halfsecTimer.isReady()) {      
      dotFlag = !dotFlag;
      if (dotFlag) clockColor();
      else for (byte i = 0; i < 5; i++) clockLED[i].fadeToBlackBy(190);
    }
    drawClock(hrs, mins, 1, CLOCK_X, CLOCK_Y);
  }
}

void chooseClockBeforeNewYear() {
    DateTime now = rtc.now(); 
    DateTime rtc_new_year = DateTime(2020, 12, 31, 23, 59, 59);
    if (now.year() <= rtc_new_year.year()) {
        clockRoutine();
      } else {
        clockBeforeNewYear();
    }
}

void clockBeforeNewYear() {

Serial.begin(9600);
    Serial.begin(9600);
    DateTime now = rtc.now();   
    
    // calculate date difference between current date and date in future 
    DateTime rtc_new_year = DateTime(2020, 12, 31, 23, 59, 59);
    int diff_days = rtc_new_year.day() - now.day();
    int diff_hours = rtc_new_year.hour() - now.hour();
    int diff_minute = rtc_new_year.minute() - now.minute();
    int diff_second = rtc_new_year.second() - now.second();
    
    Serial.print("До нового года осталось ");
    Serial.print(diff_days);
        Serial.print(":");
    Serial.print(diff_hours);
        Serial.print(":");
    Serial.print(diff_minute);
        Serial.print(":");
    Serial.println(diff_second);
     delay(1000);       
     
    FastLED.clear();
    
  if (!clockSet) {
    clockTicker();
    drawClockNewYear(rtc_new_year.hour() - now.hour(), rtc_new_year.minute() - now.minute(), rtc_new_year.second() - now.second(), 1, CLOCK_X, CLOCK_Y);
  } else {
    if (halfsecTimer.isReady()) {      
      dotFlag = !dotFlag;
      if (dotFlag) clockColor();
      else for (byte i = 0; i < 7; i++) clockLED[i].fadeToBlackBy(190);
    }
    drawClockNewYear(rtc_new_year.hour() - now.hour(), rtc_new_year.minute() - now.minute(), rtc_new_year.second() - now.second(), 1, CLOCK_X, CLOCK_Y);
  }
  
////    Serial.println("До нового года осталось ");
//    str += "До нового года осталось: ";
//    
////    Serial.print(diff_days, DEC);
//    str += diff_days;
//    if (diff_days % 10 == 4 || diff_days % 10 == 3 || diff_days % 10 == 2){
////          Serial.print(" дня ");
//          str += " дня ";
//    } else if (diff_days == 1){
////          Serial.print(" день ");
//          str += " день ";
//    } else {
////      Serial.print(" дней ");
//      str += " дней ";
//    }
//    
////    Serial.print(diff_hours, DEC);
//    str += diff_hours;
//    if (diff_hours % 10 == 4 || diff_hours % 10 == 3 || diff_hours % 10 == 2 ){
////          Serial.print(" часа ");
//      str += " часа ";        
//    } else if (diff_hours == 1){
////          Serial.print(" час ");
//      str += " час "; 
//    } else {
////      Serial.print(" часов ");
//      str += " часов ";
//    }
//    
////    Serial.print(diff_minute, DEC);
//    str += diff_minute;
//    if (diff_minute % 10 == 4 || diff_minute % 10 == 3 || diff_minute % 10 == 2 ){
////          Serial.print(" минуты ");
//          str += " минуты ";
//    } else if (diff_minute == 1){
////          Serial.print(" минута ");
//          str += " минута ";
//    } else {
////      Serial.print(" минут ");
//      str += " минут ";
//    }
////    Serial.print(diff_second, DEC);
//    str += diff_second;
//    if (diff_second % 10 == 4 || diff_second % 10 == 3 || diff_second % 10 == 2 ){
////          Serial.print(" секунды ");
//          str += " секунды ";
//    } else if (diff_second % 10 == 1){
////          Serial.print(" секунда ");
//          str += " секунда ";
//    } else {
////      Serial.print(" секунд ");
//      str += " секунд ";
//    } 
////    Serial.println();
//    
////    fillString(str, 1);
//    Serial.println(str);
}

void clockTicker() {
  if (halfsecTimer.isReady()) {
    clockHue += HUE_STEP;
    if (needColor()) clockColor();

    dotFlag = !dotFlag;
    if (dotFlag) {          // каждую секунду пересчёт времени
      secs++;
      if (secs > 59) {      // каждую минуту
        secs = 0;
        mins++;
#if (MCU_TYPE == 0 || MCU_TYPE == 1)
        DateTime now = rtc.now();
        secs = now.second();
        mins = now.minute();
        hrs = now.hour();
#endif
      }
      if (mins > 59) {      // каждый час
        mins = 0;
        hrs++;
        if (hrs > 23) hrs = 0;  // сутки!
      }
    }
  }
}

boolean needColor() {
  if (modeCode == 1 ||
      modeCode == 9 ||
      modeCode == 12 ||
      modeCode == 13 ||
      modeCode == 14 ||
      modeCode == 15 ||
      modeCode == 16 ||
      modeCode == 17 ||
      modeCode == 20) return true;
  else return false;
}

#else
void clockRoutine() {
  return;
}
#endif

#if (CLOCK_ORIENT == 0 && USE_CLOCK == 1 && OVERLAY_CLOCK == 1)
void clockOverlayWrap(byte posX, byte posY) {
  byte thisLED = 0;
  for (byte i = posX; i < posX + 15; i++) {
    for (byte j = posY; j < posY + 5; j++) {
      overlayLEDs[thisLED] = leds[getPixelNumber(i, j)];
      thisLED++;
    }
  }
  clockTicker();
  drawClock(hrs, mins, dotFlag, posX, posY);
}

void clockOverlayUnwrap(byte posX, byte posY) {
  byte thisLED = 0;
  for (byte i = posX; i < posX + 15; i++) {
    for (byte j = posY; j < posY + 5; j++) {
      leds[getPixelNumber(i, j)] = overlayLEDs[thisLED];
      thisLED++;
    }
  }
}

#elif (CLOCK_ORIENT == 1 && USE_CLOCK == 1 && OVERLAY_CLOCK == 1)
void clockOverlayWrap(byte posX, byte posY) {
  byte thisLED = 0;
  for (byte i = posX; i < posX + 7; i++) {
    for (byte j = posY; j < posY + 10; j++) {
      overlayLEDs[thisLED] = leds[getPixelNumber(i, j)];
      thisLED++;
    }
  }
  clockTicker();
  drawClock(hrs, mins, dotFlag, posX, posY);
}

void clockOverlayUnwrap(byte posX, byte posY) {
  byte thisLED = 0;
  for (byte i = posX; i < posX + 7; i++) {
    for (byte j = posY; j < posY + 10; j++) {
      leds[getPixelNumber(i, j)] = overlayLEDs[thisLED];
      thisLED++;
    }
  }
}
#endif

#if (OVERLAY_CLOCK == 1 && USE_CLOCK == 1)
boolean needUnwrap() {
  if (modeCode == 12 ||
      modeCode == 13 ||
      modeCode == 14 ||
      modeCode == 15 ||
      modeCode == 17 ||
      modeCode == 20) return true;
  else return false;
}

void blackClock() {
  for (byte i = 0; i < 5; i++) clockLED[i] = 0;
  clockLED[2] = CRGB::White;
}

void setOverlayColors() {
  switch (modeCode) {
    case 1: clockColor();
      break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8: blackClock();
      break;
    case 9: clockColor();
      break;
    case 10:
    case 11: blackClock();
      break;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17: clockColor();
      break;
    case 18:
    case 19: blackClock();
      break;
    case 20: clockColor();
      break;
  }
}

#else
void clockOverlayWrap(byte posX, byte posY) {
  return;
}
void clockOverlayUnwrap(byte posX, byte posY) {
  return;
}
boolean needUnwrap() {
  return true;
}
#endif
