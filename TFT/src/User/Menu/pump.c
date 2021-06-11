#include "pump.h"
#include "includes.h"



#define SCRIPT_PUMP_ON  "M42 P205 S255\nM42 P125 S255\n"
#define SCRIPT_PUMP_OFF "M42 P205 S0\nM42 P125 S0\n"


void menuPump(void)
{
  MENUITEMS pumpItems = {
    // title
    LABEL_READY,
    // icon               label
    {
      {ICON_PAD_FLUSH_3,  LABEL_FLUSH},
      {ICON_PAD_FLUSH_10, LABEL_FLUSH},
      {ICON_BACKGROUND,   LABEL_BACKGROUND},
      {ICON_PAD_PUMPOFF,  LABEL_PUMP_OFF},
      {ICON_PAD_DOSE20,   LABEL_DOSE},
      {ICON_PAD_DOSE50,   LABEL_DOSE},
      {ICON_BACKGROUND,   LABEL_BACKGROUND},
      {ICON_BACK,         LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&pumpItems);

  while (infoMenu.menu[infoMenu.cur] == menuPump)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        mustStoreScript(SCRIPT_PUMP_ON);
        mustStoreCmd("G4 S3\n");
        mustStoreScript(SCRIPT_PUMP_OFF);
        break;

      case KEY_ICON_1:
        mustStoreScript(SCRIPT_PUMP_ON);
        mustStoreCmd("G4 S10\n");
        mustStoreScript(SCRIPT_PUMP_OFF);
        break;

      case KEY_ICON_3:
        mustStoreScript(SCRIPT_PUMP_OFF);
        break;

      case KEY_ICON_4:
        mustStoreScript(SCRIPT_PUMP_ON);
        mustStoreCmd("G444 P20\n");
        mustStoreScript(SCRIPT_PUMP_OFF);
        break;

      case KEY_ICON_5:
        mustStoreScript(SCRIPT_PUMP_ON);
        mustStoreCmd("G444 P50\n");
        mustStoreScript(SCRIPT_PUMP_OFF);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
