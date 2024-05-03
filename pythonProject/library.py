import scikit_build_example as s


def test(a, b, audio_file):
    # a,b -> 2 given integers for calculations
    # filepath -> string path to audio file with wav extension
    print()
    print("X---------------X  ", a, " + ", b, " = ", s.add(a, b))
    print("| Test Function |  ", a, " - ", b, " = ", s.subtract(a, b))
    print("X---------------X  ", b, " - ", a, " = ", s.subtract(b, a))
    print(), print()
    s.audio_test(audio_file)


# D:  path_to_audio_file = "\\test-audio.wav"
# test(1000, 1500, path_to_audio_file)

# D:  s.show_audio[path, accuracy(from 1 to 10)] (lower = better)
# s.show_audio(path_to_audio_file, 5)

# D:  s.generate_sphere[radius(int), accuracy(int|from 1 to 10)] (lower = better)
# s.generate_sphere(1, 5)

# D:  s.generate_wave(string -> sine/cosine/square/sawtooth , frequency)
s.generate_wave("sine", 1.5)
s.generate_wave("cosine", 1.5)
s.generate_wave("square", 1.5)
s.generate_wave("sawtooth", 1.5)
