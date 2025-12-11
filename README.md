# Improved Algorithms for Fair Matroid Submodular Maximization

This repository contains the code for the NeurIPS 2025 paper **"Improved Algorithms for Fair Matroid Submodular Maximization"**
by Sepideh Mahabadi, Sherry Sarkar and Jakub Tarnawski.

It includes the implementations for our algorithms,
all baselines, as well as the full experimental setup in a ready-to-run package.
Our algorithms are only implemented for the case of partition matroids (which enables better runtimes).

## How to use this code to replicate the paper's experiments

* Necessary dependencies: make, g++, Python 3.
Also, the [included movie dataset preparation script](https://github.com/dj3500/movielens-matrix-completion) will need Python packages: `scikit-learn`, `numpy`, `pandas`, and [`fancyimpute`](https://github.com/iskandr/fancyimpute) (this can be installed by running `pip install fancyimpute`). The plot-drawing script `plot.py` requires `numpy`, `pandas`, and `matplotlib` (and probably LaTeX somewhere on the system as well).

* Clone or download this repository to some directory and enter it.

* To build the C++ sources (including the dataset preprocessors), run:

```
make
```

* To download and preprocess the datasets, run:

```
python prepare_datasets.py
```

* To remove the ~2.7GB of unnecessary intermediate files, run:

```
python prepare_datasets.py --clean
```

* To execute the experiments, run:

```
./fair-submodular.exe --movies --coverage --clustering
```

(This will produce some output on stdout, as well as creating files in the `results` directory. The "f" values in the result files correspond to the submodular objective value, "rank" to the rank $k$ of the matroid, and "error" to the violation of the fairness constraint $\mathrm{err}(S)$.)

* To produce plots, run:

```
python plots/plot.py
```

(This ingests files from the `results` directory.)

The code has been tested on all of: Windows, Linux, and MacOS.

## License Information

This code is licensed under [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0).
The datasets used are:
[Pokec](https://snap.stanford.edu/data/soc-Pokec.html) (SNAP),
[Bank Marketing](https://archive.ics.uci.edu/ml/datasets/Bank+Marketing),
[MovieLens-1M](https://grouplens.org/datasets/movielens/1m/).
The SNAP datasets are under the BSD license; the full license, copyright information, and terms of usage can be at [https://snap.stanford.edu/snap/license.html](https://snap.stanford.edu/snap/license.html).
The Bank Marketing dataset is licensed under a Creative Commons Attribution 4.0 International (CC BY 4.0) license.
The full license, copyright information, and terms of usage of the MovieLens dataset may be found [here](https://files.grouplens.org/datasets/movielens/ml-1m-README.txt).

This code builds upon that for the ICML 2023 paper "Fairness in Streaming Submodular Maximization over a Matroid Constraint" by Marwa El Halabi, Federico Fusco, Ashkan Norouzi-Fard, Jakab Tardos and Jakub Tarnawski, which is also licensed under the Apache 2.0 license and can be found [here](https://github.com/dj3500/google-research/tree/master/fair_submodular_matroid).
