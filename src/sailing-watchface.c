#include <pebble.h>
#include <pebble_fonts.h>
static AppSync sync; //some stuff for syncing data
static uint8_t sync_buffer[100];
static Window *window;
static TextLayer *text_layer;
static TextLayer *max_layer;
static  char speedstr[10] = "-5";
static  char maxstr[10] = "0.0";
char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}
/*
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}
*/

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, translate_error(app_message_error));
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
    switch (key)
    {
        case 0:
    
            strcpy(speedstr,new_tuple->value->cstring);
            layer_mark_dirty(text_layer_get_layer( text_layer));
            APP_LOG(APP_LOG_LEVEL_DEBUG,"got speed %s",new_tuple->value->cstring);
            break;
        case 1:
            if (strcmp(maxstr,new_tuple->value->cstring)!=0)
            {
                strcpy(maxstr,new_tuple->value->cstring);
                layer_mark_dirty(text_layer_get_layer( max_layer));
                APP_LOG(APP_LOG_LEVEL_DEBUG,"got new max speed %s", new_tuple->value->cstring);
                vibes_long_pulse();//vibrate on new max speed
            }
            break;
        default:
            APP_LOG(APP_LOG_LEVEL_DEBUG,"unknown key %lu",key);

    }
}
static const int Speed_height = 50;
static const int small_height = 20;
static void window_load(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG,"window_load start");
    Layer *window_layer = window_get_root_layer(window); GRect bounds = layer_get_bounds(window_layer);
    //speed
    GRect pos =  { .origin = { 0, 0 }, .size = { bounds.size.w, Speed_height } };
    text_layer = text_layer_create(pos);
    text_layer_set_font(text_layer,fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    text_layer_set_text(text_layer,speedstr );
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(text_layer));
    //max
    GRect posmax =  { .origin = { 0, bounds.size.h-Speed_height }, .size = { bounds.size.w, Speed_height } };
    max_layer = text_layer_create(posmax);
    text_layer_set_font(max_layer,fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    text_layer_set_text(max_layer,maxstr );
    text_layer_set_text_alignment(max_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(max_layer));
    APP_LOG(APP_LOG_LEVEL_DEBUG,"window load end");
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  app_sync_deinit(&sync);
}

static void init(void) {
    window = window_create();
//  window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    const int inbound_size = 64;
    const int outbound_size = 16;
    Tuplet initial_values[] = {TupletCString(0x0,"-5"),TupletCString(0x1,"0.0")};
    app_sync_init(&sync,sync_buffer,sizeof(sync_buffer),initial_values,2,sync_tuple_changed_callback,sync_error_callback,NULL);
  app_message_open(inbound_size,outbound_size);
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
