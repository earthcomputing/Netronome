/*
 * Copyright (C) 2014-2016,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file          rotate_c.h
 * @brief         Interface for rotate function
 */

#ifndef _ROTATE_C_H_
#define _ROTATE_C_H_


/** Rotate left a 32-bit word by specified number of bits.
 *
 * @param value     The 32-bit value to rotate to the left
 * @param shift     The number of bits to rotate to the left (1 - 31).
 *
 * @return         The 32-bit result of the value shifted to the left by "shift" bits.
 */
__intrinsic unsigned int rotl(unsigned int value, int shift);


#endif // _ROTATE_C_H_
