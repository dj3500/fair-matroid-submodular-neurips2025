// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include <fstream>
#include <string>
#include <utility>
#include <vector>

// this function splits string s by pattern pat
std::vector<std::string> split(std::string s, std::string pat) {
  std::vector<std::string> ret;
  ret.clear();
  int idx = 0, i = 0;
  while (idx < s.size()) {
    i = s.find(pat, idx);
    if (i == -1) break;
    if (i - idx) ret.push_back(s.substr(idx, i - idx));
    idx = i + pat.size();
  }
  if (idx < s.size()) ret.push_back(s.substr(idx));
  return ret;
}

// Extracts attributes that we care about.
// Description of attributes is given at
// https://snap.stanford.edu/data/soc-pokec-readme.txt
const int ID = 0;
const int AGE = 2;

void color_by_age(const std::vector<std::pair<int, int>> ranges) {
  std::vector<int> color(1 << 10, 0);
  int c = 0;
  for (auto &p : ranges) {
    for (int i = p.first; i <= p.second; i++) color[i] = c;
    c++;
  }

  std::ifstream fin("filtered-attributes.txt");
  std::ofstream fout("color_age_1.txt");

  std::string S;
  while (std::getline(fin, S)) {
    std::vector<std::string> attr = split(S, "\t");
    if (attr[AGE] == "null") attr[AGE] = "0";
    int age = std::stoi(attr[AGE]);
    fout << attr[ID] << " " << color[age] << std::endl;
  }

  fout.close();
  fin.close();
}

int main() {
  color_by_age(
      {{0, 0}, {1, 10}, {11, 17}, {18, 25}, {26, 35}, {36, 45}, {46, 1000}});
  return 0;
}
