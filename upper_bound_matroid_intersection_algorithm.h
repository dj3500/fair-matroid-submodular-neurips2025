// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_UPPER_BOUND_MATROID_INTERSECTION_ALGORITHM_H_
#define FAIR_SUBMODULAR_MATROID_UPPER_BOUND_MATROID_INTERSECTION_ALGORITHM_H_

#include <string>
#include <vector>

#include "algorithm.h"
#include "fairness_constraint.h"
#include "matroid.h"
#include "submodular_function.h"

class UpperBoundMatroidIntersectionAlgorithm : public Algorithm {
 public:
  // Constructor with use_greedy flag.
  explicit UpperBoundMatroidIntersectionAlgorithm(bool use_greedy_instead_of_swapping) : use_greedy_(use_greedy_instead_of_swapping) {}

  // Initialize the algorithm state.
  // Default implementation only saves the three parameters' clones into the
  // object.
  void Init(const SubmodularFunction& sub_func_f,
            const FairnessConstraint& fairness,
            const Matroid& matroid) override;

  // Handles insertion of an element.
  void Insert(int element) override;

  // Gets current solution value.
  double GetSolutionValue() override;

  // Gets current solution. Only call this after calling GetSolutionValue().
  std::vector<int> GetSolutionVector() override;

  // Gets the name of the algorithm.
  std::string GetAlgorithmName() const override;

 private:
  // Generate the solution in matroid_.
  void Solve();

  // All the elements in the universe.
  std::vector<int> universe_elements_;

  // The final solution vector.
  std::vector<int> solution_vector_;

  // The final solution value.
  double solution_value_;

  // Whether to use Greedy algorithm instead of SubMaxIntersectionSwapping.
  bool use_greedy_;
};

#endif  // FAIR_SUBMODULAR_MATROID_UPPER_BOUND_MATROID_INTERSECTION_ALGORITHM_H_
