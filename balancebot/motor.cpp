// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

extern "C" {
#include <stm32f1xx_ll_tim.h>
#include "SEGGER_RTT.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
}

#include "motor.h"

Motor::Motor(uint8_t motor_id) {
  motor_id_ = motor_id;
  power_ = 0;
}

uint8_t Motor::motor_id() const { return motor_id_; }

void Motor::set_power(q15_t power) {
  bool sign_is_same = !((power ^ power_) & 0x8000);
  if (!sign_is_same) {
    // TODO: make pins configurable.
    if (motor_id_ == LEFT_MOTOR) {
      LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4 | LL_GPIO_PIN_5);
    } else {
      LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2 | LL_GPIO_PIN_3);
    }
  }
  // Compute absolute value, and saturating 0x8000 to 0x7ffff.
  // CAUTION: absolute power corrupts absolutely. :-)
  q15_t abs_power = (power > 0) ? power : (q15_t)__QSUB16(0, power);

  // Convert from 15-bit fractional value to 10-bit duty cycle value.
  int16_t pwm_value = abs_power >> 5;
  if (motor_id_ == LEFT_MOTOR) {
    LL_TIM_OC_SetCompareCH1(TIM4, pwm_value);
    if (power > 0) {
      LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
      LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
    } else {
      LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
      LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
    }
  } else {
    LL_TIM_OC_SetCompareCH2(TIM4, pwm_value);
    if (power > 0) {
      LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_2);
      LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_3);
    } else {
      LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2);
      LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_3);
    }
  }

  power_ = power;
}

q15_t Motor::power() const { return power_; }

void Motor::init_hardware() {
  // Enable peripheral clocks.
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA |
                           LL_APB2_GRP1_PERIPH_GPIOB);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);

  // Initialize timer.
  //
  // Frequency = ClockFreq / ((PSC + 1) * (ARR + 1))
  // 2008 Hz = 72 MHz / ((34+1)*(1023+1))
  LL_TIM_InitTypeDef timer_init = {0};
  timer_init.Prescaler = 34;
  timer_init.CounterMode = LL_TIM_COUNTERMODE_UP;
  timer_init.Autoreload = 1023;
  timer_init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM4, &timer_init);
  LL_TIM_DisableARRPreload(TIM4);

  // Initialize timer output compare for channels 1 and 2.
  LL_TIM_OC_InitTypeDef timer_oc_init = {0};
  timer_oc_init.OCMode = LL_TIM_OCMODE_PWM1;
  timer_oc_init.CompareValue = 0;
  timer_oc_init.OCPolarity = LL_TIM_OCPOLARITY_HIGH;

  LL_TIM_OC_EnablePreload(TIM4, LL_TIM_CHANNEL_CH1);
  timer_oc_init.OCState = LL_TIM_OCSTATE_DISABLE;
  timer_oc_init.OCNState = LL_TIM_OCSTATE_DISABLE;
  LL_TIM_OC_Init(TIM4, LL_TIM_CHANNEL_CH1, &timer_oc_init);
  LL_TIM_OC_DisableFast(TIM4, LL_TIM_CHANNEL_CH1);

  LL_TIM_OC_EnablePreload(TIM4, LL_TIM_CHANNEL_CH2);
  timer_oc_init.OCState = LL_TIM_OCSTATE_DISABLE;
  timer_oc_init.OCNState = LL_TIM_OCSTATE_DISABLE;
  LL_TIM_OC_Init(TIM4, LL_TIM_CHANNEL_CH2, &timer_oc_init);
  LL_TIM_OC_DisableFast(TIM4, LL_TIM_CHANNEL_CH2);

  LL_TIM_SetTriggerOutput(TIM4, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM4);

  // Direction control GPIO pins:
  //    PA4  - Motor 1 Forward
  //    PA5  - Motor 1 Reverse
  //    PA2  - Motor 2 Forward
  //    PA3  - Motor 2 Reverse
  LL_GPIO_ResetOutputPin(
      GPIOA, LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5);
  LL_GPIO_InitTypeDef gpio_init = {0};
  gpio_init.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5;
  gpio_init.Mode = LL_GPIO_MODE_OUTPUT;
  gpio_init.Speed = LL_GPIO_SPEED_FREQ_LOW;
  gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &gpio_init);

  // PWM GPIO pins:
  //    PB6 - Motor 1 PWM
  //    PB7 - Motor 2 PWM
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6 | LL_GPIO_PIN_7);
  gpio_init = {0};
  gpio_init.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
  gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
  gpio_init.Speed = LL_GPIO_SPEED_FREQ_LOW;
  gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOB, &gpio_init);

  // Enable the PWM channels, enable the counter, and force an update event.
  LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);
  LL_TIM_EnableCounter(TIM4);
  LL_TIM_GenerateEvent_UPDATE(TIM4);
}
