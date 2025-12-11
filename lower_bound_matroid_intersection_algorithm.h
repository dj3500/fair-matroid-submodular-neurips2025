// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_LOWER_BOUND_MATROID_INTERSECTION_ALGORITHM_H_
#define FAIR_SUBMODULAR_MATROID_LOWER_BOUND_MATROID_INTERSECTION_ALGORITHM_H_

#include <memory>
#include <string>
#include <vector>

#include "fairness_constraint.h"
#include "algorithm.h"
#include "matroid.h"
#include "submodular_function.h"

class LowerBoundMatroidIntersectionAlgorithm : public Algorithm {
 public:
  enum PostprocessingType {
    NONE = 0, // find feasible solution with only ell_c elements in each color
    FAST_GREEDY = 1, // O-tilde(n) time greedy algorithm as in the 2023 paper
    GREEDY = 2 // O-tilde(nk) time greedy algorithm
  };

  explicit LowerBoundMatroidIntersectionAlgorithm(PostprocessingType postprocessing);

  // Initialize the algorithm state.
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
  PostprocessingType postprocessing_;

  std::vector<int> universe_elements_;

  // The final solution set.
  std::vector<int> solution_;
};

#endif  // FAIR_SUBMODULAR_MATROID_LOWER_BOUND_MATROID_INTERSECTION_ALGORITHM_H_
