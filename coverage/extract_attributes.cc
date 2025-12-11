// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// This function splits std::string s by pattern pat
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
const int GENDER = 3;
const int AGE = 7;
const int HEIGHT_WIDTH = 8;

int main() {
  std::ifstream fin1("soc-pokec-profiles.txt", std::ios::binary);
  // std::ios::binary is needed to avoid issues on Windows,
  // as the input files seems to have weird characters (Unicode?)
  int cnt = 0;
  char ch;
  while (fin1.get(ch)) cnt += ch == '\n';
  std::cout << "Number of end-line = " << cnt << "." << std::endl;
  fin1.close();

  std::ifstream fin("soc-pokec-profiles.txt", std::ios::binary);
  std::ofstream fout("filtered-attributes.txt");

  int columns = -1;
  int processed = 0;
  std::string S;
  while (std::getline(fin, S)) {
    std::vector<std::string> attr = split(S, "\t");
    if (columns == -1) {
      columns = attr.size();
    } else {
      //if (columns != attr.size())
      //  std::cout << "columns = " << columns
      //            << ", but attr.size() = " << attr.size() << std::endl;
    }
    fout << attr[ID] << "\t" << attr[GENDER] << "\t" << attr[AGE] << "\t"
         << attr[HEIGHT_WIDTH] << std::endl;
    processed++;
  }

  std::cout << "columns = " << columns << "; processed = " << processed
            << std::endl;
  fout.close();
  fin.close();
  return 0;
}
