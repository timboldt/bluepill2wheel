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

#ifndef BLUEPILL2WHEEL_WHEEL_H
#define BLUEPILL2WHEEL_WHEEL_H

extern "C" {
#include <arm_math.h>
}

#include <cstdint>

#include "encoder.h"
#include "motor.h"

class Wheel {
 public:
  enum WheelId { LEFT_WHEEL, RIGHT_WHEEL };

  // Wheel ID determines which hardware devices to use.
  Wheel(WheelId wheel_id);

  WheelId wheel_id() const;

  void set_target_speed(q15_t speed);
  q15_t target_speed() const;
  q15_t actual_speed() const;

  void update();

  static void init_hardware();
  static Wheel* get_wheel(uint8_t wheel_id);

 private:
  WheelId wheel_id_;
  q15_t target_speed_;
  q31_t actual_speed_;
  arm_pid_instance_q31 pid_;
  Motor motor_;
  Encoder encoder_;

  static Wheel wheels[];
};

#endif  // BLUEPILL2WHEEL_WHEEL_H
