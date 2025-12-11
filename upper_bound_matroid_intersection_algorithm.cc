// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "upper_bound_matroid_intersection_algorithm.h"

#include <memory>
#include <string>
#include <vector>

#include "fairness_constraint.h"
#include "matroid.h"
#include "matroid_intersection.h"
#include "submodular_function.h"

void UpperBoundMatroidIntersectionAlgorithm::Init(const SubmodularFunction& sub_func_f,
                                        const FairnessConstraint& fairness,
                                        const Matroid& matroid) {
  Algorithm::Init(sub_func_f, fairness, matroid);
  universe_elements_.clear();
  solution_vector_.clear();
}

void UpperBoundMatroidIntersectionAlgorithm::Insert(int element) {
  universe_elements_.push_back(element);
}

void UpperBoundMatroidIntersectionAlgorithm::Solve() {
  matroid_->Reset();
  sub_func_f_->Reset();
  auto fairness_matroid = fairness_->UpperBoundsToMatroid();
  if (use_greedy_) {
    Greedy(matroid_.get(), fairness_matroid.get(), sub_func_f_.get(),
                     universe_elements_);
  } else {
    SubMaxIntersectionSwapping(matroid_.get(), fairness_matroid.get(), sub_func_f_.get(),
                     universe_elements_);
  }
  solution_vector_ = matroid_->GetCurrent();
  solution_value_ =
      sub_func_f_->ObjectiveAndIncreaseOracleCall(solution_vector_);
}

double UpperBoundMatroidIntersectionAlgorithm::GetSolutionValue() {
  if (solution_vector_.empty()) {
    Solve();
  }
  return solution_value_;
}

std::vector<int> UpperBoundMatroidIntersectionAlgorithm::GetSolutionVector() {
  if (solution_vector_.empty()) {
    Solve();
  }
  return solution_vector_;
}

std::string UpperBoundMatroidIntersectionAlgorithm::GetAlgorithmName() const {
  return std::string("Upper bound matroid intersection algorithm (") + (use_greedy_ ? "greedy" : "swapping") + ")";
}
