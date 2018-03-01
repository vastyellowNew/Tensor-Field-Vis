#include "blaswrap.h"
#include "f2c.h"

/* Subroutine */ int slatzm_(char *side, integer *m, integer *n, real *v, 
	integer *incv, real *tau, real *c1, real *c2, integer *ldc, real *
	work)
{
/*  -- LAPACK routine (version 3.1) --   
       Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..   
       November 2006   


    Purpose   
    =======   

    This routine is deprecated and has been replaced by routine SORMRZ.   

    SLATZM applies a Householder matrix generated by STZRQF to a matrix.   

    Let P = I - tau*u*u',   u = ( 1 ),   
                                ( v )   
    where v is an (m-1) vector if SIDE = 'L', or a (n-1) vector if   
    SIDE = 'R'.   

    If SIDE equals 'L', let   
           C = [ C1 ] 1   
               [ C2 ] m-1   
                 n   
    Then C is overwritten by P*C.   

    If SIDE equals 'R', let   
           C = [ C1, C2 ] m   
                  1  n-1   
    Then C is overwritten by C*P.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': form P * C   
            = 'R': form C * P   

    M       (input) INTEGER   
            The number of rows of the matrix C.   

    N       (input) INTEGER   
            The number of columns of the matrix C.   

    V       (input) REAL array, dimension   
                    (1 + (M-1)*abs(INCV)) if SIDE = 'L'   
                    (1 + (N-1)*abs(INCV)) if SIDE = 'R'   
            The vector v in the representation of P. V is not used   
            if TAU = 0.   

    INCV    (input) INTEGER   
            The increment between elements of v. INCV <> 0   

    TAU     (input) REAL   
            The value tau in the representation of P.   

    C1      (input/output) REAL array, dimension   
                           (LDC,N) if SIDE = 'L'   
                           (M,1)   if SIDE = 'R'   
            On entry, the n-vector C1 if SIDE = 'L', or the m-vector C1   
            if SIDE = 'R'.   

            On exit, the first row of P*C if SIDE = 'L', or the first   
            column of C*P if SIDE = 'R'.   

    C2      (input/output) REAL array, dimension   
                           (LDC, N)   if SIDE = 'L'   
                           (LDC, N-1) if SIDE = 'R'   
            On entry, the (m - 1) x n matrix C2 if SIDE = 'L', or the   
            m x (n - 1) matrix C2 if SIDE = 'R'.   

            On exit, rows 2:m of P*C if SIDE = 'L', or columns 2:m of C*P   
            if SIDE = 'R'.   

    LDC     (input) INTEGER   
            The leading dimension of the arrays C1 and C2. LDC >= (1,M).   

    WORK    (workspace) REAL array, dimension   
                        (N) if SIDE = 'L'   
                        (M) if SIDE = 'R'   

    =====================================================================   


       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static real c_b5 = 1.f;
    
    /* System generated locals */
    integer c1_dim1, c1_offset, c2_dim1, c2_offset, i__1;
    real r__1;
    /* Local variables */
    extern /* Subroutine */ int sger_(integer *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sgemv_(char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *, integer *), scopy_(integer *, real *, integer *, real *, integer *), 
	    saxpy_(integer *, real *, real *, integer *, real *, integer *);


    --v;
    c2_dim1 = *ldc;
    c2_offset = 1 + c2_dim1;
    c2 -= c2_offset;
    c1_dim1 = *ldc;
    c1_offset = 1 + c1_dim1;
    c1 -= c1_offset;
    --work;

    /* Function Body */
    if (min(*m,*n) == 0 || *tau == 0.f) {
	return 0;
    }

    if (lsame_(side, "L")) {

/*        w := C1 + v' * C2 */

	scopy_(n, &c1[c1_offset], ldc, &work[1], &c__1);
	i__1 = *m - 1;
	sgemv_("Transpose", &i__1, n, &c_b5, &c2[c2_offset], ldc, &v[1], incv, 
		 &c_b5, &work[1], &c__1);

/*        [ C1 ] := [ C1 ] - tau* [ 1 ] * w'   
          [ C2 ]    [ C2 ]        [ v ] */

	r__1 = -(*tau);
	saxpy_(n, &r__1, &work[1], &c__1, &c1[c1_offset], ldc);
	i__1 = *m - 1;
	r__1 = -(*tau);
	sger_(&i__1, n, &r__1, &v[1], incv, &work[1], &c__1, &c2[c2_offset], 
		ldc);

    } else if (lsame_(side, "R")) {

/*        w := C1 + C2 * v */

	scopy_(m, &c1[c1_offset], &c__1, &work[1], &c__1);
	i__1 = *n - 1;
	sgemv_("No transpose", m, &i__1, &c_b5, &c2[c2_offset], ldc, &v[1], 
		incv, &c_b5, &work[1], &c__1);

/*        [ C1, C2 ] := [ C1, C2 ] - tau* w * [ 1 , v'] */

	r__1 = -(*tau);
	saxpy_(m, &r__1, &work[1], &c__1, &c1[c1_offset], &c__1);
	i__1 = *n - 1;
	r__1 = -(*tau);
	sger_(m, &i__1, &r__1, &work[1], &c__1, &v[1], incv, &c2[c2_offset], 
		ldc);
    }

    return 0;

/*     End of SLATZM */

} /* slatzm_ */
