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

    if (loadedOK) {
        std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
        std::cout << "Number of samples per channel: " << a.getSampleRate() * a.getLengthInSeconds() << std::endl;
        std::cout << "Length in Seconds: " << a.getLengthInSeconds() << std::endl;
        std::cout << std::endl;
    }
}

void show_audio(std::string filepath, int accuracy) {

    if (accuracy < 1 || accuracy > 10) {
        std::cout << "Incorrect accuracy!";
        return;
    }

    AudioFile<float> a;
    a.load(filepath);

    std::vector<float> audio_data = a.samples[0];
    std::vector<float> time;

    int sample_rate = a.getSampleRate();

    for (int i = 0; i < audio_data.size(); i += (accuracy * 10)) {
        time.push_back(static_cast<float>(i) / sample_rate);
    }

    std::vector<float> sampled_audio;

    for (int i = 0; i < time.size(); i++) {
        sampled_audio.push_back(audio_data[static_cast<int>(time[i] * sample_rate)]);
    }

    matplot::plot(time, sampled_audio);
    matplot::xlabel("Time (seconds)");
    matplot::ylabel("Amplitude");
    matplot::title("Audio Waveform");
    matplot::show();
}


void generate_and_show_sphere(int radius, int accuracy) {

    if (accuracy < 1 || accuracy > 10) {
        std::cout << "Incorrect accuracy!";
        return;
    }

    std::vector<float> x, y, z;
    float pi = 3.14;

    for (float p = 0.00; p < 2 * pi; p += (accuracy * 0.01)) {
        for (float q = -0.50 * pi; q < 0.50 * pi; q += (accuracy * 0.01)) {
            x.push_back(radius * cos(p) * cos(q));
            y.push_back(radius * sin(p) * cos(q));
            z.push_back(radius * sin(q));
        }
    }

    matplot::plot3(x, y, z);
    matplot::title("Sphere");
    matplot::xlabel("x");
    matplot::ylabel("y");
    matplot::zlabel("z");
    matplot::show();
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

    m.def("audio_test", &loadAudioFileAndPrintSummary, R"pbdoc(
        Load Audio File and Print Summary.
    )pbdoc");

    m.def("show_audio", &show_audio, R"pbdoc(
        Shows audio using matplot.
    )pbdoc");

    m.def("generate_sphere", &generate_and_show_sphere, R"pbdoc(
        Generate and show sphere.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
