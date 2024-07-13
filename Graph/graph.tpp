#include "graph.h"

namespace ZLib {

    template<typename NodeVal>
    NodeID Graph<NodeVal>::AddNode(NodeVal&& nodeVal) {
        NodeID id = currNodeId_++;
        Node<NodeVal> node{id, std::move(nodeVal), {}};
        graph_.emplace(id, std::move(node));
        return id;
    }

    template<typename NodeVal>
    bool Graph<NodeVal>::AddNode(NodeVal&& nodeVal, NodeID nodeId) {
        if (GetNode(nodeId) != nullptr)
            return false;
            
        Node<NodeVal> node{nodeId, std::move(nodeVal), {}};
        graph_.emplace(nodeId, std::move(node));
        return true;
    }

    template<typename NodeVal>
    int Graph<NodeVal>::RemoveNode(NodeID nodeId) {
        if (graph_.find(nodeId) == graph_.end())
            return -1;
        
        auto& remVal = graph_[nodeId];
        auto copyOfLinks = remVal.links_; //can be optimized
        int unlinks = UnlinkNodes(remVal.id_, copyOfLinks);
        graph_.erase(nodeId);
        return unlinks;
    }

    template<typename NodeVal>
    template<typename Container>
    int Graph<NodeVal>::LinkNodes(NodeID targetId, const Container& links) {
        if (graph_.find(targetId) == graph_.end())
            return -1;

        auto& target = graph_[targetId];
        int linksCount = 0;
        for (const auto link : links) {
            if (graph_.find(link) == graph_.end())
                continue;

            target.links_.insert(link);
            graph_[link].links_.insert(targetId);
            linksCount++;
        }

        return linksCount;
    }

    template<typename NodeVal>
    template<typename Container>
    int Graph<NodeVal>::UnlinkNodes(NodeID targetId, const Container& links) {
        if (graph_.find(targetId) == graph_.end())
            return -1;

        auto& target = graph_[targetId];
        int unlinksCount = 0;
        for (const auto link : links) {
            if (graph_.find(link) == graph_.end())
                continue;

            target.links_.erase(link);
            graph_[link].links_.erase(targetId);
            unlinksCount++;
        }

        return unlinksCount;
    }

    template<typename NodeVal>
    NodeIDs Graph<NodeVal>::GetAllNodes() const {
        NodeIDs allNodes;
        for (const auto& [nodeId, node] : graph_) {
            allNodes.push_back(nodeId);
        }
        return allNodes;
    }

    template<typename NodeVal>
    NodeVal* Graph<NodeVal>::GetNode(NodeID targetId) {
        if (graph_.find(targetId) == graph_.end())
            return nullptr;

        return &graph_[targetId].val_;
    }

    template<typename NodeVal>
    NodeIDs Graph<NodeVal>::GetNeighbors(NodeID targetId) const {
        NodeIDs allNeighbors;

        if (graph_.find(targetId) == graph_.end())
            return allNeighbors;

        const Node<NodeVal>& node = graph_.at(targetId);
        for (const auto& link : node.links_) {
            allNeighbors.push_back(link);
        }

        return allNeighbors;
    }

}