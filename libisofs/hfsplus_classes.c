/*
 * 
 * Based on Unicode 3.2.0.
 * See http://www.unicode.org/copyright.html
 * Quote from there:
 * "Copyright (c) 1991-2012 Unicode, Inc. All rights reserved.
 * [...] 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of the Unicode data files and any associated documentation
 * (the "Data Files") or Unicode software and any associated documentation
 * (the "Software") to deal in the Data Files or Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Data Files or Software, and to permit
 * persons to whom the Data Files or Software are furnished to do so, provided
 * that (a) the above copyright notice(s) and this permission notice appear
 * with all copies of the Data Files or Software, (b) both the above copyright
 * notice(s) and this permission notice appear in associated documentation,
 * and (c) there is clear notice in each modified Data File or in the Software
 * as well as in the documentation associated with the Data File(s) or
 * Software that the data or software has been modified."
 * 
 *
 * For this particular implementation:
 *
 * Copyright (c) 2012 Vladimir Serbinenko
 * Copyright (c) 2012 Thomas Schmitt
 *
 * This file is part of the libisofs project; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * or later as published by the Free Software Foundation.
 * See COPYING file for details.
 */

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <string.h>

#include "hfsplus.h"


/* This encodes a matrix of page and character, with 16-bit words as elements.
   Initially the matrix is filled with zeros.
   1: The first element is the page number.
      If it is equal or lower than the previous one, then the matrix is done.
   2: The next element is the character number
      If it is  equal or lower than the previous one, the page is done. Goto 1.
   3: The next element is the byte value. Goto 2.
*/

static uint16_t class_page_data[] = {

  /* page 03 */
  0x03,
  0x00, 0x230,
  0x01, 0x230,
  0x02, 0x230,
  0x03, 0x230,
  0x04, 0x230,
  0x05, 0x230,
  0x06, 0x230,
  0x07, 0x230,
  0x08, 0x230,
  0x09, 0x230,
  0x0a, 0x230,
  0x0b, 0x230,
  0x0c, 0x230,
  0x0d, 0x230,
  0x0e, 0x230,
  0x0f, 0x230,
  0x10, 0x230,
  0x11, 0x230,
  0x12, 0x230,
  0x13, 0x230,
  0x14, 0x230,
  0x15, 0x232,
  0x16, 0x220,
  0x17, 0x220,
  0x18, 0x220,
  0x19, 0x220,
  0x1a, 0x232,
  0x1b, 0x216,
  0x1c, 0x220,
  0x1d, 0x220,
  0x1e, 0x220,
  0x1f, 0x220,
  0x20, 0x220,
  0x21, 0x202,
  0x22, 0x202,
  0x23, 0x220,
  0x24, 0x220,
  0x25, 0x220,
  0x26, 0x220,
  0x27, 0x202,
  0x28, 0x202,
  0x29, 0x220,
  0x2a, 0x220,
  0x2b, 0x220,
  0x2c, 0x220,
  0x2d, 0x220,
  0x2e, 0x220,
  0x2f, 0x220,
  0x30, 0x220,
  0x31, 0x220,
  0x32, 0x220,
  0x33, 0x220,
  0x34, 0x1,
  0x35, 0x1,
  0x36, 0x1,
  0x37, 0x1,
  0x38, 0x1,
  0x39, 0x220,
  0x3a, 0x220,
  0x3b, 0x220,
  0x3c, 0x220,
  0x3d, 0x230,
  0x3e, 0x230,
  0x3f, 0x230,
  0x40, 0x230,
  0x41, 0x230,
  0x42, 0x230,
  0x43, 0x230,
  0x44, 0x230,
  0x45, 0x240,
  0x46, 0x230,
  0x47, 0x220,
  0x48, 0x220,
  0x49, 0x220,
  0x4a, 0x230,
  0x4b, 0x230,
  0x4c, 0x230,
  0x4d, 0x220,
  0x4e, 0x220,
  0x60, 0x234,
  0x61, 0x234,
  0x62, 0x233,
  0x63, 0x230,
  0x64, 0x230,
  0x65, 0x230,
  0x66, 0x230,
  0x67, 0x230,
  0x68, 0x230,
  0x69, 0x230,
  0x6a, 0x230,
  0x6b, 0x230,
  0x6c, 0x230,
  0x6d, 0x230,
  0x6e, 0x230,
  0x6f, 0x230,
  0x00,

  /* page04 */
  0x04,
  0x83, 0x230,
  0x84, 0x230,
  0x85, 0x230,
  0x86, 0x230,
  0x00,

  /* page05 */
  0x05,
  0x91, 0x220,
  0x92, 0x230,
  0x93, 0x230,
  0x94, 0x230,
  0x95, 0x230,
  0x96, 0x220,
  0x97, 0x230,
  0x98, 0x230,
  0x99, 0x230,
  0x9a, 0x222,
  0x9b, 0x220,
  0x9c, 0x230,
  0x9d, 0x230,
  0x9e, 0x230,
  0x9f, 0x230,
  0xa0, 0x230,
  0xa1, 0x230,
  0xa3, 0x220,
  0xa4, 0x220,
  0xa5, 0x220,
  0xa6, 0x220,
  0xa7, 0x220,
  0xa8, 0x230,
  0xa9, 0x230,
  0xaa, 0x220,
  0xab, 0x230,
  0xac, 0x230,
  0xad, 0x222,
  0xae, 0x228,
  0xaf, 0x230,
  0xb0, 0x10,
  0xb1, 0x11,
  0xb2, 0x12,
  0xb3, 0x13,
  0xb4, 0x14,
  0xb5, 0x15,
  0xb6, 0x16,
  0xb7, 0x17,
  0xb8, 0x18,
  0xb9, 0x19,
  0xbb, 0x20,
  0xbc, 0x21,
  0xbd, 0x22,
  0xbf, 0x23,
  0xc1, 0x24,
  0xc2, 0x25,
  0xc4, 0x230,
  0x00,

  /* page06 */
  0x06,
  0x4b, 0x27,
  0x4c, 0x28,
  0x4d, 0x29,
  0x4e, 0x30,
  0x4f, 0x31,
  0x50, 0x32,
  0x51, 0x33,
  0x52, 0x34,
  0x53, 0x230,
  0x54, 0x230,
  0x55, 0x220,
  0x70, 0x35,
  0xd6, 0x230,
  0xd7, 0x230,
  0xd8, 0x230,
  0xd9, 0x230,
  0xda, 0x230,
  0xdb, 0x230,
  0xdc, 0x230,
  0xdf, 0x230,
  0xe0, 0x230,
  0xe1, 0x230,
  0xe2, 0x230,
  0xe3, 0x220,
  0xe4, 0x230,
  0xe7, 0x230,
  0xe8, 0x230,
  0xea, 0x220,
  0xeb, 0x230,
  0xec, 0x230,
  0xed, 0x220,
  0x00,

  /* page07 */
  0x07,
  0x11, 0x36,
  0x30, 0x230,
  0x31, 0x220,
  0x32, 0x230,
  0x33, 0x230,
  0x34, 0x220,
  0x35, 0x230,
  0x36, 0x230,
  0x37, 0x220,
  0x38, 0x220,
  0x39, 0x220,
  0x3a, 0x230,
  0x3b, 0x220,
  0x3c, 0x220,
  0x3d, 0x230,
  0x3e, 0x220,
  0x3f, 0x230,
  0x40, 0x230,
  0x41, 0x230,
  0x42, 0x220,
  0x43, 0x230,
  0x44, 0x220,
  0x45, 0x230,
  0x46, 0x220,
  0x47, 0x230,
  0x48, 0x220,
  0x49, 0x230,
  0x4a, 0x230,
  0x00,

  /* page09 */
  0x09,
  0x3c, 0x7,
  0x4d, 0x9,
  0x51, 0x230,
  0x52, 0x220,
  0x53, 0x230,
  0x54, 0x230,
  0xbc, 0x7,
  0xcd, 0x9,
  0x00,

  /* page0a */
  0x0a,
  0x3c, 0x7,
  0x4d, 0x9,
  0xbc, 0x7,
  0xcd, 0x9,
  0x00,

  /* page0b */
  0x0b,
  0x3c, 0x7,
  0x4d, 0x9,
  0xcd, 0x9,
  0x00,

  /* page0c */
  0x0c,
  0x4d, 0x9,
  0x55, 0x84,
  0x56, 0x91,
  0xcd, 0x9,
  0x00,

  /* page0d */
  0x0d,
  0x4d, 0x9,
  0xca, 0x9,
  0x00,

  /* page0e */
  0x0e,
  0x38, 0x103,
  0x39, 0x103,
  0x3a, 0x9,
  0x48, 0x107,
  0x49, 0x107,
  0x4a, 0x107,
  0x4b, 0x107,
  0xb8, 0x118,
  0xb9, 0x118,
  0xc8, 0x122,
  0xc9, 0x122,
  0xca, 0x122,
  0xcb, 0x122,
  0x00,

  /* page0f */
  0x0f,
  0x18, 0x220,
  0x19, 0x220,
  0x35, 0x220,
  0x37, 0x220,
  0x39, 0x216,
  0x71, 0x129,
  0x72, 0x130,
  0x74, 0x132,
  0x7a, 0x130,
  0x7b, 0x130,
  0x7c, 0x130,
  0x7d, 0x130,
  0x80, 0x130,
  0x82, 0x230,
  0x83, 0x230,
  0x84, 0x9,
  0x86, 0x230,
  0x87, 0x230,
  0xc6, 0x220,
  0x00,

  /* page10 */
  0x10,
  0x37, 0x7,
  0x39, 0x9,
  0x00,

  /* page17 */
  0x17,
  0x14, 0x9,
  0x34, 0x9,
  0xd2, 0x9,
  0x00,

  /* page18 */
  0x18,
  0xa9, 0x228,
  0x00,

  /* page20 */
  0x20,
  0xd0, 0x230,
  0xd1, 0x230,
  0xd2, 0x1,
  0xd3, 0x1,
  0xd4, 0x230,
  0xd5, 0x230,
  0xd6, 0x230,
  0xd7, 0x230,
  0xd8, 0x1,
  0xd9, 0x1,
  0xda, 0x1,
  0xdb, 0x230,
  0xdc, 0x230,
  0xe1, 0x230,
  0xe5, 0x1,
  0xe6, 0x1,
  0xe7, 0x230,
  0xe8, 0x220,
  0xe9, 0x230,
  0xea, 0x1,
  0x00,

  /* page30 */
  0x30,
  0x2a, 0x218,
  0x2b, 0x228,
  0x2c, 0x232,
  0x2d, 0x222,
  0x2e, 0x224,
  0x2f, 0x224,
  0x99, 0x8,
  0x9a, 0x8,
  0x00,

  /* pagefb */
  0xfb,
  0x1e, 0x26,
  0x00,

  /* pagefe */
  0xfe,
  0x20, 0x230,
  0x21, 0x230,
  0x22, 0x230,
  0x23, 0x230,
  0x00,

  /* End of list */
  0x00
};

uint16_t *hfsplus_class_pages[256];

static uint16_t class_pages[19][256];

void make_hfsplus_class_pages()
{
    int page_idx = -1, char_idx, i;
    uint16_t *rpt, *page_pt;
    int page_count = 0;

    memset(class_pages, 0, 19 * 256 * sizeof(uint16_t));
    for (i = 0; i < 256; i++)
        hfsplus_class_pages[i] = NULL;  

    rpt = (uint16_t *) class_page_data;
    page_pt = (uint16_t *) class_pages;
    while (1) {
        if (*rpt <= page_idx)
    break;
        page_count++;
        page_idx = *(rpt++);
        char_idx = -1;
        while (1) {
            if(*rpt <= char_idx)
        break;
            char_idx = *(rpt++);
            page_pt[char_idx] = *(rpt++);
        }
        rpt++;
        hfsplus_class_pages[page_idx] = class_pages[page_count - 1];
        page_pt += 256;
    }
}

