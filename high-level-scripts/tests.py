import sys
import time
import functools
from numpy import base_repr

# SageMath imports
from sage.all import (
    log,
    randrange,
    GF,
    QQ,
    EllipticCurve,
    set_verbose,
)

set_verbose(-1)

from setup import arguments, setup_fp, setup_fq, ProgressBar
from random_walks2 import cgl_hash2, to_mont
from random_walks3 import cgl_hash3, get_order3_points, to_model, isogeny3
from basefield_chain3 import radical_walk3

runs = 25
if __name__ == '__main__':
    # main()
    setup = setup_fq(arguments(sys.argv[1:]).bits)
    Fq = setup['Fq']
    p = Fq.characteristic()
    A = Fq(6)
    
    setup['j₋₁'] = EllipticCurve([0, A + 6, 0, 4 * (2 + A), 0]).j_invariant()
    setup['j₀'] = EllipticCurve([0, A, 0, 1, 0]).j_invariant()
    
    print('\033[93m#################################################### CGLHash2 with starting curve E₆ : y² = x³ + 6x² + x\033[00m\033[00m')
    elapsed_time = 0
    for tries in range(0, runs, 1):
        ProgressBar(tries + 1, runs, f"Testing CGLHash2")
        hash_input = hex(randrange(0, p))[2:].encode()
        start_time = time.perf_counter()
        j = cgl_hash2(hash_input, setup, verbose=False)
        end_time = time.perf_counter()
        elapsed_time += (end_time - start_time)
    
    print(f'\nAverage time (CGLHash2): {elapsed_time / runs}\n')
        
    print('\033[93m#################################################### CGLHash2 with random starting curve E : y² = x³ + Ax² + x\033[00m\033[00m')
    elapsed_time = 0
    for tries in range(0, runs, 1):
        # Below three lines updates to use different curve at run
        A = to_mont(j, setup)
        setup['j₋₁'] = EllipticCurve([0, A + 6, 0, 4 * (2 + A), 0]).j_invariant()
        setup['j₀'] = EllipticCurve([0, A, 0, 1, 0]).j_invariant()
        
        ProgressBar(tries + 1, runs, f"Testing CGLHash2")
        hash_input = hex(randrange(0, p))[2:].encode()
        start_time = time.perf_counter()
        j = cgl_hash2(hash_input, setup, verbose=False)
        end_time = time.perf_counter()
        elapsed_time += (end_time - start_time)

    print(f'\nAverage time (CGLHash2): {elapsed_time / runs}\n')
    
    
    print('\033[93m#################################################### CGLHash3 with starting curve E₆ : y² = x³ + 6x² + x\033[00m\033[00m')
    
    A = Fq(6)
    def test_3div_poly(a1, a3):
        j = EllipticCurve(Fq, [a1, 0, a3, 0, 0]).j_invariant()
        F_ = EllipticCurve(Fq, [a1, 0, a3, -5 * a1 * a3, -a1**3 * a3 - 7 * a3**2])
        j_ = F_.j_invariant()
        assert(j != j_)
        
        alpha = setup['curt'](-a3)
        assert alpha**3 == -a3
        a1_, a3_ = isogeny3(alpha, a1, a3)
        j0 = EllipticCurve(Fq, [a1_, 0, a3_, -5 * a1_ * a3_, -a1_**3 * a3_ - 7 * a3_**2]).j_invariant()
        
        alpha *= setup['ζ₃']
        assert alpha**3 == -a3
        a1_, a3_ = isogeny3(alpha, a1, a3)
        j1 = EllipticCurve(Fq, [a1_, 0, a3_, -5 * a1_ * a3_, -a1_**3 * a3_ - 7 * a3_**2]).j_invariant()
        
        alpha *= setup['ζ₃']
        assert alpha**3 == -a3
        a1_, a3_ = isogeny3(alpha, a1, a3)
        j2 = EllipticCurve(Fq, [a1_, 0, a3_, -5 * a1_ * a3_, -a1_**3 * a3_ - 7 * a3_**2]).j_invariant()
        
        # Checking we have three non-isomorphic 3-isogenous curves
        assert(j0 != j1)
        assert(j0 != j2)
        assert(j1 != j2)
        # Checking non-backtracking
        assert(j0 != j_)
        assert(j1 != j_)
        assert(j2 != j_)
        # Checking non-backtracking
        assert(j0 != j)
        assert(j1 != j)
        assert(j2 != j)
        
    xs = get_order3_points(A, setup)    
    for xk in xs:
        yk = setup['sqrt'](xk**3 + A * xk**2 + xk)
        a1, a3 = to_model(A, xk)
        test_3div_poly(a1, a3)
    
    elapsed_time = 0
    setup['A'] = A
    for tries in range(0, runs, 1):
        ProgressBar(tries + 1, runs, f"Testing CGLHash3")
        hash_input = hex(randrange(0, p))[2:].encode()
        start_time = time.perf_counter()
        j = cgl_hash3(hash_input, setup, verbose=False)
        end_time = time.perf_counter()
        elapsed_time += (end_time - start_time)
    
    print(f'\nAverage time (CGLHash3): {elapsed_time / runs}\n')
    
    print('\033[93m#################################################### CGLHash3 with random starting curve E : y² = x³ + Ax² + x\033[00m\033[00m')
    elapsed_time = 0
    for tries in range(0, runs, 1):
        # Below three lines updates to use different curve at run
        A = to_mont(j, setup)
        setup['A'] = A
        
        # Test
        xs = get_order3_points(A, setup)
        for xk in xs:
            yk = setup['sqrt'](xk**3 + A * xk**2 + xk)
            a1, a3 = to_model(A, xk)
            test_3div_poly(a1, a3)
        
        ProgressBar(tries + 1, runs, f"Testing CGLHash3")
        hash_input = hex(randrange(0, p))[2:].encode()
        start_time = time.perf_counter()
        j = cgl_hash3(hash_input, setup, verbose=False)
        end_time = time.perf_counter()
        elapsed_time += (end_time - start_time)
    
    print(f'\nAverage time (CGLHash3): {elapsed_time / runs}\n')


    setup = setup_fp(arguments(sys.argv[1:]).bits)
    Fp = setup['Fp']
    p = Fp.characteristic()
    A = Fp(6)
    print('\033[93m#################################################### Radical 3-isogeny chain with starting curve E₆ : y² = x³ + 6x² + x\033[00m\033[00m')
    for value in range(0, 128, 1):
        ProgressBar(value + 1, 128, f"Testing (+) Radical 3-isogeny over GF(p)")
        A = radical_walk3(A, value, setup)
    print('')
    for value in range(0, 128, 1):
        ProgressBar(value + 1, 128, f"Testing (-) Radical 3-isogeny over GF(p)")
        A = radical_walk3(A, -value, setup)
    print('')