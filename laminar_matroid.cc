// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "laminar_matroid.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <utility>
#include <vector>

LaminarMatroid::LaminarMatroid(
    const std::map<int, std::vector<int>>& groups_map,
    const std::vector<int>& ks)
    : groups_map_(groups_map),
      ks_(ks),
      num_groups_(ks.size()),
      current_grpcards_(num_groups_, 0) {
  // Assume groups are laminar.
}

void LaminarMatroid::Reset() {
  std::fill(current_grpcards_.begin(), current_grpcards_.end(), 0);
  current_set_.clear();
}

bool LaminarMatroid::CanAdd(int element) const {
  assert(!current_set_.count(element));
  const std::vector<int>& elt_groups = groups_map_.at(element);
  for (int elt_group : elt_groups) {
    if (current_grpcards_[elt_group] + 1 > ks_[elt_group]) return false;
  }
  return true;
}

bool LaminarMatroid::CanSwap(int element, int swap) const {
  assert(!current_set_.count(element));
  assert(current_set_.count(swap));
  const std::vector<int>& elt_groups = groups_map_.at(element);
  const std::vector<int>& swap_groups = groups_map_.at(swap);
  for (int elt_group : elt_groups) {
    if (current_grpcards_[elt_group] + 1 > ks_[elt_group] &&
        std::find(swap_groups.begin(), swap_groups.end(), elt_group) ==
            swap_groups.end()) {
      return false;
    }
  }
  return true;
}

void LaminarMatroid::Add(int element) {
  assert(!current_set_.count(element));
  std::vector<int> elt_groups = groups_map_.at(element);
  for (int elt_group : elt_groups) {
    current_grpcards_[elt_group]++;
  }
  current_set_.insert(element);
}

void LaminarMatroid::Remove(int element) {
  assert(current_set_.count(element));
  std::vector<int> elt_groups = groups_map_.at(element);
  for (int elt_group : elt_groups) {
    current_grpcards_[elt_group]--;
  }
  current_set_.erase(element);
}

bool LaminarMatroid::IsFeasible(const std::vector<int>& elements) const {
  std::vector<int> grpcards = std::vector<int>(num_groups_, 0);
  std::vector<int> elt_groups;
  for (int elt : elements) {
    elt_groups = groups_map_.at(elt);
    for (int elt_group : elt_groups) {
      grpcards[elt_group]++;
      if (grpcards[elt_group] > ks_[elt_group]) {
        return false;
      }
    }
  }
  return true;
}

bool LaminarMatroid::CurrentIsFeasible() const {
  for (int i = 0; i < num_groups_; ++i) {
    if (current_grpcards_[i] > ks_[i]) {
      return false;
    }
  }
  return true;
}

std::vector<int> LaminarMatroid::GetCurrent() const {
  return std::vector<int>(current_set_.begin(), current_set_.end());
}

bool LaminarMatroid::InCurrent(int element) const {
  return current_set_.count(element);
}

std::unique_ptr<Matroid> LaminarMatroid::Clone() const {
  return std::make_unique<LaminarMatroid>(*this);
}
