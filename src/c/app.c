#include <pebble.h>

static Window* s_window;
static TextLayer* s_text_layer;
static TextLayer* s_result_layer;
static DictationSession* s_dictation_session;

static void prv_handle_dictation(DictationSession* session,
                                 DictationSessionStatus status, char* text,
                                 void* context) {
  printf("Dictation got: %s", text);
  text_layer_set_text(s_result_layer, text);
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void* context) {
  printf("Starting dictation with no confirmation");
  dictation_session_enable_confirmation(s_dictation_session, false);
  dictation_session_start(s_dictation_session);
}

static void prv_select_click_handler(ClickRecognizerRef recognizer,
                                     void* context) {
  printf("Starting dictation with confirmation");
  dictation_session_enable_confirmation(s_dictation_session, true);
  dictation_session_start(s_dictation_session);
}

static void prv_down_click_handler(ClickRecognizerRef recognizer,
                                   void* context) {
  printf("Down doesn't do anything.");
}

static void prv_click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer =
      text_layer_create(GRect(10, 10, bounds.size.w - 20, bounds.size.h - 20));
  text_layer_set_text(
      s_text_layer,
      "Up to dictate with confirm, select to dictate without confirm");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(s_text_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  s_result_layer =
      text_layer_create(GRect(10, bounds.size.h - 30, bounds.size.w - 20, 30));
  text_layer_set_text(s_result_layer, "(result shows here)");
  text_layer_set_text_alignment(s_result_layer, GTextAlignmentLeft);
  text_layer_set_overflow_mode(s_result_layer,
                               GTextOverflowModeTrailingEllipsis);
  text_layer_set_font(s_result_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_result_layer));
}

static void prv_window_unload(Window* window) {
  text_layer_destroy(s_text_layer);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = prv_window_load,
                                           .unload = prv_window_unload,
                                       });
  window_stack_push(s_window, true);

  s_dictation_session = dictation_session_create(0, prv_handle_dictation, NULL);
}

static void prv_deinit(void) {
  light_enable(false);
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
