# Copyright 2025 The Authors (see AUTHORS file)
# SPDX-License-Identifier: Apache-2.0

import os
import re
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np

cwd = os.path.abspath(os.getcwd())
if os.path.basename(cwd) == 'plots':
    RESULTS_DIR = os.path.abspath(os.path.join(cwd, '..', 'results'))
else:
    RESULTS_DIR = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'results')

EXPERIMENTS = ['clustering', 'coverage', 'movies_exp_444_0.85']

# Map experiment to output plot base name
PLOT_NAMES = {
    'clustering': 'clustering',
    'coverage': 'coverage',
    'movies_exp_444_0.85': 'movies',
}

# Set shorthand legend names for each algorithm.
# Also used to select which algorithms to plot.
ALGORITHM_LEGEND_NAME = {
    #'Approximate Fairness Algorithm (epsilon=0.000000)': r'Our ($\varepsilon=0$)',
    #'Approximate Fairness Algorithm (epsilon=0.100000)': r'Our ($\varepsilon=0.1$)',
    'Approximate Fairness Algorithm (epsilon=0.200000)': r'Our ($\varepsilon=0.2$)',
    #'Approximate Fairness Algorithm (epsilon=0.300000)': r'Our ($\varepsilon=0.3$)',
    #'Approximate Fairness Algorithm (epsilon=0.400000)': r'Our ($\varepsilon=0.4$)',
    'Approximate Fairness Algorithm (epsilon=0.500000)': r'Our ($\varepsilon=0.5$)',
    #'Approximate Fairness Algorithm (epsilon=0.600000)': r'Our ($\varepsilon=0.6$)',
    #'Approximate Fairness Algorithm (epsilon=0.700000)': r'Our ($\varepsilon=0.7$)',
    'Approximate Fairness Algorithm (epsilon=0.800000)': r'Our ($\varepsilon=0.8$)',
    #'Approximate Fairness Algorithm (epsilon=0.900000)': r'Our ($\varepsilon=0.9$)',
    #'Approximate Fairness Algorithm (epsilon=1.000000)': r'Our ($\varepsilon=1.0$)',
    #'Lower bound matroid intersection algorithm (postprocessing=FAST_GREEDY)': 'LBMI (FastGreedy)',
    'Lower bound matroid intersection algorithm (postprocessing=GREEDY)': 'LBMI',
    'Random algorithm': 'Random',
    #'Upper bound matroid intersection algorithm (swapping)': 'UB MI (Swap)',
    'Upper bound matroid intersection algorithm (greedy)': 'UBMI',
    #'Two pass algorithm (swapping)': 'Two Pass (Swap)',
    'Two pass algorithm (greedy)': 'TwoPass',
}

# Set font to Times (NeurIPS/LaTeX style)
plt.rcParams.update({
    'font.family': 'serif',
    'font.serif': ['Times New Roman', 'Times', 'DejaVu Serif'],
    'text.usetex': True,
    'axes.titlesize': 14,
    'axes.labelsize': 14,
    'legend.fontsize': 12,
    'xtick.labelsize': 12,
    'ytick.labelsize': 12,
})

DEBUG = False  # Set to True for verbose output
GENERATE_MAIN_GRID_SMALL = True  # Set to True to generate the main 2 row x 3 column grid PDF (small_plots_grid.pdf)
GENERATE_MAIN_GRID_LARGE = True  # Set to True to generate the main 3 row x 2 column grid PDF (large_plots_grid.pdf)
GENERATE_SCATTER_GRID = True  # Set to True to generate the scatter 1 row x 3 column grid PDF (scatter_grid.pdf)
SHOW_PARETO_FRONTIER = True  # Set to False to hide the Pareto frontier in scatter plots

xlabel = r'$r$'
ylabel_fairviol = r'Fairness violation fav$(S)$'
ylabel_objval = r'Objective value $f(S)$'

def parse_result_file(filepath):
    with open(filepath, 'r') as f:
        header = f.readline().strip().split()
        df = pd.read_csv(filepath, sep='\\s+', comment='/', skiprows=1, names=header)
    return df

def get_algorithm_name(filename, experiment):
    # Remove experiment prefix and .txt suffix
    name = filename[len(experiment)+1:]
    if name.endswith('.txt'):
        name = name[:-4]
    return name

def get_asymmetric_yerr(y, yerr):
    if yerr is not None:
        lower = y - np.maximum(y - yerr, 0)
        upper = yerr
        return np.array([lower, upper])
    else:
        return yerr

def all_plots():
    cwd = os.path.abspath(os.getcwd())
    plots_dir = cwd if os.path.basename(cwd) == 'plots' else os.path.join(cwd, 'plots')
    nrows = 3
    ncols = 3
    experiments = ['coverage', 'clustering', 'movies_exp_444_0.85']
    plot_titles = [
        '(a) Coverage: Objective value',
        '(b) Clustering: Objective value',
        '(c) Movies: Objective value',
        '(d) Coverage: Total fairness violation',
        '(e) Clustering: Total fairness violation',
        '(f) Movies: Total fairness violation',
    ]
    # Load all data for each experiment (shared for both layouts)
    all_alg_dfs = []
    for exp in experiments:
        files = [f for f in sorted(os.listdir(RESULTS_DIR))
                 if f.startswith(exp+'_') and f.endswith('.txt') and not f.endswith('_general.txt')]
        alg_dfs = {}
        for fname in files:
            alg_name = get_algorithm_name(fname, exp)
            if alg_name not in ALGORITHM_LEGEND_NAME:
                continue
            df = parse_result_file(os.path.join(RESULTS_DIR, fname))
            alg_dfs[alg_name] = df
        all_alg_dfs.append(alg_dfs)
    jitter_step = 0.35

    if GENERATE_MAIN_GRID_SMALL:
        nrows = 2
        ncols = 3
        fig, axes = plt.subplots(nrows, ncols, figsize=(15, 8))
        plt.subplots_adjust(hspace=0.5, wspace=-0.5, top=0.97, bottom=0.07)  # increased hspace, decreased wspace
        # First row: objective value, Second row: fairness violation
        for col, (alg_dfs, exp) in enumerate(zip(all_alg_dfs, experiments)):
            # Objective value plot (row 0)
            ax = axes[0, col]
            ax.set_box_aspect(5/6)
            n_algs = len(alg_dfs)
            for i, (alg, df) in enumerate(alg_dfs.items()):
                x = df['rank'] + (i - n_algs/2) * jitter_step
                y = df['f']
                yerr = df['stddev_f'] if 'stddev_f' in df.columns else None
                yerr_asym = get_asymmetric_yerr(y, yerr)
                legend_name = ALGORITHM_LEGEND_NAME.get(alg, alg)
                eb = ax.errorbar(x, y, yerr=yerr_asym, label=legend_name, fmt='-o', capsize=3)
                if hasattr(eb, 'lines'):
                    eb.lines[0].set_linewidth(2)
            # Set only experiment name as title for (a), (b), (c)
            exp_titles = ['Coverage', 'Clustering', 'Movies']
            ax.set_xlabel(xlabel, fontsize=20)
            if col == 0:
                ax.set_ylabel(ylabel_objval, fontsize=22)
            else:
                ax.set_ylabel('')
            ax.set_title(exp_titles[col], fontsize=24)
            ax.tick_params(axis='both', which='major', labelsize=18)
            ax.ticklabel_format(style='sci', axis='y', scilimits=(0,0))
            ax.yaxis.get_offset_text().set_fontsize(16)
            # Only show legend in the rightmost objective value plot (Movies)
            if col == 2:
                ax.legend(fontsize=18, loc='best', framealpha=0.85)
            else:
                if ax.get_legend() is not None:
                    ax.get_legend().remove()
            # Fairness violation plot (row 1)
            ax = axes[1, col]
            ax.set_box_aspect(5/6)
            n_algs = len(alg_dfs)
            for i, (alg, df) in enumerate(alg_dfs.items()):
                x = df['rank'] + (i - n_algs/2) * jitter_step
                y = df['error']
                yerr = df['stddev_error'] if 'stddev_error' in df.columns else None
                yerr_asym = get_asymmetric_yerr(y, yerr)
                legend_name = ALGORITHM_LEGEND_NAME.get(alg, alg)
                eb = ax.errorbar(x, y, yerr=yerr_asym, label=legend_name, fmt='-o', capsize=3)
                if hasattr(eb, 'lines'):
                    eb.lines[0].set_linewidth(2)
            # No title for (d), (e), (f)
            ax.set_xlabel(xlabel, fontsize=20)
            if col == 0:
                ax.set_ylabel(ylabel_fairviol, fontsize=22)
            else:
                ax.set_ylabel('')
            ax.set_title('', fontsize=24)
            ax.tick_params(axis='both', which='major', labelsize=18)
            ax.ticklabel_format(style='plain', axis='y')
            ax.yaxis.get_offset_text().set_fontsize(16)
            # Remove legend from all error plots (second row)
            if ax.get_legend() is not None:
                ax.get_legend().remove()
        plt.tight_layout(pad=0.1)
        outpath = os.path.join(plots_dir, 'small_plots_grid.pdf')
        with PdfPages(outpath) as pdf:
            pdf.savefig(fig, bbox_inches='tight')
        plt.close(fig)
        print(f"Saved combined small grid PDF: {outpath}")
    
    if GENERATE_MAIN_GRID_LARGE:
        nrows = 3
        ncols = 2
        fig, axes = plt.subplots(nrows, ncols, figsize=(12, 15))
        plt.subplots_adjust(hspace=0.32, wspace=0.12, top=0.97, bottom=0.07)  # Increased hspace for more vertical space
        row_major_letters = ['(a)', '(b)', '(c)', '(d)', '(e)', '(f)']
        for row, (alg_dfs, exp) in enumerate(zip(all_alg_dfs, experiments)):
            # f plot
            ax = axes[row, 0]
            ax.set_box_aspect(5/6)
            n_algs = len(alg_dfs)
            for i, (alg, df) in enumerate(alg_dfs.items()):
                x = df['rank'] + (i - n_algs/2) * jitter_step
                y = df['f']
                yerr = df['stddev_f'] if 'stddev_f' in df.columns else None
                yerr_asym = get_asymmetric_yerr(y, yerr)
                legend_name = ALGORITHM_LEGEND_NAME.get(alg, alg)
                eb = ax.errorbar(x, y, yerr=yerr_asym, label=legend_name, fmt='-o', capsize=3)
                if hasattr(eb, 'lines'):
                    eb.lines[0].set_linewidth(2)
            orig_title = plot_titles[row]
            new_title = f"{row_major_letters[row*2]}{orig_title[3:]}"
            ax.set_xlabel(xlabel, fontsize=20)
            ax.set_ylabel(ylabel_objval, fontsize=20)
            ax.set_title(new_title, fontsize=20)
            ax.tick_params(axis='both', which='major', labelsize=18)
            ax.ticklabel_format(style='sci', axis='y', scilimits=(0,0))
            ax.yaxis.get_offset_text().set_fontsize(16)
            # error plot
            ax = axes[row, 1]
            ax.set_box_aspect(5/6)
            n_algs = len(alg_dfs)
            for i, (alg, df) in enumerate(alg_dfs.items()):
                x = df['rank'] + (i - n_algs/2) * jitter_step
                y = df['error']
                yerr = df['stddev_error'] if 'stddev_error' in df.columns else None
                yerr_asym = get_asymmetric_yerr(y, yerr)
                legend_name = ALGORITHM_LEGEND_NAME.get(alg, alg)
                eb = ax.errorbar(x, y, yerr=yerr_asym, label=legend_name, fmt='-o', capsize=3)
                if hasattr(eb, 'lines'):
                    eb.lines[0].set_linewidth(2)
            orig_title = plot_titles[row+3]
            new_title = f"{row_major_letters[row*2+1]}{orig_title[3:]}"
            ax.set_xlabel(xlabel, fontsize=20)
            ax.set_ylabel(ylabel_fairviol, fontsize=20)
            ax.set_title(new_title, fontsize=20)
            ax.tick_params(axis='both', which='major', labelsize=18)
            ax.ticklabel_format(style='plain', axis='y')
            ax.yaxis.get_offset_text().set_fontsize(16)
            if row == 2:
                ax = axes[2, 0]
                ax.legend(fontsize=18, loc='best', framealpha=0.5)
            else:
                if ax.get_legend() is not None:
                    ax.get_legend().remove()
        plt.tight_layout(pad=0.1)
        outpath = os.path.join(plots_dir, 'large_plots_grid.pdf')
        with PdfPages(outpath) as pdf:
            pdf.savefig(fig, bbox_inches='tight')
        plt.close(fig)
        print(f"Saved combined large grid PDF: {outpath}")
    
    if GENERATE_SCATTER_GRID:
        fig, axes = plt.subplots(1, 3, figsize=(12, 5))
        scatter_titles = ['(a) Coverage', '(b) Clustering', '(c) Movies']
        for col, (alg_dfs, exp, title) in enumerate(zip(all_alg_dfs, experiments, scatter_titles)):
            ax = axes[col]
            n_algs = len(alg_dfs)
            points = []
            for i, (alg, df) in enumerate(alg_dfs.items()):
                # Compute mean and stddev over all r
                fval = df['f'].mean()
                errval = df['error'].mean()
                std_f = df['stddev_f'].mean() if 'stddev_f' in df.columns else 0
                std_e = df['stddev_error'].mean() if 'stddev_error' in df.columns else 0
                color = None
                eb = ax.errorbar([], [], label=ALGORITHM_LEGEND_NAME.get(alg, alg), fmt='-o')
                if hasattr(eb, 'lines'):
                    color = eb.lines[0].get_color()
                ax.scatter(errval, fval, color=color, s=80, label=None, zorder=3)
                # Draw rectangle for stddevs if available
                if std_f > 0 and std_e > 0:
                    rect = plt.Rectangle((errval-std_e, fval-std_f), 2*std_e, 2*std_f, color=color, alpha=0.3, zorder=2)
                    ax.add_patch(rect)
                points.append((errval, fval))
            # Compute Pareto frontier (min error, max f)
            if points and SHOW_PARETO_FRONTIER:
                points = sorted(points, key=lambda x: (x[0], -x[1]))
                pareto = []
                max_f = -float('inf')
                for err, f in points:
                    if f > max_f:
                        pareto.append((err, f))
                        max_f = f
                pareto = np.array(pareto)
                ax.plot(pareto[:,0], pareto[:,1], '--', color='black')
                if col == 0 and len(pareto) > 1:
                    mid_idx = len(pareto) // 2
                    ax.text(pareto[mid_idx,0], pareto[mid_idx,1] - 0.05 * (ax.get_ylim()[1] - ax.get_ylim()[0]),
                            'Pareto frontier', fontsize=14, color='black', ha='left', va='top', rotation=0, zorder=10)
            ax.set_xlabel(ylabel_fairviol, fontsize=20)
            if col == 0:
                ax.set_ylabel(ylabel_objval, fontsize=20)
            else:
                ax.set_ylabel('')
            ax.set_title(title, fontsize=20)
            ax.tick_params(axis='both', which='major', labelsize=18)
            if col == 2:
                ax.legend(fontsize=18, loc='best', framealpha=0.7)
            else:
                if ax.get_legend() is not None:
                    ax.get_legend().remove()
        plt.tight_layout()
        outpath = os.path.join(plots_dir, 'scatter_grid.pdf')
        with PdfPages(outpath) as pdf:
            pdf.savefig(fig, bbox_inches='tight')
        plt.close(fig)
        print(f"Saved scatter grid PDF: {outpath}")

if __name__ == '__main__':
    all_plots()
