/*  libprox
 *  BruteForceQueryHandler.hpp
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

#ifndef _PROX_BRUTE_FORCE_QUERY_HANDLER_HPP_
#define _PROX_BRUTE_FORCE_QUERY_HANDLER_HPP_

#include <prox/QueryHandler.hpp>
#include <prox/ObjectChangeListener.hpp>
#include <prox/QueryChangeListener.hpp>
#include <prox/QueryCache.hpp>

namespace Prox {

class BruteForceQueryHandler : public QueryHandler, public ObjectChangeListener, public QueryChangeListener {
public:
    BruteForceQueryHandler();
    virtual ~BruteForceQueryHandler();

    virtual void registerObject(Object* obj);
    virtual void registerQuery(Query* query);
    virtual void tick(const Time& t);

    // ObjectChangeListener Implementation
    virtual void objectPositionUpdated(Object* obj, const MotionVector3f& old_pos, const MotionVector3f& new_pos);
    virtual void objectBoundingSphereUpdated(Object* obj, const BoundingSphere3f& old_bounds, const BoundingSphere3f& new_bounds);
    virtual void objectDeleted(const Object* obj);

    // QueryChangeListener Implementation
    virtual void queryPositionUpdated(Query* query, const MotionVector3f& old_pos, const MotionVector3f& new_pos);
    virtual void queryDeleted(const Query* query);

private:
    struct QueryState {
        QueryCache cache;
    };

    typedef std::set<Object*> ObjectSet;
    typedef std::map<Query*, QueryState*> QueryMap;

    ObjectSet mObjects;
    QueryMap mQueries;
}; // class BruteForceQueryHandler

} // namespace Prox

#endif //_PROX_BRUTE_FORCE_QUERY_HANDLER_HPP_
