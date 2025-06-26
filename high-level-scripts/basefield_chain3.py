from copy import deepcopy
from sage.all import EllipticCurve

############################################
def get_order3_points_fp(A_, setup):
    [A, C] = deepcopy(A_)
    Fq = setup['Fp']
    # Constants
    one_half = 1 / Fq(2)
    one_third = 1 / Fq(3)
    one_ninth = one_third**2
    one_by_27 = one_third * one_ninth

    A_times_one_third = A * one_third             #            M𝒸
    A_squared = A**2                              #        S
    C_squared = C**2                              #        S
    t = (3 * C_squared - A_squared) * one_ninth   #            M𝒸 + 3A
    r = 16 * (A_squared * A) * one_by_27          #    M     + M𝒸 + 4A
    s = 8 * A_times_one_third                     #                 3A
    u = r - s * C_squared                         #    M          +  A

    aux = C_squared**2                            #        S
    tmp = A_squared * aux                         #    M
    aux = aux * C_squared                         #    M
    y = t + \
        one_third * \
        setup['curt'](-2 * tmp + 8 * aux)         #            M𝒸 + 3A + E

    r = 2 * y                                     #                  A
    s = 6 * t                                     #                 3A

    s0 = setup['sqrt'](r - s)                     #                  A + E
    # Below assert is for sanity check (not required)
    assert(s0**2 == (r - s))
    assert(not s0 is None)

    s0_squared = s0**2                            #       S
    v = -(r + s)                                  #                 2A
    s1 = setup['sqrt'](v * s0_squared + u * s0)   #   2M          +  A + E
    s2 = setup['sqrt'](v * s0_squared - u * s0)   #   2M          +  A + E

    z = []
    if s2 is None:
        assert(not s1 is None)
        assert(s1**2 == (v * s0**2 + u * s0))
        z.append((-s0_squared + s1) * one_half)   #            M𝒸 +  A
        z.append((-s0_squared - s1) * one_half)   #            M𝒸 + 2A
    else:
        assert(s1 is None)
        assert(s2**2 == (v * s0**2 - u * s0))
        z.append((s0_squared + s2) * one_half)    #            M𝒸 +  A
        z.append((s0_squared - s2) * one_half)    #            M𝒸 +  A

    num = [zk - s0*A_times_one_third for zk in z] #    M          + 4A
    den = s0 * C

    p = Fq.characteristic()

    # Below check takes                               7M + 2S
    x = num[0]
    y_squared = ((C * x**3) + (A * x**2 * den) + (C * den**2 * x)) * (den**3 * C)
    if y_squared**((p - 1) // 2) != Fq(1):       #                       E
        num = num[::-1]


    # Below assert is for sanity check
    inv = 1 / den
    x = num[0] * inv
    coeff = A * inv * s0
    y_squared = x**3 + coeff * x**2 + x
    assert(y_squared**((p - 1) // 2) == Fq(1))

    return num, den # Total cost: 15M + 6S + 8M𝒸 + 32A + 5E

############################################
def isogeny_3_mont(t, setup):
    # This function assumes the input and output determines an order-3
    # point on Montgomery curves (i.e., y²= x³ + Ax² + x)
    (r, s) = deepcopy(t)
    r_squared = r**2
    s_squared = s**2
    r_cubed = r_squared * r
    s_cubed = s_squared * s
    aux = r * s_squared

    alpha_cubed = r * (r_squared - s_squared)
    alpha = setup['curt'](alpha_cubed)
    assert alpha**3 == alpha_cubed
    rd = 3 * r * alpha**2
    rd += (3*r_squared - s_squared) * alpha
    rd += (3*r_cubed - 2*aux)
    return [rd, s_cubed]

############################################
def to_mont(t):
    (r, s) = deepcopy(t)
    r_squared = r**2
    r_at_four = r_squared**2
    s_squared = s**2
    s_at_four = s_squared**2

    num = -3 * r_at_four - 6 * r_squared * s_squared + s_at_four
    den = 4 * r_squared * r * s
    a = num/den
    return a

############################################
def radical_walk3(a, exponent, setup):
    # This function assumes the input and output determines Montgomery
    # curves (i.e., y²= x³ + Ax² + x)
    Fp = setup['Fp']
    p = Fp.characteristic()
    # scalar = Fp(127)
    scalar = Fp.random_element()
    while not scalar:
        scalar = Fp.random_element()
    num, den = get_order3_points_fp([scalar*a, scalar], setup)
    sign = lambda x: -1 if x < 0 else (1 if x > 0 else 0)
    choice = sign(exponent)
    t = [num[(choice + 1) // 2], den]
    for _ in range(0, choice * exponent, 1):
        t = isogeny_3_mont(t, setup)

    a_ = to_mont(t)
    assert(not (EllipticCurve(Fp, [0, a_, 0, 1, 0]).random_element() * (p + 1)))
    return a_
