#include <pebble.h>
#include <pebble_fonts.h>
#include "error_handle.h"
static AppSync sync; //some stuff for syncing data
static uint8_t sync_buffer[100];
static Window *window; //base window
static TextLayer *text_layer; //shows current speed
static TextLayer *max_layer;  //shows  max speed
static TextLayer *heading_layer;  //shows  heading
static ActionBarLayer * Alayer;
static char speedstr[10] = "-5"; //backing stores
static char maxstr[10] = "0.0";
static char headstr[10]=  "000"; 


//get data from phone to update display
//occasionally this can be called with the data in new_tuple==old_tuple if the pebble does an internal reshuffle
static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
    switch (key)
    {
        case 0:
    
            strcpy(speedstr,new_tuple->value->cstring);
            layer_mark_dirty(text_layer_get_layer( text_layer));
            APP_LOG(APP_LOG_LEVEL_DEBUG,"got speed %s",new_tuple->value->cstring);
            break;
        case 1:
            if (strcmp(maxstr,new_tuple->value->cstring)!=0)//only update if acctually a new maxspeed
            {
                strcpy(maxstr,new_tuple->value->cstring);
                layer_mark_dirty(text_layer_get_layer( max_layer));
                APP_LOG(APP_LOG_LEVEL_DEBUG,"got new max speed %s", new_tuple->value->cstring);
                vibes_long_pulse();//vibrate on new max speed
            }
            break;
        case 2:
            strcpy(headstr,new_tuple->value->cstring);
            layer_mark_dirty(text_layer_get_layer( heading_layer));
            APP_LOG(APP_LOG_LEVEL_DEBUG,"got heading%s",new_tuple->value->cstring);
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
    //heading
    pos =  (GRect){ .origin = { 0, Speed_height }, .size = { bounds.size.w, Speed_height } };
    heading_layer = text_layer_create(pos);
    text_layer_set_font(heading_layer,fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    text_layer_set_text(heading_layer,headstr );
    text_layer_set_text_alignment(heading_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(heading_layer));
    APP_LOG(APP_LOG_LEVEL_DEBUG,"window load end");
    //max
    pos = (GRect){ .origin = { 0, bounds.size.h-Speed_height }, .size = { bounds.size.w, Speed_height } };
    max_layer = text_layer_create(pos);
    text_layer_set_font(max_layer,fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    text_layer_set_text(max_layer,maxstr );
    text_layer_set_text_alignment(max_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(max_layer));
    //action bar (bar down RHS)
    Alayer = action_bar_layer_create();
    GBitmap* Pmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_P);
    GBitmap* Smap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_S);
    action_bar_layer_set_icon(Alayer,BUTTON_ID_DOWN,Pmap);
    action_bar_layer_set_icon(Alayer,BUTTON_ID_UP,Smap);
    action_bar_layer_add_to_window(Alayer,window);
    APP_LOG(APP_LOG_LEVEL_DEBUG,"window load end");
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  text_layer_destroy(max_layer);
  text_layer_destroy(heading_layer);
  action_bar_layer_destroy(Alayer);
  app_sync_deinit(&sync);
}

static void init(void) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    const int inbound_size = 64;
    const int outbound_size = 16;
    Tuplet initial_values[3] = {TupletCString(0x0,"-5"),TupletCString(0x1,"0.0"),TupletCString(0x2,"000")};
    app_sync_init(&sync,sync_buffer,sizeof(sync_buffer),initial_values,3,sync_tuple_changed_callback,sync_error_callback,NULL);
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
