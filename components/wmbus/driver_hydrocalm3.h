/*
  Based on: https://github.com/weetmuts/wmbusmeters/blob/master/src/driver_hydrocalm3.cc
  Copyright (C) 2017-2022 Fredrik Öhrström (gpl-3.0-or-later)
*/

#pragma once

#include "driver.h"

#include <vector>
#include <string>

struct Hydrocalm3: Driver
{
  Hydrocalm3() : Driver(std::string("hydrocalm3")) {};
  bool get_value(std::vector<unsigned char> &telegram, float &total_usage) override {
    bool ret_val = false;
    uint32_t usage = 0;
    size_t i = 17;
    uint32_t total_register = 0x0C0E;
    while (i < telegram.size()) {
      uint32_t c = (((uint32_t)telegram[i+0] << 8) | ((uint32_t)telegram[i+1]));
      if (c == total_register) {
        i += 2;
        usage = bcd_2_int(telegram, i, 4);
        // in kWh
        total_usage = usage / 3600000;
        ret_val = true;
        break;
      }
      i++;
    }
    return ret_val;
  };

private:
  uint32_t bcd_2_int(const std::vector<unsigned char> &telegram, size_t start, size_t length) {
    uint32_t result{0};
    uint16_t l_half{0};
    uint16_t h_half{0};
    uint32_t factor{1};
    uint8_t i{0};
    while (i < length) {
      h_half = (telegram[start + i] & 0xF0) >> 4;
      l_half = telegram[start + i] & 0x0F;
      result += ((h_half * 10) + l_half) * factor;
      factor *= 100;
      i++;
    }
    return result;
  }
};
