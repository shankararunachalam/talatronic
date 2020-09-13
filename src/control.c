// -----------------------------------------------------------------------------
// controller - the glue between the engine and the hardware
//
// reacts to events (grid press, clock etc) and translates them into appropriate
// engine actions. reacts to engine updates and translates them into user 
// interface and hardware updates (grid LEDs, CV outputs etc)
//
// should talk to hardware via what's defined in interface.h only
// should talk to the engine via what's defined in engine.h only
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <inttypes.h>
#include "compiler.h"
#include "string.h"

#include "control.h"
#include "interface.h"
#include "engine.h"
#include "taalas.c"
#include "jathis.c"
#include "konnakkol_sample.c"

const u16 TAALA_CHOICE_COUNT = 7;
const u16 JATHI_CHOICE_COUNT = 5;

preset_meta_t meta;
preset_data_t preset;
shared_data_t shared;
int selected_preset;

//general params
int selected_taala;
int selected_jathi;
int began_playing;

enum state_opts {
    INIT,
    TAALA_SELECT_SCREEN,
    JATHI_SELECT_SCREEN,
    PLAY_SCREEN
};
enum state_opts state;

// ----------------------------------------------------------------------------
// firmware dependent stuff starts here

// ----------------------------------------------------------------------------
// prototypes

static void initial_display(void);
static void display(char *text, u8 line);
static void highlight(char *text, u8 line);
static void display_text(char *text, char *text_value, u8 line);
static void display_value(char *text, u16 value, u8 line);
static void display_line(u8 hasValue, char *text, u16 value, u8 line, u8 isHighlight);

static void display_taala_choices(u16 selection);
static void display_jathi_choices(u16 selection);
static void display_play_screen(void);
static u16 calculate_knob_value(u16 divisions);
static void set_state(int state_value);

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// helper functions

void initial_display(void) {
    selected_taala = calculate_knob_value(TAALA_CHOICE_COUNT);
    display_taala_choices(selected_taala);
    set_state(TAALA_SELECT_SCREEN);
}

void display(char *text, u8 line) {
    display_line(0, text, 0, line, 0);
}

void highlight(char *text, u8 line) {
    display_line(0, text, 0, line, 1);
}

void display_text(char *text, char *text_value, u8 line) {
    u16 length = strlen(text) + strlen(text_value);
    char *buffer = malloc( sizeof(char) * ( length + 1 ) );
    strcpy(buffer, text);
    strcat(buffer, text_value);
    display_line(0, buffer, 0, line, 0);
}

void display_value(char *text, u16 value, u8 line) {
    display_line(1, text, value, line, 0);
}

void display_line(u8 has_value, char *text, u16 value, u8 line, u8 is_highlight) {
    u16 length = strlen(text) + 1;
    if(has_value == 1) {
        length = length + 1 + 10; //length of text plus space plus length of u16 value
    }
    u8 foreground = 255;
    u8 background = 0;
    char *buffer = malloc( sizeof(char) * ( length + 1 ) );
    if(has_value == 1) {
        snprintf(buffer, length, "%s %hu", text, value);
    } else {
        snprintf(buffer, length, "%s", text);
    }
    if(is_highlight == 1) {
        background = 70;
    }
    draw_str(buffer, line, foreground, background);
    refresh_screen();
}

void display_taala_choices(u16 selection) {
    clear_screen();
    for(int i = 0; i < 6; i++) {
        if(selection + i > 6) {
            display("", i);
        } else {
            if(i == 0) {
                highlight((char *)taala_type[selection + i].name, i);
            } else {
                display((char *)taala_type[selection + i].name, i);
            }
        }
    }
    display("  Select", 7);
}

void display_jathi_choices(u16 selection) {
    clear_screen();
    for(int i = 0; i < 6; i++) {
        if(selection + i > 4) {
            display("", i);
        } else {
            if(i == 0) {
                highlight((char *)jathi_type[selection + i].name, i);
            } else {
                display((char *)jathi_type[selection + i].name, i);
            }
        }
    }
    display("  Select", 7);
}

static void display_play_screen(void) {
    clear_screen();
    display("Playing file: ", 0);
    display("   Back", 7);
}

u16 calculate_knob_value(u16 divisions) {
    u16 knob_value = get_knob_value(0);
    knob_value = (knob_value / 65536.0) * divisions; //to get a value between 1 and divisions. 66536 is the max.
    return knob_value;
}

void set_state(int state_value) {
    switch(state_value) {
        case TAALA_SELECT_SCREEN:
            //timed event for knob adjustment
            add_timed_event(0, 50, 1);
            selected_taala = -1;
            selected_jathi = -1;
            break;

        case JATHI_SELECT_SCREEN:
            selected_jathi = -1;
            break;
            
        case PLAY_SCREEN:
            began_playing = -1;
            stop_timed_event(0);

            //initialize taala playing
            //sarali_length = sizeof(sarali) / sizeof(sarali[0]);
            //current_sarali_row = 0;
            //current_direction = AAROHANA;
            //current_sthana_position = 0;
            break;
        
        default:
            break;
    }
    state = state_value;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// functions for main.c

void init_presets(void) {
    // called by main.c if there are no presets saved to flash yet
    // initialize meta - some meta data to be associated with a preset, like a glyph
    // initialize shared (any data that should be shared by all presets) with default values
    // initialize preset with default values 
    // store them to flash
    
    for (u8 i = 0; i < get_preset_count(); i++) {
        store_preset_to_flash(i, &meta, &preset);
    }

    store_shared_data_to_flash(&shared);
    store_preset_index(0);
}

void init_control(void) {
    // load shared data
    // load current preset and its meta data
    
    load_shared_data_from_flash(&shared);
    selected_preset = get_preset_index();
    load_preset_from_flash(selected_preset, &preset);
    load_preset_meta_from_flash(selected_preset, &meta);

    // set up any other initial values and timers
    set_debug(1);
    initial_display();
}

void process_event(u8 event, u8 *data, u8 length) {
    switch (event) {
        case MAIN_CLOCK_RECEIVED:
            break;
        
        case MAIN_CLOCK_SWITCHED:
            break;
    
        case GATE_RECEIVED:
            break;
        
        case GRID_CONNECTED:
            break;
        
        case GRID_KEY_PRESSED:
            break;
    
        case GRID_KEY_HELD:
            break;
            
        case ARC_ENCODER_COARSE:
            break;
    
        case FRONT_BUTTON_PRESSED:
        {
            if(data[0] > 0) {
                switch(state) {
                    case TAALA_SELECT_SCREEN:
                    {
                        if(selected_taala != -1) {
                            selected_jathi = calculate_knob_value(JATHI_CHOICE_COUNT);
                            display_jathi_choices(selected_jathi);
                            set_state(JATHI_SELECT_SCREEN);
                        }
                        break;
                    }
                    case JATHI_SELECT_SCREEN:
                    {
                        display_play_screen();
                        set_state(PLAY_SCREEN);
                        break;
                    }
                    case PLAY_SCREEN:
                    {
                        if(began_playing != -1) {
                            initial_display();
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            break;
        }
    
        case FRONT_BUTTON_HELD:
            break;
    
        case BUTTON_PRESSED:
            break;
    
        case I2C_RECEIVED:
            break;
            
        case TIMED_EVENT:
        {
            if (data[0] == 0) { //knob adjustments
                switch(state) {
                    case TAALA_SELECT_SCREEN:
                    {
                        u16 knob_value = calculate_knob_value(TAALA_CHOICE_COUNT);
                        if(selected_taala != knob_value) {
                            selected_taala = knob_value;
                            display_taala_choices(selected_taala);
                        }
                        break;
                    }
                    case JATHI_SELECT_SCREEN:
                    {
                        u16 knob_value = calculate_knob_value(JATHI_CHOICE_COUNT);
                        if(selected_jathi != knob_value) {
                            selected_jathi = knob_value;
                            display_jathi_choices(selected_jathi);
                        }
                        break;
                    }
                    case PLAY_SCREEN:
                    {
                        break;
                    }
                    default:
                        break;
                }
            }
            break;
        }
        
        case MIDI_CONNECTED:
            break;
        
        case MIDI_NOTE:
            break;
        
        case MIDI_CC:
            break;
            
        case MIDI_AFTERTOUCH:
            break;
            
        case SHNTH_BAR:
            break;
            
        case SHNTH_ANTENNA:
            break;
            
        case SHNTH_BUTTON:
            break;
            
        default:
            break;
    }
}

void render_grid(void) {
    // render grid LEDs here or leave blank if not used
}

void render_arc(void) {
    // render arc LEDs here or leave blank if not used
}

