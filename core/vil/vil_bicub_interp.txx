// core/vil/vil_bicub_interp.txx
#ifndef vil_bicub_interp_txx_
#define vil_bicub_interp_txx_
//:
// \file
// \brief Bicubic interpolation functions for 2D images
//
// The vil bicub source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bilin file that would likely also benefit from
// the same change.
//
// In this particular case, there is no corresponding
// vil_bilin_interp.txx file, see vil_bilin_interp.h instead.

#include "vil_bicub_interp.h"
#include <vcl_compiler.h>

// vil_bilin_interp.h defines only inline functions, but some of the
// corresponding vil_bicub_interp functions are a little big to be
// inline.  Plus, on one platform, msvc 6.0 with /O2 optimization
// compiled the vil_bicub_interp functions without a peep but gave
// incorrect numerical results when these functions were inline and
// defined in vil_bicub_interp.h.

template<class T>
double vil_bicub_interp_unsafe(double x, double y, const T* data,
                               vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    int p1x=int(x);
    double normx = x-p1x;
    int p1y=int(y);
    double normy = y-p1y;

    const T* pix1 = data + p1y*ystep + p1x*xstep;

    // like bilinear interpolation, use separability.
    // the s's are for the x-direction and the t's for the y-direction.
    double s0 = ((2-normx)*normx-1)*normx;    // -1
    double s1 = (3*normx-5)*normx*normx+2;    //  0
    double s2 = ((4-3*normx)*normx+1)*normx;  // +1
    double s3 = (normx-1)*normx*normx;        // +2

    double t0 = ((2-normy)*normy-1)*normy;
    double t1 = (3*normy-5)*normy*normy+2;
    double t2 = ((4-3*normy)*normy+1)*normy;
    double t3 = (normy-1)*normy*normy;

#define vil_I(dx,dy) (pix1[(dx)*xstep+(dy)*ystep])

    double xi0 = s0*vil_I(-1,-1) + s1*vil_I(+0,-1) + s2*vil_I(+1,-1) + s3*vil_I(+2,-1);
    double xi1 = s0*vil_I(-1,+0) + s1*vil_I(+0,+0) + s2*vil_I(+1,+0) + s3*vil_I(+2,+0);
    double xi2 = s0*vil_I(-1,+1) + s1*vil_I(+0,+1) + s2*vil_I(+1,+1) + s3*vil_I(+2,+1);
    double xi3 = s0*vil_I(-1,+2) + s1*vil_I(+0,+2) + s2*vil_I(+1,+2) + s3*vil_I(+2,+2);

#undef vil_I

    double val = 0.25 * ( xi0*t0 + xi1*t1 + xi2*t2 + xi3*t3 );

    return val;
}

// see the comments where this variable is used below
#ifdef VCL_VC60
static double vil_bicub_interp_raw_temp_hack = 0.0;
#endif

template<class T>
double vil_bicub_interp_raw(double x, double y, const T* data,
                            vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    int p1x=int(x);
    double normx = x-p1x;
    int p1y=int(y);
    double normy = y-p1y;

    const T* pix1 = data + p1y*ystep + p1x*xstep;

    // special boundary cases can be handled more quickly first; also
    // avoids accessing an invalid pix1[t] which is going to have
    // weight 0.

    if (normx == 0.0 && normy == 0.0) return pix1[0];

    // coefficients for interpolation
    double s0=-1.0, s1=-1.0, s2=-1.0, s3=-1.0;      // in the x-direction
    double t0=-1.0, t1=-1.0, t2=-1.0, t3=-1.0;      // in the y-direction

    if (normx != 0.0) {
        s0 = ((2-normx)*normx-1)*normx;    // -1
        s1 = (3*normx-5)*normx*normx+2;    //  0
        s2 = ((4-3*normx)*normx+1)*normx;  // +1
        s3 = (normx-1)*normx*normx;        // +2
    }

    if (normy != 0.0) {
        t0 = ((2-normy)*normy-1)*normy;    // -1
        t1 = (3*normy-5)*normy*normy+2;    //  0
        t2 = ((4-3*normy)*normy+1)*normy;  // +1
        t3 = (normy-1)*normy*normy;        // +2
    }

#define vil_I(dx,dy) (pix1[(dx)*xstep+(dy)*ystep])

    if (normy == 0.0) {
        double val = 0.0;
        val += s0*vil_I(-1,+0);
        val += s1*vil_I(+0,+0);
        val += s2*vil_I(+1,+0);

#ifdef VCL_VC60
        // On some hardware platforms, with optimization, MSVC 6.0
        // miscompiles the computation of 'val' in this section of
        // code.  It appears that the computation of 'val' is lumped
        // into one large operation that is not handled properly,
        // resulting in incorrect arithmetic.  The very similar
        // section of code below, for the normx==0.0 case works fine.
        // After a lot of experimentation I have found that if we
        // force the compiler to split the computation of 'val' here
        // into two parts by assigning 'val' to a static variable
        // here, then the compilation and tests are OK.  In the past
        // this bug was dealt with by turning optimization off for
        // this file under MSVC, but this is a much better
        // solution. --Fred Wheeler
        vil_bicub_interp_raw_temp_hack = val;
#endif

        val += s3*vil_I(+2,+0);
        val *= 0.5;
        return val;
    }

    if (normx == 0.0) {
        // The computation of 'val' in this section seems to compile
        // fine, even when the very similar computation just above has
        // trouble.
        double val = t0*vil_I(+0,-1) + t1*vil_I(+0,+0) + t2*vil_I(+0,+1) + t3*vil_I(+0,+2);
        val *= 0.5;
        return val;
    }

    double xi0 = s0*vil_I(-1,-1) + s1*vil_I(+0,-1) + s2*vil_I(+1,-1) + s3*vil_I(+2,-1);
    double xi1 = s0*vil_I(-1,+0) + s1*vil_I(+0,+0) + s2*vil_I(+1,+0) + s3*vil_I(+2,+0);
    double xi2 = s0*vil_I(-1,+1) + s1*vil_I(+0,+1) + s2*vil_I(+1,+1) + s3*vil_I(+2,+1);
    double xi3 = s0*vil_I(-1,+2) + s1*vil_I(+0,+2) + s2*vil_I(+1,+2) + s3*vil_I(+2,+2);

#undef vil_I

    double val = 0.25 * ( xi0*t0 + xi1*t1 + xi2*t2 + xi3*t3 );

    return val;
}

#define VIL_BICUB_INTERP_INSTANTIATE(T) \
template double \
vil_bicub_interp_unsafe (double x, double y, const T* data, \
                         vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep); \
template double \
vil_bicub_interp_raw (double x, double y, const T* data, \
                      vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)

#endif // vil_bicub_interp_txx_
