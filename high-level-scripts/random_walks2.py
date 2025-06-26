import hashlib

# SageMath imports
from sage.all import (
    EllipticCurve,
)


############################################
def to_mont(j, setup):
    Fq = setup['Fq']
    Rx = setup['Rx']
    x = Rx.gens()[0]
    p = Fq.characteristic()

    choices = (j*(x**2 - 4) - 256*((x**2 - 3)**3)).roots()
    good = choices != []

    for (r,_) in choices:
        A = r
        Ea = EllipticCurve(Fq, [0, A, 0, 1, 0])
        good = good and (Ea.random_point() * (p + 1) == Ea(0))
    
    assert(good)
    return A


############################################
def random_walk2(j0_prev, j0, path, setup):
    Fq = setup['Fq']
    Rx = setup['Rx']
    one_half = 1 / Fq(2)

    jk = j0
    jk_prev = j0_prev
    for k in range(0, len(path), 1):
        Dk = jk**4 - 2976*(jk**3) + 2*(jk**2)*jk_prev + 2532192*(jk**2) - 2976*jk*jk_prev - 645205500*jk - 3*(jk_prev**2) + 324000*jk_prev - 8748000000
        Dk_sqrt = setup['sqrt'](Dk, check=False)
        Dk_sqrts = [Dk_sqrt, -Dk_sqrt]
        jk_next = (jk**2 - 1488*jk - jk_prev + 162000 + Dk_sqrts[path[k]]) * one_half
        jk_prev = jk
        jk = jk_next

    return jk


############################################ Charles-Goren-Lauter (CGL) Hash function concerning 2-isogeny walks
def cgl_hash2(message, setup, verbose=True):
    p = setup['Fq'].characteristic()

    # SHAKE_256 algorithm from the hashlib module.
    s = hashlib.shake_256()
    s.update(message)
    digest = s.hexdigest(p.bit_length() // 8)
    path = bin(int(digest, 16))[2:p.bit_length() + 2]

    if verbose:
        print('\nCGL Hash function over the 2-isogeny graph (CGLHash2)')
        print(f'Symmetric primitive:\t{s.name}')
        print(f'Message digest size:\t{len(digest) // 2} bytes')
        print(f'Random path of size:\t{len(path)} steps')

    jE = random_walk2(setup['j₋₁'], setup['j₀'], [ int(b) for b in path ], setup)
    return jE