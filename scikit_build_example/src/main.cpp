#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <matplot/matplot.h>
#include <AudioFile.h>
#include <iostream>
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
    int sample_rate = a.getSampleRate();

    for (int i = 0; i < audio_data.size(); i += (accuracy * 10)) {
        audio.time.push_back(static_cast<double>(i) / sample_rate);
    }

    for (int i = 0; i < audio.time.size(); i++) {
        audio.wave.push_back(audio_data[static_cast<int>(audio.time[i] * sample_rate)]);
    }
    
    std::cout << "Audio file: " << filepath << " readed succesfully!" << std::endl;
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

std::vector<std::vector<std::vector<float>>> convolve(
    const std::vector<std::vector<std::vector<float>>>& input,
    const std::vector<std::vector<std::vector<std::vector<float>>>>& kernel,
    int H, int W, int C, int h, int w, int OC) {

    int output_height = H - h + 1;
    int output_width = W - w + 1;

    std::vector<std::vector<std::vector<float>>> output(output_height, std::vector<std::vector<float>>(output_width, std::vector<float>(OC, 0)));

    for (int oh = 0; oh < output_height; ++oh) {
        for (int ow = 0; ow < output_width; ++ow) {
            for (int oc = 0; oc < OC; ++oc) {
                float sum = 0.0f;
                for (int kh = 0; kh < h; ++kh) {
                    for (int kw = 0; kw < w; ++kw) {
                        for (int c = 0; c < C; ++c) {
                            sum += input[oh + kh][ow + kw][c] * kernel[kh][kw][c][oc];
                        }
                    }
                }
                output[oh][ow][oc] = sum;
            }
        }
    }

    return output;
}

py::array_t<float> convolve_wrapper(py::array_t<float> input, py::array_t<float> kernel) {
    py::buffer_info input_info = input.request();
    py::buffer_info kernel_info = kernel.request();

    if (input_info.ndim != 3 || kernel_info.ndim != 4) {
        throw std::runtime_error("Input should be 3D and kernel should be 4D.");
    }

    int H = input_info.shape[0];
    int W = input_info.shape[1];
    int C = input_info.shape[2];
    int h = kernel_info.shape[0];
    int w = kernel_info.shape[1];
    int kernel_C = kernel_info.shape[2];
    int OC = kernel_info.shape[3];

    if (C != kernel_C) {
        throw std::runtime_error("Number of input channels must match.");
    }

    auto input_ptr = static_cast<float*>(input_info.ptr);
    auto kernel_ptr = static_cast<float*>(kernel_info.ptr);

    std::vector<std::vector<std::vector<float>>> input_vec(H, std::vector<std::vector<float>>(W, std::vector<float>(C)));
    std::vector<std::vector<std::vector<std::vector<float>>>> kernel_vec(h, std::vector<std::vector<std::vector<float>>>(w, std::vector<std::vector<float>>(C, std::vector<float>(OC))));

    // Convert input numpy array to vector
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            for (int k = 0; k < C; ++k) {
                input_vec[i][j][k] = input_ptr[i * W * C + j * C + k];
            }
        }
    }

    // Convert kernel numpy array to vector
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            for (int k = 0; k < C; ++k) {
                for (int l = 0; l < OC; ++l) {
                    kernel_vec[i][j][k][l] = kernel_ptr[i * w * C * OC + j * C * OC + k * OC + l];
                }
            }
        }
    }

    // Perform convolution
    std::vector<std::vector<std::vector<float>>> output_vec = convolve(input_vec, kernel_vec, H, W, C, h, w, OC);

    int output_height = H - h + 1;
    int output_width = W - w + 1;

    // Convert output vector to numpy array
    py::array_t<float> output = py::array_t<float>({ output_height, output_width, OC });
    py::buffer_info output_info = output.request();
    auto output_ptr = static_cast<float*>(output_info.ptr);

    for (int i = 0; i < output_height; ++i) {
        for (int j = 0; j < output_width; ++j) {
            for (int k = 0; k < OC; ++k) {
                output_ptr[i * output_width * OC + j * OC + k] = output_vec[i][j][k];
            }
        }
    }

    return output;
}

// Function to create a Gaussian kernel
std::vector<std::vector<float>> create_gaussian_kernel(int kernel_size, float sigma) {
    int half_size = kernel_size / 2;
    float sum = 0.0;
    std::vector<std::vector<float>> kernel(kernel_size, std::vector<float>(kernel_size));

    for (int i = -half_size; i <= half_size; ++i) {
        for (int j = -half_size; j <= half_size; ++j) {
            float value = std::exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            kernel[i + half_size][j + half_size] = value;
            sum += value;
        }
    }

    // Normalize the kernel
    for (int i = 0; i < kernel_size; ++i) {
        for (int j = 0; j < kernel_size; ++j) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

// Function to apply the Gaussian filter to an image
py::array_t<uint8_t> gaussian_filter(py::array_t<uint8_t> input, int kernel_size, float sigma) {
    // Get input buffer info
    auto buf = input.request();
    if (buf.ndim != 3) {
        throw std::runtime_error("Input should be a 3D NumPy array");
    }

    int height = buf.shape[0];
    int width = buf.shape[1];
    int channels = buf.shape[2];

    // Create the Gaussian kernel
    auto kernel = create_gaussian_kernel(kernel_size, sigma);
    int half_size = kernel_size / 2;

    // Prepare the output array
    py::array_t<uint8_t> output({ height, width, channels });
    auto output_buf = output.request();

    uint8_t* input_ptr = static_cast<uint8_t*>(buf.ptr);
    uint8_t* output_ptr = static_cast<uint8_t*>(output_buf.ptr);

    // Apply the Gaussian filter
    for (int h = 0; h < height; ++h) {
        for (int w = 0; w < width; ++w) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0;
                for (int i = -half_size; i <= half_size; ++i) {
                    for (int j = -half_size; j <= half_size; ++j) {
                        int x = std::min(std::max(w + j, 0), width - 1);
                        int y = std::min(std::max(h + i, 0), height - 1);
                        sum += input_ptr[(y * width + x) * channels + c] * kernel[i + half_size][j + half_size];
                    }
                }
                output_ptr[(h * width + w) * channels + c] = std::min(std::max(static_cast<int>(sum), 0), 255);
            }
        }
    }

    return output;
}

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {

    py::class_<Wave>(m, "Wave")
        .def(py::init<>())
        .def_readwrite("time", &Wave::time)
        .def_readwrite("wave", &Wave::wave);

    m.def("read_audio", &read_audio, "Reads audio into waveform");
    m.def("generate_wave", &generate_wave, "Generate chosen wave");
    m.def("plot_line", &plot_line, "A function that plots a line using Matplot++",
        py::arg("x"), py::arg("y"));
    m.def("convolve", &convolve_wrapper, "Apply 2D convolution to an image");
    m.def("gaussian_filter", &gaussian_filter, "Apply a Gaussian filter to an image",
        py::arg("input"), py::arg("kernel_size"), py::arg("sigma"));

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
