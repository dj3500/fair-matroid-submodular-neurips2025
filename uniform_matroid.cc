// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "uniform_matroid.h"

#include <cassert>
#include <memory>
#include <set>
#include <vector>

UniformMatroid::UniformMatroid(int k) : k_(k) {}

void UniformMatroid::Reset() { current_set_.clear(); }

bool UniformMatroid::CanAdd(int element) const {
  assert(!current_set_.count(element));
  return current_set_.size() + 1 <= k_;
}

bool UniformMatroid::CanSwap(int element, int swap) const {
  assert(!current_set_.count(element));
  assert(current_set_.count(swap));
  return true;
}

std::vector<int> UniformMatroid::GetAllSwaps(int element) const {
  return GetCurrent();
}

void UniformMatroid::Add(int element) {
  assert(!current_set_.count(element));
  current_set_.insert(element);
}

void UniformMatroid::Remove(int element) {
  assert(current_set_.count(element));
  current_set_.erase(element);
}

bool UniformMatroid::IsFeasible(const std::vector<int>& elements) const {
  return elements.size() <= k_;
}

bool UniformMatroid::CurrentIsFeasible() const {
  return current_set_.size() <= k_;
}

std::vector<int> UniformMatroid::GetCurrent() const {
  return std::vector<int>(current_set_.begin(), current_set_.end());
}

bool UniformMatroid::InCurrent(int element) const {
  return current_set_.count(element);
}

std::unique_ptr<Matroid> UniformMatroid::Clone() const {
  return std::make_unique<UniformMatroid>(*this);
}
