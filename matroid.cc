// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "matroid.h"

#include <cassert>
#include <vector>

std::vector<int> Matroid::GetAllSwaps(int element) const {
  std::vector<int> all_swaps;
  for (int swap : GetCurrent()) {
    if (CanSwap(element, swap)) {
      all_swaps.push_back(swap);
    }
  }
  return all_swaps;
}

void Matroid::Swap(int element, int swap) {
  assert(InCurrent(swap));
  Remove(swap);
  Add(element);
}

bool Matroid::CurrentIsFeasible() const { return IsFeasible(GetCurrent()); }

bool Matroid::InCurrent(int element) const {
  for (int current_element : GetCurrent()) {
    if (current_element == element) {
      return true;
    }
  }
  return false;
}
