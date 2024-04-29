import scikit_build_example as s


def test(a, b):
    print(a, " + ", b, " = ", s.add(a, b))
    print(a, " - ", b, " = ", s.substract(a, b))
    print(b, " - ", a, " = ", s.substract(b, a))


test(1000, 1500)
