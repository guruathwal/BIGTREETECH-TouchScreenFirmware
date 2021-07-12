#include "Printing.h"
#include "includes.h"

const GUI_RECT printinfo_val_rect[6] = {
  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0+PICON_VAL_LG_EX, PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1+PICON_VAL_LG_EX, PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_VAL_SM_EX, PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0+PICON_VAL_LG_EX, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1+PICON_VAL_LG_EX, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_VAL_SM_EX, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y+BYTE_HEIGHT},
};

const GUI_RECT ProgressBar = {START_X + 1,                                PICON_START_Y + PICON_HEIGHT * 2 + PICON_SPACE_Y * 2 + 1,
                              START_X + 4 * ICON_WIDTH + 3 * SPACE_X - 1, ICON_START_Y + ICON_HEIGHT + SPACE_Y - PICON_SPACE_Y - 1};

const  char *const Speed_ID[2] = {"Speed", "Flow"};

#define TOGGLE_TIME  1000  // 1 seconds is 1000
#define LAYER_DELTA  0.1   // minimal layer height change to update the layer display (avoid congestion in vase mode)

#define LAYER_TITLE "Layer"
//#define EXT_ICON_POS 0
//#define BED_ICON_POS 1
//#define FAN_ICON_POS 2
#define TIM_ICON_POS 0 //3
//#define Z_ICON_POS   4
#define SPD_ICON_POS 1 //5
#define FLW_ICON_POS 2
#define PBR_RECT_POS 3

const ITEM itemIsPause[2] = {
  // icon       label
  {ICON_PAUSE,  LABEL_PAUSE},
  {ICON_RESUME, LABEL_RESUME},
};

const ITEM itemPrintFinished[4] = {
  // icon         label
  {ICON_MAINMENU,    LABEL_MAINMENU},
  {ICON_PREVIEW,     LABEL_BACKGROUND},
  {ICON_PAD_RESTART, LABEL_REPRINT},
  {ICON_BACK,        LABEL_BACK},
};

void menuBeforePrinting(void)
{
  // load stat/end/cancel gcodes from spi flash

  switch (infoFile.source)
  {
    case BOARD_SD:  // GCode from file on ONBOARD SD
      {
        uint32_t size;

        size = request_M23_M36(infoFile.title + 5);
        //if (powerFailedCreate(infoFile.title) == false)
        //{
        //
        //}  // FIXME: Powerfail resume is not yet supported for ONBOARD_SD. Need more work.

        if (size == 0)
        {
          ExitDir();
          infoMenu.cur--;
          return;
        }

        //if (powerFailedExist())
        //{
        request_M24(0);
        //}
        //else
        //{
        //request_M24(infoBreakPoint.offset);
        //}

        printStart(NULL, size);
        break;
      }

    case TFT_UDISK:
    case TFT_SD:  // GCode from file on TFT SD
      {
        FIL file;

        if (f_open(&file, infoFile.title, FA_OPEN_EXISTING | FA_READ) != FR_OK)
        {
          ExitDir();
          infoMenu.cur--;
          return;
        }

        printStart(&file, f_size(&file));
        break;
      }

    default:
      ExitDir();
      infoMenu.cur--;
      return;
  }

  infoMenu.menu[infoMenu.cur] = menuPrinting;
}

static inline void reDrawSpeed(int icon_pos)
{
  char tempstr[10];

  ICON_ReadDisplay(rect_of_keyPrint[icon_pos].x0, rect_of_keyPrint[icon_pos].y0, ICON_PRINTING_SPEED);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  sprintf(tempstr, "%d%%", speedGetCurPercent(0));
  GUI_DispString(rect_of_keyPrint[icon_pos].x0 + PICON_TITLE_X, rect_of_keyPrint[icon_pos].y0 + PICON_TITLE_Y,
                 (uint8_t *)Speed_ID[0]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawFlow(int icon_pos)
{
  char tempstr[10];

  ICON_ReadDisplay(rect_of_keyPrint[icon_pos].x0, rect_of_keyPrint[icon_pos].y0, ICON_PRINTING_FLOW);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  sprintf(tempstr, "%d%%", speedGetCurPercent(1));
  GUI_DispString(rect_of_keyPrint[icon_pos].x0 + PICON_TITLE_X, rect_of_keyPrint[icon_pos].y0 + PICON_TITLE_Y,
                 (uint8_t *)Speed_ID[1]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawProgress(int icon_pos, uint8_t prevProgress)
{
  char progress[6];
  char timeElapsed[10];
  uint8_t hour, min, sec;
  uint8_t newProgress = getPrintProgress();

  getPrintTimeDetail(&hour, &min, &sec);
  sprintf(progress, "%d%%", newProgress);
  sprintf(timeElapsed, "%02u:%02u", min, sec); // display only minutes and seconds
  GUI_SetNumMode(GUI_NUMMODE_ZERO);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(rect_of_keyPrint[icon_pos].x0, rect_of_keyPrint[icon_pos].y0, ICON_PRINTING_TIMER);
  GUI_DispString(rect_of_keyPrint[icon_pos].x0 + PICON_TITLE_X, rect_of_keyPrint[icon_pos].y0 + PICON_TITLE_Y, (uint8_t *)progress);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)timeElapsed);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

  if (newProgress != prevProgress)
  {
    uint16_t progStart = ((rect_of_keyPrint[PBR_RECT_POS].x1 - rect_of_keyPrint[PBR_RECT_POS].x0) * prevProgress) / 100;
    uint16_t progEnd = ((rect_of_keyPrint[PBR_RECT_POS].x1 - rect_of_keyPrint[PBR_RECT_POS].x0) * newProgress) / 100;
    GUI_FillRectColor(rect_of_keyPrint[PBR_RECT_POS].x0 + progStart, rect_of_keyPrint[PBR_RECT_POS].y0,
                      rect_of_keyPrint[PBR_RECT_POS].x0 + progEnd, rect_of_keyPrint[PBR_RECT_POS].y1, MAT_ORANGE);
  }
}

static inline void printingDrawPage(void)
{
  updatePrintProgress();
  reDrawProgress(TIM_ICON_POS, 0);
  reDrawSpeed(SPD_ICON_POS);
  reDrawFlow(SPD_ICON_POS);

  GUI_SetColor(DARKGRAY);
  GUI_FillPrect(&rect_of_keyPrint[PBR_RECT_POS]);
  GUI_SetColor(ORANGE);
  GUI_DrawPrect(&rect_of_keyPrint[PBR_RECT_POS]);
  GUI_RestoreColorDefault();
}

void stopConfirm(void)
{
  printAbort();
  infoMenu.cur--;
}

void refreshPrintingIcons(MENUITEMS * printingItems)
{
  if(!isPrinting())
  {
    for (uint8_t i = 0; i < 4; i++)
    {
      uint8_t iPos = i+4;
      if (iPos != 5)
      {
        printingItems->items[iPos] = itemPrintFinished[i];
        menuDrawItem(&printingItems->items[iPos], iPos);
      }
    }
  }
}

void menuPrinting(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS printingItems = {
    // title
    LABEL_BACKGROUND,
    // icon                          label
    {
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_MORE,                    LABEL_MORE},
      {ICON_STOP,                    LABEL_STOP},
    }
  };

  uint8_t oldProgress = 0;
  uint16_t curspeed[2] = {0};
  uint32_t time = 0;
  HEATER nowHeat;
  bool lastPause = isPaused();
  bool lastPrinting = isPrinting();

  memset(&nowHeat, 0, sizeof(HEATER));

  if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
    printingItems.title.address = (uint8_t *) infoFile.Longfile[infoFile.fileIndex];
  else
    printingItems.title.address = getPrintName(infoFile.title);

  printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];

  if (isPrintModelIcon())
    printingItems.items[5].icon = ICON_PREVIEW;

  refreshPrintingIcons(&printingItems);
  menuDrawPage(&printingItems);
  printingDrawPage();

  while (infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
    // check printing progress
    if (getPrintSize() != 0)
    {
      // check print time or progress percentage change
      if ((time != getPrintTime()) || (updatePrintProgress()))
      {
        time = getPrintTime();
        RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
        reDrawProgress(TIM_ICON_POS, oldProgress);
        oldProgress = getPrintProgress();
      }
    }
    else
    {
      if (getPrintProgress() != 100)
      {
        updatePrintProgress();
        reDrawProgress(TIM_ICON_POS, oldProgress);
        oldProgress = getPrintProgress();
      }
    }

    // check change in speed
    if (curspeed[0] != speedGetCurPercent(0))
    {
      curspeed[0] = speedGetCurPercent(0);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawSpeed(SPD_ICON_POS);
    }

    // check change in flow
    if (curspeed[1] != speedGetCurPercent(1))
    {
      curspeed[1] = speedGetCurPercent(1);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawFlow(FLW_ICON_POS);
    }

    // check if print is paused
    if (lastPause != isPaused())
    {
      lastPause = isPaused();
      printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
      menuDrawItem(&printingItems.items[KEY_ICON_4], KEY_ICON_4);
    }

    // change icons after printing finished
    if(lastPrinting != isPrinting())
    {
      lastPrinting = isPrinting();
      refreshPrintingIcons(&printingItems);
    }

    KEY_VALUES key_num = menuKeyGetValue();
    switch (key_num)
    {
      // speed icon
      case KEY_ICON_1:
        setSpeedItemIndex(0);
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;

      // flow icon
      case KEY_ICON_2:
        setSpeedItemIndex(1);
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;

      // pause icon
      case KEY_ICON_4:
        if (isPrinting())
        {
          if (!isHostDialog())
            printPause(!isPaused(), PAUSE_NORMAL);
          else
            addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_BUSY));
        }
        else
        {
          clearInfoPrint();
          clearInfoFile();
          infoMenu.cur = 0;
        }
        break;

      // more icon
      case KEY_ICON_5:
        if (isPrinting())
        {
          clearInfoPrint();
          infoMenu.cur--;
        }
        break;

      // more icon
      case KEY_ICON_6:
        if (isPrinting())
        {
          infoMenu.menu[++infoMenu.cur] = menuMore;
        }
        else
        {
          // reprint current file
          char temp_info[FILE_NUM + 50];
          sprintf(temp_info, (char *)textSelect(LABEL_START_PRINT), (uint8_t *)strrchr( infoFile.title, '/') + 1);
          // confirm file selction
          setDialogText(LABEL_PRINT, (uint8_t *)temp_info, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, activateReprint, NULL, NULL);
        }
        break;

      case KEY_ICON_7:
        if (isPrinting())
        {
          setDialogText(LABEL_WARNING, LABEL_STOP_PRINT, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ALERT, stopConfirm, NULL, NULL);
        }
        else
        {
          clearInfoPrint();
          infoMenu.cur--;
        }
        break;

      default:
        break;
    }

    loopProcess();
  }
}
