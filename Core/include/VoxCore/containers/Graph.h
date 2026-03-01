//
// Created by deril on 2/25/26.
//

#pragma once

#include "Containers.h"

VOX_NS
    template<typename Value>
    struct GraphNode {
        int tempInDegree = 0;
        Vector<GraphNode *> next;
        Value *value;

        explicit GraphNode(Value* value) : value(value) {}
        explicit GraphNode(Value* value, Vector<GraphNode*> next) : value(value), next(std::move(next)) {}

        NO_COPY_MOVE_DEFAULT(GraphNode)

        static bool Dfs(int v, Vector<GraphNode<Value>> &adj, Vector<bool> &visited, Vector<bool> &recStack) {
            visited[v] = true;
            recStack[v] = true;

            for (int u : adj[v]) {
                if (!visited[u] && dfs(u, adj, visited, recStack)) return true;
                else if (recStack[u]) return true; // найден цикл
            }

            recStack[v] = false;
            return false;
        }

        static bool HasCycle(Vector<GraphNode<Value>> &adj, int n) {
            Vector<bool> visited(n, false);
            Vector<bool> recStack(n, false);

            for (int i = 0; i < n; i++) {
                if (!visited[i] && dfs(i, adj, visited, recStack)) return true;
            }
            return false;
        }

        static Vector<GraphNode<Value>*> TopologicalSort(Vector<GraphNode<Value>*> &nodes) {
            Vector<GraphNode<Value> *> result;
            result.reserve(nodes.size());

            for (auto *node: nodes)
                node->tempInDegree = 0;

            for (auto *node: nodes) {
                for (auto *next: node->next)
                    next->tempInDegree++;
            }

            std::queue<GraphNode<Value> *> q;

            for (auto *node: nodes) {
                if (node->tempInDegree == 0)
                    q.push(node);
            }

            while (!q.empty()) {
                auto *node = q.front();
                q.pop();

                result.push_back(node);

                for (auto *next: node->next) {
                    next->tempInDegree--;

                    if (next->tempInDegree == 0)
                        q.push(next);
                }
            }

            VOX_CHECK(result.size() != nodes.size(), "Cycle detected");
            return result;
        }
    };

NS_END
