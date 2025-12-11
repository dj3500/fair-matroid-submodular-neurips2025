// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_MOVIES_USER_UTILITY_FUNCTION_H_
#define FAIR_SUBMODULAR_MATROID_MOVIES_USER_UTILITY_FUNCTION_H_

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "submodular_function.h"

class MoviesUserUtilityFunction : public SubmodularFunction {
 public:
  explicit MoviesUserUtilityFunction(int user);

  // Sets S = empty set.
  void Reset() override;

  // Adds a new element to set S.
  void Add(int e) override;

  // Removes an element from S.
  void Remove(int e) override;

  // Returns the universe of the utility function, as pairs.
  const std::vector<int>& GetUniverse() const override;

  // Get name of utility function.
  std::string GetName() const override;

  // Clone the object.
  std::unique_ptr<SubmodularFunction> Clone() const override;

 protected:
  // Computes f(S u {e}) - f(S).
  double Delta(int movie) override;

  // Computes f(S) - f(S - e).
  double RemovalDelta(int movie) override;

  // Needed for accessing Delta and RemovalDelta.
  double Objective(const std::vector<int>& elements) const override;

  // Needed to access Delta and RemovalDelta.
  friend class MoviesMixedUtilityFunction;

 private:
  const int user_;  // ID of user for whom the movie quality is computed.
  // Elements currently present in the solution.
  std::set<int> present_elements_;
};

#endif  // FAIR_SUBMODULAR_MATROID_MOVIES_USER_UTILITY_FUNCTION_H_
