/*  libprox
 *  MotionVector.hpp
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

#ifndef _MOTION_VECTOR_HPP_
#define _MOTION_VECTOR_HPP_

#include <prox/Vector3.hpp>
#include <prox/Time.hpp>
#include <prox/Duration.hpp>

namespace Prox {

template<typename CoordType_t>
class MotionVector {
public:
    typedef CoordType_t CoordType;

    MotionVector(const Time& t, const CoordType& pos, const CoordType& vel)
     : mTime(t), mStart(pos), mDirection(vel)
    {
    }

    const Time& updateTime() const {
        return mTime;
    }

    const CoordType& position() const {
        return mStart;
    }

    CoordType position(const Duration& dt) const {
        return mStart + mDirection * dt.seconds();
    }

    CoordType position(const Time& t) const {
        return position(t - mTime);
    }

    const CoordType& velocity() const {
        return mDirection;
    }
    MotionVector& operator+=(const CoordType &offset) {
        mStart+=offset;
        return *this;
    }
    void update(const Time& t, const CoordType& pos, const CoordType& vel) {
        assert(t > mTime);
        mTime = t;
        mStart = pos;
        mDirection = vel;
    }
private:
    MotionVector();

    Time mTime;
    CoordType mStart;
    CoordType mDirection;
}; // class MotionVector

typedef MotionVector<Vector3f> MotionVector3f;

} // namespace Prox

#endif //_MOTION_VECTOR_HPP_
