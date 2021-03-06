/*  libprox
 *  Query.cpp
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

#include <prox/Query.hpp>
#include <prox/QueryChangeListener.hpp>
#include <prox/QueryEventListener.hpp>
#include <float.h>
#include <algorithm>

namespace Prox {

const float Query::InfiniteRadius = FLT_MAX;

Query::Query(const MotionVector3f& pos, const SolidAngle& minAngle)
 : mPosition(pos),
   mMinSolidAngle(minAngle),
   mMaxRadius(InfiniteRadius),
   mChangeListeners(),
   mEventListener(NULL),
   mNotified(false)
{
}

Query::Query(const MotionVector3f& pos, const SolidAngle& minAngle, float radius)
 : mPosition(pos),
   mMinSolidAngle(minAngle),
   mMaxRadius(radius),
   mNotified(false)
{
}

Query::Query(const Query& cpy)
 : mPosition(cpy.mPosition),
   mMinSolidAngle(cpy.mMinSolidAngle),
   mMaxRadius(cpy.mMaxRadius),
   mNotified(false)
{
}

Query::~Query() {
    for(ChangeListenerList::iterator it = mChangeListeners.begin(); it != mChangeListeners.end(); it++)
        (*it)->queryDeleted(this);
}

const MotionVector3f& Query::position() const {
    return mPosition;
}

Vector3f Query::position(const Time& t) const {
    return mPosition.position(t);
}

const SolidAngle& Query::angle() const {
    return mMinSolidAngle;
}

const float Query::radius() const {
    return mMaxRadius;
}

void Query::position(const MotionVector3f& new_pos) {
    MotionVector3f old_pos = mPosition;
    mPosition = new_pos;
    for(ChangeListenerList::iterator it = mChangeListeners.begin(); it != mChangeListeners.end(); it++)
        (*it)->queryPositionUpdated(this, old_pos, new_pos);
}

void Query::addChangeListener(QueryChangeListener* listener) {
    mChangeListeners.push_back(listener);
}

void Query::removeChangeListener(QueryChangeListener* listener) {
    ChangeListenerList::iterator it = std::find(mChangeListeners.begin(), mChangeListeners.end(), listener);
    if (it != mChangeListeners.end())
        mChangeListeners.erase(it);
}

void Query::setEventListener(QueryEventListener* listener) {
    mEventListener = listener;
}

void Query::removeEventListener() {
    mEventListener = NULL;
}

void Query::pushEvent(const QueryEvent& evt) {
    {
        boost::mutex::scoped_lock lock(mEventQueueMutex);

        mEventQueue.push_back(evt);

        if (mNotified) return;
        mNotified = true;
    }

    if (mEventListener != NULL)
        mEventListener->queryHasEvents(this);
}

void Query::pushEvents(std::deque<QueryEvent>& evts) {
    {
        boost::mutex::scoped_lock lock(mEventQueueMutex);

        while( !evts.empty() ) {
            mEventQueue.push_back( evts.front() );
            evts.pop_front();
        }

        if (mNotified) return;

        mNotified = true;
    }

    if (mEventListener != NULL)
        mEventListener->queryHasEvents(this);
}

void Query::popEvents(std::deque<QueryEvent>& evts) {
    boost::mutex::scoped_lock lock(mEventQueueMutex);

    assert( evts.empty() );
    mEventQueue.swap(evts);
    mNotified = false;
}

} // namespace Prox
