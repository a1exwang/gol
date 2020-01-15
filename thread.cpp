#include "thread.hpp"

#include <glog/logging.h>

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
    case OP_DUP: {
      // make sure the children have at least 1 energy.
      if (this->energy_ >= 2 + dup_energy_consumption) {
        auto target_address = space_->round(operand0);
        this->dup(*space_->spawn(), target_address);
      }
      break;
    }
    case OP_JUMP:
      VLOG(2) << "report::instruction jump "
                  << "0x" << std::hex << id_ << " "
                  << "0x" << std::hex << pc_ <<  " "
                  << std::dec << operand0;
      move(pc_, operand0);
      break;
    case OP_COLLECT:
      // collect energy from target, move target by 1
      target_ = space_->round(target_);

      auto energy_collected = abs(space_->at(target_)) % 100;
      space_->at(target_) = 0;

      energy_ += energy_collected;
      move(target_, 1);

      VLOG(2) << "report::instruction collect " << energy_collected << " 0x" << std::hex << pc_;
      break;
//      default:
//        // invalid operation results death
//        is_alive_ = false;
  }

  energy_ -= passive_energy_consumption;

  if (energy_ <= 0) {
    die("out of energy");
    return;
  }
}

}
