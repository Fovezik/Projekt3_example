#include <pybind11/pybind11.h>
#include <matplot/matplot.h>
#include <cmath>
#include <string>
#include <vector>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using namespace std;

string add(int i, int j) {
    return to_string(i+j);
}

string substract(int i, int j) {
    if (i < j) return "First number is smaller than second number!";
    else return to_string(i - j);
}

int sinus() {
    vector<double> x = linspace(0, 2 * pi);
    vector<double> y = transform(x, [](auto x) { return sin(x); });
    matplot::plot(x, y, "-o");
    matplot::hold(on);
    matplot::plot(x, transform(y, [](auto y) { return -y; }), "--xr");
    matplot::plot(x, transform(x, [](auto x) { return x / pi - 1.; }), "-:gs");
    matplot::plot({ 1.0, 0.7, 0.4, 0.0, -0.4, -0.7, -1 }, "k");
    matplot::show();
    return 0;
}

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: scikit_build_example

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

    m.def("add", &add, R"pbdoc(
        Add two numbers.
    )pbdoc");

    m.def ("substract", &substract, R"pbdoc(
        Substract two numbers.
    )pbdoc");

    m.def("sinus", &sinus, R"pbdoc(
        Generates sinus.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
