// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_BPM 75
#define INITIAL_SPO2 99
#define INITIAL_LOW_BPM 55
#define INITIAL_UP_BPM 100
#define INITIAL_LOW_SPO2 94
#define BPM_ATTR_MAX_VALUE 300
#define SPO2_ATTR_MAX_VALUE 100
#define VOLTAGE 5.0

typedef struct {
  pin_t pin_vcc;
  pin_t pin_gnd;
  pin_t pin_scl;
  pin_t pin_sda;
  pin_t pin_int;
  pin_t pin_ird;
  uint32_t pulserate_attr;
  uint32_t oxygen_attr;
  uint32_t low_pulserate_treshold_attr;
  uint32_t up_pulserate_treshold_attr;
  uint32_t low_oxygen_treshold_attr;
} chip_state_t;

static void chip_timer_event(void *user_data);

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  chip->pin_vcc = pin_init("VCC", INPUT_PULLDOWN);  //5V
  chip->pin_gnd = pin_init("GND", INPUT_PULLUP);    //GROUND
  chip->pin_scl = pin_init("SCL", ANALOG);          //BPM
  chip->pin_sda = pin_init("SDA", ANALOG);          //SpO2
  chip->pin_int = pin_init("INT", OUTPUT);    //BPM Treshold
  chip->pin_ird = pin_init("IRD", OUTPUT);    //SpO2 Treshold

  chip->pulserate_attr = attr_init("pulseRate", INITIAL_BPM);
  chip->oxygen_attr = attr_init("oxygen", INITIAL_SPO2);
  chip->low_pulserate_treshold_attr = attr_init("lowerPulseRateTreshold", INITIAL_LOW_BPM);
  chip->up_pulserate_treshold_attr = attr_init("upperPulseRateTreshold", INITIAL_UP_BPM);
  chip->low_oxygen_treshold_attr = attr_init("lowerOxygenTreshold", INITIAL_LOW_SPO2);

  const timer_config_t timer_config = {
    .callback = chip_timer_event,
    .user_data = chip
  };

  timer_t timer_id = timer_init(&timer_config);

  timer_start(timer_id, 1000, true);

}

void chip_timer_event(void *user_data){
  chip_state_t *chip = (chip_state_t*)user_data;
  if(pin_read(chip->pin_vcc) && !pin_read(chip->pin_gnd)){
    float sclVoltage = attr_read(chip->pulserate_attr) * VOLTAGE / BPM_ATTR_MAX_VALUE;
    float sdaVoltage = attr_read(chip->oxygen_attr) * VOLTAGE / SPO2_ATTR_MAX_VALUE;
    pin_dac_write(chip->pin_scl, sclVoltage);
    pin_dac_write(chip->pin_sda, sdaVoltage);

    //printf("SCL Voltage: %f, SDA Voltage: %f\n", sclVoltage, sdaVoltage);

    float low_pulserate_treshold_v = attr_read(chip->low_pulserate_treshold_attr) * VOLTAGE / BPM_ATTR_MAX_VALUE;
    float up_pulserate_treshold_v = attr_read(chip->up_pulserate_treshold_attr) * VOLTAGE / BPM_ATTR_MAX_VALUE;     

    //printf("Pulse thresholds: low %f, up %f\n", low_pulserate_treshold_v, up_pulserate_treshold_v);

    if(sclVoltage <= low_pulserate_treshold_v || sclVoltage >= up_pulserate_treshold_v){
      pin_write(chip->pin_int, HIGH);
    }
    else{
      pin_write(chip->pin_int, LOW);
    } 

    float low_oxygen_treshold_v = attr_read(chip->low_oxygen_treshold_attr) * VOLTAGE / SPO2_ATTR_MAX_VALUE;

    //printf("Oxygen threshold: %f, SDA Voltage: %f\n", low_oxygen_treshold_v, sdaVoltage);

    if(sdaVoltage >= low_oxygen_treshold_v){
      pin_write(chip->pin_ird, HIGH);
    }
    else{
      pin_write(chip->pin_ird, LOW);
    } 
  }
}
