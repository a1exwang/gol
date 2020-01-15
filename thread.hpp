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
#include <glog/logging.h>


namespace gol {

class Thread {
 public:
  Thread(Space *space, int64_t id) :space_(space), id_(id) {}
  Thread(Space *space, int64_t id,
      int64_t pc, int64_t direction, Word initial_engery, int64_t gene_begin, int64_t gene_length)
      : space_(space), id_(id), pc_(pc), direction_(direction), is_alive_(true), energy_(initial_engery),
        gene_begin_(gene_begin), gene_length_(gene_length) {
  }

  Word read_word() {
    CHECK(pc_ >= 0 && pc_ < space_->size) << "0x" << std::hex << id_ << " 0x" << std::hex << pc_;
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

  void dump_death_info() {
    CHECK(!is_alive());

    LOG(INFO) << "report::die "
              << "0x" << std::hex << id_ << " "
              << "0x" << std::hex << std::setw(8) << std::setfill('0') << pc_ << " "
              << std::dec << time_alive_ << " '"
              << death_cause_ << "'"  << std::endl;
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
  void inherit(Thread *parent) {
    this->passive_energy_consumption_ = parent->passive_energy_consumption_;
    this->dup_energy_consumption_ = parent->dup_energy_consumption_;
    this->dup_range_ = parent->dup_range_;
    this->collect_efficiency_ = parent->collect_efficiency_;
  }
  void die(const std::string &cause) {
    is_alive_ = false;
    death_cause_ = cause;
  }

  void dup(Thread &child, Word target_address);

  Space *space_;
  int64_t id_ = -1;
  int64_t parent_id_ = -1;
  Word pc_;
  Word direction_;

  // registers
  Word gene_begin_;
  Word gene_length_;
  Word gene_entry_offset_;
  Word target_;

  // status
  int64_t energy_ = 0;

  // attributes
  Word passive_energy_consumption_ = 1;
  Word dup_energy_consumption_ = 1;
  Word dup_range_ = 128;
  double collect_efficiency_ = 1e-4;

  // statistics
  int64_t time_alive_ = 0;

  bool is_alive_ = true;
  std::string death_cause_;
};

}

