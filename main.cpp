#include "common.hpp"
#include "instruction.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <random>


namespace gol {

class Thread {
 public:
  Thread(Space *space, int64_t id, int64_t pc, int64_t direction, Word initial_engery)
      : space_(space), id_(id), pc_(pc), direction_(direction), is_alive_(true), energy_(initial_engery) {
  }

  Word read_word() {
    auto ret = space_->data_[pc_];
    move(pc_, 1);
    return ret;
  }

  void move(Word &target, Word offset) {
    assert(target >= 0 && target < space_->size);
    assert(labs(direction_) < space_->size);

    target += direction_ * offset;
    while (target < 0)
      target += space_->size;
    target %= space_->size;
  }

  void step() {
    auto operation = read_word();
    auto attribute = read_word();
    auto operand0 = read_word();
    auto operand1 = read_word();

//    std::cout << "thread<" << id_ << "> " << std::hex << pc_ << std::endl;

    switch (operation) {
      case OP_SUICIDE:
        die("suicide");
        break;
      case OP_NOOP:
        // do nothing
        break;
      case OP_DUP:
        // TODO
        // duplicate self with the same genes, consumes 1 energy.
        // resulting two same threads, each with half energy
        break;
      case OP_JUMP:
        move(pc_, operand0);
        break;
      case OP_COLLECT:
        // collect data from target, move target by 1
        if (space_->is_valid(target_)) {
          energy_ += space_->at(target_);
          move(target_, 1);
        }
        break;
//      default:
//        // invalid operation results death
//        is_alive_ = false;
    }

    energy_ -= energy_consumption_;

    if (energy_ <= 0) {
      die("out of energy");
      return;
    }


  }

  bool is_alive() const { return is_alive_; }
  int64_t id() const { return id_; }
  Word pc() const { return pc_; }
  std::string death_cause() const { return death_cause_; }

 private:
  void die(const std::string &cause) {
    is_alive_ = false;
    death_cause_ = cause;
  }

  Space *space_;
  int64_t id_;
  Word pc_;
  Word direction_;

  // registers
  Word gene_begin_;
  Word gene_end_;
  Word target_;

  // status
  Word energy_;

  // attributes
  Word energy_consumption_ = 1;

  bool is_alive_;
  std::string death_cause_;
};

}
using namespace gol;

int main() {
  Space space;
  size_t world_size = 4 * 10;
  space.data_ = new Word[world_size];
  space.size = world_size;

  // randomly fill space
  uint32_t seed = 123;
  std::mt19937 rng(seed);
  std::uniform_int_distribution<Word> dist(std::numeric_limits<Word>::min(), std::numeric_limits<Word>::max());
  for (size_t i = 0; i < world_size; i++) {
    space.data_[i] = dist(rng);
  }

  dump_instructions(std::cout, &space, 0, world_size / InstructionWidth, 1);

  std::list<Thread> threads;

  std::uniform_int_distribution<Word> pc_dist(0, world_size);
  for (int i = 0; i < 4; i++) {
    threads.emplace_back(&space, i, pc_dist(rng), i % 2 == 0 ? 1 : -1, i * 100);
  }

  while (!threads.empty()) {
    for (auto it = threads.begin(); it != threads.end(); ) {
      if (!it->is_alive()) {
        std::cout << "Thread<" << it->id() << "> died at "
                  << std::hex << std::setw(8) << std::setfill('0') << it->pc()
                  << ", cause: '" << it->death_cause() << "'" << std::endl;
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
}
