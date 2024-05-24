#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <matplot/matplot.h>
#include <AudioFile.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
namespace mp = matplot;

const double PI = 3.14;

class Wave {
    public:
        std::vector<double> time;
        std::vector<double> wave;
        int sample_rate;
};

Wave read_audio(std::string filepath) {

    Wave audio;

    AudioFile<double> a;
    a.load(filepath);

    std::vector<double> audio_data = a.samples[0];
    audio.sample_rate = a.getSampleRate();

    for (int i = 0; i < audio_data.size(); i++) {
        audio.time.push_back(static_cast<double>(i) / audio.sample_rate);
    }

    for (int i = 0; i < audio.time.size(); i++) {
        audio.wave.push_back(audio_data[static_cast<int>(audio.time[i] * audio.sample_rate)]);
    }
    
    std::cout << "Audio file: " << filepath << " readed succesfully!" << std::endl;

    return audio;
}

Wave lowpass_filter(Wave& audio, double cutoff_freq) {

    Wave filtered_audio;

    filtered_audio.time = audio.time;
    filtered_audio.sample_rate = audio.sample_rate;
    filtered_audio.wave.resize(audio.wave.size());

    double wc = 2.0 * PI * cutoff_freq / filtered_audio.sample_rate;
    double alpha = sin(wc) / (2.0 * cos(wc));
    double b0 = (1.0 - cos(wc)) / 2.0;
    double b1 = 1.0 - cos(wc);
    double b2 = (1.0 - cos(wc)) / 2.0;
    double a0 = 1.0 + alpha;
    double a1 = -2.0 * cos(wc);
    double a2 = 1.0 - alpha;

    double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0;

    for (int i = 0; i < audio.wave.size(); i++) {
        double x0 = audio.wave[i];
        double y0 = (b0 / a0) * x0 + (b1 / a0) * x1 + (b2 / a0) * x2 - (a1 / a0) * y1 - (a2 / a0) * y2;
        x2 = x1;
        x1 = x0;
        y2 = y1;
        y1 = y0;
        filtered_audio.wave[i] = y0;
    }

    AudioFile<double> a;
    a.setNumChannels(2);
    a.setNumSamplesPerChannel(filtered_audio.sample_rate);
    for (int i = 0; i < a.getNumChannels(); i++) {
        a.samples[i] = filtered_audio.wave;
    }
    a.save("outputs\\test-audio-filtered.wav", AudioFileFormat::Wave);

    return filtered_audio;
}

Wave generate_wave(std::string choice, double frequency) {

    Wave generated_wave;

    if (frequency <= 0) {
        std::cout << frequency << " is incorrect frequency!" << std::endl;
        return generated_wave;
    }
    
    double length;

    if (frequency < 1) length = 628 * (1 / frequency);
    else length = 628;

    if (choice == "sine") {
        for (int i = 0; i < length; i++) {
            generated_wave.time.push_back(static_cast<double>(i) / 314);
            generated_wave.wave.push_back(std::sin(2 * PI * frequency * generated_wave.time[i]));
        }
        std::cout << "Sine wave with frequency = " << frequency << " generated succesfully!" << std::endl;
        return generated_wave;
    }

    if (choice == "cosine") {
        for (int i = 0; i < length; i++) {
            generated_wave.time.push_back(static_cast<double>(i) / 314);
            generated_wave.wave.push_back(std::cos(2 * PI * frequency * generated_wave.time[i]));
        }
        std::cout << "Cosine wave with frequency = " << frequency << " generated succesfully!" << std::endl;
        return generated_wave;
    }

    if (choice == "square") {
        for (int i = 0; i < length; i++) {
            generated_wave.time.push_back(static_cast<double>(i) / 314);
            generated_wave.wave.push_back((i % static_cast<int>(314 / frequency) < (314 / (2 * frequency))) ? 1 : -1);
        }
        std::cout << "Square wave with frequency = " << frequency << " generated succesfully!" << std::endl;
        return generated_wave;
    }

    if (choice == "sawtooth") {
        for (int i = 0; i < length; i++) {
            generated_wave.time.push_back(static_cast<double>(i) / 314);
            generated_wave.wave.push_back(((i * frequency / 314) - floor(i * frequency / 314)) * 2 - 1);
        }
        std::cout << "Sawtooth wave with frequency = " << frequency << " generated succesfully!" << std::endl;
        return generated_wave;
    }
    std::cout << "No " << choice << " wave avaible!" << std::endl;
    std::cout << "Press ENTER to continue...";
    std::cin.get();
    return generated_wave;
}

void plot_line(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.empty() || y.empty()) return;
    mp::plot(x, y);
    mp::show();
}
 
py::array_t<int> gaussian_filter(py::array_t<int> input, float sigma) {

    auto buf = input.request();

    int height = buf.shape[0];
    int width = buf.shape[1];
    int channels = buf.shape[2];

    float sum = 0.0;
    std::vector<std::vector<float>> kernel(5, std::vector<float>(5));

    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            float value = std::exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            kernel[i + 2][j + 2] = value;
            sum += value;
        }
    }

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            kernel[i][j] /= sum;
        }
    }

    py::array_t<int> output({ height, width, channels });
    auto output_buf = output.request();

    int* input_ptr = static_cast<int*>(buf.ptr);
    int* output_ptr = static_cast<int*>(output_buf.ptr);

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            for (int c = 0; c < channels; c++) {
                float sum2 = 0.0;
                for (int i = -2; i <= 2; i++) {
                    for (int j = -2; j <= 2; j++) {
                        int x = std::min(std::max(w + j, 0), width - 1);
                        int y = std::min(std::max(h + i, 0), height - 1);
                        sum2 += input_ptr[(y * width + x) * channels + c] * kernel[i + 2][j + 2];
                    }
                }
                output_ptr[(h * width + w) * channels + c] = std::min(std::max(static_cast<int>(sum2), 0), 255);
            }
        }
    }

    return output;
}

py::array_t<int> grayscale(py::array_t<int> input) {
 
    auto buf = input.request();

    int height = buf.shape[0];
    int width = buf.shape[1];

    auto output = py::array_t<int>({ height, width });
    auto output_buf = output.request();

    int* input_ptr = static_cast<int*>(buf.ptr);
    int* output_ptr = static_cast<int*>(output_buf.ptr);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            int index = i * width * 3 + j * 3;

            int r = input_ptr[index];
            int g = input_ptr[index + 1];
            int b = input_ptr[index + 2];

            int gray = static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b);

            output_ptr[i * width + j] = gray;
        }
    }

    return output;
}

PYBIND11_MODULE(_core, m) {

    py::class_<Wave>(m, "Wave")
        .def(py::init<>())
        .def_readwrite("time", &Wave::time)
        .def_readwrite("wave", &Wave::wave)
        .def_readwrite("sample_rate", &Wave::sample_rate);

    m.def("read_audio", &read_audio, "Reads audio into waveform");
    m.def("generate_wave", &generate_wave, "Generate chosen wave");
    m.def("lowpass_filter", &lowpass_filter, "Cuts off all frequencies above given cut-off frequency");
    m.def("plot_line", &plot_line, "A function that plots a line using Matplot++",
        py::arg("x"), py::arg("y"));
    m.def("gaussian_filter", &gaussian_filter, "Apply a Gaussian filter to an image",
        py::arg("input"), py::arg("sigma"));
    m.def("grayscale", &grayscale, "Convert RGB image to Grayscale");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
