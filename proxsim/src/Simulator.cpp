/*  proxsim
 *  Simulator.cpp
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

#include "Simulator.hpp"
#include <algorithm>

using namespace Prox;

namespace ProxSim {

static float randFloat() {
    return float(rand()) / RAND_MAX;
}

Simulator::Simulator(QueryHandler* handler)
 : mObjectIDSource(0),
   mHandler(handler)
{
}

Simulator::~Simulator() {
    while(!mObjects.empty()) {
        Object* obj = mObjects.front();
        removeObject(obj);
        delete obj;
    }

    while(!mQueries.empty()) {
        Query* query = mQueries.front();
        removeQuery(query);
        delete query;
    }
}

void Simulator::initialize(const Time& t, const Prox::BoundingBox3f& region, int nobjects, int nqueries) {
    Vector3f region_min = region.min();
    Vector3f region_extents = region.extents();

    for(int i = 0; i < nobjects; i++) {
        Object* obj = new Object(
            ObjectID(mObjectIDSource++),
            MotionVector3f(
                t,
                region_min + Vector3f(region_extents.x * randFloat(), region_extents.y * randFloat(), region_extents.z * randFloat()),
                Vector3f(randFloat() * 2.f - 1.f, randFloat() * 2.f - 1.f, randFloat() * 2.f - 1.f)
            ),
            BoundingBox3f( Vector3f(-1, -1, -1), Vector3f(1, 1, 1))
        );
        addObject(obj);
    }

    for(int i = 0; i < nqueries; i++) {
        Query* query = new Query(
            MotionVector3f(
                t,
                region_min + Vector3f(region_extents.x * randFloat(), region_extents.y * randFloat(), region_extents.z * randFloat()),
                Vector3f(randFloat() * 2.f - 1.f, randFloat() * 2.f - 1.f, randFloat() * 2.f - 1.f)
            ),
            SolidAngle( SolidAngle::Max / 10000 )
        );
        addQuery(query);
    }
}

void Simulator::addListener(SimulatorListener* listener) {
    assert( std::find(mListeners.begin(), mListeners.end(), listener) == mListeners.end() );
    mListeners.push_back(listener);
}

void Simulator::removeListener(SimulatorListener* listener) {
    ListenerList::iterator it = std::find(mListeners.begin(), mListeners.end(), listener);
    assert( it != mListeners.end() );
    mListeners.erase(it);
}

void Simulator::tick(const Time& t) {
    mHandler->tick(t);
}

void Simulator::addObject(Object* obj) {
    mObjects.push_back(obj);
    mHandler->registerObject(obj);
    for(ListenerList::iterator it = mListeners.begin(); it != mListeners.end(); it++)
        (*it)->simulatorAddedObject(obj);
}

void Simulator::removeObject(Object* obj) {
    ObjectList::iterator it = std::find(mObjects.begin(), mObjects.end(), obj);
    mObjects.erase(it);

    for(ListenerList::iterator it = mListeners.begin(); it != mListeners.end(); it++)
        (*it)->simulatorRemovedObject(obj);
}

void Simulator::addQuery(Query* query) {
    mQueries.push_back(query);
    mHandler->registerQuery(query);
    for(ListenerList::iterator it = mListeners.begin(); it != mListeners.end(); it++)
        (*it)->simulatorAddedQuery(query);
}

void Simulator::removeQuery(Query* query) {
    QueryList::iterator it = std::find(mQueries.begin(), mQueries.end(), query);
    mQueries.erase(it);

    for(ListenerList::iterator it = mListeners.begin(); it != mListeners.end(); it++)
        (*it)->simulatorRemovedQuery(query);
}

Simulator::ObjectIterator Simulator::objectsBegin() {
    return mObjects.begin();
}

Simulator::ObjectIterator Simulator::objectsEnd() {
    return mObjects.end();
}

Simulator::QueryIterator Simulator::queriesBegin() {
    return mQueries.begin();
}

Simulator::QueryIterator Simulator::queriesEnd() {
    return mQueries.end();
}

} // namespace ProxSim
