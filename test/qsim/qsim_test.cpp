#include <iostream>

#include "grid/qsystem1D.hpp"
#include "grid/wave.hpp"
#include "potentials/uniform.hpp"
#include "evolvers/explicit.hpp"

#include <fstream>

using namespace qsim;
using namespace qsim::grid;
using namespace std;

wave_vector init_wave();

constexpr double mass = 1;
constexpr double dt = 1;
constexpr std::pair<double, double> bounds = {-50, 50};
constexpr size_t steps = 100;

int main() {
    
    // flat zero potential
    auto V = std::make_shared<pot::uniform<size_t>>();
    auto integ = std::make_shared<qsystem1D::explicit_evolver>();
    
    // step 1, initialization
    qsystem1D system(mass, bounds, init_wave(), V, integ);

    ofstream out("wave_test.out");

    for (size_t k = 0; k < steps; k++) {
        // print out psi
        for (const auto& psi : system.psi())
            out << abs(psi) << "  ";
        out << endl;

        system.evolve(dt); 
    }

    out.close();
}

wave_vector init_wave() {

    using namespace std::complex_literals;

    constexpr size_t N = 50;

    wave_vector v(N);
    constexpr double L = bounds.second - bounds.first;
    constexpr double s_norm = 1;
    constexpr double x0 = 0;
    constexpr double n = 1;
    constexpr double sigma(s_norm * L);
    constexpr double k0(2 * M_PI * n / L);

    for (size_t k = 0; k < N; ++k) {
        double x = bounds.first + k * L / N;
        v[k] = exp(1i * k0 * x - pow((x - x0)/sigma, 2) / 2);
    }

    return v;
}

