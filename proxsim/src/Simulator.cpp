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

using namespace Prox;

namespace ProxSim {

static float randFloat() {
    return float(rand()) / RAND_MAX;
}

Simulator::Simulator(const BoundingBox3f& region, int nobjects, int nqueries)
 : mObjectIDSource(0)
{
    Vector3f region_min = region.min();
    Vector3f region_extents = region.extents();

    for(int i = 0; i < nobjects; i++) {
        mObjects.push_back(
            new Object(
                ObjectID(mObjectIDSource++),
                region_min + Vector3f(region_extents.x * randFloat(), region_extents.y * randFloat(), region_extents.z * randFloat()),
                BoundingBox3f( Vector3f(-1, -1, -1), Vector3f(1, 1, 1))
            )
        );
    }

    for(int i = 0; i < nqueries; i++) {
        mQueries.push_back(
            new Query(
                region_min + Vector3f(region_extents.x * randFloat(), region_extents.y * randFloat(), region_extents.z * randFloat()),
                SolidAngle( SolidAngle::Max / 100 )
            )
        );
    }
}

Simulator::~Simulator() {
    while(!mObjects.empty()) {
        Object* obj = mObjects.front();
        delete obj;
        mObjects.pop_front();
    }

    while(!mQueries.empty()) {
        Query* query = mQueries.front();
        delete query;
        mQueries.pop_front();
    }
}

void Simulator::tick() {

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
