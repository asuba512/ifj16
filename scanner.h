/**
 * \file scanner.h
 * \brief Header file of lexical analyzer module.
 *
 */

#include <stdio.h>

/* List of tokens follows: */
#define ID 1
#define INTEGER 2
#define REAL 3
// ...

typedef enum {
	state_init,
	state_identifier,
	state_integer,
	_state_double_point,
	state_double,
	_state_double_e,
	_state_double_e_sign,
	state_double_e,
} t_state;

int get_token(FILE *);
