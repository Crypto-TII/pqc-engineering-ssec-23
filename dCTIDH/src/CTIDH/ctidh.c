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
#include "isogeny_walks.h"

#include "../common/primes.h"
#include "../common/int64mask.h"
#include "../common/elligator.h"
#include "../common/random.h"


#include "../common/fp/fp-counters.h"

#ifdef ENABLE_CT_TESTING
#include <valgrind/memcheck.h>
#endif



const public_key base = {.A = {0}, .seed = ELLIGATOR_SEED}; /* A = 0 */

// static void fp_print(fp A)
// {pi
// 	uint8_t i;
//   printf("{");
// 	for(i = 0; i < NUMBER_OF_WORDS; i++)
// 		printf("0x%016lX,", A[i]);
// 	printf("}\n");
// }


static void cmov(int64_t *r, const int64_t *a, int64_t b)
{
	uint64_t t;

	t = (*r ^ *a) & b;
	*r ^= t;
}

/* get priv[pos] in constant time  */
static int64_t lookup(int64_t pos, const int64_t *priv)
{
    int64_t b;
    int64_t r;
    for (int64_t i = 0; i < primes_num; i++)
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

/*
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
*/

// Obtaining points of full order
void fulltorsion_points(fp u, fp const a)
{
    proj Tp, Tm, Pp[batch_stop[primes_batches - 1] + 1], Pm[batch_stop[primes_batches - 1] + 1], A;
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
        VALGRIND_MAKE_MEM_DEFINED(&A, sizeof(proj));
        VALGRIND_MAKE_MEM_DEFINED(&boolp, sizeof(uint8_t));
        VALGRIND_MAKE_MEM_DEFINED(&boolm, sizeof(uint8_t));
        VALGRIND_MAKE_MEM_DEFINED(&Tp, sizeof(proj));
        VALGRIND_MAKE_MEM_DEFINED(Pp, sizeof(proj) * (batch_stop[primes_batches - 1] + 1 - batch_start[0]));
        VALGRIND_MAKE_MEM_DEFINED(Pm, sizeof(proj) * (batch_stop[primes_batches - 1] + 1 - batch_start[0]));
#endif

        fp_add(u, u, fp_1); // u <- u + 1 ... Recall, 1 must be in Montgomery domain
        elligator_seeded(&Tp, &Tm, &A, (const fp *)u);

        clearpublicprimes(&Tp, &A);
        clearpublicprimes(&Tm, &A);
        // clear ells replaced by radical 3-isogenies
        for (j = 0; j < batch_start[0]; j++)
        {
            xMUL_dac(&Tp, &A, 0, &Tp, primes_dac[j], primes_daclen[j], primes_daclen[j]);
            xMUL_dac(&Tm, &A, 0, &Tm, primes_dac[j], primes_daclen[j], primes_daclen[j]);
        }
        // clear ells not used in the keyspace
        for (j = batch_stop[primes_batches - 1] + 1; j < primes_num; j++)
        {
            xMUL_dac(&Tp, &A, 0, &Tp, primes_dac[j], primes_daclen[j], primes_daclen[j]);
            xMUL_dac(&Tm, &A, 0, &Tm, primes_dac[j], primes_daclen[j], primes_daclen[j]);
        }

        // Checking if Tp is an order (p+1)/(2^e)
        proj_copy(&Pp[0], &Tp);
        cofactor_multiples(Pp, A, 0, batch_stop[primes_batches - 1] + 1);
        boolp = 1;
        for (j = batch_start[0]; j < (int)(batch_stop[primes_batches - 1] + 1); j++)
            boolp &= (1 - fp_iszero(Pp[j].z));

        if (1 - boolp)
            continue;

        // ---> This can be removed for wd1 style
        // Checking if Tm is an order (p+1)/(2^e)
        proj_copy(&Pm[0], &Tm);
        cofactor_multiples(Pm, A, 0, batch_stop[primes_batches - 1] + 1);

        boolm = 1;
        for (j = batch_start[0]; j < (int)(batch_stop[primes_batches - 1] + 1); j++)
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
// #if !defined(_M1_) && !defined(_M2_) && !defined(_M3_) && !defined(_M4_) && !defined(_M5_)
//   printf("\nNOT using radical 3-isogenies (action).\n\n");
// #else
//   printf("\nUsing 2^%ld radical 3-isogenies.\n\n", batch_start[0] - 1);
// #endif

  init_counters();

  proj A;
  fp_copy(A.x, in->A);
  fp_copy(A.z, fp_1);

  proj A24;
  xA24(&A24, &A);

  proj Points[2];
  fp seed;
  fp_set(seed, in->seed);
  fp_enc(seed, seed);
  elligator_seeded(&Points[0], &Points[1], &A24, (const fp *)seed);

  clearpublicprimes(&Points[0], &A24);
  clearpublicprimes(&Points[1], &A24);

  // clear ells replaced by radical 3-isogenies
  for (int16_t j = 0; j < batch_start[0]; j++)
  {
      xMUL_dac(&Points[0], &A24, 0, &Points[0], primes_dac[j], primes_daclen[j], primes_daclen[j]);
      xMUL_dac(&Points[1], &A24, 0, &Points[1], primes_dac[j], primes_daclen[j], primes_daclen[j]);
  }
  // clear ells not used in the keyspace
  for (int16_t j = batch_stop[primes_batches - 1] + 1; j < primes_num; j++)
  {
    xMUL_dac(&Points[0], &A24, 0, &Points[0], primes_dac[j], primes_daclen[j], primes_daclen[j]);
    xMUL_dac(&Points[1], &A24, 0, &Points[1], primes_dac[j], primes_daclen[j], primes_daclen[j]);
  }

  // collect primes not used in the key ...
  int16_t unused[batch_stop[primes_batches - 1] + 1];

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

    int64_t dac = lookup(t, primes_dac);
    int64_t daclen = lookup(t, primes_daclen);

    xMUL_dac(&Points[0], &A24, 0, &Points[0], dac, daclen, batch_maxdac[tmp_b]);
    xMUL_dac(&Points[1], &A24, 0, &Points[1], dac, daclen, batch_maxdac[tmp_b]);
  }


  proj ramifications[2 * WOMBATKEYS] = {0};
  int inner,
      block = 0, // current size of ramifications
      pos,       // index of the current small odd prime to be processed
      k = 0;     // strategy element

  int64_t moves = 0, // required for reaching an torsion-(l_pos) point
      xmul_counter[WOMBATKEYS] = {0},
          Plen = 0;
  (void)pos;

  int8_t swap = 0;

  proj_copy(&ramifications[0], (const proj *)&Points[0]); // point on E[\pi + 1]
  proj_copy(&ramifications[1], (const proj *)&Points[1]); // point on E[\pi - 1]

  int16_t current_batch = 0; //primes_batches - 1;
  int16_t current_batch_inner = batch_numkeys[current_batch];

  proj Points_[2 * WOMBATKEYS] = {0};

  // #pragma unroll WOMBATKEYS
  //int64_t tmp_cost = fpmul+fpsqr;
  for (int i = WOMBATKEYS - 1; i >= 0; i--)
  {
    if ((WOMBATKEYS - i - 1) > batch_keybounds_stop[current_batch])
    {
      current_batch++;
      current_batch_inner = batch_numkeys[current_batch];
    }


    uint16_t flip_index = WOMBATKEYS - i -1;
    uint16_t current_ell_index = priv->ells[flip_index];
    uint64_t current_ell = lookup(current_ell_index, primes);
    // 0 -> dummy, 1 -> + direction, 2 -> - direction
    int64_t direction = priv->directions[flip_index];

    uint64_t lowerend_ell = primes[batch_start[current_batch] + batch_numkeys[current_batch] - current_batch_inner];
    uint64_t upperend_ell = primes[batch_stop[current_batch] - current_batch_inner + 1];


    // conditional swap based on direction
    swap = -int64mask_equal(direction, (int64_t)2);
    proj_cswap(&ramifications[0+ 2 * block], &ramifications[1+ 2 * block], swap);

    tmp_b = primes_batches-1;
    while (moves < i)
    {
      block += 1;

      proj_copy(&ramifications[0 + 2 * block], (const proj *)&ramifications[0 + 2 * (block - 1)]); // point on E[\pi + 1]
      proj_copy(&ramifications[1 + 2 * block], (const proj *)&ramifications[1 + 2 * (block - 1)]); // point on E[\pi - 1]

      // #pragma unroll
      for (inner = moves; inner < (moves + strategy[k]); inner++)
      {
        pos = WOMBATKEYS - inner -1;
        while (pos < batch_keybounds_start[tmp_b])
          tmp_b--;

        int64_t dac = lookup(priv->ells[pos], primes_dac);
        int64_t daclen = lookup(priv->ells[pos], primes_daclen);

        if(moves + strategy[k] < i)
        {
          xMUL_dac(&ramifications[0 + 2 * block], &A24, 0, &ramifications[0 + 2 * block], dac, daclen, batch_maxdac[tmp_b]);
          xMUL_dac(&ramifications[1 + 2 * block], &A24, 0, &ramifications[1 + 2 * block], dac, daclen, batch_maxdac[tmp_b]);
        }
        else { // current block
          xMUL_dac(&ramifications[0 + 2 * block], &A24, 0, &ramifications[0 + 2 * block], dac, daclen, batch_maxdac[tmp_b]);
        }
      }

      xmul_counter[block] = strategy[k]; // the k-th element is used for next iteration on moves
      moves += strategy[k];              // moves is incremented
      k += 1;
    } 

    // how many points should be evaluated?
    Plen = 2 * block;

 
    proj Anew;
    proj_copy(&Anew, &A);
    for (int j = 0; j < Plen; j++)
    {
      // backup for the dummy case
      proj_copy(&Points_[j], &ramifications[j]);
    }

    xISOG_matryoshka(&Anew, Points_, Plen, &ramifications[0 + 2 * block], current_ell, lowerend_ell, upperend_ell);


    // copy back
    proj_cmov(&A, &Anew, -int64mask_nonzero(direction));
    xA24(&A24, &A);


    for (int j = 0; j < Plen; j++)
    {
      proj_cmov(&ramifications[j], &Points_[j], -int64mask_nonzero(direction));

      // in case of dummy, we still need to "remove" degree
      
      int64_t dac = lookup(current_ell_index, primes_dac);
      int64_t daclen = lookup(current_ell_index, primes_daclen);

      xMUL_dac(&ramifications[j], &A24, 0, &ramifications[j], dac, daclen, batch_maxdac[current_batch]);
    }


    // Configuring for the next iteration
    moves -= xmul_counter[block];
    xmul_counter[block] = 0;
    block -= 1;
    // swap back based on direction
    proj_cswap(&ramifications[0+ 2 * block], &ramifications[1+ 2 * block], swap);

    current_batch_inner--;
  }

#if defined(_M0_)
    fp_inv(A.z);
    fp_mul2(&A.x, (const fp *)&A.z);
    fp_copy(out->A, A.x);
#else
    isogeny_walks_3_fp(out->A, A, priv->radical_length, priv->radical_direction);
#endif

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
