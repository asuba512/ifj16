#ifndef SCANNER_INC
#define SCANNER_INC 1

#include <stdio.h>
#include "token.h"
#include "infinite_string.h"

string_t buff;

typedef enum {
	state_init,
	state_less,
	state_more,
	state_eq,
	state_neq,
	state_and,
	state_or,
	_state_division,
	_state_blockcomment,
	state_blockcomment,
	state_linecomment,
	state_identifier,
	_state_dot_fqidentifier,
	state_fqidentifier,
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
} t_state; // enumeration of states

int get_token(FILE *, token_t *t);
int check_keyword(token_t *t);

#endif
