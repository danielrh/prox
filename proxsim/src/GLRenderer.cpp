/*  proxsim
 *  GLRenderer.cpp
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

#include "GLRenderer.hpp"
#include "Simulator.hpp"
#include <GL/glut.h>
#include <cassert>

using namespace Prox;

namespace ProxSim {

static GLRenderer* GLRenderer_sRenderer = NULL;


void glut_display() {
    GLRenderer_sRenderer->display();
}

void glut_reshape(int w, int h) {
    GLRenderer_sRenderer->reshape(w, h);
}

void glut_keyboard(unsigned char key, int x, int y) {
    GLRenderer_sRenderer->keyboard(key, x, y);
}

void glut_timer(int val) {
    GLRenderer_sRenderer->timer();
}

GLRenderer::GLRenderer(Simulator* sim)
 : Renderer(sim)
{
    assert(GLRenderer_sRenderer == NULL);
    GLRenderer_sRenderer = this;

    int argc = 0;
    glutInit( &argc, NULL );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( 512, 512 );

    glutCreateWindow( "Proximity Simulation" );

    glutDisplayFunc( glut_display );
    glutReshapeFunc( glut_reshape );
    glutKeyboardFunc( glut_keyboard );
}

GLRenderer::~GLRenderer() {
    GLRenderer_sRenderer = NULL;
}

void GLRenderer::run() {
    glutTimerFunc(16, glut_timer, 0);
    glutMainLoop();
}

void GLRenderer::display() {
    glClear( GL_COLOR_BUFFER_BIT );

    for(Simulator::ObjectIterator it = mSimulator->objectsBegin(); it != mSimulator->objectsEnd(); it++) {
        Object* obj = *it;
        BoundingBox3f bb = obj->worldBBox();
        drawbb(bb);
    }

    glutSwapBuffers();
}

void GLRenderer::reshape(int w, int h) {
    glClearColor( .3, .3, .3, 1 );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( -100, 100, -100, 100, -100, 100 );
    glViewport( 0, 0, w, h );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glutPostRedisplay();
}

void GLRenderer::timer() {
    mSimulator->tick();
    glutTimerFunc(16, glut_timer, 0);
}

void GLRenderer::keyboard(unsigned char key, int x, int y) {
    if (key == 27) // ESC
        exit(0);
}

void GLRenderer::drawbb(const BoundingBox3f& bb) {
    glBegin(GL_QUADS);

    glVertex3f ( bb.min().x,  bb.min().y,  bb.min().z);
    glVertex3f ( bb.max().x,  bb.min().y,  bb.min().z);
    glVertex3f ( bb.max().x,  bb.max().y,  bb.min().z);
    glVertex3f ( bb.min().x,  bb.max().y,  bb.min().z);

    glVertex3f ( bb.max().x,  bb.min().y,  bb.min().z);
    glVertex3f ( bb.max().x,  bb.min().y,  bb.max().z);
    glVertex3f ( bb.max().x,  bb.max().y,  bb.max().z);
    glVertex3f ( bb.max().x,  bb.max().y,  bb.min().z);

    glVertex3f ( bb.max().x,  bb.min().y,  bb.max().z);
    glVertex3f ( bb.min().x,  bb.min().y,  bb.max().z);
    glVertex3f ( bb.min().x,  bb.max().y,  bb.max().z);
    glVertex3f ( bb.max().x,  bb.max().y,  bb.max().z);

    glVertex3f ( bb.min().x,  bb.min().y,  bb.max().z);
    glVertex3f ( bb.min().x,  bb.min().y,  bb.min().z);
    glVertex3f ( bb.min().x,  bb.max().y,  bb.min().z);
    glVertex3f ( bb.min().x,  bb.max().y,  bb.max().z);

    glVertex3f ( bb.min().x,  bb.max().y,  bb.min().z);
    glVertex3f ( bb.max().x,  bb.max().y,  bb.min().z);
    glVertex3f ( bb.max().x,  bb.max().y,  bb.max().z);
    glVertex3f ( bb.min().x,  bb.max().y,  bb.max().z);

    glVertex3f ( bb.min().x,  bb.min().y,  bb.max().z);
    glVertex3f ( bb.max().x,  bb.min().y,  bb.max().z);
    glVertex3f ( bb.max().x,  bb.min().y,  bb.min().z);
    glVertex3f ( bb.min().x,  bb.min().y,  bb.min().z);

    glEnd();
}

} // namespace ProxSim
