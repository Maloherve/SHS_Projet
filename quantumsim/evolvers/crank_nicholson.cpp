#include "crank_nicholson.hpp"

#include "grid/qsystem1D.hpp"
#include "grid/qsystem2D.hpp"
#include "math/composition.hpp"

#include "debug.hpp"

using namespace qsim::grid;
using namespace qsim::evo;

using namespace std::complex_literals;

template <class Matrix, class Vector>
Vector solve_tridiagonal(const Matrix&, Vector);

/*
 * 1D case
 */

wave_vector crank_nicholson::evolve(const qsystem1D& system, double dt) const {
    auto H = system.H();
    // deduce y vector
    wave_vector y = (wave_t(1.0) - (1i * dt / (2 *system.hbar())) * H) * system.psi();
    // solve y = ( 1 + i * dt / (2 * hbar) * H ) * x 
    wave_vector x = solve_tridiagonal<H_matrix_1D, wave_vector>((wave_t(1.0) + (1i * dt / (2 *system.hbar())) * H), y);
    return x;
}

/*
 * 2D case
 */

typedef qsim::math::composition<
                              wave_t, 
                              math::diagonals<wave_t, 3>, //dx
                             > laplace_t;

typedef qsim::math::composition<
                              wave_t, 
                              math::diagonals<wave_t, 3>, //dx
                              math::diag_functor<wave_t>
                             > H_matrix_2D;

wave_grid crank_nicholson::evolve(const qsystem2D& system, double dt) const {
    // laplace coefficient
    wave_t l_tau = 1i * system.hbar() * dt / (2 * system.mass());
    // potential coefficient
    wave_t p_tau = 1i * dt / (2 * system.hbar());
    
    // copy the system.psi() buffer
    wave_grid psi(system.psi());

    // for each column, compute first matrix multiplication
    for (size_t j = 0; j < psi.cols_nb(); ++j) {
        // generate the operator
        H_matrix_2D Ty(wave_t(1.0), 
                      l_tau * qsystem2D::A,
                      (-p_tau) * system.potential_on_column(j));
        
        // re-assign the column only
        // TODO, check if move semantics is applied
        psi.get_column(j) = Ty * psi.get_column(j);
    }

    // for each line, compute second matrix multiplication
    for (size_t i = 0; i < psi.cols_nb(); ++j) {
        // generate the operator
        laplace_t Tx(wave_t(1.0), l_tau * qsystem2D::A);
        
        // re-assign the line only
        // TODO, check if move semantics is applied
        psi.get_row(j) = Tx * psi.get_row(j);
    }

    // for each line, compute the half-implicit step
    for (size_t i = 0; i < psi.cols_nb(); ++j) {
        // generate the operator
        laplace_t Tx(wave_t(1.0), (-l_tau) * qsystem2D::A);
        
        // re-assign the line only
        // TODO, check if move semantics is applied
        psi.get_row(j) = solve_tridiagonal(Tx, psi.get_row(j));
    }

    // for each column, complete the other half-implicit step
    for (size_t j = 0; j < psi.cols_nb(); ++j) {
        // generate the operator
        H_matrix_2D Ty(wave_t(1.0), 
                      (-l_tau) * qsystem2D::A,
                      p_tau * system.potential_on_column(j));
        
        // re-assign the column only
        // TODO, check if move semantics is applied
        psi.get_column(j) = solve_tridiagonal(Ty, psi.get_column(j));
    }

    return psi;
}

/*
 * Gauss jordan solver with the following hypothesis:
 *    - A is tridiagonal
 */
template <class Matrix, class Vector>
Vector solve_tridiagonal(const Matrix& A, Vector x) {
	Vector new_diag(x);

    new_diag[0] = A.at(0,0);
    
    for(std::size_t i(1); i < x.size(); ++i) {

	    wave_t pivot = A.at(i-1, i) / A.at(i-1, i-1);
	    x[i] -= x[i-1] * pivot;
	    new_diag[i] = A.at(i,i) - A.at(i, i-1) * pivot;
	}

	for(std::size_t i(x.size()-1); i > 0; --i) {
	    x[i] /= new_diag[i];
        x[i-1] -= A.at(i-1, i) * x[i];
	}
    
    x[0u] /= new_diag[0u];

	return x;
}


