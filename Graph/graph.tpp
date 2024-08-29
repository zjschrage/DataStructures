#include "graph.h"

#ifdef DEBUG
#include <cstdio>
#define LOG(msg) printf(msg)
#define LOG_VA(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define LOG(msg)
#define LOG_VA(fmt, ...)
#endif

namespace ZLib {

    template<typename NodeVal>
    NodeID Graph<NodeVal>::AddNode(NodeVal&& nodeVal) {
        std::scoped_lock lock(graphMutex_);
        
        NodeID id = currNodeId_++;
        AddNode(std::move(nodeVal), id);
        
        return id;
    }

    template<typename NodeVal>
    bool Graph<NodeVal>::AddNode(NodeVal&& nodeVal, NodeID nodeId) {
        std::scoped_lock lock(graphMutex_);
        
        if (GetNode(nodeId) != nullptr)
            return false;
        
        Node<NodeVal> node{nodeId, std::move(nodeVal), {}};
        graph_.emplace(nodeId, std::move(node));
        
        LOG_VA("Created node. ID: %llu\n", nodeId);
        return true;
    }

    template<typename NodeVal>
    int Graph<NodeVal>::RemoveNode(NodeID nodeId) {
        std::scoped_lock lock(graphMutex_);
        
        if (graph_.find(nodeId) == graph_.end())
            return -1;
        
        auto& remVal = graph_[nodeId];
        auto copyOfLinks = remVal.links_; //can be optimized
        
        int unlinks = UnlinkNodes(remVal.id_, copyOfLinks);
        graph_.erase(nodeId);
        
        LOG_VA("Removed node. ID: %llu\n", nodeId);
        return unlinks;
    }

    template<typename NodeVal>
    template<typename Container>
    int Graph<NodeVal>::LinkNodes(NodeID targetId, const Container& links) {
        std::scoped_lock lock(graphMutex_);
        
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
            LOG_VA("Linked nodes %llu and %llu\n", targetId, link);
        }

        LOG_VA("Linked %d nodes\n", linksCount);
        return linksCount;
    }

    template<typename NodeVal>
    template<typename Container>
    int Graph<NodeVal>::UnlinkNodes(NodeID targetId, const Container& links) {
        std::scoped_lock lock(graphMutex_);
        
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
            LOG_VA("Unlinked nodes %llu and %llu\n", targetId, link);
        }

        LOG_VA("Unlinked %d nodes\n", unlinksCount);
        return unlinksCount;
    }

    template<typename NodeVal>
    NodeIDs Graph<NodeVal>::GetAllNodes() const {
        std::scoped_lock lock(graphMutex_);
        
        NodeIDs allNodes;
        for (const auto& [nodeId, node] : graph_) {
            allNodes.push_back(nodeId);
        }
        
        LOG_VA("Retrieved %lu nodes from graph\n", allNodes.size());
        return allNodes;
    }

    template<typename NodeVal>
    NodeVal* Graph<NodeVal>::GetNodeMutable(NodeID targetId) {
        std::scoped_lock lock(graphMutex_);

        if (graph_.find(targetId) == graph_.end())
            return nullptr;

        LOG_VA("Retrieved value from node id %llu\n", targetId);
        return &graph_.at(targetId).val_;
    }

    template<typename NodeVal>
    const NodeVal* Graph<NodeVal>::GetNode(NodeID targetId) const {
        std::scoped_lock lock(graphMutex_);

        if (graph_.find(targetId) == graph_.end())
            return nullptr;
        
        LOG_VA("Retrieved value from node id %llu\n", targetId);
        return &graph_.at(targetId).val_;
    }

    template<typename NodeVal>
    NodeIDs Graph<NodeVal>::GetNeighbors(NodeID targetId) const {
        std::scoped_lock lock(graphMutex_);
        NodeIDs allNeighbors;

        if (graph_.find(targetId) == graph_.end())
            return allNeighbors;

        const Node<NodeVal>& node = graph_.at(targetId);
        for (const auto& link : node.links_) {
            allNeighbors.push_back(link);
        }

        LOG_VA("Retrieved %lu neighbors from graph\n", allNeighbors.size());
        return allNeighbors;
    }

}