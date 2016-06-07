/* aes.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2015 Daniel Otte (bg@nerilex.org)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * \file     aes_types.h
 * \email    bg@nerilex.org
 * \author   Daniel Otte
 * \date     2008-12-30
 * \license  GPLv3 or later
 *
 */
#ifndef AES_TYPES_H_
#define AES_TYPES_H_

#include <stdint.h>

typedef struct{
	uint8_t ks[16];
} aes_roundkey_t;

typedef struct{
	aes_roundkey_t key[10+1];
} aes128_ctx_t;

typedef struct{
	aes_roundkey_t key[12+1];
} aes192_ctx_t;

typedef struct{
	aes_roundkey_t key[14+1];
} aes256_ctx_t;

typedef struct{
	aes_roundkey_t key[1]; /* just to avoid the warning */
} aes_genctx_t;

typedef struct{
	uint8_t s[16];
} aes_cipher_state_t;

#endif
