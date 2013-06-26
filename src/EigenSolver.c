/* eigensolver.c */

#include <math.h>

#define SQRT3   1.732050807568877
#define EPSILON 1e-12

/* calcula as raizes do polinomio -x^3 + c2*x^2 - c1*x + c0 */
static int cubic_roots(float c2, float c1, float c0, float *r)
{
    float a = (3*c1 + c2*c2)/3,
          b = (-2*c2*c2*c2 + 9*c1*c2 - 27*c0),
          Q = b*b/4 + a*a*a/27;

    if (fabs(Q) < EPSILON) {
        float s = c2/2, t = pow(b/2, 1.0/3);
        
        r[0] = s + t;
        r[1] = r[0];
        r[2] = s - 2*t; 
        
        return 2;
    }
    if (Q < 0) {
        float theta = atan2(sqrt(-Q), -b/2), ro = sqrt(b*b/4 - Q),
              s = c2/3, t = pow(ro, 1.0/3), u = cos(theta/3), v = sin(theta/3);

        r[0] = s + 2*t*u;
        r[1] = s - t*(u + SQRT3*v);
        r[2] = s - t*(u - SQRT3*v);

        return 3;
    }
    return 1;
}
   
/* encontra os autovetores da matriz simetrica A */
void eigenvectors3x3(float *A, float *P)
{
    float r[3], c0, c1, c2;
    int nroots;

    c0 = A[0]*A[4]*A[8] + 2*A[1]*A[2]*A[5] - A[0]*A[5]*A[5] + A[4]*A[2] -
         A[8]*A[1]*A[1];
    c1 = A[0]*A[4] - A[1]*A[1] + A[0]*A[8] - A[2]*A[2] + A[4]*A[8] - A[5]*A[5];
    c2 = A[0] + A[4] + A[8];

    nroots = cubic_roots(c0, c1, c2, r);

    if (nroots == 1) {
        P[0][0] = 1; P[0][1] = 0; P[0][2] = 0;
        P[1][0] = 0; P[1][1] = 1; P[1][2] = 0;
        P[2][0] = 0; P[2][1] = 0; P[2][2] = 1;
    } else if (nroots == 2) {


    } else {
    }
}
