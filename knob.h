#ifndef KNOB_H
#define KNOB_H
#include <stdint.h>

#define KNOB_VID 0x77d
#define KNOB_PID 0x410
#define KNOB_IN_ENDPOINT 0x81
#define KNOB_OUT_ENDPOINT 2
#define KNOB_IN_SIZE 6

typedef union
{
  uint8_t status;
  struct {
    unsigned pulse_enabled      : 1;
    unsigned                    : 1;
    unsigned pulse_while_asleep : 1;
    unsigned                    : 1;
    unsigned pulse_speed        : 2;
    unsigned pulse_style        : 2; 
  } bits;
} LEDStatus;

typedef struct KnobInTransferData
{
  uint8_t   button_state;
  int8_t    knob_displacement;
  uint8_t   _unused;
  uint8_t   led_brightness;
  LEDStatus led_status;
  uint8_t   led_multiplier;
} KnobInTransferData;

typedef struct KnobState
{
  KnobInTransferData data;
  char *    pulse_speed_name;
  char *    pulse_style_name;
} KnobState;

int get_knob(void);
void show_knob_endpoints(void);
void drive_knob(void);
void close_knob(void);

int knob_is_waiting(void);
void knob_handle_events(void);

#endif
