#include <cassert>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <random>

typedef int64_t Word;

constexpr Word OP_DIE = 0x0;
constexpr Word OP_NOOP = 0x1;
constexpr Word OP_DUP = 0x2;
constexpr Word OP_COPY = 0x3;
constexpr Word OP_JUMP = 0x4;

constexpr int64_t InstructionWidth = 4;


struct Space {
  Word *data;
  size_t size;


};

class Thread {
 public:
  Thread(Space *space, int64_t id, int64_t pc, int64_t direction)
      :space_(space), id_(id), pc_(pc), direction_(direction), is_alive_(true) {
  }

  Word read_word() {
    auto ret = space_->data[pc_];
    move_pc(1);
    return ret;
  }

  void move_pc(Word offset) {
    assert(pc_ >= 0 && pc_ < space_->size);
    assert(labs(direction_) < space_->size);

    pc_ += direction_ * offset;
    while (pc_ < 0)
      pc_ += space_->size;
    pc_ %= space_->size;
  }

  void step() {
    auto operation = read_word();
    auto attribute = read_word();
    auto operand0 = read_word();
    auto operand1 = read_word();

    std::cout << "thread<" << id_ << "> " << std::hex << pc_ << std::endl;

    switch (operation) {
      case OP_NOOP:
        break;
      case OP_DIE:
        is_alive_ = false;
        break;
      case OP_JUMP:
        move_pc(operand0);
        break;
      default:
        // invalid operation results death
        is_alive_ = false;
    }
  }

  bool is_alive() const { return is_alive_; }
  int64_t id() const { return id_; }
  Word pc() const { return pc_; }

 private:
  Space *space_;
  int64_t id_;
  Word pc_;
  Word direction_;

  bool is_alive_;

};


int main() {
  Space space;
  size_t world_size = 4 * 10;
  space.data = new Word[world_size];
  space.size = world_size;

  // randomly fill space
  uint32_t seed = 123;
  std::mt19937 rng(seed);
  std::uniform_int_distribution<Word> dist(std::numeric_limits<Word>::min(), std::numeric_limits<Word>::max());
  for (size_t i = 0; i < world_size; i++) {
//    space.data[i] = dist(rng);
    space.data[i] = 0;
  }


  std::list<Thread> threads;

  std::uniform_int_distribution<Word> pc_dist(0, world_size);
  for (int i = 0; i < 4; i++) {
    threads.emplace_back(&space, i, pc_dist(rng), i % 2 == 0 ? 1 : -1);
  }

  while (!threads.empty()) {
    for (auto it = threads.begin(); it != threads.end(); ) {
      if (!it->is_alive()) {
        std::cout << "Thread<" << it->id() << "> died at " << std::hex << std::setw(8) << std::setfill('0') << it->pc() << std::endl;
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