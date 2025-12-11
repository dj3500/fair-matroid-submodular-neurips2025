// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_APPROXIMATE_FAIRNESS_ALGORITHM_H_
#define FAIR_SUBMODULAR_MATROID_APPROXIMATE_FAIRNESS_ALGORITHM_H_

#include <memory>
#include <string>
#include <vector>

#include "algorithm.h"
#include "fairness_constraint.h"
#include "matroid.h"
#include "submodular_function.h"

// WARNING: ONLY IMPLEMENTED FOR PARTITION MATROIDS!

class ApproximateFairnessAlgorithm : public Algorithm {
 public:
  // Constructor with epsilon parameter.
  explicit ApproximateFairnessAlgorithm(double epsilon);

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

 protected:
  // The final solution set.
  std::vector<int> solution_;

  std::vector<int> universe_elements_;

  double epsilon_;
};

#endif  // FAIR_SUBMODULAR_MATROID_APPROXIMATE_FAIRNESS_ALGORITHM_H_
