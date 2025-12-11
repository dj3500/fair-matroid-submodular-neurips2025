// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "random_algorithm.h"

#include "utilities.h"

void RandomAlgorithm::Init(const SubmodularFunction& sub_func_f,
                     const FairnessConstraint& fairness,
                     const Matroid& matroid) {
  Algorithm::Init(sub_func_f, fairness, matroid);
  universe_elements_.clear();
  solution_.clear();
}

void RandomAlgorithm::Insert(int element) {
  universe_elements_.push_back(element);
}

double RandomAlgorithm::GetSolutionValue() {
  matroid_->Reset();
  solution_.clear();
  std::unique_ptr<Matroid> upper_bound_matroid = fairness_->UpperBoundsToMatroid();

  RandomHandler::Shuffle(universe_elements_);
  for (int element : universe_elements_) {
    if (matroid_->CanAdd(element) && upper_bound_matroid->CanAdd(element)) {
      matroid_->Add(element);
      upper_bound_matroid->Add(element);
      solution_.push_back(element);
    }
  }

  return sub_func_f_->ObjectiveAndIncreaseOracleCall(solution_);
}

std::vector<int> RandomAlgorithm::GetSolutionVector() {
  return solution_;
}

std::string RandomAlgorithm::GetAlgorithmName() const {
  return "Random algorithm";
}
