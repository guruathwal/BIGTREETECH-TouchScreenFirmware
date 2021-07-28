#include "ScreenSaver.h"
#include "includes.h"

uint32_t screensaverTimer;
bool screensaverBlocked = false;


// Block/unblock screensaver from running
void blockScreensaver(bool state)
{
  screensaverBlocked = state;
}

void loopScreensaver(void)
{
  if (infoSettings.screensaver == 0 || screensaverBlocked ||
       isPrinting() || infoHost.printing == true ||
      infoMenu.menu[infoMenu.cur] != menuScreenSaver)
  {
    screensaverTimer = OS_GetTimeMs();
    return;
  }

  if (isPress()
    #if LCD_ENCODER_SUPPORT
      || encoder_CheckState() || encoder_ReadBtn(LCD_BUTTON_INTERVALS)
    #endif
  )
  {
    screensaverTimer = OS_GetTimeMs();
  }
  else
  {
    if (OS_GetTimeMs() - screensaverTimer < (infoSettings.screensaver * 1000))
      return;

    if (infoMenu.menu[infoMenu.cur] != menuScreenSaver)
      infoMenu.menu[++infoMenu.cur] = menuScreenSaver;
  }
}

void menuScreenSaver(void)
{
  GUI_RECT touchArea = (GUI_RECT) {0, 0, LCD_WIDTH, LCD_HEIGHT};
  KEY_VALUES key_num = KEY_IDLE;

  //if (isPrinting() || infoHost.printing == true)
  //  infoMenu.cur--;

  setMenu(MENU_TYPE_FULLSCREEN, NULL, 1, &touchArea, NULL, NULL);
  IMAGE_ReadDisplay(0, 0, SCREENSAVER_ADDR);

  while (infoMenu.menu[infoMenu.cur] == menuScreenSaver)
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
        BUZZER_PLAY(sound_keypress);
        infoMenu.cur--;
        break;
      default:
        break;
    }

    loopBackEnd();
  }
}
