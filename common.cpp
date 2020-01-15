#include "common.hpp"
#include "thread.hpp"

#include <random>
#include <iostream>
#include <iomanip>


namespace gol {

Thread *Space::spawn() {
  this->threads.emplace_back(this, last_thread_id_++);
  return &this->threads.back();

}
Space::Space(uint32_t seed, size_t size) :data_(new Word[size]), size(size), seed_(seed), rng_(seed) {
  std::uniform_int_distribution<Word> dist(std::numeric_limits<Word>::min(), std::numeric_limits<Word>::max());
  for (size_t i = 0; i < size; i++) {
    data_[i] = dist(rng_);
  }


  size_t thread_count = 16;
  std::uniform_int_distribution<Word> pc_dist(0, size);
  std::uniform_int_distribution<Word> energy_dist(0, size/InstructionWidth * 16);
  for (int thread_id = 0; thread_id < thread_count; thread_id++) {
    threads.emplace_back(this, thread_id, pc_dist(rng()), thread_id % 2 == 0 ? 1 : -1, energy_dist(rng()));
  }
}

void Space::run() {
  while (!threads.empty()) {
    for (auto it = threads.begin(); it != threads.end(); ) {
      if (!it->is_alive()) {
        it->dump_death_info();
        auto it2 = it;
        it++;
        threads.erase(it2);
        continue;
      }

      it->step();
      it++;
    }
    time_++;
    VLOG(2) << "report::population " << time_ << " " << threads.size();
  }
}

void Space::dump_threads(std::ostream &os) {
  for (auto &thread : threads) {
    thread.dump(os) << std::endl;
  }
  os << std::endl;
}

}