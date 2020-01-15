#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>

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
};


}

