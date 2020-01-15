#include "common.hpp"
#include "thread.hpp"


#include <random>
#include <iostream>
#include <iomanip>

#include <SDL2/SDL.h>


namespace gol {

Thread *Space::spawn() {
  this->threads.emplace_back(this, last_thread_id_++);
  return &this->threads.back();

}
Space::Space(uint32_t seed, size_t size, bool use_renderer) :data_(new Word[size]), size(size), seed_(seed), rng_(seed), use_renderer_(use_renderer) {
  std::uniform_int_distribution<Word> dist(0, Word(1) << 16);
  for (size_t i = 0; i < size; i++) {
    data_[i] = dist(rng_);
  }

  size_t thread_count = 16;
  int64_t max_initial_energy = size/InstructionWidth * 16;

  std::uniform_int_distribution<Word> pc_dist(0, size);
  std::uniform_int_distribution<Word> energy_dist(0, max_initial_energy);
  for (int i = 0; i < thread_count; i++) {
    threads.emplace_back(this, last_thread_id_++, pc_dist(rng()), i % 2 == 0 ? 1 : -1, energy_dist(rng()));
  }

  if (use_renderer_) {
    init_renderer();
  }
}

void Space::init_renderer() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
    exit(1);
  }
  window = SDL_CreateWindow(
      "Mandelbrot Set",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      window_width, window_height,
      SDL_WINDOW_SHOWN
  );
  if (window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    exit(1);
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

}

void Space::run() {
  while (!threads.empty()) {
    auto total_energy_income = energy_inlet();
    for (auto it = threads.begin(); it != threads.end(); ) {
      if (!it->is_alive()) {
        it->dump_death_info();
        auto it2 = it;
        it++;
        threads.erase(it2);
        continue;
      }

      it->step();
      it++;
    }
    time_++;
    VLOG(2) << "report::global_info "
            << time_ << " "
            << threads.size() << " "
            << calculate_total_energy() << " "
            << total_energy_income << " "
            ;
    if (threads.size() > max_thread_count_) {
      LOG(ERROR) << "max_thread_count_(" << max_thread_count_ << ") exceeded, exiting" << std::endl;
      return;
    }

    if (use_renderer_) {
      render();
    }
  }
}

void Space::dump_threads(std::ostream &os) {
  for (auto &thread : threads) {
    thread.dump(os) << std::endl;
  }
  os << std::endl;
}
int64_t Space::energy_inlet() {

  std::normal_distribution<double> dist_address(energy_income_address_average, energy_income_address_stdev);
  std::uniform_int_distribution<size_t> dist_energy(0, max_energy_income_rate_);

  int64_t total_income = 0;
  for (int64_t i = 0; i < energy_income_count_; i++) {
    auto address = Word(dist_address(rng_));
    auto energy = int64_t(dist_energy(rng_));
    if (energy < 0)
      energy = 0;
    // prevent energy overflow
    if (data_[address] > 0 && std::numeric_limits<Word>::max() - data_[address] < energy) {
      energy = std::numeric_limits<Word>::max() - data_[address];
    }
    data_[address] += energy;

    total_income += energy;

    VLOG(2) << "report::energy_inlet " << time_
            << " 0x" << std::hex << address
            << " " << std::dec << energy;
  }
  return total_income;
}

void Space::render() {
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
    exit(0);
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

  int64_t bucket_count = 1024;
  auto bucket_size = size / bucket_count;
  std::vector<int64_t> buckets(bucket_count);
  for (auto &thread : threads) {
    buckets[thread.pc() / bucket_size]++;
  }
  double bucket_width = double(window_width) / bucket_count;
  double bucket_height_ratio = double(window_height) / max_thread_count_ * bucket_count;
  for (int i = 0; i < buckets.size(); i++) {
    int x = i * bucket_width;
    int y = 0;
    SDL_RenderDrawLine(renderer, x, window_height - y, x, window_height - (y+buckets[i]*bucket_height_ratio));
  }
  SDL_RenderPresent(renderer);
}

bool Space::throw_a_coin(double probability) {
  CHECK(probability >= 0 && probability <= 1);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  return dist(rng()) < probability;
}

Space::~Space() {
  if (use_renderer_) {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
  }

}

}