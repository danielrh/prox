/*  libprox
 *  RTreeQueryHandler.cpp
 *
 *  Copyright (c) 2009, Ewen Cheslack-Postava
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of libprox nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <prox/RTreeQueryHandler.hpp>
#include <prox/BoundingSphere.hpp>
#include <cassert>
#include <float.h>

namespace Prox {

struct RTreeNode {
private:
    static const uint8 LeafFlag = 0x02; // elements are object pointers instead of node pointers

    union {
        RTreeNode** nodes;
        Object** objects;
        void** magic;
    } elements;
    RTreeNode* mParent;
    BoundingSphere3f bounding_sphere;
    uint8 flags;
    uint8 count;
    uint8 max_elements;

public:
    struct NodeChildOperations {
        RTreeNode* child(RTreeNode* parent, int idx) {
            return parent->node(idx);
        }

        BoundingSphere3f bounds(RTreeNode* child, const Time& ) {
            return child->bounds();
        }

        void insert(RTreeNode* parent, RTreeNode* newchild, const Time& ) {
            parent->insert(newchild);
        }
    };

    struct ObjectChildOperations {
        Object* child(RTreeNode* parent, int idx) {
            return parent->object(idx);
        }

        BoundingSphere3f bounds(Object* child, const Time& t) {
            return child->worldBounds(t);
        }

        void insert(RTreeNode* parent, Object* newchild, const Time& t) {
            parent->insert(newchild,t);
        }
    };


    RTreeNode(uint8 _max_elements)
     : mParent(NULL), bounding_sphere(), flags(0), count(0), max_elements(_max_elements)
    {
        elements.magic = new void*[max_elements];
        for(int i = 0; i < max_elements; i++)
            elements.magic[i] = NULL;

        leaf(true);
    }

    ~RTreeNode() {
        delete[] elements.magic;
    }

    bool leaf() const {
        return (flags & LeafFlag);
    }
    void leaf(bool d) {
        flags = (flags & ~LeafFlag) | (d ? LeafFlag : 0x00);
    }

    bool empty() const {
        return (count == 0);
    }
    bool full() const {
        return (count == max_elements);
    }
    uint8 size() const {
        return count;
    }
    uint8 capacity() const {
        return max_elements;
    }

    RTreeNode* parent() const {
        return mParent;
    }
    void parent(RTreeNode* _p) {
        mParent = _p;
    }

    Object* object(int i) const {
        assert( leaf() );
        assert( i < count );
        return elements.objects[i];
    }

    RTreeNode* node(int i) const {
        assert( !leaf() );
        assert( i < count );
        return elements.nodes[i];
    }

    const BoundingSphere3f& bounds() const {
        return bounding_sphere;
    }
    void bounds(const BoundingSphere3f& new_bounds) {
        bounding_sphere = new_bounds;
    }

    BoundingSphere3f childBounds(int i, const Time& t) {
        if (leaf())
            return object(i)->worldBounds(t);
        else
            return node(i)->bounds();
    }

    void recomputeBounds(const Time& t) {
        bounding_sphere = BoundingSphere3f();
        for(int i = 0; i < size(); i++)
            bounding_sphere.mergeIn( childBounds(i, t) );
    }

    void clear() {
        count = 0;
        for(int i = 0; i < max_elements; i++)
            elements.magic[i] = NULL;
        bounding_sphere = BoundingSphere3f();
    }

    void insert(Object* obj, const Time& t) {
        assert (count < max_elements);
        assert (leaf() == true);
        elements.objects[count] = obj;
        count++;
        bounding_sphere = bounding_sphere.merge(obj->worldBounds(t));
    }

    void insert(RTreeNode* node) {
        assert (count < max_elements);
        assert (leaf() == false);
        node->parent(this);
        elements.nodes[count] = node;
        count++;
        bounding_sphere = bounding_sphere.merge(node->bounds());
    }
};


RTreeNode* RTree_choose_leaf(RTreeNode* root, Object* obj, const Time& t) {
    BoundingSphere3f obj_bounds = obj->worldBounds(t);
    RTreeNode* node = root;

    while(!node->leaf()) {
        float min_increase = 0.f;
        RTreeNode* min_increase_node = NULL;

        for(int i = 0; i < node->size(); i++) {
            RTreeNode* child_node = node->node(i);
            BoundingSphere3f merged = child_node->bounds().merge(obj_bounds);
            float increase = merged.volume() - child_node->bounds().volume();
            if (min_increase_node == NULL || increase < min_increase) {
                min_increase = increase;
                min_increase_node = child_node;
            }
        }

        node = min_increase_node;
    }

    return node;
}

template<typename ChildType>
struct RTree_child_split_info {
    static const int32 unassigned = -1;

    RTree_child_split_info(ChildType* c, const BoundingBox3f& bs)
     : child(c), bounds(bs), group(unassigned) {}

    ChildType* child;
    BoundingSphere3f bounds;
    int32 group;
};

// Quadratic algorithm for picking node split seeds
template<typename ChildType>
void RTree_quadratic_pick_seeds(std::vector< RTree_child_split_info<ChildType> >& child_split_info, BoundingSphere3f* bounds0, BoundingSphere3f* bounds1) {
    float max_waste = -FLT_MAX;
    int32 seed0 = -1, seed1 = -1;
    for(uint32 idx0 = 0; idx0 < child_split_info.size(); idx0++) {
        for(uint32 idx1 = idx0+1; idx1 < child_split_info.size(); idx1++) {
            BoundingSphere3f merged = child_split_info[idx0].bounds.merge(child_split_info[idx1].bounds);

            float waste = merged.volume() - child_split_info[idx0].bounds.volume() - child_split_info[idx1].bounds.volume();

            if (waste > max_waste) {
                max_waste = waste;
                seed0 = idx0;
                seed1 = idx1;
            }
        }
    }
    assert( seed0 != -1 && seed1 != -1 );

    child_split_info[seed0].group = 0;
    child_split_info[seed1].group = 1;
    *bounds0 = child_split_info[seed0].bounds;
    *bounds1 = child_split_info[seed1].bounds;
}

// Choose the next child to assign to a group
template<typename ChildType>
void RTree_pick_next_child(std::vector<RTree_child_split_info<ChildType> >& child_split_info, BoundingSphere3f& group_bound_0, BoundingSphere3f& group_bound_1) {
    float max_preference = -1.0f;
    int32 max_idx = -1;
    int32 selected_group;

    for(uint32 i = 0; i < child_split_info.size(); i++) {
        if (child_split_info[i].group != RTree_child_split_info<ChildType>::unassigned) continue;

        BoundingSphere3f merged0 = group_bound_0.merge(child_split_info[i].bounds);
        BoundingSphere3f merged1 = group_bound_1.merge(child_split_info[i].bounds);

        float diff0 = merged0.volume() - child_split_info[i].bounds.volume();
        float diff1 = merged1.volume() - child_split_info[i].bounds.volume();

        float preference = fabs(diff0 - diff1);
        if (preference > max_preference) {
            max_preference = preference;
            max_idx = i;
            selected_group = (diff0 < diff1) ? 0 : 1;
        }
    }

    assert(max_idx != -1);

    child_split_info[max_idx].group = selected_group;
    if (selected_group == 0)
        group_bound_0 = group_bound_0.merge(child_split_info[max_idx].bounds);
    else
        group_bound_1 = group_bound_1.merge(child_split_info[max_idx].bounds);

    return ;
}

// Splits a node, inserting the given node, and returns the second new node
template<typename ChildType, typename ChildOperations>
RTreeNode* RTree_split_node(RTreeNode* node, ChildType* to_insert, const Time& t) {
    ChildOperations child_ops;

    // collect the info for the children
    std::vector< RTree_child_split_info<ChildType> > child_split_info;
    for(int i = 0; i < node->size(); i++)
        child_split_info.push_back( RTree_child_split_info<ChildType>(child_ops.child(node, i), node->childBounds(i,t)) );
    child_split_info.push_back( RTree_child_split_info<ChildType>( to_insert, child_ops.bounds(to_insert, t) ) );

    // find the initial seeds
    BoundingSphere3f group_bounds_0, group_bounds_1;
    RTree_quadratic_pick_seeds(child_split_info, &group_bounds_0, &group_bounds_1);

    // group the remaining ones
    for(uint32 i = 0; i < child_split_info.size()-2; i++)
        RTree_pick_next_child(child_split_info, group_bounds_0, group_bounds_1);

    // copy data into the correct nodes
    node->clear();
    RTreeNode* nn = new RTreeNode(node->capacity());
    nn->leaf(node->leaf());
    for(uint32 i = 0; i < child_split_info.size(); i++) {
        RTreeNode* newparent = (child_split_info[i].group == 0) ? node : nn;
        child_ops.insert( newparent, child_split_info[i].child, t );
    }

    return nn;
}

// Fixes up the tree after insertion. Returns the new root node
RTreeNode* RTree_adjust_tree(RTreeNode* L, RTreeNode* LL, const Time& t) {
    assert(L->leaf());
    RTreeNode* node = L;
    RTreeNode* nn = LL;

    while(node->parent() != NULL) {
        RTreeNode* parent = node->parent();

        // FIXME this is inefficient
        node->recomputeBounds(t);

        RTreeNode* pp = NULL;
        if (nn != NULL) {
            if (parent->full())
                pp = RTree_split_node<RTreeNode, RTreeNode::NodeChildOperations>(parent, nn, t);
            else
                parent->insert(nn);
        }

        node = parent;
        nn = pp;
    }

    // if we have a leftover split node, the root was split and we need to create
    // a new root one level higher
    if (nn != NULL) {
        RTreeNode* new_root = new RTreeNode(node->capacity());
        new_root->leaf(false);
        new_root->insert(node);
        new_root->insert(nn);

        node = new_root;
        nn = NULL;
    }

    return node;
}

// Inserts a new object into the tree, updating any nodes as necessary. Returns the new root node.
RTreeNode* RTree_insert_object(RTreeNode* root, Object* obj, const Time& t) {
    RTreeNode* leaf_node = RTree_choose_leaf(root, obj, t);

    RTreeNode* split_node = NULL;
    if (leaf_node->full())
        split_node = RTree_split_node<Object, RTreeNode::ObjectChildOperations>(leaf_node, obj, t);
    else
        leaf_node->insert(obj, t);

    RTreeNode* new_root = RTree_adjust_tree(leaf_node, split_node, t);

    return new_root;
}

void RTree_verify_bounds(RTreeNode* root, const Time& t) {
    for(int i = 0; i < root->size(); i++)
//        if (root->bounds().merge(root->leaf() ? root->object(i)->bounds() : root->node(i)->bounds()) != root->bounds())
        if (!root->bounds().contains( root->leaf() ? root->object(i)->worldBounds(t) : root->node(i)->bounds() )) {
            const BoundingSphere3f& child_bs = root->leaf() ? root->object(i)->worldBounds(t) : root->node(i)->bounds();
            printf("child exceeds bounds %s %f\n",
                (root->leaf() ? "object" : "node"),
                root->bounds().radius() - ((root->bounds().center() - child_bs.center()).length() + child_bs.radius())
            );
        }
    if (!root->leaf()) {
        for(int i = 0; i < root->size(); i++)
            RTree_verify_bounds(root->node(i), t);
    }
}

RTreeQueryHandler::RTreeQueryHandler(uint8 elements_per_node)
 : QueryHandler(),
   ObjectChangeListener(),
   QueryChangeListener(),
   mLastTime(0)
{
    mRTreeRoot = new RTreeNode(elements_per_node);
}

RTreeQueryHandler::~RTreeQueryHandler() {
    mObjects.clear();
    for(QueryMap::iterator it = mQueries.begin(); it != mQueries.end(); it++) {
        QueryState* state = it->second;
        delete state;
    }
    mQueries.clear();
}

void RTreeQueryHandler::registerObject(Object* obj) {
    insert(obj, mLastTime);
    mObjects.insert(obj);
    obj->addChangeListener(this);
}

void RTreeQueryHandler::registerQuery(Query* query) {
    QueryState* state = new QueryState;
    mQueries[query] = state;
    query->addChangeListener(this);
}

bool RTreeQueryHandler::satisfiesConstraints(const Vector3f& qpos, const float qradius, const SolidAngle& qangle, const BoundingSphere3f& obounds) {
    Vector3f obj_pos = obounds.center();
    Vector3f to_obj = obj_pos - qpos;

    // Must satisfy radius constraint
    if (qradius != Query::InfiniteRadius && (to_obj).lengthSquared() < (qradius+obounds.radius())*(qradius+obounds.radius()))
        return false;

    // Must satisfy solid angle constraint
    SolidAngle solid_angle = SolidAngle::fromCenterRadius(to_obj, obounds.radius());

    if (solid_angle < qangle)
        return false;

    return true;
}

void RTreeQueryHandler::tick(const Time& t) {
    //RTree_verify_bounds(mRTreeRoot, mLastTime);
    int count = 0;
    int ncount = 0;
    for(QueryMap::iterator query_it = mQueries.begin(); query_it != mQueries.end(); query_it++) {
        Query* query = query_it->first;
        QueryState* state = query_it->second;
        QueryCache newcache;

        Vector3f qpos = query->position(t);
        float qradius = query->radius();
        const SolidAngle& qangle = query->angle();

        std::stack<RTreeNode*> node_stack;
        node_stack.push(mRTreeRoot);
        while(!node_stack.empty()) {
            RTreeNode* node = node_stack.top();
            node_stack.pop();

            if (node->leaf()) {
                for(int i = 0; i < node->size(); i++) {
                    count++;
                    Object* obj = node->object(i);
                    if (satisfiesConstraints(qpos, qradius, qangle, obj->worldBounds(t)))
                        newcache.add(obj->id());
                }
            }
            else {
                for(int i = 0; i < node->size(); i++) {
                    count++;
                    RTreeNode* child = node->node(i);
                    if (satisfiesConstraints(qpos, qradius, qangle, child->bounds()))
                        node_stack.push(child);
                    else
                        ncount++;
                }
            }
        }

        std::deque<QueryEvent> events;
        state->cache.exchange(newcache, &events);

        query->pushEvents(events);
    }
    printf("count: %d %d\n", count, ncount);
    mLastTime = t;
}

void RTreeQueryHandler::objectPositionUpdated(Object* obj, const MotionVector3f& old_pos, const MotionVector3f& new_pos) {
    // FIXME update info in RTree
}

void RTreeQueryHandler::objectBoundingSphereUpdated(Object* obj, const BoundingSphere3f& old_bounds, const BoundingSphere3f& new_bounds) {
    // FIXME update bounding info in RTree, possibly removing and reinserting
}

void RTreeQueryHandler::objectDeleted(const Object* obj) {
    assert( mObjects.find(const_cast<Object*>(obj)) != mObjects.end() );
    mObjects.erase(const_cast<Object*>(obj));
    // FIXME remove object from RTree
}

void RTreeQueryHandler::queryPositionUpdated(Query* query, const MotionVector3f& old_pos, const MotionVector3f& new_pos) {
    // Nothing to be done, we use values directly from the query
}

void RTreeQueryHandler::queryDeleted(const Query* query) {
    QueryMap::iterator it = mQueries.find(const_cast<Query*>(query));
    assert( it != mQueries.end() );
    QueryState* state = it->second;
    delete state;
    mQueries.erase(it);
}

void RTreeQueryHandler::insert(Object* obj, const Time& t) {
    mRTreeRoot = RTree_insert_object(mRTreeRoot, obj, t);
}

} // namespace Prox
