/*  libprox
 *  QueryCache.cpp
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

#include <prox/QueryCache.hpp>
#include <algorithm>

namespace Prox {

QueryCache::QueryCache() {
}

QueryCache::~QueryCache() {
}

void QueryCache::add(const ObjectID& id) {
    assert( mObjects.find(id) == mObjects.end() );
    mObjects.insert(id);
}

bool QueryCache::contains(const ObjectID& id) {
    return (mObjects.find(id) != mObjects.end());
}

void QueryCache::remove(const ObjectID& id) {
    assert( mObjects.find(id) != mObjects.end() );
    mObjects.erase(id);
}

void QueryCache::exchange(QueryCache& newcache, std::deque<QueryEvent>* changes) {
    if (changes != NULL) {
        std::set<ObjectID> added_objs;
        std::set_difference(
            newcache.mObjects.begin(), newcache.mObjects.end(),
            mObjects.begin(), mObjects.end(),
            std::inserter(added_objs, added_objs.begin())
        );

        std::set<ObjectID> removed_objs;
        std::set_difference(
            mObjects.begin(), mObjects.end(),
            newcache.mObjects.begin(), newcache.mObjects.end(),
            std::inserter(removed_objs, removed_objs.begin())
        );

        for(std::set<ObjectID>::iterator it = added_objs.begin(); it != added_objs.end(); it++)
            changes->push_back(QueryEvent(QueryEvent::Added, *it));

        for(std::set<ObjectID>::iterator it = removed_objs.begin(); it != removed_objs.end(); it++)
            changes->push_back(QueryEvent(QueryEvent::Removed, *it));
    }

    mObjects = newcache.mObjects;
}

} // namespace Prox
