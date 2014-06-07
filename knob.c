#include "knob.h"
#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>

typedef libusb_device_handle KnobHandle;

libusb_context *knob_usb_context;
libusb_device  **knob_context_list;
libusb_device  *knob_device;
libusb_device_handle *knob_handle;
int knob_interface_error;

int is_knob(libusb_device *dev);

int get_knob_from_list(void);
int knob_handle_from_dev(void);

int get_knob(void)
{
  if(get_knob_from_list())
  {
    return knob_handle_from_dev();
  }
  else
  {
    printf("No interesting devices found\n");
    return 0;
  }
}

ssize_t knob_get_list(void);

int get_knob_from_list(void)
{
  libusb_device *temp_dev = NULL;
  ssize_t cnt, i;

  cnt = knob_get_list();

  for(i = 0; i < cnt; i++)
  {
    temp_dev = knob_context_list[i];
    if(is_knob(temp_dev))
    {
        knob_device = temp_dev;
        printf("Device found!\n");
        return 1;
    }
  }

  return 0;
}

ssize_t knob_get_list(void)
{
  int error_code = libusb_init(&knob_usb_context);
  ssize_t cnt = 0;

  if(error_code != 0)
  {
    printf("Failed to initialize: %s\n", libusb_strerror(error_code));
    return cnt;
  }

  printf("Context initialized\n");

  libusb_set_debug(knob_usb_context, LIBUSB_LOG_LEVEL_INFO);
  
  printf("Set debug log level to INFO\n");
  
  cnt = libusb_get_device_list(knob_usb_context, &knob_context_list);
  
  if(cnt < 0)
  {
    printf("Found no devices...?\n");
  }

  return cnt;
}

int is_knob(libusb_device *dev)
{
    struct libusb_device_descriptor desc;

    libusb_get_device_descriptor(dev, &desc);

    return desc.idVendor  == KNOB_VID &&
           desc.idProduct == KNOB_PID;
}


int knob_handle_from_dev(void)
{
  int error_code = libusb_open(knob_device, &knob_handle);
  if(error_code)
  {
    printf("Error opening handle: %s\n", libusb_strerror(error_code));
  }
  else
  {
    printf("Device opened!\n");
    knob_interface_error = libusb_claim_interface(knob_handle, 0);
    if(knob_interface_error == LIBUSB_SUCCESS)
    {
      printf("Claimed Interface 0\n");
      return 1;
    }
  }

  return 0;
}

unsigned char knob_endpoint_direction(struct libusb_endpoint_descriptor endpoint)
{
  return (endpoint.bEndpointAddress & LIBUSB_ENDPOINT_IN) >> 7;
}

void show_knob_endpoint(struct libusb_endpoint_descriptor endpoint)
{
  printf("Endpoint Description:\n");
  printf("\tAddress:\t%x\n", endpoint.bEndpointAddress);
  printf("\tDirection:\t%d\n", knob_endpoint_direction(endpoint));
  printf("\n");
}

void show_knob_endpoints(void)
{
  int i;
  struct libusb_config_descriptor *config;
  libusb_get_active_config_descriptor(knob_device, &config);

  for(i = 0; i < 2; i++)
  {
    show_knob_endpoint(config->interface[0].altsetting[0].endpoint[i]);
  }
}


void on_knob_event(struct libusb_transfer *transfer);
void on_output_done(struct libusb_transfer *transfer);

void drive_knob(void)
{
  KnobState *state = (KnobState *)malloc(sizeof(KnobState));
  unsigned char *out_buffer = (unsigned char *)malloc(2);
  out_buffer[0] = 120;
  out_buffer[1] = '\0';

  struct libusb_transfer *input_transfer = libusb_alloc_transfer(0);
  struct libusb_transfer *output_transfer = libusb_alloc_transfer(0);

  libusb_fill_interrupt_transfer(input_transfer, knob_handle, KNOB_IN_ENDPOINT,
    (unsigned char *)state, KNOB_IN_SIZE, on_knob_event, NULL, 100);

  libusb_fill_interrupt_transfer(output_transfer, knob_handle, KNOB_OUT_ENDPOINT,
    out_buffer, 1, on_output_done, NULL, 100);

  libusb_submit_transfer(input_transfer);
  libusb_submit_transfer(output_transfer);
}

void report_state(KnobState *state);

void on_knob_event(struct libusb_transfer *transfer)
{
  KnobState *state = (KnobState *)transfer->buffer;
  report_state(state);
}

void on_output_done(struct libusb_transfer *transfer)
{
  printf("Command completed\n");
}

void report_state(KnobState *state)
{
  KnobInTransferData data = state->data;
  LEDStatus led_status = data.led_status;

  printf("Displacement:\t\t\t%d\n", data.knob_displacement);
  printf("Button state:\t\t\t%d\n", data.button_state);
  printf("LED Brightness:\t\t\t%d\n", data.led_brightness);
  printf("LED Multiplier:\t\t\t%d\n", data.led_multiplier);
  printf("LED Status Byte:\t\t\t%x\n", led_status.status);
  printf("\tPulse Enabled:\t\t\t%d\n", led_status.bits.pulse_enabled);
  printf("\tPulse While Asleep:\t%d\n", led_status.bits.pulse_while_asleep);
  printf("\tPulse Speed:\t\t%d\n", led_status.bits.pulse_speed);
  printf("\tPulse Style:\t\t%d\n", led_status.bits.pulse_style);

  printf("\n");
}

void close_knob(void)
{
  libusb_release_interface(knob_handle, 0);
  printf("Released interface\n");
  libusb_close(knob_handle);
  printf("Device closed\n");  

  libusb_free_device_list(knob_context_list, 1);
  printf("Device list closed\n");

  libusb_exit(knob_usb_context);
  
  printf("Deinitialized\n");
}
