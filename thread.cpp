#include "thread.hpp"

namespace gol {

void Thread::step() {
  auto operation = read_word() & 0x0f;
  auto attribute = read_word();
  auto operand0 = read_word();
  auto operand1 = read_word();

  time_alive_++;

//  std::cout << "thread<" << id_ << "> " << std::hex << pc_ << ": ";
//  dump_instructions(std::cout, space_, pc_, 1, direction_);

  switch (operation) {
    case OP_SUICIDE:
//      die("suicide");
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
      target_ = space_->round(target_);

      auto energy_collected = abs(space_->at(target_)) % 100;
      space_->at(target_) = 0;

      energy_ += energy_collected;
      print_id(std::cout) << " collect " << energy_collected << " at 0x" << std::hex << pc_ << std::endl;
      move(target_, 1);
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

}
