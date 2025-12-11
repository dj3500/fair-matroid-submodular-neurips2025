// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_PARTITION_MATROID_H_
#define FAIR_SUBMODULAR_MATROID_PARTITION_MATROID_H_

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "matroid.h"

class PartitionMatroid : public Matroid {
 public:
  PartitionMatroid(const std::map<int, int>& groups_map,
                   const std::vector<int>& ks);

  ~PartitionMatroid() override = default;

  // Reset to empty set.
  void Reset() override;

  // Return whether adding an element would be feasible.
  bool CanAdd(int element) const override;

  // Return whether add element while removing anothe one would be feasible.
  bool CanSwap(int element, int swap) const override;

  // Returns all possible swaps for a given new element.
  std::vector<int> GetAllSwaps(int elements) const override;

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

  // Returns the group of an element.
  int GetGroup(int element) const;

  // Returns number of groups. 
  int GetGroupsNum() const;

  // Returns whether an element is in the current set.
  bool InCurrent(int element) const override;

  // Clone the object.
  std::unique_ptr<Matroid> Clone() const override;

 private:
  // Map universe elements to groups
  std::map<int, int> groups_map_;

  // Groups upper bounds
  std::vector<int> ks_;

  // Number of groups
  int num_groups_;

  // Current number of elements per group
  std::vector<int> current_grpcards_;

  // Current set
  std::set<int> current_set_;
};

#endif  // FAIR_SUBMODULAR_MATROID_PARTITION_MATROID_H_
