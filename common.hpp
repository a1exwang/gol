#pragma once


#include <cstdint>
#include <cstddef>
#include <cassert>
#include <cmath>
#include <memory>
#include <list>
#include <random>

namespace gol {
typedef int32_t Word;
class Thread;

struct Space {
 public:
  Space(uint32_t seed, size_t size);

  std::mt19937 &rng() { return rng_; }

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

  void run();

  Thread *spawn();

  void dump_threads(std::ostream &os);

  std::unique_ptr<Word[]> data_;
  size_t size;
  uint32_t seed_;
  std::mt19937 rng_;

 private:
  int64_t last_thread_id_ = 0;
  std::list<Thread> threads;
  int64_t time_ = 0;
};

}

