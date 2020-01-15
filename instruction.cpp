#include "instruction.hpp"
#include "common.hpp"

#include <iomanip>
#include <iostream>

namespace gol {

std::string get_operation_name(Word operation) {
  switch (operation) {
    case OP_SUICIDE:
      return "suicide";
    case OP_NOOP:
      return "noop";
    case OP_DUP:
      return "dup";
    case OP_COPY:
      return "copy";
    case OP_JUMP:
      return "jump";
    case OP_COLLECT:
      return "collect";
    case OP_LOAD:
      return "load";
    case OP_STORE:
      return "store";
    case OP_SAY_HELLO:
      return "say_hello";
    case OP_RANDOM:
      return "random";
    case OP_ADD:
      return "add";
    case OP_MUL:
      return "mul";
    case OP_XOR:
      return "xor";
    case OP_AND:
      return "and";
    case OP_OR:
      return "or";
    case OP_NOT:
      return "not";
    default:
      return "<unknown>";
  }
}

void dump_instructions(std::ostream &os, const Space *space, int64_t start, int64_t count, int64_t direction) {
  int64_t offset = start;

  // offset += direction within range
  auto move_offset = [&offset, direction, space]() {
    offset += direction;
    while (offset < 0)
      offset += space->size;
    offset %= space->size;
  };

  for (int64_t i = 0; i < count; i++) {
    os << std::hex << std::setw(8) << std::setfill('0') << offset << ": ";

    Word operation = space->data_[offset];
    operation = operation & 0x0f;
    move_offset();
    Word attribute = space->data_[offset];
    move_offset();
    Word operand0 = space->data_[offset];
    move_offset();
    Word operand1 = space->data_[offset];
    move_offset();

    os << std::hex << std::setw(4) << std::setfill('0') << (operation & (0x10000-1)) << " "
       << std::setw(8) << std::setfill('0') << attribute << " "
       << std::setw(8) << std::setfill('0') << operand0 << " "
       << std::setw(8) << std::setfill('0') << operand1 << " | ";
    os << get_operation_name(operation) << std::endl;
  }
}

}