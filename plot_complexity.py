# tools/plot_complexity.py
import csv
import math
import sys
import matplotlib.pyplot as plt
from pathlib import Path


def load_csv(path):
    Ns = []
    build_ms = []
    view_ms = []
    insert_ms = []
    space = []
    with open(path, newline="") as f:
        r = csv.reader(f)
        for row in r:
            if not row or not row[0].strip():
                continue
            try:
                n = int(row[0])
            except ValueError:
                continue
            Ns.append(n)
            build_ms.append(float(row[1]))
            view_ms.append(float(row[2]))
            insert_ms.append(float(row[3]))
            space.append(float(row[4]))
    return Ns, build_ms, view_ms, insert_ms, space


def make_ref_on(Ns, base_time):
    n0 = Ns[0]
    return [base_time * (n / n0) for n in Ns]


def make_ref_onlogn(Ns, base_time):
    n0 = Ns[0]
    return [base_time * (n * math.log2(n)) / (n0 * math.log2(n0)) for n in Ns]


def plot_pair(ax_time, ax_space, title, Ns, build_ms, view_ms, insert_ms, space):
    # time
    ax_time.plot(Ns, build_ms, marker="o", label="build indexes")
    ax_time.plot(Ns, view_ms, marker="o", label="view sort")
    ax_time.plot(Ns, insert_ms, marker="o", label="insert+rebuild")

    # refs
    ref_on = make_ref_on(Ns, build_ms[0])
    ref_onlogn = make_ref_onlogn(Ns, view_ms[0] if view_ms[0] > 0 else build_ms[0])

    ax_time.plot(Ns, ref_on, "--", color="gray", label="O(n) ref (build)")
    ax_time.plot(Ns, ref_onlogn, "--", color="orange", label="O(n log n) ref (sort)")

    ax_time.set_title(f"{title} — Time")
    ax_time.set_xlabel("N (records)")
    ax_time.set_ylabel("Time (ms)")
    ax_time.grid(True, linestyle="--", alpha=0.5)
    ax_time.legend(fontsize=7)

    # space
    ax_space.plot(Ns, space, marker="o", label="measured space")
    ref_space = make_ref_on(Ns, space[0])
    ax_space.plot(Ns, ref_space, "--", color="red", label="O(n) ref")

    ax_space.set_title(f"{title} — Space")
    ax_space.set_xlabel("N (records)")
    ax_space.set_ylabel("Space (approx units)")
    ax_space.grid(True, linestyle="--", alpha=0.5)
    ax_space.legend(fontsize=7)


def main():
    # default paths
    base = Path("data")
    dep_path = base / "bench_deposits.csv"
    loan_path = base / "bench_loans.csv"
    cred_path = base / "bench_credits.csv"

    fig, axes = plt.subplots(3, 2, figsize=(10, 12))
    fig.suptitle(
        "FinCalc-DSA Benchmarks — Time & Space with Theoretical References", fontsize=12
    )

    # deposits
    if dep_path.exists():
        Ns, b, v, ins, sp = load_csv(dep_path)
        plot_pair(axes[0, 0], axes[0, 1], "Deposits", Ns, b, v, ins, sp)
    else:
        axes[0, 0].set_title("Deposits — missing CSV")
        axes[0, 1].set_title("Deposits — missing CSV")

    # loans
    if loan_path.exists():
        Ns, b, v, ins, sp = load_csv(loan_path)
        plot_pair(axes[1, 0], axes[1, 1], "Loans", Ns, b, v, ins, sp)
    else:
        axes[1, 0].set_title("Loans — missing CSV")
        axes[1, 1].set_title("Loans — missing CSV")

    # credits
    if cred_path.exists():
        Ns, b, v, ins, sp = load_csv(cred_path)
        plot_pair(axes[2, 0], axes[2, 1], "Credits", Ns, b, v, ins, sp)
    else:
        axes[2, 0].set_title("Credits — missing CSV")
        axes[2, 1].set_title("Credits — missing CSV")

    plt.tight_layout(rect=[0, 0, 1, 0.97])
    plt.savefig("bench_all.png", dpi=150)
    print("✅ wrote bench_all.png")


if __name__ == "__main__":
    main()
