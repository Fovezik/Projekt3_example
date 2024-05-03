import scikit_build_example as lib
import imageio as im
import scipy as sc


def show_audio(audio_file, accuracy):
    # audio_file -> string path to audio file with wav extension
    # accuracy -> given integer (from 1 to 10)
    lib.show_audio(audio_file, accuracy)


def generate_sphere(radius, accuracy):
    # radius -> given integer
    # accuracy -> given integer (from 1 to 10)
    lib.generate_sphere(radius, accuracy)


def generate_wave(type_of_wave, frequency):
    # type_of_wave -> string (sine/cosine/square/sawtooth)
    # frequency -> given double type positive number
    lib.generate_wave(type_of_wave, frequency)


path_to_audio_file = "test-audio.wav"

show_audio(path_to_audio_file, 5)
generate_sphere(1, 5)

generate_wave("sine", 3)
generate_wave("cosine", 3)
generate_wave("square", 3)
generate_wave("sawtooth", 3)
