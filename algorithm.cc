// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "algorithm.h"

#include <cassert>
#include <memory>

#include "fairness_constraint.h"
#include "matroid.h"
#include "submodular_function.h"

void Algorithm::Init(const SubmodularFunction& sub_func_f,
                     const FairnessConstraint& fairness,
                     const Matroid& matroid) {
  sub_func_f_ = sub_func_f.Clone();
  sub_func_f_->Reset();
  fairness_ = fairness.Clone();
  fairness_->Reset();
  matroid_ = matroid.Clone();
  matroid_->Reset();
}

int Algorithm::GetNumberOfPasses() const {
  // default is one-pass
  return 1;
}

void Algorithm::BeginNextPass() {
  // multi-pass algorithms should overload this, so if we ever reach here,
  // this means that the algorithm is single-pass (so this should not be called)
  assert(false);
}
