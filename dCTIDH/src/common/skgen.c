#include <string.h>
#include <assert.h>

#include "primes.h"
#include "random.h"

#ifdef CTIDH
#include "../CTIDH/ctidh.h"

// WOMBat keygen
void ctidh_private(private_key *priv)
{
    memset(priv, 0, sizeof(private_key));

    uint8_t rnd;

    uint64_t batch_sumkeys = 0;
    for (uint32_t b = 0; b < primes_batches; b++)
    {
        random_wombats(priv->ells, batch_numkeys[b], batch_start[b], batch_stop[b], batch_sumkeys);
        batch_sumkeys += batch_numkeys[b];
    }

    // todo: uniform sampling needed!
    for (uint32_t b = 0; b < WOMBATKEYS; b++)
    {
        randombytes(&rnd, 1);
        priv->directions[b] = (rnd % 3);
    }

#if defined(_M1_)
    priv->radical_length = 1;
    randombytes(&rnd, 1);
    priv->radical_direction = rnd % 2;
#elif defined(_M2_)
    randombytes(&rnd, 1);
    priv->radical_length = rnd % 3;
    randombytes(&rnd, 1);
    priv->radical_direction = rnd % 2;
#elif defined(_M3_)
    randombytes(&rnd, 1);
    priv->radical_length = rnd % 5;
    randombytes(&rnd, 1);
    priv->radical_direction = rnd % 2;
#elif defined(_M4_)
    randombytes(&rnd, 1);
    priv->radical_length = rnd % 9;
    randombytes(&rnd, 1);
    priv->radical_direction = rnd % 2;
#elif defined(_M5_)
    randombytes(&rnd, 1);
    priv->radical_length = rnd % 17;
    randombytes(&rnd, 1);
    priv->radical_direction = rnd % 2;
#else
    priv->radical_length = 0;
    priv->radical_direction = 0;
#endif
}

#else
#include "../CSIDH/csidh.h"

void csidh_private(private_key *priv)
{
    int i;
    uint8_t rnd;
    for (i = 0; i < (int)N; i++)
    {
        randombytes(&rnd, 1);
        rnd = rnd & 0x1;
        priv->e[i] = rnd - (rnd ^ 0x1);
    };
    // memset(priv->e, 1, sizeof(private_key));
}

#endif
