#include "qgridsystem1D.hpp"

#include "quantumsim/evolvers/explicit.hpp"
#include "wave_packets.hpp"
#include "uniform_potential.hpp"

using namespace godot;
using namespace qsim::grid;
using namespace qsim::pot;

qgridsystem1D::qgridsystem1D() 
    : qsystem1D(1.0, 
                {-1.0, 1.0},
                gaussian_packet().generate()->ref(), 
                nullptr, // zero potential, not a godot reference
                std::make_shared<qsystem1D::explicit_evolver>()),
        m_potential(nullptr) {

        }

void qgridsystem1D::_init() {
    // default values
    set_potential(new grid_uniform_potential);

    // initialize reference
    Ref<gaussian_packet> w(new gaussian_packet);
    _set_wave(w->generate());
}

qgridsystem1D::~qgridsystem1D() {}

void qgridsystem1D::_register_methods() {
    // direct methods
    register_method("_process", &qgridsystem1D::_process);
    register_property<qgridsystem1D, double>("mass", &qgridsystem1D::set_mass, &qgridsystem1D::mass, 1.0);
    register_method("energy", &qgridsystem1D::_energy);
    register_method("position", &qgridsystem1D::_position);
    register_method("momentum", &qgridsystem1D::_momentum);
    register_method("normalize", &qgridsystem1D::_normalize);
    register_method("map", &qgridsystem1D::_map);
    register_method("size", &qgridsystem1D::_size);
    
    // methods that need to be adapted
    register_property<qgridsystem1D, Ref<potential<size_t>>>("V", &qgridsystem1D::set_potential, &qgridsystem1D::get_potential, nullptr);
    register_property<qgridsystem1D, Ref<grid_wave>>("wave", &qgridsystem1D::_set_wave, &qgridsystem1D::_get_wave, nullptr);
}


double qgridsystem1D::_energy() const {
    return energy();
}

double qgridsystem1D::_position() const {
    return position();
}

double qgridsystem1D::_momentum() const {
    return momentum();
}

double qgridsystem1D::_normalize() {
    return normalize();
}

double qgridsystem1D::_map(size_t m) const {
    return map(m);
}

double qgridsystem1D::_size() const {
    return size();
}

void qgridsystem1D::set_potential(Ref<grid_potential> pot) {

    potential<size_t> * ptr = *pot; 

    if (!(ptr != nullptr && ptr->is_safe())) {
        // TODO throw godot exception
        return;
    }
    
    // set the old potential 
    m_potential = pot; 
    
    // finally set the potential 
    qsim::grid::qsystem1D::set_potential(*ptr); 
}

Ref<grid_potential> qgridsystem1D::get_potential() const {
    return m_potential;
}

void qgridsystem1D::_set_wave(Ref<grid_wave> buffer) {
    // buffer is moved!!
    replace_wave(std::forward<wave_vector>(buffer->ref()));
}

Ref<grid_wave> qgridsystem1D::_get_wave() {
    // create a new wrapper, do not delete at the end
    return new grid_wave(&psi(), false);
}

/*void set_evolver(Reference* evo) {
    auto ptr = dynamic_cast<qsim::grid::qsystem1D::evolver*>(evo);

    if (ptr == nullptr) {
        // TODO throw godot exception
        return;
    }

    if(!evo->reference()) {
        // TODO cannot bind reference
        return;
    }

    if (m_evolver != nullptr && !m_evolver->unreference()) {
        // TODO cannot unreference old potential
        return;
    }
    
    // set the old potential 
    m_evolver = evo; 
    
    // finally set the potential 
    qsim::grid::qsystem1D::set_evolver(ptr);
}*/


/*Reference * get_evolver() const {
    return m_evolver;
}*/

#include <iostream>


void qgridsystem1D::_process(double dt) {
    // default values
    qsim::grid::qsystem1D::evolve(dt);
}
