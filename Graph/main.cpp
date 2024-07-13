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
    g.LinkNodes(target, std::vector<NodeID>{static_cast<NodeID>(links)...});
}

template<typename V, typename... Args>
void UnlinkNodes(Graph<V>& g, int target, Args... links) {
    g.UnlinkNodes(target, std::vector<NodeID>{static_cast<NodeID>(links)...});
}

int main() {
    Graph<MyValue> g;

    //Add initial nodes
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
}