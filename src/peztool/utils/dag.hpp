#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include "peztool/core/index_vector.hpp"


template<typename TID>
struct DAG
{
    using ID = TID;
    struct Node
    {
        uint32_t         incoming = 0;
        uint32_t         depth    = 0;
        std::vector<TID> out;

        [[nodiscard]]
        size_t getOutConnectionCount() const
        {
            return out.size();
        }
    };

    siv::Vector<Node> nodes;

    void createNode()
    {
        nodes.emplace_back();
    }

    bool setParent(TID child, TID parent)
    {
        return createConnection(parent, child);
    }

    bool createConnection(TID from, TID to)
    {
        // Ensure both nodes exist
        if (!isValid(from)) {
            return false;
        }
        if (!isValid(to)) {
            return false;
        }
        // Ensure there is no cycle
        if (from == to) {
            return false;
        }

        if (isAncestor(to, from)) {
            return false;
        }
        // Ensure the connection doesn't already exist
        if (isParent(from, to)) {
            return false;
        }
        // Add the connection in the parent node
        nodes[from].out.push_back(to);
        // Increase the incoming connections count of the child node
        nodes[to].incoming++;
        return true;
    }

    [[nodiscard]]
    bool isValid(TID i) const
    {
        return nodes.isValidID(i);
    }

    /// Checks if @p node_1 has @p node_2 in its children list
    [[nodiscard]]
    bool isParent(TID node_1, TID node_2) const
    {
        auto& out = nodes[node_1].out;
        return std::any_of(out.begin(), out.end(), [&](TID o) { return (o == node_2); });
    }

    /// Checks if @p node_1 is an ancestor of @p node_2
    [[nodiscard]]
    bool isAncestor(TID node_1, TID node_2) const
    {
        auto& out = nodes[node_1].out;
        return isParent(node_1, node_2) || std::any_of(out.begin(), out.end(), [&](TID o) {
            return isAncestor(o, node_2);
        });
    }

    /// Computes the depth of each node
    void computeDepth()
    {
        auto const node_count = nodes.size();
        // Nodes with no incoming edge
        std::vector<TID> start_nodes;
        // Current incoming edge state
        std::vector<TID> incoming;
        incoming.reserve(node_count);

        // Initialize incoming state
        for (auto const& n : nodes) {
            incoming.push_back(n.incoming);
        }

        // Initialize the set of nodes with no incoming edge
        uint32_t i{0};
        for (auto& n : nodes) {
            if (n.incoming == 0) {
                n.depth = 0;
                start_nodes.push_back(i);
            }
            ++i;
        }

        // Perform the sort
        while (!start_nodes.empty()) {
            // Extract a node from the starting set
            TID const idx = start_nodes.back();
            start_nodes.pop_back();

            // Remove incoming connection for all children of this node
            Node const& n = nodes[idx];
            for (auto const o : n.out) {
                incoming[o]--;
                Node& connected = nodes[o];
                connected.depth = std::max(connected.depth, n.depth + 1);
                // If a children has no incoming edge anymore, add it to the starting set
                if (incoming[o] == 0) {
                    start_nodes.push_back(o);
                }
            }
        }
    }

    /// Returns nodes indexes sorted topologically
    [[nodiscard]]
    std::vector<TID> getOrder() const
    {
        std::vector<TID> order(nodes.size());
        for (size_t i{0}; i < nodes.size(); ++i) {
            order[i] = i;
        }

        std::sort(order.begin(), order.end(), [this](TID a, TID b) {
            return nodes[a].depth < nodes[b].depth;
        });

        return order;
    }

    void removeConnection(TID from, TID to)
    {
        auto&        connections = nodes[from].out;
        size_t const count       = connections.size();
        size_t found = 0;
        for (size_t i{0}; i < count - found;) {
            if (connections[i] == to) {
                std::swap(connections[i], connections.back());
                connections.pop_back();
                --nodes[to].incoming;
                ++found;
            } else {
                ++i;
            }
        }

        if (!found) {
            std::cout << "[WARNING] Connection " << from << " -> " << to << " not found" << std::endl;
        }
    }
};
