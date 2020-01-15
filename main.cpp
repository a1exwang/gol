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

int main(int argc, const char **argv) {
  bool use_renderer = false;
  if (argc >= 2) {
    use_renderer = atoi(argv[1]);
  }

//  size_t thread_count = 16;
  size_t world_size = 1024 * 128;

  // randomly fill space
  uint32_t seed = 124;
  Space space(seed, world_size, use_renderer);

//  std::cout << "Space: " << std::endl;
//  dump_instructions(std::cout, &space, 0, world_size / InstructionWidth, 1);
//  std::cout << std::endl;

  std::cout << "Threads: " << std::endl;
  space.dump_threads(std::cout);

  space.run();

  LOG(INFO) << "Running complete" << std::endl;

//  VLOG(3) << "Final Space: " << std::endl;
//  {
//    std::stringstream ss;
//    dump_instructions(ss, &space, 0, world_size / InstructionWidth, 1);
//    VLOG(3) << ss.str();
//  }
}
