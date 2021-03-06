/*  libprox
 *  Query.hpp
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

#ifndef _PROX_QUERY_HPP_
#define _PROX_QUERY_HPP_

#include <prox/Platform.hpp>
#include <prox/SolidAngle.hpp>
#include <prox/MotionVector.hpp>
#include <prox/QueryEvent.hpp>
#include <boost/thread.hpp>

namespace Prox {

class QueryChangeListener;
class QueryEventListener;

class Query {
public:
    typedef MotionVector3f PositionVectorType;

    const static float InfiniteRadius;

    Query(const MotionVector3f& pos, const SolidAngle& minAngle);
    Query(const MotionVector3f& pos, const SolidAngle& minAngle, float radius);
    Query(const Query& cpy);
    ~Query();

    const MotionVector3f& position() const;
    Vector3f position(const Time& t) const;
    const SolidAngle& angle() const;
    const float radius() const;

    void position(const MotionVector3f& new_center);

    void addChangeListener(QueryChangeListener* listener);
    void removeChangeListener(QueryChangeListener* listener);

    void setEventListener(QueryEventListener* listener);
    void removeEventListener();

    void pushEvent(const QueryEvent& evt);
    void pushEvents(std::deque<QueryEvent>& evts);
    void popEvents(std::deque<QueryEvent>& evts);

protected:
    Query();
    void notifyEventListeners();

    PositionVectorType mPosition;
    SolidAngle mMinSolidAngle;
    float mMaxRadius;

    typedef std::list<QueryChangeListener*> ChangeListenerList;
    ChangeListenerList mChangeListeners;
    QueryEventListener* mEventListener;

    typedef std::deque<QueryEvent> EventQueue;
    EventQueue mEventQueue;
    bool mNotified; // whether we've notified event listeners of new events
    boost::mutex mEventQueueMutex;
}; // class Query

} // namespace Prox

#endif //_PROX_QUERY_HPP_
