#pragma once

#include "evolver.hpp"
#include "constants.hpp"

#include "debug.hpp"

namespace qsim::evo {

    template <typename Coords, class WaveFunction, class H>
    class explicit_scheme : public qsim::evolver<Coords, WaveFunction, H> {

    public:

        virtual WaveFunction evolve(const qsystem<Coords, WaveFunction, H>& system, double dt) const {
            return (1.0 - (dt / system.hbar()) * system.hemiltonian()) * system.psi();
        }
    };
}
