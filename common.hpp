#pragma once


#include <cstdint>
#include <cstddef>
#include <cassert>
#include <cmath>
#include <memory>
#include <list>
#include <random>

struct SDL_Renderer;
struct SDL_Window;

namespace gol {
typedef int64_t Word;
class Thread;

struct Space {
 public:
  Space(uint32_t seed, size_t size, bool gui);
  ~Space();

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

  bool throw_a_coin(double probability);

  void run();

  Thread *spawn();

  void dump_threads(std::ostream &os);
  void render();

  std::unique_ptr<Word[]> data_;
  size_t size;
  uint32_t seed_;
  std::mt19937 rng_;

 private:
  void init_renderer();

  int64_t energy_inlet();

  int64_t calculate_total_energy() const {
    int64_t ret = 0;
    for (int64_t i = 0; i < size; i++) {
      if (data_[i] > 0) {
        ret += data_[i];
      }
    }
    return ret;
  }

  int64_t last_thread_id_ = 0;
  std::list<Thread> threads;
  int64_t time_ = 0;
  size_t max_thread_count_ = 1024 * 1024;
  int64_t max_energy_income_rate_ = 1000L * 1000L;
  double energy_income_address_average = 1024;
  double energy_income_address_stdev = 1;
  int64_t energy_income_count_ = 1280;


  SDL_Renderer *renderer;
  SDL_Window *window;
  int window_width = 2048, window_height = 1024;
  bool use_renderer_;
};

}

