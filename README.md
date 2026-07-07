# 🗺️ Graph Theory and Route Optimization

A comprehensive C++ project that applies **graph theory algorithms** to model and optimize a smart transportation network of 500 cities across 25 geographical regions. The project demonstrates real-world applications of graph algorithms including shortest path finding, minimum spanning trees, network resilience analysis, and traffic-aware routing.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Algorithms Implemented](#algorithms-implemented)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Complexity Analysis](#complexity-analysis)
- [License](#license)

---

## 🔍 Overview

This project simulates a **Smart Transportation Network** where:

- **500 cities** are distributed across **25 geographical regions**
- Cities within the same region are connected via intra-region roads
- Sparse **inter-region bridge roads** connect different regions
- Edge weights (5–50) represent travel distances between cities

The network is intentionally designed to be **partially disconnected** (multiple connected components), mimicking real-world transportation constraints where not all cities are directly reachable.

---

## ✨ Features

### Assignment 2 — Foundation (`ass2.cpp`)
| Task | Description |
|------|-------------|
| **Network Generation** | Procedurally generates a weighted, undirected graph with region-based clustering |
| **BFS Connectivity Analysis** | Identifies the largest connected component and network coverage |
| **Cluster Detection** | Discovers and categorizes all connected components by size |
| **Dijkstra's Shortest Path** | Finds shortest paths from a random source, reports reachable/unreachable cities |

### Final Assignment — Advanced Analysis (`finalAssn.cpp`)
| Task | Description |
|------|-------------|
| **Task 1: MST Optimization** | Kruskal's algorithm with Union-Find to build a cost-minimized spanning forest |
| **Task 2: Strategic City Identification** | Degree centrality ranking to identify cities optimal for airports, logistics hubs, and railway junctions |
| **Task 3: Disaster Recovery Routing** | Simulates natural disasters (node/edge removal) and computes alternate routes via modified Dijkstra |
| **Task 4: Traffic-Aware Smart Routing** | Dynamic edge-weight adjustment based on traffic scenarios (Random, Peak Hours, Low Traffic) |
| **Task 5: Critical Infrastructure Analysis** | Identifies single points of failure — the most critical city and road whose removal maximally fragments the network |

---

## 🧮 Algorithms Implemented

| Algorithm | Purpose | Data Structure |
|-----------|---------|----------------|
| **BFS** (Breadth-First Search) | Connected component detection, network traversal | Queue |
| **Dijkstra's Algorithm** | Shortest path with priority queue (min-heap) | Priority Queue |
| **Kruskal's Algorithm** | Minimum Spanning Tree / Forest construction | Union-Find (DSU) |
| **Union-Find (DSU)** | Disjoint set operations with path compression & union by rank | Array |
| **Degree Centrality** | Node importance ranking by connection count | Sorting |
| **Node/Edge Elimination** | Vulnerability analysis by iterative removal | BFS per removal |

---

## 📁 Project Structure

```
Graph-Theory-and-Route-Optimization/
├── ass2.cpp              # Assignment 2: Network generation, BFS, clustering, Dijkstra
├── finalAssn.cpp         # Final Assignment: MST, centrality, disaster recovery, traffic routing, critical infra
├── MID_EVAL_PPT.pdf      # Mid-evaluation presentation
├── README.md             # This file
├── LICENSE               # MIT License
└── .gitignore            # Git ignore rules
```

---

## 🚀 Getting Started

### Prerequisites

- A C++ compiler supporting **C++11** or later (g++, clang++, MSVC)

### Build & Run

#### Assignment 2
```bash
g++ -std=c++11 -O2 -o ass2 ass2.cpp
./ass2
```

#### Final Assignment
```bash
g++ -std=c++11 -O2 -o finalAssn finalAssn.cpp
./finalAssn
```

> **Note:** On Windows with MSVC, use:
> ```powershell
> cl /EHsc /O2 finalAssn.cpp
> .\finalAssn.exe
> ```

---

## 🎮 Usage

### Assignment 2 (`ass2.cpp`)
Runs fully automatically — generates the network and displays all analysis results without any user input.

### Final Assignment (`finalAssn.cpp`)
Interactive prompts appear for **Tasks 3 and 4**:

#### Task 3 — Disaster Recovery
```
Select disruption type:
  [1] Remove Cities (Node Removal)
  [2] Remove Roads  (Edge Removal)
Enter choice (1 or 2): 1

How many cities to remove? 2
Enter city IDs to remove (0 to 499):
  City 1: 42
  City 2: 100

Enter Source City A: 10
Enter Source City B: 200
Enter Destination City D: 450
```

#### Task 4 — Traffic-Aware Routing
```
Enter Source City: 10
Enter Destination City: 300

Select Traffic Scenario:
  [1] Random Traffic
  [2] Peak Hours
  [3] Low Traffic
Enter choice (1-3): 2
```

---

## 📊 Complexity Analysis

| Algorithm | Time Complexity | Space Complexity |
|-----------|----------------|-----------------|
| Network Generation | O(V × k) | O(V + E) |
| BFS Traversal | O(V + E) | O(V) |
| Connected Components | O(V + E) | O(V) |
| Kruskal's MST (Union-Find) | O(E log E) | O(V + E) |
| Degree Centrality Ranking | O(V log V) | O(V) |
| Dijkstra (Priority Queue) | O((V + E) log V) | O(V) |
| Dijkstra (Traffic-Aware) | O((V + E) log V) | O(V + E) |
| Node Elimination Analysis | O(V × (V + E)) | O(V) |
| Edge Elimination Analysis | O(E × (V + E)) | O(V + E) |

Where **V = 500** cities, **E ≈ 400–600** roads, **k = max connections per city**

---

## 🛠️ Configuration

The network parameters can be tuned via constants at the top of each source file:

```cpp
const int NUM_CITIES  = 500;   // Total nodes
const int NUM_REGIONS = 25;    // Geographical clusters
const int MIN_WEIGHT  = 5;     // Minimum edge weight
const int MAX_WEIGHT  = 50;    // Maximum edge weight
const int MIN_CONN    = 1;     // Min intra-region connections per city
const int MAX_CONN    = 2;     // Max intra-region connections per city
const int MIN_BRIDGES = 3;     // Min inter-region bridge roads
const int MAX_BRIDGES = 8;     // Max inter-region bridge roads
```

---

## 📄 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
