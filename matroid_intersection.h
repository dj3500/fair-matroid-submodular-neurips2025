// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_MATROID_INTERSECTION_H_
#define FAIR_SUBMODULAR_MATROID_MATROID_INTERSECTION_H_

#include <set>
#include <vector>

#include "matroid.h"
#include "fairness_constraint.h"
#include "submodular_function.h"
#include "partition_matroid.h"

// Constructs a maximum cardinality set in the intersections of two matroids.
// The solution is constructed in place in both input matroid objects.
void MaxIntersection(Matroid* matroid_a,
                     Matroid* matroid_b,
                     const std::vector<int>& elements);

 // Constructs a *fair* maximum cardinality set in the intersections of two matroids.
std::vector<int> FairMaxIntersection(Matroid* matroid_a,
                      FairnessConstraint* fairness_constraint,
                      const std::vector<int>& elements);
    
void SubMaxIntersectionSwapping(Matroid* matroid_a,
                        Matroid* matroid_b,
                        SubmodularFunction* sub_func_f,
                        const std::vector<int>& universe);

// Runs the Greedy algorithm to choose feasible elements to add
std::vector<int> Greedy(Matroid* matroid_a,
                        Matroid* matroid_b,
                        SubmodularFunction* sub_func_f,
                        const std::vector<int>& universe);

// Returns a set of augmenting paths w.r.t set_Y and the intersection of a partition matroid and a fairness constraint.
std::vector<std::vector<int>> ReturnPaths(PartitionMatroid* matroid_a,
                        FairnessConstraint* fairness_constraints,
                        SubmodularFunction* sub_func_f,
                        const std::vector<int>& set_Y,
                        const std::vector<int>& set_P);


void ApproximateFairSubmodularMaximization(PartitionMatroid* matroid_a,
                        FairnessConstraint* fairness_constraints,
                        SubmodularFunction* sub_func_f,
                        float epsilon);  
#endif  
