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
//  size_t thread_count = 16;
  size_t world_size = InstructionWidth * 4096;

  // randomly fill space
  uint32_t seed = 123;
  Space space(seed, world_size);

  std::cout << "Space: " << std::endl;
  dump_instructions(std::cout, &space, 0, world_size / InstructionWidth, 1);
  std::cout << std::endl;

  std::cout << "Threads: " << std::endl;
  space.dump_threads(std::cout);

  space.run();

  LOG(INFO) << "all threads died, quiting" << std::endl;

  VLOG(3) << "Final Space: " << std::endl;
  {
    std::stringstream ss;
    dump_instructions(ss, &space, 0, world_size / InstructionWidth, 1);
    VLOG(3) << ss.str();
  }
}
