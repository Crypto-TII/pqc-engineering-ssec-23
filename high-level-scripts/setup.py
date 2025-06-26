import sys
import argparse

# SageMath imports
from sage.all import (
    log,
    ZZ,
    GF,
    PolynomialRing,
    previous_prime,
)


############################################
def arguments(args=sys.argv[1:]):
    parser = argparse.ArgumentParser(description="Parses command.")
    parser.add_argument('-b', '--bits', type=int, help='Bitlength of the field characteristic', required=True)
    parser.add_argument('-v', '--verbose', action='store_true', help='verbose help')

    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    options = parser.parse_args(args)
    return options


############################################
def ProgressBar(index, total, label):
    n_bar = 50  # Progress bar width
    progress = index / total
    sys.stdout.write('\r')
    sys.stdout.write(f"[{'=' * int(n_bar * progress):{n_bar}s}] {int(100 * progress)}%  {label}")
    sys.stdout.flush()


############################################
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


############################################
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


############################################
def sqrt_fp(a, check=True):
    # Square root over Fp
    p = a.parent().characteristic()
    assert(p % 4 == 3)
    if check:
        if not is_quadratic_residue(a):
            return None
    return l2r(a, (p + 1) // 4)


############################################
def sqrt_fp2(a, root2_squared, check=True):
    # Square root over Fq with q = p²: Kong et al. algorithm. We assume p² = 9 mod 16 and p = 3 mod 4
    p = a.parent().characteristic()
    q = p**2
    assert(q % 16 == 9)
    if check:
        if not is_quadratic_residue(a):
            return None

    exponent1 = (q - 9) // 16
    exponent2 = (q - 9) // 8
    b = l2r(2 * a, exponent1)
    c = 2 * a * b**2
    r = c**2

    s0 = a * b * (c - 1)
    constant3 = l2r(root2_squared, exponent2)
    u = b * constant3
    c = 2 * u**2 * root2_squared**2 * a
    s1 = u * root2_squared * a * (c - 1)
    return (ZZ(1 - r) >> 1) * s0 + (ZZ(1 + r) >> 1) * s1


############################################
def setup_fp(bits):
    p = previous_prime(2**bits)
    while (p % 4 != 3) or (p**2 % 16 != 9) or( (p**2 - 1) % 9 == 0) or (p % 3 == 1):
        p = previous_prime(p)

    assert(p % 4 == 3)      # p = 3 mod 4
    assert(p**2 % 16 == 9)  # p^2 = 9 mod 16
    assert((p**2 - 1) % 9)  # 9 does NOT divide (p^2 - 1)

    print('\n+++++++ Setup')
    print(f'p: {hex(p)}')
    print(f'p % 4 = {p % 4}')
    print(f'p % 3 = {p % 3}\n')
    f = {2: '(p + 1)', 1: '(p - 1)'}[p % 3]
    g = {1: '(p + 1)', 2: '(p - 1)'}[p % 3]
    print(f'In particular, 3 divides {f} but not {g}\n')

    # Prime Field
    Fq = GF(p)
    def sqrt_fq(input, check=True):
        return sqrt_fp(input, check=check)

    r3 = (2*p - 1) // 3
    def curt_fq(a):
        return l2r(a, r3)

    setup = {
        'Fp': Fq,
        'sqrt': sqrt_fq,
        'curt': curt_fq,
    }

    return setup


############################################
def setup_fq(bits):
    p = previous_prime(2**bits)
    while (p % 4 != 3) or (p**2 % 16 != 9) or( (p**2 - 1) % 9 == 0):
        p = previous_prime(p)
    
    assert(p % 4 == 3)      # p = 3 mod 4
    assert(p**2 % 16 == 9)  # p^2 = 9 mod 16
    assert((p**2 - 1) % 9)  # 9 does NOT divide (p^2 - 1)
    
    print('\n+++++++ Setup')
    print(f'p: {hex(p)}')
    print(f'p % 4 = {p % 4}')
    print(f'p % 3 = {p % 3}\n')
    f = {2: '(p + 1)', 1: '(p - 1)'}[p % 3]
    g = {1: '(p + 1)', 2: '(p - 1)'}[p % 3]
    print(f'In particular, 3 divides {f} but not {g}\n')
    
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

    def sqrt_fq(input, check=True):
        return sqrt_fp2(input, rootq2_squared, check=check)
    
    # Concerning 3rd roots of unity
    N3 = (p**2 - 1) // 3
    r3 = ZZ(3).inverse_mod(N3)  # assume three does not divide p - 1, but p + 1 does
    def curt_fq(a):
        return l2r(a, r3)

    zeta3 = (-1 + sqrt_fq(Fq(-3))) / Fq(2)
    assert(zeta3**3 == 1)
    
    setup = {
    'Fq': Fq,
    'Rx':Rx,
    'ζ₃':zeta3,
    'sqrt': sqrt_fq,
    'curt': curt_fq,
    }
    
    return setup
