/*  libprox
 *  Object.hpp
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

#ifndef _PROX_OBJECT_HPP_
#define _PROX_OBJECT_HPP_

#include <prox/ObjectID.hpp>
#include <prox/MotionVector.hpp>
#include <prox/BoundingSphere.hpp>

namespace Prox {

class ObjectChangeListener;

class Object {
public:
    typedef MotionVector3f PositionVectorType;

    Object(const ObjectID& id, const PositionVectorType& c, const BoundingSphere3f& bs);
    Object(const Object& cpy);
    ~Object();

    const ObjectID& id() const;
    const MotionVector3f& position() const;
    Vector3f position(const Time& t) const;
    const BoundingSphere3f& bounds() const;
    BoundingSphere3f worldBounds(const Time& t) const;
    void position(const MotionVector3f& new_pos);
    void bounds(const BoundingSphere3f& new_bounds);

    void addChangeListener(ObjectChangeListener* listener);
    void removeChangeListener(ObjectChangeListener* listener);
protected:

    ObjectID mID;
    PositionVectorType mPosition;
    BoundingSphere3f mBounds;

    typedef std::list<ObjectChangeListener*> ChangeListenerList;
    ChangeListenerList mChangeListeners;

private:
    Object();
}; // class Object

} // namespace Prox

#endif //_PROX_OBJECT_HPP_
