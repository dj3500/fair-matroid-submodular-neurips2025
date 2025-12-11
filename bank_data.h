// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_BANK_DATA_H_
#define FAIR_SUBMODULAR_MATROID_BANK_DATA_H_

#include <map>
#include <vector>

class BankData {
 public:
  std::vector<std::vector<double>> input_;
  std::vector<int> age_grpcards_;
  std::vector<int> balance_grpcards_;
  std::map<int, int> age_map_;
  std::map<int, int> balance_map_;

  explicit BankData();
  ~BankData() = default;
};

#endif  // FAIR_SUBMODULAR_MATROID_BANK_DATA_H_
