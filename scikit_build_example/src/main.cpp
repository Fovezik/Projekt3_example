#include <pybind11/pybind11.h>
#include <matplot/matplot.h>
#include <AudioFile.h>
#include <cmath>
#include <string>
#include <vector>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

int add(int i, int j) {
    return i + j; 
}
int subtract(int i, int j) { 
    return i - j; 
}

void loadAudioFileAndPrintSummary(std::string filePath) {
    
    std::cout << "*********************************" << std::endl;
    std::cout << "Load Audio File and Print Summary" << std::endl;
    std::cout << "*********************************" << std::endl << std::endl;

    AudioFile<float> a;
    bool loadedOK = a.load(filePath);
    assert(loadedOK);

    std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
    std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
    std::cout << "Num Channels: " << a.getNumChannels() << std::endl;
    std::cout << "Length in Seconds: " << a.getLengthInSeconds() << std::endl;
    std::cout << std::endl;
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

    m.def ("subtract", &subtract, R"pbdoc(
        Substract two numbers.
    )pbdoc");

    m.def("audio", &loadAudioFileAndPrintSummary, R"pbdoc(
        Load Audio File and Print Summary.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
