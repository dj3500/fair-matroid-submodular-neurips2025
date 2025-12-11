// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_UTILITIES_H_
#define FAIR_SUBMODULAR_MATROID_UTILITIES_H_

#include <stdint.h>

#include <random>
#include <string>
#include <vector>
#include <algorithm>

class RandomHandler {
 public:
  // Default-initialized, to get reproducible cross-platform randomness.
  static std::mt19937 generator_;

  // Shuffles a vector.
  // Own implementation to get cross-platform reproducibility.
  template <class T>
  static void Shuffle(std::vector<T>& input) {
    std::shuffle(input.begin(), input.end(), generator_);
  }

  // Rounds a number up or down to one of the two closest integers,
  // so that the expected value is equal to the input number.
  static int RoundUpOrDown(double x) {
    double f = x - floor(x);
    // generate a coin flip with probability f to round up.
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    if (distribution(generator_) > f) {
      return static_cast<int>(floor(x));
    } else {
      return static_cast<int>(ceil(x));
    }
  }
};

// Returns a number in a way that is easier to read.
// Formats numbers like 1078546 -> 1,078,546.
std::string PrettyNum(int64_t number);

// This functions is called if there is something wrong and the error string
// 'error' will be written in "cerr'.
void Fail(const std::string& error);

template <typename T, typename Fun>
void Filter(std::vector<T>& input, const Fun& predicate) {
  auto source = input.begin(), target = input.begin();
  for (; source != input.end(); ++source) {
    if (predicate(*source)) {
      *target = *source;
      ++target;
    }
  }
  input.erase(target, input.end());
}

// Returns a sequence: small, ..., large.
// Where we roughly have a[i+1]/a[i] = base (we use base = 1+eps).
std::vector<double> LogSpace(double small, double large, double base);

template <typename T>
std::vector<T> append(std::vector<T> v1, const std::vector<T>& v2) {
  v1.insert(v1.end(), v2.begin(), v2.end());
  return v1;
}

#endif  // FAIR_SUBMODULAR_MATROID_UTILITIES_H_
