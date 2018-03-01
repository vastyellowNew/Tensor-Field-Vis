#include "blaswrap.h"
#include "f2c.h"

/* Subroutine */ int slarrc_(char *jobt, integer *n, real *vl, real *vu, real 
	*d__, real *e, real *pivmin, integer *eigcnt, integer *lcnt, integer *
	rcnt, integer *info)
{
/*  -- LAPACK auxiliary routine (version 3.1) --   
       Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..   
       November 2006   


    Purpose   
    =======   

    Find the number of eigenvalues of the symmetric tridiagonal matrix T   
    that are in the interval (VL,VU] if JOBT = 'T', and of L D L^T   
    if JOBT = 'L'.   

    Arguments   
    =========   

    JOBT    (input) CHARACTER*1   
            = 'T':  Compute Sturm count for matrix T.   
            = 'L':  Compute Sturm count for matrix L D L^T.   

    N       (input) INTEGER   
            The order of the matrix. N > 0.   

    VL      (input) DOUBLE PRECISION   
    VU      (input) DOUBLE PRECISION   
            The lower and upper bounds for the eigenvalues.   

    D       (input) DOUBLE PRECISION array, dimension (N)   
            JOBT = 'T': The N diagonal elements of the tridiagonal matrix T.   
            JOBT = 'L': The N diagonal elements of the diagonal matrix D.   

    E       (input) DOUBLE PRECISION array, dimension (N)   
            JOBT = 'T': The N-1 offdiagonal elements of the matrix T.   
            JOBT = 'L': The N-1 offdiagonal elements of the matrix L.   

    PIVMIN  (input) DOUBLE PRECISION   
            The minimum pivot in the Sturm sequence for T.   

    EIGCNT  (output) INTEGER   
            The number of eigenvalues of the symmetric tridiagonal matrix T   
            that are in the interval (VL,VU]   

    LCNT    (output) INTEGER   
    RCNT    (output) INTEGER   
            The left and right negcounts of the interval.   

    INFO    (output) INTEGER   

    Further Details   
    ===============   

    Based on contributions by   
       Beresford Parlett, University of California, Berkeley, USA   
       Jim Demmel, University of California, Berkeley, USA   
       Inderjit Dhillon, University of Texas, Austin, USA   
       Osni Marques, LBNL/NERSC, USA   
       Christof Voemel, University of California, Berkeley, USA   

    =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    integer i__1;
    real r__1;
    /* Local variables */
    static integer i__;
    static real sl, su, tmp, tmp2;
    static logical matt;
    extern logical lsame_(char *, char *);
    static real lpivot, rpivot;

    --e;
    --d__;

    /* Function Body */
    *info = 0;
    *lcnt = 0;
    *rcnt = 0;
    *eigcnt = 0;
    matt = lsame_(jobt, "T");
    if (matt) {
/*        Sturm sequence count on T */
	lpivot = d__[1] - *vl;
	rpivot = d__[1] - *vu;
	if (lpivot <= 0.f) {
	    ++(*lcnt);
	}
	if (rpivot <= 0.f) {
	    ++(*rcnt);
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	    r__1 = e[i__];
	    tmp = r__1 * r__1;
	    lpivot = d__[i__ + 1] - *vl - tmp / lpivot;
	    rpivot = d__[i__ + 1] - *vu - tmp / rpivot;
	    if (lpivot <= 0.f) {
		++(*lcnt);
	    }
	    if (rpivot <= 0.f) {
		++(*rcnt);
	    }
/* L10: */
	}
    } else {
/*        Sturm sequence count on L D L^T */
	sl = -(*vl);
	su = -(*vu);
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    lpivot = d__[i__] + sl;
	    rpivot = d__[i__] + su;
	    if (lpivot <= 0.f) {
		++(*lcnt);
	    }
	    if (rpivot <= 0.f) {
		++(*rcnt);
	    }
	    tmp = e[i__] * d__[i__] * e[i__];

	    tmp2 = tmp / lpivot;
	    if (tmp2 == 0.f) {
		sl = tmp - *vl;
	    } else {
		sl = sl * tmp2 - *vl;
	    }

	    tmp2 = tmp / rpivot;
	    if (tmp2 == 0.f) {
		su = tmp - *vu;
	    } else {
		su = su * tmp2 - *vu;
	    }
/* L20: */
	}
	lpivot = d__[*n] + sl;
	rpivot = d__[*n] + su;
	if (lpivot <= 0.f) {
	    ++(*lcnt);
	}
	if (rpivot <= 0.f) {
	    ++(*rcnt);
	}
    }
    *eigcnt = *rcnt - *lcnt;
    return 0;

/*     end of SLARRC */

} /* slarrc_ */
