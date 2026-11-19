# BOME — Order Book Matching Engine

**BOME** a deterministic, low latency and cache conscious Limit Order Book (LOB) matching engine that is written in **C++20**. Designed around fixed point arithmetic, with a tightly packed data structure and zero allocation memory pooling, supporting minimize heap contention and latency jitter.

## Key Features

* **Price-Time Priority Matching:** At each price level it has strict FIFO execution, bid are sorted descending(highest price first), asks sorted ascending(lowest price first) and incoming orders execute at the resting (maker) order's price.
* **Core Order Types:**
  * **Limit:** Matches against crossing liquidity and rests any unfilled remainder on the book.
  * **Market:** Sweeps available liquidity at the best available prices without resting.
  * **IOC (Immediate-or-Cancel):** Fills as much quantity as immediately available and cancels any remainder.
  * **FOK (Fill-or-Kill):** Performs a non-mutating liquidity check before matching; executes 100% immediately or rejects completely.
* **Custom Fixed-Size Memory Pool:** Pre-allocates contiguous chunks of memory with an embedded intrusive free list, providing $O(1)$ order allocation and deallocation with zero heap fragmentation.
* **Cache-Conscious Layout:** Fixed-width integer types (`uint64_t` fixed-point ticks, `uint32_t` quantities) and size-ordered struct members keep `Order` compact at **24 bytes**, allowing multiple orders to fit within a single 64-byte CPU cache line.

## Architecture & Project Structure

```text
BOME/
├── CMakeLists.txt           # C++20 build config (-O3 -march=native)
├── docs/
│   └── The_C++_Programming_Language_4th_Edition_Bjarne_Stroustrup.pdf
├── include/
│   ├── order.hpp            # Packed Order struct, Side/OrderType enums, Trade record
│   ├── memory_pool.hpp      # Fixed-size block allocator with placement new
│   └── order_book.hpp       # Price-Time Priority matching engine & book state
└── src/
    └── main.cpp             # Execution test harness
```

### Component Overview

1. **`include/order.hpp`** — Defines strongly typed `Side` and `OrderType` enums, the 24-byte `Order` struct, and the `Trade` execution report struct.
2. **`include/memory_pool.hpp`** — Implements `MemoryPool<T, BlockSize>` using a union-based intrusive free list inside pre-allocated chunks (`4096` blocks per chunk by default). Aggressive orders that fill immediately never touch the pool; only resting remainders are allocated.
3. **`include/order_book.hpp`** — Manages the bid and ask books, executes dry-run liquidity checks (`can_fill`) for FOK orders, runs the matching loop (`match_against`), recycles filled orders back to `order_pool_`, and prunes empty price levels.

## Getting Started

### Prerequisites

* A C++20-compatible compiler (`g++` 11+ or `clang++` 14+)
* `cmake` 3.15+
* Linux / WSL2 (Ubuntu) or macOS

### Build & Run

```bash
# Clone the repository
git clone <your-repo-url>
cd BOME

# Configure and compile with native optimizations
mkdir -p build && cd build
cmake ..
cmake --build .

# Run the test harness
./bome_engine
```

## Sample Output

Running `./bome_engine` will execute a sequence of resting limit orders, with a rejected FOK order, partial IOC fill, aggressive limit sweep across two price levels and a market sweep:

```text
=== INITIAL BOOK ===
--- ORDER BOOK SNAPSHOT ---
ASKS (Sells):
  Price: 10055 | Qty: 200
  Price: 10052 | Qty: 100
---------------------------
BIDS (Buys):
  Price: 10050 | Qty: 100
  Price: 10048 | Qty: 150
---------------------------
[FOK Buy 150 @ 10052 (Expect Kill)] Executions: 0
[IOC Buy 150 @ 10052 (Expect 100 Fill)] Executions: 1
  TRADE | BuyID: 3002 | SellID: 2001 | Price: 10052 | Qty: 100
[Limit Sell 150 @ 10048 (Expect 2 Fills)] Executions: 2
  TRADE | BuyID: 1001 | SellID: 3003 | Price: 10050 | Qty: 100
  TRADE | BuyID: 1002 | SellID: 3003 | Price: 10048 | Qty: 50
[Market Buy 120 (Expect 120 @ 10055)] Executions: 1
  TRADE | BuyID: 3004 | SellID: 2002 | Price: 10055 | Qty: 120

=== FINAL BOOK ===
--- ORDER BOOK SNAPSHOT ---
ASKS (Sells):
  Price: 10055 | Qty: 80
---------------------------
BIDS (Buys):
  Price: 10048 | Qty: 100
---------------------------
```

## Roadmap

* [ ] $O(1)$ Order Cancellation & Modification via an order-ID lookup table
* [ ] Additional order types (Post-Only, Iceberg, Trailing Stop)
* [ ] Pre-trade Risk Management (Self-Trade Prevention, fat-finger limits, kill switch)
* [ ] Lock-free SPSC ring buffer to decouple order ingestion from the matching thread
* [ ] Nanosecond latency benchmarking harness (p50 / p99 / p99.9)

## Documentation

To help explain the core C++ concepts and systems programming best practice used in this project check out the reference guide written by one of `the greats` located in `docs/`:

* [The C++ Programming Language (4th Edition)](docs/The_C++_Programming_Language_4th_Edition_Bjarne_Stroustrup.pdf): Reference material on memory management, templates, placement `new`, pointer arithmetic, and modern C++ design.

## PDF Text Extraction & Viewing

To search and extract text from PDF reference documents directly in the terminal use the standard Unix pipelines combined with the `poppler-utils` (`pdftotext`) tool.

**Install `poppler-utils` (Ubuntu / WSL2):**

```bash
sudo apt update && sudo apt install poppler-utils -y
```

### Example Usage

**Print extracted text to standard output (acting as a filter):**

```bash
pdftotext docs/The_C++_Programming_Language_4th_Edition_Bjarne_Stroustrup.pdf -
```

**Search for concepts or keywords inside the PDF using a pipe:**

```bash
pdftotext docs/The_C++_Programming_Language_4th_Edition_Bjarne_Stroustrup.pdf - | grep -i "keyword"
```

**Extract and save text to a new file:**

```bash
pdftotext docs/The_C++_Programming_Language_4th_Edition_Bjarne_Stroustrup.pdf extracted_output.txt
```