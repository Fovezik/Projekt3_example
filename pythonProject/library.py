import scikit_build_example as s


def test(a, b, audio_file):
    # a,b -> 2 given integers for calculations
    # audio_file -> string path to audio file with wav extension
    print()
    print("X---------------X  ", a, " + ", b, " = ", s.add(a, b))
    print("| Test Function |  ", a, " - ", b, " = ", s.subtract(a, b))
    print("X---------------X  ", b, " - ", a, " = ", s.subtract(b, a))
    print(), print()
    s.audio_test(audio_file)


def show_audio(audio_file, accuracy):
    # audio_file -> string path to audio file with wav extension
    # accuracy -> given integer (from 1 to 10)
    s.show_audio(audio_file, accuracy)


def generate_sphere(radius, accuracy):
    # radius -> given integer
    # accuracy -> given integer (from 1 to 10)
    s.generate_sphere(radius, accuracy)


def generate_wave(type_of_wave, frequency):
    # type_of_wave -> string (sine/cosine/square/sawtooth)
    # frequency -> given double type positive number
    s.generate_wave(type_of_wave, frequency)


path_to_audio_file = "test-audio.wav"
test(1000, 1500, path_to_audio_file)

show_audio(path_to_audio_file, 5)
generate_sphere(1, 5)

generate_wave("sine", 3)
generate_wave("cosine", 3)
generate_wave("square", 3)
generate_wave("sawtooth", 3)
