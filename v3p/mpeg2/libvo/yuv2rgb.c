/*
 * yuv2rgb.c
 * Copyright (C) 2000-2002 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 * See http://libmpeg2.sourceforge.net/ for updates.
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "mm_accel.h"
#include "video_out.h"
#include "video_out_internal.h"

uint32_t matrix_coefficients = 6;

const int32_t Inverse_Table_6_9[8][4] = {
    {117504, 138453, 13954, 34903}, /* no sequence_display_extension */
    {117504, 138453, 13954, 34903}, /* ITU-R Rec. 709 (1990) */
    {104597, 132201, 25675, 53279}, /* unspecified */
    {104597, 132201, 25675, 53279}, /* reserved */
    {104448, 132798, 24759, 53109}, /* FCC */
    {104597, 132201, 25675, 53279}, /* ITU-R Rec. 624-4 System B, G */
    {104597, 132201, 25675, 53279}, /* SMPTE 170M */
    {117579, 136230, 16907, 35559}  /* SMPTE 240M (1987) */
};

void (* yuv2rgb) (uint8_t * image, uint8_t * py, uint8_t * pu, uint8_t * pv,
                  int h_size, int v_size,
                  int rgb_stride, int y_stride, int uv_stride);

static void (* yuv2rgb_c_internal) (uint8_t *, uint8_t *,
                                    uint8_t *, uint8_t *,
                                    void *, void *, int);

static void yuv2rgb_c (uint8_t * dst, uint8_t * py,
                       uint8_t * pu, uint8_t * pv,
                       int width, int height,
                       int rgb_stride, int y_stride, int uv_stride)
{
    height >>= 1;
    do {
        yuv2rgb_c_internal (py, py + y_stride, pu, pv,
                            dst, dst + rgb_stride, width);

        py += 2 * y_stride;
        pu += uv_stride;
        pv += uv_stride;
        dst += 2 * rgb_stride;
    } while (--height);
}

void * table_rV[256];
void * table_gU[256];
int table_gV[256];
void * table_bU[256];

#define RGB(i)                                                  \
        U = pu[i];                                              \
        V = pv[i];                                              \
        r = table_rV[V];                                        \
        g = (void *) (((uint8_t *)table_gU[U]) + table_gV[V]);  \
        b = table_bU[U];

#define DST1(i)                                 \
        Y = py_1[2*i];                          \
        dst_1[2*i] = r[Y] + g[Y] + b[Y];        \
        Y = py_1[2*i+1];                        \
        dst_1[2*i+1] = r[Y] + g[Y] + b[Y];

#define DST2(i)                                 \
        Y = py_2[2*i];                          \
        dst_2[2*i] = r[Y] + g[Y] + b[Y];        \
        Y = py_2[2*i+1];                        \
        dst_2[2*i+1] = r[Y] + g[Y] + b[Y];

#define DST1RGB(i)                                                      \
        Y = py_1[2*i];                                                  \
        dst_1[6*i] = r[Y]; dst_1[6*i+1] = g[Y]; dst_1[6*i+2] = b[Y];    \
        Y = py_1[2*i+1];                                                \
        dst_1[6*i+3] = r[Y]; dst_1[6*i+4] = g[Y]; dst_1[6*i+5] = b[Y];

#define DST2RGB(i)                                                      \
        Y = py_2[2*i];                                                  \
        dst_2[6*i] = r[Y]; dst_2[6*i+1] = g[Y]; dst_2[6*i+2] = b[Y];    \
        Y = py_2[2*i+1];                                                \
        dst_2[6*i+3] = r[Y]; dst_2[6*i+4] = g[Y]; dst_2[6*i+5] = b[Y];

#define DST1BGR(i)                                                      \
        Y = py_1[2*i];                                                  \
        dst_1[6*i] = b[Y]; dst_1[6*i+1] = g[Y]; dst_1[6*i+2] = r[Y];    \
        Y = py_1[2*i+1];                                                \
        dst_1[6*i+3] = b[Y]; dst_1[6*i+4] = g[Y]; dst_1[6*i+5] = r[Y];

#define DST2BGR(i)                                                      \
        Y = py_2[2*i];                                                  \
        dst_2[6*i] = b[Y]; dst_2[6*i+1] = g[Y]; dst_2[6*i+2] = r[Y];    \
        Y = py_2[2*i+1];                                                \
        dst_2[6*i+3] = b[Y]; dst_2[6*i+4] = g[Y]; dst_2[6*i+5] = r[Y];

static void yuv2rgb_c_32 (uint8_t * py_1, uint8_t * py_2,
                          uint8_t * pu, uint8_t * pv,
                          void * dst_1_, void * dst_2_, int width)
{
    int U, V, Y;
    uint32_t * r, * g, * b;
    uint32_t * dst_1, * dst_2;

    width >>= 3;
    dst_1 = dst_1_;
    dst_2 = dst_2_;

    do {
        RGB(0);
        DST1(0);
        DST2(0);

        RGB(1);
        DST2(1);
        DST1(1);

        RGB(2);
        DST1(2);
        DST2(2);

        RGB(3);
        DST2(3);
        DST1(3);

        pu += 4;
        pv += 4;
        py_1 += 8;
        py_2 += 8;
        dst_1 += 8;
        dst_2 += 8;
    } while (--width);
}

/* This is very near from the yuv2rgb_c_32 code */
static void yuv2rgb_c_24_rgb (uint8_t * py_1, uint8_t * py_2,
                              uint8_t * pu, uint8_t * pv,
                              void * dst_1_, void * dst_2_, int width)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    width >>= 3;
    dst_1 = dst_1_;
    dst_2 = dst_2_;

    do {
        RGB(0);
        DST1RGB(0);
        DST2RGB(0);

        RGB(1);
        DST2RGB(1);
        DST1RGB(1);

        RGB(2);
        DST1RGB(2);
        DST2RGB(2);

        RGB(3);
        DST2RGB(3);
        DST1RGB(3);

        pu += 4;
        pv += 4;
        py_1 += 8;
        py_2 += 8;
        dst_1 += 24;
        dst_2 += 24;
    } while (--width);
}

/* only trivial mods from yuv2rgb_c_24_rgb */
static void yuv2rgb_c_24_bgr (uint8_t * py_1, uint8_t * py_2,
                              uint8_t * pu, uint8_t * pv,
                              void * dst_1_, void * dst_2_, int width)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    width >>= 3;
    dst_1 = dst_1_;
    dst_2 = dst_2_;

    do {
        RGB(0);
        DST1BGR(0);
        DST2BGR(0);

        RGB(1);
        DST2BGR(1);
        DST1BGR(1);

        RGB(2);
        DST1BGR(2);
        DST2BGR(2);

        RGB(3);
        DST2BGR(3);
        DST1BGR(3);

        pu += 4;
        pv += 4;
        py_1 += 8;
        py_2 += 8;
        dst_1 += 24;
        dst_2 += 24;
    } while (--width);
}

/* This is exactly the same code as yuv2rgb_c_32 except for the types of */
/* r, g, b, dst_1, dst_2 */
static void yuv2rgb_c_16 (uint8_t * py_1, uint8_t * py_2,
                          uint8_t * pu, uint8_t * pv,
                          void * dst_1_, void * dst_2_, int width)
{
    int U, V, Y;
    uint16_t * r, * g, * b;
    uint16_t * dst_1, * dst_2;

    width >>= 3;
    dst_1 = dst_1_;
    dst_2 = dst_2_;

    do {
        RGB(0);
        DST1(0);
        DST2(0);

        RGB(1);
        DST2(1);
        DST1(1);

        RGB(2);
        DST1(2);
        DST2(2);

        RGB(3);
        DST2(3);
        DST1(3);

        pu += 4;
        pv += 4;
        py_1 += 8;
        py_2 += 8;
        dst_1 += 8;
        dst_2 += 8;
    } while (--width);
}

static int div_round (int dividend, int divisor)
{
    if (dividend > 0)
        return (dividend + (divisor>>1)) / divisor;
    else
        return -((-dividend + (divisor>>1)) / divisor);
}

static void yuv2rgb_c_init (int bpp, int mode)
{
    int i;
    uint8_t table_Y[1024];
    uint32_t * table_32 = 0;
    uint16_t * table_16 = 0;
    uint8_t * table_8 = 0;
    int entry_size = 0;
    void *table_r = 0, *table_g = 0, *table_b = 0;

    int crv = Inverse_Table_6_9[matrix_coefficients][0];
    int cbu = Inverse_Table_6_9[matrix_coefficients][1];
    int cgu = -Inverse_Table_6_9[matrix_coefficients][2];
    int cgv = -Inverse_Table_6_9[matrix_coefficients][3];

    for (i = 0; i < 1024; i++) {
        int j;

        j = (76309 * (i - 384 - 16) + 32768) >> 16;
        j = (j < 0) ? 0 : ((j > 255) ? 255 : j);
        table_Y[i] = j;
    }

    switch (bpp) {
    case 32:
        yuv2rgb_c_internal = yuv2rgb_c_32;

        table_32 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint32_t));

        entry_size = sizeof (uint32_t);
        table_r = table_32 + 197;
        table_b = table_32 + 197 + 685;
        table_g = table_32 + 197 + 2*682;

        for (i = -197; i < 256+197; i++)
            ((uint32_t *) table_r)[i] =
                table_Y[i+384] << ((mode==MODE_RGB) ? 16 : 0);
        for (i = -132; i < 256+132; i++)
            ((uint32_t *) table_g)[i] = table_Y[i+384] << 8;
        for (i = -232; i < 256+232; i++)
            ((uint32_t *) table_b)[i] =
                table_Y[i+384] << ((mode==MODE_RGB) ? 0 : 16);
        break;

    case 24:
        yuv2rgb_c_internal = (mode==MODE_RGB) ? yuv2rgb_c_24_rgb : yuv2rgb_c_24_bgr;

        table_8 = malloc ((256 + 2*232) * sizeof (uint8_t));

        entry_size = sizeof (uint8_t);
        table_r = table_g = table_b = table_8 + 232;

        for (i = -232; i < 256+232; i++)
            ((uint8_t * )table_b)[i] = table_Y[i+384];
        break;

    case 15:
    case 16:
        yuv2rgb_c_internal = yuv2rgb_c_16;

        table_16 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint16_t));

        entry_size = sizeof (uint16_t);
        table_r = table_16 + 197;
        table_b = table_16 + 197 + 685;
        table_g = table_16 + 197 + 2*682;

        for (i = -197; i < 256+197; i++) {
            int j = table_Y[i+384] >> 3;

            if (mode == MODE_RGB)
                j <<= ((bpp==16) ? 11 : 10);

            ((uint16_t *)table_r)[i] = j;
        }
        for (i = -132; i < 256+132; i++) {
            int j = table_Y[i+384] >> ((bpp==16) ? 2 : 3);

            ((uint16_t *)table_g)[i] = j << 5;
        }
        for (i = -232; i < 256+232; i++) {
            int j = table_Y[i+384] >> 3;

            if (mode == MODE_BGR)
                j <<= ((bpp==16) ? 11 : 10);

            ((uint16_t *)table_b)[i] = j;
        }
        break;

    default:
        fprintf (stderr, "%ibpp not supported by yuv2rgb\n", bpp);
        exit (1);
    }

    for (i = 0; i < 256; i++) {
        table_rV[i] = (((uint8_t *)table_r) +
                       entry_size * div_round (crv * (i-128), 76309));
        table_gU[i] = (((uint8_t *)table_g) +
                       entry_size * div_round (cgu * (i-128), 76309));
        table_gV[i] = entry_size * div_round (cgv * (i-128), 76309);
        table_bU[i] = (((uint8_t *)table_b) +
                       entry_size * div_round (cbu * (i-128), 76309));
    }

    yuv2rgb = yuv2rgb_c;
}

void yuv2rgb_init (int bpp, int mode)
{
    yuv2rgb = NULL;
#ifdef ARCH_X86
    if ((yuv2rgb == NULL) && (vo_mm_accel & MM_ACCEL_X86_MMXEXT)) {
        if (! yuv2rgb_init_mmxext (bpp, mode))
            fprintf (stderr, "Using MMXEXT for colorspace transform\n");
    }
    if ((yuv2rgb == NULL) && (vo_mm_accel & MM_ACCEL_X86_MMX)) {
        if (! yuv2rgb_init_mmx (bpp, mode))
            fprintf (stderr, "Using MMX for colorspace transform\n");
    }
#endif
#ifdef LIBVO_MLIB
    if ((yuv2rgb == NULL) && (vo_mm_accel & MM_ACCEL_MLIB)) {
        if (! yuv2rgb_init_mlib (bpp, mode))
            fprintf (stderr, "Using mlib for colorspace transform\n");
    }
#endif
    if (yuv2rgb == NULL) {
        fprintf (stderr, "No accelerated colorspace conversion found\n");
        yuv2rgb_c_init (bpp, mode);
    }
}
