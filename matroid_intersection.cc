// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include "matroid_intersection.h"

#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "utilities.h"
#include "matroid.h"
#include "partition_matroid.h"
#include "fairness_constraint.h"
#include "submodular_function.h"

void MaxIntersection(Matroid* matroid_a, Matroid* matroid_b,
                     const std::vector<int>& elements) {
  matroid_a->Reset();
  matroid_b->Reset();
  // Adjacency lists;
  std::map<int, std::vector<int>> exchange_graph;
  while (true) {
    // Greedily add elements to the solution;
    for (int element : elements) {
      if (matroid_a->InCurrent(element)) {
        continue;
      }
      if (matroid_a->CanAdd(element) && matroid_b->CanAdd(element)) {
        matroid_a->Add(element);
        matroid_b->Add(element);
      }
    }

    // Construct the exchange graph.
    exchange_graph.clear();
    for (int element : elements) {
      if (matroid_a->InCurrent(element)) {
        continue;
      }
      for (int a_swap : matroid_a->GetAllSwaps(element)) {
        exchange_graph[a_swap].push_back(element);
      }
      for (int b_swap : matroid_b->GetAllSwaps(element)) {
        exchange_graph[element].push_back(b_swap);
      }
    }

    // Find an augmenting path via BFS.
    std::map<int, int> bfs_parent;
    std::queue<int> queue;
    int aug_path_dest = -1;
    for (int element : elements) {
      if (matroid_a->InCurrent(element)) {
        continue;
      }
      if (matroid_a->CanAdd(element)) {
        bfs_parent[element] = -1;
        queue.push(element);
      }
    }
    while (!queue.empty()) {
      const int element = queue.front();
      queue.pop();
      if (!matroid_b->InCurrent(element) && matroid_b->CanAdd(element)) {
        aug_path_dest = element;
        break;
      }
      for (int neighbor : exchange_graph[element]) {
        if (!bfs_parent.count(neighbor)) {
          bfs_parent[neighbor] = element;
          queue.push(neighbor);
        }
      }
    }

    if (aug_path_dest == -1) {
      // No augmenting path found.
      break;
    }

    // Swap along the augmenting path.
    std::cerr << "we are applying an augmenting path" << std::endl;
    int out_element = aug_path_dest;
    int in_element = bfs_parent[aug_path_dest];
    while (in_element != -1) {
      matroid_a->Swap(out_element, in_element);
      matroid_b->Swap(out_element, in_element);
      out_element = bfs_parent[in_element];
      in_element = bfs_parent[out_element];
    }
    matroid_a->Add(out_element);
    matroid_b->Add(out_element);
  }

  assert(matroid_a->CurrentIsFeasible());
  assert(matroid_b->CurrentIsFeasible());
}

std::vector<int> FairMaxIntersection(Matroid* matroid_a, FairnessConstraint* fairness_constraints,
                     const std::vector<int>& elements) {
  
  std::unique_ptr<Matroid> lower_bound_matroid_ptr = fairness_constraints->LowerBoundsToMatroid();
  Matroid* lower_bound_matroid = lower_bound_matroid_ptr.get();

  assert(matroid_a != nullptr);
  assert(lower_bound_matroid != nullptr);
  MaxIntersection(matroid_a, lower_bound_matroid, elements); // At this point, we've made a fair set, stored within matroid_a and matroid_b.
  
  std::vector<std::pair<int, int>> bounds = fairness_constraints->GetBounds();
  int lower_bound_sum = 0;
  for (int i = 0; i < bounds.size(); i++) {
    lower_bound_sum += bounds[i].first;
  }
  // Check if the size of lower_bound_matroid->GetCurrent() is equal to the sum of lower bound contraints in fairness_constraints
  if (lower_bound_matroid->GetCurrent().size() != lower_bound_sum) {
    std::cerr << "No fair set exists" << std::endl;
    return std::vector<int>();
  }

  std::unique_ptr<Matroid> matroid_b_ptr = fairness_constraints->UpperBoundsToMatroid();
  Matroid* matroid_b = matroid_b_ptr.get();

  for (int element : lower_bound_matroid->GetCurrent()) {
    matroid_b->Add(element);
  }
  // Now we have a fair set in matroid_a and matroid_b. We finish off by taking any valid augmenting paths. 
  // Adjacency lists;
  std::map<int, std::vector<int>> exchange_graph;
  while (true) {
    // Greedily add elements to the solution;
    for (int element : elements) {
      if (matroid_a->InCurrent(element)) {
        continue;
      }
      if (matroid_a->CanAdd(element) && matroid_b->CanAdd(element)) {
        matroid_a->Add(element);
        matroid_b->Add(element);
      }
    }

    // Construct the exchange graph.
    exchange_graph.clear();
    for (int element : elements) {
      if (matroid_a->InCurrent(element)) {
        continue;
      }
      for (int a_swap : matroid_a->GetAllSwaps(element)) {
        exchange_graph[a_swap].push_back(element);
      }
      for (int b_swap : matroid_b->GetAllSwaps(element)) {
        exchange_graph[element].push_back(b_swap);
      }
    }

    // Find an augmenting path via BFS.
    std::map<int, int> bfs_parent;
    std::queue<int> queue;
    int aug_path_dest = -1;
    for (int element : elements) {
      if (matroid_a->InCurrent(element)) {
        continue;
      }
      if (matroid_a->CanAdd(element)) {
        bfs_parent[element] = -1;
        queue.push(element);
      }
    }
    while (!queue.empty()) {
      const int element = queue.front();
      queue.pop();
      if (!matroid_b->InCurrent(element) && matroid_b->CanAdd(element)) {
        aug_path_dest = element;
        break;
      }
      for (int neighbor : exchange_graph[element]) {
        if (!bfs_parent.count(neighbor)) {
          bfs_parent[neighbor] = element;
          queue.push(neighbor);
        }
      }
    }

    if (aug_path_dest == -1) {
      // No augmenting path found.
      break;
    }

    // Swap along the augmenting path.
    std::cerr << "we are applying an augmenting path" << std::endl;
    int out_element = aug_path_dest;
    int in_element = bfs_parent[aug_path_dest];
    while (in_element != -1) {
      matroid_a->Swap(out_element, in_element);
      matroid_b->Swap(out_element, in_element);
      out_element = bfs_parent[in_element];
      in_element = bfs_parent[out_element];
    }
    matroid_a->Add(out_element);
    matroid_b->Add(out_element);
  }

  assert(matroid_a->CurrentIsFeasible());
  assert(matroid_b->CurrentIsFeasible());
  return matroid_a->GetCurrent();
}

// Returns if an element is needed to be removed from `matroid_` to insert
// `element`. Returns "-1" if no element is needed to be remove and "-2" if
// the element cannot be swapped.
int MinWeightElementToRemove(Matroid* matroid,
                             std::unordered_map<int, double>& weight,
                             const int element) {
  if (matroid->CanAdd(element)) {
    return -1;
  }
  int best_element = -2;
  for (const int& swap : matroid->GetAllSwaps(element)) {
    //if (const_elements.find(swap) != const_elements.end()) continue;
    if (best_element < 0 || weight[best_element] > weight[swap]) {
      best_element = swap;
    }
  }
  return best_element;
}

void SubMaxIntersectionSwapping(Matroid* matroid_a, Matroid* matroid_b,
                        SubmodularFunction* sub_func_f,
                        const std::vector<int>& universe) {
  // DO NOT reset the matroids here.
  std::unordered_map<int, double> weight;
  for (const int& element : universe) {
    //if (const_elements.count(element)) continue;  // don't add const_elements
    int first_swap =
        MinWeightElementToRemove(matroid_a, weight, element);
    int second_swap =
        MinWeightElementToRemove(matroid_b, weight, element);
    if (first_swap == -2 || second_swap == -2) continue;
    double total_decrease = weight[first_swap] + weight[second_swap];
    double cont_element = sub_func_f->DeltaAndIncreaseOracleCall(element);
    if (2 * total_decrease <= cont_element) {
      if (first_swap >= 0) {
        matroid_a->Remove(first_swap);
        matroid_b->Remove(first_swap);
        sub_func_f->Remove(first_swap);
      }
      if (second_swap >= 0 && first_swap != second_swap) {
        matroid_a->Remove(second_swap);
        matroid_b->Remove(second_swap);
        sub_func_f->Remove(second_swap);
      }
      matroid_a->Add(element);
      matroid_b->Add(element);
      sub_func_f->Add(element);
      weight[element] = cont_element;
    }
  }
}

// This method runs the greedy algorithm
std::vector<int> Greedy(Matroid* matroid_a, Matroid* matroid_b, SubmodularFunction* sub_func_f, const std::vector<int>& universe){
  while (true) {
    // Find the element with the maximum marginal gain.
    double max_gain = 0;
    int best_element = -1;
    for (int element : universe) {
      if (matroid_a->InCurrent(element) || matroid_b->InCurrent(element)) {
        // Skip if the element is already in the matroid.
        continue;
      }
      if (!matroid_a->CanAdd(element) || !matroid_b->CanAdd(element)) {
        // Skip if the element cannot be added to both matroids.
        continue;
      }
      double gain = sub_func_f->DeltaAndIncreaseOracleCall(element);
      if (gain > max_gain) {
        max_gain = gain;
        best_element = element;
      }
    }
    // Add the best element to the matroid.
    if (best_element != -1) {
      matroid_a->Add(best_element);
      matroid_b->Add(best_element);
      sub_func_f->Add(best_element);
    } else {
      if (!matroid_a->CurrentIsFeasible() || !matroid_b->CurrentIsFeasible()) {
        std::cerr << "Something is wrong with this method! The current solution is not feasible" << std::endl;
        return std::vector<int>(); 
      }
      return matroid_a->GetCurrent(); 
    }
  }
  return matroid_a->GetCurrent();
}

std::vector<std::vector<int>> ReturnPaths(PartitionMatroid* matroid_a, FairnessConstraint* fairness_constraints, SubmodularFunction* sub_func_f, const std::vector<int>& set_Y, const std::vector<int>& set_P) {
  // First, we're going to create a bipartite directed exchange graph between fairness constraints and matroid_a.
  // The directed edges are going to be the elements in set_Y and set_P.
  // Edges i -e-> j mean that e is in set_P. 
  // Edges i <-e- j mean that e is in set_Y.
  // We'll use DFS to find augmenting paths, alternating paths (or cycles and remove them) in this graph. 
  // Start from an unsaturated node in the fairness constraint. End in either an unsaturated node in the fairness constraint or a sink node in the matroid_a.
  // Remove theis path and add it to the list of paths we'll return. 

  fairness_constraints->Reset(); // I don't need anything in these constraints right now. 
  matroid_a->Reset(); 

  std::vector<std::vector<int>> list_of_paths;

  std::map<int, std::map<int, int>> exchange_graph; // We're going to think of the LHS as fairness constraints (with odd vertices) and the RHS as matroid_a (with even vertices).
  std::map<int, int> fair_to_number; // The map that takes in a fairness constraint and maps it to it's vertex number in the exchange graph.
  std::map<int, int> part_to_number; // The map that takes in a part and maps it to it's vertex number in the exchange graph.

  for (int i = 0; i < fairness_constraints->GetColorNum(); i++) {
    fair_to_number[i] = 2*i + 1; // Odd vertices are fairness constraints.
  }

  for (int i = 0; i < matroid_a->GetGroupsNum(); i++) {
    part_to_number[i] = 2*i + 2; // Even positive vertices are matroid_a.
  }

  std::set<int> Y_union_P(set_Y.begin(), set_Y.end());
  Y_union_P.insert(set_P.begin(), set_P.end());

  // Iterate over the union set
  for (const int& element : Y_union_P) {
    // Find out what fairness constraint this element belongs to. 
    int fairness_constraint = fairness_constraints->GetColor(element);
    int part = matroid_a->GetGroup(element);

    if (std::find(set_P.begin(), set_P.end(), element) != set_P.end() &&
        std::find(set_Y.begin(), set_Y.end(), element) != set_Y.end()) {
      // If the element is in both sets, we don't add any edges.
      continue;
    }
    
    if (std::find(set_P.begin(), set_P.end(), element) != set_P.end()) {
      // Add edges from P to be directed left to right.
      exchange_graph[fair_to_number[fairness_constraint]][element] = part_to_number[part];
    }
    if (std::find(set_Y.begin(), set_Y.end(), element) != set_Y.end()) {
      // Add the edges from Y to be directed right to left.
      exchange_graph[part_to_number[part]][element] = fair_to_number[fairness_constraint];
    }
  }

  //std::cerr << "Built exchange graph" << std::endl;
  // DEBUGGING: Print out the whole exchange graph.
  //std::cerr << "Exchange graph: " << std::endl;
  //for (int a = 0; a < fairness_constraints->GetColors().size(); a++) {
    //if (exchange_graph[2 * a + 1].size() > 0) {
    //std::cerr << "Fairness constraint " << 2 * a + 1 << ": ";
    //for (const auto& pair : exchange_graph[2 * a + 1]) {
      //std::cerr << pair.first << " , " << pair.second << " | ";
    //}
    //std::cerr << std::endl;
    //}
  //}
  //for (int a = 0; a < matroid_a->GetGroups().size(); a++) {
    //if (exchange_graph[2 * a + 2].size() > 0) {
    //std::cerr << "Matroid constraint " << 2 * a + 2 << ": ";
    //for (const auto& pair : exchange_graph[2 * a + 2]) {
      //std::cerr << pair.first << " , " << pair.second << " | ";
    //}
    //std::cerr << std::endl;
    //}
  //}

  // Now we have to find what the LHS sources are on the fairness constraints. 
  std::vector<int> sources;
  std::map<int, int> P_out_degree; // a map from LHS vertices (odd number) to out-degree. 
  std::map<int, int> Y_in_degree;
  for (const int& element : set_P) {
    int fair = fairness_constraints->GetColor(element);
    P_out_degree[fair_to_number[fair]]++;
  }

  for (const int& element : set_Y) {
    int fair = fairness_constraints->GetColor(element);
    Y_in_degree[fair_to_number[fair]]++;
  }

  for (int h = 0; h < fairness_constraints->GetColorNum(); h++) {
    int lhs_vertex = 2 * h + 1;
    int difference = P_out_degree[lhs_vertex] - Y_in_degree[lhs_vertex];
    if (difference > 0) {
      for (int j = 0; j < difference; j++) {
        sources.push_back(lhs_vertex);
      }
    }
  }

  std::cerr << "Found sources." << std::endl;

  // DEBUGGING: Print out the sources.
  //std::cerr << "Sources: " << std::endl;
  //for (int i = 0; i < sources.size(); i++) {
    //std::cerr << sources[i] << " ";
  //}
  //std::cerr << std::endl;
  //std::cerr << std::endl;

  // FOR DEBUGGING PURPOSES: Let's look at the sources in random order. 
  //std::random_device rd;
  //std::mt19937 g(rd());
  //std::shuffle(sources.begin(), sources.end(), g);
  // FOR DEBUGGING PURPOSES: Let's look at the sources in random order.

  for (int i = 0; i < sources.size(); i++) {
    int source = sources[i];
    //std::cerr << "Starting from source: " << source << std::endl;
    int head = source; 
    
    std::set<int> visited_vertices;
    std::vector<std::pair<int, int>> walk;

    bool found_cycle = false;
    bool reached_sink = false;
    bool augmenting = false;

    //walk.push_back({-1, head}); // The first element is the edge, the second is the vertex.

    while (!found_cycle && !reached_sink) {
      visited_vertices.insert(head);
      int next_edge = exchange_graph[head].begin()->first; 
      int next_vertex = exchange_graph[head][next_edge];
      walk.push_back({next_edge, next_vertex});

      head = next_vertex;
      found_cycle = visited_vertices.count(head) > 0;
      reached_sink = exchange_graph[head].size() == 0;
      if (reached_sink) {
        if (head%2 == 0) { // If the head is even, that means we reached a sink in matroid_a, we have an odd sized path.
          augmenting = true;
        }
      }
    }

    int tail; // The tail is the starting vertex with which we delete edges along the path or cycle. 
    std::vector<std::pair<int, int>> edges_to_delete;
    std::vector<int> path;
    //std::cerr << "Here's the walk: ";
      //for (const auto& pair : walk) {
        //std::cerr << pair.first << " , " << pair.second << " | ";
      //}
    if (reached_sink) {
      for (const auto& pair : walk) {
        path.push_back(pair.first); // Just adding the edges. Even indexed edges are the ones you add. Odd ones are the ones you remove. 
      }
      edges_to_delete = walk;
      if (augmenting) {
        //std::cerr << "Found an AUGMENTING path!" << std::endl;
      } else {
        //std::cerr << "Found an alternating path!" << std::endl;
        assert(edges_to_delete.size() % 2 == 0); // DEBUGGING PURPOSES: This should be true.
      }
      list_of_paths.emplace_back(path);
      tail = source; 
    } else if (found_cycle) { 
      //std::cerr << "Got a cycle instead." << std::endl;
      bool found_head = sources[i] == head; // Will probably be false at first. 
      for (const auto& pair : walk) {
        if (found_head) {
          edges_to_delete.push_back(pair);
        }
        else {
          found_head = pair.second == head; 
        }
      }
      assert(edges_to_delete.size() % 2 == 0); // DEBUGGING PURPOSES: This should be true.
      i -= 1; // If we've found a cycle, we have to go back one step in the sources vector. 
      tail = head; // error here????
    }
    // Remove the edges in edges_to_delete from the exchange graph.
    // print edges_to_delete
    //std::cerr << "Edges to delete: ";
    //for (const auto& pair : edges_to_delete) {
    // std::cerr << pair.first << " , " << pair.second << " | ";
    //}
    //std::cerr << std::endl;

    // Remove the edges from the exchange graph.
    
    for (const auto& pair : edges_to_delete) {
      int edge = pair.first;
      assert(exchange_graph[tail].count(edge) > 0); // Ensure edge exists
      exchange_graph[tail].erase(edge);
      tail = pair.second; 
    }
  }

  // DEBUGGING: Print out the paths.
  //std::cerr << "Printing out the paths...: " << std::endl;
  //for (const auto& path : list_of_paths) {
  // std::cerr << "Path: ";
    //for (const auto& e : path) {
      //std::cerr << e << " ";
    //}
    //std::cerr << std::endl;
  //}

  // TESTING CODE: At the end, we can check that every vertex on the left hand side has in-degree = out-degree. That means all the sources are on the LHS.
  //std::cout << "Size of list_of_paths: " << list_of_paths.size() << std::endl;
  //std::cout << "Size of sources: " << sources.size() << std::endl;
  std::map<int, int> lhs_in_degree;
  for (int j = 0; j < matroid_a->GetGroupsNum(); j++) {
    int rhs_vertex = 2 * j + 2; 
    for (int e = 0; e < exchange_graph[rhs_vertex].size(); e++) {
      int lhs_vertex = exchange_graph[rhs_vertex][e];
      lhs_in_degree[lhs_vertex]++;
    }
  }

  for (int i = 0; i < fairness_constraints->GetColorNum(); i++) {
    int lhs_vertex = 2 * i + 1;
    int lhs_vertex_out_degree = exchange_graph[lhs_vertex].size();
    if (lhs_vertex_out_degree - lhs_in_degree[lhs_vertex] != 0) {
      std::cerr << "Exchange Graph Error: The in-degree and out-degree of this vertex are not equal." << std::endl;
    }
  }

  if (list_of_paths.size() != sources.size()) {
    std::cerr << "Exchange Graph Error: The number of paths found is not equal to the number of sources." << std::endl;
  }
  std::cout << "Testing Done" << std::endl;
  std::cerr << std::endl;

  return list_of_paths; 
}

void ApproximateFairSubmodularMaximization(PartitionMatroid* matroid_a, FairnessConstraint* fairness_constraints, SubmodularFunction* sub_func_f, float epsilon) {
  std::vector<int> set_P = FairMaxIntersection(matroid_a, fairness_constraints, sub_func_f->GetUniverse());
  //std::cerr << "set_P made" << std::endl;
  matroid_a->Reset();
  std::unique_ptr<Matroid> upper_matroid = fairness_constraints->UpperBoundsToMatroid();
  std::vector<int> set_Y = Greedy(matroid_a, upper_matroid.get(), sub_func_f, sub_func_f->GetUniverse());
  //std::cerr << "set_Y made" << std::endl;
  std::vector<std::vector<int>> paths = ReturnPaths(matroid_a, fairness_constraints, sub_func_f, set_Y, set_P);
  //std::cerr << "Paths made" << std::endl;

  matroid_a->Reset();
  fairness_constraints->Reset();
  sub_func_f->Reset();
  for (int i = 0; i < set_Y.size(); i++) {
    matroid_a->Add(set_Y[i]);
    fairness_constraints->Add(set_Y[i]);
    sub_func_f->Add(set_Y[i]);
  }
  assert(matroid_a->CurrentIsFeasible());
  assert(fairness_constraints->UpperBoundsToMatroid()->IsFeasible(matroid_a->GetCurrent()));
  // Randomly shuffle the paths.
  //std::cerr << "Initialized matroids to have set_Y." << std::endl;
  RandomHandler::Shuffle(paths);

  //print the elements in matroid_a
  std::cout << "Size of Greedy Algorithm: " << set_Y.size() << std::endl;
  //std::cerr << "set_Y: ";
  //for (int k = 0; k < set_Y.size(); k++) {
    //std::cerr << set_Y[k] << " "; 
  //}
  //std::cerr << std::endl;
  //std::cout << "Size of Maximum Fair Set: " << set_P.size() << std::endl;
  //std::cerr << "set_P: ";
  //for (int k = 0; k < set_P.size(); k++) {
    //std::cerr << set_P[k] << " "; 
  //}
  //std::cerr << std::endl;
  std::cout << "Is Greedy Fair?: " << fairness_constraints->IsFeasible(set_Y) << std::endl;
  //std::cerr << std::endl;
  int num_iterations = RandomHandler::RoundUpOrDown(paths.size() * (1 - epsilon));
  for (int i = 0; i < num_iterations; i++) {
    std::vector<int> path = paths[i];
    //std::cerr << "Path: ";
    //for (int k = 0; k < path.size(); k++) {
      //std::cerr << path[k] << " "; 
  //}
    //std::cerr << std::endl;
    for (int j = 0; j < path.size(); j++) {
      int element = path[j];
      if (j % 2 == 0) {
        matroid_a->Add(element);
        fairness_constraints->Add(element);
        sub_func_f->Add(element);
      }
      else {
        matroid_a->Remove(element);
        fairness_constraints->Remove(element);
        sub_func_f->Remove(element);
      }
    }
    // print the current solution in matroid_a. 
    //std::cout << "Current solution: "; 
    //for (int k = 0; k < matroid_a->GetCurrent().size(); k++) {
      //std::cout << matroid_a->GetCurrent()[k] << " ";
    //}
    if (!matroid_a->CurrentIsFeasible() || !fairness_constraints->UpperBoundsToMatroid()->IsFeasible(matroid_a->GetCurrent())) {
      std::cerr << "Error!! The current solution is not feasible." << std::endl;
    }
  }
  std::cout << "Size of algorithm's set: " << matroid_a->GetCurrent().size() << std::endl;
  std::cout << "Value of Greedy's set: " << sub_func_f->ObjectiveAndIncreaseOracleCall(set_Y) << std::endl;
  std::cout << "Value of algorithm's set: " << sub_func_f->ObjectiveAndIncreaseOracleCall(matroid_a->GetCurrent()) << std::endl;
  std::cout << "Value of Maximum Fair's set: " << sub_func_f->ObjectiveAndIncreaseOracleCall(set_P) << std::endl;
}
