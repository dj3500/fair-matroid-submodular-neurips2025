// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_ALGORITHM_H_
#define FAIR_SUBMODULAR_MATROID_ALGORITHM_H_

#include <memory>
#include <string>
#include <vector>

#include "fairness_constraint.h"
#include "matroid.h"
#include "submodular_function.h"

// Any algorithm should be used as follows:
// * Init()
// * set SubmodularFunction::oracle_calls_ = 0
// * n times Insert()
// * if two-pass: BeginNextPass(), then again n times Insert()
// * GetSolutionValue() (obligatory! the algorithm might only compute the final
//   solution here)
// * GetSolutionVector() (optional)
// * read SubmodularFunction::oracle_calls_

class Algorithm {
 public:
  // Initialize the algorithm state.
  // Default implementation only saves the three parameters' clones into the
  // object.
  virtual void Init(const SubmodularFunction& sub_func_f,
                    const FairnessConstraint& fairness, const Matroid& matroid);

  // Handles insertion of an element.
  virtual void Insert(int element) = 0;

  // Gets current solution value.
  virtual double GetSolutionValue() = 0;

  // Gets current solution. Only call this after calling GetSolutionValue().
  virtual std::vector<int> GetSolutionVector() = 0;

  // Gets the name of the algorithm.
  virtual std::string GetAlgorithmName() const = 0;

  // Returns the number of passes the algorithm makes (1 or 2 for us). Default
  // is 1.
  virtual int GetNumberOfPasses() const;

  // Signal to the algorithm that the next pass is beginning.
  // (Do not call before the first pass, or after the last pass.
  //  Do not call at all for single-pass algorithms.)
  virtual void BeginNextPass();

  virtual ~Algorithm() = default;

 protected:
  // Color lower and upper bounds.
  std::unique_ptr<FairnessConstraint> fairness_;

  // Submodular function.
  std::unique_ptr<SubmodularFunction> sub_func_f_;

  // Matroid.
  std::unique_ptr<Matroid> matroid_;
};

#endif  // FAIR_SUBMODULAR_MATROID_ALGORITHM_H_
