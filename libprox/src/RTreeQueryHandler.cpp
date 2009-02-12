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

namespace Prox {

struct RTreeNode {
private:
    static const uint8 LeafFlag = 0x02; // elements are object pointers instead of node pointers

    union {
        RTreeNode** nodes;
        Object** objects;
        void** magic;
    } elements;
    BoundingSphere3f bounding_sphere;
    uint8 flags;
    uint8 count;
    uint8 max_elements;

public:
    RTreeNode(uint8 _max_elements)
     : bounding_sphere(), flags(0), count(0), max_elements(_max_elements)
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
        elements.nodes[count] = node;
        count++;
        bounding_sphere = bounding_sphere.merge(node->bounds());
    }
};


void RTree_split_node(RTreeNode* node, const Time& t) {
    assert(node);
    assert(node->full());

    RTreeNode* left = new RTreeNode(node->capacity());
    RTreeNode* right = new RTreeNode(node->capacity());

    for(int i = 0; i < node->capacity()/2; i++) {
        Object* obj = node->object(i);
        left->insert(obj, t);
    }
    for(int i = node->capacity()/2; i < node->capacity(); i++) {
        Object* obj = node->object(i);
        right->insert(obj, t);
    }

    node->clear();
    node->leaf(false);
    node->insert(left);
    node->insert(right);
}

void RTree_insert_object(Object* obj, const Time& t, RTreeNode* root) {
    RTreeNode* node = root;
    BoundingSphere3f obj_bounds = obj->worldBounds(t);

    while(true) {
        if (node->leaf()) {
            if (!node->full()) {
                node->insert(obj, t);
                break;
            }

            // this node is full, so split. it turns into a regular node, so
            // we then exit the conditional and continue as we would for a
            // regular node
            RTree_split_node(node, t);
        }

        float min_increase = 1000000000000000.f; // FIXME FLT_MAX
        RTreeNode* min_increase_node = NULL;

        for(int i = 0; i < node->size(); i++) {
            RTreeNode* child_node = node->node(i);
            BoundingSphere3f merged = child_node->bounds().merge(obj_bounds);
            float increase = merged.volume() - child_node->bounds().volume();
            if (increase < min_increase) {
                min_increase = increase;
                min_increase_node = child_node;
            }
        }

        if (min_increase_node == NULL ||
            (!node->full() && obj_bounds.volume() < min_increase) ) {
            RTreeNode* new_child = new RTreeNode(node->capacity());
            new_child->leaf(true);
            new_child->insert(obj, t);
            node->insert(new_child);
            return;
        }
        else {
            node->bounds( node->bounds().merge(obj_bounds) );
            node = min_increase_node;
        }
    }
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

bool RTreeQueryHandler::satisfiesConstraints(const Vector3f& qpos, const float qradius, const SolidAngle& qangle, const Vector3f& opos, const BoundingSphere3f& obounds) {
    Vector3f obj_pos = opos + obounds.center();

    // Must satisfy radius constraint
    if (qradius != Query::InfiniteRadius && (obj_pos-qpos).lengthSquared() < (qradius+obounds.radius())*(qradius+obounds.radius()))
        return false;

    // Must satisfy solid angle constraint
    Vector3f to_obj = obj_pos - qpos;
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
                    if (satisfiesConstraints(qpos, qradius, qangle, obj->position(t), obj->bounds()))
                        newcache.add(obj->id());
                }
            }
            else {
                for(int i = 0; i < node->size(); i++) {
                    count++;
                    RTreeNode* child = node->node(i);
                    if (satisfiesConstraints(qpos, qradius, qangle, Vector3f(0.0f, 0.0f, 0.0f), child->bounds()))
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
    RTree_insert_object(obj, t, mRTreeRoot);
}

} // namespace Prox
