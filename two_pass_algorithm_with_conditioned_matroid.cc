// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "two_pass_algorithm_with_conditioned_matroid.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "conditioned_matroid.h"
#include "fairness_constraint.h"
#include "matroid_intersection.h"
#include "uniform_matroid.h"
#include "utilities.h"

void TwoPassAlgorithmWithConditionedMatroid::Init(
    const SubmodularFunction& sub_func_f, const FairnessConstraint& fairness,
    const Matroid& matroid) {
  Algorithm::Init(sub_func_f, fairness, matroid);
  bounds_ = fairness.GetBounds();
  universe_elements_.clear();
}

void TwoPassAlgorithmWithConditionedMatroid::Insert(int element) {
  universe_elements_.push_back(element);
}

/*
void TwoPassAlgorithmWithConditionedMatroid::GreedyFirstPass() {
  matroid_->Reset();
  fairness_->Reset();
  sub_func_f_->Reset();
  BetterGreedyAlgorithm greedy(true);
  greedy.Init(*sub_func_f_, *fairness_, *matroid_);
  for (const int element : universe_elements_) {
    greedy.Insert(element);
  }
  double solution_value = greedy.GetSolutionValue();
  std::cout << "Solution value after greedy first pass: " << solution_value <<
      std::endl;
  first_round_solution_ = greedy.GetSolutionVector();
}
*/

/*
void TwoPassAlgorithmWithConditionedMatroid::FirstPass() {
  per_color_solutions_ =
      std::vector<std::vector<int>>(bounds_.size(), std::vector<int>());
  std::vector<std::unique_ptr<Matroid>> per_color_matroids;
  matroid_->Reset();
  per_color_matroids.reserve(bounds_.size());
  for (int i = 0; i < bounds_.size(); i++) {
    per_color_matroids.push_back(matroid_->Clone());
  }
  for (const int element : universe_elements_) {
    int color = fairness_->GetColor(element);
    if (per_color_matroids[color]->CanAdd(element)) {
      // && per_color_solutions_[color].size() < bounds_[color].first) {
      per_color_matroids[color]->Add(element);
      per_color_solutions_[color].push_back(element);
    }
  }
}*/

void TwoPassAlgorithmWithConditionedMatroid::FindFeasibleSolution() {
  /*std::vector<int> all_colors_solution;
  for (const auto& solution : per_color_solutions_) {
    all_colors_solution.insert(all_colors_solution.end(), solution.begin(),
                               solution.end());
  }*/
  matroid_->Reset();
  fairness_->Reset();
  MaxIntersection(matroid_.get(), fairness_->LowerBoundsToMatroid().get(),
                  universe_elements_);
  first_round_solution_ = matroid_->GetCurrent();
}

void TwoPassAlgorithmWithConditionedMatroid::DivideSolution() {
  lower_bound_solutions_.clear();
  lower_bound_solutions_.push_back(std::vector<int>());
  lower_bound_solutions_.push_back(std::vector<int>());
  std::vector<int> num_picked_per_color(bounds_.size(), 0);
  for (const auto& element : first_round_solution_) {
    lower_bound_solutions_
        [(num_picked_per_color[fairness_->GetColor(element)]++) % 2]
            .push_back(element);
  }
}

std::vector<int> TwoPassAlgorithmWithConditionedMatroid::SecondPass(
    std::vector<int> start_solution) {
  matroid_->Reset();
  fairness_->Reset();
  sub_func_f_->Reset();
  weights_.clear();

  ConditionedMatroid condmatroid(*matroid_, start_solution);

  std::unique_ptr<Matroid> color_mat = fairness_->UpperBoundsToMatroid();
  color_mat->Reset();
  if (use_greedy_) {
    Greedy(&condmatroid, color_mat.get(), sub_func_f_.get(),
                     universe_elements_);
  } else {
    SubMaxIntersectionSwapping(&condmatroid, color_mat.get(), sub_func_f_.get(),
                     universe_elements_);
  }

  std::vector<int> current_sol = color_mat->GetCurrent();
  std::vector<int> start_solution_not_chosen;
  for (int el : start_solution) {
    if (!color_mat->InCurrent(el)) {
      start_solution_not_chosen.push_back(el);
    }
  }

  // find the best subset of start_solution to add by
  // max {F(S U S_current) : S subset of S_start, S U S_current in I^C}.
  UniformMatroid dummy_mat(1'000'000'000);
  ConditionedMatroid cond_fairness(*color_mat, current_sol);
  // sub_func_f_ already has current sol.
  // dummy_mat has nothing.
  // cond_fairness also has nothing (it's reset when created).
  if (use_greedy_) {
    Greedy(&dummy_mat, &cond_fairness, sub_func_f_.get(),
                     start_solution_not_chosen);
  } else {
    SubMaxIntersectionSwapping(&dummy_mat, &cond_fairness, sub_func_f_.get(),
                     start_solution_not_chosen);
  }

  return append(current_sol, cond_fairness.GetCurrent());
}

double TwoPassAlgorithmWithConditionedMatroid::GetSolutionValue() {
  //GreedyFirstPass();
  //FirstPass();
  FindFeasibleSolution();
  DivideSolution();
  std::pair<std::vector<int>, double> answer[2];
  for (int i = 0; i < 2; ++i) {
    answer[i].first = SecondPass(lower_bound_solutions_[i]);
    answer[i].second =
        sub_func_f_->ObjectiveAndIncreaseOracleCall(answer[i].first);
  }

  final_solution_ =
      answer[0].second > answer[1].second ? answer[0].first : answer[1].first;
  return std::max(answer[0].second, answer[1].second);
}

std::vector<int> TwoPassAlgorithmWithConditionedMatroid::GetSolutionVector() {
  return final_solution_;
}

std::string TwoPassAlgorithmWithConditionedMatroid::GetAlgorithmName() const {
  return std::string("Two pass algorithm (") + (use_greedy_ ? "greedy" : "swapping") + ")";
}
