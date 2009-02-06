
#include "Simulator.hpp"
#include "GLRenderer.hpp"

#include <iostream>

int main(int argc, char** argv) {
    using namespace Prox;
    using namespace ProxSim;

    Simulator* simulator = new Simulator( BoundingBox3f( Vector3f(-100.f, -100.f, -100.f), Vector3f(100.f, 100.f, 100.f) ), 100, 5 );
    Renderer* renderer = new GLRenderer(simulator);

    renderer->run();

    delete renderer;
    delete simulator;

    return 0;
}
