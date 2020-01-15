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
      if (this->energy_ >= 2 + dup_energy_consumption_) {
//        auto target_address = space_->clip(operand0);
//        auto target_address = space_->clip(space_->rng()());

        std::uniform_int_distribution<Word> offset_dist(-dup_range_, dup_range_);
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
      target_ = space_->clip(target_);

      if (space_->at(target_) > 0) {
        std::uniform_real_distribution<double> dist(0, 1);
        double collect_ratio = dist(space_->rng());
        auto energy_used = int64_t(space_->at(target_) * collect_ratio);
        auto energy_collected = int64_t(energy_used * collect_efficiency_);

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

  energy_ -= passive_energy_consumption_;

  if (energy_ <= 0) {
    die("out of energy");
    return;
  }
}
void Thread::dup(Thread &child, Word target_address) {

  this->energy_ -= dup_energy_consumption_;
  child.space_ = this->space_;
  child.pc_ = space_->clip(target_address + this->gene_entry_offset_);
  child.direction_ = this->direction_;
  child.gene_begin_ = target_address;
  child.gene_length_ = this->gene_length_;
  child.gene_entry_offset_ = this->gene_entry_offset_;
  child.target_ = (this->gene_begin_ + (this->gene_length_ + 1) * this->direction_);
  child.energy_ = this->energy_ / 2;
  child.parent_id_ = this->id_;
  child.inherit(this);
  this->energy_ = this->energy_ / 2;

  space_->memcpy(
      child.gene_begin_, child.direction_,
      this->gene_begin_, this->direction_,
      this->gene_length_);


  LOG(INFO) << "report::instruction dup "
            << "0x" << std::hex << this->id_ << " "
            << "0x" << std::hex << child.id_ << " "
            << "0x" << std::hex << target_address << " "
            << std::dec << this->energy_;
}

}
