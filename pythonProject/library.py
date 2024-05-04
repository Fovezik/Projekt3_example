import scikit_build_example as clib
import imageio as im
import scipy as sc


def show_audio(audio_file, accuracy):
    # audio_file -> string path to audio file with wav extension
    # accuracy -> given integer (from 1 to 10 -> lower = better)
    clib.show_audio(audio_file, accuracy)


def show_wave(type_of_wave, frequency):
    # type_of_wave -> string (sine/cosine/square/sawtooth)
    # frequency -> given double type positive number
    clib.generate_wave(type_of_wave, frequency)


show_audio("test-audio.wav", 5)
show_wave("sine", 0.1)
show_wave("cosine", 0.1)
show_wave("square", 0.1)
show_wave("sawtooth", 0.1)
