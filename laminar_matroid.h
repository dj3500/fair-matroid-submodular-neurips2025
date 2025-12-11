// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_LAMINAR_MATROID_H_
#define FAIR_SUBMODULAR_MATROID_LAMINAR_MATROID_H_

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "matroid.h"

class LaminarMatroid : public Matroid {
 public:
  LaminarMatroid(const std::map<int, std::vector<int>>& groups_map,
                 const std::vector<int>& ks);

  ~LaminarMatroid() override = default;

  // Reset to empty set.
  void Reset() override;

  // Return whether adding an element would be feasible.
  bool CanAdd(int element) const override;

  // Return whether add element while removing anothe one would be feasible.
  bool CanSwap(int element, int swap) const override;

  // Add an element. Assumes that the element can be added.
  void Add(int element) override;

  // Removes the element.
  void Remove(int element) override;

  // Checks if a set is feasible.
  bool IsFeasible(const std::vector<int>& elements) const override;

  // Checks whether the current set is feasible.
  bool CurrentIsFeasible() const override;

  // Return the current set.
  std::vector<int> GetCurrent() const override;

  // Returns whether an element is in the current set.
  bool InCurrent(int element) const override;

  // Clone the object.
  std::unique_ptr<Matroid> Clone() const override;

 private:
  // map universe elements to list of groups they belong to
  std::map<int, std::vector<int>> groups_map_;
  // groups upper bounds
  std::vector<int> ks_;
  // number of groups
  int num_groups_;
  // current number of elements per group
  std::vector<int> current_grpcards_;
  // current set
  std::set<int> current_set_;
};

#endif  // FAIR_SUBMODULAR_MATROID_LAMINAR_MATROID_H_
