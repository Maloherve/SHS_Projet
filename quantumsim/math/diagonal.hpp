#pragma once

namespace qsim::math {
    
    // constant random access
    template<typename T>
    class diagonal {

        virtual T operator[](size_t m) const = 0;
    };

    template<typename T, size_t N>
    class diag_array : public diagonal<T> {

        // array
        std::array<T,N> data;
    
    public:
        diag_array(const std::array<T,N>& _data) : data(_data) {}

        virtual T operator[](size_t m) const override {
            return data[m];
        }
    };

    template<typename T, Closure F>
    class diag_functor {
        
        // functor T f(size_t)
        F f; 
    
    public:
        diag_functor(F&& _f) : f(_f) {}

        virtual T operator[](size_t m) const override {
            return f(m);
        }
    };
}

/*
 * Matrix multiplication by diagonal
 */
template<typename T, size_t N, template <typename _T, size_t _N> class V>
V<T,N> operator*(const qsim::math::diagonal<T>& mat, V<T,N> v) {

    for (size_t k = 0; k < N; ++k)
        v[k] *= mat[k];

    return v;
}
