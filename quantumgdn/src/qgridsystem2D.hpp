#pragma once

#include "quantumsim/grid/qsystem1D.hpp"

#include <Godot.hpp>
#include <Node2D.hpp>
#include <Reference.hpp>
#include "potential.hpp"

#include "wave_init2D.hpp"
#include "grid_wave.hpp"

namespace godot {

    class qgridsystem2D : private qsim::grid::qsystem2D, public Node2D {
        GODOT_CLASS(qgridsystem2D, Node2D)

        // implicit child, accessible by psi
        grid_wave * wave_fct;

        // explicit child, accessible by V
        // it's supposed to be an external object
        grid_potential * m_potential;
        
    public:
        qgridsystem2D();
        ~qgridsystem2D();
        
        /*
         * Methods declarations
         */

        Vector2 position() const;
        Vector2 momentum() const;

        /*
         * Potential property
         */
        void _set_potential(grid_potential * pot);
        grid_potential * _get_potential() const;

        //void set_evolver(Reference* integ);
        //Reference * get_evolver() const;
        
        /*
         * Access to wave function
         */

        void set_wave(Ref<wave_init2D>);

        // GD constructor
        void _init();
        void _ready();
        
        // GD evolve
        void _fixed_process(double dt);

        // bindings
        static void _register_methods();
    };
}
