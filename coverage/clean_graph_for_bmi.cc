// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include <fstream>
#include <iostream>
#include <unordered_map>

int main() {
  std::ifstream fin_colors("color-BMI.txt");
  int processed = 0;
  int u, v, c;
  std::unordered_map<int, int> map_v_to_c;
  while (fin_colors >> v >> c) {
    map_v_to_c[v] = c;
    processed++;
  }

  fin_colors.close();
  std::cout << "processed = " << processed << std::endl;
  // processed = 582319

  std::ifstream fin_edges("soc-pokec-relationships.txt");
  std::ofstream fout_edges_bmi("BMI-soc-pokec-relationships.txt");
  processed = 0;
  while (fin_edges >> u >> v) {
    if (!map_v_to_c.count(u) || !map_v_to_c.count(v)) continue;
    fout_edges_bmi << u << " " << v << std::endl;
    processed++;
  }
  std::cout << "processed = " << processed << std::endl;
  // processed = 5834695
  fout_edges_bmi.close();
  fin_edges.close();
  return 0;
}
