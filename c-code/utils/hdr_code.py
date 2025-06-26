#!/usr/bin/env python3
import argparse
import sys
from sage.all import ceil, log, GF, ZZ, PolynomialRing

# sage -python hdr_code.py -a 246 -c 201 -l 128 > ../src/parameters/p254.h

# sage -python hdr_code.py -a 372 -c 437 -l 128 > ../src/parameters/p381.h
# sage -python hdr_code.py -a 567 -c 139 -l 192 > ../src/parameters/p575.h
# sage -python hdr_code.py -a 756 -c 257 -l 256 > ../src/parameters/p765.h

# sage -python hdr_code.py -a 390 -c 165 -l 128 > ../src/parameters/p398.h
# sage -python hdr_code.py -a 582 -c 921 -l 192 > ../src/parameters/p592.h
# sage -python hdr_code.py -a 774 -c 411 -l 256 > ../src/parameters/p783.h

# sage -python hdr_code.py -a 2 -c 10980266050796090380028661398194450349184449178169757955514086995044995524265 -l 128 > ../src/parameters/p255.h
# sage -python hdr_code.py -a 2 -c 4265069153285891173087371089085058777333948346280563290592617735285026033366853728148093776366671044752906631128723 -l 192 > ../src/parameters/p383.h
# sage -python hdr_code.py -a 2 -c 1331684699081905773686966904488651388517342873708180584403111660513502390006644134406723028256595313406156735410987361198165720310405343322235720072016415 -l 256 > ../src/parameters/p511.h

# +++++++++++++++++++++++++++++++
def arguments(args=sys.argv[1:]):
    parser = argparse.ArgumentParser(description="Parses command.")
    parser.add_argument('-a', '--exponent_of_two', type=int, help='exponent of two', required=True)
    parser.add_argument('-c', '--cofactor', type=int, help='cofactor', required=True)
    parser.add_argument('-l', '--security_level', type=int, help='security level', required=True)

    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    options = parser.parse_args(args)
    return options


# ++++++++++++++++++++++++++++++++++
def word_extractor(p: int, p_words: int, length: int, var: str):
    k = 64
    p_word = []
    content_str = "static const uint64_t " + var + f"[{length}] = {{\n"
    pp = p
    for i in range(1, p_words + 1):
        if ((i - 1) % 4) == 0:
            content_str = content_str + "\t"
        p_word.append(pp % 2 ** k)
        if (i % 4) != 0 and (i != p_words):
            content_str = content_str + "0x{:X}".format(p_word[i - 1]) + ", "
        else:
            if (i != p_words):
                content_str = content_str + "0x{:X}".format(p_word[i - 1]) + ",\n"
            else:
                content_str = content_str + "0x{:X}".format(p_word[i - 1]) + "\n"
        pp = pp // (2 ** k)

    content_str = content_str + "};\n"
    print(f'{content_str}')


# ++++++++++++++++++++++++++++++++++
def word_extractor_twice(p: list, p_words: int, length: int, var: str):
    k = 64
    p_word = []
    content_str = "static const uint64_t " + var + f"[2 * {length}] = {{\n"
    pp = ZZ(p[0])
    for i in range(1, p_words + 1):
        if ((i - 1) % 4) == 0:
            content_str = content_str + "\t"
        p_word.append(pp % 2 ** k)
        if (i % 4) != 0 and (i != p_words):
            content_str = content_str + "0x{:X}".format(p_word[i - 1]) + ", "
        else:
            content_str = content_str + "0x{:X}".format(p_word[i - 1]) + ",\n"
        pp = pp // (2 ** k)
    p_word = []
    pp = ZZ(p[1])
    for i in range(1, p_words + 1):
        if ((i - 1) % 4) == 0:
            content_str = content_str + "\t"
        p_word.append(pp % 2 ** k)
        if (i % 4) != 0 and (i != p_words):
            content_str = content_str + "0x{:X}".format(p_word[i - 1]) + ", "
        else:
            if (i != p_words):
                content_str = content_str + "0x{:X}".format(p_word[i - 1]) + ",\n"
            else:
                content_str = content_str + "0x{:X}".format(p_word[i - 1]) + "\n"
        pp = pp // (2 ** k)

    content_str = content_str + "};\n"
    print(f'{content_str}')


def cube_root(p):
    pbits = p.bit_length()
    pwords = (pbits + 63) // 64
    montgomery_one = 2 ** (64 * pwords) % p
    
    p_minus_one_halves = (p - 1) // 2
    p_minus_3_quarters = (p - 3) // 4
    
    # Fp
    fp = GF(p)
    Rx = PolynomialRing(fp, name="x")
    x = Rx.gens()[0]
    fp2 = fp.extension(x**2 + 1, 'i')
    i = fp2.gen()
    assert(i**2 + 1 == 0)

    def fp2_issquare(a):
        a1 = a ** p_minus_3_quarters
        alpha = a1 ** 2
        alpha = alpha * a

        alpha_conjugated = alpha.conjugate()

        a0 = alpha * alpha_conjugated
        if a0 == fp2(-1):
            # a doesn't have sqrt in fp2
            return False, None

        x0 = a1 * a
        if alpha == fp2(-1):
            return True, x0 * i

        else:

            alpha = alpha + 1
            b = alpha ** p_minus_one_halves
            b = b * x0
            return True, b
    
    N3 = (p**2 - 1) // 3
    r3 = ZZ(3).inverse_mod(N3)  # assume three does not divide p - 1, but p + 1 does
    assert(r3 * 3 % N3 == 1)
    
    correct, square_root_of_three = fp2_issquare(fp2(-3))
    assert(correct)
    assert(square_root_of_three**2 == -3)
    zeta3 = (-1 + square_root_of_three) / fp2(2)
    assert(zeta3**3 == 1)
    return r3, (zeta3 * montgomery_one).list()
    
    
def print_parameters(a: int, c: int, l: int):
    # assert(c % 3 != 0)
    p = (2**a) * c - 1
    assert p % 4 == 3
    
    # // size of p in bits, bytes ,words
    pbits = p.bit_length()
    pbytes = (pbits + 7) // 8
    pwords = (pbits + 63) // 64
    pmask = pbits % 64
    if pmask == 0:
        pmask = hex((1 << 64) - 1);
    else:
        pmask = hex((1 << pmask) - 1)

    print(f'// Parameters concerning P{pbits}\n')
    print(f'#ifndef SSEC_PARAMETERS_P{pbits}_H')
    print(f'#define SSEC_PARAMETERS_P{pbits}_H\n')

    print(f'#define FIELD_NAME\t\t\t"p{pbits}"')
    print(f'#define FIELD_BITS\t\t\t{pbits}')
    print(f'#define FIELD_BYTES\t\t\t{pbytes}')
    print(f'#define FIELD_64BITS_WORDS\t{pwords}')
    print(f'#define QFIELD_BYTES\t\t{2 * pbytes}')
    print(f'#define MASK_FIELD_ELEMENT\t{pmask}')
    print(f'#define SECURITY_BITS\t\t{l}\n')
    
    trits = int(ceil((2.0 * l) / log(3.0, 2)))
    print(f'#define BIT_LENGTH_PATH\t\t{2 * l}')
    print(f'#define TRITLENGTH_PATH\t\t{trits}\n')

    print("// Field characteristic p")
    word_extractor(p, pwords, "FIELD_64BITS_WORDS", "FIELD_CHARACTERISTIC")

    montgomery_one = 2 ** (64 * pwords) % p
    print("// Neutral multiplicative in Montgomery domain: R = 2ᵉ mod p")
    word_extractor(montgomery_one, pwords, "FIELD_64BITS_WORDS", "MONTGOMERY_CONSTANT_ONE")
    print("// Montgomery constant R² = (2ᵉ)² mod p where e = 0 mod 64 s.t. 2ᵉ⁻⁶⁴ < p < 2ᵉ")
    word_extractor((montgomery_one ** 2) % p, pwords, "FIELD_64BITS_WORDS", "MONTGOMERY_CONSTANT_R_SQUARED")
    
    print("// Exponent constant required for field inversion: p - 2")
    word_extractor(p - 2, pwords, "FIELD_64BITS_WORDS", "FIELD_INVERSION_EXPONENT")
    
    print("// Exponent constant required for computing square-roots in GF(p): (p - 1) / 2")
    word_extractor((p - 1) // 2, pwords, "FIELD_64BITS_WORDS", "SQUARE_ROOT_EXPONENT_12")
    q = p**2
    
    print("// Exponent constant required for computing square-roots in GF(p²): (p - 3) / 4")
    word_extractor((p - 3) // 4, pwords, "FIELD_64BITS_WORDS", "SQUARE_ROOT_EXPONENT_34")
    
    print("// Exponent constant required for computing square-roots in GF(p²): (p + 1) / 4")
    word_extractor((p + 1) // 4, pwords, "FIELD_64BITS_WORDS", "SQUARE_ROOT_EXPONENT_14")
    
    if (p % 3 == 2):
        print("#define SSEC_CUBE_ROOT_OVER_FP")
        print("// Exponent constant required for computing square-roots in GF(p): (2p - 1) / 3")
        word_extractor((2*p - 1) // 3, pwords, "FIELD_64BITS_WORDS", "CUBE_ROOT_EXPONENT_213")
        
    if (q % 16 == 9):
        def l2r(a, exponent):
            # Left-to-right algorithm: Exponentiation over finite fields
            temporal = 1 * exponent
            power = a * 1
            result = 1
            while temporal > 0:
                if (temporal % 2) == 1:
                    result = result * power;
                power = power**2
                # Dividing m by 2 in every iteration
                temporal = temporal // 2
            return result
        
        def is_quadratic_residue(a):
            p = a.parent().characteristic()
            q = a.parent().cardinality()
            m = int(log(q, p))
            b = 1 * a
            c = 1
            for index in range(0, m, 1):
                c *= b
                b = l2r(b, p)
            return not (l2r(c, (p - 1) // 2) - 1)
        
        # Prime Field
        Fp = GF(p)
        Rx = PolynomialRing(Fp, name="x")
        X = Rx.gens()[0]

        # Quadratic Field 
        Fq = Fp.extension(X**2 + 1, 'i')
        rootq = Fq.gen()
        assert(rootq**2 + 1 == 0)
        Rx = PolynomialRing(Fq, name="x")

        # Non quadratic residue
        rootq2_squared = rootq
        while is_quadratic_residue(-rootq2_squared):
            rootq2_squared += 1
    
        exponent1 = (q - 9) // 16
        exponent2 = (q - 9) // 8
        
        constant3 = l2r(rootq2_squared, exponent2)
        print("#define SSEC_KONG_ET_AL_S_ALGORITHM")
        print("// Constant required in Kong et al.'s algorithm, d: quadratic non-residue")
        word_extractor_twice((rootq2_squared * montgomery_one).list(), pwords, "FIELD_64BITS_WORDS", "SQUARE_ROOT_CONSTANT_D")
        print("// Constant required in Kong et al.'s algorithm, t: d raised at the power (p² - 9) / 8")
        word_extractor_twice((constant3 * montgomery_one).list(), pwords, "FIELD_64BITS_WORDS", "SQUARE_ROOT_CONSTANT_T")
        print("// Exponent constant required for computing square-roots in GF(p²): (p² - 9) / 16")
        e1words = (exponent1.bit_length() + 63) // 64
        print(f'#define SQUARE_ROOT_EXPONENT_BITS\t\t\t{exponent1.bit_length()}')
        word_extractor(exponent1, e1words, "2 * FIELD_64BITS_WORDS", "SQUARE_ROOT_EXPONENT_916")
        
    
    n3, zeta3 = cube_root(p)
    n3words = (n3.bit_length() + 63) // 64
    print("// Exponent constant required for computing cube-roots")
    print(f'#define CUBE_ROOT_EXPONENT_BITS\t\t\t{n3.bit_length()}')
    word_extractor(n3, n3words, "2 * FIELD_64BITS_WORDS", "CUBE_ROOT_EXPONENT")
    print("// Cube root of unity in Montgomery domain")
    word_extractor_twice(zeta3, pwords, "FIELD_64BITS_WORDS", "CUBE_ROOT_OF_UNITY")
    
    montgomery_one = 2 ** (64 * pwords) % p
    one_half = ZZ(2).inverse_mod(p)
    word_extractor((one_half * montgomery_one) % p, pwords, "FIELD_64BITS_WORDS", "ONE_HALF")
    one_third = ZZ(3).inverse_mod(p)
    word_extractor((one_third * montgomery_one) % p, pwords, "FIELD_64BITS_WORDS", "ONE_THIRD")
    one_ninth = (one_third**2) % p
    word_extractor((one_ninth * montgomery_one) % p, pwords, "FIELD_64BITS_WORDS", "ONE_NINTH")
    one_by_27 = (one_third * one_ninth) % p
    word_extractor((one_by_27 * montgomery_one) % p, pwords, "FIELD_64BITS_WORDS", "ONE_BY_27")

    print(f'#endif // SSEC_PARAMETERS_P{pbits}_H')


def main():
    a = arguments(sys.argv[1:]).exponent_of_two
    c = arguments(sys.argv[1:]).cofactor
    l = arguments(sys.argv[1:]).security_level
    print_parameters(a, c, l)


# ++++++++++++++++++++++++
if __name__ == "__main__":
    main()
