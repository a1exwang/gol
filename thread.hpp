#pragma once

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

  std::ostream &print_id(std::ostream &os) {
    os << "thread<" << id_ << ">";
    return os;
  }

  std::ostream &dump(std::ostream &os) {
    print_id(os);
    os << " (pc energy) = (0x" << std::hex << std::setfill('0') << std::setw(8) << pc_ << " " << std::dec << energy_ << ")";
    return os;
  }

  std::ostream &dump_death_info(std::ostream &os) {
    if (!is_alive()) {
      print_id(os) << "died at "
                << std::hex << std::setw(8) << std::setfill('0') << pc_
                << ", cause: '" << death_cause_ << "', time_alive " << std::dec << time_alive_ << std::endl;
    }
    return os;
  }

  void move(Word &target, Word offset) {
    assert(target >= 0 && target < space_->size);
    assert(labs(direction_) < space_->size);

    target += direction_ * offset;
    while (target < 0)
      target += space_->size;
    target %= space_->size;
  }

  void step();

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

  // statistics
  int64_t time_alive_;

  bool is_alive_;
  std::string death_cause_;
};

}

