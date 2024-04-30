import scikit_build_example as s


def test(a, b):
    print()
    print("*****************  ", a, " + ", b, " = ", s.add(a, b))
    print("* Test Function *  ", a, " - ", b, " = ", s.subtract(a, b))
    print("*****************  ", b, " - ", a, " = ", s.subtract(b, a))
    print(), print()


test(1000, 1500)

s.audio("C:\\Users\\Lukasz\\Desktop\\Techniki_programowania\\Projekt3_example\\scikit_build_example\\AudioFile\\examples\\test-audio.wav")

