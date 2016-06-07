/* gf256mul.h */
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
#ifndef GF256MUL_H_
#define GF256MUL_H_

/**
 * \author  Daniel Otte
 * \email   bg@nerilex.org
 * \date    2008-12-19
 * \license GPLv3
 * \brief
 * 
 * 
 */

#include <stdint.h>

uint8_t gf256mul(uint8_t a, uint8_t b, uint8_t reducer);

#endif /* GF256MUL_H_ */

