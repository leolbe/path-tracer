//
// Created by Leonard on 2025-02-06.
//

#ifndef AABBBVH_TPP
#define AABBBVH_TPP

#include "AabbBvh.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <utility>

#include "../math/aabb_util.h"
#include "../config.h"
#include "../scene/scene_objects/BoxObject.h"



template<AabbBvhType T, AabbBvhOps<T> Ops>
AabbBvh<T, Ops>::Node::Node(Connection connection, Aabb aabb)
  : connection(std::move(connection)), aabb(std::move(aabb)) {}

template<AabbBvhType T, AabbBvhOps<T> Ops>
void AabbBvh<T, Ops>::Node::split(Axis axis) {
    if constexpr (DEBUG_ASSERTS)
        assert(std::holds_alternative<Leaf>(connection));

    const auto objects = std::get<Leaf>(connection);
    std::ranges::sort(
        objects,
        [this, axis](const T &a, const T &b) {
            const Aabb a_aabb = Ops::get_aabb(a);
            const Aabb b_aabb = Ops::get_aabb(b);
            // sort by aabb centerpoint coordinate
            return a_aabb.min.scalar_component(axis) + a_aabb.max.scalar_component(axis)
                 < b_aabb.min.scalar_component(axis) + b_aabb.max.scalar_component(axis);
        }
    );
    const size_t object_count = objects.size();

    if constexpr (DEBUG_ASSERTS)
        assert(object_count >= 2);

    const auto min_objects = objects.subspan(0, object_count / 2);
    const auto max_objects = objects.subspan(object_count / 2, object_count - object_count / 2);

    auto min_child = std::make_unique<Node>(Leaf(min_objects), Ops::wrap_aabb(min_objects));
    auto max_child = std::make_unique<Node>(Leaf(max_objects), Ops::wrap_aabb(max_objects));

    std::array<std::unique_ptr<Node>, 2> children = {std::move(min_child), std::move(max_child)};
    connection.template emplace<Branch>(axis, std::move(children));
}

/**
 * Split @c node until either the @c subtree_max_height reaches @c 0 or the leaves get smaller than @c min_leaf_size.
 */
template<AabbBvhType T, AabbBvhOps<T> Ops>
void split_recursively(typename AabbBvh<T, Ops>::Node &node, const uint32_t subtree_max_height,
    const uint32_t min_leaf_size)
{
    using Leaf = typename AabbBvh<T, Ops>::Node::Leaf;
    using Branch = typename AabbBvh<T, Ops>::Node::Branch;

    if constexpr (DEBUG_ASSERTS)
        assert(std::holds_alternative<Leaf>(node.connection));

    if (subtree_max_height == 0)
        return;
    if (std::get<Leaf>(node.connection).size() <= min_leaf_size)
        return;

    // TODO more efficient axis selection
    Axis axis;
    switch (subtree_max_height % 3) {
    case 0:
        axis = X;
        break;
    case 1:
        axis = Y;
        break;
    default:
        axis = Z;
        break;
    }
    node.split(axis);

    if constexpr (DEBUG_ASSERTS)
        assert(std::holds_alternative<Branch>(node.connection));

    auto &[_, children] = std::get<Branch>(node.connection);
    split_recursively<T, Ops>(*children[0], subtree_max_height - 1, min_leaf_size);
    split_recursively<T, Ops>(*children[1], subtree_max_height - 1, min_leaf_size);
}

template<AabbBvhType T, AabbBvhOps<T> Ops>
AabbBvh<T, Ops>::AabbBvh(std::vector<T> &&objects, const uint32_t max_tree_height, const uint32_t min_leaf_size)
  : objects(std::move(objects)),
    root_node(std::make_unique<Node>(typename Node::Leaf(objects), Ops::wrap_aabb(objects))),
    traversal_stack(std::stack<TraversalRecord>())
{
    // TODO consider using a vec with pre-allocated memory for all the nodes
    // std::vector<Node> vec = new std::vector<Node>();
    split_recursively<T, Ops>(*root_node, max_tree_height, min_leaf_size);
}

template<AabbBvhType T, AabbBvhOps<T> Ops>
AabbBvh<T, Ops>::TraversalRecord::TraversalRecord(const Node &node) : node(node), second_pass(false) {}

template<AabbBvhType T, AabbBvhOps<T> Ops>
bool AabbBvh<T, Ops>::ray_cast(const Ray &ray, double &min_dist, Vec3 &pos, Vec3 &normal, const T *&hit_object) {
    while (!traversal_stack.empty())
        traversal_stack.pop();

    // start by iterating the root node
    traversal_stack.emplace(*root_node);

    // determines what child is checked for intersections first
    // negative sign ⇒ signbit = 1 ⇒ max child is checked first
    const size_t dir_indices[3] = {
        std::signbit(ray.dir.x),
        std::signbit(ray.dir.y),
        std::signbit(ray.dir.z),
    };

    bool hit = false;
    while (!traversal_stack.empty()) {
        TraversalRecord &current_record = traversal_stack.top();
        const Node &current_node = current_record.node;
        if (std::holds_alternative<typename Node::Leaf>(current_node.connection)) {
            // iterate all objects in leaf node to look for an intersection
            const auto objects = std::get<typename Node::Leaf>(current_node.connection);
            for (auto &object : objects) {
                // disregard object if its bounding box doesn't even intersect the ray
                if (!Ops::possibly_intersects(object, ray))
                    continue;
                // distance, pos and normal are updated if and only if this is the closest intersection yet
                const bool outside_hit = Ops::ray_cast(object, ray, min_dist, pos, normal);
                if (outside_hit) {
                    hit_object = &object;
                    hit = true;
                }
            }
            traversal_stack.pop();
        } else {
            const auto &[axis, children] = std::get<typename Node::Branch>(current_node.connection);
            // next node to iterate is determined by the ray direction
            size_t next_node_index = dir_indices[axis];

            // if the first child has already been iterated, iterate over the second one instead and remove the
            // parent node.
            if (current_record.second_pass) {
                next_node_index ^= 0x1;
                traversal_stack.pop();
            } else {
                current_record.second_pass = true;
            }

            const auto &next_node = children[next_node_index];

            const Aabb aabb = next_node->aabb;
            const double child_dist = aabb.ray_length(ray);

            // add the child node to the stack if its closest possible intersection is closer than the so-far lowest
            // intersection distance found
            if (child_dist <= min_dist && child_dist != -1)
                traversal_stack.emplace(*next_node);
        }
    }
    return hit;
}

template <AabbBvhType T, AabbBvhOps<T> Ops>
const std::vector<T>& AabbBvh<T, Ops>::get_objects() const {
    return objects;
}



#endif //AABBBVH_TPP
