/**
 * \file scanner.h
 * \brief Header file of lexical analyzer module.
 *
 */

#include <stdio.h>
#include "token.h"

/* List of tokens follows: */
#define ID 1
#define INTEGER 2
#define REAL 3
// ...

typedef enum {
	state_init,
	_state_division,
	_state_blockcomment,
	state_blockcomment,
	state_linecomment,
	state_identifier,
	state_integer,
	_state_double_point,
	state_double,
	_state_double_e,
	_state_double_e_sign,
	state_double_e,
	state_string,
	_state_string_escape,
	_state_string_octalx,
	_state_string_octalxx
} t_state;

token_t get_token(FILE *);

