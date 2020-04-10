#include "grid/qsystem2D.hpp"
#include "constants.hpp"
#include "potential.hpp"

#include "grid/integrators.hpp"

using namespace grid;

const qsim::math::diagonals<double, 3> qsystem2D::A_x = math::diagonals<double, 3>({math::sdiag_entry(-1, 1.0), math::sdiag_entry(0, -2.0), math::sdiag_entry(1, 1.0)});

const qsim::math::diagonals<double, 3> qsystem2D::A_y(size_t M) {
    return math::diagonals<double, 3>({math::sdiag_entry(-M, 1.0), math::sdiag_entry(0, -2.0), {math::sdiag_entry(M, 1.0));
}

qsim::math::diagonals<double, 3> qsystem2D::H_zero_x() const {
    return math::diagonals<double,3>((- hbar() * hbar() / (2 * mass() * pow(dx, 2))) * A_x);
}

qsim::math::diagonals<double, 3> qsystem2D::H_zero_y() const {
    return math::diagonals<double,3>((- hbar() * hbar() / (2 * mass() * pow(dy, 2))) * A_y(_M));
}

// init pack
wave_vector qsystem2D::init_pack::generate(double dx, double dy) const {
    wave_vector w((N+2)*(M+2), qsim::wave_t(0.0));

    // construct it using the analytic expression
    for (size_t i = 1; i <= N; ++i) {
        for (size_t j = 1; j <= M; ++j)
            w[(M-1)*i+j] = f(i * dx, j * dy);
    }

    return w;
}

// constructor
qsystem2D::qsystem2D(double _m, 
                     double _dx, double _dy,
                     std::shared_ptr<potential<size_t>> _V,
                     const init_pack& init,
                     std::shared_ptr<evolver<size_t, wave_vector, grid_H_2D>> _evolver,
                     double hbar()
                    )
    : qgridsystem<H_matrix_2D>(_m, init.generate(_dx,_dy), _V, _evolver, hbar()),
      H(
          0.0,
          H_zero_x(), 
          H_zero_y(), 
          std::function<double (size_t)>([&] (size_t k) -> double { return V()(k); }) 
      ),
      dx(_dx), dy(_dy), _N(init.N), _M(init.M)
{
    boundaries_setup();   
}

void qsystem2D::update_H_x() {
    H.get<1>() = H_zero_x();
}

void qsystem2D::update_H_y() {
    H.get<2>() = H_zero_y();
}

void qsystem2D::boundaries_setup() {

    // j = 0, j = M
    for (size_t i = 0; i <= N+1; ++i) {
        wave[map(i,0)] = 0.0;
        wave[map(i,M+1)] = 0.0;
    }

    // i = 0, i = N
    for (size_t j = 0; j <= M+1; ++j) {
        wave[map(0,j)] = 0.0;
        wave[map(N+1,j)] = 0.0;
    }
}

// change the hemiltonian expression
void qsystem2D::set_mass(double _m)  {
    qgridsystem<H_matrix_2D>::set_mass(_m);
    update_H();
}

// allow to set boundaries
void qsystem2D::post(double) {
    boundaries_setup();
}

// implementations
double qsystem2D::energy() const {
    qsim::wave_t E(0);
    double psix = pow(hbar()/dx,2) / (2 * mass());
    double psiy = pow(hbar()/dy,2) / (2 * mass());
    double v = 2 * (psix + psiy);
    
    // apply the hamiltonian
    for (auto it = begin(); it != end(); ++it()) {
        E += (V(it.x(), it.y()) + v) * (*it) 
           - psix * (it.right() + it.left()) 
           - psiy * (it.up() + it.down());
    }

    if (E.imag() > qsim::machine_prec) {
        npdebug("Value: ", E)
        throw std::runtime_error("Energy computation isn't fully real");
    }

    return E.real();
}

std::pair<double,double> qsystem2D::position() const {
    std::pair<double,double> out(0, 0);
    for (auto it = begin(); it != end(); ++it()) {
        out.first += it.x() * std::norm(*it);
        out.second += it.y() * std::norm(*it);
    }

    out.first *= dx * dy;
    out.second *= dx * dy;
    return out;
}

std::pair<double,double> qsystem2D::momentum() const {
    qsim::wave_t px(0), py(0);
    for (auto it = begin(); it != end(); ++it()) {
        px += it.right() - it.left();
        py += it.up() - it.down();
    }

    using namespace std::complex_literals;

    px *= - 1i * hbar() * dy / 2;
    py *= - 1i * hbar() * dx / 2;

    if (px.imag() > qsim::precision || py.imag() > qsim::precision) {
        npdebug("Values: (", px, ", ", py, ")")
        throw std::runtime_error("Momentum computation isn't fully real");
    }

    return std::pair<double, double>(px.real(), py.imag());
}

// normalize the wave function
double qsystem2D::norm() const {
    double out(0);
    for (const qsim::wave_t& val : *this)
        out += std::norm(val);

    return out * dx * dy;
}

//  these functions
void qsystem2D::replace_wave(const wave_vector& other, size_t M_) {
    qgridsystem<H_matrix_1D>::replace_wave(other);
    _M = M_;
    boundaries_setup();
    update_H_y(); // update matrix
}

void replace_wave(const std::function<qsim::wave_t (double, double)>& init, size_t N_, size_t M_) {
    wave_vector w((N_+2)*(M_+2), qsim::wave_t(0.0));
    _N = N_;
    _M = M_;

    // construct it using the analytic expression
    for (auto it = begin(); it != end(); ++it()) {
        *it = init(it.x(), it.y());
    }
    
    update_H_y(); 
    wave = std::move(w);
}

void qsystem2D::replace_wave(wave_vector&& other, size_t M_) {
    qgridsystem<H_matrix_1D>::replace_wave(other);
    _M = M_;
    boundaries_setup();
    update_H_y(); // update matrix
}

size_t qsystem2D::N() const {
    return _N;
}
size_t qsystem2D::M() const {
    return _M;
}


iterator qsystem2D::begin() {
    return iterator(*this, 1, 1);
}

const_iterator qsystem2D::begin() const {
    return const_iterator(*this, 1, 1);
}

iterator qsystem2D::end() {
    return iterator(*this, _N+1, 1);
}

const_iterator qsystem2D::end() const {
    return const_iterator(*this, _N+1, 1);
}

/*
 * iterator section
 */ 

iterator::iterator(qsystem2D& _sys, size_t _i, size_t _j) 
    : sys(_sys), i(_i), j(_j)
{
}

void qsystem2D::iterator::increment {
    if (j < sys.M())
        ++j;
    else {
        ++i;
        j = 1;
    }
}


bool qsystem2D::iterator::operator!=(const iterator& other) const {
    return i != other.i || j != other.j;
}

qsim::wave_t& qsystem2D::iterator::operator*() const {
    return sys.wave[sys.map(i,j)];
}


qsim::wave_t& qsystem2D::iterator::up() const {
    return sys.wave[sys.map(i,j+1)];
}

qsim::wave_t& qsystem2D::iterator::down() const {
    return sys.wave[sys.map(i,j-1)];
}

qsim::wave_t& qsystem2D::iterator::left() const {
    return sys.wave[sys.map(i-1,j)];
}

qsim::wave_t& qsystem2D::iterator::right() const {
    return sys.wave[sys.map(i+1,j)];
}

double qsystem2D::iterator::x() const {
    return sys.x(i);
}

double qsystem2D::iterator::y() const {
    return sys.y(j);
}

// const iterator

const_iterator::const_iterator(qsystem2D& _sys, size_t _i, size_t _j) 
    : sys(_sys), i(_i), j(_j)
{
}

void qsystem2D::const_iterator::increment {
    if (j < sys.M())
        ++j;
    else {
        ++i;
        j = 1;
    }
}

bool qsystem2D::const_iterator::operator!=(const const_iterator& other) const {
    return i != other.i || j != other.j;
}

qsim::wave_t& qsystem2D::const_iterator::operator*() const {
    return sys.wave[sys.map(i,j)];
}


qsim::wave_t& qsystem2D::const_iterator::up() const {
    return sys.wave[sys.map(i,j+1)];
}

qsim::wave_t& qsystem2D::const_iterator::down() const {
    return sys.wave[sys.map(i,j-1)];
}

qsim::wave_t& qsystem2D::const_iterator::left() const {
    return sys.wave[sys.map(i-1,j)];
}

qsim::wave_t& qsystem2D::const_iterator::right() const {
    return sys.wave[sys.map(i+1,j)];
}

double qsystem2D::const_iterator::x() const {
    return sys.x(i);
}

double qsystem2D::const_iterator::y() const {
    return sys.y(j);
}
