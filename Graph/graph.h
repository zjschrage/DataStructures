#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

namespace ZLib {

    using NodeID = uint64_t;
    using NodeIDs = std::vector<NodeID>;
    using NodeIDLinks = std::unordered_set<NodeID>;

    template<typename NodeVal>
    struct Node {
        NodeID id_;
        NodeVal val_;
        NodeIDLinks links_;
    };

    template<typename NodeVal>
    class Graph {
    public:
        //Adds a node to the graph and returns its assigned ID
        NodeID AddNode(NodeVal&& nodeVal);

        //Adds a node to the graph with custom ID
        //Returns true on success or false if node ID is already in use
        bool AddNode(NodeVal&& nodeVal, NodeID nodeId);

        //Removes a node from the graph
        //Returns the number of unlinks (0 - n) or -1 if no nodes were removed
        int RemoveNode(NodeID nodeId);

        //Links others nodes to a target node
        //Returns number of linkages made (0 - n) or -1 if targetId does not exist
        template<typename Container>
        int LinkNodes(NodeID targetId, const Container& links);

        //Unlinks others nodes to a target node
        //Returns number of linkages removed (0 - n) or -1 if targetId does not exist
        template<typename Container>
        int UnlinkNodes(NodeID targetId, const Container& links);

        //Returns all node ids in graph;
        NodeIDs GetAllNodes() const;

        //Returns node value from a node id
        NodeVal* GetNode(NodeID targetId);

        //Returns node ids of neighbors of a given node
        NodeIDs GetNeighbors(NodeID targetId) const;
        
    private:
        std::unordered_map<NodeID, Node<NodeVal>> graph_;
        NodeID currNodeId_{0};
        mutable std::recursive_mutex graphMutex_;
    };

}

#include "graph.tpp"
