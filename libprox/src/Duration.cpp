/*  libprox
 *  Duration.cpp
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

#include <prox/Duration.hpp>
#include <prox/Time.hpp>

namespace Prox {

Duration::Duration(uint64 microsecs)
 : mMicrosecs(microsecs)
{
}

Duration::Duration(const Duration& cpy)
 : mMicrosecs(cpy.mMicrosecs)
{
}

Duration::~Duration() {
}

Duration Duration::operator+(const Duration& rhs) const {
    return Duration(mMicrosecs + rhs.mMicrosecs);
}

Duration& Duration::operator+=(const Duration& rhs) {
    mMicrosecs += rhs.mMicrosecs;
    return *this;
}

Time Duration::operator+(const Time& rhs) const {
    return rhs + *this;
}

Duration Duration::operator-(const Duration& rhs) const {
    return Duration(mMicrosecs - rhs.mMicrosecs);
}

Duration& Duration::operator-=(const Duration& rhs) {
    mMicrosecs -= rhs.mMicrosecs;
    return  *this;
}

bool Duration::operator<(const Duration& rhs) const {
    return mMicrosecs < rhs.mMicrosecs;
}

bool Duration::operator==(const Duration& rhs) const {
    return mMicrosecs == rhs.mMicrosecs;
}


} // namespace Prox
