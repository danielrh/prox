/*  proxsim
 *  GLRenderer.hpp
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

#ifndef _PROXSIM_GLRENDERER_HPP_
#define _PROXSIM_GLRENDERER_HPP_

#include "Renderer.hpp"
#include <prox/BoundingBox.hpp>
#include <prox/QueryEventListener.hpp>
#include <prox/ObjectID.hpp>
#include <prox/Time.hpp>
#include "SimulatorListener.hpp"

namespace ProxSim {

class GLRenderer : public Renderer, public Prox::QueryEventListener, public SimulatorListener {
public:
    GLRenderer(Simulator* sim);
    virtual ~GLRenderer();

    // Renderer Interface
    virtual void run();

    // QueryEventListener Interface
    virtual void queryHasEvents(Prox::Query* query);

    // SimulatorListener Interface
    virtual void simulatorAddedObject(Prox::Object* obj);
    virtual void simulatorRemovedObject(Prox::Object* obj);
    virtual void simulatorAddedQuery(Prox::Query* query);
    virtual void simulatorRemovedQuery(Prox::Query* query);

    // GLRenderer Interface
    void display();
    void reshape(int w, int h);
    void timer();
    void keyboard(unsigned char key, int x, int y);

protected:
    GLRenderer();

    void drawbb(const Prox::BoundingBox3f& bb);
    void drawbs(const Prox::BoundingSphere3f& bs);

    Prox::Time mTime;
    std::set<Prox::ObjectID> mSeenObjects;
}; // class Renderer

} // namespace ProxSim

#endif //_PROXSIM_GLRENDERER_HPP_
