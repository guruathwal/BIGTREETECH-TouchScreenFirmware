#include "More.h"
#include "includes.h"

const MENUITEMS moreItems = {
  // title
  LABEL_MORE,
  // icon                   label
  {
    {ICON_PERCENTAGE,       LABEL_PERCENTAGE},
    {ICON_FEATURE_SETTINGS, LABEL_FEATURE_SETTINGS},
    {ICON_MACHINE_SETTINGS, LABEL_MACHINE_SETTINGS},
    {ICON_GCODE,            LABEL_TERMINAL},
    {ICON_BACKGROUND,       LABEL_BACKGROUND},
    {ICON_BACKGROUND,       LABEL_BACKGROUND},
    {ICON_BACKGROUND,       LABEL_BACKGROUND},
    {ICON_BACK,             LABEL_BACK},
  }
};

void menuMore(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&moreItems);

  while (infoMenu.menu[infoMenu.cur] == menuMore)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuFeatureSettings;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuMachineSettings;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuTerminal;
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
