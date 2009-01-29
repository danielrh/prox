/*  libprox
 *  BoundingBox.hpp
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

#ifndef _BOUNDING_BOX_HPP_
#define _BOUNDING_BOX_HPP_

#include <prox/Platform.hpp>
#include <cmath>

// FIXME these should really come from float.h or the like
#define BBOX_MIN -1e30f
#define BBOX_MAX 1e30f

namespace Prox {

template<typename CoordType>
class BoundingSphere;

template<typename CoordType>
class BoundingBox {
public:
    static const uint8 size = CoordType::size;

    BoundingBox()
     : mMin(BBOX_MAX),
       mMax(BBOX_MIN)
    {
    }

    BoundingBox(const CoordType& min, const CoordType& max)
     : mMin(min),
       mMax(max)
    {
    }

    BoundingBox(const BoundingSphere<CoordType>& sphere)
    {
        typedef typename CoordType::real realType;
        realType offdim = (realType) sqrt( sphere.radius()*sphere.radius() / size );
        CoordType offvector(offdim);
        mMin = sphere.center() - offvector;
        mMax = sphere.center() + offvector;
    }

    const CoordType& min() const {
        return mMin;
    }

    const CoordType& max() const {
        return mMax;
    }

    BoundingBox& mergeIn(const BoundingBox& rhs) {
        mMin = mMin.min(rhs.mMin);
        mMax = mMax.max(rhs.mMax);
        return *this;
    }

    BoundingBox merge(const BoundingBox& rhs) {
        return BoundingBox(mMin.min(rhs.mMin), mMax.max(rhs.mMax));
    }

    bool degenerate() const {
        for(int i = 0; i < size; i++)
            if (mMin[i] > mMax[i]) return true;
        return false;
    }

private:
    CoordType mMin;
    CoordType mMax;
}; // class BoundingBox


template<typename scalar>
class Vector3;

typedef BoundingBox< Vector3<float> > BoundingBox3f;

} // namespace Prox

#endif //_BOUNDING_BOX_HPP_
