#ifndef __PARAM_H__
#define __PARAM_H__

#include "const.h"
#include "bigint.h"
#include "curvep.h"
#include "sm2.h"

typedef struct
{
    mpz_t           fieldp;
    mpz_t           subgrpq;
    mpz_t           cofactor;
	mpz_t			coa;
	mpz_t			cob;
	point_affn_t	generator;
}ecc_param;

extern ecc_param *paramptr;

#define FIELD_P				paramptr->fieldp
#define SUBGRP_ORDER		paramptr->subgrpq
#define COA					paramptr->coa
#define COB					paramptr->cob
#define GENERATOR			paramptr->generator

#endif
