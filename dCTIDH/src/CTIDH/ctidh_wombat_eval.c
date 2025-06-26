//*******************************//
//                               //
//   here                        //
//        lives                  //
//              WOMBats          //
//                               //
//_______________________________//

#include <string.h>
#include <assert.h>

#include "ctidh.h"

#include "../common/primes.h"
#include "../common/int64mask.h"
#include "../common/elligator.h"
#include "../common/random.h"
// #include "crypto_declassify.h"

#ifdef ENABLE_CT_TESTING
#include <valgrind/memcheck.h>
#endif

const public_key base = {.A = {0}, .seed = ELLIGATOR_SEED}; /* A = 0 */

// static void fp_print(fp A)
// {
// 	uint8_t i;
//   printf("{");
// 	for(i = 0; i < NUMBER_OF_WORDS; i++)
// 		printf("0x%016lX,", A[i]);
// 	printf("}\n");
// }

static void cmov(int64_t *r, const long long *a, int64_t b)
{
	uint64_t t;

	t = (*r ^ *a) & b;
	*r ^= t;
}

/* get priv[pos] in constant time  */
static int64_t lookup(int64_t pos, const long long *priv)
{
    int64_t b;
    int64_t r;
    for (long long i = 0; i < primes_num; i++)
    {
        b = int64mask_equal(i, pos);
        cmov(&r, &priv[i], b);
    }
    return r;
}

static void clearpublicprimes(proj *P, const proj *A24)
{
    // clear powers of 2
    for (int64_t i = 0; i < two_cofactor; i++)
    {
        xDBL(P, P, A24, 0);
    }
}

static void multiples(proj Q[], proj const P, proj const A)
{
    int j;

    proj_copy(&Q[0], &P);
    for (j = 0; j < (int)primes_num; j++)
    {
        if (primes[j] == 3 || primes[j] == 5 || primes[j] == 7)
            continue;

        xMUL_dac(&Q[0], &A, 1, &Q[0], primes_dac[j], primes_daclen[j], primes_daclen[j]);
    }

    //  --- multiplying by 3
    xMUL_dac(&Q[1], &A, 1, &Q[0], 0, 0, 0);
    xMUL_dac(&Q[1], &A, 1, &Q[1], 0, 0, 0);
    //  --- multiplying by 5
    xMUL_dac(&Q[0], &A, 1, &Q[0], 0, 1, 1);
    xMUL_dac(&Q[0], &A, 1, &Q[0], 0, 1, 1);
    xMUL_dac(&Q[2], &A, 1, &Q[1], 0, 1, 1);
    xMUL_dac(&Q[2], &A, 1, &Q[2], 0, 1, 1);
    //  --- multiplying by 7
    xMUL_dac(&Q[0], &A, 1, &Q[0], 2, 2, 2);
    xMUL_dac(&Q[0], &A, 1, &Q[0], 2, 2, 2);
    xMUL_dac(&Q[1], &A, 1, &Q[1], 2, 2, 2);
    xMUL_dac(&Q[1], &A, 1, &Q[1], 2, 2, 2);
}

// Obtaining points of full order
void fulltorsion_points(fp u, fp const a)
{
    proj Tp, Tm, Pp[primes_num], Aux_Tp[3], Pm[primes_num], Aux_Tm[3], A;
    int j;

    // Convert curve to projective Montgomery form (A' + 2C : 4C)
    fp_copy(A.x, a);
    fp_set1(A.z);
    fp_add(A.z, A.z, A.z); // 2C
    fp_add(A.x, A.x, A.z); // A' + 2C
    fp_add(A.z, A.z, A.z); // 4C

    fp_set0(u); // u <- 0
    uint8_t boolp = 0, boolm = 0;

    do
    {

#ifdef ENABLE_CT_TESTING
        VALGRIND_MAKE_MEM_DEFINED(Aux_Tp, sizeof(proj) * 3);
        VALGRIND_MAKE_MEM_DEFINED(Aux_Tm, sizeof(proj) * 3);
        VALGRIND_MAKE_MEM_DEFINED(&A, sizeof(proj));
        VALGRIND_MAKE_MEM_DEFINED(&boolp, sizeof(uint8_t));
        VALGRIND_MAKE_MEM_DEFINED(&boolm, sizeof(uint8_t));
        VALGRIND_MAKE_MEM_DEFINED(&Tp, sizeof(proj));
        VALGRIND_MAKE_MEM_DEFINED(Pp, sizeof(proj) * primes_num);
        VALGRIND_MAKE_MEM_DEFINED(Pm, sizeof(proj) * primes_num);
#endif

        fp_add(u, u, fp_1); // u <- u + 1 ... Recall, 1 must be in Montgomery domain
        elligator_seeded(&Tp, &Tm, &A, (const fp *)u);

        clearpublicprimes(&Tp, &A);
        clearpublicprimes(&Tm, &A);

#ifdef ENABLE_CT_TESTING
        memset(Aux_Tp, 0, sizeof(proj) * 3);
#endif
        multiples(Aux_Tp, Tp, A);
        if (fp_iszero(Aux_Tp[0].z) | fp_iszero(Aux_Tp[1].z) | fp_iszero(Aux_Tp[2].z))
            continue;

#ifdef ENABLE_CT_TESTING
        memset(Aux_Tm, 0, sizeof(proj) * 3);
#endif
        multiples(Aux_Tm, Tm, A);
        if (fp_iszero(Aux_Tm[0].z) | fp_iszero(Aux_Tm[1].z) | fp_iszero(Aux_Tm[2].z))
            continue;

        // Checking if Tp is an order (p+1)/(2^e)
        proj_copy(&Pp[0], &Tp);
        cofactor_multiples(Pp, A, 0, primes_num);
        boolp = 1;
        boolp &= (1 - fp_iszero(Pp[0].z)) | (1 - fp_iszero(Aux_Tp[0].z));
        boolp &= (1 - fp_iszero(Pp[1].z)) | (1 - fp_iszero(Aux_Tp[1].z));
        boolp &= (1 - fp_iszero(Pp[2].z)) | (1 - fp_iszero(Aux_Tp[2].z));
        for (j = 3; j < (int)primes_num; j++)
            boolp &= (1 - fp_iszero(Pp[j].z));

        if (1 - boolp)
            continue;

        // ---> This can be removed for wd1 style
        // Checking if Tm is an order (p+1)/(2^e)
        proj_copy(&Pm[0], &Tm);
        cofactor_multiples(Pm, A, 0, primes_num);

        boolm = 1;
        boolm &= (1 - fp_iszero(Pm[0].z)) | (1 - fp_iszero(Aux_Tm[0].z));
        boolm &= (1 - fp_iszero(Pm[1].z)) | (1 - fp_iszero(Aux_Tm[1].z));
        boolm &= (1 - fp_iszero(Pm[2].z)) | (1 - fp_iszero(Aux_Tm[2].z));
        for (j = 3; j < (int)primes_num; j++)
            boolm &= (1 - fp_iszero(Pm[j].z));

        if (1 - boolm)
            continue;
        // <---
    } while ((1 - boolp) | (1 - boolm));

    fp_dec(u, (uint64_t *const)u);
}

/* goal: constant time */
void action(public_key *out, public_key const *in, private_key const *priv)
{

    init_counters();

    proj A;
    fp_copy(A.x, in->A);
    fp_copy(A.z, fp_1);

    proj A24;
    xA24(&A24, &A);

    proj Points[4];
    fp seed;
    fp_set(seed, in->seed);
    fp_enc(seed, seed);
    elligator_seeded(&Points[0], &Points[1], &A24, (const fp *)seed);

    clearpublicprimes(&Points[0], &A24);
    clearpublicprimes(&Points[1], &A24);

    // clear ells not used in the keyspace
    for (int16_t j = batch_stop[primes_batches - 1] + 1; j < primes_num; j++)
    {
        xMUL_dac(&Points[0], &A24, 0, &Points[0], primes_dac[j], primes_daclen[j], primes_daclen[j]);
        xMUL_dac(&Points[1], &A24, 0, &Points[1], primes_dac[j], primes_daclen[j], primes_daclen[j]);
    }

    // collect primes not used in the key ...
    int16_t unused[batch_stop[primes_batches - 1] + 1];
    // int16_t unused[255] = {0};

    int16_t u = 0;
    int16_t e = 0;
    int16_t w = 0;
    for (e = 0; e <= batch_stop[primes_batches - 1]; e++)
    {
#ifdef ENABLE_CT_TESTING
        VALGRIND_MAKE_MEM_DEFINED(&u, sizeof(int16_t));
        VALGRIND_MAKE_MEM_DEFINED(&e, sizeof(int16_t));
        VALGRIND_MAKE_MEM_DEFINED(&w, sizeof(int16_t));
#endif

        unused[u] = e;

        int64_t mov = -int64mask_equal((int64_t)e, (int64_t)priv->ells[w]);

        fp t1, t2;
        t1[0] = u;
        t2[0] = u + 1;
        fp_cmov(&t1, (const fp *)&t2, 1 - mov);
        u = t1[0];

        t1[0] = w;
        t2[0] = w + 1;
        fp_cmov(&t1, (const fp *)&t2, mov);
        w = t1[0];
    }

    // ... and remove them from our points
    int16_t tmp_b = 0;
    for (u = 0; u <= batch_stop[primes_batches - 1] - WOMBATKEYS; u++)
    {
        int16_t t = unused[u];
        if (t > batch_stop[tmp_b])
            tmp_b++;
        
        long long dac = lookup(t, primes_dac);
        long long daclen = lookup(t, primes_daclen);

        xMUL_dac(&Points[0], &A24, 0, &Points[0], dac, daclen, batch_maxdac[tmp_b]);
        xMUL_dac(&Points[1], &A24, 0, &Points[1], dac, daclen, batch_maxdac[tmp_b]);
    }

    // ACTION!
    // copy "outer" points
    proj_copy(&Points[2], &Points[0]);
    proj_copy(&Points[3], &Points[1]);

    int16_t current_key = WOMBATKEYS - 1;
    for (int16_t current_batch = primes_batches - 1; current_batch >= 0; current_batch--)
    {
        uint8_t inner_counter = 0;

        // outer points are now the inner points
        proj_copy(&Points[0], &Points[2]);
        proj_copy(&Points[1], &Points[3]);

        // remove degrees not in the batch
        tmp_b = 0;
        for (int16_t j = 0; j < batch_keybounds_start[current_batch]; j++)
        {
            if (j > batch_keybounds_stop[tmp_b])
                tmp_b++;

            long long dac = lookup(priv->ells[j], primes_dac);
            long long daclen = lookup(priv->ells[j], primes_daclen);

            xMUL_dac(&Points[0], &A24, 0, &Points[0], dac, daclen, batch_maxdac[tmp_b]);
            xMUL_dac(&Points[1], &A24, 0, &Points[1], dac, daclen, batch_maxdac[tmp_b]);
        }


        for (; current_key >= batch_keybounds_start[current_batch]; current_key--)
        {
            uint16_t current_ell_index = priv->ells[current_key];
            uint64_t current_ell = lookup(current_ell_index, primes);
            // 0 -> dummy, 1 -> + direction, 2 -> - direction
            int64_t direction = priv->directions[current_key];

            uint64_t lowerend_ell = primes[batch_start[current_batch] + batch_numkeys[current_batch] - inner_counter - 1];
            uint64_t upperend_ell = primes[batch_stop[current_batch] - inner_counter];
            // printf("%3d: %2d - %4ld-isogeny to %ld for cost of (%4ld,%4ld)\n",
            //           current_key, current_batch, current_ell, direction, lowerend_ell, upperend_ell);

            // multiply out other factors
            proj K;
            proj_cmov(&K, &Points[0], -int64mask_equal(direction, (int64_t)2));
            proj_cmov(&K, &Points[1], 1 + int64mask_equal(direction, (int64_t)2));

            // we skip the "last/already processed" and remove only the "other" degrees
            for (int16_t j = batch_keybounds_start[current_batch]; j < batch_keybounds_stop[current_batch] - inner_counter; j++)
            {
                xMUL_dac(&K, &A24, 0, &K, lookup(priv->ells[j], primes_dac), lookup(priv->ells[j], primes_daclen), batch_maxdac[current_batch]);
            }
            
            // make a copy to trow away in the dummy case
            proj A_;
            proj Points_[4];
            proj_copy(&Points_[0], &Points[0]);
            proj_copy(&Points_[1], &Points[1]);
            proj_copy(&Points_[2], &Points[2]);
            proj_copy(&Points_[3], &Points[3]);
            proj_copy(&A_, &A);
            // we can now compute the isogeny
            if (current_key == 0)
            {
                // this is the last isogeny we need to compute! so we don't need to push points
                xISOG_matryoshka(&A_, Points_, 0, &K, current_ell, lowerend_ell, upperend_ell);
            }
            else if (current_key == batch_keybounds_start[current_batch])
            {
                // on the last isogeny of the batch, we only need to push the 2 "outer" points
                xISOG_matryoshka(&A_, &Points_[2], 2, &K, current_ell, lowerend_ell, upperend_ell);
            }
            else if (current_batch == 0)
            {
                // on the last batch, we only need the inner points
                xISOG_matryoshka(&A_, Points_, 2, &K, current_ell, lowerend_ell, upperend_ell);
            }
            else if (current_key == batch_keybounds_start[current_batch] + 1)
            {
                // on the second to last isogeny of a batch, we only need to push 3 points
                // the one needed for the last isogeny of the batch + the two outer points
                proj_cswap(&Points_[0], &Points_[1], -int64mask_equal(priv->directions[current_key - 1], (int64_t)2));
                xISOG_matryoshka(&A_, &Points_[1], 3, &K, current_ell, lowerend_ell, upperend_ell);
                proj_cswap(&Points_[0], &Points_[1], -int64mask_equal(priv->directions[current_key - 1], (int64_t)2));
            }
            else
            {
                xISOG_matryoshka(&A_, Points_, 4, &K, current_ell, lowerend_ell, upperend_ell);
            }

            // skip isogeny in case of dummy isog
            proj_cmov(&A, &A_, -int64mask_nonzero(direction));
            proj_cmov(&Points[0], &Points_[0], -int64mask_nonzero(direction));
            proj_cmov(&Points[1], &Points_[1], -int64mask_nonzero(direction));
            proj_cmov(&Points[2], &Points_[2], -int64mask_nonzero(direction));
            proj_cmov(&Points[3], &Points_[3], -int64mask_nonzero(direction));

            xA24(&A24, &A);


            long long dac = lookup(current_ell_index, primes_dac);
            long long daclen = lookup(current_ell_index, primes_daclen);

            // needed to remove the order incase a dummy isogeny was used
            if (current_key > batch_keybounds_start[current_batch])
            {
                // not needed for the inner points in the last round
                if (current_key == batch_keybounds_start[current_batch] + 1)
                {
                    proj_cswap(&Points[0], &Points[1], -int64mask_equal(priv->directions[current_key - 1], (int64_t)2));
                    xMUL_dac(&Points[1], &A24, 0, &Points[1], dac, daclen, batch_maxdac[current_batch]);
                    proj_cswap(&Points[0], &Points[1], -int64mask_equal(priv->directions[current_key - 1], (int64_t)2));
                }
                else
                {
                    xMUL_dac(&Points[0], &A24, 0, &Points[0], dac, daclen, batch_maxdac[current_batch]);
                    xMUL_dac(&Points[1], &A24, 0, &Points[1], dac, daclen, batch_maxdac[current_batch]);
                }
            }

            if (current_batch != 0)
            {
                xMUL_dac(&Points[2], &A24, 0, &Points[2], dac, daclen, batch_maxdac[current_batch]);
                xMUL_dac(&Points[3], &A24, 0, &Points[3], dac, daclen, batch_maxdac[current_batch]);
            }

            inner_counter++;
        }
    }

    fp_inv(A.z);
    fp_mul2(&A.x, (const fp *)&A.z);
    fp_copy(out->A, A.x);
}

/* includes public-key validation. */
bool csidh(public_key *out, public_key const *in, private_key const *priv)
{
    if (!validate(in))
    {
        fp_random(out->A);
        return false;
    }
    action(out, in, priv);
    return true;
}

//
//
//
//                 ,.--""""--.._
//              ."     .'      `-.
//             ;      ;           ;
//            '      ;             )
//           /     '             . ;
//          /     ;     `.        `;
//        ,.'     :         .     : )
//        ;|\'    :      `./|) \  ;/
//        ;| \"  -,-   "-./ |;  ).;
//        /\/              \/   );
//       :                 \    ;
//       :     _      _     ;   )
//       `.   \;\    /;/    ;  /
//         !    :   :     ,/  ;
//          (`. : _ : ,/""   ;
//           \\\`"^" ` :    ;
//                    (    )
//                     ////
//
//
// by https://ascii.co.uk/art/wombat
