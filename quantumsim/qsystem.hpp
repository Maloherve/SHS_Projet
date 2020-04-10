#pragma once

#include <complex>
#include <memory>
#include "math/composition.hpp"
#include "evolver.hpp"

namespace qsim {
    
    template <typename Coords>
    class potential;

    /*
     * Define the WaveFunction internal type as std::complex<double>
     */
    
    typedef std::complex<double> wave_t;

    /*
     * Most general description of quantum system
     */
    template <typename Coords, class WaveFunction, class H>
    class qsystem {
    protected:

        // the wave function
        WaveFunction wave;

    private:
        // mass
        double m;

        // a potential, external management
        std::shared_ptr<potential<Coords>> pot;

        // an evolver
        std::shared_ptr<evolver<Coords, WaveFunction, H>> m_evolver;
        
        // plank constant
        double plank;

    public:
        qsystem(double _m, 
                const WaveFunction& _wave,
                std::shared_ptr<potential<Coords>> _V,
                std::shared_ptr<evolver<Coords, WaveFunction, H>> _evolver,
                double _hbar = 1.0
                )
            : wave(_wave), m(_m), pot(_V), m_evolver(_evolver), plank(_hbar) {

            if (pot == nullptr) {
                // TODO, throw error
            }

            if (m_evolver == nullptr) {
                // TODO, throw error
            }
                
            if (m < 0)
                m = -m;
            
            if (plank < 0)
                plank = -plank;
        }
        
        // very important the virtual destructor
        virtual ~qsystem() {}

        // access to mass
        double mass() const {
            return m;
        }
        
        // eventually change the behaviour
        virtual void set_mass(double _m) {
            this->m = abs(_m);
        }

        virtual void set_hbar(double hb) {
            plank = hb;
        }

        inline double hbar() const {
            return plank;
        }

        // evolution in time
        void evolve(double dt) {
            wave = std::move(m_evolver->evolve(*this, dt));
            post(dt);
        }

        void set_evolver(std::shared_ptr<evolver<Coords, WaveFunction, H>> evo) {
            m_evolver = evo;
        }

        void set_potential(std::shared_ptr<potential<Coords>> _V) {
            pot = _V;
        }
        
        // just in case after the evolution, some constraints must be set
        virtual void post(double dt) {}


        inline const WaveFunction& psi() const {
            return wave;
        }

        inline void replace_wave(const WaveFunction& other) {
            wave = other;
        }

        inline void replace_wave(WaveFunction&& other) {
            wave = std::move(other);
        }

        inline const potential<Coords>& V() const {
            return const_cast<const potential<Coords>&>(*pot);
        }

        // hemiltonian access
        virtual H hemiltonian() const = 0;
        
        // averanges
        virtual double energy() const = 0;
        
        // normalization
        virtual double normalize() = 0;

    protected:
        
        // protected access
        inline WaveFunction& psi() {
            return wave;
        }

        std::shared_ptr<potential<Coords>> potential_ptr() const {
            return pot;
        }
    };
}
