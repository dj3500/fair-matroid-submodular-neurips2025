// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#ifndef FAIR_SUBMODULAR_MATROID_GRAPH_H_
#define FAIR_SUBMODULAR_MATROID_GRAPH_H_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>


class Graph {
 public:
  // Forbids copying.
  Graph(const Graph&) = delete;
  Graph& operator=(const Graph&) = delete;
  // Allow moving.
  Graph(Graph&&) = default;

  explicit Graph(const std::string& name);

  // Returns the graph.
  static Graph& GetGraph(const std::string& name) {
    static auto* const name_to_graph = new std::map<std::string, Graph>();
    if (!name_to_graph->count(name)) {
      name_to_graph->emplace(name, Graph(name));
    }
    return name_to_graph->at(name);
  }

  // Returns the coverable vertices, i.e., the vertices with an ingoing edge.
  const std::vector<int>& GetCoverableVertices() const;

  // Returns the all the vertices that have an outgoing edge.
  const std::vector<int>& GetUniverseVertices() const;

  // Returns the list of neighbors of a vertex.
  const std::vector<int>& GetNeighbors(int vertex_i) const;

  // Returns the name of the graph.
  const std::string& GetName() const;

  // Returns the number of vertices of each color.
  const std::vector<int>& GetColorsCards() const;

  // Returns the number of vertices of each group.
  const std::vector<int>& GetGroupsCards() const;

  // Returns the map from vertices to colors.
  const std::map<int, int>& GetColorsMap() const;

  // Returns the map from vertices to groups.
  const std::map<int, int>& GetGroupsMap() const;

 private:
  // Name of dataset.
  const std::string name_;
  // Number of edges in the graph.
  int64_t num_edges_;
  // Number of vertices in the graph.
  int num_vertices_;
  // Number of colors.
  int num_colors_;
  // Number of groups.
  int num_groups_;

  // Number of vertices of each color.
  std::vector<int> colors_cards_;
  // Number of vertices in each group.
  std::vector<int> groups_cards_;
  // Map from vertices to colors.
  std::map<int, int> colors_map_;
  // Map from vertices to groups.
  std::map<int, int> groups_map_;

  // Neighbors[i] = list of i's neighbors.
  std::vector<std::vector<int>> neighbors_;

  // Those that have an edge out of them (the universe of f).
  std::vector<int> left_vertices_;

  // Those that have an edge to them (what we're covering).
  // If graph is bipartite, these are disjoint.
  // If graph is non-bipartite, these are (usually/ideally) equal.
  std::vector<int> right_vertices_;
};

#endif  // FAIR_SUBMODULAR_MATROID_GRAPH_H_
