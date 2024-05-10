#include <pybind11/pybind11.h>
#include <matplot/matplot.h>
#include <AudioFile.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

const double PI = 3.14;

struct Wave {
    std::vector<double> time;
    std::vector<double> wave;
};

void show_audio(std::string filepath, int accuracy);
void show_2d_wave(Wave generated_wave);
Wave generate_wave(std::string choice, double frequency);

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

void generate_and_show_wave(std::string choice, double frequency) {
    Wave generated_wave = generate_wave(choice, frequency);
    if (!generated_wave.time.empty() || !generated_wave.wave.empty()) {
        matplot::plot(generated_wave.time, generated_wave.wave);
        matplot::title("Generated wave");
        matplot::xlabel("Preiod (pi)");
        matplot::ylabel("Amplitude");
        matplot::ylim({ -1.5, 1.5 });
        matplot::show();
    }
    else {
        std::cout << "No " << choice << " wave avaible!" << std::endl;
        return;
    }
}

Wave generate_wave(std::string choice, double frequency) {

    Wave generated_wave;

    if (frequency <= 0) {
        std::cout << frequency << " is incorrect frequency!" << std::endl;
        return generated_wave;
    }
    
    double length;

    if (frequency < 1) { length = 628 * (1 / frequency); }
    else { length = 628; }

    if (choice == "sine") {
        for (int i = 0; i < length; i++) {
            generated_wave.time.push_back(static_cast<double>(i) / 314);
            generated_wave.wave.push_back(std::sin(PI * frequency * generated_wave.time[i]));
        }
        std::cout << "Sine wave with frequency = " << frequency << " generated succesfully!" << std::endl;
        return generated_wave;
    }

    if (choice == "cosine") {
        for (int i = 0; i < length; i++) {
            generated_wave.time.push_back(static_cast<double>(i) / 314);
            generated_wave.wave.push_back(std::cos(PI * frequency * generated_wave.time[i]));
        }
        std::cout << "Cosine wave with frequency = " << frequency << " generated succesfully!" << std::endl;
        return generated_wave;
    }

    if (choice == "square") {
        for (int i = 0; i < length; i++) {
            generated_wave.time.push_back(static_cast<double>(i) / 314);
            generated_wave.wave.push_back((i % static_cast<int>(628 / frequency) < (628 / (2 * frequency))) ? 1 : -1);
        }
        std::cout << "Square wave with frequency = " << frequency << " generated succesfully!" << std::endl;
        return generated_wave;
    }

    if (choice == "sawtooth") {
        for (int i = 0; i < length; i++) {
            generated_wave.time.push_back(static_cast<double>(i) / 314);
            generated_wave.wave.push_back(((i * frequency / 628) - floor(i * frequency / 628)) * 2 - 1);
        }
        std::cout << "Sawtooth wave with frequency = " << frequency << " generated succesfully!" << std::endl;
        return generated_wave;
    }

    return generated_wave;
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

    m.def("show_audio", &show_audio, R"pbdoc(
        Shows audio using matplot.
    )pbdoc");

    m.def("generate_wave", &generate_and_show_wave, R"pbdoc(
        Generate and show chosen wave.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
