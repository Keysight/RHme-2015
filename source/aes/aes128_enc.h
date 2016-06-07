/* aes128_enc.h */
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
 * \file     aes128_enc.h
 * \email    bg@nerilex.org
 * \author   Daniel Otte
 * \date     2008-12-30
 * \license  GPLv3 or later
 * \ingroup  AES
 */

#ifndef AES128_ENC_H_
#define AES128_ENC_H_

#include "aes_types.h"
#include "aes_enc.h"

/**
 * \brief encrypt with 128 bit key.
 *
 * This function encrypts one block with the AES algorithm under control of
 * a keyschedule produced from a 128 bit key.
 * \param buffer pointer to the block to encrypt
 * \param ctx    pointer to the key schedule
 */
void aes128_enc(void *buffer, aes128_ctx_t *ctx, uint8_t countermeasures);

#endif /* AES128_ENC_H_ */
