#include "graph.h"

#include <unordered_set>
#include <queue>
#include <string>
#include <iostream>

template <typename NodeVal>
using Graph = ZLib::Graph<NodeVal>;
using NodeID = ZLib::NodeID;

struct MyValue {
    int i_;
    std::string s_;

    MyValue() = default;
    MyValue(int i, std::string s) : i_(i), s_(std::move(s)) {}
    MyValue(MyValue&&) = default;
    MyValue& operator=(MyValue&&) = default;
};

namespace std {
    std::string to_string(const MyValue& val) {
        std::string s;
        s.append("i: ");
        s.append(std::to_string(val.i_));
        s.append(" ,s: ");
        s.append(val.s_);
        return s;
    }
}

template<typename V>
void PrintGraph(const Graph<V>& g) {
    for (const auto& nodeId : g.GetAllNodes()) {
        const auto* val = g.GetNode(nodeId);
        const auto neighbors = g.GetNeighbors(nodeId);
        printf("Node ID: %llu, Value: [%s], Neighbors: ", nodeId, std::to_string(*val).c_str());
        for (NodeID i : neighbors) {
            printf("%llu ", i);
        }
        printf("\n");
    }
    printf("\n");
}

template<typename V>
void PrintAllNodeVals(const Graph<V>& g) {
    for (const auto nodeId : g.GetAllNodes()) {
        const auto* valPtr = g.GetNode(nodeId);
        printf("Node: %s\n", std::to_string(*valPtr).c_str());
    }
    printf("\n");
}

template<typename V, typename... Args>
void LinkNodes(Graph<V>& g, int target, Args... links) {
    g.LinkNodes(target, std::vector<NodeID>{static_cast<NodeID>(links)...});
}

template<typename V, typename... Args>
void UnlinkNodes(Graph<V>& g, int target, Args... links) {
    g.UnlinkNodes(target, std::vector<NodeID>{static_cast<NodeID>(links)...});
}

void SimpleAPITest() {
    Graph<MyValue> g;

    int numNodes = 10;
    const int asciiA = 97;
    for (int i = 0; i < numNodes; i++) {
        std::string s;
        s += (char)(i + asciiA);
        g.AddNode(MyValue(i, s), i);
    }

    g.LinkNodes(0, std::vector<NodeID>{1, 2});
    PrintGraph(g);

    LinkNodes(g, 1, 5, 7, 9);
    LinkNodes(g, 2, 8, 9);
    LinkNodes(g, 3, 4, 5, 6, 7);
    LinkNodes(g, 4, 0, 6);
    PrintGraph(g);

    UnlinkNodes(g, 1, 9);
    UnlinkNodes(g, 3, 7);
    UnlinkNodes(g, 5, 1, 3);
    PrintGraph(g);

    g.RemoveNode(4);
    PrintGraph(g);

    PrintAllNodeVals(g);
    g.GetNodeMutable(5)->s_ = "x";
    PrintAllNodeVals(g);

    g.AddNode(MyValue(100, "test"), 10);
    PrintGraph(g);
}

std::unordered_map<int, std::pair<int, int>> GetGraphShortestDistancesFromSource(const Graph<int>& g, int startNode) {
    std::unordered_map<int, std::pair<int, int>> paths; //<id, <prev, hops>>
    std::unordered_set<int> visited;
    std::queue<int> queue;
    queue.push(startNode);
    paths[startNode] = std::make_pair(-1, 0);

    while (!queue.empty()) {
        auto curr = queue.front();
        queue.pop();
        if (visited.find(curr) != visited.end())
            continue;
        visited.insert(curr);

        for (const auto neighbor : g.GetNeighbors(curr)) {
            queue.push(neighbor);

            auto currTotalHops = paths.at(curr).second;
            if (paths.find(neighbor) == paths.end() || paths.at(neighbor).second > currTotalHops + 1) {
                paths[neighbor] = std::make_pair(curr, currTotalHops + 1);
            }
        }
    }

    return paths;
}

void PrintGraphShortestDistances(std::unordered_map<int, std::pair<int, int>> paths) {
    for (const auto& [node, prevHopsPair] : paths) {
        printf("Node: %d, Prev: %d, Hops %d\n", node, prevHopsPair.first, prevHopsPair.second);
    }
    printf("\n");
}

void BFSTest() {
    Graph<int> g;
    for (int i = 0; i <= 10; i++) {
        g.AddNode((int)i, i);
    }
    LinkNodes(g, 0, 1, 2);
    LinkNodes(g, 1, 2);
    LinkNodes(g, 3, 2, 4, 6);
    LinkNodes(g, 4, 3, 5, 8);
    LinkNodes(g, 6, 3, 5, 7, 8);
    LinkNodes(g, 8, 4, 7, 9);
    LinkNodes(g, 9, 8, 10);
    PrintGraph(g);

    auto paths = GetGraphShortestDistancesFromSource(g, 0);
    PrintGraphShortestDistances(paths);

    LinkNodes(g, 9, 0);
    paths = GetGraphShortestDistancesFromSource(g, 0);
    PrintGraphShortestDistances(paths);

    auto pathsFromNodeTen = GetGraphShortestDistancesFromSource(g, 10);
    PrintGraphShortestDistances(pathsFromNodeTen);
}

int main() {
    BFSTest();
}