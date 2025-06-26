clear;
bits := 72;

sop := PrimesUpTo(2048);
fct := sop[1..15]; // bits: 72
// fct := sop[1..42]; // bits: 255
// fct := sop[1..58]; // bits: 383
q := fct[#fct];
for i:= 1 to 25 do
    q := NextPrime(q);
    for j:= 1 to #fct do
        sop := fct[1..(j-1)] cat fct[(j+1)..#fct] cat [q];
        p := 2 * &*sop * q - 1;
        if IsPrime(p) and #Intseq(p, 2) eq bits and 3 in sop then
            if p mod 4 eq 3 and p^2 mod 16 eq 9 then
                 print p;
            end if;
        end if;
    end for;
end for;