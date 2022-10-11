#ifndef _SPIRO_H
#define _SPIRO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
/* User passes an array of SpiroCP in this format for Spiro to solve */
    double x;		/* Spiro CodePoint Xloc */
    double y;		/* Spiro CodePoint Yloc */
    char ty;		/* Spiro CodePoint Type */
} spiro_cp;

struct spiro_seg_s {
/* run_spiro() uses array of information given in the structure above and */
/* creates an array in this structure format to use by spiro_to_bpath for */
/* building bezier curves */
    double x;		/* SpiroCP segment_chord startX */
    double y;		/* SpiroCP segment_chord startY */
    char ty;		/* Spiro CodePoint Type */
    double bend_th;	/* bend theta between this vector and next vector */
    double ks[4];
    double seg_ch;	/* segment_chord distance from xy to next SpiroCP */
    double seg_th;	/* segment_theta angle for this SpiroCP */
    double l;
};

typedef struct spiro_seg_s spiro_seg;

spiro_seg *
run_spiro(const spiro_cp *src, int n); /* deprecated */

void
free_spiro(spiro_seg *s);

void
spiro_to_bpath(const spiro_seg *s, int n, bezctx *bc); /* deprecated */

double get_knot_th(const spiro_seg *s, int i);

spiro_seg *
run_spiro0(const spiro_cp *src, double *dm, int ncq, int n);

void
spiro_to_bpath0(const spiro_cp *src, const spiro_seg *s,
		double *dm, int ncq, int n, bezctx *bc);

int spiroreverse(spiro_cp *src, int n); /* internal use */
 
const char *
LibSpiroVersion(void);

#ifdef __cplusplus
}
#endif
#endif
