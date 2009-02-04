/*  libprox
 *  SolidAngle.hpp
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

#ifndef _PROX_SOLID_ANGLE_HPP_
#define _PROX_SOLID_ANGLE_HPP_

#include <prox/ArcAngle.hpp>

namespace Prox {

class SolidAngle {
public:
    static const float Pi = 3.14159f;
    static const SolidAngle Min;
    static const SolidAngle Max;

    SolidAngle();
    explicit SolidAngle(float sa);
    SolidAngle(const SolidAngle& cpy);
    ~SolidAngle();

    /// Get the approximate radius of the region
    ArcAngle radius() const;

    /// Get the solid angle represented by the circular area with the given radius
    static SolidAngle fromRadius(const ArcAngle& al);

    SolidAngle operator+(const SolidAngle& rhs) const;
    SolidAngle& operator+=(const SolidAngle& rhs);
    SolidAngle operator-(const SolidAngle& rhs) const;
    SolidAngle& operator-=(const SolidAngle& rhs);

    SolidAngle operator*(float rhs) const;
    SolidAngle& operator*=(float rhs);

    SolidAngle operator/(float rhs) const;
    SolidAngle& operator/=(float rhs);

protected:
    static const float MinVal = 0.0f;
    static const float MaxVal = 4.0f * 3.14159f;

    void clamp();

    float mSolidAngle;
}; // class SolidAngle

} // namespace Prox

#endif //_PROX_SOLID_ANGLE_HPP_
