// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_MOVIES_FACILITY_LOCATION_FUNCTION_H_
#define FAIR_SUBMODULAR_MATROID_MOVIES_FACILITY_LOCATION_FUNCTION_H_

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "submodular_function.h"

class MoviesFacilityLocationFunction : public SubmodularFunction {
 public:
  MoviesFacilityLocationFunction();

  // Sets S = empty set.
  void Reset() override;

  // Adds a new element to set S.
  void Add(int movie) override;

  // Removes an element from S.
  void Remove(int movie) override;

  // Removes an element from S and increases the oracle calls.
  // Not necessary, but overloaded for efficiency
  double RemoveAndIncreaseOracleCall(int movie) override;

  // Returns the universe of the utility function, as pairs.
  const std::vector<int>& GetUniverse() const override;

  // Get name of utility function.
  std::string GetName() const override;

  // Clone the object.
  std::unique_ptr<SubmodularFunction> Clone() const override;

 protected:
  // Computes f(S u {e}) - f(S).
  double Delta(int e) override;

  // Computes f(S) - f(S - e).
  double RemovalDelta(int e) override;

  // Computes f(S).
  double Objective(const std::vector<int>& elements) const override;

  // Needed for accessing Delta and RemovalDelta.
  friend class MoviesMixedUtilityFunction;

 private:
  // max_sim[i] = {0.0} u { sim(i,j) : j in S }
  std::vector<std::multiset<double, std::greater<double>>> max_sim_;
};

#endif  // FAIR_SUBMODULAR_MATROID_MOVIES_FACILITY_LOCATION_FUNCTION_H_
