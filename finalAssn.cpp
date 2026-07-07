#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <numeric>

using namespace std;

// ============================================================
//  GLOBAL CONSTANTS & DATA STRUCTURES
// ============================================================

const int NUM_CITIES = 500;
const int NUM_REGIONS = 25;
const int MIN_WEIGHT = 5;
const int MAX_WEIGHT = 50;
const int MIN_CONN = 1;
const int MAX_CONN = 2;
const int MIN_BRIDGES = 3;
const int MAX_BRIDGES = 8;

vector<vector<pair<int, int>>> adjList(NUM_CITIES);
int totalEdges = 0;

// ============================================================
//  UTILITY FUNCTIONS (Reused from Assignment 2)
// ============================================================

int randomWeight() {
  return MIN_WEIGHT + rand() % (MAX_WEIGHT - MIN_WEIGHT + 1);
}

void addEdge(int u, int v, int w) {
  adjList[u].push_back({v, w});
  adjList[v].push_back({u, w});
  totalEdges++;
}

void printSeparator(char ch = '=', int len = 60) {
  cout << "  " << string(len, ch) << endl;
}

void printHeader(const string &title) {
  cout << endl;
  printSeparator('=', 65);
  cout << "     " << title << endl;
  printSeparator('=', 65);
  cout << endl;
}

// ============================================================
//  BFS & COMPONENT DETECTION
// ============================================================

void bfs(int start, vector<bool> &visited, vector<int> &component) {
  queue<int> q;
  q.push(start);
  visited[start] = true;

  while (!q.empty()) {
    int curr = q.front();
    q.pop();
    component.push_back(curr);

    for (int i = 0; i < (int)adjList[curr].size(); i++) {
      int neighbor = adjList[curr][i].first;
      if (!visited[neighbor]) {
        visited[neighbor] = true;
        q.push(neighbor);
      }
    }
  }
}

vector<vector<int>> findAllComponents() {
  vector<vector<int>> components;
  vector<bool> visited(NUM_CITIES, false);

  for (int i = 0; i < NUM_CITIES; i++) {
    if (!visited[i]) {
      vector<int> component;
      bfs(i, visited, component);
      components.push_back(component);
    }
  }

  return components;
}

// BFS that respects disabled nodes (for Task 3 & 5)
void bfsWithDisabled(int start, vector<bool> &visited,
                     vector<int> &component,
                     const vector<bool> &disabledNodes) {
  queue<int> q;
  q.push(start);
  visited[start] = true;

  while (!q.empty()) {
    int curr = q.front();
    q.pop();
    component.push_back(curr);

    for (int i = 0; i < (int)adjList[curr].size(); i++) {
      int neighbor = adjList[curr][i].first;
      if (!visited[neighbor] && !disabledNodes[neighbor]) {
        visited[neighbor] = true;
        q.push(neighbor);
      }
    }
  }
}

// BFS that respects disabled nodes AND disabled edges (for Task 5)
void bfsWithDisabledEdges(int start, vector<bool> &visited,
                          vector<int> &component,
                          const vector<bool> &disabledNodes,
                          const set<pair<int, int>> &disabledEdges) {
  queue<int> q;
  q.push(start);
  visited[start] = true;

  while (!q.empty()) {
    int curr = q.front();
    q.pop();
    component.push_back(curr);

    for (int i = 0; i < (int)adjList[curr].size(); i++) {
      int neighbor = adjList[curr][i].first;
      if (!visited[neighbor] && !disabledNodes[neighbor]) {
        int a = min(curr, neighbor);
        int b = max(curr, neighbor);
        if (disabledEdges.count({a, b}) == 0) {
          visited[neighbor] = true;
          q.push(neighbor);
        }
      }
    }
  }
}

// Count components with disabled nodes
int countComponentsWithDisabledNodes(const vector<bool> &disabledNodes) {
  vector<bool> visited(NUM_CITIES, false);
  int count = 0;

  for (int i = 0; i < NUM_CITIES; i++) {
    if (!visited[i] && !disabledNodes[i]) {
      vector<int> component;
      bfsWithDisabled(i, visited, component, disabledNodes);
      count++;
    }
  }

  return count;
}

// Count components with disabled edges
int countComponentsWithDisabledEdges(const set<pair<int, int>> &disabledEdges) {
  vector<bool> visited(NUM_CITIES, false);
  vector<bool> disabledNodes(NUM_CITIES, false);
  int count = 0;

  for (int i = 0; i < NUM_CITIES; i++) {
    if (!visited[i]) {
      vector<int> component;
      bfsWithDisabledEdges(i, visited, component, disabledNodes,
                           disabledEdges);
      count++;
    }
  }

  return count;
}

// ============================================================
//  NETWORK GENERATION 
// ============================================================

void generateNetwork() {
  vector<int> region(NUM_CITIES);
  for (int i = 0; i < NUM_CITIES; i++) {
    region[i] = rand() % NUM_REGIONS;
  }

  vector<vector<int>> regionMembers(NUM_REGIONS);
  for (int i = 0; i < NUM_CITIES; i++) {
    regionMembers[region[i]].push_back(i);
  }

  cout << "  Geographical Region Distribution:" << endl;
  printSeparator('-', 45);

  for (int r = 0; r < NUM_REGIONS; r++) {
    int sz = regionMembers[r].size();
    cout << "    Region " << setw(2) << r << ": " << setw(3) << sz
         << " cities  ";
    int barLen = sz / 2;
    for (int b = 0; b < barLen; b++)
      cout << "#";
    cout << endl;
  }
  cout << endl;

  set<pair<int, int>> edgeSet;

  for (int r = 0; r < NUM_REGIONS; r++) {
    vector<int> &members = regionMembers[r];
    int n = members.size();
    if (n <= 1)
      continue;

    for (int idx = 0; idx < n; idx++) {
      int city = members[idx];
      int numConnections = MIN_CONN + rand() % (MAX_CONN - MIN_CONN + 1);

      for (int c = 0; c < numConnections; c++) {
        int neighborIdx = rand() % n;
        int neighbor = members[neighborIdx];
        if (neighbor == city)
          continue;

        int u = min(city, neighbor);
        int v = max(city, neighbor);
        if (edgeSet.count({u, v}) == 0) {
          addEdge(u, v, randomWeight());
          edgeSet.insert({u, v});
        }
      }
    }
  }

  int bridgeTarget = MIN_BRIDGES + rand() % (MAX_BRIDGES - MIN_BRIDGES + 1);
  int bridgesAdded = 0;
  int maxAttempts = 300;

  cout << "  Adding inter-region bridge roads..." << endl;

  for (int attempt = 0; attempt < maxAttempts && bridgesAdded < bridgeTarget;
       attempt++) {
    int u = rand() % NUM_CITIES;
    int v = rand() % NUM_CITIES;

    if (u != v && region[u] != region[v]) {
      int a = min(u, v);
      int b = max(u, v);
      if (edgeSet.count({a, b}) == 0) {
        addEdge(a, b, randomWeight());
        edgeSet.insert({a, b});
        bridgesAdded++;
      }
    }
  }

  cout << "  Bridge roads added: " << bridgesAdded << endl;
  cout << endl;

  vector<vector<int>> components = findAllComponents();

  if ((int)components.size() <= 1) {
    cout << "  [!] WARNING: Only 1 component detected." << endl;
    cout << "      The graph is too connected." << endl;
  } else {
    cout << "  [OK] Network has " << components.size()
         << " distinct clusters — realistic topology achieved!" << endl;
  }
}

void printNetworkStats() {
  double avgDegree = (2.0 * totalEdges) / NUM_CITIES;

  cout << endl;
  cout << "  Network Statistics:" << endl;
  printSeparator('-', 45);
  cout << "    Total Cities (Nodes) : " << NUM_CITIES << endl;
  cout << "    Total Roads  (Edges) : " << totalEdges << endl;
  cout << "    Average Degree       : " << fixed << setprecision(2) << avgDegree
       << endl;

  int maxDeg = 0, minDeg = NUM_CITIES;
  int zeroDegCount = 0;
  vector<int> degrees(NUM_CITIES);

  for (int i = 0; i < NUM_CITIES; i++) {
    degrees[i] = adjList[i].size();
    maxDeg = max(maxDeg, degrees[i]);
    minDeg = min(minDeg, degrees[i]);
    if (degrees[i] == 0)
      zeroDegCount++;
  }

  cout << "    Maximum Degree       : " << maxDeg << endl;
  cout << "    Minimum Degree       : " << minDeg << endl;
  cout << "    Isolated Cities      : " << zeroDegCount << endl;
  cout << endl;

  cout << "  Degree Distribution:" << endl;
  printSeparator('-', 45);

  vector<int> degFreq(maxDeg + 1, 0);
  for (int i = 0; i < NUM_CITIES; i++) {
    degFreq[degrees[i]]++;
  }

  for (int d = 0; d <= min(maxDeg, 12); d++) {
    if (degFreq[d] > 0) {
      cout << "    Degree " << setw(2) << d << ": " << setw(4) << degFreq[d]
           << " cities  ";
      int bar = degFreq[d] / 3;
      for (int b = 0; b < bar; b++)
        cout << "|";
      cout << endl;
    }
  }
  if (maxDeg > 12) {
    int remaining = 0;
    for (int d = 13; d <= maxDeg; d++)
      remaining += degFreq[d];
    if (remaining > 0)
      cout << "    Degree 13+: " << setw(3) << remaining << " cities" << endl;
  }
  cout << endl;

  cout << "  Sample Adjacency List (first 15 connected cities):" << endl;
  printSeparator('-', 55);

  int printed = 0;
  for (int i = 0; i < NUM_CITIES && printed < 15; i++) {
    if (!adjList[i].empty()) {
      cout << "    City " << setw(3) << i << " -> ";
      int count = 0;
      for (int j = 0; j < (int)adjList[i].size(); j++) {
        if (count > 0)
          cout << ", ";
        cout << adjList[i][j].first << "(w=" << adjList[i][j].second << ")";
        count++;
        if (count >= 6 && (int)adjList[i].size() > 6) {
          cout << " ... [+" << (adjList[i].size() - 6) << " more]";
          break;
        }
      }
      cout << endl;
      printed++;
    }
  }
}

// ============================================================
//  UNION-FIND (Disjoint Set Union) for Kruskal's MST
// ============================================================

struct UnionFind {
  vector<int> parent, rank_;

  UnionFind(int n) : parent(n), rank_(n, 0) {
    for (int i = 0; i < n; i++)
      parent[i] = i;
  }

  int find(int x) {
    if (parent[x] != x)
      parent[x] = find(parent[x]); // Path compression
    return parent[x];
  }

  bool unite(int x, int y) {
    int px = find(x), py = find(y);
    if (px == py)
      return false; // Already in same set
    // Union by rank
    if (rank_[px] < rank_[py])
      swap(px, py);
    parent[py] = px;
    if (rank_[px] == rank_[py])
      rank_[px]++;
    return true;
  }
};

// ============================================================
//  DIJKSTRA WITH DISABLED NODES & EDGES (for Tasks 3 & 4)
// ============================================================

struct DijkstraResult {
  vector<int> dist;
  vector<int> parent;
  bool pathExists;
  int pathDist;
  vector<int> path;
};

DijkstraResult dijkstraModified(int source, int destination,
                                 const vector<bool> &disabledNodes,
                                 const set<pair<int, int>> &disabledEdges,
                                 const vector<double> &edgeMultipliers = {}) {
  DijkstraResult result;
  result.dist.assign(NUM_CITIES, INT_MAX);
  result.parent.assign(NUM_CITIES, -1);
  result.pathExists = false;
  result.pathDist = INT_MAX;

  vector<bool> processed(NUM_CITIES, false);
  priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

  result.dist[source] = 0;
  pq.push({0, source});

  while (!pq.empty()) {
    int u = pq.top().second;
    pq.pop();

    if (processed[u])
      continue;
    processed[u] = true;

    for (int i = 0; i < (int)adjList[u].size(); i++) {
      int v = adjList[u][i].first;
      int w = adjList[u][i].second;

      // Skip disabled nodes
      if (disabledNodes[v])
        continue;

      // Skip disabled edges
      int a = min(u, v);
      int b = max(u, v);
      if (disabledEdges.count({a, b}) > 0)
        continue;

      // Apply traffic multiplier if provided
      int adjustedW = w;
      if (!edgeMultipliers.empty()) {
        // Find edge index for multiplier
        // We use a simple hash: use the sorted edge pair to look up
        adjustedW = (int)(w * edgeMultipliers[a * NUM_CITIES + b]);
      }

      if (result.dist[u] != INT_MAX && result.dist[u] + adjustedW < result.dist[v]) {
        result.dist[v] = result.dist[u] + adjustedW;
        result.parent[v] = u;
        pq.push({result.dist[v], v});
      }
    }
  }

  // Reconstruct path to destination
  if (destination >= 0 && destination < NUM_CITIES &&
      result.dist[destination] != INT_MAX) {
    result.pathExists = true;
    result.pathDist = result.dist[destination];

    int curr = destination;
    while (curr != -1) {
      result.path.push_back(curr);
      curr = result.parent[curr];
    }
    reverse(result.path.begin(), result.path.end());
  }

  return result;
}

// Simpler Dijkstra with traffic multiplier map (for Task 4)
DijkstraResult dijkstraWithTraffic(int source, int destination,
                                    const map<pair<int,int>, double> &trafficMult) {
  DijkstraResult result;
  result.dist.assign(NUM_CITIES, INT_MAX);
  result.parent.assign(NUM_CITIES, -1);
  result.pathExists = false;
  result.pathDist = INT_MAX;

  vector<bool> processed(NUM_CITIES, false);
  priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

  result.dist[source] = 0;
  pq.push({0, source});

  while (!pq.empty()) {
    int u = pq.top().second;
    pq.pop();

    if (processed[u])
      continue;
    processed[u] = true;

    for (int i = 0; i < (int)adjList[u].size(); i++) {
      int v = adjList[u][i].first;
      int w = adjList[u][i].second;

      int a = min(u, v);
      int b = max(u, v);

      double mult = 1.0;
      auto it = trafficMult.find({a, b});
      if (it != trafficMult.end()) {
        mult = it->second;
      }

      int adjustedW = (int)(w * mult);

      if (result.dist[u] != INT_MAX && result.dist[u] + adjustedW < result.dist[v]) {
        result.dist[v] = result.dist[u] + adjustedW;
        result.parent[v] = u;
        pq.push({result.dist[v], v});
      }
    }
  }

  if (destination >= 0 && destination < NUM_CITIES &&
      result.dist[destination] != INT_MAX) {
    result.pathExists = true;
    result.pathDist = result.dist[destination];

    int curr = destination;
    while (curr != -1) {
      result.path.push_back(curr);
      curr = result.parent[curr];
    }
    reverse(result.path.begin(), result.path.end());
  }

  return result;
}

// ============================================================
//  TASK 1: MST OPTIMIZATION (Kruskal's Algorithm)
// ============================================================

void task1_MSTOptimization() {
  printHeader("TASK 1: MST OPTIMIZATION");

  cout << "  Minimum Spanning Tree Execution" << endl;
  cout << "  Constructs a cost-minimized network connecting all reachable" << endl;
  cout << "  nodes with absolute efficiency." << endl;
  cout << endl;
  cout << "  Algorithmic Method: Kruskal's Algorithm with Union-Find" << endl;
  printSeparator('-', 55);
  cout << endl;

  // Step 1: Collect all edges
  struct Edge {
    int u, v, w;
  };
  vector<Edge> edges;
  set<pair<int, int>> seen;

  long long originalCost = 0;

  for (int u = 0; u < NUM_CITIES; u++) {
    for (int i = 0; i < (int)adjList[u].size(); i++) {
      int v = adjList[u][i].first;
      int w = adjList[u][i].second;
      int a = min(u, v);
      int b = max(u, v);
      if (seen.count({a, b}) == 0) {
        edges.push_back({a, b, w});
        seen.insert({a, b});
        originalCost += w;
      }
    }
  }

  cout << "  Total edges collected: " << edges.size() << endl;
  cout << endl;

  // Step 2: Sort edges by weight
  sort(edges.begin(), edges.end(),
       [](const Edge &a, const Edge &b) { return a.w < b.w; });

  // Step 3: Kruskal's algorithm with Union-Find
  UnionFind uf(NUM_CITIES);
  vector<Edge> mstEdges;
  long long mstCost = 0;

  for (int i = 0; i < (int)edges.size(); i++) {
    if (uf.unite(edges[i].u, edges[i].v)) {
      mstEdges.push_back(edges[i]);
      mstCost += edges[i].w;
    }
  }

  // Step 4: Display results
  double costSaved = 0.0;
  if (originalCost > 0)
    costSaved = ((double)(originalCost - mstCost) / originalCost) * 100.0;

  cout << "  Results:" << endl;
  printSeparator('-', 55);
  cout << "    Original Cost = " << originalCost << endl;
  cout << "    MST Cost      = " << mstCost << endl;
  cout << "    Cost Saved    = " << fixed << setprecision(1) << costSaved << " %"
       << endl;
  cout << endl;

  // Count how many components the MST spans
  // (If graph is disconnected, MST will be a forest)
  int mstComponents = NUM_CITIES - (int)mstEdges.size();
  cout << "    MST Edges Selected    : " << mstEdges.size() << endl;
  cout << "    Spanning Components   : " << mstComponents << endl;
  cout << endl;

  // Print selected roads
  cout << "  Selected Roads:" << endl;
  printSeparator('-', 45);

  int displayLimit = min((int)mstEdges.size(), 30);
  for (int i = 0; i < displayLimit; i++) {
    cout << "    (" << mstEdges[i].u << ", " << mstEdges[i].v
         << ")  weight = " << mstEdges[i].w << endl;
  }

  if ((int)mstEdges.size() > displayLimit) {
    cout << "    ... [+" << (mstEdges.size() - displayLimit)
         << " more edges]" << endl;
  }

  cout << endl;
  cout << "  [All " << mstEdges.size()
       << " structural spans successfully written to path buffer]" << endl;
}

// ============================================================
//  TASK 2: STRATEGIC CITY IDENTIFICATION (Degree Centrality)
// ============================================================

void task2_StrategicCityIdentification() {
  printHeader("TASK 2: STRATEGIC CITY IDENTIFICATION");

  cout << "  Connectivity Centroid Calculations" << endl;
  cout << "  Identify the most influential nodes using centrality analysis" << endl;
  cout << "  to map urbanization priority indicators." << endl;
  printSeparator('-', 55);
  cout << endl;

  // Compute degree centrality for every vertex
  struct CityDegree {
    int cityId;
    int degree;
  };

  vector<CityDegree> cities(NUM_CITIES);
  for (int i = 0; i < NUM_CITIES; i++) {
    cities[i].cityId = i;
    cities[i].degree = adjList[i].size();
  }

  // Sort by degree descending
  sort(cities.begin(), cities.end(),
       [](const CityDegree &a, const CityDegree &b) {
         return a.degree > b.degree;
       });

  // Print Top 10 rankings
  cout << "  Top 10 Cities by Degree Centrality:" << endl;
  printSeparator('-', 55);

  int topCount = min(10, NUM_CITIES);
  for (int i = 0; i < topCount; i++) {
    cout << "    Rank " << setw(2) << (i + 1) << " -> City " << setw(3)
         << cities[i].cityId << " -> Degree " << cities[i].degree << endl;
  }

  cout << endl;

  // Strategic Module: Automated recommendations
  cout << "  Top Development Candidates:" << endl;
  printSeparator('-', 55);

  if (topCount >= 1) {
    cout << "    * Airport: City " << cities[0].cityId << endl;
    cout << "      \"City " << cities[0].cityId << " is connected to "
         << cities[0].degree << " other cities and is a" << endl;
    cout << "       strong candidate for an international airport.\"" << endl;
    cout << endl;
  }
  if (topCount >= 2) {
    cout << "    * Logistics Hub: City " << cities[1].cityId << endl;
    cout << "      \"City " << cities[1].cityId << " has " << cities[1].degree
         << " connections making it ideal for a logistics hub.\"" << endl;
    cout << endl;
  }
  if (topCount >= 3) {
    cout << "    * Railway Junction: City " << cities[2].cityId << endl;
    cout << "      \"City " << cities[2].cityId << " with " << cities[2].degree
         << " connections is optimal for a railway junction.\"" << endl;
    cout << endl;
  }

  // Additional: Development priority zones (cities with degree >= average * 2)
  double avgDegree = (2.0 * totalEdges) / NUM_CITIES;
  int threshold = (int)(avgDegree * 1.5);

  cout << "  Development Priority Zones (degree >= " << threshold << "):" << endl;
  printSeparator('-', 55);

  int priorityCount = 0;
  for (int i = 0; i < NUM_CITIES; i++) {
    if (cities[i].degree >= threshold) {
      if (priorityCount < 20) {
        cout << "    City " << setw(3) << cities[i].cityId
             << " | Degree: " << setw(3) << cities[i].degree
             << " | Priority Rank: " << (i + 1) << endl;
      }
      priorityCount++;
    }
  }
  if (priorityCount > 20) {
    cout << "    ... [+" << (priorityCount - 20) << " more priority cities]"
         << endl;
  }
  if (priorityCount == 0) {
    cout << "    No cities exceed the priority threshold." << endl;
  }

  cout << endl;
  cout << "  Recommendations generated:" << endl;
  if (topCount >= 1)
    cout << "  \"City " << cities[0].cityId << " is connected to "
         << cities[0].degree << " other cities and is a" << endl
         << "   strong candidate for an international airport.\"" << endl;
}

// ============================================================
//  TASK 3: DISASTER RECOVERY ROUTING
// ============================================================

void task3_DisasterRecoveryRouting() {
  printHeader("TASK 3: DISASTER RECOVERY ROUTING");

  cout << "  Dynamic Network Disruption Scenarios" << endl;
  cout << "  Evaluate alternate routes when natural disasters destroy" << endl;
  cout << "  cities (node removal) or roads (edge removal)." << endl;
  printSeparator('-', 55);
  cout << endl;

  // Step 1: Choose removal type
  int removalType;
  cout << "  Select disruption type:" << endl;
  cout << "    [1] Remove Cities (Node Removal)" << endl;
  cout << "    [2] Remove Roads  (Edge Removal)" << endl;
  cout << "  Enter choice (1 or 2): ";
  cin >> removalType;
  cout << endl;

  vector<bool> disabledNodes(NUM_CITIES, false);
  set<pair<int, int>> disabledEdges;

  if (removalType == 1) {
    // Node removal
    int numRemove;
    cout << "  How many cities to remove? ";
    cin >> numRemove;

    cout << "  Enter city IDs to remove (0 to " << NUM_CITIES - 1 << "):" << endl;
    for (int i = 0; i < numRemove; i++) {
      int city;
      cout << "    City " << (i + 1) << ": ";
      cin >> city;
      if (city >= 0 && city < NUM_CITIES) {
        disabledNodes[city] = true;
        cout << "    [!] City " << city << " destroyed by disaster." << endl;
      } else {
        cout << "    [ERROR] Invalid city ID. Skipping." << endl;
      }
    }
  } else {
    // Edge removal
    int numRemove;
    cout << "  How many roads to remove? ";
    cin >> numRemove;

    cout << "  Enter road endpoints (city1 city2):" << endl;
    for (int i = 0; i < numRemove; i++) {
      int u, v;
      cout << "    Road " << (i + 1) << " (u v): ";
      cin >> u >> v;
      int a = min(u, v);
      int b = max(u, v);
      disabledEdges.insert({a, b});
      cout << "    [!] Road (" << a << ", " << b
           << ") destroyed by disaster." << endl;
    }
  }

  cout << endl;

  // Step 2: Get source cities and destination
  int srcA, srcB, dest;
  cout << "  Enter Source City A: ";
  cin >> srcA;
  cout << "  Enter Source City B: ";
  cin >> srcB;
  cout << "  Enter Destination City D: ";
  cin >> dest;
  cout << endl;

  // Validate inputs
  if (srcA < 0 || srcA >= NUM_CITIES || srcB < 0 || srcB >= NUM_CITIES ||
      dest < 0 || dest >= NUM_CITIES) {
    cout << "  [ERROR] Invalid city IDs. Aborting task." << endl;
    return;
  }

  if (disabledNodes[srcA] || disabledNodes[srcB] || disabledNodes[dest]) {
    cout << "  [ERROR] Source or destination city has been destroyed!" << endl;
    if (disabledNodes[srcA])
      cout << "    City " << srcA << " (Source A) is destroyed." << endl;
    if (disabledNodes[srcB])
      cout << "    City " << srcB << " (Source B) is destroyed." << endl;
    if (disabledNodes[dest])
      cout << "    City " << dest << " (Destination D) is destroyed." << endl;
    return;
  }

  // Step 3: Compute shortest paths
  printSeparator('-', 55);
  cout << "  Computing disaster recovery routes..." << endl;
  cout << endl;

  // Path A -> D
  DijkstraResult resultA = dijkstraModified(srcA, dest, disabledNodes, disabledEdges);

  cout << "  Source A Path: ";
  if (resultA.pathExists) {
    cout << "Distance = " << resultA.pathDist << " km | Path: ";
    for (int i = 0; i < (int)resultA.path.size(); i++) {
      cout << resultA.path[i];
      if (i < (int)resultA.path.size() - 1)
        cout << " -> ";
    }
    cout << endl;
  } else {
    cout << "No valid route available after disaster." << endl;
  }

  // Path B -> D
  DijkstraResult resultB = dijkstraModified(srcB, dest, disabledNodes, disabledEdges);

  cout << "  Source B Path: ";
  if (resultB.pathExists) {
    cout << "Distance = " << resultB.pathDist << " km | Path: ";
    for (int i = 0; i < (int)resultB.path.size(); i++) {
      cout << resultB.path[i];
      if (i < (int)resultB.path.size() - 1)
        cout << " -> ";
    }
    cout << endl;
  } else {
    cout << "No valid route available after disaster." << endl;
  }

  cout << endl;

  // Fallback state
  if (!resultA.pathExists && !resultB.pathExists) {
    cout << "  Fallback state (If path does not exist):" << endl;
    cout << "  \"No valid route available after disaster.\"" << endl;
  } else {
    // Recommend the shorter path
    if (resultA.pathExists && resultB.pathExists) {
      if (resultA.pathDist <= resultB.pathDist) {
        cout << "  Recommended Route: Source A (shorter by "
             << (resultB.pathDist - resultA.pathDist) << " km)" << endl;
      } else {
        cout << "  Recommended Route: Source B (shorter by "
             << (resultA.pathDist - resultB.pathDist) << " km)" << endl;
      }
    } else if (resultA.pathExists) {
      cout << "  Only Source A route is viable." << endl;
    } else {
      cout << "  Only Source B route is viable." << endl;
    }
  }
}

// ============================================================
//  TASK 4: TRAFFIC-AWARE SMART ROUTING
// ============================================================

void task4_TrafficAwareRouting() {
  printHeader("TASK 4: TRAFFIC-AWARE SMART ROUTING");

  cout << "  Dynamic Edge Weights" << endl;
  cout << "  Simulates real-world traffic flows by updating the path metrics" << endl;
  cout << "  in real-time based on congestion states." << endl;
  printSeparator('-', 55);
  cout << endl;

  // Get source and destination
  int source, destination;
  cout << "  Enter Source City: ";
  cin >> source;
  cout << "  Enter Destination City: ";
  cin >> destination;
  cout << endl;

  if (source < 0 || source >= NUM_CITIES || destination < 0 ||
      destination >= NUM_CITIES) {
    cout << "  [ERROR] Invalid city IDs. Aborting task." << endl;
    return;
  }

  // Traffic scenario selection
  int scenario;
  cout << "  Select Traffic Scenario:" << endl;
  cout << "    [1] Random Traffic    - each road gets a random traffic state" << endl;
  cout << "    [2] Peak Hours        - most roads have Medium/High traffic" << endl;
  cout << "    [3] Low Traffic       - most roads have Low traffic" << endl;
  cout << "  Enter choice (1-3): ";
  cin >> scenario;
  cout << endl;

  // Build traffic multiplier map
  // Low = 1.0, Medium = 1.5, High = 2.0
  map<pair<int, int>, double> trafficMult;
  map<pair<int, int>, string> trafficState;

  set<pair<int, int>> allEdges;
  for (int u = 0; u < NUM_CITIES; u++) {
    for (int i = 0; i < (int)adjList[u].size(); i++) {
      int v = adjList[u][i].first;
      int a = min(u, v);
      int b = max(u, v);
      allEdges.insert({a, b});
    }
  }

  int lowCount = 0, medCount = 0, highCount = 0;

  for (auto &edge : allEdges) {
    double mult;
    string state;

    if (scenario == 1) {
      // Random
      int r = rand() % 3;
      if (r == 0) {
        mult = 1.0;
        state = "Low";
        lowCount++;
      } else if (r == 1) {
        mult = 1.5;
        state = "Medium";
        medCount++;
      } else {
        mult = 2.0;
        state = "High";
        highCount++;
      }
    } else if (scenario == 2) {
      // Peak hours: 20% Low, 40% Medium, 40% High
      int r = rand() % 10;
      if (r < 2) {
        mult = 1.0;
        state = "Low";
        lowCount++;
      } else if (r < 6) {
        mult = 1.5;
        state = "Medium";
        medCount++;
      } else {
        mult = 2.0;
        state = "High";
        highCount++;
      }
    } else {
      // Low traffic: 70% Low, 20% Medium, 10% High
      int r = rand() % 10;
      if (r < 7) {
        mult = 1.0;
        state = "Low";
        lowCount++;
      } else if (r < 9) {
        mult = 1.5;
        state = "Medium";
        medCount++;
      } else {
        mult = 2.0;
        state = "High";
        highCount++;
      }
    }

    trafficMult[edge] = mult;
    trafficState[edge] = state;
  }

  cout << "  Traffic Distribution:" << endl;
  printSeparator('-', 45);
  cout << "    Low Traffic    : " << lowCount << " roads (1.0x)" << endl;
  cout << "    Medium Traffic : " << medCount << " roads (1.5x)" << endl;
  cout << "    High Traffic   : " << highCount << " roads (2.0x)" << endl;
  cout << endl;

  // Compute normal route (no traffic)
  map<pair<int, int>, double> noTraffic; // Empty = all 1.0x
  DijkstraResult normalResult = dijkstraWithTraffic(source, destination, noTraffic);

  // Compute traffic-aware route
  DijkstraResult trafficResult = dijkstraWithTraffic(source, destination, trafficMult);

  // Display results
  cout << "  Rerouting Output Metrics:" << endl;
  printSeparator('-', 55);

  cout << "  Normal Route:" << endl;
  if (normalResult.pathExists) {
    cout << "    Distance = " << normalResult.pathDist << " km" << endl;
    cout << "    Path: ";
    int displayLimit = min((int)normalResult.path.size(), 15);
    for (int i = 0; i < displayLimit; i++) {
      cout << normalResult.path[i];
      if (i < displayLimit - 1)
        cout << " -> ";
    }
    if ((int)normalResult.path.size() > displayLimit)
      cout << " -> ...";
    cout << endl;
  } else {
    cout << "    No route available between these cities." << endl;
  }

  cout << endl;

  cout << "  Traffic Route:" << endl;
  if (trafficResult.pathExists) {
    cout << "    Distance = " << trafficResult.pathDist << " km" << endl;
    cout << "    Path: ";
    int displayLimit = min((int)trafficResult.path.size(), 15);
    for (int i = 0; i < displayLimit; i++) {
      cout << trafficResult.path[i];
      if (i < displayLimit - 1)
        cout << " -> ";
    }
    if ((int)trafficResult.path.size() > displayLimit)
      cout << " -> ...";
    cout << endl;

    if (normalResult.pathExists && normalResult.pathDist > 0) {
      double delay = ((double)(trafficResult.pathDist - normalResult.pathDist) /
                      normalResult.pathDist) *
                     100.0;
      cout << "    Delay = " << fixed << setprecision(1) << delay << "%" << endl;
    }
  } else {
    cout << "    No route available between these cities." << endl;
  }

  cout << endl;
  cout << "  [System actively calculated congestion bypass routes" << endl;
  cout << "   based on elevated edge penalties]" << endl;
}

// ============================================================
//  TASK 5: CRITICAL INFRASTRUCTURE ANALYSIS
// ============================================================

void task5_CriticalInfrastructureAnalysis() {
  printHeader("TASK 5: CRITICAL INFRASTRUCTURE ANALYSIS");

  cout << "  Systemic Vulnerability Analysis" << endl;
  cout << "  Diagnoses specific single points of failure that cause extreme" << endl;
  cout << "  structural fractures inside the system." << endl;
  printSeparator('-', 55);
  cout << endl;

  // Get baseline component count
  vector<vector<int>> baseComponents = findAllComponents();
  int baseCount = baseComponents.size();

  cout << "  Baseline connected components: " << baseCount << endl;
  cout << endl;

  // ---- Node Elimination ----
  cout << "  Phase 1: Node Elimination Analysis" << endl;
  cout << "  Temporarily removing each city node and measuring disruption..." << endl;
  printSeparator('-', 55);
  cout << endl;

  int mostCriticalCity = -1;
  int maxDisconnectedByCity = 0;
  int maxComponentsByCity = baseCount;

  // For efficiency, only test nodes that have edges
  int testedNodes = 0;
  for (int city = 0; city < NUM_CITIES; city++) {
    if (adjList[city].empty())
      continue; // Isolated nodes won't affect connectivity

    testedNodes++;

    // Temporarily disable this city
    vector<bool> disabled(NUM_CITIES, false);
    disabled[city] = true;

    int compCount = countComponentsWithDisabledNodes(disabled);

    // The removed node itself doesn't count as a component
    // Compare with baseline minus consideration for the removed node
    if (compCount > maxComponentsByCity) {
      maxComponentsByCity = compCount;
      maxDisconnectedByCity = compCount;
      mostCriticalCity = city;
    }
  }

  cout << "  Nodes tested: " << testedNodes << " (skipped "
       << (NUM_CITIES - testedNodes) << " isolated nodes)" << endl;
  cout << endl;

  if (mostCriticalCity >= 0) {
    cout << "  Most Critical City:" << endl;
    cout << "    City " << mostCriticalCity << endl;
    cout << endl;
    cout << "  Disconnected Components Created:" << endl;
    cout << "    " << maxDisconnectedByCity << endl;
  } else {
    cout << "  No critical city found (all nodes are isolated)." << endl;
  }

  cout << endl;

  // ---- Edge Elimination ----
  cout << "  Phase 2: Edge Elimination Analysis" << endl;
  cout << "  Temporarily removing each road edge and measuring disruption..." << endl;
  printSeparator('-', 55);
  cout << endl;

  // Collect all unique edges
  struct EdgeInfo {
    int u, v;
  };
  vector<EdgeInfo> allEdges;
  set<pair<int, int>> seen;

  for (int u = 0; u < NUM_CITIES; u++) {
    for (int i = 0; i < (int)adjList[u].size(); i++) {
      int v = adjList[u][i].first;
      int a = min(u, v);
      int b = max(u, v);
      if (seen.count({a, b}) == 0) {
        allEdges.push_back({a, b});
        seen.insert({a, b});
      }
    }
  }

  int mostCriticalEdgeU = -1, mostCriticalEdgeV = -1;
  int maxComponentsByEdge = baseCount;

  for (int i = 0; i < (int)allEdges.size(); i++) {
    set<pair<int, int>> disabledEdge;
    disabledEdge.insert({allEdges[i].u, allEdges[i].v});

    int compCount = countComponentsWithDisabledEdges(disabledEdge);

    if (compCount > maxComponentsByEdge) {
      maxComponentsByEdge = compCount;
      mostCriticalEdgeU = allEdges[i].u;
      mostCriticalEdgeV = allEdges[i].v;
    }
  }

  cout << "  Edges tested: " << allEdges.size() << endl;
  cout << endl;

  if (mostCriticalEdgeU >= 0) {
    cout << "  Most Critical Road:" << endl;
    cout << "    (" << mostCriticalEdgeU << ", " << mostCriticalEdgeV << ")" << endl;
    cout << endl;
    cout << "  Components after removal: " << maxComponentsByEdge << endl;
  } else {
    cout << "  No critical road found (removing any single edge doesn't" << endl;
    cout << "  increase disconnected components)." << endl;
  }

  cout << endl;

  // Final diagnostic output
  printSeparator('-', 55);
  cout << "  Critical Diagnostic Output:" << endl;
  printSeparator('-', 55);

  if (mostCriticalCity >= 0) {
    cout << "  [Removal of City " << mostCriticalCity
         << " breaks network graph into " << maxDisconnectedByCity
         << " disconnected subgrids]" << endl;
  }
  if (mostCriticalEdgeU >= 0) {
    cout << "  [Removal of Road (" << mostCriticalEdgeU << ", "
         << mostCriticalEdgeV << ") splits network into "
         << maxComponentsByEdge << " components]" << endl;
  }
}

// ============================================================
//  COMPLEXITY ANALYSIS SUMMARY
// ============================================================

void printComplexityAnalysis() {
  printHeader("COMPLEXITY ANALYSIS SUMMARY");

  cout << "  +--------------------------------------+--------------------+-----------+" << endl;
  cout << "  | Algorithm                            | Time Complexity    | Space     |" << endl;
  cout << "  +--------------------------------------+--------------------+-----------+" << endl;
  cout << "  | Network Generation                   | O(V * k)           | O(V + E)  |" << endl;
  cout << "  | Kruskal's MST (Union-Find)           | O(E log E)         | O(V + E)  |" << endl;
  cout << "  | Degree Centrality Ranking            | O(V log V)         | O(V)      |" << endl;
  cout << "  | Dijkstra (Disaster Recovery)         | O((V + E) log V)   | O(V)      |" << endl;
  cout << "  | Dijkstra (Traffic-Aware)             | O((V + E) log V)   | O(V + E)  |" << endl;
  cout << "  | Node Elimination (Critical Infra)    | O(V * (V + E))     | O(V)      |" << endl;
  cout << "  | Edge Elimination (Critical Infra)    | O(E * (V + E))     | O(V + E)  |" << endl;
  cout << "  +--------------------------------------+--------------------+-----------+" << endl;
  cout << endl;
  cout << "  V = " << NUM_CITIES << " cities  |  E = " << totalEdges
       << " roads  |  k = max connections per city" << endl;
}

// ============================================================
//  MAIN
// ============================================================

int main() {
  srand(time(0));

  // Banner
  cout << endl;
  printSeparator('*', 65);
  cout << "     SMART TRANSPORTATION NETWORK — FINAL ASSIGNMENT" << endl;
  cout << "     Graph Theory Based Infrastructure Analysis" << endl;
  cout << "     Cities: " << NUM_CITIES << "  |  Regions: " << NUM_REGIONS
       << "  |  Weights: " << MIN_WEIGHT << "-" << MAX_WEIGHT << endl;
  printSeparator('*', 65);
  cout << endl;

  // Network generation (reused from Assignment 2)
  printHeader("NETWORK GENERATION");

  cout << "  Configuration:" << endl;
  cout << "    Total Cities          : " << NUM_CITIES << endl;
  cout << "    Geographical Regions  : " << NUM_REGIONS << endl;
  cout << "    Connections per City  : " << MIN_CONN << " to " << MAX_CONN << endl;
  cout << "    Inter-Region Bridges  : " << MIN_BRIDGES << " to " << MAX_BRIDGES
       << endl;
  cout << "    Edge Weight Range     : " << MIN_WEIGHT << " to " << MAX_WEIGHT
       << endl;
  cout << endl;

  generateNetwork();
  printNetworkStats();

  // Execute all 5 tasks sequentially
  task1_MSTOptimization();
  task2_StrategicCityIdentification();
  task3_DisasterRecoveryRouting();
  task4_TrafficAwareRouting();
  task5_CriticalInfrastructureAnalysis();

  // Complexity analysis
  printComplexityAnalysis();

  cout << endl;
  cout << "  Program completed successfully." << endl;
  cout << endl;

  return 0;
}
