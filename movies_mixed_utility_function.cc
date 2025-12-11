// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "movies_mixed_utility_function.h"

#include <memory>
#include <string>
#include <vector>

#include "movies_data.h"

MoviesMixedUtilityFunction::MoviesMixedUtilityFunction(int user, double alpha)
    : mf_(), mu_(user), alpha_(alpha) {}

void MoviesMixedUtilityFunction::Reset() {
  mf_.Reset();
  mu_.Reset();
}

double MoviesMixedUtilityFunction::Delta(int movie) {
  return alpha_ * mf_.Delta(movie) + (1 - alpha_) * mu_.Delta(movie);
}

double MoviesMixedUtilityFunction::RemovalDelta(int movie) {
  return alpha_ * mf_.RemovalDelta(movie) +
         (1 - alpha_) * mu_.RemovalDelta(movie);
}

void MoviesMixedUtilityFunction::Add(int movie) {
  mu_.Add(movie);
  mf_.Add(movie);
}

void MoviesMixedUtilityFunction::Remove(int movie) {
  mu_.Remove(movie);
  mf_.Remove(movie);
}

// Not necessary, but overloaded for efficiency
double MoviesMixedUtilityFunction::RemoveAndIncreaseOracleCall(int movie) {
  --oracle_calls_;  // Since the below line will incur two oracle calls.
  return alpha_ * mf_.RemoveAndIncreaseOracleCall(movie) +
         (1 - alpha_) * mu_.RemoveAndIncreaseOracleCall(movie);
}

double MoviesMixedUtilityFunction::Objective(
    const std::vector<int>& movies) const {
  return alpha_ * mf_.Objective(movies) + (1 - alpha_) * mu_.Objective(movies);
}

const std::vector<int>& MoviesMixedUtilityFunction::GetUniverse() const {
  return MoviesData::GetInstance().GetMovieIds();
}

std::string MoviesMixedUtilityFunction::GetName() const {
  return "mix of: " + std::to_string(alpha_) + " of " + mf_.GetName() +
         " and " + std::to_string(1 - alpha_) + " of " + mu_.GetName();
}

std::unique_ptr<SubmodularFunction> MoviesMixedUtilityFunction::Clone() const {
  return std::make_unique<MoviesMixedUtilityFunction>(*this);
}
