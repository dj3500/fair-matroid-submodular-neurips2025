# Copyright 2025 The Authors (see AUTHORS file)
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import urllib.request
import shutil
import subprocess
from pathlib import Path
import ssl
import zipfile

DATASET_BASE_URL = "https://snap.stanford.edu/data/"
FILES_TO_DOWNLOAD = [
    "soc-pokec-relationships.txt.gz",
    "soc-pokec-profiles.txt.gz",
]
COVERAGE_DIR = Path("coverage")

# (exe_path, expected_output)
PREPROCESSING_STEPS = [
    # all of these expect the input files to be in coverage/ and the
    # current working directory to be coverage/
    (COVERAGE_DIR / "extract_attributes.exe", COVERAGE_DIR / "filtered-attributes.txt"),
    (COVERAGE_DIR / "color_vertices.exe", COVERAGE_DIR / "color_age_1.txt"),
    (COVERAGE_DIR / "statistics_bmi.exe", COVERAGE_DIR / "color-BMI.txt"),
    (COVERAGE_DIR / "clean_graph_for_bmi.exe", COVERAGE_DIR / "BMI-soc-pokec-relationships.txt"),
]

# --- SSL context that ignores certificate verification (to avoid CERTIFICATE_VERIFY_FAILED) ---
UNVERIFIED_SSL_CONTEXT = ssl.create_default_context()
UNVERIFIED_SSL_CONTEXT.check_hostname = False
UNVERIFIED_SSL_CONTEXT.verify_mode = ssl.CERT_NONE

CLUSTERING_DIR = Path("clustering")
BANK_ZIP_URL = "https://archive.ics.uci.edu/static/public/222/bank+marketing.zip"
BANK_ZIP_NAME = "bank_marketing.zip"  # local filename
BANK_ZIP_PATH = CLUSTERING_DIR / BANK_ZIP_NAME
BANK_INNER_ZIP_NAME = "bank.zip"
BANK_CSV_NAME = "bank.csv"
BANK_OUTPUT = CLUSTERING_DIR / "bank_output.txt"
BANK_CONVERTER_EXE = CLUSTERING_DIR / "bank_input_converter_main.exe"

MOVIES_DIR = Path("movies")
MOVIES_PREPARE_SCRIPT = MOVIES_DIR / "prepare_movies.py"
MOVIES_ZIP_URL = "https://files.grouplens.org/datasets/movielens/ml-1m.zip"
MOVIES_ZIP_NAME = "ml-1m.zip"
MOVIES_ZIP_PATH = MOVIES_DIR / MOVIES_ZIP_NAME
# paths where we want the extracted files to end up (directly under movies/)
MOVIES_DAT = MOVIES_DIR / "movies.dat"
MOVIES_RATINGS = MOVIES_DIR / "ratings.dat"


def _print_progress(prefix: str, downloaded: int, total: int | None) -> None:
    if total and total > 0:
        frac = downloaded / total
        bar_len = 40
        filled = int(bar_len * frac)
        bar = "#" * filled + "-" * (bar_len - filled)
        pct = int(frac * 100)
        msg = f"\r{prefix} [{bar}] {pct:3d}% ({downloaded / (1024*1024):.1f} / {total / (1024*1024):.1f} MB)"
    else:
        msg = f"\r{prefix} {downloaded / (1024*1024):.1f} MB"
    print(msg, end="", flush=True)


def download_file(url: str, dest: Path) -> None:
    dest.parent.mkdir(parents=True, exist_ok=True)
    print(f"Downloading {url} -> {dest}")
    req = urllib.request.Request(url)
    with urllib.request.urlopen(req, context=UNVERIFIED_SSL_CONTEXT) as response, open(dest, "wb") as out_file:
        total_len_str = response.getheader("Content-Length")
        total_len = int(total_len_str) if total_len_str is not None else None
        downloaded = 0
        chunk_size = 1024 * 64
        while True:
            chunk = response.read(chunk_size)
            if not chunk:
                break
            out_file.write(chunk)
            downloaded += len(chunk)
            _print_progress("  Progress", downloaded, total_len)
    print()  # newline after progress bar
    print(f"  Done: {dest}")


def gunzip_file(gz_path: Path, dst_path: Path) -> None:
    """Decompress a .gz file into dst_path using gzip module."""
    import gzip

    print(f"Decompressing {gz_path} -> {dst_path}")
    with gzip.open(gz_path, "rb") as f_in, open(dst_path, "wb") as f_out:
        shutil.copyfileobj(f_in, f_out)
    print(f"  Done: {dst_path}")


def run_executable(exe_path: Path) -> int:
    """
    Run a local executable in a cross-platform way.

    - Uses an absolute path to avoid resolution issues on Linux.
    - Sets cwd to the executable's directory so that any relative
      file paths used inside the binary match the layout of this
      project (e.g., coverage/ or clustering/).
    """
    exe_path = exe_path.resolve()

    if not exe_path.exists():
        print(f"Executable not found: {exe_path}")
        print("Run `make` first before running this script.")
        return 1

    # On non-Windows systems, just check that it is executable.
    if os.name != "nt" and not os.access(exe_path, os.X_OK):
        print(f"{exe_path} is not executable (missing +x permission).")
        print("Please run `chmod +x` on the binary and try again.")
        return 1

    exe_dir = exe_path if exe_path.is_dir() else exe_path.parent
    print(f"Running {exe_path} with cwd={exe_dir} ...")
    try:
        result = subprocess.run(
            [str(exe_path)],
            cwd=str(exe_dir),
            check=False,
        )
        return result.returncode
    except OSError as e:
        print(f"  Failed to run {exe_path}: {e}")
        return 1


def clean_coverage_intermediates() -> None:
    print("\n[1.C] Cleaning up temporary coverage files")
    print("-" * 60)

    # Remove downloaded .gz files
    for gz_name in FILES_TO_DOWNLOAD:
        gz_path = COVERAGE_DIR / gz_name
        if gz_path.exists():
            try:
                gz_path.unlink()
                print(f"[1.C] Removed {gz_path}")
            except OSError as e:
                print(f"[1.C] Could not remove {gz_path}: {e}")

    # Remove uncompressed raw txt files and intermediate filtered-attributes.txt
    to_remove = [
        COVERAGE_DIR / "soc-pokec-relationships.txt",
        COVERAGE_DIR / "soc-pokec-profiles.txt",
        COVERAGE_DIR / "filtered-attributes.txt",
    ]
    for path in to_remove:
        if path.exists():
            try:
                path.unlink()
                print(f"[1.C] Removed {path}")
            except OSError as e:
                print(f"[1.C] Could not remove {path}: {e}")


def clean_clustering_intermediates() -> None:
    print("\n[2.C] Cleaning up intermediate clustering files")
    print("-" * 60)

    bank_csv_path = CLUSTERING_DIR / BANK_CSV_NAME
    files_to_delete = {
        bank_csv_path,
        BANK_ZIP_PATH,
        CLUSTERING_DIR / "bank.zip",
        CLUSTERING_DIR / "bank-additional.zip",
        CLUSTERING_DIR / "bank-full.csv",
        CLUSTERING_DIR / "bank-names.txt",
    }

    for path in files_to_delete:
        if path.exists():
            try:
                path.unlink()
                print(f"[2.C] Removed file {path}")
            except OSError as e:
                print(f"[2.C] Could not remove {path}: {e}")


def clean_movies_intermediates() -> None:
    print("\n[3.C] Cleaning up intermediate movies files")
    print("-" * 60)

    files_to_delete = {
        MOVIES_ZIP_PATH,
        MOVIES_RATINGS,
    }
    for path in files_to_delete:
        if path.exists():
            try:
                path.unlink()
                print(f"[3.C] Removed file {path}")
            except OSError as e:
                print(f"[3.C] Could not remove {path}: {e}")


def prepare_coverage_dataset() -> int:
    print("=" * 60)
    print("[1] Maximum Coverage Experiment - Dataset Preparation")
    print("=" * 60)

    print(f"\n[1.0] Working directory: {Path.cwd()}\n")

    # Step 1: Download dataset files into coverage/
    print("[1.1] Downloading dataset files")
    print("-" * 60)
    for fname in FILES_TO_DOWNLOAD:
        url = DATASET_BASE_URL + fname
        dest = COVERAGE_DIR / fname
        if dest.exists():
            print(f"Skipping download, already exists: {dest}")
            continue
        try:
            download_file(url, dest)
        except Exception as e:
            print(f"[1.1] ERROR downloading {url}: {e}")
            return 1

    # Step 1b: Decompress .gz files to .txt
    print("\n[1.2] Decompressing downloaded files")
    print("-" * 60)
    for gz_name in FILES_TO_DOWNLOAD:
        gz_path = COVERAGE_DIR / gz_name
        txt_name = gz_name[:-3] if gz_name.endswith(".gz") else gz_name
        txt_path = COVERAGE_DIR / txt_name

        if txt_path.exists():
            print(f"Skipping decompression, already exists: {txt_path}")
            continue

        if not gz_path.exists():
            print(f"[1.2] ERROR: Expected archive not found: {gz_path}")
            return 1

        try:
            gunzip_file(gz_path, txt_path)
        except Exception as e:
            print(f"[1.2] Error decompressing {gz_path}: {e}")
            return 1

    # Verify required .txt files
    print("\n[1.3] Verifying decompressed files")
    print("-" * 60)
    required_txt = [
        COVERAGE_DIR / "soc-pokec-relationships.txt",
        COVERAGE_DIR / "soc-pokec-profiles.txt",
    ]
    for p in required_txt:
        if not p.exists():
            print(f"[1.3] ERROR: Required file missing after decompression: {p}")
            return 1

    # Step 2: Run preprocessing executables
    print("\n" + "=" * 60)
    print("[1.4] Running preprocessing executables")
    print("-" * 60)

    for exe_path, expected_output in PREPROCESSING_STEPS:
        print(f"\n[1.4] Step: {exe_path.name}")
        ret = run_executable(exe_path)
        if ret != 0:
            print(f"[1.4] ERROR: {exe_path} exited with code {ret}")
            return 1

        if expected_output.exists():
            print(f"[1.4] Success: generated {expected_output}")
        else:
            print(f"[1.4] ERROR: expected output not found: {expected_output}")
            return 1

    print("\n" + "=" * 60)
    print("[1.5] Coverage experiment dataset preparation complete")
    print("=" * 60)
    print("\n[1.5] Generated files:")
    for _, output in PREPROCESSING_STEPS:
        status = "OK" if output.exists() else "MISSING"
        print(f"  {status} - {output}")

    # Note: no automatic cleanup here anymore
    return 0


def prepare_clustering_dataset() -> int:
    print("\n" + "=" * 60)
    print("[2] Clustering Experiment - Dataset Preparation")
    print("=" * 60)

    CLUSTERING_DIR.mkdir(parents=True, exist_ok=True)

    # 1) Download outer bank marketing zip
    print("\n[2.1] Download Bank Marketing dataset zip")
    print("-" * 60)
    if BANK_ZIP_PATH.exists():
        print(f"[2.1] Skipping download, already exists: {BANK_ZIP_PATH}")
    else:
        try:
            download_file(BANK_ZIP_URL, BANK_ZIP_PATH)
        except Exception as e:
            print(f"[2.1] ERROR downloading {BANK_ZIP_URL}: {e}")
            return 1

    # 2) Unzip outer zip to clustering/
    print("\n[2.2] Unzip bank_marketing.zip")
    print("-" * 60)
    try:
        with zipfile.ZipFile(BANK_ZIP_PATH, "r") as zf:
            zf.extractall(CLUSTERING_DIR)
        print(f"[2.2] Extracted {BANK_ZIP_PATH} into {CLUSTERING_DIR}")
    except Exception as e:
        print(f"[2.2] ERROR extracting {BANK_ZIP_PATH}: {e}")
        return 1

    inner_zip_path = CLUSTERING_DIR / BANK_INNER_ZIP_NAME
    if not inner_zip_path.exists():
        print(f"[2.2] ERROR: Expected inner zip not found: {inner_zip_path}")
        return 1

    # 3) Unzip inner bank.zip (contains bank.csv)
    print("\n[2.3] Unzip inner bank.zip")
    print("-" * 60)
    try:
        with zipfile.ZipFile(inner_zip_path, "r") as zf:
            zf.extractall(CLUSTERING_DIR)
        print(f"[2.3] Extracted {inner_zip_path} into {CLUSTERING_DIR}")
    except Exception as e:
        print(f"[2.3] ERROR extracting {inner_zip_path}: {e}")
        return 1

    bank_csv_path = CLUSTERING_DIR / BANK_CSV_NAME
    if not bank_csv_path.exists():
        print(f"[2.3] ERROR: Expected CSV not found: {bank_csv_path}")
        return 1

    # 4) Run bank_input_converter_main.exe
    print("\n[2.4] Run bank_input_converter_main.exe")
    print("-" * 60)
    ret = run_executable(BANK_CONVERTER_EXE)
    if ret != 0:
        print(f"[2.4] ERROR: {BANK_CONVERTER_EXE} exited with code {ret}")
        return 1

    if not BANK_OUTPUT.exists():
        print(f"[2.4] ERROR: Expected output file not found: {BANK_OUTPUT}")
        return 1
    print(f"[2.4] Success: generated {BANK_OUTPUT}")

    # Note: no automatic cleanup; user can run with --clean
    print("\n[2.5] Clustering experiment dataset preparation complete")
    print("=" * 60)
    return 0


def prepare_movies_dataset() -> int:
    print("\n" + "=" * 60)
    print("[3] Movies Experiment - Dataset Preparation")
    print("=" * 60)

    MOVIES_DIR.mkdir(parents=True, exist_ok=True)

    # 1) Download ml-1m.zip
    print("\n[3.1] Download MovieLens 1M dataset zip")
    print("-" * 60)
    if MOVIES_ZIP_PATH.exists():
        print(f"[3.1] Skipping download, already exists: {MOVIES_ZIP_PATH}")
    else:
        try:
            download_file(MOVIES_ZIP_URL, MOVIES_ZIP_PATH)
        except Exception as e:
            print(f"[3.1] ERROR downloading {MOVIES_ZIP_URL}: {e}")
            return 1

    # 2) Unzip only needed files (movies.dat, ratings.dat) directly into movies/
    print("\n[3.2] Unzip selected files from ml-1m.zip")
    print("-" * 60)
    try:
        with zipfile.ZipFile(MOVIES_ZIP_PATH, "r") as zf:
            members = {
                "ml-1m/movies.dat": MOVIES_DAT,
                "ml-1m/ratings.dat": MOVIES_RATINGS,
            }
            for arc_name, target_path in members.items():
                try:
                    with zf.open(arc_name) as src, open(target_path, "wb") as dst:
                        shutil.copyfileobj(src, dst)
                    print(f"[3.2] Extracted {arc_name} -> {target_path}")
                except KeyError:
                    print(f"[3.2] ERROR: {arc_name} not found in zip")
                    return 1
    except Exception as e:
        print(f"[3.2] ERROR extracting from {MOVIES_ZIP_PATH}: {e}")
        return 1

    # Verify expected files
    print("\n[3.3] Verifying extracted MovieLens files")
    print("-" * 60)
    missing = [p for p in (MOVIES_DAT, MOVIES_RATINGS) if not p.exists()]
    if missing:
        print("[3.3] ERROR: Missing expected files after extraction:")
        for p in missing:
            print(f"  {p}")
        return 1

    # 3) Run movies/prepare_movies.py
    print("\n[3.4] Run movies/prepare_movies.py (this will take a few minutes)")
    print("-" * 60)
    if not MOVIES_PREPARE_SCRIPT.exists():
        print(f"[3.4] ERROR: {MOVIES_PREPARE_SCRIPT} not found")
        return 1

    cmd = [sys.executable, str(MOVIES_PREPARE_SCRIPT.name)]
    try:
        result = subprocess.run(cmd, cwd=str(MOVIES_DIR), check=False)
    except OSError as e:
        print(f"[3.4] ERROR: Failed to run {MOVIES_PREPARE_SCRIPT}: {e}")
        return 1

    if result.returncode != 0:
        print(f"[3.4] ERROR: {MOVIES_PREPARE_SCRIPT} exited with code {result.returncode}")
        return 1

    # Note: no automatic cleanup; user can run with --clean
    print("\n[3.5] Movies experiment dataset preparation complete")
    print("=" * 60)
    return 0


def main() -> int:
    if len(sys.argv) > 1 and sys.argv[1] == "--clean":
        print("[C] Cleaning mode: removing intermediate/temporary files only")
        clean_coverage_intermediates()
        clean_clustering_intermediates()
        clean_movies_intermediates()
        print("\n[C] Cleanup complete")
        print("=" * 60)
        return 0

    rc = prepare_coverage_dataset()
    if rc != 0:
        return rc

    rc = prepare_clustering_dataset()
    if rc != 0:
        return rc

    rc = prepare_movies_dataset()
    if rc != 0:
        return rc

    print("\nAll datasets prepared successfully.\nTo delete intermediate files, run: python prepare_datasets.py --clean")

    print("=" * 60)

    return 0


if __name__ == "__main__":
    sys.exit(main())
