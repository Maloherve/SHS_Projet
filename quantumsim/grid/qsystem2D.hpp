#pragma once

#include "fwd.hpp"
#include "qsystem.hpp"
#include "math/composition.hpp"
#include "math/diagonals.hpp"
#include "math/diagonal.hpp"

#include "math/matrix.hpp"

namespace qsim::grid {

    typedef qsim::math::composition<
                              wave_t, 
                              math::diagonals<wave_t, 3> //dx
                             > laplace_t;

    // concretization for a 2D grid
    class qsystem2D : public qbi_gridsystem {
    private:

        static const math::diagonals<wave_t, 3> A;
 
        double dx; // discretization along x
        double dy; // discretization along y

    public: 

        struct init_pack {
            std::function<qsim::wave_t (double, double)> f;
            size_t N;
            size_t M;

            init_pack(const std::function<qsim::wave_t (double, double)>& _f = std::function<qsim::wave_t (double, double)>(),
                      size_t _N = 0,
                      size_t _M = 0
                     ) : f(_f), N(_N), M(_M) {}

            inline qsim::wave_t operator()(double x, double y) const {
                return f(x,y);
            }

            wave_grid generate(double dx, double dy) const;
        };

        qsystem2D(double _m, 
                  double _dx, double _dy,
                  std::shared_ptr<potential<size_t, size_t>> _V,
                  const init_pack& init = init_pack(),
                  std::shared_ptr<evolver> _evolver = nullptr,
                  double hbar = 1.0
                  );
        
        virtual void evolve(double) override;
        
        // integrals
        virtual double norm() const override;
        virtual double energy() const override;
        std::pair<double,double> position() const;
        std::pair<double,double> momentum() const;

        /*
         * External access to the potential operator
         */

        math::diag_functor<wave_t> potential_on_row(size_t i) const;
        math::diag_functor<wave_t> potential_on_column(size_t j) const;

        /*
         * Discretization setter
         */

        void set_delta_x(double);
        double delta_x() const;

        void set_delta_y(double);
        double delta_y() const;
        
        // mapping
        inline double x(size_t i) const {
            return static_cast<double>(i) * dx;
        }

        inline double y(size_t j) const {
            return static_cast<double>(j) * dy;
        }

        /*
         * Causing of boundaries size is decreased of 2
         */
        
        size_t N() const;
        size_t M() const;
    };
}
