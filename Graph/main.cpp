#include "graph.h"

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

    std::string to_string() const {
        std::string s;
        s.append("i: ");
        s.append(std::to_string(i_));
        s.append(" ,s: ");
        s.append(s_);
        return s;
    }
};

std::ostream& operator<<(std::ostream& os, const MyValue& obj) {
    os << obj.to_string();
    return os;
}

template<typename V>
void PrintGraph(Graph<V>& g) {
    for (const auto& nodeId : g.GetAllNodes()) {
        auto* val = g.GetNode(nodeId);
        const auto neighbors = g.GetNeighbors(nodeId);
        printf("Node ID: %llu, Value: [%s], Neighbors: ", nodeId, val->to_string().c_str());
        for (NodeID i : neighbors) {
            printf("%llu ", i);
        }
        printf("\n");
    }
    printf("\n");
}

template<typename V, typename... Args>
void LinkNodes(Graph<V>& g, int target, Args... links) {
    g.LinkNodes(target, std::vector<NodeID>{links...});
}

template<typename V, typename... Args>
void UnlinkNodes(Graph<V>& g, int target, Args... links) {
    g.UnlinkNodes(target, std::vector<NodeID>{links...});
}

int main() {
    Graph<MyValue> g;
    std::unordered_map<int, int> nodeTracker;

    //Add initial nodes
    int offset = 100;
    int numNodes = 10;
    const int asciiA = 97;
    for (int i = offset; i < offset + numNodes; i++) {
        std::string s;
        s += (char)(i - offset + asciiA);
        int nodeId = g.AddNode(MyValue(i, s));
        nodeTracker[i] = nodeId;
    }

    g.LinkNodes(nodeTracker[offset + 0], std::vector<NodeID>{(NodeID)nodeTracker[offset + 1], (NodeID)nodeTracker[offset + 2]});
    PrintGraph(g);

    LinkNodes(g, nodeTracker[offset + 1], (NodeID)nodeTracker[offset + 5], (NodeID)nodeTracker[offset + 7], (NodeID)nodeTracker[offset + 9]);
    LinkNodes(g, nodeTracker[offset + 2], (NodeID)nodeTracker[offset + 8], (NodeID)nodeTracker[offset + 9]);
    LinkNodes(g, nodeTracker[offset + 3], (NodeID)nodeTracker[offset + 4], (NodeID)nodeTracker[offset + 5], (NodeID)nodeTracker[offset + 6], (NodeID)nodeTracker[offset + 7]);
    LinkNodes(g, nodeTracker[offset + 4], (NodeID)nodeTracker[offset + 0], (NodeID)nodeTracker[offset + 6]);
    PrintGraph(g);

    UnlinkNodes(g, nodeTracker[offset + 1], (NodeID)nodeTracker[offset + 9]);
    UnlinkNodes(g, nodeTracker[offset + 3], (NodeID)nodeTracker[offset + 7]);
    UnlinkNodes(g, nodeTracker[offset + 5], (NodeID)nodeTracker[offset + 1], (NodeID)nodeTracker[offset + 3]);
    PrintGraph(g);

    g.RemoveNode(nodeTracker[offset + 4]);
    PrintGraph(g);
}