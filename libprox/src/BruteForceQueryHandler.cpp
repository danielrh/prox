/*  libprox
 *  BruteForceQueryHandler.cpp
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

#include <prox/BruteForceQueryHandler.hpp>
#include <prox/BoundingSphere.hpp>
#include <cassert>

namespace Prox {

BruteForceQueryHandler::BruteForceQueryHandler()
 : QueryHandler(),
   ObjectChangeListener(),
   QueryChangeListener()
{
}

BruteForceQueryHandler::~BruteForceQueryHandler() {
    mObjects.clear();
    for(QueryMap::iterator it = mQueries.begin(); it != mQueries.end(); it++) {
        QueryState* state = it->second;
        delete state;
    }
    mQueries.clear();
}

void BruteForceQueryHandler::registerObject(Object* obj) {
    mObjects.insert(obj);
    obj->addChangeListener(this);
}

void BruteForceQueryHandler::registerQuery(Query* query) {
    QueryState* state = new QueryState;
    mQueries[query] = state;
    query->addChangeListener(this);
}

void BruteForceQueryHandler::tick(const Time& t) {
    for(QueryMap::iterator query_it = mQueries.begin(); query_it != mQueries.end(); query_it++) {
        Query* query = query_it->first;
        QueryState* state = query_it->second;
        QueryCache newcache;

        for(ObjectSet::iterator obj_it = mObjects.begin(); obj_it != mObjects.end(); obj_it++) {
            Object* obj = *obj_it;

            // Must satisfy radius constraint
            if (query->radius() != Query::InfiniteRadius && (obj->position(t)-query->position(t)).lengthSquared() < query->radius()*query->radius())
                continue;

            // Must satisfy solid angle constraint
            BoundingSphere3f bs = obj->bounds();
            Vector3f obj_pos = obj->position(t) + bs.center();
            Vector3f to_obj = obj_pos - query->position(t);
            SolidAngle solid_angle = SolidAngle::fromCenterRadius(to_obj, bs.radius());

            if (solid_angle < query->angle())
                continue;

            newcache.add(obj->id());
        }

        std::deque<QueryEvent> events;
        state->cache.exchange(newcache, &events);

        query->pushEvents(events);
    }
}

void BruteForceQueryHandler::objectPositionUpdated(Object* obj, const MotionVector3f& old_pos, const MotionVector3f& new_pos) {
    // Nothing to be done, we use values directly from the object
}

void BruteForceQueryHandler::objectBoundingSphereUpdated(Object* obj, const BoundingSphere3f& old_bounds, const BoundingSphere3f& new_bounds) {
    // Nothing to be done, we use values directly from the object
}

void BruteForceQueryHandler::objectDeleted(const Object* obj) {
    assert( mObjects.find(const_cast<Object*>(obj)) != mObjects.end() );
    mObjects.erase(const_cast<Object*>(obj));
}

void BruteForceQueryHandler::queryPositionUpdated(Query* query, const MotionVector3f& old_pos, const MotionVector3f& new_pos) {
    // Nothing to be done, we use values directly from the query
}

void BruteForceQueryHandler::queryDeleted(const Query* query) {
    QueryMap::iterator it = mQueries.find(const_cast<Query*>(query));
    assert( it != mQueries.end() );
    QueryState* state = it->second;
    delete state;
    mQueries.erase(it);
}

} // namespace Prox
