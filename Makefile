# Make 'all' the default goal
.DEFAULT_GOAL := all

SRC_FILES := $(wildcard *.cc)
H_FILES := $(wildcard *.h)
CXXFLAGS := -O3 -W -Wall -Wshadow -Wno-unused-parameter -Wno-sign-compare -std=c++17
BIN := fair-submodular.exe

$(BIN): $(SRC_FILES) $(H_FILES)
		g++ $(CXXFLAGS) -o $@ $(SRC_FILES)

COVERAGE_PREPROCESSING_BINS := \
	coverage/clean_graph_for_bmi.exe \
	coverage/color_vertices.exe \
	coverage/extract_attributes.exe \
	coverage/statistics_bmi.exe

coverage/%.exe: coverage/%.cc
		g++ $(CXXFLAGS) -o $@ $<

CLUSTERING_PREPROCESSING_BIN := clustering/bank_input_converter_main.exe

$(CLUSTERING_PREPROCESSING_BIN): clustering/bank_input_converter_main.cc
		g++ $(CXXFLAGS) -o $@ $<

all: $(BIN) $(COVERAGE_PREPROCESSING_BINS) $(CLUSTERING_PREPROCESSING_BIN)
