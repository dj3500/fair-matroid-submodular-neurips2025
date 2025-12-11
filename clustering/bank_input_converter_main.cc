// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// Converts the format of the bank dataset to the desired one.
int main() {
  std::ifstream fin("bank.csv");
  if (!fin) {
    std::cerr << "Failed to open bank.csv for reading\n";
    return 1;
  }
  std::ofstream fout("bank_output.txt");
  if (!fout) {
    std::cerr << "Failed to open bank_output.txt for writing\n";
    return 1;
  }

  std::vector<std::vector<double>> a;
  std::string s;
  fin >> s;
  int age;
  while (fin >> age) {
    a.push_back(std::vector<double>());
    a[a.size() - 1].push_back(age);
    char ch;
    int counter = 0;
    while (counter < 5) {
      fin >> ch;
      if (ch == ';') counter++;
    }
    int b;
    fin >> b;
    a[a.size() - 1].push_back(b);
    counter = 0;
    while (counter < 4) {
      fin >> ch;
      if (ch == ';') counter++;
    }
    fin >> b;
    a[a.size() - 1].push_back(b);
    counter = 0;
    while (counter < 2) {
      fin >> ch;
      if (ch == ';') counter++;
    }
    fin >> b;
    a[a.size() - 1].push_back(b);
    fin >> ch >> b;
    a[a.size() - 1].push_back(b);
    fin >> ch >> b;
    a[a.size() - 1].push_back(b);
    fin >> ch >> b;
    a[a.size() - 1].push_back(b);
    std::getline(fin, s);
  }
  if (a.empty()) {
    std::cerr << "No data parsed from bank.csv\n";
    return 1;
  }
  fout << a.size() << " " << a[0].size() << std::endl;
  for (int i = 0; i < static_cast<int>(a.size()); i++) {
    for (int j = 0; j < static_cast<int>(a[i].size()); j++) fout << a[i][j] << " ";
    fout << std::endl;
  }
  return 0;
}
