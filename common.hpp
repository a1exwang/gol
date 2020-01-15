#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <cmath>

namespace gol {
typedef int32_t Word;

struct Space {
  Word *data_;
  size_t size;
  bool is_valid(Word address) { return address >= 0 && address < size; }
  Word &at(Word address) {
    assert(is_valid(address));
    return data_[address];
  }
  Word round(Word address) {
    if (is_valid(address))
      return address;

    if (address < 0) {
      return (address + Word(ceil(double(-address) / size)) * size) % size;
    } else {
      return address % size;
    }

  }
};


}

