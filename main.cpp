#include "common.hpp"
#include "instruction.hpp"
#include "thread.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <random>


using namespace gol;

int main() {
  size_t thread_count = 16;
  Space space;
  size_t world_size = InstructionWidth * 128;
  space.data_ = new Word[world_size];
  space.size = world_size;

  // randomly fill space
  uint32_t seed = 123;
  std::mt19937 rng(seed);
  std::uniform_int_distribution<Word> dist(std::numeric_limits<Word>::min(), std::numeric_limits<Word>::max());
  for (size_t i = 0; i < world_size; i++) {
    space.data_[i] = dist(rng);
  }

  std::cout << "Space: " << std::endl;
  dump_instructions(std::cout, &space, 0, world_size / InstructionWidth, 1);
  std::cout << std::endl;

  std::list<Thread> threads;

  std::uniform_int_distribution<Word> pc_dist(0, world_size);
  std::uniform_int_distribution<Word> energy_dist(0, world_size/InstructionWidth * 16);
  for (int thread_id = 0; thread_id < thread_count; thread_id++) {
    threads.emplace_back(&space, thread_id, pc_dist(rng), thread_id % 2 == 0 ? 1 : -1, energy_dist(rng));
  }

  std::cout << "Threads: " << std::endl;
  for (auto &thread : threads) {
    thread.dump(std::cout) << std::endl;
  }
  std::cout << std::endl;

  while (!threads.empty()) {
    for (auto it = threads.begin(); it != threads.end(); ) {
      if (!it->is_alive()) {
        it->dump_death_info(std::cout);
        auto it2 = it;
        it++;
        threads.erase(it2);
        continue;
      }

      it->step();
      it++;
    }
  }

  std::cout << "all threads died, quiting" << std::endl;

  std::cout << "Final Space: " << std::endl;
  dump_instructions(std::cout, &space, 0, world_size / InstructionWidth, 1);
  std::cout << std::endl;
}
