import scikit_build_example as s


def test(a, b, filepath):
    # a,b -> 2 given integers for calculations
    # filepath -> string path to audio file with wav extension
    print()
    print("X---------------X  ", a, " + ", b, " = ", s.add(a, b))
    print("| Test Function |  ", a, " - ", b, " = ", s.subtract(a, b))
    print("X---------------X  ", b, " - ", a, " = ", s.subtract(b, a))
    print(), print()
    s.audio_test(filepath)


path_to_audio_file = "C:\\Users\\Lukasz\\Desktop\\Techniki_programowania\\Projekt3_example\\test-audio.wav"

test(1000, 1500, path_to_audio_file)

# s.show_audio[path, accuracy(from 1 to 10)] (lower = better)
# s.show_audio(path_to_audio_file, 1)

# s.generate_sphere[radius(int), accuracy(int|from 1 to 10)] (lower = better)
# s.generate_sphere(1, 10)

# s.generate_wave(
s.generate_wave("sin", 0.1)
s.generate_wave("cos", 0.1)
