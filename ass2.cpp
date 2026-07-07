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

using namespace std;



const int NUM_CITIES = 500; 
const int NUM_REGIONS = 25; 
const int MIN_WEIGHT = 5;   
const int MAX_WEIGHT = 50;  
const int MIN_CONN = 1;     
const int MAX_CONN = 2;     
const int MIN_BRIDGES = 3;  
const int MAX_BRIDGES = 8;  

std::vector<std::vector<std::pair<int, int>>> adjList(NUM_CITIES);


int totalEdges = 0;

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

void bfs(int start, std::vector<bool> &visited, std::vector<int> &component) {
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


std::vector<std::vector<int>> findAllComponents() {
  std::vector<std::vector<int>> components;
  std::vector<bool> visited(NUM_CITIES, false);

  for (int i = 0; i < NUM_CITIES; i++) {
    if (!visited[i]) {
      std::vector<int> component;
      bfs(i, visited, component);
      components.push_back(component);
    }
  }

  return components;
}

void generateNetwork() {
  
  std::vector<int> region(NUM_CITIES);
  for (int i = 0; i < NUM_CITIES; i++) {
    region[i] = rand() % NUM_REGIONS;
  }

  
  std::vector<std::vector<int>> regionMembers(NUM_REGIONS);
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
    std::vector<int> &members = regionMembers[r];
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

  
  std::vector<std::vector<int>> components = findAllComponents();

  if ((int)components.size() <= 1) {
    cout << "  [!] WARNING: Only 1 component detected." << endl;
    cout << "      The graph is too connected." << endl;
    cout << "      Reduce MAX_CONN or MAX_BRIDGES for more clusters." << endl;
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
  std::vector<int> degrees(NUM_CITIES);

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

  std::vector<int> degFreq(maxDeg + 1, 0);
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

void connectivityAnalysis() {
  std::vector<std::vector<int>> components = findAllComponents();

  
  int largestIdx = 0;
  for (int i = 1; i < (int)components.size(); i++) {
    if (components[i].size() > components[largestIdx].size()) {
      largestIdx = i;
    }
  }

  std::vector<int> &largest = components[largestIdx];
  sort(largest.begin(), largest.end());

  double percentage = (100.0 * largest.size()) / NUM_CITIES;

  cout << "  Largest Connected Component:" << endl;
  printSeparator('-', 50);
  cout << "    Size       : " << largest.size() << " cities" << endl;
  cout << "    Coverage   : " << fixed << setprecision(1) << percentage
       << "% of total network" << endl;
  cout << endl;

  
  int displayLimit = min((int)largest.size(), 50);
  cout << "  Cities in Largest Component";
  if ((int)largest.size() > displayLimit) {
    cout << " (showing " << displayLimit << " of " << largest.size() << ")";
  }
  cout << ":" << endl << "    ";

  for (int i = 0; i < displayLimit; i++) {
    cout << setw(4) << largest[i];
    if ((i + 1) % 12 == 0 && i + 1 < displayLimit)
      cout << endl << "    ";
  }
  if ((int)largest.size() > displayLimit)
    cout << " ...";
  cout << endl;
}

void clusterDetection() {
  std::vector<std::vector<int>> components = findAllComponents();

  
  sort(components.begin(), components.end(),
       [](const vector<int> &a, const vector<int> &b) {
         return a.size() > b.size();
       });

  int totalComponents = components.size();
  int largestSize = components.front().size();
  int smallestSize = components.back().size();

  
  int isolatedCount = 0;
  for (int i = 0; i < (int)components.size(); i++) {
    if ((int)components[i].size() == 1)
      isolatedCount++;
  }

  cout << "  Cluster Overview:" << endl;
  printSeparator('-', 50);
  cout << "    Total Connected Components : " << totalComponents << endl;
  cout << "    Largest Cluster Size       : " << largestSize << " cities"
       << endl;
  cout << "    Smallest Cluster Size      : " << smallestSize << " city/cities"
       << endl;
  cout << "    Isolated Cities (size = 1) : " << isolatedCount << endl;
  cout << endl;

  
  int largeClusters = 0;  
  int mediumClusters = 0; 
  int smallClusters = 0;  
  int tinyClusters = 0;   

  for (int i = 0; i < (int)components.size(); i++) {
    int sz = components[i].size();
    if (sz >= 40)
      largeClusters++;
    else if (sz >= 15)
      mediumClusters++;
    else if (sz >= 5)
      smallClusters++;
    else
      tinyClusters++;
  }

  cout << "  Cluster Size Categories:" << endl;
  printSeparator('-', 50);
  cout << "    Large   (>= 40 cities) : " << largeClusters << " clusters"
       << endl;
  cout << "    Medium  (15-39 cities)  : " << mediumClusters << " clusters"
       << endl;
  cout << "    Small   (5-14 cities)   : " << smallClusters << " clusters"
       << endl;
  cout << "    Tiny    (< 5 cities)    : " << tinyClusters << " clusters"
       << endl;
  cout << endl;

  
  cout << "  All Clusters (sorted by size, descending):" << endl;
  printSeparator('-', 65);
  cout << "    " << setw(14) << left << "Cluster" << setw(8) << "Size"
       << "Sample Cities" << endl;
  printSeparator('-', 65);

  for (int i = 0; i < totalComponents; i++) {
    sort(components[i].begin(), components[i].end());

    string label = "Cluster " + to_string(i + 1);
    cout << "    " << setw(14) << left << label << setw(8)
         << components[i].size();

    
    int showCount = min((int)components[i].size(), 10);
    for (int j = 0; j < showCount; j++) {
      cout << components[i][j];
      if (j < showCount - 1)
        cout << ", ";
    }
    if ((int)components[i].size() > 10)
      cout << " ...";
    cout << endl;
  }
}

void dijkstraShortestPath() {
  
  int source = -1;
  for (int attempt = 0; attempt < 200; attempt++) {
    int candidate = rand() % NUM_CITIES;
    if (!adjList[candidate].empty()) {
      source = candidate;
      break;
    }
  }

  if (source == -1) {
    cout << "  [ERROR] No connected city found for Dijkstra!" << endl;
    return;
  }

  cout << "  Source City: " << source
       << "  (degree = " << adjList[source].size() << ")" << endl;
  cout << endl;

  
  std::vector<int> dist(NUM_CITIES, INT_MAX);     
  std::vector<int> parent(NUM_CITIES, -1);        
  std::vector<bool> processed(NUM_CITIES, false); 

  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      pq;

  dist[source] = 0;
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

      
      if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        parent[v] = u;
        pq.push({dist[v], v});
      }
    }
  }

  
  int reachableCount = 0;
  int unreachableCount = 0;
  int farthestCity = source;
  int farthestDist = 0;

  for (int i = 0; i < NUM_CITIES; i++) {
    if (dist[i] != INT_MAX) {
      reachableCount++;
      if (dist[i] > farthestDist) {
        farthestDist = dist[i];
        farthestCity = i;
      }
    } else {
      unreachableCount++;
    }
  }

  cout << "  Dijkstra Results Summary:" << endl;
  printSeparator('-', 50);
  cout << "    Reachable Cities   : " << reachableCount << endl;
  cout << "    Unreachable Cities : " << unreachableCount
       << " (in different clusters)" << endl;
  cout << "    Farthest City      : " << farthestCity
       << " (distance = " << farthestDist << ")" << endl;
  cout << endl;

  
  cout << "  Shortest Distance Table (sample of reachable cities):" << endl;
  printSeparator('-', 60);
  cout << "    " << setw(8) << left << "City" << setw(12) << "Distance"
       << "Shortest Path" << endl;
  printSeparator('-', 60);

  int printed = 0;
  for (int i = 0; i < NUM_CITIES && printed < 25; i++) {
    if (dist[i] != INT_MAX && i != source) {
      cout << "    " << setw(8) << left << i << setw(12) << dist[i];

      
      std::vector<int> path;
      int curr = i;
      int pathLimit = 10; 

      while (curr != -1 && (int)path.size() < pathLimit) {
        path.push_back(curr);
        curr = parent[curr];
      }

      reverse(path.begin(), path.end());

      for (int j = 0; j < (int)path.size(); j++) {
        cout << path[j];
        if (j < (int)path.size() - 1)
          cout << " -> ";
      }
      if (curr != -1)
        cout << " -> ...";

      cout << endl;
      printed++;
    }
  }

  
  cout << endl;
  cout << "  Full Path to Farthest City (" << farthestCity << "):" << endl;
  printSeparator('-', 50);

  std::vector<int> farthestPath;
  int curr = farthestCity;
  while (curr != -1) {
    farthestPath.push_back(curr);
    curr = parent[curr];
  }
  reverse(farthestPath.begin(), farthestPath.end());

  cout << "    Hops: " << farthestPath.size() - 1 << endl;
  cout << "    Path: ";
  for (int j = 0; j < (int)farthestPath.size(); j++) {
    cout << farthestPath[j];
    if (j < (int)farthestPath.size() - 1)
      cout << " -> ";
    if ((j + 1) % 10 == 0 && j + 1 < (int)farthestPath.size())
      cout << endl << "          ";
  }
  cout << endl;

  
  cout << endl;
  cout << "  Sample Unreachable Cities (up to 30):" << endl;
  printSeparator('-', 50);
  cout << "    ";

  printed = 0;
  for (int i = 0; i < NUM_CITIES && printed < 30; i++) {
    if (dist[i] == INT_MAX) {
      cout << setw(4) << i;
      printed++;
      if (printed % 12 == 0 && printed < 30)
        cout << endl << "    ";
    }
  }

  if (unreachableCount > 30)
    cout << " ... [+" << (unreachableCount - 30) << " more]";
  cout << endl;
}


int main() {
  
  srand(time(0));

  
  cout << endl;
  cout << "     SMART TRANSPORTATION NETWORK DESIGN                  " << endl;
  cout << "     Graph Theory Based Analysis                          " << endl;
  cout << "     Cities: " << NUM_CITIES << "  |  Regions: " << NUM_REGIONS
       << "  |  Weights: " << MIN_WEIGHT << "-" << MAX_WEIGHT << endl;
  cout << endl;

  

  cout << "     TASK A: NETWORK GENERATION                           " << endl;
  cout << endl;
  cout << "  Configuration:" << endl;
  cout << "    Total Cities          : " << NUM_CITIES << endl;
  cout << "    Geographical Regions  : " << NUM_REGIONS << endl;
  cout << "    Connections per City  : " << MIN_CONN << " to " << MAX_CONN
       << endl;
  cout << "    Inter-Region Bridges  : " << MIN_BRIDGES << " to " << MAX_BRIDGES
       << endl;
  cout << "    Edge Weight Range     : " << MIN_WEIGHT << " to " << MAX_WEIGHT
       << endl;
  cout << endl;

  generateNetwork();
  printNetworkStats();

  
  cout << endl;
  cout << "     TASK B: CONNECTIVITY ANALYSIS (BFS)                  " << endl;
  cout << endl;

  connectivityAnalysis();

  
  cout << endl;
  cout << "     TASK C: CLUSTER DETECTION                            " << endl;
  cout << endl;

  clusterDetection();

  
  cout << endl;
  cout << "     TASK D: DIJKSTRA'S SHORTEST PATH                     " << endl;
  cout << endl;

  dijkstraShortestPath();

  
  cout << endl;
  cout << "     COMPLEXITY ANALYSIS SUMMARY                          " << endl;
  cout << endl;
  cout << "  +------------------------+--------------------+-----------+"
       << endl;
  cout << "  | Algorithm              | Time Complexity    | Space     |"
       << endl;
  cout << "  +------------------------+--------------------+-----------+"
       << endl;
  cout << "  | Network Generation     | O(V * k)           | O(V + E)  |"
       << endl;
  cout << "  | BFS Traversal          | O(V + E)           | O(V)      |"
       << endl;
  cout << "  | Connected Components   | O(V + E)           | O(V)      |"
       << endl;
  cout << "  | Dijkstra (Priority Q)  | O((V + E) log V)   | O(V)      |"
       << endl;
  cout << "  +------------------------+--------------------+-----------+"
       << endl;
  cout << endl;
  cout << "  V = " << NUM_CITIES << " cities  |  E = " << totalEdges
       << " roads  |  k = max connections per city" << endl;
  cout << endl;
  return 0;
}