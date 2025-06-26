clear;
Pt<t> := PolynomialRing(Rationals());
Ri<i> := ext<Rationals() | t^2 + 1>; 
Ra<A> := PolynomialRing(Ri);
RxPyP<xP, yP> := PolynomialRing(Ra, 2);
Rxy<x, y> := PolynomialRing(RxPyP, 2);

print "\nBaseline curve model, F: y² = x³ + Ax² + x";

h := y^2 - x^3 - A*x^2 - x;
f := Evaluate(h, [x + xP, y + yP]);
a := MonomialCoefficient(f, x) / MonomialCoefficient(f, y);
g := Evaluate(f, [x, y - (Numerator(a) * x) / Denominator(a)]);

a1 := MonomialCoefficient(Numerator(g), x*y) / Denominator(g);
a3 := MonomialCoefficient(Numerator(g), y) / Denominator(g);

print "\n3-isogeny curve model, E: y² + a₁xy + a₃y = x³\nwhere";
printf "a1:\t %o\n", a1;
printf "a3:\t %o\n\n", a3;

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

// test isomorphic curve
_num, _den := j_invariant(a1, a3, 0, 0, 0);
num_, den_ := j_invariant(a1 * yP, a3 * (yP^3), 0, 0, 0);

print "\nIsomorphic 3-isogeny curve, Ẽ: y² + ã₁xy + ã₃y = x³\nwhere";
printf "\nã1:\t %o\n", a1 * yP;
printf "ã3:\t %o\n\n", a3 * (yP^3);

assert(_num/_den eq num_/den_);
printf "Do we have j(E) = j(Ẽ)? %o\n", _num/_den eq num_/den_;

exit;