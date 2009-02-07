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

void BruteForceQueryHandler::tick() {
    for(QueryMap::iterator query_it = mQueries.begin(); query_it != mQueries.end(); query_it++) {
        Query* query = query_it->first;
        QueryState* state = query_it->second;

        for(ObjectSet::iterator obj_it = mObjects.begin(); obj_it != mObjects.end(); obj_it++) {
            Object* obj = *obj_it;

            if (query->radius() == Query::InfiniteRadius || (obj->center()-query->center()).dot(obj->center()-query->center()) < query->radius()*query->radius())
                query->pushEvent( QueryEvent(QueryEvent::Added, obj->id()) );
        }
    }
}

void BruteForceQueryHandler::objectCenterUpdated(Object* obj, const Vector3f& old_center, const Vector3f& new_center) {
    // Nothing to be done, we use values directly from the object
}

void BruteForceQueryHandler::objectBoundingBoxUpdated(Object* obj, const BoundingBox3f& oldbb, const BoundingBox3f& newbb) {
    // Nothing to be done, we use values directly from the object
}

void BruteForceQueryHandler::objectDeleted(const Object* obj) {
    assert( mObjects.find(const_cast<Object*>(obj)) != mObjects.end() );
    mObjects.erase(const_cast<Object*>(obj));
}

void BruteForceQueryHandler::queryCenterUpdated(Query* query, const Vector3f& old_center, const Vector3f& new_center) {
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
