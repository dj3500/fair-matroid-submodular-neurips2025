// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "approximate_fairness_algorithm.h"

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

#include "fairness_constraint.h"
#include "matroid.h"
#include "matroid_intersection.h"
#include "submodular_function.h"

// WARNING: ONLY IMPLEMENTED FOR PARTITION MATROIDS!

ApproximateFairnessAlgorithm::ApproximateFairnessAlgorithm(double epsilon)
    : epsilon_(epsilon) {}

void ApproximateFairnessAlgorithm::Init(const SubmodularFunction& sub_func_f,
                           const FairnessConstraint& fairness,
                           const Matroid& matroid) {     
  Algorithm::Init(sub_func_f, fairness, matroid);
  matroid_->Reset();
  solution_.clear();
}

void ApproximateFairnessAlgorithm::Insert(int element) {
  universe_elements_.push_back(element);
}

// WARNING: ONLY IMPLEMENTED FOR PARTITION MATROIDS!
double ApproximateFairnessAlgorithm::GetSolutionValue() {
  // Run the approximate fairness algorithm.
  assert(matroid_ != nullptr);
  ApproximateFairSubmodularMaximization(static_cast<PartitionMatroid*>(matroid_.get()),
    fairness_.get(),
    sub_func_f_.get(),
    epsilon_
  );
  solution_ = matroid_->GetCurrent();
  return sub_func_f_->ObjectiveAndIncreaseOracleCall(solution_);
}

std::vector<int> ApproximateFairnessAlgorithm::GetSolutionVector() { return solution_; }

std::string ApproximateFairnessAlgorithm::GetAlgorithmName() const {
  return "Approximate Fairness Algorithm (epsilon=" + std::to_string(epsilon_) + ")";
}
