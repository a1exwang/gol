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
    case OP_SUICIDE: {
      auto probability = 0.01; //double(operand0) / std::numeric_limits<Word>::max();
      if (space_->throw_a_coin(probability)) {
        die("suicide");
      }
      break;
    }
    case OP_NOOP:
      // do nothing
      break;
    case OP_DUP: {
      // make sure the children have at least 1 energy.
      if (this->energy_ >= 2 + dup_energy_consumption) {
//        auto target_address = space_->round(operand0);
//        auto target_address = space_->round(space_->rng()());

        std::uniform_int_distribution<Word> offset_dist(-128, 128);
        auto target_address = pc_ + offset_dist(space_->rng());

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

      if (space_->at(target_) > 0) {
        std::uniform_real_distribution<double> dist(0, 1);
        double collect_ratio = dist(space_->rng());
        auto energy_used = int64_t(space_->at(target_) * collect_ratio);
        auto energy_collected = int64_t(energy_used * collect_efficiency);

        space_->at(target_) -= energy_used;
        energy_ += energy_collected;

        VLOG(2) << "report::instruction collect "
                << collect_ratio << " "
                << energy_collected << " "
                << energy_used
                << " 0x" << std::hex << pc_;
      }
      move(target_, 1);

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
