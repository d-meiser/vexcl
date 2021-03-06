#include <iostream>
#include <fstream>
#include <string>
#include <random>

#include <vexcl/vexcl.hpp>
#include <vexcl/mba.hpp>

// https://github.com/ddemidov/mba
#include <mba/mba.hpp>

template <typename T = double>
inline std::array<T, 2> make_array(T x, T y) {
    std::array<T, 2> p = {{x, y}};
    return p;
}

int main(int argc, char *argv[]) {
    const size_t n = argc < 2 ? 1024 * 1024 : std::stoi(argv[1]);
    const size_t m = 100;

    vex::Context ctx( vex::Filter::Env );
    std::cout << ctx << std::endl;

    vex::profiler<> prof(ctx);

    prof.tic_cpu("generate data");
    std::default_random_engine rng(0);
    std::uniform_real_distribution<double> rnd(0.0, 1.0);

    std::vector< std::array<double, 2> > p(n);
    std::generate(p.begin(), p.end(),
            [&]() {
                return make_array(rnd(rng), rnd(rng));
            });

    std::vector<double> v(n);
    std::transform(p.begin(), p.end(), v.begin(),
            [](const std::array<double,2> &c) {
                double x = c[0] - 0.5;
                double y = c[1] - 0.5;
                return x * x + y * y;
            });

    std::vector< double > x(n);
    std::vector< double > y(n);

    std::generate(x.begin(), x.end(), [&]() { return rnd(rng); });
    std::generate(y.begin(), y.end(), [&]() { return rnd(rng); });

    x[0] = y[0] = 0.5;
    prof.toc("generate data");

    prof.tic_cpu("GPU");
    {
        prof.tic_cpu("setup");
        vex::mba<2> surf(
                ctx, make_array(-0.01, -0.01), make_array(1.01, 1.01),
                p, v, make_array<size_t>(2, 2)
                );
        prof.toc("setup");

        vex::multivector<double, 2> C(ctx, n);
        vex::vector<double> Z(ctx, n);

        vex::copy(x, C(0));
        vex::copy(y, C(1));

        prof.tic_cl("interpolate");
        for(size_t i = 0; i < m; ++i)
            Z = surf(C(0), C(1));
        prof.toc("interpolate");
        std::cout << "surf(0.5, 0.5) = " << Z[0] << std::endl;
    }
    prof.toc("GPU");

    prof.tic_cpu("CPU");
    {
        prof.tic_cpu("setup");
        mba::cloud<2> surf(
                make_array(-0.01, -0.01), make_array(1.01, 1.01),
                p, v, make_array<size_t>(2, 2)
                );
        prof.toc("setup");

        std::vector<double> z(n);

        prof.tic_cpu("interpolate");
        for(size_t j = 0; j < m; ++j)
#pragma omp parallel for
            for(size_t i = 0; i < n; ++i)
                z[i] = surf(x[i], y[i]);
        prof.toc("interpolate");
        std::cout << "surf(0.5, 0.5) = " << z[0] << std::endl;
    }
    prof.toc("CPU");

    std::cout << prof << std::endl;
}
