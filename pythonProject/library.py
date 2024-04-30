import scikit_build_example as s


def test_math_func(a, b):
    print()
    print("*****************  ", a, " + ", b, " = ", s.add(a, b))
    print("* Test Function *  ", a, " - ", b, " = ", s.subtract(a, b))
    print("*****************  ", b, " - ", a, " = ", s.subtract(b, a))
    print(), print()


def test_audio_func(filepath):
    s.audio(filepath)


test_math_func(1000, 1500)
test_audio_func("C:\\Users\\Lukasz\\Desktop\\Techniki_programowania\\Projekt3_example\\scikit_build_example\\AudioFile\\examples\\test-audio.wav")
