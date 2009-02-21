/*  libprox
 *  SolidAngle.cpp
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

#include <prox/SolidAngle.hpp>
#include <cassert>
#include <cmath>

namespace Prox {

const SolidAngle SolidAngle::Min = SolidAngle(SolidAngle::MinVal);
const SolidAngle SolidAngle::Max = SolidAngle(SolidAngle::MaxVal);

SolidAngle::SolidAngle()
 : mSolidAngle(MinVal)
{
}

SolidAngle::SolidAngle(float sa)
 : mSolidAngle(sa)
{
    clamp();
}

SolidAngle::SolidAngle(const SolidAngle& rhs)
 : mSolidAngle(rhs.mSolidAngle)
{
}

SolidAngle::~SolidAngle() {
}

float SolidAngle::asFloat() const {
    return mSolidAngle;
}

ArcAngle SolidAngle::radius() const {
    return ArcAngle( acos(1.0f - (mSolidAngle / (2.0f * Pi))) );
}

SolidAngle SolidAngle::fromRadius(const ArcAngle& al) {
    return SolidAngle( 2.0f * Pi * (1.0f - cos(al.asFloat())) );
}

SolidAngle SolidAngle::fromCenterRadius(const Vector3f& to_center, float radius) {

    Vector3f ortho_to_obj = to_center.cross( Vector3f(1.f, 1.f, 1.f) ).normal(); // FIXME this could break in some cases
    Vector3f to_bound = to_center + ortho_to_obj * radius;
    Vector3f norm_to_center = to_center.normal();
    to_bound.normalizeThis();
    return SolidAngle( 2.0f * Pi * (1.0f - norm_to_center.dot(to_bound)) );
/*
    return SolidAngle( 2.f * Pi * (1.f-cos(atan(radius/to_center.length()))) );
*/
}

SolidAngle SolidAngle::operator+(const SolidAngle& rhs) const {
    return SolidAngle( mSolidAngle + rhs.mSolidAngle );
}

SolidAngle& SolidAngle::operator+=(const SolidAngle& rhs) {
    mSolidAngle += rhs.mSolidAngle;
    clamp();
    return *this;
}

SolidAngle SolidAngle::operator-(const SolidAngle& rhs) const {
    return SolidAngle( mSolidAngle - rhs.mSolidAngle );
}

SolidAngle& SolidAngle::operator-=(const SolidAngle& rhs) {
    mSolidAngle -= rhs.mSolidAngle;
    clamp();
    return *this;
}

SolidAngle SolidAngle::operator*(float rhs) const {
    assert(rhs >= 0.f);
    return SolidAngle( mSolidAngle * rhs );
}

SolidAngle& SolidAngle::operator*=(float rhs) {
    assert(rhs >= 0.f);
    mSolidAngle *= rhs;
    clamp();
    return *this;
}

SolidAngle SolidAngle::operator/(float rhs) const {
    assert(rhs > 0.f);
    return SolidAngle( mSolidAngle / rhs );
}

SolidAngle& SolidAngle::operator/=(float rhs) {
    assert(rhs > 0.f);
    mSolidAngle /= rhs;
    clamp();
    return *this;
}

bool SolidAngle::operator<(const SolidAngle& rhs) const {
    return (mSolidAngle < rhs.mSolidAngle);
}

bool SolidAngle::operator==(const SolidAngle& rhs) const {
    return (mSolidAngle == rhs.mSolidAngle);
}

void SolidAngle::clamp() {
    if (mSolidAngle < MinVal) mSolidAngle = MinVal;
    if (mSolidAngle > MaxVal) mSolidAngle = MaxVal;
}

} // namespace Prox
