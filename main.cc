// Copyright 2025 The Authors (see AUTHORS file)
// SPDX-License-Identifier: Apache-2.0

// Experiments for fair submodular maximization.

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

#include "algorithm.h"
#include "bank_data.h"
#include "lower_bound_matroid_intersection_algorithm.h"
#include "upper_bound_matroid_intersection_algorithm.h"
#include "approximate_fairness_algorithm.h"
#include "clustering_function.h"
#include "fairness_constraint.h"
#include "graph.h"
#include "graph_utility.h"
#include "laminar_matroid.h"
#include "matroid_intersection.h"
#include "movies_data.h"
#include "movies_mixed_utility_function.h"
#include "partition_matroid.h"
#include "submodular_function.h"
#include "two_pass_algorithm_with_conditioned_matroid.h"
#include "utilities.h"
#include "random_algorithm.h"

std::vector<int> Range(int n) {
  std::vector<int> range(n);
  for (int i = 0; i < n; ++i) {
    range[i] = i;
  }
  return range;
}

void PrintSolutionVector(const std::vector<int>& solution, std::ofstream& fout,
                         std::string alg_name, const int rank,
                         bool verbose = true) {
  if (verbose) {
    fout << "Solution for " << alg_name << " for rank = " << rank << std::endl;
  }
  for (auto& p : solution) {
    fout << p + 1 << " ";  // add 1 for matlab indexing
  }
  fout << std::endl;
}

bool FeasibleSolutionExists(std::unique_ptr<Matroid>& matroid,
                            FairnessConstraint& fairness,
                            const std::vector<int>& universe) {
  MaxIntersection(matroid.get(), fairness.LowerBoundsToMatroid().get(),
                  universe);
  std::vector<int> solution = matroid->GetCurrent();
  std::cout << "Is feasible: " << matroid->IsFeasible(solution) << " "
            << fairness.IsFeasible(solution) << std::endl;
  return fairness.IsFeasible(solution);
}

bool IsRandomized(Algorithm& alg) {
  return alg.GetAlgorithmName() == "Random algorithm" || 
         alg.GetAlgorithmName().find("Approximate Fairness") == 0;
}

// global parameters, set in main() from command line arguments
bool save_solutions = false;
int random_repeats = 10;

// GetSolutionValue() should always be called once, before GetSolutionVector()
void SingleKBaseExperiment(
    SubmodularFunction& f, const int rank,
    const std::unique_ptr<Matroid>& matroid, const FairnessConstraint& fairness,
    const std::vector<std::reference_wrapper<Algorithm>>& algorithms,
    std::vector<std::ofstream>& result_files,
    std::vector<std::ofstream>& solutions_files,
    std::ofstream& general_log_file) {
  int number_of_colors = fairness.GetColorNum();
  // Fixing the parameters.
  // Running all the algorithms. For the random algorithms, we repeat random_repeats times.
  for (int idx = 0; idx < algorithms.size(); idx++) {
    // Reset seed for fair comparison
    RandomHandler::generator_.seed(1);

    Algorithm& alg = algorithms[idx];
    std::ofstream& of = result_files[idx];

    of << rank << " ";
    int num_rep = 1;
    // For the random algorithms, we repeat random_repeats times.
    if (IsRandomized(alg)) {
      num_rep = random_repeats;
    }

    std::vector<double> values;
    std::vector<int> errors;
    std::vector<double> lb_ratios;
    
    for (int j = 0; j < num_rep; j++) {
      std::cout << "Now running " << alg.GetAlgorithmName()
                << " with rank=" << rank << "...\n";
      alg.Init(f, fairness, *matroid);
      const std::vector<int>& universe = f.GetUniverse();
      for (int i = 0; i < universe.size(); i++) {
        alg.Insert(universe[i]);
      }
      double solution_value = alg.GetSolutionValue();
      std::cout << "Value: " << solution_value << std::endl;
      values.push_back(solution_value);
      std::vector<int> solution = alg.GetSolutionVector();
      if (num_rep == 1) {
        PrintSolutionVector(solution, general_log_file, alg.GetAlgorithmName(),
                            rank);
      }
      if (save_solutions) {
        PrintSolutionVector(solution, solutions_files[idx],
                          alg.GetAlgorithmName(), rank, false);
      }

      std::vector<int> occurance(number_of_colors, 0);
      std::vector<std::pair<int, int>> bounds = fairness.GetBounds();
      for (int i = 0; i < solution.size(); i++) {
        occurance[fairness.GetColor(solution[i])]++;
      }
      int error = 0;
      double ratio = 1;
      std::cout << "Color distribution: ";
      for (int i = 0; i < occurance.size(); i++) {
        std::cout << occurance[i] << " ";
        error += std::max(0, occurance[i] - bounds[i].second);
        error += std::max(0, -occurance[i] + bounds[i].first);
        ratio = std::min(
            ratio, static_cast<double>(occurance[i]) / (bounds[i].first / 2));
      }
      std::cout << std::endl << "error :" << error << std::endl << std::endl;
      std::cout << "worst lower bound ratio :" << ratio << std::endl
                << std::endl;
      errors.push_back(error);
      lb_ratios.push_back(ratio);
    }
    // Computing average and variance.
    if (num_rep != 1) {
      double average_value = 0., average_error = 0., average_ratio = 0.;
      double var_value = 0., var_error = 0., var_ratio = 0.;
      for (auto& value : values) average_value += value;
      for (auto& error : errors) average_error += error;
      for (auto& ratio : lb_ratios) average_ratio += ratio;
      average_value /= values.size();
      average_error /= errors.size();
      average_ratio /= lb_ratios.size();
      for (auto& value : values)
        var_value += (value - average_value) * (value - average_value);
      double samplestddev_value = sqrt(var_value / (values.size() - 1));
      for (auto& error : errors)
        var_error += (error - average_error) * (error - average_error);
      double samplestddev_error = sqrt(var_error / (errors.size() - 1));
      for (auto& ratio : lb_ratios)
        var_ratio += (ratio - average_ratio) * (ratio - average_ratio);
      std::cout << "Average value: " << average_value << " "
                << samplestddev_value << std::endl;
      of << average_value << " ";
      general_log_file << "Variance in % for rank = " << rank << " "
                       << sqrt(var_value / (values.size() - 1)) / average_value
                       << std::endl;
      std::cout << "Average error: " << average_error << " "
                << samplestddev_error << std::endl;
      of << average_error << " ";
      std::cout << "Average ratio: " << average_ratio << " "
                << sqrt(var_ratio / (lb_ratios.size() - 1)) << std::endl;
      //of << average_ratio << " ";

      of << samplestddev_value << " ";
      of << samplestddev_error << " ";
    } else {
      of << values[0] << " ";
      of << errors[0] << " ";
      //of << lb_ratios[0] << " ";
    }
    of << f.oracle_calls_ << std::endl;
    f.oracle_calls_ = 0;
  }
}

void BaseExperiment(SubmodularFunction& f, std::vector<int>& ranks,
                    std::vector<std::unique_ptr<Matroid>>& matroids,
                    std::vector<FairnessConstraint>& fairness,
                    std::string exp_name) {
  // Algorithms to run
  //TwoPassAlgorithmWithConditionedMatroid two_pass_swap(false);
  TwoPassAlgorithmWithConditionedMatroid two_pass_greedy(true);
  //LowerBoundMatroidIntersectionAlgorithm lbalgo_fastgreedy(
  //    LowerBoundMatroidIntersectionAlgorithm::FAST_GREEDY);
  LowerBoundMatroidIntersectionAlgorithm lbalgo_greedy(
      LowerBoundMatroidIntersectionAlgorithm::GREEDY);
  UpperBoundMatroidIntersectionAlgorithm ubalgo_greedy(true);
  //UpperBoundMatroidIntersectionAlgorithm ubalgo_swap(false); 
  //ApproximateFairnessAlgorithm approx_fair_10(1.0);
  //ApproximateFairnessAlgorithm approx_fair_09(0.9);
  ApproximateFairnessAlgorithm approx_fair_08(0.8);
  //ApproximateFairnessAlgorithm approx_fair_07(0.7);
  //ApproximateFairnessAlgorithm approx_fair_06(0.6);
  ApproximateFairnessAlgorithm approx_fair_05(0.5);
  //ApproximateFairnessAlgorithm approx_fair_04(0.4);
  //ApproximateFairnessAlgorithm approx_fair_03(0.3);
  ApproximateFairnessAlgorithm approx_fair_02(0.2);
  //ApproximateFairnessAlgorithm approx_fair_01(0.1);
  //ApproximateFairnessAlgorithm approx_fair_00(0.0);
  RandomAlgorithm random;

  std::vector<std::reference_wrapper<Algorithm>> algorithms = {
    random,
    lbalgo_greedy,
    ubalgo_greedy,
    two_pass_greedy,
    approx_fair_08,
    approx_fair_05,
    approx_fair_02
  };

  // Create files to output results and save solution sets.
  std::vector<std::ofstream> result_files;
  std::vector<std::ofstream> solutions_files;
  std::string exp_base_path = "results/" + exp_name;

  for (Algorithm& alg : algorithms) {
    result_files.emplace_back(exp_base_path + "_" + alg.GetAlgorithmName() +
                              ".txt");
    const std::string csv_header_row = IsRandomized(alg)
        ? "rank f error stddev_f stddev_error OC"
        : "rank f error OC";
    result_files.back() << csv_header_row << std::endl;
    if (save_solutions) {
      solutions_files.emplace_back(exp_base_path + "_sols_" +
                                   alg.GetAlgorithmName() + ".txt");
    }
  }
  std::ofstream general_log_file(exp_base_path + "_general.txt");

  for (int i = 0; i < ranks.size(); i++) {
    // Skip rank if no feasible solution
    if (!FeasibleSolutionExists(matroids[i], fairness[i], f.GetUniverse())) {
      std::cerr << "No feasible solution for " << exp_name
                << " with rank = " << ranks[i] << std::endl;
      continue;
    }

    SingleKBaseExperiment(f, ranks[i], matroids[i], fairness[i], algorithms,
                          result_files, solutions_files, general_log_file);
  }
  for (std::ofstream& of : result_files) of.close();

  for (std::ofstream& of : solutions_files) of.close();

  general_log_file.close();
}

void ClusteringExperiment() {
  BankData data;
  ClusteringFunction f(data.input_);
  int ngrps = (int)data.balance_grpcards_.size();
  int ncolors = (int)data.age_grpcards_.size();

  std::vector<int> ranks;
  std::vector<std::unique_ptr<Matroid>> matroids;
  std::vector<FairnessConstraint> fairness;

  for (int i = 3; i <= 12; i++) {
    int rank = 5 * i;
    ranks.push_back(rank);
    std::vector<int> groups_bounds(ngrps, i);
    std::cout << "group bound: " << ngrps << " " << groups_bounds[0]
              << std::endl;
    matroids.emplace_back(
        new PartitionMatroid(data.balance_map_, groups_bounds));

    std::vector<std::pair<int, int>> color_bounds(ncolors, {i / 2 + 2, 2 * i});
    std::cout << "color bounds: " << color_bounds[0].first << " "
              << color_bounds[0].second << std::endl;
    fairness.emplace_back(data.age_map_, color_bounds);
  }
  std::cout << "ranks size " << ranks.size() << " " << ranks[0] << std::endl;
  BaseExperiment(f, ranks, matroids, fairness, "clustering");
}

void CoverageExperiment(int lower_i, int upper_i) {
  Graph graph("pokec_age_BMI");  // "pokec_BMI_age"
  GraphUtility f(graph);
  int n = graph.GetUniverseVertices().size();
  std::cout << "n = " << n << std::endl;
  int ncolors = graph.GetColorsCards().size();
  std::cout << "ncolors = " << ncolors << std::endl;
  int ngrps = graph.GetGroupsCards().size();
  std::cout << "ngrps = " << ngrps << std::endl;

  std::vector<int> ranks;
  std::vector<std::unique_ptr<Matroid>> matroids;
  std::vector<FairnessConstraint> fairness;
  constexpr double lower_coeff = 0.9, upper_coeff = 1.5;

  for (int i = lower_i; i <= upper_i; i++) {
    int rank = 10 * i;
    std::cout << "rank = " << rank << std::endl;
    ranks.push_back(rank);
    double bound;
    std::vector<int> groups_bounds;
    std::cout << "group bounds:" << std::endl;
    for (int card : graph.GetGroupsCards()) {
      groups_bounds.emplace_back(rank * static_cast<double>(card) / n + 0.999);
      std::cout << groups_bounds.back() << std::endl;
    }
    matroids.emplace_back(
        new PartitionMatroid(graph.GetGroupsMap(), groups_bounds));

    int lower_bd;
    std::vector<std::pair<int, int>> color_bounds;
    std::cout << "color bounds:" << std::endl;
    for (int j = 0; j < ncolors; j++) {
      bound = rank * static_cast<double>(graph.GetColorsCards()[j]) / n;
      lower_bd = (int)(lower_coeff * bound + 0.001);
      color_bounds.emplace_back(lower_bd, upper_coeff * bound + 0.999);

      std::cout << " " << color_bounds.back().first << " "
                << color_bounds.back().second << std::endl;
    }
    fairness.emplace_back(graph.GetColorsMap(), color_bounds);
  }
  BaseExperiment(
      f, ranks, matroids, fairness,
      "coverage");
}

void MovieExperiment() {
  bool laminar = false;
  // ApproximateFairnessAlgorithm only supports partition matroids!

  MoviesMixedUtilityFunction f(444, 0.85);
  std::vector<int> ranks;
  std::vector<std::unique_ptr<Matroid>> matroids;
  std::vector<FairnessConstraint> fairness;
  for (int r = 10; r <= 200; r += 10) {
    // r is kinda like rank, but not really
    ranks.push_back(r);
    std::cerr << std::endl << std::endl << "r = " << r << std::endl;

    std::cerr << "group (matroid) bounds:";
    std::vector<int> groups_bounds;
    for (double p :
         MoviesData::GetInstance().GetMovieYearBandBoundPercentages()) {
      groups_bounds.push_back(r * p + 0.999);
      std::cerr << " " << groups_bounds.back();
    }
    if (!laminar) {
      matroids.emplace_back(new PartitionMatroid(
          MoviesData::GetInstance().GetMovieIdToYearBandMap(), groups_bounds));
    } else {
      const int noYearBands = groups_bounds.size();
      // small groups: 0 .. noYearBands-1
      // large groups: noYearBands onwards (one large group consists of L small
      // groups)
      constexpr int L = 3;
      std::map<int, std::vector<int>> group_map;
      for (const auto& p :
           MoviesData::GetInstance().GetMovieIdToYearBandMap()) {
        // p == {element, its small group id}
        group_map[p.first] = {p.second, noYearBands + p.second / L};
      }
      std::cerr << " |";
      for (int gr = 0; gr < noYearBands; gr += L) {
        // make new large group
        int sumOfGroupBounds = 0;
        for (int g = gr; g < noYearBands && g < gr + L; ++g) {
          sumOfGroupBounds += groups_bounds[g];
        }
        groups_bounds.push_back(0.8 * sumOfGroupBounds + 0.999);
        std::cerr << " " << groups_bounds.back();
      }
      matroids.emplace_back(new LaminarMatroid(group_map, groups_bounds));
    }

    std::cerr << std::endl;

    std::cerr << "color bounds:";
    std::vector<std::pair<int, int>> color_bounds;
    for (const std::pair<double, double>& p :
         MoviesData::GetInstance().GetMovieGenreBoundPercentages()) {
      color_bounds.emplace_back(r * p.first + 0.001, r * p.second + 0.999);
      std::cerr << " " << color_bounds.back().first << "-"
                << color_bounds.back().second;
    }
    fairness.emplace_back(MoviesData::GetInstance().GetMovieIdToGenreIdMap(),
                          color_bounds);
    std::cerr << std::endl;
  }
  std::string is_laminar = laminar ? "_laminar" : "";
  BaseExperiment(f, ranks, matroids, fairness,
                 "movies_exp_444_0.85" + is_laminar);
}

void Test_FairMaxIntersection(int lower_i, int upper_i) {
  BankData data;
  ClusteringFunction f(data.input_);

  std::unique_ptr<Matroid> matroid = std::make_unique<PartitionMatroid>(data.balance_map_, std::vector<int>{10, 10, 10, 10, 10});
  FairnessConstraint fairness(data.age_map_, {{1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}});
  
  FairMaxIntersection(matroid.get(), &fairness, f.GetUniverse());
}

void Test_Greedy() {
  // Test the Greedy algorithm with an example. 
  std::cout << "Testing Greedy..." << std::endl;
  BankData data;
  ClusteringFunction f(data.input_);

  std::unique_ptr<Matroid> matroid = std::make_unique<PartitionMatroid>(data.balance_map_, std::vector<int>{10, 10, 10, 10, 10});
  FairnessConstraint fairness(data.age_map_, {{1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}});
  std::unique_ptr<Matroid> upper_matroid = fairness.UpperBoundsToMatroid();

  std::vector<int> current_elements = Greedy(matroid.get(), upper_matroid.get(), &f, f.GetUniverse());
  std::cout << "Greedy took the following elements: ";
  for (const int& element : current_elements) {
    std::cout << element << " ";
  }
  std::cout << std::endl;
}

void Test_Paths(){
  // Test the ReturnPaths function with an example. 
  std::cout << "Testing ReturnPaths..." << std::endl;
  BankData data;
  ClusteringFunction f(data.input_);

  std::unique_ptr<PartitionMatroid> matroid = std::make_unique<PartitionMatroid>(data.balance_map_, std::vector<int>{10, 10, 10, 10, 10});
  FairnessConstraint fairness(data.age_map_, {{1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}});
  std::unique_ptr<Matroid> upper_matroid = fairness.UpperBoundsToMatroid();

  Greedy(matroid.get(), upper_matroid.get(), &f, f.GetUniverse());
  std::vector<int> set_Y = matroid.get()->GetCurrent();

  FairMaxIntersection(matroid.get(), &fairness, f.GetUniverse());
  std::vector<int> set_P = matroid.get()->GetCurrent();

  ReturnPaths(matroid.get(), &fairness, &f, set_Y, set_P);
}

void Test_ApproximateFairSubmodularMaximization(){
  // Test the ApproximateFairSubmodularMaximization function with an example. 
  std::cout << "Testing ApproximateFairSubmodularMaximization...." << std::endl;
  BankData data;
  ClusteringFunction f(data.input_);

  std::unique_ptr<PartitionMatroid> matroid = std::make_unique<PartitionMatroid>(data.balance_map_, std::vector<int>{5, 5, 5, 5, 5});
  FairnessConstraint fairness(data.age_map_, {{4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 10}});

  ApproximateFairSubmodularMaximization(matroid.get(), &fairness, &f, 0.2);
}

int main(int argc, char* argv[]) {
  //Test_FairMaxIntersection(1, 1);
  //Test_Greedy();
  //Test_ApproximateFairSubmodularMaximization();

  bool run_movies = false;
  bool run_coverage = false;
  bool run_clustering = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--movies") {
      run_movies = true;
    } else if (arg == "--coverage") {
      run_coverage = true;
    } else if (arg == "--clustering") {
      run_clustering = true;
    } else if (arg == "--save-solutions") {
      save_solutions = true;
    } else if (arg == "--random-repeats" && i + 1 < argc) {
      random_repeats = std::stoi(argv[++i]);
    }
  }

  if (run_clustering) {
    ClusteringExperiment();
  }
  if (run_movies) {
    MovieExperiment();
  }
  if (run_coverage) {
    CoverageExperiment(1, 10);
  }
  if (!run_movies && !run_coverage && !run_clustering) {
    std::cout << "Usage: " << argv[0] << " [--movies] [--coverage] [--clustering] [--save-solutions] [--random-repeats N]" << std::endl;
  }
  return 0;
}
