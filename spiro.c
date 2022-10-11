/*
ppedit - A pattern plate editor for Spiro splines.
Copyright (C) 2007... Raph Levien
Copyright (C) 2013... Joe Da Silva (improvements plus 'a','h')

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.

*/
/* C implementation of third-order polynomial spirals. */

#include <math.h>
#ifdef HAVE_FINITE
#include <float.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "spiroentrypoints.h"
#include "bezctx_intf.h"
#include "spiro.h"

#include "spiro-config.h"
#ifdef VERBOSE
#include <stdio.h>
#endif

typedef struct {
    double a[11];	/* band-diagonal matrix */
    double al[5];	/* lower part of band-diagonal decomposition */
} bandmat;

#ifndef M_PI
#define M_PI		3.14159265358979323846	/* pi */
#endif

#ifndef N_IS
/* int n = 4; */
#define N_IS		4
#endif

#ifndef ORDER
#define ORDER 12
#endif

/* Integrate polynomial spiral curve over range -.5 .. .5. */
static void
integrate_spiro(const double ks[4], double xy[2], int n)
{
#if 0
    int n = 1024;
#endif
    double th1 = ks[0];
    double th2 = .5 * ks[1];
    double th3 = (1./6) * ks[2];
    double th4 = (1./24) * ks[3];
    double x, y;
    double ds = 1. / n;
    double ds2 = ds * ds;
    double ds3 = ds2 * ds;
    double k0 = ks[0] * ds;
    double k1 = ks[1] * ds;
    double k2 = ks[2] * ds;
    double k3 = ks[3] * ds;
    int i;
    double s = .5 * ds - .5;

    x = 0;
    y = 0;

    for (i = 0; i < n; i++) {

#if ORDER > 2
	double u, v;
	double km0, km1, km2, km3;

	if (n == 1) {
	    km0 = k0;
	    km1 = k1 * ds;
	    km2 = k2 * ds2;
	} else {
	    km0 = (((1./6) * k3 * s + .5 * k2) * s + k1) * s + k0;
	    km1 = ((.5 * k3 * s + k2) * s + k1) * ds;
	    km2 = (k3 * s + k2) * ds2;
	}
	km3 = k3 * ds3;
#endif

	{

#if ORDER == 4
	double km0_2 = km0 * km0;
	u = 24 - km0_2;
	v = km1;
#endif

#if ORDER == 6
	double km0_2 = km0 * km0;
	double km0_4 = km0_2 * km0_2;
	u = 24 - km0_2 + (km0_4 - 4 * km0 * km2 - 3 * km1 * km1) * (1./80);
	v = km1 + (km3 - 6 * km0_2 * km1) * (1./80);
#endif

#if ORDER == 8
	double t1_1 = km0;
	double t1_2 = .5 * km1;
	double t1_3 = (1./6) * km2;
	double t1_4 = (1./24) * km3;
	double t2_2 = t1_1 * t1_1;
	double t2_3 = 2 * (t1_1 * t1_2);
	double t2_4 = 2 * (t1_1 * t1_3) + t1_2 * t1_2;
	double t2_5 = 2 * (t1_1 * t1_4 + t1_2 * t1_3);
	double t2_6 = 2 * (t1_2 * t1_4) + t1_3 * t1_3;
	double t3_4 = t2_2 * t1_2 + t2_3 * t1_1;
	double t3_6 = t2_2 * t1_4 + t2_3 * t1_3 + t2_4 * t1_2 + t2_5 * t1_1;
	double t4_4 = t2_2 * t2_2;
	double t4_5 = 2 * (t2_2 * t2_3);
	double t4_6 = 2 * (t2_2 * t2_4) + t2_3 * t2_3;
	double t5_6 = t4_4 * t1_2 + t4_5 * t1_1;
	double t6_6 = t4_4 * t2_2;
	u = 1;
	v = 0;
	v += (1./12) * t1_2 + (1./80) * t1_4;
	u -= (1./24) * t2_2 + (1./160) * t2_4 + (1./896) * t2_6;
	v -= (1./480) * t3_4 + (1./2688) * t3_6;
	u += (1./1920) * t4_4 + (1./10752) * t4_6;
	v += (1./53760) * t5_6;
	u -= (1./322560) * t6_6;
#endif

#if ORDER == 10
	double t1_1 = km0;
	double t1_2 = .5 * km1;
	double t1_3 = (1./6) * km2;
	double t1_4 = (1./24) * km3;
	double t2_2 = t1_1 * t1_1;
	double t2_3 = 2 * (t1_1 * t1_2);
	double t2_4 = 2 * (t1_1 * t1_3) + t1_2 * t1_2;
	double t2_5 = 2 * (t1_1 * t1_4 + t1_2 * t1_3);
	double t2_6 = 2 * (t1_2 * t1_4) + t1_3 * t1_3;
	double t2_7 = 2 * (t1_3 * t1_4);
	double t2_8 = t1_4 * t1_4;
	double t3_4 = t2_2 * t1_2 + t2_3 * t1_1;
	double t3_6 = t2_2 * t1_4 + t2_3 * t1_3 + t2_4 * t1_2 + t2_5 * t1_1;
	double t3_8 = t2_4 * t1_4 + t2_5 * t1_3 + t2_6 * t1_2 + t2_7 * t1_1;
	double t4_4 = t2_2 * t2_2;
	double t4_5 = 2 * (t2_2 * t2_3);
	double t4_6 = 2 * (t2_2 * t2_4) + t2_3 * t2_3;
	double t4_7 = 2 * (t2_2 * t2_5 + t2_3 * t2_4);
	double t4_8 = 2 * (t2_2 * t2_6 + t2_3 * t2_5) + t2_4 * t2_4;
	double t5_6 = t4_4 * t1_2 + t4_5 * t1_1;
	double t5_8 = t4_4 * t1_4 + t4_5 * t1_3 + t4_6 * t1_2 + t4_7 * t1_1;
	double t6_6 = t4_4 * t2_2;
	double t6_7 = t4_4 * t2_3 + t4_5 * t2_2;
	double t6_8 = t4_4 * t2_4 + t4_5 * t2_3 + t4_6 * t2_2;
	double t7_8 = t6_6 * t1_2 + t6_7 * t1_1;
	double t8_8 = t6_6 * t2_2;
	u = 1;
	v = 0;
	v += (1./12) * t1_2 + (1./80) * t1_4;
	u -= (1./24) * t2_2 + (1./160) * t2_4 + (1./896) * t2_6 + (1./4608) * t2_8;
	v -= (1./480) * t3_4 + (1./2688) * t3_6 + (1./13824) * t3_8;
	u += (1./1920) * t4_4 + (1./10752) * t4_6 + (1./55296) * t4_8;
	v += (1./53760) * t5_6 + (1./276480) * t5_8;
	u -= (1./322560) * t6_6 + (1./1.65888e+06) * t6_8;
	v -= (1./1.16122e+07) * t7_8;
	u += (1./9.28973e+07) * t8_8;
#endif

#if ORDER == 12
	double t1_1 = km0;
	double t1_2 = .5 * km1;
	double t1_3 = (1./6) * km2;
	double t1_4 = (1./24) * km3;
	double t2_2 = t1_1 * t1_1;
	double t2_3 = 2 * (t1_1 * t1_2);
	double t2_4 = 2 * (t1_1 * t1_3) + t1_2 * t1_2;
	double t2_5 = 2 * (t1_1 * t1_4 + t1_2 * t1_3);
	double t2_6 = 2 * (t1_2 * t1_4) + t1_3 * t1_3;
	double t2_7 = 2 * (t1_3 * t1_4);
	double t2_8 = t1_4 * t1_4;
	double t3_4 = t2_2 * t1_2 + t2_3 * t1_1;
	double t3_6 = t2_2 * t1_4 + t2_3 * t1_3 + t2_4 * t1_2 + t2_5 * t1_1;
	double t3_8 = t2_4 * t1_4 + t2_5 * t1_3 + t2_6 * t1_2 + t2_7 * t1_1;
	double t3_10 = t2_6 * t1_4 + t2_7 * t1_3 + t2_8 * t1_2;
	double t4_4 = t2_2 * t2_2;
	double t4_5 = 2 * (t2_2 * t2_3);
	double t4_6 = 2 * (t2_2 * t2_4) + t2_3 * t2_3;
	double t4_7 = 2 * (t2_2 * t2_5 + t2_3 * t2_4);
	double t4_8 = 2 * (t2_2 * t2_6 + t2_3 * t2_5) + t2_4 * t2_4;
	double t4_9 = 2 * (t2_2 * t2_7 + t2_3 * t2_6 + t2_4 * t2_5);
	double t4_10 = 2 * (t2_2 * t2_8 + t2_3 * t2_7 + t2_4 * t2_6) + t2_5 * t2_5;
	double t5_6 = t4_4 * t1_2 + t4_5 * t1_1;
	double t5_8 = t4_4 * t1_4 + t4_5 * t1_3 + t4_6 * t1_2 + t4_7 * t1_1;
	double t5_10 = t4_6 * t1_4 + t4_7 * t1_3 + t4_8 * t1_2 + t4_9 * t1_1;
	double t6_6 = t4_4 * t2_2;
	double t6_7 = t4_4 * t2_3 + t4_5 * t2_2;
	double t6_8 = t4_4 * t2_4 + t4_5 * t2_3 + t4_6 * t2_2;
	double t6_9 = t4_4 * t2_5 + t4_5 * t2_4 + t4_6 * t2_3 + t4_7 * t2_2;
	double t6_10 = t4_4 * t2_6 + t4_5 * t2_5 + t4_6 * t2_4 + t4_7 * t2_3 + t4_8 * t2_2;
	double t7_8 = t6_6 * t1_2 + t6_7 * t1_1;
	double t7_10 = t6_6 * t1_4 + t6_7 * t1_3 + t6_8 * t1_2 + t6_9 * t1_1;
	double t8_8 = t6_6 * t2_2;
	double t8_9 = t6_6 * t2_3 + t6_7 * t2_2;
	double t8_10 = t6_6 * t2_4 + t6_7 * t2_3 + t6_8 * t2_2;
	double t9_10 = t8_8 * t1_2 + t8_9 * t1_1;
	double t10_10 = t8_8 * t2_2;
	u = 1;
	v = 0;
	v += (1./12) * t1_2 + (1./80) * t1_4;
	u -= (1./24) * t2_2 + (1./160) * t2_4 + (1./896) * t2_6 + (1./4608) * t2_8;
	v -= (1./480) * t3_4 + (1./2688) * t3_6 + (1./13824) * t3_8 + (1./67584) * t3_10;
	u += (1./1920) * t4_4 + (1./10752) * t4_6 + (1./55296) * t4_8 + (1./270336) * t4_10;
	v += (1./53760) * t5_6 + (1./276480) * t5_8 + (1./1.35168e+06) * t5_10;
	u -= (1./322560) * t6_6 + (1./1.65888e+06) * t6_8 + (1./8.11008e+06) * t6_10;
	v -= (1./1.16122e+07) * t7_8 + (1./5.67706e+07) * t7_10;
	u += (1./9.28973e+07) * t8_8 + (1./4.54164e+08) * t8_10;
	v += (1./4.08748e+09) * t9_10;
	u -= (1./4.08748e+10) * t10_10;
#endif

#if ORDER == 14
	double t1_1 = km0;
	double t1_2 = .5 * km1;
	double t1_3 = (1./6) * km2;
	double t1_4 = (1./24) * km3;
	double t2_2 = t1_1 * t1_1;
	double t2_3 = 2 * (t1_1 * t1_2);
	double t2_4 = 2 * (t1_1 * t1_3) + t1_2 * t1_2;
	double t2_5 = 2 * (t1_1 * t1_4 + t1_2 * t1_3);
	double t2_6 = 2 * (t1_2 * t1_4) + t1_3 * t1_3;
	double t2_7 = 2 * (t1_3 * t1_4);
	double t2_8 = t1_4 * t1_4;
	double t3_4 = t2_2 * t1_2 + t2_3 * t1_1;
	double t3_6 = t2_2 * t1_4 + t2_3 * t1_3 + t2_4 * t1_2 + t2_5 * t1_1;
	double t3_8 = t2_4 * t1_4 + t2_5 * t1_3 + t2_6 * t1_2 + t2_7 * t1_1;
	double t3_10 = t2_6 * t1_4 + t2_7 * t1_3 + t2_8 * t1_2;
	double t3_12 = t2_8 * t1_4;
	double t4_4 = t2_2 * t2_2;
	double t4_5 = 2 * (t2_2 * t2_3);
	double t4_6 = 2 * (t2_2 * t2_4) + t2_3 * t2_3;
	double t4_7 = 2 * (t2_2 * t2_5 + t2_3 * t2_4);
	double t4_8 = 2 * (t2_2 * t2_6 + t2_3 * t2_5) + t2_4 * t2_4;
	double t4_9 = 2 * (t2_2 * t2_7 + t2_3 * t2_6 + t2_4 * t2_5);
	double t4_10 = 2 * (t2_2 * t2_8 + t2_3 * t2_7 + t2_4 * t2_6) + t2_5 * t2_5;
	double t4_11 = 2 * (t2_3 * t2_8 + t2_4 * t2_7 + t2_5 * t2_6);
	double t4_12 = 2 * (t2_4 * t2_8 + t2_5 * t2_7) + t2_6 * t2_6;
	double t5_6 = t4_4 * t1_2 + t4_5 * t1_1;
	double t5_8 = t4_4 * t1_4 + t4_5 * t1_3 + t4_6 * t1_2 + t4_7 * t1_1;
	double t5_10 = t4_6 * t1_4 + t4_7 * t1_3 + t4_8 * t1_2 + t4_9 * t1_1;
	double t5_12 = t4_8 * t1_4 + t4_9 * t1_3 + t4_10 * t1_2 + t4_11 * t1_1;
	double t6_6 = t4_4 * t2_2;
	double t6_7 = t4_4 * t2_3 + t4_5 * t2_2;
	double t6_8 = t4_4 * t2_4 + t4_5 * t2_3 + t4_6 * t2_2;
	double t6_9 = t4_4 * t2_5 + t4_5 * t2_4 + t4_6 * t2_3 + t4_7 * t2_2;
	double t6_10 = t4_4 * t2_6 + t4_5 * t2_5 + t4_6 * t2_4 + t4_7 * t2_3 + t4_8 * t2_2;
	double t6_11 = t4_4 * t2_7 + t4_5 * t2_6 + t4_6 * t2_5 + t4_7 * t2_4 + t4_8 * t2_3 + t4_9 * t2_2;
	double t6_12 = t4_4 * t2_8 + t4_5 * t2_7 + t4_6 * t2_6 + t4_7 * t2_5 + t4_8 * t2_4 + t4_9 * t2_3 + t4_10 * t2_2;
	double t7_8 = t6_6 * t1_2 + t6_7 * t1_1;
	double t7_10 = t6_6 * t1_4 + t6_7 * t1_3 + t6_8 * t1_2 + t6_9 * t1_1;
	double t7_12 = t6_8 * t1_4 + t6_9 * t1_3 + t6_10 * t1_2 + t6_11 * t1_1;
	double t8_8 = t6_6 * t2_2;
	double t8_9 = t6_6 * t2_3 + t6_7 * t2_2;
	double t8_10 = t6_6 * t2_4 + t6_7 * t2_3 + t6_8 * t2_2;
	double t8_11 = t6_6 * t2_5 + t6_7 * t2_4 + t6_8 * t2_3 + t6_9 * t2_2;
	double t8_12 = t6_6 * t2_6 + t6_7 * t2_5 + t6_8 * t2_4 + t6_9 * t2_3 + t6_10 * t2_2;
	double t9_10 = t8_8 * t1_2 + t8_9 * t1_1;
	double t9_12 = t8_8 * t1_4 + t8_9 * t1_3 + t8_10 * t1_2 + t8_11 * t1_1;
	double t10_10 = t8_8 * t2_2;
	double t10_11 = t8_8 * t2_3 + t8_9 * t2_2;
	double t10_12 = t8_8 * t2_4 + t8_9 * t2_3 + t8_10 * t2_2;
	double t11_12 = t10_10 * t1_2 + t10_11 * t1_1;
	double t12_12 = t10_10 * t2_2;
	u = 1;
	v = 0;
	v += (1./12) * t1_2 + (1./80) * t1_4;
	u -= (1./24) * t2_2 + (1./160) * t2_4 + (1./896) * t2_6 + (1./4608) * t2_8;
	v -= (1./480) * t3_4 + (1./2688) * t3_6 + (1./13824) * t3_8 + (1./67584) * t3_10 + (1./319488) * t3_12;
	u += (1./1920) * t4_4 + (1./10752) * t4_6 + (1./55296) * t4_8 + (1./270336) * t4_10 + (1./1.27795e+06) * t4_12;
	v += (1./53760) * t5_6 + (1./276480) * t5_8 + (1./1.35168e+06) * t5_10 + (1./6.38976e+06) * t5_12;
	u -= (1./322560) * t6_6 + (1./1.65888e+06) * t6_8 + (1./8.11008e+06) * t6_10 + (1./3.83386e+07) * t6_12;
	v -= (1./1.16122e+07) * t7_8 + (1./5.67706e+07) * t7_10 + (1./2.6837e+08) * t7_12;
	u += (1./9.28973e+07) * t8_8 + (1./4.54164e+08) * t8_10 + (1./2.14696e+09) * t8_12;
	v += (1./4.08748e+09) * t9_10 + (1./1.93226e+10) * t9_12;
	u -= (1./4.08748e+10) * t10_10 + (1./1.93226e+11) * t10_12;
	v -= (1./2.12549e+12) * t11_12;
	u += (1./2.55059e+13) * t12_12;
#endif

#if ORDER == 16
	double t1_1 = km0;
	double t1_2 = .5 * km1;
	double t1_3 = (1./6) * km2;
	double t1_4 = (1./24) * km3;
	double t2_2 = t1_1 * t1_1;
	double t2_3 = 2 * (t1_1 * t1_2);
	double t2_4 = 2 * (t1_1 * t1_3) + t1_2 * t1_2;
	double t2_5 = 2 * (t1_1 * t1_4 + t1_2 * t1_3);
	double t2_6 = 2 * (t1_2 * t1_4) + t1_3 * t1_3;
	double t2_7 = 2 * (t1_3 * t1_4);
	double t2_8 = t1_4 * t1_4;
	double t3_4 = t2_2 * t1_2 + t2_3 * t1_1;
	double t3_6 = t2_2 * t1_4 + t2_3 * t1_3 + t2_4 * t1_2 + t2_5 * t1_1;
	double t3_8 = t2_4 * t1_4 + t2_5 * t1_3 + t2_6 * t1_2 + t2_7 * t1_1;
	double t3_10 = t2_6 * t1_4 + t2_7 * t1_3 + t2_8 * t1_2;
	double t3_12 = t2_8 * t1_4;
	double t4_4 = t2_2 * t2_2;
	double t4_5 = 2 * (t2_2 * t2_3);
	double t4_6 = 2 * (t2_2 * t2_4) + t2_3 * t2_3;
	double t4_7 = 2 * (t2_2 * t2_5 + t2_3 * t2_4);
	double t4_8 = 2 * (t2_2 * t2_6 + t2_3 * t2_5) + t2_4 * t2_4;
	double t4_9 = 2 * (t2_2 * t2_7 + t2_3 * t2_6 + t2_4 * t2_5);
	double t4_10 = 2 * (t2_2 * t2_8 + t2_3 * t2_7 + t2_4 * t2_6) + t2_5 * t2_5;
	double t4_11 = 2 * (t2_3 * t2_8 + t2_4 * t2_7 + t2_5 * t2_6);
	double t4_12 = 2 * (t2_4 * t2_8 + t2_5 * t2_7) + t2_6 * t2_6;
	double t4_13 = 2 * (t2_5 * t2_8 + t2_6 * t2_7);
	double t4_14 = 2 * (t2_6 * t2_8) + t2_7 * t2_7;
	double t5_6 = t4_4 * t1_2 + t4_5 * t1_1;
	double t5_8 = t4_4 * t1_4 + t4_5 * t1_3 + t4_6 * t1_2 + t4_7 * t1_1;
	double t5_10 = t4_6 * t1_4 + t4_7 * t1_3 + t4_8 * t1_2 + t4_9 * t1_1;
	double t5_12 = t4_8 * t1_4 + t4_9 * t1_3 + t4_10 * t1_2 + t4_11 * t1_1;
	double t5_14 = t4_10 * t1_4 + t4_11 * t1_3 + t4_12 * t1_2 + t4_13 * t1_1;
	double t6_6 = t4_4 * t2_2;
	double t6_7 = t4_4 * t2_3 + t4_5 * t2_2;
	double t6_8 = t4_4 * t2_4 + t4_5 * t2_3 + t4_6 * t2_2;
	double t6_9 = t4_4 * t2_5 + t4_5 * t2_4 + t4_6 * t2_3 + t4_7 * t2_2;
	double t6_10 = t4_4 * t2_6 + t4_5 * t2_5 + t4_6 * t2_4 + t4_7 * t2_3 + t4_8 * t2_2;
	double t6_11 = t4_4 * t2_7 + t4_5 * t2_6 + t4_6 * t2_5 + t4_7 * t2_4 + t4_8 * t2_3 + t4_9 * t2_2;
	double t6_12 = t4_4 * t2_8 + t4_5 * t2_7 + t4_6 * t2_6 + t4_7 * t2_5 + t4_8 * t2_4 + t4_9 * t2_3 + t4_10 * t2_2;
	double t6_13 = t4_5 * t2_8 + t4_6 * t2_7 + t4_7 * t2_6 + t4_8 * t2_5 + t4_9 * t2_4 + t4_10 * t2_3 + t4_11 * t2_2;
	double t6_14 = t4_6 * t2_8 + t4_7 * t2_7 + t4_8 * t2_6 + t4_9 * t2_5 + t4_10 * t2_4 + t4_11 * t2_3 + t4_12 * t2_2;
	double t7_8 = t6_6 * t1_2 + t6_7 * t1_1;
	double t7_10 = t6_6 * t1_4 + t6_7 * t1_3 + t6_8 * t1_2 + t6_9 * t1_1;
	double t7_12 = t6_8 * t1_4 + t6_9 * t1_3 + t6_10 * t1_2 + t6_11 * t1_1;
	double t7_14 = t6_10 * t1_4 + t6_11 * t1_3 + t6_12 * t1_2 + t6_13 * t1_1;
	double t8_8 = t6_6 * t2_2;
	double t8_9 = t6_6 * t2_3 + t6_7 * t2_2;
	double t8_10 = t6_6 * t2_4 + t6_7 * t2_3 + t6_8 * t2_2;
	double t8_11 = t6_6 * t2_5 + t6_7 * t2_4 + t6_8 * t2_3 + t6_9 * t2_2;
	double t8_12 = t6_6 * t2_6 + t6_7 * t2_5 + t6_8 * t2_4 + t6_9 * t2_3 + t6_10 * t2_2;
	double t8_13 = t6_6 * t2_7 + t6_7 * t2_6 + t6_8 * t2_5 + t6_9 * t2_4 + t6_10 * t2_3 + t6_11 * t2_2;
	double t8_14 = t6_6 * t2_8 + t6_7 * t2_7 + t6_8 * t2_6 + t6_9 * t2_5 + t6_10 * t2_4 + t6_11 * t2_3 + t6_12 * t2_2;
	double t9_10 = t8_8 * t1_2 + t8_9 * t1_1;
	double t9_12 = t8_8 * t1_4 + t8_9 * t1_3 + t8_10 * t1_2 + t8_11 * t1_1;
	double t9_14 = t8_10 * t1_4 + t8_11 * t1_3 + t8_12 * t1_2 + t8_13 * t1_1;
	double t10_10 = t8_8 * t2_2;
	double t10_11 = t8_8 * t2_3 + t8_9 * t2_2;
	double t10_12 = t8_8 * t2_4 + t8_9 * t2_3 + t8_10 * t2_2;
	double t10_13 = t8_8 * t2_5 + t8_9 * t2_4 + t8_10 * t2_3 + t8_11 * t2_2;
	double t10_14 = t8_8 * t2_6 + t8_9 * t2_5 + t8_10 * t2_4 + t8_11 * t2_3 + t8_12 * t2_2;
	double t11_12 = t10_10 * t1_2 + t10_11 * t1_1;
	double t11_14 = t10_10 * t1_4 + t10_11 * t1_3 + t10_12 * t1_2 + t10_13 * t1_1;
	double t12_12 = t10_10 * t2_2;
	double t12_13 = t10_10 * t2_3 + t10_11 * t2_2;
	double t12_14 = t10_10 * t2_4 + t10_11 * t2_3 + t10_12 * t2_2;
	double t13_14 = t12_12 * t1_2 + t12_13 * t1_1;
	double t14_14 = t12_12 * t2_2;
	u = 1;
	u -= 1./24 * t2_2 + 1./160 * t2_4 + 1./896 * t2_6 + 1./4608 * t2_8;
	u += 1./1920 * t4_4 + 1./10752 * t4_6 + 1./55296 * t4_8 + 1./270336 * t4_10 + 1./1277952 * t4_12 + 1./5898240 * t4_14;
	u -= 1./322560 * t6_6 + 1./1658880 * t6_8 + 1./8110080 * t6_10 + 1./38338560 * t6_12 + 1./176947200 * t6_14;
	u += 1./92897280 * t8_8 + 1./454164480 * t8_10 + 4.6577500191e-10 * t8_12 + 1.0091791708e-10 * t8_14;
	u -= 2.4464949595e-11 * t10_10 + 5.1752777990e-12 * t10_12 + 1.1213101898e-12 * t10_14;
	u += 3.9206649992e-14 * t12_12 + 8.4947741650e-15 * t12_14;
	u -= 4.6674583324e-17 * t14_14;
	v = 0;
	v += 1./12 * t1_2 + 1./80 * t1_4;
	v -= 1./480 * t3_4 + 1./2688 * t3_6 + 1./13824 * t3_8 + 1./67584 * t3_10 + 1./319488 * t3_12;
	v += 1./53760 * t5_6 + 1./276480 * t5_8 + 1./1351680 * t5_10 + 1./6389760 * t5_12 + 1./29491200 * t5_14;
	v -= 1./11612160 * t7_8 + 1./56770560 * t7_10 + 1./268369920 * t7_12 + 8.0734333664e-10 * t7_14;
	v += 2.4464949595e-10 * t9_10 + 5.1752777990e-11 * t9_12 + 1.1213101898e-11 * t9_14;
	v -= 4.7047979991e-13 * t11_12 + 1.0193728998e-13 * t11_14;
	v += 6.5344416654e-16 * t13_14;
#endif

	}

	if (n == 1) {
#if ORDER == 2
	    x = 1;
	    y = 0;
#else
	    x = u;
	    y = v;
#endif
	} else {
	    double th = (((th4 * s + th3) * s + th2) * s + th1) * s;
	    double cth = cos(th);
	    double sth = sin(th);

#if ORDER == 2
	    x += cth;
	    y += sth;
#else
	    x += cth * u - sth * v;
	    y += cth * v + sth * u;
#endif
	    s += ds;
	}
    }

#if ORDER == 4 || ORDER == 6
    xy[0] = x * (1./24 * ds);
    xy[1] = y * (1./24 * ds);
#else
    xy[0] = x * ds;
    xy[1] = y * ds;
#endif
}

static void
set_dm_to_1(double *dm)
{
    /* assume spiro within -0.5 to +0.5 */
    /* values loaded: scale, xoff, yoff */
    dm[0] = 1.; dm[1] = dm[2] = 0.;
}

static void
set_di_to_x1y1(double *di, double *dm, double x1, double y1)
{
    /* assume IEEE 754 rounding errors. */
    di[3] = di[4] = x1;
    di[6] = di[7] = y1;
    di[3] -= di[1]; di[4] += di[1];
    di[6] -= di[1]; di[7] += di[1];
    di[2] = x1 * dm[0] + dm[1];
    di[5] = y1 * dm[0] + dm[2];
}

static double
compute_ends(const double ks[4], double ends[2][4], double seg_ch)
{
    double xy[2];
    double ch, th;
    double l, l2, l3;
    double th_even, th_odd;
    double k0_even, k0_odd;
    double k1_even, k1_odd;
    double k2_even, k2_odd;

    integrate_spiro(ks, xy, N_IS);
    ch = hypot(xy[0], xy[1]);
    th = atan2(xy[1], xy[0]);
    l = ch / seg_ch;

    th_even = .5 * ks[0] + (1./48) * ks[2];
    th_odd = .125 * ks[1] + (1./384) * ks[3] - th;
    ends[0][0] = th_even - th_odd;
    ends[1][0] = th_even + th_odd;
    k0_even = l * (ks[0] + .125 * ks[2]);
    k0_odd = l * (.5 * ks[1] + (1./48) * ks[3]);
    ends[0][1] = k0_even - k0_odd;
    ends[1][1] = k0_even + k0_odd;
    l2 = l * l;
    k1_even = l2 * (ks[1] + .125 * ks[3]);
    k1_odd = l2 * .5 * ks[2];
    ends[0][2] = k1_even - k1_odd;
    ends[1][2] = k1_even + k1_odd;
    l3 = l2 * l;
    k2_even = l3 * ks[2];
    k2_odd = l3 * .5 * ks[3];
    ends[0][3] = k2_even - k2_odd;
    ends[1][3] = k2_even + k2_odd;

    return l;
}

static void
compute_pderivs(const spiro_seg *s, double ends[2][4], double derivs[4][2][4],
		int jinc)
{
    double recip_d = 2e6;
    double delta = 1./ recip_d;
    double try_ks[4];
    double try_ends[2][4];
    int i, j, k;

    compute_ends(s->ks, ends, s->seg_ch);
    for (i = 0; i < jinc; i++) {
	for (j = 0; j < 4; j++)
	    try_ks[j] = s->ks[j];
	try_ks[i] += delta;
	compute_ends(try_ks, try_ends, s->seg_ch);
	for (k = 0; k < 2; k++)
	    for (j = 0; j < 4; j++)
		derivs[j][k][i] = recip_d * (try_ends[k][j] - ends[k][j]);
    }
}

static double
mod_2pi(double th)
{
    double u = th / (2 * M_PI);
    return 2 * M_PI * (u - floor(u + 0.5));
}

static spiro_seg *
setup_path0(const spiro_cp *src, double *dm, int n)
{
    int i, ilast, n_seg, z;
    double dx, dy;
    double xmin, xmax, ymin, ymax;
    spiro_seg *r;

    z = -1;
    if (src[n - 1].ty == 'z') z = --n;
    if (src[0].ty == ']' || src[n - 1].ty == '[') { /* pair */
#ifdef VERBOSE
	fprintf(stderr, "ERROR: LibSpiro: cannot use cp type ']' as start, or '[' as end.\n");
#endif
	return 0;
    }
    if (src[0].ty == 'h' || src[n - 1].ty == 'a') { /* pair */
#ifdef VERBOSE
	fprintf(stderr, "ERROR: LibSpiro: cannot use cp type 'h' as start, or 'a' as end.\n");
#endif
	return 0;
    }

#ifdef CHECK_INPUT_FINITENESS
    /* Verify that input values are within realistic limits */
    for (i = 0; i < n; i++) {
	if (IS_FINITE(src[i].x)==0 || IS_FINITE(src[i].y)==0) {
#ifdef VERBOSE
	    fprintf(stderr, "ERROR: LibSpiro: #%d={'%c',%g,%g} is not finite.\n", \
		    i, src[i].ty, src[i].x, src[i].y);
#endif
	    return 0;
	}
    }
#endif

    n_seg = src[0].ty == '{' ? n - 1 : n;
    i = (int)((unsigned int)(n_seg + 1) * sizeof(spiro_seg));
    if (i <= 0 || (r=(spiro_seg *)malloc((unsigned int)(i))) == NULL) return 0;

    if (dm[0] < 0.9) {
	/* for math to be scalable fit it within -0.5..+0.5 */
	xmin = xmax = src[0].x; ymin = ymax = src[0].y;
	for (i = 0; i < n_seg; i++) {
	    if (src[i].ty != 'z' && src[i].ty != 'h') {
		if (src[i].x < xmin) xmin = src[i].x; else
		if (src[i].x > xmax) xmax = src[i].x;
		if (src[i].y < ymin) ymin = src[i].y; else
		if (src[i].y > ymax) ymax = src[i].y;
	    }
	}
	dm[1] /* xoff */ = (xmin + xmax) / 2; xmax -= xmin;
	dm[2] /* yoff */ = (ymin + ymax) / 2; ymax -= ymin;
	dm[0] /* scale */ = fabs((fabs(xmax) >= fabs(ymax)) ? xmax : ymax);
	if (xmax >= ymax) dm[0] = xmax; else dm[0] = ymax;
	dm[0] /* scale */ /= 500.; /* ~ backward compatible */
    }
#ifdef VERBOSE
	printf("scale=%g, x_offset=%g, y_offset=%g, n=%d, n_seg=%d\n", dm[0], dm[1], dm[2], n, n_seg);
#endif

    for (i = 0; i < n_seg; i++) {
	/* gigo test: error if src[i].ty isn't a known type */
	if (src[i].ty == 'a') {
	    if (src[i + 1].ty == 'h' || (i == n_seg-1 && src[i + 1].ty == '}'))
		;
	    else
		/* did not find 'ah' (or 'a}' as last pair) */
		goto setup_path_error1;
	} else if (src[i].ty == 'h') {
	    if (src[i - 1].ty == 'a' || (i == 1 || src[0].ty == '{'))
		;
	    else
		/* didn't find 'ah' (or '{h' as first pair) */
		goto setup_path_error1;
	}
	r[i].ty = src[i].ty;
	r[i].x = (src[i].x - dm[1]) / dm[0];
	r[i].y = (src[i].y - dm[2]) / dm[0];
	r[i].ks[0] = 0.;
	r[i].ks[1] = 0.;
	r[i].ks[2] = 0.;
	r[i].ks[3] = 0.;
    }
    r[n_seg].x = (src[n_seg % n].x - dm[1]) / dm[0];
    r[n_seg].y = (src[n_seg % n].y - dm[2]) / dm[0];
    r[n_seg].ty = src[n_seg % n].ty;

    for (i = 0; i < n_seg; i++) {
	if (r[i].ty == 'h' || (i == n_seg-1 && i > 0 && r[i].ty == '}' && r[i - 1].ty == 'a')) {
	    /* behave like a disconnected pair of '[' & ']' */
	    /* point 'a' holds vector to old 'h' and now we */
	    /* change x,y here to be the same as point 'a'. */
	    /* curve fitting is based on vectors and angles */
	    /* but final curves will be based on x,y points */
	    r[i].x = r[i - 1].x;
	    r[i].y = r[i - 1].y;
	}
	dx = r[i + 1].x - r[i].x;
	dy = r[i + 1].y - r[i].y;
#ifndef CHECK_INPUT_FINITENESS
	r[i].seg_ch = hypot(dx, dy);
#else
	if (IS_FINITE(dx)==0 || IS_FINITE(dy)==0 || \
	    IS_FINITE((r[i].seg_ch = hypot(dx, dy)))==0) {
#ifdef VERBOSE
	    fprintf(stderr, "ERROR: LibSpiro: #%d={'%c',%g,%g} hypot error.\n", \
		    i, src[i].ty, src[i].x, src[i].y);
#endif
	    goto setup_path_error0;
	}
#endif
	r[i].seg_th = atan2(dy, dx);
    }

    ilast = n_seg - 1;
    for (i = 0; i < n_seg; i++) {
	if (r[i].ty == '{' || r[i].ty == '}' || r[i].ty == 'v')
	    r[i].bend_th = 0.;
	else
	    r[i].bend_th = mod_2pi(r[i].seg_th - r[ilast].seg_th);
	ilast = i;
#ifdef VERBOSE
	printf("input #%d={'%c',%g=>%g,%g=>%g}, hypot=%g, atan2=%g, bend_th=%g\n", \
		    i, src[i].ty, src[i].x, r[i].x * dm[0] + dm[1], \
		    src[i].y, r[i].y * dm[0] + dm[2], r[i].seg_ch * dm[0], \
		    r[i].seg_th, r[i].bend_th);
#endif
    }
#ifdef VERBOSE
    if (n_seg < n)
	printf("input #%d={'%c',%g=>%g,%g=>%g}\n", i, src[i].ty, \
		src[i].x, r[i].x * dm[0] + dm[1], src[i].y, r[i].y * dm[0] + dm[2]);
#endif
    if (z >= 0) r[z].ty = 'z'; /* wrong n, maintain z. */
    return r;

setup_path_error1:
#ifdef VERBOSE
    fprintf(stderr, "ERROR: LibSpiro: #%d={'%c',%g,%g} found unpaired anchor+handle 'ah'.\n", \
	    i, src[i].ty, src[i].x, src[i].y);
#endif
#ifdef CHECK_INPUT_FINITENESS
setup_path_error0:
#endif
    free(r);
    return 0;
}

/* deprecated / backwards compatibility / not scalable */
static spiro_seg *
setup_path(const spiro_cp *src, int n)
{
    double dm[6];
    set_dm_to_1(dm);
    return setup_path0(src, dm, n);
}

static void
bandec11(bandmat *m, int *perm, int n)
{
    int i, j, k, l, pivot;
    double pivot_val, pivot_scale, tmp, x;

    /* pack top triangle to the left. */
    for (i = 0; i < 5; i++) {
	for (j = 0; j < i + 6; j++)
	    m[i].a[j] = m[i].a[j + 5 - i];
	for (; j < 11; j++)
	    m[i].a[j] = 0.;
    }
    l = 5;
    for (k = 0; k < n; k++) {
	pivot = k;
	pivot_val = m[k].a[0];

	l = l < n ? l + 1 : n;

	for (j = k + 1; j < l; j++)
	    if (fabs(m[j].a[0]) > fabs(pivot_val)) {
		pivot_val = m[j].a[0];
		pivot = j;
	    }

	perm[k] = pivot;
	if (pivot != k) {
	    for (j = 0; j < 11; j++) {
		tmp = m[k].a[j];
		m[k].a[j] = m[pivot].a[j];
		m[pivot].a[j] = tmp;
	    }
	}

	if (fabs(pivot_val) < 1e-12) pivot_val = 1e-12;
	pivot_scale = 1. / pivot_val;
	for (i = k + 1; i < l; i++) {
	    x = m[i].a[0] * pivot_scale;
	    m[k].al[i - k - 1] = x;
	    for (j = 1; j < 11; j++)
		m[i].a[j - 1] = m[i].a[j] - x * m[k].a[j];
	    m[i].a[10] = 0.;
	}
    }
}

static void
banbks11(const bandmat *m, const int *perm, double *v, int n)
{
    int i, k, l;
    double tmp, x;

    /* forward substitution */
    l = 5;
    for (k = 0; k < n; k++) {
	i = perm[k];
	if (i != k) {
	    tmp = v[k];
	    v[k] = v[i];
	    v[i] = tmp;
	}
	if (l < n) l++;
	for (i = k + 1; i < l; i++)
	    v[i] -= m[k].al[i - k - 1] * v[k];
    }

    /* back substitution */
    l = 1;
    for (i = n - 1; i >= 0; i--) {
	x = v[i];
	for (k = 1; k < l; k++)
	    x -= m[i].a[k] * v[k + i];
	v[i] = x / m[i].a[0];
	if (l < 11) l++;
    }
}

static int compute_jinc(char ty0, char ty1)
{
    if (ty0 == 'o' || ty1 == 'o' || \
	ty0 == ']' || ty1 == '[' || \
	ty0 == 'h' || ty1 == 'a')
	return 4;
    else if (ty0 == 'c' && ty1 == 'c')
	return 2;
    else if ((ty1 == 'c' && (ty0 == '{' || ty0 == 'v' || ty0 == '[' || ty0 == 'a')) ||
	     (ty0 == 'c' && (ty1 == '}' || ty1 == 'v' || ty1 == ']' || ty1 == 'h')))
	return 1;
    else
	return 0;
}

static int count_vec(const spiro_seg *s, int *jinca, int nseg)
{
    int i, n;

    n = 0;

    for (i = 0; i < nseg; i++)
	n += (jinca[i] = compute_jinc(s[i].ty, s[i + 1].ty));
    return n;
}

static void
add_mat_line(bandmat *m, double *v,double derivs[4],
	     double x, double y, int j, int jj, int jinc, int nmat)
{
    int joff, k;

    if (jj >= 0) {
	joff =  (j + 5 - jj + nmat) % nmat;
	if (nmat < 6) {
	    joff = j + 5 - jj;
	} else if (nmat == 6) {
	    joff = 2 + (j + 3 - jj + nmat) % nmat;
	}
#ifdef VERBOSE
	printf("add_mat_line j=%d jj=%d jinc=%d nmat=%d joff=%d\n", j, jj, jinc, nmat, joff);
#endif
	v[jj] += x;
	for (k = 0; k < jinc; k++)
	    m[jj].a[joff + k] += y * derivs[k];
    }
}

static double
spiro_iter(spiro_seg *s, bandmat *m, int *perm, double *v, int *jinca, int n, int cyclic, int nmat)
{
    unsigned int l;
    int i, j, jthl, jthr, jk0l, jk0r, jk1l, jk1r, jk2l, jk2r, jinc, jj, k, n_invert;
    char ty0, ty1;
    double dk, norm, th;
    double ends[2][4];
    double derivs[4][2][4];

    for (i = 0; i < nmat; i++) {
	v[i] = 0.;
	for (j = 0; j < 11; j++)
	    m[i].a[j] = 0.;
	for (j = 0; j < 5; j++)
	    m[i].al[j] = 0.;
    }

    j = 0;
    if (s[0].ty == 'o')
	jj = nmat - 2;
    else if (s[0].ty == 'c')
	jj = nmat - 1;
    else
	jj = 0;
    for (i = 0; i < n; i++) {
	ty0 = s[i].ty;
	ty1 = s[i + 1].ty;
	jinc = jinca[i];
	th = s[i].bend_th;
	jthl = jk0l = jk1l = jk2l = -1;
	jthr = jk0r = jk1r = jk2r = -1;

	compute_pderivs(&s[i], ends, derivs, jinc);

	/* constraints crossing left */
	if (ty0 == 'o' || ty0 == 'c' || ty0 == '[' || ty0 == ']' || \
	    ty0 == 'a' || ty0 == 'h') {
	    jthl = jj++;
	    jj %= nmat;
	    jk0l = jj++;
	    if (ty0 == 'o') {
		jj %= nmat;
		jk1l = jj++;
		jk2l = jj++;
	    }
	}

	if (jinc == 4) {
	    /* constraints on left */
	    if (ty0 == 'c' || ty0 == 'v' || ty0 == '[' || \
		ty0 == 'a' || ty0 == '{') {
		if (ty0 != 'c')
		    jk1l = jj++;
		jk2l = jj++;
	    }
	    /* constraints on right */
	    if (ty1 == 'c' || ty1 == 'v' || ty1 == ']' || \
		ty1 == 'h' || ty1 == '}') {
		if (ty1 != 'c')
		    jk1r = jj++;
		jk2r = jj++;
	    }
	}

	/* constraints crossing right */
	if (ty1 == 'o' || ty1 == 'c' || ty1 == '[' || ty1 == ']' || \
	    ty1 == 'a' || ty1 == 'h') {
	    jthr = jj;
	    jk0r = (jj + 1) % nmat;
	    if (ty1 == 'o') {
		jk1r = (jj + 2) % nmat;
		jk2r = (jj + 3) % nmat;
	    }
	}

	add_mat_line(m, v, derivs[0][0], th - ends[0][0], 1, j, jthl, jinc, nmat);
	add_mat_line(m, v, derivs[1][0], ends[0][1], -1, j, jk0l, jinc, nmat);
	add_mat_line(m, v, derivs[2][0], ends[0][2], -1, j, jk1l, jinc, nmat);
	add_mat_line(m, v, derivs[3][0], ends[0][3], -1, j, jk2l, jinc, nmat);
	add_mat_line(m, v, derivs[0][1], -ends[1][0], 1, j, jthr, jinc, nmat);
	add_mat_line(m, v, derivs[1][1], -ends[1][1], 1, j, jk0r, jinc, nmat);
	add_mat_line(m, v, derivs[2][1], -ends[1][2], 1, j, jk1r, jinc, nmat);
	add_mat_line(m, v, derivs[3][1], -ends[1][3], 1, j, jk2r, jinc, nmat);
	if (jthl >= 0)
	    v[jthl] = mod_2pi(v[jthl]);
	if (jthr >= 0)
	    v[jthr] = mod_2pi(v[jthr]);
	j += jinc;
    }
    if (cyclic) {
	l = sizeof(bandmat) * (unsigned int)(nmat);
	memcpy(m + nmat, m, l);
	memcpy(m + 2 * nmat, m, l);
	l = sizeof(double) * (unsigned int)(nmat);
	memcpy(v + nmat, v, l);
	memcpy(v + 2 * nmat, v, l);
	n_invert = 3 * nmat;
	j = nmat;
#ifdef VERBOSE
	printf("cyclic\n");
#endif
    } else {
	n_invert = nmat;
	j = 0;
    }

#ifdef VERBOSE
    for (i = 0; i < n; i++) {
	for (k = 0; k < 11; k++)
	    printf(" %2.4f", m[i].a[k]);
	printf(": %2.4f\n", v[i]);
    }
    printf("---\n");
#endif
    bandec11(m, perm, n_invert);
    banbks11(m, perm, v, n_invert);
    norm = 0.;
    for (i = 0; i < n; i++) {
	jinc = jinca[i];

	for (k = 0; k < jinc; k++) {
	    dk = v[j++];

#ifdef VERBOSE
	    printf("s[%d].ks[%d] += %f\n", i, k, dk);
#endif
	    s[i].ks[k] += dk;
	    norm += dk * dk;

	    /* Break if calculations are headed for failure */
	    if (IS_FINITE(s[i].ks[k]) == 0) return s[i].ks[k];
	}
        s[i].ks[0] = 2.0 * mod_2pi(s[i].ks[0]/2.0);
    }
    return norm;
}

static int
solve_spiro(spiro_seg *s, int n)
{
    int i, converged, cyclic, nmat, n_alloc, nseg, z;
    bandmat *m;
    double *v;
    int *perm, *jinca;
    double norm;

    i = converged = 0; /* not solved (yet) */
    z = -1;
    if (s[0].ty == '{')
	nseg = n - 1;
    else {
	if (s[n - 1].ty == 'z') {
	    z = --n;
	    s[z].ty = s[0].ty;
	}
	nseg = n;
    }

    if (nseg <= 1) {
	converged = 1; /* means no convergence problems */
	goto solve_spiroerr0;
    }

    if ((jinca = (int *)malloc(sizeof(int) * (int)(nseg))) == NULL) {
#ifdef VERBOSE
	fprintf(stderr, "ERROR: LibSpiro: failed to alloc memory.\n");
#endif
	goto solve_spiroerr0;
    }
    nmat = count_vec(s, jinca, nseg);
    if (nmat == 0) {
	converged = 1; /* means no convergence problems */
	goto solve_spiroerr1;
    }
    n_alloc = nmat;
    cyclic = 0;
    if (s[0].ty != '{' && s[0].ty != 'v') {
	n_alloc *= 3;
	++cyclic;
    }
    if (n_alloc < 5)
	n_alloc = 5;
#ifdef VERBOSE
    printf("nmat=%d, alloc=%d, cyclic=%d, n=%d, nseg=%d\n", nmat, n_alloc, cyclic, n, nseg);
#endif
    m = (bandmat *)malloc(sizeof(bandmat) * (unsigned int)(n_alloc));
    v = (double *)malloc(sizeof(double) * (unsigned int)(n_alloc));
    perm = (int *)malloc(sizeof(int) * (unsigned int)(n_alloc));

    if ( m!=NULL && v!=NULL && perm!=NULL ) {
	while (i++ < 60) {
	    norm = spiro_iter(s, m, perm, v, jinca, nseg, cyclic, nmat);
	    if (IS_FINITE(norm)==0) break;
#ifdef VERBOSE
	    printf("iteration #%d, %% norm = %g\n", i, norm);
#endif
	    if (norm < 1e-12) { converged = 1; break; }
	}
#ifdef VERBOSE
	if (converged==0)
	    fprintf(stderr, "ERROR: LibSpiro: failed to converge after %d attempts to converge.\n", i);
    } else {
	fprintf(stderr, "ERROR: LibSpiro: failed to alloc memory.\n");
#endif
    }

    free(perm);
    free(v);
    free(m);
solve_spiroerr1:
    free(jinca);
solve_spiroerr0:
    if (z >= 0) s[z].ty = 'z';
    return converged;
}

static void
spiro_seg_to_bpath1(const double ks[4], double *dm, double *di,
		   double x0, double y0, double x1, double y1,
		   bezctx *bc, int ncq, int depth)
{
    double bend, seg_ch, seg_th, ch, th, scale, rot;
    double th_even, th_odd, ul, vl, ur, vr;
    double thsub, xmid, ymid, cth, sth;
    double ksub[4], xysub[2], xy[2];

    bend = fabs(ks[0]) + fabs(.5 * ks[1]) + fabs(.125 * ks[2]) +
	fabs((1./48) * ks[3]);

    if (bend <= 1e-8) {
	if (di[3] < x1 && x1 < di[4] && di[6] < y1 && y1 < di[7]) {
#ifdef VERBOSE
	    printf("...to next knot point...\n");
#endif
	    bezctx_lineto(bc, di[2], di[5]);
	} else {
	    bezctx_lineto(bc, x1 * dm[0] + dm[1], y1 * dm[0] + dm[2]);
	}
    } else {
	seg_ch = hypot(x1 - x0, y1 - y0);
	seg_th = atan2(y1 - y0, x1 - x0);

	integrate_spiro(ks, xy, N_IS);
	ch = hypot(xy[0], xy[1]);
	th = atan2(xy[1], xy[0]);
	scale = seg_ch / ch;
	rot = seg_th - th;
	if (ncq == 0 && (depth > 5 || bend < di[0])) {
	    /* calculate cubic, and output bezier points */
	    th_even = (1./384) * ks[3] + (1./8) * ks[1] + rot;
	    th_odd = (1./48) * ks[2] + .5 * ks[0];
	    ul = (scale * (1./3)) * cos(th_even - th_odd);
	    vl = (scale * (1./3)) * sin(th_even - th_odd);
	    ur = (scale * (1./3)) * cos(th_even + th_odd);
	    vr = (scale * (1./3)) * sin(th_even + th_odd);
	    if (di[3] < x1 && x1 < di[4] && di[6] < y1 && y1 < di[7]) {
#ifdef VERBOSE
		printf("...to next knot point...\n");
#endif
		bezctx_curveto(bc, ((x0 + ul) * dm[0] + dm[1]), ((y0 + vl) * dm[0] + dm[2]),
				((x1 - ur) * dm[0] + dm[1]), ((y1 - vr) * dm[0] + dm[2]),
				di[2], di[5]);
	    } else {
		bezctx_curveto(bc, ((x0 + ul) * dm[0] + dm[1]), ((y0 + vl) * dm[0] + dm[2]),
				((x1 - ur) * dm[0] + dm[1]), ((y1 - vr) * dm[0] + dm[2]),
				(x1 * dm[0] + dm[1]), (y1 * dm[0] + dm[2]));
	    }
	} else {
	    /* subdivide */
	    ksub[0] = .5 * ks[0] - .125 * ks[1] + (1./64) * ks[2] - (1./768) * ks[3];
	    ksub[1] = .25 * ks[1] - (1./16) * ks[2] + (1./128) * ks[3];
	    ksub[2] = .125 * ks[2] - (1./32) * ks[3];
	    ksub[3] = (1./16) * ks[3];
	    thsub = rot - .25 * ks[0] + (1./32) * ks[1] - (1./384) * ks[2] + (1./6144) * ks[3];
	    cth = .5 * scale * cos(thsub);
	    sth = .5 * scale * sin(thsub);
	    integrate_spiro(ksub, xysub, N_IS);
	    xmid = x0 + cth * xysub[0] - sth * xysub[1];
	    ymid = y0 + cth * xysub[1] + sth * xysub[0];
	    if (ncq != 0 && (depth > 5 || bend < di[0])) {
		if (ncq < 0) {
		    /* looks like an arc (use quadto output). */
		    if (di[3] < x1 && x1 < di[4] && di[6] < y1 && y1 < di[7]) {
#ifdef VERBOSE
			printf("...to next knot point...\n");
#endif
			bezctx_quadto(bc, (xmid * dm[0] + dm[1]), (ymid * dm[0] + dm[2]), di[2], di[5]);
		    } else {
			bezctx_quadto(bc, (xmid * dm[0] + dm[1]), (ymid * dm[0] + dm[2]), (x1 * dm[0] + dm[1]), (y1 * dm[0] + dm[2]));
		    }
		} else {
		    /* create quadratic bezier approximations */
		    th_even = (1./384) * ks[3] + (1./8) * ks[1] + rot;
		    th_odd = (1./48) * ks[2] + .5 * ks[0];
		    ul = (scale * (1./6)) * cos(th_even - th_odd);
		    vl = (scale * (1./6)) * sin(th_even - th_odd);
		    ur = (scale * (1./6)) * cos(th_even + th_odd);
		    vr = (scale * (1./6)) * sin(th_even + th_odd);
		    bezctx_quadto(bc, ((x0 + ul) * dm[0] + dm[1]), ((y0 + vl) * dm[0] + dm[2]), \
					(xmid * dm[0] + dm[1]), (ymid * dm[0] + dm[2]));
		    if (di[3] < x1 && x1 < di[4] && di[6] < y1 && y1 < di[7]) {
#ifdef VERBOSE
			printf("...to next knot point...\n");
#endif
			bezctx_quadto(bc, ((x1 - ur) * dm[0] + dm[1]), ((y1 - vr) * dm[0] + dm[2]), \
					    di[2], di[5]);
		    } else {
			bezctx_quadto(bc, ((x1 - ur) * dm[0] + dm[1]), ((y1 - vr) * dm[0] + dm[2]), \
					    (x1 * dm[0] + dm[1]), (y1 * dm[0] + dm[2]));
		    }
		}
	    } else {
#ifdef VERBOSE
		printf("...subdivide curve...\n");
#endif
		spiro_seg_to_bpath1(ksub, dm, di, x0, y0, xmid, ymid, bc, ncq, depth + 1);
		ksub[0] += .25 * ks[1] + (1./384) * ks[3];
		ksub[1] += .125 * ks[2];
		ksub[2] += (1./16) * ks[3];
		spiro_seg_to_bpath1(ksub, dm, di, xmid, ymid, x1, y1, bc, ncq, depth + 1);
	    }
	}
    }
}

/* deprecated / keep backwards compatibility / not scalable */
static void
spiro_seg_to_bpath(const double ks[4],
		   double x0, double y0, double x1, double y1,
		   bezctx *bc, int depth)
{
    double di[8], dm[6];
    set_dm_to_1(dm);
    di[1] = 0.0005 * 500; /* assume size in range {0..1000} */
    set_di_to_x1y1(di, dm, x1, y1);
    spiro_seg_to_bpath1(ks, dm, di, x0, y0, x1, y1, bc, SPIRO_RETRO_VER1, depth);
}

/* This function reverses src path for calling application. */
/* Spiro calculations might not translate well in the other */
/* direction, however, there may be a need to reverse path. */
/* Function leaves src unmodified if cannot reverse values. */
int
spiroreverse(spiro_cp *src, int n)
{
    char c;
    int i, j;
    double x, y;
    spiro_cp *tmp;

    if (n > 2 && src[0].ty == '{' && \
	(src[1].ty == 'h' || src[n - 2].ty == 'a')) {
#ifdef VERBOSE
	fprintf(stderr, "ERROR: LibSpiro: cannot reverse this list because it starts with '{','h' or ends with 'a','}'.\n");
#endif
	return -1;
    }

    if (src[n - 1].ty == 'z') --n;

    i = (int)((unsigned int)(n) * sizeof(spiro_cp));
    if ( i <= 0 || (tmp=(spiro_cp *)malloc((unsigned int)(i))) == NULL ) return -1;

#ifdef VERBOSE
    printf("reverse n=%d values:\n",n);
#endif

    for (i=0,j=--n; i <= j; i++, j--) {
	/* NOTE: For graphic programs that repeat this over */
	/* and over again, this reversal is best done once, */
	/* and then you use the reversed string repeatedly; */
	/* This helps avoid wasting time to recalculate the */
	/* string over and over again unnecessarily. Script */
	/* and non-graphic programs tend to need this once, */
	/* or speed isn't as important as script simplicity */
	/* so this suggestion (to pre-save) is unnecessary. */
	tmp[j].ty = src[i].ty; tmp[j].x = src[i].x; tmp[j].y = src[i].y;
	if (i == j) break;
	tmp[i].ty = src[j].ty; tmp[i].x = src[j].x; tmp[i].y = src[j].y;
    }
    for (i=0; i <= n; i++) {
	c = tmp[i].ty;
	if (c == '[')
	    tmp[i].ty = ']';
	else if (c == ']')
	    tmp[i].ty = '[';
	else if (c == '{')
	    tmp[i].ty = '}';
	else if (c == '}')
	    tmp[i].ty = '{';
	else if (c == 'h') {
	    tmp[i].ty = 'a';
	    x = tmp[i].x; tmp[i].x = tmp[i + 1].x; x -= tmp[i].x;
	    y = tmp[i].y; tmp[i].y = tmp[i + 1].y; y -= tmp[i].y;
	    if ( tmp[++i].ty != 'a')
		goto errspiroreverse;
	    tmp[i].ty = 'h';
	    tmp[i].x -= x;
	    tmp[i].y -= y;
	} else if (c == 'a')
	    goto errspiroreverse;
    }
    for (i=0; i <= n; i++) {
	src[i].ty = tmp[i].ty;
	src[i].x = tmp[i].x;
	src[i].y = tmp[i].y;
#ifdef VERBOSE
	printf("reversed %d: ty=%c, x=%g, y=%g\n", i, src[i].ty, src[i].x, src[i].y );
#endif
    }
    free(tmp);
    return 0;

errspiroreverse:
    free(tmp);
    return -1;
}

spiro_seg *
run_spiro0(const spiro_cp *src, double *dm, int ncq, int n)
{
    spiro_seg *s;

    if (src==NULL || n <= 0 || ncq < 0) return 0;

    if ((ncq & SPIRO_RETRO_VER1)) set_dm_to_1(dm); else dm[0] = -1.;

    s = setup_path0(src, dm, n);
    if (s) {
	if (solve_spiro(s, n)) return s;
	free(s);
    }
    return 0;
}

/* deprecated / backwards compatibility / not scalable */
spiro_seg *
run_spiro(const spiro_cp *src, int n)
{
    double dm[6];
    return run_spiro0(src, dm, SPIRO_RETRO_VER1, n);
}

void
free_spiro(spiro_seg *s)
{
    if (s) free(s);
}

void
spiro_to_bpath0(const spiro_cp *src, const spiro_seg *s,
		double *dm, int ncq, int n, bezctx *bc)
{
    int i, j, lk, nsegs, z;
    double di[8], x0, y0, x1, y1;

    if (s==NULL || n <= 0 || ncq < 0 || bc==NULL) return;

    nsegs = n;
    if (s[0].ty == '{') {
	if (n >= 2 && s[n - 2].ty == 'a')
	    --nsegs;
	--nsegs;
	z = -1;
    } else {
	if (s[n - 1].ty == 'z')
	    --nsegs;
	z = nsegs - 1;
    }
#ifdef VERBOSE
    printf("spiro_to_bpath0 ncq=0x%x n=%d nsegs=%d s=%d bc=%d\n",ncq,n,nsegs,s==NULL ? 0:1,bc==NULL ? 0:1);
#endif

    x0 = x1 = s[0].x; y0 = y1 = s[0].y;
    for (i = 1; i < nsegs; i++) {
	if (s[i].ty != 'z' && s[i].ty != 'h') {
	    if (s[i].x < x0) x0 = s[i].x; else
	    if (s[i].x > x1) x1 = s[i].x;
	    if (s[i].y < y0) y0 = s[i].y; else
	    if (s[i].y > y1) y1 = s[i].y;
	}
    }
    x1 -= x0; y1 -= y0;
    di[1] = (x1 >= y1) ? x1: y1;
    di[1] *= 0.0005;

    di[0] = 1.; /* default cubic to bezier bend */

    lk = (ncq & SPIRO_INCLUDE_LAST_KNOT) && s[n - 1].ty == '}' ? 1 : 0;

    if ( (ncq &= SPIRO_ARC_CUB_QUAD_MASK)==0 ) {
	/* default action = cubic bezier output */;
    } else if (ncq == SPIRO_CUBIC_MIN_MAYBE) { /* visual inspection advised */
	ncq = 0; /* NOTE: experimental, best to look at results first */
	di[0] = M_PI / 2 + .000001;
    } else if (ncq == SPIRO_ARC_MAYBE) { /* visual inspection advised */
	ncq = -1; /* NOTE: these are arcs (maybe), not quadratic */
    } else if (ncq == SPIRO_ARC_MIN_MAYBE) { /* visual inspection advised */
	ncq = -1; /* NOTE: these are arcs (maybe), not quadratic */
	di[0] = M_PI / 2 + .000001;
    } else if (ncq == SPIRO_QUAD0_TO_BEZIER) {
	/* roughly approximate a cubic using two quadratic arcs. */
	ncq = 0x10;
    }

    for (i=j=0; i < nsegs; i++,j++) {
	x0 = s[i].x; y0 = s[i].y;
	if (i == 0) {
	    if (src != NULL) {
		bezctx_moveto(bc, src[0].x, src[0].y, s[0].ty == '{');
	    } else {
		bezctx_moveto(bc, (x0 * dm[0] + dm[1]), (y0 * dm[0] + dm[2]), s[0].ty == '{');
	    }
	    if (nsegs > 1 && s[1].ty == 'h') ++i;
	} else
	    if (s[i].ty == 'a') ++i;
	if (i == z) {
	    x1 = s[0].x; y1 = s[0].y;
	} else {
	    x1 = s[i + 1].x; y1 = s[i + 1].y;
	}
	set_di_to_x1y1(di, dm, x1, y1);
	if (src != NULL) {
	    if (i == z) {
		di[2] = src[0].x; di[5] = src[0].y;
	    } else {
		di[2] = src[i + 1].x; di[5] = src[i + 1].y;
	    }
	}

	bezctx_mark_knot(bc, j);
	spiro_seg_to_bpath1(s[i].ks, dm, di, x0, y0, x1, y1, bc, ncq, 0);
    }
    if (lk) bezctx_mark_knot(bc, j);
}

/* deprecated / backwards compatibility / not scalable */
void
spiro_to_bpath(const spiro_seg *s, int n, bezctx *bc)
{
    double dm[6];
    set_dm_to_1(dm);
    spiro_to_bpath0(NULL, s, dm, SPIRO_RETRO_VER1, n, bc);
}

const char *
LibSpiroVersion(void)
{
    return( LS_VERSION_STR );
}

double
get_knot_th(const spiro_seg *s, int i)
{
    double ends[2][4];

    if (i == 0) {
	compute_ends(s[i].ks, ends, s[i].seg_ch);
	return s[i].seg_th - ends[0][0];
    } else {
	compute_ends(s[i - 1].ks, ends, s[i - 1].seg_ch);
	return s[i - 1].seg_th + ends[1][0];
    }
}
