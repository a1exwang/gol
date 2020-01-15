#pragma once

#include "common.hpp"

#include <cstdint>
#include <iostream>


namespace gol {
struct Space;

constexpr Word OP_SUICIDE = 0x0;
constexpr Word OP_NOOP = 0x1;
constexpr Word OP_DUP = 0x2;
constexpr Word OP_COPY = 0x3;
constexpr Word OP_JUMP = 0x4;
constexpr Word OP_COLLECT = 0x5;
constexpr Word OP_LOAD = 0x6;
constexpr Word OP_STORE = 0x7;
constexpr Word OP_SAY_HELLO = 0x8;
constexpr Word OP_RANDOM = 0x9;
constexpr Word OP_ADD = 0xa;
constexpr Word OP_MUL = 0xb;
constexpr Word OP_XOR = 0xc;
constexpr Word OP_AND = 0xd;
constexpr Word OP_OR = 0xe;
constexpr Word OP_NOT = 0xf;

constexpr int64_t InstructionWidth = 4;

void dump_instructions(std::ostream &os, const Space *space, int64_t start, int64_t instruction_count, int64_t direction);

}
