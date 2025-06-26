//
// Prime field parameters GF(p)
//
#ifndef SSEC_PARAMETERS_H
#define SSEC_PARAMETERS_H


#if defined(_P254_)
#include "parameters/p254.h"
#elif defined(_P255_)
#include "parameters/p255.h"
#elif defined(_P381_)
#include "parameters/p381.h"
#elif defined(_P383_)
#include "parameters/p383.h"
#elif defined(_P398_)
#include "parameters/p398.h"
#elif defined(_P511_)
#include "parameters/p511.h"
#elif defined(_P575_)
#include "parameters/p575.h"
#elif defined(_P592_)
#include "parameters/p592.h"
#elif defined(_P765_)
#include "parameters/p765.h"
#elif defined(_P783_)
#include "parameters/p783.h"
// #elif defined(_PXXX_)
// #include "parameters/pXXX.h"
#else
#include "parameters/p255.h"
#endif


#endif // SSEC_PARAMETERS_H
