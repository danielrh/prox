/*  proxsim
 *  Simulator.hpp
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

#ifndef _PROXSIM_SIMULATOR_HPP_
#define _PROXSIM_SIMULATOR_HPP_

#include <prox/Object.hpp>
#include <prox/Query.hpp>
#include <prox/QueryHandler.hpp>
#include <prox/Time.hpp>
#include "SimulatorListener.hpp"

namespace ProxSim {

class Simulator {
private:
    typedef std::list<Prox::Object*> ObjectList;
    typedef std::list<Prox::Query*> QueryList;
public:
    Simulator(Prox::QueryHandler* handler);
    ~Simulator();

    void initialize(const Prox::Time& t, const Prox::BoundingBox3f& region, int nobjects, int nqueries);

    void addListener(SimulatorListener* listener);
    void removeListener(SimulatorListener* listener);

    void tick(const Prox::Time& t);

    typedef ObjectList::iterator ObjectIterator;
    typedef QueryList::iterator QueryIterator;

    ObjectIterator objectsBegin();
    ObjectIterator objectsEnd();

    QueryIterator queriesBegin();
    QueryIterator queriesEnd();
private:
    void addObject(Prox::Object* obj);
    void removeObject(Prox::Object* obj);

    void addQuery(Prox::Query* query);
    void removeQuery(Prox::Query* query);

    Prox::int64 mObjectIDSource;
    Prox::QueryHandler* mHandler;
    ObjectList mObjects;
    QueryList mQueries;
    typedef std::list<SimulatorListener*> ListenerList;
    ListenerList mListeners;
}; // class Simulator

} // namespace ProxSim

#endif //_PROXSIM_SIMULATOR_HPP_
