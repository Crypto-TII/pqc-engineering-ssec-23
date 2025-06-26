clear;
function j_invariant(a1, a3, a2, a4, a6)
    b2 := a1^2 + 4 * a2;
    b4 := a1 * a3 + 2 * a4;
    b6 := a3^2 + 4 * a6;
    b8 := a1^2 * a6 - a1 * a3 * a4 + a2 * a3^2 + 4 * a2 * a6 - (a4^2);
    Disc := -(b2^2) * b8 + 9 * b2 * b4 * b6 - 8 * b4^3 - 27 * b6^2;

    c4 := b2^2 - 24 * b4;
    c6 := -(b2^3) + 36 * b2 *b4 - 216 * b6;

    var1 := c4^3 / Disc;
    var2 := c4^3 / ((c4^3 - c6^2) / 1728);

    assert(var1 eq var2);
    return c4^3, (c4^3 - c6^2) / 1728;
end function;

Pt<t> := PolynomialRing(Rationals());
Ru<u> := ext<Rationals() | t^2 - 3>;
Px<x> := PolynomialRing(Ru);
Ri<i> := ext<Ru | x^2 + 1>;
Ra<a1, a3, alpha> := PolynomialRing(Ri, 3);

zeta3 := (-1 + i * u) / 2;
alpha_ :=  zeta3 * alpha;
_alpha :=  zeta3^2 * alpha;

num, den := j_invariant(a1, a3, 0, 0, 0);
fac_num := Factorization(num);
assert num div &*[fk[1]^fk[2] : fk in fac_num] eq 1;
fac_den := Factorization(den);
printf "j(G):\n(%o)\n-------------------------------------------------------\n\t\t(%o)\n", num,den;
assert den div &*[fk[1]^fk[2] : fk in fac_den] eq 1;
printf "\nits numerator factors as: %o\n", fac_num;
printf "its denominator factors as: %o\n\n", fac_den;


num, den := j_invariant(a1, a3, 0, -5 * a1 * a3, -a1^3 * a3 - 7 * a3^2);
fac_num := Factorization(num);
assert num div &*[fk[1]^fk[2] : fk in fac_num] eq 1;
fac_den := Factorization(den);
printf "j(E):\n(%o)\n-------------------------------------------------------------\n   (%o)\n", num,den;
assert den div &*[fk[1]^fk[2] : fk in fac_den] eq 1;
printf "\nits numerator factors as: %o\n", fac_num;
printf "its denominator factors as: %o\n\n", fac_den;


RA<A> := PolynomialRing(Ri);

num, den := j_invariant(0, 0, A, 1, 0);
fac_num := Factorization(num);
c_num := num div &*[fk[1]^fk[2] : fk in fac_num];
fac_den := Factorization(den);
printf "j(F₀):\n(%o)\n--------------------------------------------\n             (%o)\n", num,den;
c_den := den div &*[fk[1]^fk[2] : fk in fac_den];
printf "\nits numerator factors as: %o\n", fac_num;
printf "its denominator factors as: %o\n", fac_den;
printf "and constant factor: %o\n\n", c_num / c_den;

num, den := j_invariant(0, 0, A + 6, 4 * (2 + A), 0);
fac_num := Factorization(num);
c_num := num div &*[fk[1]^fk[2] : fk in fac_num];
fac_den := Factorization(den);
printf "j(F₋₁):\n(%o)\n-----------------------------------------------\n        (%o)\n", num,den;
c_den := den div &*[fk[1]^fk[2] : fk in fac_den];
printf "\nits numerator factors as: %o\n", fac_num;
printf "its denominator factors as: %o\n", fac_den;
printf "and constant factor: %o\n\n", c_num / c_den;

exit;