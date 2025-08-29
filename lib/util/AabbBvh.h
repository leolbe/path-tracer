//
// Created by Leonard on 2025-02-06.
//

#ifndef AABBBVH_H
#define AABBBVH_H

#include <memory>
#include <span>
#include <variant>
#include <stack>
#include <vector>

#include "Axis.h"
#include "../math/Aabb.h"
#include "../math/Ray.h"
#include "../math/Vec3.h"



template<typename T>
concept AabbBvhType = std::swappable<T>;

template<typename Ops, typename T>
concept AabbBvhOps = requires(const std::span<T> span, const T &object, const Ray &ray, double &min_dist, Vec3 &pos,
    Vec3 &normal)
{
    { Ops::get_aabb(object) } -> std::same_as<Aabb>;
    { Ops::wrap_aabb(span) } -> std::same_as<Aabb>;
    { Ops::possibly_intersects(object, ray) } -> std::same_as<bool>;
    { Ops::ray_cast(object, ray, min_dist, pos, normal) } -> std::same_as<bool>;
};

/**
 * Acceleration data structure that allows for faster evaluation of ray-scene intersection tests. A BVH (bounding volume
 * hierarchy) using AABBs (axis-aligned bounding boxes) as the bounding volumes.
 * @tparam T Any type.
 * @tparam Ops A type that defines some required operations on T, as defined by @c AabbBvhOps.
 */
template<AabbBvhType T, AabbBvhOps<T> Ops>
class AabbBvh {
public:
    struct Node {
        using Leaf = std::span<T>;
        struct Branch {
            Axis axis;
            std::array<std::unique_ptr<Node>, 2> children;
        };

        using Connection = std::variant<Leaf, Branch>;
        Connection connection;

        Aabb aabb;

        /**
         * @param connection @c Leaf if the node contains a collection of objects. @c Branch if the node is a parent to
         * two other nodes.
         * @param aabb The smallest @c Aabb that contains all the children of this node.
         */
        Node(Connection connection, Aabb aabb);

        /**
         * Splits a leaf node into two leaf nodes, converting the current node to a branch node and assigning the new
         * leaves as children.
         * @param axis The axis to sort the objects along before performing the split across the axis.
         */
        void split(Axis axis);
    };

private:
    struct TraversalRecord {
        const Node &node;
        bool second_pass;

        explicit TraversalRecord(const Node &node);
    };

    std::vector<T> objects;

    std::unique_ptr<Node> root_node;

    std::stack<TraversalRecord> traversal_stack;

public:
    /**
     * @param objects Owning collection of the objects that are contained in this BVH.
     * @param max_tree_height Limits the number of times a single node may be split.
     * @param min_leaf_size Limits the number of objects required in a leaf node to be split.
     */
    AabbBvh(std::vector<T> &&objects, uint32_t max_tree_height, uint32_t min_leaf_size);

    /**
     * Find the nearest intersection with any object contained in the BVH.
     * @param min_dist The distance from the ray origin to the intersection point.
     * @param pos The intersection position vector.
     * @param normal The intersection normal vector.
     * @param hit_object The object that was hit.
     */
    bool ray_cast(const Ray &ray, double &min_dist, Vec3 &pos, Vec3 &normal, const T *&hit_object);

    const std::vector<T>& get_objects() const;
};



#endif //AABBBVH_H
