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

const double PI = 3.14;

class Wave {
    public:
        std::vector<double> time;
        std::vector<double> wave;
        int sample_rate;
};

Wave read_audio(std::string filepath, int accuracy) {

    Wave audio;

    if (accuracy < 1 || accuracy > 10) {
        std::cout << "Incorrect accuracy!";
        return audio;
    }

    AudioFile<double> a;
    a.load(filepath);

    std::vector<double> audio_data = a.samples[0];
    audio.sample_rate = a.getSampleRate();

    for (int i = 0; i < audio_data.size(); i += (accuracy * 10)) {
        audio.time.push_back(static_cast<double>(i) / audio.sample_rate);
    }

    for (int i = 0; i < audio.time.size(); i++) {
        audio.wave.push_back(audio_data[static_cast<int>(audio.time[i] * audio.sample_rate)]);
    }
    
    std::cout << "Audio file: " << filepath << " readed succesfully!" << std::endl;
    return audio;
}

Wave lowpass_filter(Wave audio) {

    double nyquist_freq = audio.sample_rate / 2;
    double cutoff_freq = 5000;
    double normalized_cutoff_freq = cutoff_freq / nyquist_freq;

    int order = 4;
    std::vector<double> b(order + 1), a(order + 1);
    double wn = tan(PI * normalized_cutoff_freq);
    double k = 1.0 / (1.0 + sqrt(2.0) * wn + pow(wn, 2));
    a[0] = pow(wn, 2) * k;
    a[1] = 2 * a[0];
    a[2] = a[0];
    b[0] = k;
    b[2] = -k;
    
    audio.wave[0] = b[0] * audio.wave[0];
    audio.wave[1] = b[0] * audio.wave[1] + b[1] * audio.wave[0] - a[1] * audio.wave[0];
    for (size_t n = 2; n < audio.wave.size(); ++n) {
        audio.wave[n] = b[0] * audio.wave[n] + b[1] * audio.wave[n - 1] + b[2] * audio.wave[n - 2]
            - a[1] * audio.wave[n - 1] - a[2] * audio.wave[n - 2];
    }

    return audio;
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
    matplot::plot(x, y);
    matplot::show();
}

std::vector<std::vector<float>> create_gaussian_kernel(float sigma) {
    float sum = 0.0;
    std::vector<std::vector<float>> kernel(5, std::vector<float>(5));

    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            float value = std::exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            kernel[i + 2][j + 2] = value;
            sum += value;
        }
    }

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}
 
py::array_t<uint8_t> gaussian_filter(py::array_t<uint8_t> input, float sigma) {

    auto buf = input.request();
    if (buf.ndim != 3) {
        throw std::runtime_error("Input should be a 3D NumPy array");
    }

    int height = buf.shape[0];
    int width = buf.shape[1];
    int channels = buf.shape[2];

    auto kernel = create_gaussian_kernel(sigma);

    py::array_t<uint8_t> output({ height, width, channels });
    auto output_buf = output.request();

    uint8_t* input_ptr = static_cast<uint8_t*>(buf.ptr);
    uint8_t* output_ptr = static_cast<uint8_t*>(output_buf.ptr);

    for (int h = 0; h < height; ++h) {
        for (int w = 0; w < width; ++w) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0;
                for (int i = -2; i <= 2; ++i) {
                    for (int j = -2; j <= 2; ++j) {
                        int x = std::min(std::max(w + j, 0), width - 1);
                        int y = std::min(std::max(h + i, 0), height - 1);
                        sum += input_ptr[(y * width + x) * channels + c] * kernel[i + 2][j + 2];
                    }
                }
                output_ptr[(h * width + w) * channels + c] = std::min(std::max(static_cast<int>(sum), 0), 255);
            }
        }
    }

    return output;
}

py::array_t<uint8_t> grayscale(py::array_t<uint8_t> input) {
 
    auto buf = input.request();
    if (buf.ndim != 3 || buf.shape[2] != 3) {
        throw std::runtime_error("Input should have shape (height, width, 3)");
    }

    size_t height = buf.shape[0];
    size_t width = buf.shape[1];

    auto result = py::array_t<uint8_t>({ height, width });
    auto result_buf = result.request();

    uint8_t* input_ptr = static_cast<uint8_t*>(buf.ptr);
    uint8_t* result_ptr = static_cast<uint8_t*>(result_buf.ptr);

    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {

            size_t index = i * width * 3 + j * 3;

            uint8_t r = input_ptr[index];
            uint8_t g = input_ptr[index + 1];
            uint8_t b = input_ptr[index + 2];

            uint8_t gray = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);

            result_ptr[i * width + j] = gray;
        }
    }

    return result;
}

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {

    py::class_<Wave>(m, "Wave")
        .def(py::init<>())
        .def_readwrite("time", &Wave::time)
        .def_readwrite("wave", &Wave::wave)
        .def_readwrite("sample_rate", &Wave::sample_rate);

    m.def("read_audio", &read_audio, "Reads audio into waveform");
    m.def("generate_wave", &generate_wave, "Generate chosen wave");
    m.def("lowpass_filter", &lowpass_filter, "Cut-offs all frequencies above 5kHz");
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
