#include "qgridsystem1D.hpp"

#include "qsimbox.hpp"
#include "grid_wave.hpp"

#include "quantumsim/evolvers/explicit.hpp"
#include "uniform_potential.hpp"

#include "debug.hpp"

using namespace godot;
using namespace qsim::grid;
using namespace qsim::pot;


qgridsystem1D::qgridsystem1D() 
    : qsystem1D(1.0, 1.0, std::make_shared<qsim::pot::uniform<size_t>>(), 
                gauss_init1D(),
                std::make_shared<qsystem1D::explicit_evolver>())
{}

void qgridsystem1D::_init() {
    _wave = grid_wave1D::_new();
    _wave->wave_ref = &wave;
}

qgridsystem1D::~qgridsystem1D() {
    delete _wave;
}

void qgridsystem1D::_register_methods() {
    // godot built-in
    register_method("_physics_process", &qgridsystem1D::_fixed_process);
    register_method("_ready", &qgridsystem1D::_ready);

    // direct methods
    register_method("energy", &qgridsystem1D::_energy);
    register_method("position", &qgridsystem1D::_position);
    register_method("momentum", &qgridsystem1D::_momentum);
    register_method("set_wave", &qgridsystem1D::set_wave);
    register_method("wave", &qgridsystem1D::get_wave);
    register_method("psi", &qgridsystem1D::psi);
    register_method("psi_norm", &qgridsystem1D::psi_norm);
    register_method("psi_real", &qgridsystem1D::psi_real);
    register_method("psi_imag", &qgridsystem1D::psi_imag);
    register_method("N", &qgridsystem1D::N);
    register_method("x", &qgridsystem1D::x);
    
    // properties
    register_property<qgridsystem1D, double>("mass", &qgridsystem1D::set_mass, &qgridsystem1D::mass, 1.0);
    register_property<qgridsystem1D, double>("hbar", &qgridsystem1D::set_hbar, &qgridsystem1D::get_hbar, 1.0);
}


double qgridsystem1D::_energy() const {
    return qsystem1D::energy();
}

double qgridsystem1D::_position() const {
    auto * b = box();
    double pos = qsystem1D::position();
    return pos + ((b != nullptr) ? b->x() : 0);
}

double qgridsystem1D::_momentum() const {
    return qsystem1D::momentum();
}

void qgridsystem1D::set_hbar(double plank) {
    qsystem1D::set_hbar(plank);
}

double qgridsystem1D::get_hbar() const {
    return qsystem1D::hbar();
}

Vector2 qgridsystem1D::psi(int index) const {
    return Vector2(wave[index].real(), wave[index].imag());
}

double qgridsystem1D::psi_norm(int index) const {
    return std::norm(wave[index]);
}

double qgridsystem1D::psi_real(int index) const {
    return wave[index].real();
}

double qgridsystem1D::psi_imag(int index) const {
    return wave[index].imag();
}

size_t qgridsystem1D::N() const {
    return size();
}

double qgridsystem1D::x(size_t i) const {
    return qsystem1D::x(i);
}

void qgridsystem1D::set_wave(Ref<wave_init1D> init) {
    if (init != nullptr) {

        qsimbox * b = box();
        if (b != nullptr) {
            npdebug("Setting wave: width = ", b->width(), ", N = ", init->N)
            set_delta(b->width() / static_cast<double>(init->N));
        }

        replace_wave(*(*init));
        _wave->wave_ref = &wave;
    }
}

grid_wave1D * qgridsystem1D::get_wave() const {
    return _wave;
}

/*grid_potential1D * qgridsystem1D::set_potential(grid_potential1D * pot) {
    if (pot != nullptr && pot->is_safe()) {
        qsim::grid::qsystem1D::set_potential(*pot);
        m_pot = pot;
    }
}*/

void qgridsystem1D::_ready() {

    /*qsimbox * par = Object::cast_to<qsimbox>(get_parent());

    if (par != nullptr) {
        // obtain boundary information
        box = area;
    }*/


    //npdebug("Looking for a potential")
    // look for a potential
    /*auto list = get_children();
    for (int i = 0; i < list.size(); ++i) {
        Variant child = list[i];
        grid_potential * pot = Object::cast_to<grid_potential>(child);

        if (pot != nullptr && pot->is_safe()) {
            npdebug("Potential child found!")
            qsim::grid::qsystem1D::set_potential(*pot);
            m_pot = pot;
            break; // found
        }
    }*/
}

bool qgridsystem1D::_set_potential(grid_potential1D * pot) {
    if (pot->is_safe())
        qsim::grid::qsystem1D::set_potential(*pot);
    return pot->is_safe();
}

qsimbox * qgridsystem1D::box() const {
    return Object::cast_to<qsimbox>(get_parent());
}


void qgridsystem1D::_fixed_process(double dt) {
    // default values
    qsim::grid::qsystem1D::evolve(dt);
    qsim::grid::qsystem1D::normalize();
}
