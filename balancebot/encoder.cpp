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
#include "SEGGER_RTT.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_tim.h"
}

#include "encoder.h"

Encoder::Encoder(uint8_t encoder_id) {
  encoder_id_ = encoder_id;
  last_counter_ = 0;
}

uint8_t Encoder::encoder_id() const { return encoder_id_; }

q15_t Encoder::counter_delta() {
  q15_t saved_counter = last_counter_;
  if (encoder_id_ == LEFT_ENCODER) {
    last_counter_ = (q15_t)(LL_TIM_GetCounter(TIM2));
  } else {
    last_counter_ = (q15_t)(LL_TIM_GetCounter(TIM3));
  }
  return last_counter_ - saved_counter;
}

void Encoder::init_hardware() {
  // Enable peripheral clocks.
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2 | LL_APB1_GRP1_PERIPH_TIM3);

  // Encoder GPIO pins:
  //    PA0  - TIM2 Ch 1
  //    PA1  - TIM2 Ch 2
  //    PA6  - TIM3 Ch 1
  //    PA7  - TIM3 Ch 2
  LL_GPIO_InitTypeDef gpio_init = {0};
  gpio_init.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
  gpio_init.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOA, &gpio_init);

  LL_TIM_InitTypeDef timer_init = {0};
  timer_init.Prescaler = 0;
  timer_init.CounterMode = LL_TIM_COUNTERMODE_UP;
  timer_init.Autoreload = 65535;
  timer_init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;

  // TIM2
  LL_TIM_SetEncoderMode(TIM2, LL_TIM_ENCODERMODE_X4_TI12);
  LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1,
                           LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2,
                           LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_Init(TIM2, &timer_init);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);
  LL_TIM_EnableCounter(TIM2);

  // TIM3
  LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X4_TI12);
  LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH1,
                           LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH2,
                           LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_Init(TIM3, &timer_init);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);
  LL_TIM_EnableCounter(TIM3);
}