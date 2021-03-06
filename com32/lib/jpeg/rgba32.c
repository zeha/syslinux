/*
 * Small jpeg decoder library
 *
 * Copyright (c) 2006, Luc Saillard <luc@saillard.org>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 * - Neither the name of the author nor the names of its contributors may be
 *  used to endorse or promote products derived from this software without
 *  specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tinyjpeg.h"
#include "tinyjpeg-internal.h"

/*******************************************************************************
 *
 * Colorspace conversion routine
 *
 *
 * Note:
 * YCbCr is defined per CCIR 601-1, except that Cb and Cr are
 * normalized to the range 0..MAXJSAMPLE rather than -0.5 .. 0.5.
 * The conversion equations to be implemented are therefore
 *      R = Y                + 1.40200 * Cr
 *      G = Y - 0.34414 * Cb - 0.71414 * Cr
 *      B = Y + 1.77200 * Cb
 *
 ******************************************************************************/
static unsigned char clamp(int i)
{
  if (i<0)
    return 0;
  else if (i>255)
    return 255;
  else
    return i;
}

/**
 *  YCrCb -> RGBA32 (1x1)
 *  .---.
 *  | 1 |
 *  `---'
 */
static void YCrCB_to_RGBA32_1x1(struct jdec_private *priv)
{
  const unsigned char *Y, *Cb, *Cr;
  unsigned char *p;
  int i,j;
  int offset_to_next_row;

#define SCALEBITS       10
#define ONE_HALF        (1UL << (SCALEBITS-1))
#define FIX(x)          ((int)((x) * (1UL<<SCALEBITS) + 0.5))

  p = priv->plane[0];
  Y = priv->Y;
  Cb = priv->Cb;
  Cr = priv->Cr;
  offset_to_next_row = priv->bytes_per_row[0] - 8*4;
  for (i=0; i<8; i++) {

    for (j=0;j<8;j++) {

       int y, cb, cr;
       int add_r, add_g, add_b;
       int r, g , b, a;

       y  = (*Y++) << SCALEBITS;
       cb = *Cb++ - 128;
       cr = *Cr++ - 128;
       add_r = FIX(1.40200) * cr + ONE_HALF;
       add_g = - FIX(0.34414) * cb - FIX(0.71414) * cr + ONE_HALF;
       add_b = FIX(1.77200) * cb + ONE_HALF;

       r = (y + add_r) >> SCALEBITS;
       *p++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p++ = clamp(b);
       a = 255;
       *p++ = a;
    }

    p += offset_to_next_row;
  }

#undef SCALEBITS
#undef ONE_HALF
#undef FIX

}

/**
 *  YCrCb -> RGBA32 (2x1)
 *  .-------.
 *  | 1 | 2 |
 *  `-------'
 */
static void YCrCB_to_RGBA32_2x1(struct jdec_private *priv)
{
  const unsigned char *Y, *Cb, *Cr;
  unsigned char *p;
  int i,j;
  int offset_to_next_row;

#define SCALEBITS       10
#define ONE_HALF        (1UL << (SCALEBITS-1))
#define FIX(x)          ((int)((x) * (1UL<<SCALEBITS) + 0.5))

  p = priv->plane[0];
  Y = priv->Y;
  Cb = priv->Cb;
  Cr = priv->Cr;
  offset_to_next_row = priv->bytes_per_row[0] - 16*4;
  for (i=0; i<8; i++) {

    for (j=0; j<8; j++) {

       int y, cb, cr;
       int add_r, add_g, add_b;
       int r, g , b, a;

       y  = (*Y++) << SCALEBITS;
       cb = *Cb++ - 128;
       cr = *Cr++ - 128;
       add_r = FIX(1.40200) * cr + ONE_HALF;
       add_g = - FIX(0.34414) * cb - FIX(0.71414) * cr + ONE_HALF;
       add_b = FIX(1.77200) * cb + ONE_HALF;

       r = (y + add_r) >> SCALEBITS;
       *p++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p++ = clamp(b);
       a = 255;
       *p++ = a;

       y  = (*Y++) << SCALEBITS;
       r = (y + add_r) >> SCALEBITS;
       *p++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p++ = clamp(b);
       a = 255;
       *p++ = a;
    }

    p += offset_to_next_row;
  }

#undef SCALEBITS
#undef ONE_HALF
#undef FIX

}


/**
 *  YCrCb -> RGBA32 (1x2)
 *  .---.
 *  | 1 |
 *  |---|
 *  | 2 |
 *  `---'
 */
static void YCrCB_to_RGBA32_1x2(struct jdec_private *priv)
{
  const unsigned char *Y, *Cb, *Cr;
  unsigned char *p, *p2;
  int i,j;
  int offset_to_next_row;

#define SCALEBITS       10
#define ONE_HALF        (1UL << (SCALEBITS-1))
#define FIX(x)          ((int)((x) * (1UL<<SCALEBITS) + 0.5))

  p = priv->plane[0];
  p2 = priv->plane[0] + priv->bytes_per_row[0];
  Y = priv->Y;
  Cb = priv->Cb;
  Cr = priv->Cr;
  offset_to_next_row = 2*priv->bytes_per_row[0] - 8*4;
  for (i=0; i<8; i++) {

    for (j=0; j<8; j++) {

       int y, cb, cr;
       int add_r, add_g, add_b;
       int r, g , b, a;

       cb = *Cb++ - 128;
       cr = *Cr++ - 128;
       add_r = FIX(1.40200) * cr + ONE_HALF;
       add_g = - FIX(0.34414) * cb - FIX(0.71414) * cr + ONE_HALF;
       add_b = FIX(1.77200) * cb + ONE_HALF;

       y  = (*Y++) << SCALEBITS;
       r = (y + add_r) >> SCALEBITS;
       *p++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p++ = clamp(b);
       a = 255;
       *p++ = a;

       y  = (Y[8-1]) << SCALEBITS;
       r = (y + add_r) >> SCALEBITS;
       *p2++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p2++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p2++ = clamp(b);
       a = 255;
       *p2++ = a;

    }
    Y += 8;
    p += offset_to_next_row;
    p2 += offset_to_next_row;
  }

#undef SCALEBITS
#undef ONE_HALF
#undef FIX

}

/**
 *  YCrCb -> RGBA32 (2x2)
 *  .-------.
 *  | 1 | 2 |
 *  |---+---|
 *  | 3 | 4 |
 *  `-------'
 */
static void YCrCB_to_RGBA32_2x2(struct jdec_private *priv)
{
  const unsigned char *Y, *Cb, *Cr;
  unsigned char *p, *p2;
  int i,j;
  int offset_to_next_row;

#define SCALEBITS       10
#define ONE_HALF        (1UL << (SCALEBITS-1))
#define FIX(x)          ((int)((x) * (1UL<<SCALEBITS) + 0.5))

  p = priv->plane[0];
  p2 = priv->plane[0] + priv->bytes_per_row[0];
  Y = priv->Y;
  Cb = priv->Cb;
  Cr = priv->Cr;
  offset_to_next_row = 2*priv->bytes_per_row[0] - 16*4;
  for (i=0; i<8; i++) {

    for (j=0;j<8;j++) {

       int y, cb, cr;
       int add_r, add_g, add_b;
       int r, g , b, a;

       cb = *Cb++ - 128;
       cr = *Cr++ - 128;
       add_r = FIX(1.40200) * cr + ONE_HALF;
       add_g = - FIX(0.34414) * cb - FIX(0.71414) * cr + ONE_HALF;
       add_b = FIX(1.77200) * cb + ONE_HALF;

       y  = (*Y++) << SCALEBITS;
       r = (y + add_r) >> SCALEBITS;
       *p++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p++ = clamp(b);
       a = 255;
       *p++ = a;

       y  = (*Y++) << SCALEBITS;
       r = (y + add_r) >> SCALEBITS;
       *p++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p++ = clamp(b);
       a = 255;
       *p++ = a;

       y  = (Y[16-2]) << SCALEBITS;
       r = (y + add_r) >> SCALEBITS;
       *p2++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p2++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p2++ = clamp(b);
       a = 255;
       *p2++ = a;

       y  = (Y[16-1]) << SCALEBITS;
       r = (y + add_r) >> SCALEBITS;
       *p2++ = clamp(r);
       g = (y + add_g) >> SCALEBITS;
       *p2++ = clamp(g);
       b = (y + add_b) >> SCALEBITS;
       *p2++ = clamp(b);
       a = 255;
       *p2++ = a;
    }
    Y  += 16;
    p  += offset_to_next_row;
    p2 += offset_to_next_row;
  }

#undef SCALEBITS
#undef ONE_HALF
#undef FIX

}

static int initialize_rgba32(struct jdec_private *priv,
			     unsigned int *bytes_per_blocklines,
			     unsigned int *bytes_per_mcu)
{
  if (priv->components[0] == NULL)
    priv->components[0] = (uint8_t *)malloc(priv->width * priv->height * 4);
  if (!priv->bytes_per_row[0])
    priv->bytes_per_row[0] = priv->width * 4;

  bytes_per_blocklines[0] = priv->bytes_per_row[0];
  bytes_per_mcu[0] = 4*8;

  return !priv->components[0];
}

static const struct tinyjpeg_colorspace format_rgba32 =
  {
    {
      YCrCB_to_RGBA32_1x1,
      YCrCB_to_RGBA32_1x2,
      YCrCB_to_RGBA32_2x1,
      YCrCB_to_RGBA32_2x2,
    },
    tinyjpeg_decode_mcu_3comp_table,
    initialize_rgba32
  };

const tinyjpeg_colorspace_t TINYJPEG_FMT_RGBA32 = &format_rgba32;
