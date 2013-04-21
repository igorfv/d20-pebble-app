#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "itoa.h"

#define MY_UUID { 0x69, 0xA7, 0x50, 0x95, 0x77, 0xDB, 0x4A, 0xB3, 0x92, 0x43, 0xED, 0xB0, 0x4C, 0xD2, 0x90, 0xE1 }
PBL_APP_INFO(MY_UUID, "d20", "Igor Vieira", 1,0, /* App version */ RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_STANDARD_APP);

#define TIME_ZONE_OFFSET -5

Window window;

TextLayer titleText;
TextLayer selectionText;
TextLayer debug;

int dices[] = {4, 6, 8, 10, 12, 20, 100};
int dicesSize = 7;
signed int selectedDice = 0;
signed int multiDices = 1;
int setupPhase = 0;

long randSeed;


/**************************************************
* RANDOM FUNCTION BY http://github.com/rickschrader
***************************************************/

int get_unix_time_from_current_time(PblTm *current_time)
{
  unsigned int unix_time;

  /* Convert time to seconds since epoch. */
      unix_time = ((0-TIME_ZONE_OFFSET)*3600) + /* time zone offset */ 
              + current_time->tm_sec /* start with seconds */
              + current_time->tm_min*60 /* add minutes */
              + current_time->tm_hour*3600 /* add hours */
              + current_time->tm_yday*86400 /* add days */
              + (current_time->tm_year-70)*31536000 /* add years since 1970 */
              + ((current_time->tm_year-69)/4)*86400 /* add a day after leap years, starting in 1973 */
              - ((current_time->tm_year-1)/100)*86400 /* remove a leap day every 100 years, starting in 2001 */
              + ((current_time->tm_year+299)/400)*86400; /* add a leap day back every 400 years, starting in 2001*/

  return unix_time;
}

int get_unix_time()
{
  PblTm current_time;
  get_time(&current_time);

  return get_unix_time_from_current_time(&current_time);
}

int random(int max)
{
  randSeed = (((randSeed * 214013L + 2531011L) >> 16) & 32767);
  return ((randSeed % max) + 1);
}


//Show dice text
void showDiceText() {
  char *newDice = "d100";
  static char diceNum[] = "000";

  itoa(dices[selectedDice], diceNum, 10);

  strcpy(newDice, "d");
  strcat(newDice, diceNum);

  text_layer_set_text(&selectionText, newDice);
}

//New Setup
void newSetup() {
  setupPhase = 0;
  selectedDice = 0;
  multiDices = 1;

  text_layer_set_text(&titleText, "Select your dice:");

  showDiceText();
}

//Show # rolls text
void showRollsText() {
  char *rolls = "000";
  static char numRolls[] = "000";

  itoa(multiDices, numRolls, 10);

  strcpy(rolls, numRolls);

  text_layer_set_text(&selectionText, rolls);
}


//Show # rolls text
void dicesRolled() {
  char *rolls = "000";
  static char numRolls[] = "000";

  itoa(multiDices, numRolls, 10);

  strcpy(rolls, "Total for ");
  strcat(rolls, numRolls);
  strcat(rolls, " dices");

  text_layer_set_text(&titleText, rolls);
}


//Show # rolls text
void runDices() {
  unsigned int total = 0;
  unsigned int partial = 0;

  char *totalText = "000000";
  static char totalBuff[] = "000000";

  for(int cont=0;cont<multiDices;cont++)
  {
    partial = random(dices[selectedDice]);
    total += partial;
  }

  
  itoa(total, totalBuff, 10);
  strcpy(totalText, totalBuff);
  text_layer_set_text(&selectionText, totalText);
}


// Modify these common button handlers
void menu_up_click(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(setupPhase == 0)
  {
    selectedDice++;
    if(selectedDice >= dicesSize){ selectedDice = 0; }

    showDiceText();
  }
  else if(setupPhase == 1)
  {
    multiDices++;
    //if(multiDices >= multiDices){ multiDices = 0; }

    showRollsText();
  }
}


void menu_down_click(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(setupPhase == 0)
  {
    selectedDice--;
    if(selectedDice < 0){ selectedDice = dicesSize-1; }

    showDiceText();
  }
  else if(setupPhase == 1)
  {
    multiDices--;
    if(multiDices < 1){ multiDices = 1; }

    showRollsText();
  }
}

void menu_select_click(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(setupPhase == 0)
  {
    setupPhase++;
    text_layer_set_text(&titleText, "Number of dices:");
    showRollsText();
  }
  else if(setupPhase == 1)
  {
    setupPhase++;
    text_layer_set_text(&titleText, "Result:");
    runDices();
  }
  else if(setupPhase == 2)
  {
    newSetup();
  }
}


void menu_long_select_click(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(setupPhase == 2)
  { 
    vibes_short_pulse();
    runDices();
  }
}


// This usually won't need to be modified

void menu_click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) menu_select_click;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) menu_long_select_click;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) menu_up_click;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) menu_down_click;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}



// Standard app initialisation

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "d20");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) menu_click_config_provider);

  text_layer_init(&titleText, GRect(0,0, 144, 30));
  text_layer_set_text_color(&titleText, GColorBlack);
  text_layer_set_background_color(&titleText, GColorWhite);
  text_layer_set_font(&titleText, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(&titleText, GTextAlignmentCenter);
  text_layer_set_text(&titleText, "Select your dice:");
  layer_add_child(&window.layer, &titleText.layer);

  text_layer_init(&selectionText, GRect(0,46, 144, 168-46));
  text_layer_set_text_color(&selectionText, GColorWhite);
  text_layer_set_background_color(&selectionText, GColorClear);
  text_layer_set_font(&selectionText, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
  text_layer_set_text_alignment(&selectionText, GTextAlignmentCenter);
  text_layer_set_text(&selectionText, "d4");
  layer_add_child(&window.layer, &selectionText.layer);

  newSetup();
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
