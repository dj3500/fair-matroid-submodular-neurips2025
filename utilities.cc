// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "utilities.h"

#include <stdint.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

std::mt19937 RandomHandler::generator_;

std::string PrettyNum(int64_t number) {
  std::string pretty_number = std::to_string(number);
  for (int i = static_cast<int>(pretty_number.size()) - 3; i > 0; i -= 3) {
    pretty_number.insert(pretty_number.begin() + i, ',');
  }
  return pretty_number;
}

void Fail(const std::string& error) {
  std::cerr << error << std::endl;
  exit(1);
}

std::vector<double> LogSpace(double small, double large, double base) {
  if (small > large) {
    std::vector<double> log_space = LogSpace(large, small, base);
    std::reverse(log_space.begin(), log_space.end());
    return log_space;
  }
  assert(base > 1);
  int steps =
      static_cast<int>(ceil((log(large) - log(small)) / log(base) - 1e-6));
  double step = pow(large / small, 1.0 / steps);
  std::vector<double> log_space = {small};
  for (int i = 0; i < steps; ++i) {
    log_space.push_back(log_space.back() * step);
  }
  return log_space;
}
