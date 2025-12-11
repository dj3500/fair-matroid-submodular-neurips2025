// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_CLUSTERING_FUNCTION_H_
#define FAIR_SUBMODULAR_MATROID_CLUSTERING_FUNCTION_H_

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "submodular_function.h"

class ClusteringFunction : public SubmodularFunction {
 public:
  explicit ClusteringFunction(const std::vector<std::vector<double>>& input);

  void Reset() override;

  void Add(int element) override;

  void Remove(int element) override;

  double RemoveAndIncreaseOracleCall(int element) override;

  const std::vector<int>& GetUniverse() const override;

  std::string GetName() const override;

  std::unique_ptr<SubmodularFunction> Clone() const override;

  ~ClusteringFunction() override = default;

 protected:
  double Delta(int element) override;

  double RemovalDelta(int element) override;

  // F(S) = sum_{i in V} dist(i,-1) - min_{j in S U {-1}} dist(i, j)
  double Objective(const std::vector<int>& elements) const override;

 private:
  // current distances: min_dist[i] = dist(i,-1) U {dist(i, j) : j in S}
  std::vector<std::multiset<double, std::less<double>>> min_dist_;

  // Distance squared between two points, or to origin if y=-1
  double distance(int x, int y) const;

  // Coordinates of input points.
  std::vector<std::vector<double>> input_;

  // Elements of the universe.
  std::vector<int> universe_;

  // The maximum possible solution value;
  double max_value_ = 0;
};

#endif  // FAIR_SUBMODULAR_MATROID_CLUSTERING_FUNCTION_H_
