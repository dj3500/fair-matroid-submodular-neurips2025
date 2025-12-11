// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

// Random Subset Algorithm
//
// This algorithm shuffles the universe, then greedily adds elements
// as long as it's matroid-feasible and upper-bound-feasible.
//

#ifndef FAIR_SUBMODULAR_MAXIMIZATION_2020_RANDOM_SUBSET_ALGORITHM_H_
#define FAIR_SUBMODULAR_MAXIMIZATION_2020_RANDOM_SUBSET_ALGORITHM_H_

#include "algorithm.h"
#include "utilities.h"

class RandomAlgorithm : public Algorithm {
 public:
  // Initialize the algorithm state.
  virtual void Init(const SubmodularFunction& sub_func_f,
                    const FairnessConstraint& fairness,
                    const Matroid& matroid);


  // Handles insertion of an element.
  void Insert(int element);

  // Gets current solution value.
  double GetSolutionValue();

  // Gets current solution.
  std::vector<int> GetSolutionVector();

  // Gets the name of the algorithm.
  std::string GetAlgorithmName() const;

 private:

  // The solution (i.e. sampled elements), all the elements in the universe.
  std::vector<int> solution_, universe_elements_;
};

#endif  // FAIR_SUBMODULAR_MAXIMIZATION_2020_RANDOM_SUBSET_ALGORITHM_H_
