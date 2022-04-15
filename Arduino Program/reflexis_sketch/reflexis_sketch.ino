// 2, 4, 18, 19, 21, 22, 23, 13, 12, 27, 26, 25, 33, 32 – нормальные обычные пины
#include <Nextion.h>
#include <Arduino.h>
#include <DFPlayer_Mini_Mp3.h>
#include <EEPROM.h>
#include <MedianFilter.h>

SoftwareSerial mp3Serial(18, 19);

#define INA 21
#define INB 22
#define MOTORPWM 23
#define ANGLEPIN 25
#define EMGPIN 26
#define eepromSize 2
#define DEBUG_SERIAL_ENABLE

bool cameFromBlocks = false;
int highLimit;                //2420
int lowLimit;                 //1100

MedianFilter test(50, lowLimit);

NexSlider sliderBrightness = NexSlider(4, 1, "h0");
NexSlider sliderVolume = NexSlider(4, 4, "h1");

NexHotspot startPageToSettings = NexHotspot(1, 1, "m0");
NexHotspot createProgramToSettings = NexHotspot(2, 2, "m1");
NexHotspot lopToSettings = NexHotspot(3, 2, "m2");
NexHotspot createBlockActToSettings = NexHotspot(5, 22, "m9");
NexHotspot createBlockPasToSettings = NexHotspot(11, 2, "m1");
NexHotspot fromSettings = NexHotspot(4, 2, "m0");
NexHotspot createProgActToSettings = NexHotspot(9, 2, "m1");
NexHotspot createProgPasToSettings = NexHotspot(10, 2, "m1");
NexHotspot contactsToSettings = NexHotspot(6, 1, "m0");

NexHotspot activeAdd = NexHotspot(5, 23, "m10");
NexHotspot passiveAdd = NexHotspot(11, 16, "m11");

NexHotspot actLeft = NexHotspot(9, 11, "m5");
NexHotspot actRight = NexHotspot(9, 12, "m6");

NexHotspot pasLeft = NexHotspot(10, 10, "m5");
NexHotspot pasRight = NexHotspot(10, 11, "m6");

NexHotspot delAct = NexHotspot(9, 3, "m2");
NexHotspot delPas = NexHotspot(10, 3, "m2");

NexHotspot delProgStart = NexHotspot(2, 1, "m0");
NexHotspot delProgAct = NexHotspot(9, 1, "m0");
NexHotspot delProgPas = NexHotspot(10, 1, "m0");

NexHotspot backFromCreateBlockA = NexHotspot(5, 13, "m0");
NexHotspot backFromCreateBlockP = NexHotspot(11, 1, "m0");

NexHotspot addBlockFromA = NexHotspot(9, 5, "m4");
NexHotspot addBlockFromP = NexHotspot(10, 4, "m3");

NexHotspot executeProgramA = NexHotspot(9, 4, "m3");
NexHotspot executeProgramP = NexHotspot(10, 5, "m4");

NexHotspot stopProgramA = NexHotspot(8, 1, "m0");
NexHotspot stopProgramP = NexHotspot(7, 1, "m0");

NexHotspot contacts = NexHotspot(4, 3, "m1");

NexHotspot firstDebug = NexHotspot(6, 2, "m1");

NexHotspot firstProgram = NexHotspot(3, 3, "m0");
NexHotspot secondProgram = NexHotspot(3, 4, "m3");
NexHotspot thirdProgram = NexHotspot(3, 5, "m4");

NexButton startDebug = NexButton(12, 4, "b1");
NexButton stopDebug = NexButton(12, 3, "b0");

NexPage beforeSettings = NexPage(1, 0, "StartPage");
NexPage settingsPage = NexPage(4, 0, "Settings");

NexPage createProgramPas = NexPage(10, 0, "CreateProgPas");
NexPage createProgramAct = NexPage(9, 0, "CreateProgAct");

NexTimer timerLoading = NexTimer(0, 1, "tm0");

NexTouch *loop_listen_list[] =
{
  &sliderBrightness,              //------------------СЛАЙДЕРЫ В НАСТРОЙКАХ
  &sliderVolume,
  &startPageToSettings,           //------------------ДЛЯ ПЕРЕХОДА ЭКРАНА НАСТРОЕК
  &createProgramToSettings,
  &lopToSettings,
  &createBlockActToSettings,
  &createBlockPasToSettings,
  &fromSettings,
  &createProgActToSettings,
  &createProgPasToSettings,
  &activeAdd,
  &passiveAdd,
  &createProgramPas,
  &createProgramAct,
  &actLeft,
  &actRight,
  &pasLeft,
  &pasRight,
  &delAct,
  &delPas,
  &backFromCreateBlockA,
  &backFromCreateBlockP,
  &delProgStart,
  &delProgAct,
  &delProgPas,
  &addBlockFromA,
  &addBlockFromP,
  &executeProgramA,
  &executeProgramP,
  &startDebug,
  &contacts,
  &contactsToSettings,
  &firstDebug,
  &firstProgram,
  &secondProgram,
  &thirdProgram,
  NULL
};

NexTouch *debug_listen_list[] =
{
  &stopDebug,
  NULL
};

NexTouch *active_listen_list[] =
{
  &stopProgramA,
  NULL
};

NexTouch *passive_listen_list[] =
{
  &stopProgramP,
  NULL
};

//----------------------------------------------------ПЕРЕХОД ЭКРАНА НАСТРОЕК

void startPageToSettingsPopCallback(void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to settings from start page");
  beforeSettings = NexPage(1, 0, "StartPage");
  cameFromBlocks = false;
  settingsPage.show();
  sliderBrightness.setValue(EEPROM.read(0));
  sliderVolume.setValue(EEPROM.read(1));
}

void createProgramToSettingsPopCallback(void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to settings from CreateProgram page");
  beforeSettings = NexPage(2, 0, "CreateProgram");
  cameFromBlocks = false;
  settingsPage.show();
  sliderBrightness.setValue(EEPROM.read(0));
  sliderVolume.setValue(EEPROM.read(1));
}

void lopToSettingsPopCallback(void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to settings from LOP1 page");
  beforeSettings = NexPage(3, 0, "LOP1");
  cameFromBlocks = false;
  settingsPage.show();
  sliderBrightness.setValue(EEPROM.read(0));
  sliderVolume.setValue(EEPROM.read(1));
}

void createBlockActToSettingsPopCallback(void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to settings from CreateBlockA page");
  beforeSettings = NexPage(5, 0, "CreateBlockA");
  cameFromBlocks = false;
  settingsPage.show();
  sliderBrightness.setValue(EEPROM.read(0));
  sliderVolume.setValue(EEPROM.read(1));
}

void createBlockPasToSettingsPopCallback(void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to settings from CreateBlockP page");
  beforeSettings = NexPage(11, 0, "CreateBlockP");
  cameFromBlocks = false;
  settingsPage.show();
  sliderBrightness.setValue(EEPROM.read(0));
  sliderVolume.setValue(EEPROM.read(1));
}

void createProgActToSettingsPopCallback(void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to settings from CreateProgPas page");
  cameFromBlocks = true;
  settingsPage.show();
  sliderBrightness.setValue(EEPROM.read(0));
  sliderVolume.setValue(EEPROM.read(1));
}

void createProgPasToSettingsPopCallback(void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to settings from CreateProgPas page");
  cameFromBlocks = true;
  settingsPage.show();
  sliderBrightness.setValue(EEPROM.read(0));
  sliderVolume.setValue(EEPROM.read(1));
}

void fromContactsToSettings (void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to settings from Contacts");
  cameFromBlocks = false;
  settingsPage.show();
  sliderBrightness.setValue(EEPROM.read(0));
  sliderVolume.setValue(EEPROM.read(1));
}

void fromSettingsToContacts (void *ptr)
{
  mp3_play(2);
  dbSerialPrintln("Go to contacts from settings");
  cameFromBlocks = false;
  uint32_t brightness = 0;
  uint32_t volume = 0;
  sliderBrightness.getValue(&brightness);
  sliderVolume.getValue(&volume);
  EEPROM.write(0, brightness);
  EEPROM.commit();
  EEPROM.write(1, volume);
  EEPROM.commit();
  Serial2.print("page Contacts");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}


struct trainingProgram {
  int participation;
  int spd;
  int angleStart;
  int angleEnd;
  int amount;
  int tm;
  bool isActive;
};

trainingProgram progs[10] = {(trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram){ -1, 0, 0, 0, 0, 0, false}, (trainingProgram) {
  -1, 0, 0, 0, 0, 0, false
}
                            };
int currentBlock = -1;

void fromSettingsPopCallback(void *ptr)
{
  mp3_play(2);
  uint32_t brightness = 0;
  uint32_t volume = 0;
  sliderBrightness.getValue(&brightness);
  sliderVolume.getValue(&volume);
  EEPROM.write(0, brightness);
  EEPROM.commit();
  EEPROM.write(1, volume);
  EEPROM.commit();
  delay(10);
  mp3_set_volume (EEPROM.read(1));
  delay(10);
  if (cameFromBlocks) {
    if (progs[currentBlock].isActive) {
      showActiveBlock(ptr);
    } else {
      showPassiveBlock(ptr);
    }
    return;
  }
  beforeSettings.show();
}

//---------------------------------------------------ВЫПОЛНЕНИЕ ПАССИВНОЙ ПРОГРАММЫ
bool isStopped = false;
bool isPremade = false;

void passiveProgramm(int spd, int loAngle, int hiAngle, int minsTime, int number, void *ptr) {
  Serial2.print("page PassiveExec");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  String spdString = "n1.val=" + String(spd);
  Serial2.print(spdString);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  String num = "n0.val=" + String(number);
  Serial2.print(num);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  String tm = "n2.val=" + String(minsTime - 1);
  Serial2.print(tm);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.print("tm1.en=1");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  int spdNorm = 95 + spd * 16;
  int curTime = millis();
  lowLimit = analogRead(ANGLEPIN);
  highLimit = lowLimit + 1345;
  MedianFilter test(50, lowLimit);
  int curAngle = calibrateAngle(analogRead(ANGLEPIN));
  ledcWrite(0, spdNorm);
  //dbSerialPrintln(curTime);
  //dbSerialPrintln(millis() - curTime);
  //dbSerialPrintln(minsTime * 60 * 1000);
  //dbSerialPrintln(spdNorm);
  mp3_play(3);
  delay(2000);
  while (millis() - curTime < minsTime * 60 * 1000) {
    digitalWrite(INA, LOW);            //сгибаем руку
    digitalWrite(INB, HIGH);
    mp3_play(6);
    while (abs(curAngle - hiAngle) > 3 && millis() - curTime < minsTime * 60 * 1000) {
      curAngle = calibrateAngle(analogRead(ANGLEPIN));
      dbSerialPrintln(curAngle);
      nexListen(passive_listen_list);
      if (isStopped) {
        return;
      }
      delay(50);
    }

    digitalWrite(INB, LOW);         // разогибаем руку
    digitalWrite(INA, HIGH);
    mp3_play(7);
    while (abs(curAngle - loAngle) > 3 && millis() - curTime < minsTime * 60 * 1000) {
      curAngle = calibrateAngle(analogRead(ANGLEPIN));
      nexListen(passive_listen_list);
      if (isStopped) {
        return;
      }
      delay(50);
    }
  }

  ledcWrite(0, 111);
  while (abs(curAngle) > 2) {
    curAngle = calibrateAngle(analogRead(ANGLEPIN));
    delay(50);
  }
}

//---------------------------------------------------ВЫПОЛНЕНИЕ АКТИВНОЙ ПРОГРАММЫ

void activeProgramm(int involvment, int spd, int loAngle, int hiAngle, int flex, int number, void *ptr) {
  Serial2.print("page ActiveExec");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  String num = "n3.val=" + String(number);
  Serial2.print(num);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  String spdString = "n2.val=" + String(spd);
  Serial2.print(spdString);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  String flexMax = "n0.val=" + String(flex);
  Serial2.print(flexMax);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  String participation;
  int spdNorm = 100 + spd * 11;
  int flexCounter = 0;
  String flexAmount;
  lowLimit = analogRead(ANGLEPIN);
  highLimit = lowLimit + 1345;
  MedianFilter test(50, lowLimit);
  int curAngle = calibrateAngle(analogRead(ANGLEPIN));
  int curEmg = 0;
  int curEmgNorm;
  ledcWrite(0, 111);
  mp3_play(4);

  digitalWrite(INA, LOW);
  digitalWrite(INB, HIGH);

  while (abs(curAngle - hiAngle) > 3) {
    nexListen(active_listen_list);
    if (isStopped) {
      return;
    }
    curAngle = calibrateAngle(analogRead(ANGLEPIN));
    delay(50);
  }
  mp3_play(5);
  delay(6000);

  while (flexCounter < flex) {
    flexAmount = "n1.val=" + String(flexCounter);
    Serial2.print(flexAmount);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);

    digitalWrite(INB, LOW);         // разгибаем руку
    digitalWrite(INA, HIGH);
    mp3_play(7);
    ledcWrite(0, spdNorm);
    while (abs(curAngle - loAngle) > 3) {
      curAngle = calibrateAngle(analogRead(ANGLEPIN));
      curEmg = analogRead(EMGPIN);
      curEmgNorm = map(curEmg, 1800, 4086, 0, 100);
      participation = "j0.val=" + String(curEmgNorm);
      Serial2.print(participation);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      nexListen(active_listen_list);
      if (isStopped) {
        return;
      }
      if (curEmg >= 585 + involvment * 585) {
        ledcWrite(0, spdNorm + 45);
        mp3_play(random(8, 10));
        delay(2000);
        ledcWrite(0, spdNorm);
      }
      delay(50);
    }

    digitalWrite(INA, LOW);            //сгибаем руку
    digitalWrite(INB, HIGH);
    mp3_play(6);
    ledcWrite(0, spdNorm);
    while (abs(curAngle - hiAngle) > 3) {
      curAngle = calibrateAngle(analogRead(ANGLEPIN));
      curEmg = analogRead(EMGPIN);
      curEmgNorm = map(curEmg, 1800, 4086, 0, 100);
      participation = "j0.val=" + String(curEmgNorm);
      Serial2.print(participation);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      nexListen(active_listen_list);
      if (isStopped) {
        return;
      }
      if (curEmg >= 585 + involvment * 585) {
        ledcWrite(0, spdNorm + 45);
        mp3_play(random(8, 10));
        delay(2000);
        ledcWrite(0, spdNorm);
      }
      delay(50);
    }
    flexCounter++;
  }
  flexAmount = "n1.val=" + String(flexCounter);
  Serial2.print(flexAmount);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  ledcWrite(0, 111);
  digitalWrite(INB, LOW);
  digitalWrite(INA, HIGH);
  while (abs(curAngle) > 3) {
    curAngle = calibrateAngle(analogRead(ANGLEPIN));
    delay(50);
  }
}

//Выполнение составленной программы

void executePrograms(void *ptr) {
  int i = 0;
  while (progs[i].participation != -1) {
    if (progs[i].isActive) {
      activeProgramm(progs[i].participation, progs[i].spd,  progs[i].angleStart, progs[i].angleEnd, progs[i].amount, i + 1, ptr);
    } else {
      passiveProgramm(progs[i].spd, progs[i].angleStart, progs[i].angleEnd, progs[i].tm, i + 1, ptr);
    }
    i++;
    if (i == 10) {
      break;
    }
    if (isStopped) {
      isStopped = false;
      break;
    }
  }
  mp3_play(11);
  if (!isPremade) {
    if (progs[currentBlock].isActive) {
      showActiveBlock(ptr);
    } else {
      showPassiveBlock(ptr);
    }
  } else {
    isPremade = false;
    Serial2.print("page LOP1");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
}

//Остановка программы

void stopProgram(void *ptr) {
  dbSerialPrintln("СТОООООООП");
  mp3_play(2);
  delay(100);
  mp3_play(12);
  if (!isPremade) {
    if (progs[currentBlock].isActive) {
      showActiveBlock(ptr);
    } else {
      showPassiveBlock(ptr);
    }
  } else {
    isPremade = false;
    Serial2.print("page LOP1");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  isStopped = true;
  ledcWrite(0, 127);
  digitalWrite(INB, LOW);         // разгибаем руку
  digitalWrite(INA, HIGH);
  /*  int curAngle = calibrateAngle(analogRead(ANGLEPIN));
    while (abs(curAngle) > 2) {
      curAngle = calibrateAngle(analogRead(ANGLEPIN));
      //dbSerialPrintln(analogRead(ANGLEPIN));
      //dbSerialPrintln(curAngle);
      delay(50);
    }*/
  /*НЕ УВЕРЕН НАДО ЛИ ТУТ ЕЩЕ ЧТО-НИБУДЬ ОСТАНАВЛИВАТЬ*/
}

//------------------------------------------------------Блоки и их чтение
//----------------------------------------------------Активные
void addActivePopCallBack (void *ptr) {
  mp3_play(2);
  NexNumber an0 = NexNumber(5, 1, "n0");
  NexNumber an1 = NexNumber(5, 2, "n1");
  NexNumber an2 = NexNumber(5, 3, "n2");
  NexNumber an3 = NexNumber(5, 4, "n3");
  NexNumber an4 = NexNumber(5, 25, "n4");
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      return;
    }
  }
  uint32_t participation;
  uint32_t spd;
  uint32_t angleStart;
  uint32_t angleEnd;
  uint32_t amount;
  an0.getValue(&participation);
  an1.getValue(&angleStart);
  an2.getValue(&spd);
  an3.getValue(&amount);
  an4.getValue(&angleEnd);
  trainingProgram newProg = {(int)participation, (int)spd, (int)angleStart, (int)angleEnd, (int)amount, -1, true};
  progs[i] = newProg;
  currentBlock = i;
  Serial2.print("page CreateProgAct");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(1);
  showActiveBlock(ptr);
}

void showActiveBlock(void *ptr) {
  mp3_play(2);
  if (currentBlock == -1) {
    return;
  }
  Serial2.print("page CreateProgAct");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      break;
    }
  }
  if ((i == currentBlock + 1) && (currentBlock != 0)) {
    Serial2.print("CreateProgAct.pic=16");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  } else {
    if (i == 1) {
      Serial2.print("CreateProgAct.pic=17");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
    } else {
      if (currentBlock == 0) {
        Serial2.print("CreateProgAct.pic=14");
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
      } else {
        Serial2.print("CreateProgAct.pic=15");
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
      }
    }
  }
  String participation = "n0.val=" + String(progs[currentBlock].participation);
  String spd = "n1.val=" + String(progs[currentBlock].spd);
  String angleStart = "n2.val=" + String(progs[currentBlock].angleStart);
  String amount = "n3.val=" + String(progs[currentBlock].amount);
  String angleEnd = "n4.val=" + String(progs[currentBlock].angleEnd);
  String number = "n5.val=" + String(currentBlock + 1);

  Serial2.print(participation);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(spd);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(angleStart);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(amount);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(angleEnd);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(number);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void actLeftPush(void *ptr) {
  mp3_play(2);
  if (currentBlock == 0) {
    return;
  }
  currentBlock--;
  if (progs[currentBlock].isActive) {
    showActiveBlock(ptr);
  } else {
    showPassiveBlock(ptr);
  }
}

void actRightPush(void *ptr) {
  mp3_play(2);
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      break;
    }
  }
  if (i == currentBlock + 1) {
    return;
  }
  currentBlock++;
  if (progs[currentBlock].isActive) {
    showActiveBlock(ptr);
  } else {
    showPassiveBlock(ptr);
  }
}

void delBlock (void *ptr) {
  mp3_play(2);
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      break;
    }
  }
  progs[currentBlock] = (trainingProgram) {
    -1, 0, 0, 0, 0, 0, false
  };
  for (int j = currentBlock; j < i - 1; j++) {
    progs[j] = progs[j + 1];
    progs[j + 1] = (trainingProgram) {
      -1, 0, 0, 0, 0, 0, false
    };
  }
  if (currentBlock == 0) {
    if (i == 1) {
      currentBlock = -1;
      Serial2.print("page CreateProgram");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      return;
    } else {
      currentBlock = 0;
    }
  } else {
    currentBlock--;
  }
  if (progs[currentBlock].isActive) {
    showActiveBlock(ptr);
  } else {
    showPassiveBlock(ptr);
  }
}

//----------------------------------------------------Пассивное
void addPassivePopCallBack (void *ptr) {
  mp3_play(2);
  NexNumber pn0 = NexNumber(11, 4, "n0");
  NexNumber pn1 = NexNumber(11, 5, "n1");
  NexNumber pn2 = NexNumber(11, 6, "n2");
  NexNumber pn3 = NexNumber(11, 7, "n3");
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      return;
    }
  }
  uint32_t spd;
  uint32_t angleStart;
  uint32_t angleEnd;
  uint32_t tm;
  pn0.getValue(&spd);
  pn1.getValue(&angleStart);
  pn2.getValue(&tm);
  pn3.getValue(&angleEnd);
  trainingProgram newProg = {0, (int)spd, (int)angleStart,  (int)angleEnd, 0, (int)tm, false};
  progs[i] = newProg;
  currentBlock = i;
  Serial2.print("page CreateProgPas");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(1);
  showPassiveBlock(ptr);
}

void showPassiveBlock(void *ptr) {
  mp3_play(2);
  if (currentBlock == -1) {
    return;
  }
  Serial2.print("page CreateProgPas");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(1);
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      break;
    }
  }
  if (i == currentBlock + 1 && currentBlock != 0) {
    Serial2.print("CreateProgPas.pic=8");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  } else {
    if (i == 1) {
      Serial2.print("CreateProgPas.pic=7");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
    } else {
      if (currentBlock == 0) {
        Serial2.print("CreateProgPas.pic=10");
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
      } else {
        Serial2.print("CreateProgPas.pic=9");
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
      }
    }
  }
  String spd = "n0.val=" + String(progs[currentBlock].spd);
  String angleStart = "n1.val=" + String(progs[currentBlock].angleStart);
  String angleEnd = "n2.val=" + String(progs[currentBlock].angleEnd);
  String tm = "n3.val=" + String(progs[currentBlock].tm);
  String number = "n4.val=" + String(currentBlock + 1);

  Serial2.print(spd);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(angleStart);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(angleEnd);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(tm);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print(number);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}
void pasLeftPush(void *ptr) {
  mp3_play(2);
  if (currentBlock == 0) {
    return;
  }

  currentBlock--;
  if (progs[currentBlock].isActive) {
    showActiveBlock(ptr);
  } else {
    showPassiveBlock(ptr);
  }
}

void pasRightPush(void *ptr) {
  mp3_play(2);
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      break;
    }
  }
  if (i == currentBlock + 1) {
    return;
  }
  currentBlock++;
  if (progs[currentBlock].isActive) {
    showActiveBlock(ptr);
  } else {
    showPassiveBlock(ptr);
  }
}


void backFromCreate(void *ptr) {
  mp3_play(2);
  if (currentBlock == -1) {
    Serial2.print("page CreateProgram");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    return;
  }
  if (progs[currentBlock].isActive) {
    showActiveBlock(ptr);
  } else {
    showPassiveBlock(ptr);
  }
}

void backToStart(void *ptr) {
  mp3_play(2);
  currentBlock = -1;
  for (int i = 0; i < 10; i++) {
    progs[i] = (trainingProgram) {
      -1, 0, 0, 0, 0, 0, false
    };
  }
  Serial2.print("page StartPage");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void toAddBlockA(void *ptr) {
  mp3_play(2);
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      return;
    }
  }
  if (i < 10) {
    Serial2.print("page CreateBlockA");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
}

void toAddBlockB(void *ptr) {
  mp3_play(2);
  int i = 0;
  while (progs[i].participation != -1) {
    i++;
    if (i == 10) {
      return;
    }
  }
  if (i < 10) {
    Serial2.print("page CreateBlockP");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
}
//----------------------------------------------------DEBUG

void debugPressedFirst(void *ptr) {
  dbSerialPrintln("debuuuuug");
  Serial2.print("page Debug");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

bool isDebugStopped = false;

void stopDebugPush(void *ptr) {
  isDebugStopped = true;
  ledcWrite(0, 127);
  digitalWrite(INB, LOW);         // разгибаем руку
  digitalWrite(INA, HIGH);
  int curAngle = calibrateAngle(analogRead(ANGLEPIN));
  while (abs(curAngle) > 2) {
    curAngle = calibrateAngle(analogRead(ANGLEPIN));
    String currentAngle = "n0.val=" + String(curAngle);
    Serial2.print(currentAngle);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(50);
  }
}

void startDebugPush(void *ptr) {
  int spdNorm = 127;
  int flexCounter = 0;
  String currentAngle;
  String currentEmg;
  lowLimit = analogRead(ANGLEPIN);
  highLimit = lowLimit + 1345;
  MedianFilter test(50, lowLimit);
  int curAngle = calibrateAngle(analogRead(ANGLEPIN));
  int curEmg = 0;
  int curEmgNorm;
  ledcWrite(0, 127);

  digitalWrite(INA, LOW);
  digitalWrite(INB, HIGH);

  while (abs(curAngle - 135) > 3) {
    if (isDebugStopped) {
      isDebugStopped = false;
      return;
    }
    //dbSerialPrintln(analogRead(ANGLEPIN));
    curAngle = calibrateAngle(analogRead(ANGLEPIN));
    //dbSerialPrintln(curAngle);
    currentAngle = "n0.val=" + String(curAngle);
    Serial2.print(currentAngle);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(50);
  }

  ledcWrite(0, spdNorm);
  delay(700);
  while (flexCounter < 9999) {
    delay(1000);
    digitalWrite(INB, LOW);         // разгибаем руку
    digitalWrite(INA, HIGH);
    while (abs(curAngle) > 3) {
      //dbSerialPrintln(analogRead(ANGLEPIN));
      //dbSerialPrintln(curAngle);
      nexListen(debug_listen_list);
      curAngle = calibrateAngle(analogRead(ANGLEPIN));
      curEmg = analogRead(EMGPIN);
      currentAngle = "n0.val=" + String(curAngle);
      Serial2.print(currentAngle);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      currentEmg = "n1.val=" + String(curEmg);
      Serial2.print(currentEmg);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      //curEmgNorm = map(curEmg, 0, 4096, 0, 201);
      curEmgNorm = map(curAngle, 0, 135, 0, 201);
      String curEmgNormGraph = "add 1,0," + String(curEmgNorm);
      Serial2.print(curEmgNormGraph);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      if (isDebugStopped) {
        isDebugStopped = false;
        return;
      }
      delay(50);
    }

    delay(1000);
    digitalWrite(INA, LOW);            //сгибаем руку
    digitalWrite(INB, HIGH);
    while (abs(curAngle - 135) > 3) {
      //dbSerialPrintln(analogRead(ANGLEPIN));
      //dbSerialPrintln(curAngle);
      nexListen(debug_listen_list);
      curAngle = calibrateAngle(analogRead(ANGLEPIN));
      curEmg = analogRead(EMGPIN);
      //curEmgNorm = map(curEmg, 0, 4096, 0, 201);
      curEmgNorm = map(curAngle, 0, 135, 0, 201);
      currentAngle = "n0.val=" + String(curAngle);
      Serial2.print(currentAngle);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      currentEmg = "n1.val=" + String(curEmg);
      Serial2.print(currentEmg);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      String curEmgNormGraph = "add 1,0," + String(curEmgNorm);
      Serial2.print(curEmgNormGraph);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      if (isDebugStopped) {
        isDebugStopped = false;
        return;
      }
      delay(50);
    }
    flexCounter++;
  }

  ledcWrite(0, 127);
  digitalWrite(INB, LOW);
  digitalWrite(INA, HIGH);
  while (abs(curAngle) > 2) {
    curAngle = calibrateAngle(analogRead(ANGLEPIN));
    delay(50);
  }
  digitalWrite(INB, LOW);         //стоп
  digitalWrite(INA, LOW);
}

//Предзаготовленные программы

//Комбинированная
void launchFirstProgram(void *ptr) {
  isPremade = true;
  progs[0] = {0, 1, 0,  135, 0, 1, false};
  progs[1] = {1, 2, 0,  135, 1, 0, true};
  progs[2] = {0, 4, 0,  135, 0, 1, false};
  progs[3] = {2, 5, 0,  135, 2, 0, true};
  progs[4] = {0, 7, 0,  135, 0, 1, false};
  progs[5] = {3, 8, 0,  135, 3, 1, false};
  progs[6] = {0, 2, 0,  135, 0, 1, false};
  executePrograms(ptr);
  for (int i = 0; i < 10; i++) {
    progs[i] = { -1, 0, 0,  0, 0, 0, false};
  }
}

//Увеличение скорости
void launchSecondProgram(void *ptr) {
  isPremade = true;
  progs[0] = {0, 1, 0,  135, 0, 1, false};
  progs[1] = {0, 3, 0,  135, 0, 1, false};
  progs[2] = {0, 5, 0,  135, 0, 1, false};
  progs[3] = {0, 7, 0,  135, 0, 1, false};
  progs[4] = {0, 9, 0,  135, 0, 1, false};
  executePrograms(ptr);
  for (int i = 0; i < 10; i++) {
    progs[i] = { -1, 0, 0,  0, 0, 0, false};
  }
}

//Минимальная активноть
void launchThirdProgram(void *ptr) {
  isPremade = true;
  progs[0] = { 0, 5, 0,  135, 5, 0, true};
  executePrograms(ptr);
  for (int i = 0; i < 10; i++) {
    progs[i] = { -1, 0, 0,  0, 0, 0, false};
  }
}

int angleArray[7] = {0, 0, 0, 0, 0, 0, 0};

int calibrateAngle(int angle) {
  /*int sum = 0;
    int newAngle = map(angle, lowLimit, highLimit, -1, 136);
    if (abs(newAngle - angleArray[6]) > 50) {
    lowLimit += angle - map(angleArray[6], -1, 136, lowLimit, highLimit);
    highLimit = lowLimit + 1585;
    newAngle = map(angle, lowLimit, highLimit, -1, 136);
    }
    if (newAngle < -1) {
    newAngle = -1;
    } else if (newAngle > 136) {
    newAngle = 136;
    }
    for (int i = 0; i < 6; i++) {
    angleArray[i] = angleArray[i + 1];
    }
    angleArray[6] = newAngle;
    for (int i = 0; i < 7; i++) {
    sum += angleArray[i];
    }
    return sum / 7;*/
  if (angle < lowLimit) {
    angle = lowLimit;
  } else if (angle > highLimit) {
    angle =  highLimit;
  }
  test.in(angle);
  int result = map(test.out(), lowLimit, highLimit, -1, 136);
  if (result <= -1) {
    result = 0;
  } else if (result >= 136) {
    result = 135;
  }
  return result;
}

void nexListen(NexTouch *nex_listen_list[])
{
  static uint8_t ___buffer[10];

  uint16_t i;
  uint8_t c;
  while (nexSerial.available() > 0) {
    c = nexSerial.read();
    if ((0x65) == c)
    {
      if (nexSerial.available() >= 6)
      {
        ___buffer[0] = c;
        for (i = 1; i < 7; i++)
        {
          ___buffer[i] = nexSerial.read();
        }
        ___buffer[i] = 0x00;

        if (0xFF == ___buffer[4] && 0xFF == ___buffer[5] && 0xFF == ___buffer[6])
        {
          NexTouch::iterate(nex_listen_list, ___buffer[1], ___buffer[2], (int32_t)___buffer[3]);
        }
      }
    }
  }
}

//----------------------------------------------------SETUP

void setup(void)
{
  Serial.begin(9600);
  Serial2.begin(9600); // открытие порта дисплея на стандартной скорости
  mp3Serial.begin(9600);
  EEPROM.begin(eepromSize);
  delay(100);

  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(MOTORPWM, OUTPUT);
  ledcSetup(0, 5000, 8);

  nexInit();

  startPageToSettings.attachPush(startPageToSettingsPopCallback);
  createProgramToSettings.attachPush(createProgramToSettingsPopCallback);
  lopToSettings.attachPush(lopToSettingsPopCallback);
  createBlockActToSettings.attachPush(createBlockActToSettingsPopCallback);
  createBlockPasToSettings.attachPush(createBlockPasToSettingsPopCallback);
  createProgActToSettings.attachPush(createProgActToSettingsPopCallback);
  createProgPasToSettings.attachPush(createProgPasToSettingsPopCallback);
  fromSettings.attachPush(fromSettingsPopCallback);

  activeAdd.attachPush(addActivePopCallBack);
  passiveAdd.attachPush(addPassivePopCallBack);

  actLeft.attachPush(actLeftPush);
  actRight.attachPush(actRightPush);

  pasLeft.attachPush(pasLeftPush);
  pasRight.attachPush(pasRightPush);

  delAct.attachPush(delBlock);
  delPas.attachPush(delBlock);

  delProgStart.attachPush(backToStart);
  delProgAct.attachPush(backToStart);
  delProgPas.attachPush(backToStart);

  backFromCreateBlockA.attachPush(backFromCreate);
  backFromCreateBlockP.attachPush(backFromCreate);

  addBlockFromA.attachPush(toAddBlockA);
  addBlockFromP.attachPush(toAddBlockB);

  executeProgramA.attachPush(executePrograms);
  executeProgramP.attachPush(executePrograms);

  stopProgramA.attachPush(stopProgram);
  stopProgramP.attachPush(stopProgram);

  contacts.attachPush(fromSettingsToContacts);
  contactsToSettings.attachPush(fromContactsToSettings);

  firstDebug.attachPush(debugPressedFirst);
  //secondDebug.attachPop(debugPressedSecond);
  //thirdDebug.attachPop(debugPressedThird);

  startDebug.attachPush(startDebugPush);
  stopDebug.attachPush(stopDebugPush);

  firstProgram.attachPush(launchFirstProgram);
  secondProgram.attachPush(launchSecondProgram);
  thirdProgram.attachPush(launchThirdProgram);

  timerLoading.enable();

  ledcAttachPin(MOTORPWM, 0);

  //dbSerialPrintln("setup done");
  //dbSerialPrintln("lol");

  delay(700);
  Serial2.print("dim=");
  Serial2.print(EEPROM.read(0));
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(10);
  mp3_set_serial (mp3Serial);
  delay(10);
  mp3_set_volume (EEPROM.read(1));
  delay(10);
  mp3_play(1);
  lowLimit = analogRead(ANGLEPIN);
  highLimit = lowLimit + 1585;
  ledcWrite(0, 127);
  digitalWrite(INB, LOW);         // разгибаем руку
  digitalWrite(INA, HIGH);
}

//----------------------------------------------------LOOP

void loop(void)
{
  nexLoop(loop_listen_list);
}
