// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "lower_bound_matroid_intersection_algorithm.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "fairness_constraint.h"
#include "matroid.h"
#include "matroid_intersection.h"
#include "submodular_function.h"

LowerBoundMatroidIntersectionAlgorithm::LowerBoundMatroidIntersectionAlgorithm(PostprocessingType postprocessing)
    : postprocessing_(postprocessing) {}

void LowerBoundMatroidIntersectionAlgorithm::Init(const SubmodularFunction& sub_func_f,
                                 const FairnessConstraint& fairness,
                                 const Matroid& matroid) {
  Algorithm::Init(sub_func_f, fairness, matroid);
  solution_.clear();
  universe_elements_.clear();
}

void LowerBoundMatroidIntersectionAlgorithm::Insert(int element) {
  universe_elements_.push_back(element);
}

double LowerBoundMatroidIntersectionAlgorithm::GetSolutionValue() {
  std::unique_ptr<Matroid> lower_matroid = fairness_->LowerBoundsToMatroid();
  std::vector<int> all_elements = universe_elements_;
  MaxIntersection(matroid_.get(), lower_matroid.get(), all_elements);
  std::vector<int> solution = matroid_->GetCurrent();
  assert(fairness_->IsFeasible(solution));

  if (postprocessing_ == FAST_GREEDY) {
    // Populate fairness_ and sub_func_f_.
    for (int element : solution) {
      fairness_->Add(element);
      sub_func_f_->Add(element);
    }

    // Add more elements greedily.
    std::vector<int> elements_left;
    std::sort(all_elements.begin(), all_elements.end());
    std::sort(solution.begin(), solution.end());
    std::set_difference(all_elements.begin(), all_elements.end(),
                        solution.begin(), solution.end(),
                        std::inserter(elements_left, elements_left.begin()));
    std::sort(elements_left.begin(), elements_left.end(),
              [this](int lhs, int rhs) {
                return sub_func_f_->DeltaAndIncreaseOracleCall(lhs) >
                       sub_func_f_->DeltaAndIncreaseOracleCall(rhs);
              });
    for (int element : elements_left) {
      if (fairness_->CanAdd(element) && matroid_->CanAdd(element)) {
        fairness_->Add(element);
        matroid_->Add(element);
        solution.push_back(element);
      }
    }
  } else if (postprocessing_ == GREEDY) {
    std::unique_ptr<Matroid> upper_matroid = fairness_->UpperBoundsToMatroid();
    // populate upper_matroid and sub_func_f_.
    for (int element : solution) {
      upper_matroid->Add(element);
      sub_func_f_->Add(element);
    }
    // now all of: matroid_, upper_matroid, and sub_func_f_ have the solution.
    Greedy(matroid_.get(), upper_matroid.get(), sub_func_f_.get(),
           universe_elements_);
    solution = matroid_->GetCurrent();
    assert(fairness_->IsFeasible(solution));
  } // else if postprocessing_ == NONE, do nothing.

  solution_ = solution;
  return sub_func_f_->ObjectiveAndIncreaseOracleCall(solution_);
}

std::vector<int> LowerBoundMatroidIntersectionAlgorithm::GetSolutionVector() { return solution_; }

std::string LowerBoundMatroidIntersectionAlgorithm::GetAlgorithmName() const {
  switch (postprocessing_) {
    case NONE:
      return "Lower bound matroid intersection algorithm (postprocessing=NONE)";
    case FAST_GREEDY:
      return "Lower bound matroid intersection algorithm (postprocessing=FAST_GREEDY)";
    case GREEDY:
      return "Lower bound matroid intersection algorithm (postprocessing=GREEDY)";
    default:
      assert(false && "Unknown postprocessing type.");
      return "Lower bound matroid intersection algorithm (postprocessing=UNKNOWN)";
  }
}
