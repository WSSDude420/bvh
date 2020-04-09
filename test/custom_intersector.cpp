#include <vector>
#include <iostream>

#include <bvh/bvh.hpp>
#include <bvh/vector.hpp>
#include <bvh/ray.hpp>
#include <bvh/binned_sah_builder.hpp>
#include <bvh/single_ray_traversal.hpp>
#include <bvh/intersectors.hpp>

using Scalar      = float;
using Vector3     = bvh::Vector3<Scalar>;
using BoundingBox = bvh::BoundingBox<Scalar>;
using Ray         = bvh::Ray<Scalar>;

int main() {
    // The input of the BVH construction algorithm is just bounding boxes and centers
    std::vector<BoundingBox> bboxes;
    std::vector<Vector3> centers;

    // Fill the bboxes and centers with the proper values taken from the geometry of interest
    // ...

    assert(bboxes.size() == centers.size());

    // Number of bins to evaluate the SAH.
    // Higher values means a more precise estimate of the SAH,
    // at a higher memory and computation cost. Values of 32 or
    // 64 are good enough for most scenes.
    static constexpr size_t bin_count = 64;

    // Maximum depth of the tree.
    // Increasing this value increases the traversal stack size.
    static constexpr size_t max_depth = 64;

    using Bvh = bvh::Bvh<Scalar, max_depth>;

    Bvh bvh;

    bvh::BinnedSahBuilder<Bvh, bin_count> builder(&bvh);
    builder.build(bboxes.data(), centers.data(), bboxes.size());

    // Boolean controlling whether the intersection routine exits
    // immediately after an intersection is found.
    static constexpr bool any_hit = false;

    // Intersectors are used by the traversal algorithm to intersect the primitives
    // the BVH. Since the BVH itself has no knowledge of the primitives, this structure
    // does the role of a proxy between the traversal algorithm and the primitive data.
    // It can also be used to cache precomputed intersection data, so as to speed up
    // primitive intersection.
    struct Intersector {
        // Required type: result of the intersection function
        struct Result {
            // More members can be added here
            int dummy = 42;

            // Required member: distance along the ray
            Scalar distance() {
                return std::numeric_limits<Scalar>::max();
            }
        };

        // Required member: intersect the primitive at index `bvh.primitive_indices[index]`
        std::optional<Result> operator () (size_t /*index*/, const Ray& /*ray*/) const {
            // Note: a common optimization is to reorder the primitives such that
            // there is no need for an indirection through `bvh.primitive_indices`.
            return std::nullopt;
        }
    };

    Intersector intersector;
    bvh::SingleRayTraversal<Bvh> traversal(&bvh);

    // Setup the ray (see above for an example)
    Ray ray(Vector3(0.0), Vector3(1.0), 0, 1);
    auto hit = traversal.intersect<any_hit>(ray, intersector);
    if (hit) {
        auto dummy_value = hit->dummy;

        // Do something with the intersection here
        // ...

        std::cout << "Dummy value: " << dummy_value << std::endl;
    }
    return 0;
}
