#include "vil_exact_distance_transform.h"
//:
// \file
// \brief Euclidean Distance Transform algorithms
// \author Ricardo Fabbri (Brown)
//
// \todo Idea for the future: define these in a class; use of inheritance may be useful e.g. by
// defining a base "distance_tranform" compute class wich abstracts all EDT
// algorithms (independent of the author) and imposes uniformity to their interfaces.
// Otherwise we might end up with multiple DT implementations, each with a
// different interface and differents assumption on the input image (e.g. 0
// pixels as interest or not).

#include <vcl_limits.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>


//: Row-wise 1D EDT 
//
// This is the first step for independent-scanning EDT algorithms.
//
// \param[in,out] im : input image.  The non-zero pixels must have a very large
// value (larger than the number of rows of the image). In the output, non-zero
// pixels will contain minimum distance to the zero-pixels along the rows.
//
// \Remarks
// This particular implementation is based on the 1st part of the following method:
// R. Lotufo and F. Zampirolli, Fast multidimensional parallel euclidean distance 
// transform based on mathematical morphology, in T. Wu and D. Borges, editors,
// Proccedings of SIBGRAPI 2001, XIV Brazilian Symposium on Computer Graphics 
// and Image Processing, pages 100-105. IEEE Computer Society, 2001.
//
inline bool 
vil_exact_distance_transform_1d_horizontal(vil_image_view<vxl_uint_32> &im)
{

   unsigned ni=im.ni(), i,
            nj=im.nj(), j;
   vxl_uint_32 b;


   for (j=0; j < nj; j++) {
      b=1;
      for (i=1; i<ni; i++)
         if (im(i,j) > im(i-1,j) + b) {
            im(i,j) = im(i-1,j) + b;
            b += 2;
         } else
            b = 1;
      b=1;
      for (i=ni-2; i != (vxl_uint_32)-1; i--) {
         if (im(i,j) > im(i+1,j) + b) {
            im(i,j) = im(i+1,j) + b;
            b += 2;
         } else
            b = 1;
      }
   }

   // NOTE: Lotufo's implementation (obtained by requesting him) of this first 
   // part is less readable. Although pointers could be used more efficiently,
   // this first part is much faster than the 2nd part and is not worth
   // optimizing.  So I kept it readable, close to the paper's pseudocode.
   // TODO: VIL uses asserts for bounds in the above code. This is not
   // acceptable for stable code. Optimize this or let the user disable it in
   // CMake?.

   return true;
}

//  --------------------------------------
//    Maurer's EDT Implementation
//  --------------------------------------

//: internal test function as in Maurer's paper
inline bool 
remove_edt(int du, int dv, int dw, 
           int u,  int v,  int w)
{
    // 11 integer expressions
    int a = v - u,
        b = w - v,
        c = w - u;

    return c*dv - b*du - a*dw > a*b*c;
}

//: global variable storing the infinity value for each image
static vxl_uint_32 infty_;

//: Internal function that eliminates unnecessary sites and computes 2D
// Euclidean distances to the nearest sites.
inline bool 
maurer_voronoi_edt_2D(vil_image_view<vxl_uint_32> &im, unsigned j1, int *g, int *h)
{
   int l, ns, tmp0, tmp1, tmp2;
   unsigned i, ni, nj;
   vxl_uint_32 fi;

   ni = im.ni(); nj=im.nj();

   l = -1;
   for (i=0; i < nj; ++i){
      if ((fi = im(j1,i)) != infty_) {
      // TODO: VIL uses asserts for bounds in the above code. Optimize this!
         while ( l >= 1 && remove_edt(g[l-1], g[l], fi, h[l-1], h[l], i) )
            --l;
         ++l; g[l] = fi; h[l] = i;
      }
   }

   // The following are lines 15-25 of the paper
   if ((ns=l) == -1) return true;

   l = 0;
   for (i=0; i < im.nj(); ++i) {
      tmp0 = h[l] - i;
      tmp1 = g[l] + tmp0*tmp0;
      while(true) {
         if (l >= ns) break;

         tmp2 = h[l+1] - i;

         if (tmp1 <= g[l+1] + tmp2*tmp2) break;

         ++l;
         tmp0 = h[l] - i;
         tmp1 = g[l] + tmp0*tmp0;
      }

      im(j1,i) = tmp1;
      // TODO: VIL uses asserts for bounds in the above code. Optimize this!
   }

   return true;
}

//: Internal function that computes 2D EDT given 1D EDT using Maurer's Voronoi
// algorithm for the integer grid.
inline bool
edt_maurer_2D_from_1D(vil_image_view<vxl_uint_32> &im)
{
   bool stat;
   unsigned i1; 
   int *g, *h; // same naming as in the paper

   // Call voronoi_edt_2D for every row.
   // OBS: g and h are internal to maurer_voronoi_edt_2d and are
   // pre-allocated here for efficiency.
   g = new int[im.nj()];
   h = new int[im.nj()];

   for (i1=0; i1 < im.ni(); ++i1) {
      stat = maurer_voronoi_edt_2D(im, i1,  /* internal: */ g, h);
      if (!stat) return false;
   }

   delete [] h;
   delete [] g;

   return true;
}



//: Internal function
inline bool 
test_contiguous(vil_image_view<vxl_uint_32> &im)
{
   if (!im.is_contiguous()) {
#ifndef NDEBUG
      vcl_cerr << "edt: only contiguous row-wise images currently supported\n"; 
#endif
      return false;
   }

   if (im.istep() != 1) {
#ifndef NDEBUG
      vcl_cerr << "edt(2): only contiguous row-wise images currently supported\n"; 
      vcl_cerr << "istep: " << im.istep() << vcl_endl;
#endif
      return false;
   }

   return true;
}

//: Linear-time, 2D exact Euclidean distance transform (Maurer's Algorithm)
//
// This is one of the fastest methods according to a recent survey (distance.sourceforge.net)
// Paper: Calvin Maurer et. al. PAMI feb. 2003 
//
// Squared Euclidean distances are computed in-place for every pixel relative to the zero-pixels.
// 
// \param[in,out] im   The input binary image using vxl_uint_32 storage. The
// squared distance map is output to the same array, since the squared Euclidean
// distances are integers, assuming pixels are unit distance apart.
//
// \remarks Non-contiguous images not currently supported.
//
//  Implementation influenced by the LTI lib, which is licensed under LGPL:
//    http://ltilib.sourceforge.net
//
bool 
vil_exact_distance_transform_maurer(vil_image_view<vxl_uint_32> &im)
{
   unsigned i,r,c;
   vxl_uint_32 *data;

   if (!test_contiguous(im))
      return false;

   r = im.nj();  c = im.ni();
   infty_ = vcl_numeric_limits<vxl_uint_32>::max() - r*r - c*c -1;

   data = im.top_left_ptr(); 
   for (i=0;  i<r*c;  ++i)
      if (data[i])
         data[i] = infty_;

   bool stat;

   // Vertical row-wise EDT
   stat = vil_exact_distance_transform_1d_horizontal(im);
   if (!stat) return false; 

   stat = edt_maurer_2D_from_1D(im);
   if (!stat) return false; 

   return true;
}

//: Cubic-time, exact Euclidean distance transform (Saito's algorithm)
//
// This algorithm is linear-time on average, but it is cubic-time in the worst
// case. It has reasonably good performance compared to newer algorithms,
// according to my experiments (distance.sourceforge.net).
//
// Paper: T. Saito and J.I. Toriwaki, "New algorithms for Euclidean distance
// transformations of an n-dimensional digitised picture with applications",
// Pattern Recognition, 27(11), pp. 1551-1565, 1994
//
// Squared Distances are computed for every pixel relative to the zero-pixels.
//
// \param[in,out] im   The input binary image using vxl_uint_32 storage. The
// squared distance map is output to the same array, since the squared Euclidean
// distances are integers, assuming pixels are unit distance apart.
//
// \remarks Non-contiguous images not currently supported.
//
// Final implementation by Ricardo Fabbri, 
// based on two independent implementations by Olivier Cuisenaire
// and Julio Torelli.
//
bool 
vil_exact_distance_transform_saito(vil_image_view<vxl_uint_32> &im, unsigned plane_idx)
{
  unsigned i,r,c; 
  r = im.nj();  c = im.ni(); 
  unsigned n = r*c;   

  unsigned diag1 = (unsigned)vcl_ceil( vcl_sqrt(double(r*r + c*c)) ) -1;

  unsigned nsqr = 2*(diag1 + 1);   /* was: 2*r + 2 in Cuisenaire's code */
  vcl_vector<unsigned> sq;
  sq.resize(nsqr);
  for (i=0; i<nsqr; ++i)
     sq[i] = i*i;

  infty_ = vcl_numeric_limits<vxl_uint_32>::max() - r*r - c*c -1;

  vxl_uint_32 *data;
  data = im.top_left_ptr(); 
  for (i=0;  i < n;  ++i)
     if (data[i])
        data[i] = infty_;

  return vil_exact_distance_transform_saito(im, plane_idx, sq);
}

//: Exact 3D EDT 
// 
// \remarks See comment on vil_exact_distance_transform_saito
//
bool 
vil_exact_distance_transform_saito_3D(vil_image_view<vxl_uint_32> &im)
{



  unsigned i,r,c,nk; 
  r = im.nj();  c = im.ni(); nk = im.nplanes();
  unsigned n = r*c*nk;   

  unsigned diag1 = (unsigned)vcl_ceil( vcl_sqrt(double(r*r + c*c + nk*nk)) ) -1;

  unsigned nsqr = 2*(diag1 + 1);   /* was: 2*r + 2 in Cuisenaire's code */
  vcl_vector<unsigned> sq;
  sq.resize(nsqr);
  for (i=0; i<nsqr; ++i)
     sq[i] = i*i;

  infty_ = vcl_numeric_limits<vxl_uint_32>::max() - r*r - c*c - nk*nk -1;

  vxl_uint_32 *data;
  data = im.top_left_ptr(); 
  for (i=0;  i < n;  ++i)
     if (data[i])
        data[i] = infty_;

  //: 2D EDT for each plane
  for (unsigned k=0; k < nk; ++k) {
    bool stat = vil_exact_distance_transform_saito(im, k, sq);

    if (!stat)
      return false;
  }


  //: Now, for each pixel, compute final distance by searching along Z direction
  

  unsigned rc = r*c;
  for (unsigned j=0; j < r; ++j, data+=c) {
    vcl_vector<unsigned> buff(nk);
    
    vxl_uint_32 *pt;

    for (unsigned i=0; i < c; ++i) {
      pt = data + i;

      for (unsigned k=0; k < nk; ++k, pt += rc)
        buff[k]=*pt;

      pt = data + i + rc;
      unsigned a = 0;
      unsigned buffer = buff[0];

      for (unsigned k=1; k < nk; ++k, pt += rc) {
        if (a != 0)
          --a;
        if (buff[k] > buffer+1) {
           unsigned b = (buff[k] - buffer-1) / 2;
           if (k+b+1 > nk)
              b = nk-1 -k;

           vxl_uint_32 *npt = pt + a*rc;
           for (unsigned l=a; l<=b; ++l) {
              unsigned m = buffer + sq[l+1];
              if (buff[k+l] <= m)
                 break;   /* go to next plane k */
              if (m < *npt)
                 *npt = m;
              npt += rc;
           }
           a = b;
        } else
           a = 0;
        buffer = buff[k];
      }

      a = 0;
      pt -= 2*rc;
      buffer = buff[nk-1];

      for (unsigned k=nk-2;  k != (unsigned)-1;  --k, pt-=rc) {
         if (a != 0)
            --a;
         if (buff[k] > buffer+1) {
            unsigned b = (buff[k] - buffer-1) / 2;
            if (k < b)
               b = k;

            vxl_uint_32 *npt = pt - a*rc;
            for (unsigned l=a; l<=b; ++l) {
               unsigned m = buffer + sq[l+1];
               if (buff[k-l] <= m)
                  break;   /* go to next column k */
               if (m < *npt)
                  *npt = m;
               npt -= rc;
            }
            a = b;
         } else
            a = 0;
         buffer = buff[k];
      }
    }
  }

  return true;
}

//: Overload that assumes given a Lookup table of integer squares. Also assumes the image im
// already has infinity in all non-zero points.
// 
// \sa see documentation of vil_exact_distance_transform_saito(vil_image_view<vxl_uint_32> &im, unsigned plane_idx=0);
//
bool 
vil_exact_distance_transform_saito(vil_image_view<vxl_uint_32> &im, unsigned plane_idx, const vcl_vector<unsigned> &sq)
{

   if (!test_contiguous(im))
      return false;

   unsigned r,c,nk; //: TODO: unsigned long might be needed for large volumetric data
   r = im.nj();  c = im.ni(); nk = im.nplanes();
   unsigned n = r*c*nk;   


   /* Cuisenaire's idea: a LUT with precomputed i*i TODO move this outside
    * this fn (class? parameter?) */

   // Create a temporary 2D image view for this plane, whose top_left_ptr is
   // data plus r*c

   vxl_uint_32 *data;
   data = im.top_left_ptr(); 
   data += plane_idx*r*c;
   vil_image_view<vxl_uint_32> plane_im;

   plane_im.set_to_memory(data, c, r, 1, 1, c,n);

   // Vertical row-wise EDT
   bool stat;
   stat = vil_exact_distance_transform_1d_horizontal(plane_im);
   if (!stat) return false; 


   // ----------- Step 2 -----------



   vcl_vector<unsigned> buff;
   buff.resize(r);

   
   unsigned *pt;

   for (unsigned i=0; i < c; ++i) {
     pt = data + i;

     for (unsigned j=0; j < r; ++j, pt+=c)
       buff[j]=*pt;

     pt = data + i + c;
     unsigned a = 0;
     unsigned buffer = buff[0];

     for (unsigned j=1; j < r; ++j, pt += c) {
       if (a != 0)
         --a;
       if (buff[j] > buffer+1) {
          unsigned b = (buff[j] - buffer-1) / 2;
          if (j+b+1 > r)
             b = r-1 -j;

          unsigned *npt = pt + a*c;
          for (unsigned l=a; l<=b; ++l) {
             unsigned m = buffer + sq[l+1];
             if (buff[j+l] <= m)
                break;   /* go to next column j */
             if (m < *npt)
                *npt = m;
             npt += c;
          }
          a = b;
       } else
          a = 0;
       buffer = buff[j];
     }

      a = 0;
      pt -= 2*c;
      buffer = buff[r-1];

      for (unsigned j=r-2;  j != (unsigned)-1;  --j, pt-=c) {
         if (a != 0)
            --a;
         if (buff[j] > buffer+1) {
            unsigned b = (buff[j] - buffer-1) / 2;
            if (j < b)
               b = j;

            unsigned *npt = pt - a*c;
            for (unsigned l=a; l<=b; ++l) {
               unsigned m = buffer + sq[l+1];
               if (buff[j-l] <= m)
                  break;   /* go to next column j */
               if (m < *npt)
                  *npt = m;
               npt -= c;
            }
            a = b;
         } else
            a = 0;
         buffer = buff[j];
      }
   }
   

   return true;
}



//  -----------------------------
//    Brute-Force (for testing)
//  -----------------------------

//: Naive implementation of exact Euclidean distance transform (for ground-truth)
//
// Quadratic complexity: O(N^2) for image with N pixels.
// Works for both 3D and 2D.
//
// Squared Euclidean distances are computed in-place for every pixel relative to the zero-pixels.
//
// \param[in,out] im   The input binary image using vxl_uint_32 storage. The
// squared distance map is output to the same array, since the squared Euclidean
// distances are integers, assuming pixels are unit distance apart.
//
//
bool 
vil_exact_distance_transform_brute_force(vil_image_view<vxl_uint_32> &im)
{
  unsigned i, xi, yi, zi,
           j, xj, yj, zj,
           dx,dy, dz, ni,nj,np,
           n, dst;
  vxl_uint_32 *I=im.top_left_ptr();

  if (!test_contiguous(im))
     return false;

  infty_ = vcl_numeric_limits<vxl_uint_32>::max();

  ni = im.ni();
  nj = im.nj();
  unsigned ninj = ni*nj;
  np = im.nplanes();
  n  = ninj*np;

  for (i=0; i<n; ++i)
     if (I[i]) {
        I[i] = infty_;
        zi = i / ninj;
        unsigned idx_2Dimg = i % ninj;
        xi = idx_2Dimg % ni;   yi = idx_2Dimg / ni;

        for (j=0; j<n; ++j)
           if (I[j] == 0) {

              zj = j / ninj;
              unsigned j_idx_2dimg = j % ninj;

              xj  = j_idx_2dimg % ni; yj = j_idx_2dimg / ni; 

              dx  = xi-xj; dy = yi-yj; dz = zi-zj; // ok if its unsigned (modular arithmetic)
              dst = dx*dx + dy*dy + dz*dz;
              if (I[i] > dst)
                 I[i] = dst;
           }
     }

  return true;
}

//: Another Naive implementation of the exact Euclidean distance transform (for ground-truth).
// 
// This implementation uses about 2x more memory than plain brute-force but is faster in
// most cases. It is O(N^2) if the number of white pixels is about the same as
// the number of black pixels. In general, the complexity is between O(N) and
// O(N^2) depending on the content (N is the total number of pixels).
//
// \sa description of vil_exact_distance_transform_brute_force 
//
bool 
vil_exact_distance_transform_brute_force_with_list(vil_image_view<vxl_uint_32> &im)
{
   unsigned i, xi, yi,
            j, xj, yj,
            dx,dy, c,
            n, dst;

   vxl_uint_32 *I=im.top_left_ptr(), *list, n_ones, ptr_zeros;

   if (!test_contiguous(im))
      return false;

   infty_ = vcl_numeric_limits<vxl_uint_32>::max();

   c = im.ni();
   n = im.nj()*c;

   list = new vxl_uint_32[n];

   n_ones = 0;
   ptr_zeros = n-1;
   for (i=0; i<n; ++i)
      if (I[i]) {
         list[n_ones++] = i;
         I[i] = infty_;
      } else
         list[ptr_zeros--] = i;

   for (i=0; i<n_ones; ++i) { // for each 1-pixel
         xi = list[i] % c;   yi = list[i] / c;
         for (j=n_ones; j<n; ++j) { // for each 0-pixel
            xj  = list[j] % c; yj = list[j] / c;

            dx  = xi-xj; dy = yi-yj;  // ok, modular arithmetic
            dst = dx*dx + dy*dy;
            if (I[list[i]] > dst)
               I[list[i]] = dst;
         }
   }

   delete [] list;

   return true;
}

//: Computes signed Euclidean distance transform by using unsigned EDT of an
// image and its binary complement. 
//
// The input image will be modified as an auxiliary array, so if you want to
// keep the input you are responsible for making a copy before calling this
// function.
//
// Regions on input image are pixels where intensity > 0;
// Distance Transform is > 0 INSIDE the regions amd 
//                       < 0 OUTSIDE the regions.
//
// \Remarks The code for combining the interior and exterior distance maps 
// has been blindly adapted from a previous implementation; the original code was from
// a legacy repository at Brown University (algo/contourtracing/signed_dt.cpp)
//
bool 
vil_exact_distance_transform_signed(
    vil_image_view<unsigned int> &input_image, 
    vil_image_view<float> &signed_edt_image) 
{
  float distance_from_interior,
        surface_value, diff;
  unsigned ni = input_image.ni(),
           nj = input_image.nj(),
            n = ni*input_image.nj();
  const float cutoff_margin = 1000.0; // ??
  float *signed_edt;
  unsigned i;


  vxl_uint_32 *image_data = input_image.top_left_ptr();

  signed_edt_image.set_size(ni,nj);
  signed_edt = signed_edt_image.top_left_ptr();

  vil_image_view <vxl_uint_32> dt_complement_img(ni, nj);
  vxl_uint_32 *dt_complement = dt_complement_img.top_left_ptr();

  for (i=0; i<n; ++i)
    dt_complement[i] = !image_data[i];

  vil_exact_distance_transform_maurer(input_image);
  const vxl_uint_32 *dt_input = image_data; // alias, for readability

  vil_exact_distance_transform_maurer(dt_complement_img);

  for (i=0; i<n; ++i) {
    if (image_data[i] == 0) {
      distance_from_interior = (float)((dt_complement[i] >= 1) ?  vcl_sqrt((double)dt_complement[i]-1.0) : 0.0);
      diff = cutoff_margin - distance_from_interior;
      if (diff < 0.0)
        diff = 0.0;
      surface_value = diff - cutoff_margin;
    } else {
      surface_value = vcl_sqrt((float)dt_input[i]); // distance_from_exterior
    }

    signed_edt[i] = (float)(surface_value - 0.5);
  }
  
  return true;
}
