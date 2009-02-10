/*  libprox
 *  Object.cpp
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

#include <prox/Object.hpp>
#include <prox/ObjectChangeListener.hpp>
#include <algorithm>

namespace Prox {

Object::Object(const ObjectID& id, const MotionVector3f& c, const BoundingSphere3f& bs)
 : mID(id),
   mPosition(c),
   mBounds(bs)
{
}

Object::Object(const Object& cpy)
 : mID(cpy.mID),
   mPosition(cpy.mPosition),
   mBounds(cpy.mBounds)
{
}

Object::~Object() {
    for(ChangeListenerList::iterator it = mChangeListeners.begin(); it != mChangeListeners.end(); it++)
        (*it)->objectDeleted(this);
}

const ObjectID& Object::id() const {
    return mID;
}

const MotionVector3f& Object::position() const {
    return mPosition;
}

Vector3f Object::position(const Time& t) const {
    return mPosition.position(t);
}

const BoundingSphere3f& Object::bounds() const {
    return mBounds;
}

BoundingSphere3f Object::worldBounds(const Time& t) const {
    return BoundingSphere3f( mBounds.center() + mPosition.position(t), mBounds.radius() );
}

void Object::position(const MotionVector3f& new_pos) {
    MotionVector3f old_pos = mPosition;
    mPosition = new_pos;
    for(ChangeListenerList::iterator it = mChangeListeners.begin(); it != mChangeListeners.end(); it++)
        (*it)->objectPositionUpdated(this, old_pos, new_pos);
}

void Object::bounds(const BoundingSphere3f& new_bounds) {
    BoundingSphere3f old_bounds = mBounds;
    mBounds = new_bounds;
    for(ChangeListenerList::iterator it = mChangeListeners.begin(); it != mChangeListeners.end(); it++)
        (*it)->objectBoundingSphereUpdated(this, old_bounds, new_bounds);
}

void Object::addChangeListener(ObjectChangeListener* listener) {
    assert(listener != NULL);
    assert(std::find(mChangeListeners.begin(), mChangeListeners.end(), listener) == mChangeListeners.end());

    mChangeListeners.push_back(listener);
}

void Object::removeChangeListener(ObjectChangeListener* listener) {
    assert(listener != NULL);

    ChangeListenerList::iterator it = std::find(mChangeListeners.begin(), mChangeListeners.end(), listener);
    if (it == mChangeListeners.end())
        return;

    mChangeListeners.erase(it);
}

} // namespace Prox
