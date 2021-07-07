#include "Printing.h"
#include "includes.h"

const GUI_POINT printinfo_points[6] = {
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
};

const GUI_RECT printinfo_val_rect[6] = {
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},
};

const GUI_RECT ProgressBar = {START_X + 1,                                PICON_START_Y + PICON_HEIGHT * 2 + PICON_SPACE_Y * 2 + 1,
                              START_X + 4 * ICON_WIDTH + 3 * SPACE_X - 1, ICON_START_Y + ICON_HEIGHT + SPACE_Y - PICON_SPACE_Y - 1};

const  char *const Speed_ID[2] = {"Speed", "Flow"};

#define TOGGLE_TIME  2000  // 1 seconds is 1000
#define LAYER_DELTA  0.1   // minimal layer height change to update the layer display (avoid congestion in vase mode)

#define LAYER_TITLE "Layer"
#define EXT_ICON_POS 0
#define BED_ICON_POS 1
#define FAN_ICON_POS 2
#define TIM_ICON_POS 3
#define Z_ICON_POS   4
#define SPD_ICON_POS 5

const ITEM itemIsPause[2] = {
  // icon                        label
  {ICON_PAUSE,                   LABEL_PAUSE},
  {ICON_RESUME,                  LABEL_RESUME},
};

const ITEM itemIsPrinting[3] = {
  // icon                        label
  {ICON_BACKGROUND,              LABEL_BACKGROUND},
  {ICON_MAINMENU,                LABEL_MAIN_SCREEN},
  {ICON_BACK,                    LABEL_BACK},
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

static inline void reValueNozzle(int icon_pos)
{
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_NOZZLE);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (uint8_t *)heatDisplayID[currentTool]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reValueBed(int icon_pos)
{
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_BED);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (uint8_t *)heatDisplayID[BED]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}


static inline void reDrawSpeed(int icon_pos)
{
  char tempstr[10];

  if (currentSpeedID == 0)
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_SPEED);
  else
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_FLOW);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  sprintf(tempstr, "%d%%", speedGetCurPercent(currentSpeedID));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (uint8_t *)Speed_ID[currentSpeedID]);
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
  sprintf(timeElapsed, "%02u:%02u:%02u", hour, min, sec);
  GUI_SetNumMode(GUI_NUMMODE_ZERO);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_TIMER);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (uint8_t *)progress);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)timeElapsed);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

  if (newProgress != prevProgress)
  {
    uint16_t progStart = ((ProgressBar.x1 - ProgressBar.x0) * prevProgress) / 100;
    uint16_t progEnd = ((ProgressBar.x1 - ProgressBar.x0) * newProgress) / 100;
    GUI_FillRectColor(ProgressBar.x0 + progStart, ProgressBar.y0, ProgressBar.x0 + progEnd, ProgressBar.y1, MAT_ORANGE);
  }
}

static inline void reDrawLayer(int icon_pos)
{
  char tempstr[10];

  sprintf(tempstr, "%.2fmm", (infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_ZLAYER);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                  (uint8_t *)LAYER_TITLE);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void toggleInfo(void)
{
  if (nextScreenUpdate(TOGGLE_TIME))
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool + 1) % infoSettings.hotend_count;
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reValueNozzle(EXT_ICON_POS);
    }

    currentSpeedID = (currentSpeedID + 1) % 2;
    RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
    reDrawSpeed(SPD_ICON_POS);
    speedQuery();

    if (infoFile.source >= BOARD_SD)
      coordinateQuery(TOGGLE_TIME / 1000);
  }
}

static inline void printingDrawPage(void)
{
  updatePrintProgress();
  reValueNozzle(EXT_ICON_POS);
  reValueBed(BED_ICON_POS);
  reDrawProgress(TIM_ICON_POS, 0);
  reDrawLayer(Z_ICON_POS);
  reDrawSpeed(SPD_ICON_POS);
  GUI_SetColor(ORANGE);
  GUI_DrawRect(ProgressBar.x0 - 1, ProgressBar.y0 - 1, ProgressBar.x1 + 1, ProgressBar.y1 + 1);
  GUI_SetColor(DARKGRAY);
  GUI_FillPrect(&ProgressBar);
  GUI_RestoreColorDefault();
}

void stopConfirm(void)
{
  printAbort();
  infoMenu.cur--;
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
      {ICON_BACKGROUND,              LABEL_BABYSTEP},
      {ICON_MORE,                    LABEL_MORE},
      {ICON_STOP,                    LABEL_STOP},
    }
  };

  uint8_t oldProgress = 0;
  uint16_t curspeed[2] = {0};
  uint32_t time = 0;
  HEATER nowHeat;
  float curLayer = 0;
  float usedLayer = 0;
  float prevLayer = 0;
  bool layerDrawEnabled = false;
  bool lastPause = isPaused();

  memset(&nowHeat, 0, sizeof(HEATER));

  if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
    printingItems.title.address = (uint8_t *) infoFile.Longfile[infoFile.fileIndex];
  else
    printingItems.title.address = getPrintName(infoFile.title);

  printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
  printingItems.items[KEY_ICON_5].icon = (infoFile.source < BOARD_SD && isPrintModelIcon()) ? ICON_PREVIEW : ICON_BABYSTEP;

  menuDrawPage(&printingItems);
  printingDrawPage();

  while (infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
    //Scroll_DispString(&titleScroll, LEFT);  // Scroll display file name will take too many CPU cycles

    // check nozzle temp change
    if (nowHeat.T[currentTool].current != heatGetCurrentTemp(currentTool) ||
        nowHeat.T[currentTool].target != heatGetTargetTemp(currentTool))
    {
      nowHeat.T[currentTool].current = heatGetCurrentTemp(currentTool);
      nowHeat.T[currentTool].target = heatGetTargetTemp(currentTool);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reValueNozzle(EXT_ICON_POS);
    }

    // check bed temp change
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reValueBed(BED_ICON_POS);
    }

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

    // Z_AXIS coordinate
    curLayer = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS));
    if (prevLayer != curLayer)
    {
      if (ABS(curLayer - usedLayer) >= LAYER_DELTA)
      {
        layerDrawEnabled = true;
      }
      if (layerDrawEnabled == true)
      {
        usedLayer = curLayer;
        RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
        reDrawLayer(Z_ICON_POS);
      }
      if (ABS(curLayer - prevLayer) < LAYER_DELTA)
      {
        layerDrawEnabled = false;
      }
      prevLayer = curLayer;
    }

    // check change in speed or flow
    if (curspeed[currentSpeedID] != speedGetCurPercent(currentSpeedID))
    {
      curspeed[currentSpeedID] = speedGetCurPercent(currentSpeedID);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawSpeed(SPD_ICON_POS);
    }

    // check if print is paused
    if (lastPause != isPaused())
    {
      lastPause = isPaused();
      printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
      menuDrawItem(&printingItems.items[KEY_ICON_4], KEY_ICON_4);
    }

    toggleInfo();

    KEY_VALUES key_num = menuKeyGetValue();
    switch (key_num)
    {
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

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuMore;
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
