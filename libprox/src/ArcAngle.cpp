/*  libprox
 *  ArcAngle.cpp
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

#include <prox/ArcAngle.hpp>
#include <cassert>

namespace Prox {

const ArcAngle ArcAngle::Min = ArcAngle(ArcAngle::MinVal);
const ArcAngle ArcAngle::Max = ArcAngle(ArcAngle::MaxVal);

ArcAngle::ArcAngle()
 : mArcAngle(MinVal)
{
}

ArcAngle::ArcAngle(float al)
 : mArcAngle(al)
{
    clamp();
}

ArcAngle::ArcAngle(const ArcAngle& cpy)
 : mArcAngle(cpy.mArcAngle)
{
}

ArcAngle::~ArcAngle() {
}

float ArcAngle::asFloat() const {
    return mArcAngle;
}

ArcAngle ArcAngle::operator+(const ArcAngle& rhs) const {
    return ArcAngle( mArcAngle + rhs.mArcAngle );
}

ArcAngle& ArcAngle::operator+=(const ArcAngle& rhs) {
    mArcAngle += rhs.mArcAngle;
    clamp();
    return *this;
}

ArcAngle ArcAngle::operator-(const ArcAngle& rhs) const {
    return ArcAngle( mArcAngle - rhs.mArcAngle );
}

ArcAngle& ArcAngle::operator-=(const ArcAngle& rhs) {
    mArcAngle -= rhs.mArcAngle;
    clamp();
    return *this;
}

ArcAngle ArcAngle::operator*(float rhs) const {
    assert(rhs >= 0.0f);
    return ArcAngle( mArcAngle * rhs );
}

ArcAngle& ArcAngle::operator*=(float rhs) {
    assert(rhs >= 0.0f);
    mArcAngle *= rhs;
    clamp();
    return *this;
}

ArcAngle ArcAngle::operator/(float rhs) const {
    assert(rhs > 0.0f);
    return ArcAngle( mArcAngle / rhs );
}

ArcAngle& ArcAngle::operator/=(float rhs) {
    assert(rhs > 0.0f);
    mArcAngle /= rhs;
    clamp();
    return *this;
}

void ArcAngle::clamp() {
    if (mArcAngle < MinVal) mArcAngle = MinVal;
    if (mArcAngle > MaxVal) mArcAngle = MaxVal;
}

} // namespace Prox
