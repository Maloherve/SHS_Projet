#include <Godot.hpp>
#include <Node.hpp>

#include "potential.hpp"
#include "uniform_potential.hpp"
#include "grid_wave.hpp"
#include "qgridsystem1D.hpp"

using namespace godot;

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o)
{
	Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o)
{
	Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle)
{
	Godot::nativescript_init(handle);
    
    // test
	register_class<grid_potential>();
	register_class<uniform_potential>();
	register_class<grid_wave>();
	register_class<qgridsystem1D>();
}

