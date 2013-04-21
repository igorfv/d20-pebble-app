
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x69, 0xA7, 0x50, 0x95, 0x77, 0xDB, 0x4A, 0xB3, 0x92, 0x43, 0xED, 0xB0, 0x4C, 0xD2, 0x90, 0xE1 }
PBL_APP_INFO(MY_UUID, "d20", "Igor Vieira", 1,0, /* App version */ RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_STANDARD_APP);


Window window;

TextLayer titleText;
TextLayer selectionText;
TextLayer setupText;

int dices[] = {4, 6, 8, 10, 12, 20, 100};
int selectedDice = 0;
int multiDices = 1;
int setupPhase = 0;


// Modify these common button handlers

void menu_up_click(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
}


void menu_down_click(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
}

void menu_select_click(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
}


void menu_long_select_click(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
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
  text_layer_set_text(&selectionText, "d20");
  layer_add_child(&window.layer, &selectionText.layer);

  text_layer_init(&setupText, GRect(0,168-30-16, 144, 30));
  text_layer_set_text_color(&setupText, GColorWhite);
  text_layer_set_background_color(&setupText, GColorClear);
  text_layer_set_font(&setupText, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(&setupText, GTextAlignmentCenter);
  text_layer_set_text(&setupText, "Last: d100 > 10 times");
  layer_add_child(&window.layer, &setupText.layer);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
