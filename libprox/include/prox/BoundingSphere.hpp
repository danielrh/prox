/*  libprox
 *  BoundingSphere.hpp
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

#ifndef _BOUNDING_SPHERE_HPP_
#define _BOUNDING_SPHERE_HPP_

#include <prox/Platform.hpp>
#include <cmath>

namespace Prox {

template<typename CoordType>
class BoundingBox;

template<typename CoordType>
class BoundingSphere {
public:
    typedef typename CoordType::real real;
    static const real Pi = 3.14159;

    BoundingSphere()
     : mCenter((real)0),
       mRadius(0)
    {
    }

    BoundingSphere(const CoordType& c, const real r)
     : mCenter(c),
       mRadius(r)
    {
    }

    BoundingSphere(const BoundingBox<CoordType>& bb)
    {
        mCenter = (bb.min() + bb.max()) * .5;
        mRadius = (bb.max() - bb.min()).length() * .5;
    }

    const CoordType& center() const {
        return mCenter;
    }

    real radius() const {
        return mRadius;
    }

    BoundingSphere& mergeIn(const BoundingSphere& rhs) {
        *this = merge(rhs);
        return *this;
    }

    BoundingSphere merge(const BoundingSphere& rhs) const {
        if (rhs.degenerate())
            return *this;

        if (this->degenerate())
            return rhs;

        real center_dist = (rhs.mCenter - mCenter).length();
        if (center_dist + mRadius <= rhs.mRadius)
            return rhs;
        if (center_dist + rhs.mRadius <= mRadius)
            return *this;

        real new_radius = (mRadius + center_dist + rhs.mRadius) * 0.5;
        real ratio = (new_radius - mRadius) / center_dist;
        CoordType new_center = mCenter + (rhs.mCenter - mCenter) * ratio;
        return BoundingSphere(new_center, new_radius);
    }

    bool contains(const BoundingSphere& other) const {
        real centers_len = (mCenter - other.mCenter).length();
        return (mRadius >= centers_len + other.mRadius);
    }

    bool degenerate() const {
        return ( mRadius <= 0 );
    }

    real volume() const {
        if (degenerate()) return 0.0;
        return 4.0 / 3.0 * Pi * mRadius * mRadius * mRadius;
    }

    bool operator==(const BoundingSphere& rhs) {
        return (mCenter == rhs.mCenter && mRadius == rhs.mRadius);
    }
    bool operator!=(const BoundingSphere& rhs) {
        return (mCenter != rhs.mCenter || mRadius != rhs.mRadius);
    }
private:
    CoordType mCenter;
    real mRadius;
}; // class BoundingSphere


template<typename scalar>
class Vector3;

typedef BoundingSphere< Vector3<float32> > BoundingSphere3f;
typedef BoundingSphere< Vector3<float64> > BoundingSphere3d;

} // namespace Prox

#endif //_BOUNDING_SPHERE_HPP_
