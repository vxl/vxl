// This is oxl/osl/osl_edge_detector.cxx
#include "osl_edge_detector.h"
//:
//  \file

#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_list.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>

#include <osl/osl_canny_base.h>
#include <osl/osl_kernel.h>
#include <osl/osl_canny_smooth.h>
#include <osl/osl_canny_gradient.h>
#include <osl/osl_chamfer.h>
#include <osl/internals/osl_reorder_chain.h>
#include <vnl/vnl_math.h>

const float DUMMYTHETA = 10000.0f;
const float ALPHA = 0.9f;

#define FAR 65535

#define Make_float_image(w, h) osl_canny_base_make_raw_image(w, h, (float*)0)
#define Make_int_image(w, h) osl_canny_base_make_raw_image(w, h, (int*)0)
#define Free_float_image(i,dummy) osl_canny_base_free_raw_image(i)
#define Free_int_image(i,dummy) osl_canny_base_free_raw_image(i)

//-----------------------------------------------------------------------

osl_edge_detector::osl_edge_detector(osl_edge_detector_params const &params)
  : osl_edge_detector_params(params)
{
  //Set up histogram stuff -  old style maintained for compatability
  gradient_histogram_ = false; //Do we need to compute a histogram?
  histogram_resolution_ = 15; // The number of buckets

 width_ = int(sigma_*vcl_sqrt(2*vcl_log(1.0/gauss_tail_))+1); // round up to int
  k_size_ = 2*width_+ 1;
  kernel_ = new float[k_size_];
  max_gradient_ = low_;
  xjunc_ = new vcl_list<int>;
  yjunc_ = new vcl_list<int>;
  vlist_ = new vcl_list<osl_Vertex*>;

  jval_ = 2000.0;

  vertidcount_ = 0;
}

//-----------------------------------------------------------------------------

osl_edge_detector::~osl_edge_detector() {
  Free_float_image(dx_,xsize_);
  Free_float_image(dy_,xsize_);
  Free_float_image(grad_,xsize_);

  Free_float_image(thin_,xsize_);
  Free_float_image(theta_,xsize_);
  Free_float_image(thresh_,xsize_);

  Free_int_image(dist_,xsize_);
  Free_int_image(jx_,xsize_);
  Free_int_image(jy_,xsize_);
  Free_int_image(junction_,xsize_);

  //no point vlist_->clear();
  delete vlist_;
  delete [] kernel_;
  delete xjunc_;
  delete yjunc_;
}


//-----------------------------------------------------------------------------

void osl_edge_detector::detect_edges(vil1_image const &image,
                                     vcl_list<osl_edge*> *edges,
                                     bool maintain_topology)
{
  assert(edges!=0);

  //
  xsize_ = image.height();
  ysize_ = image.width();
  xstart_ = 0;
  ystart_ = 0;

  //vcl_cerr << "xstart_ = " << xstart_ << " ystart_ = " << ystart_ << vcl_endl
  //         << "xsize_ = " << xsize_ << " ysize_ = " << ysize_ << vcl_endl;

  dx_ = Make_float_image(xsize_,ysize_);
  dy_ = Make_float_image(xsize_,ysize_);
  grad_ = Make_float_image(xsize_,ysize_);
  smooth_ = Make_float_image(xsize_,ysize_);

  thin_ = Make_float_image(xsize_,ysize_);
  thresh_ = Make_float_image(xsize_,ysize_);
  theta_ = Make_float_image(xsize_,ysize_);

  dist_ = Make_int_image(xsize_,ysize_);
  junction_ = Make_int_image(xsize_,ysize_);
  jx_ = Make_int_image(xsize_,ysize_);
  jy_ = Make_int_image(xsize_,ysize_);

  if (verbose_)
    vcl_cerr << "Doing canny on image region "
             << xsize_ << " by " << ysize_ << vcl_endl
             << "Gaussian tail = " << gauss_tail_ << vcl_endl
             << "Sigma         = " << sigma_ << vcl_endl
             << "Kernel size   = " << k_size_ << vcl_endl
             << "Threshold     = " << low_ << vcl_endl;

  if (verbose_)
    vcl_cerr << "setting convolution kernel and zeroing images\n";
  osl_kernel_DOG(sigma_, kernel_, k_size_, width_);

  osl_canny_base_fill_raw_image(thin_, xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(thresh_, xsize_, ysize_, low_);
  osl_canny_base_fill_raw_image(theta_, xsize_, ysize_, DUMMYTHETA);

  // A suitably large value (FAR) - perhaps should be more. This represents the
  // distance of a point to the nearest edgel
  osl_canny_base_fill_raw_image(dist_, xsize_, ysize_, FAR);

  // Do the traditional Canny parts, and use non-maximal suppression to
  // set the thresholds.
  if (verbose_) vcl_cerr << "smoothing the image\n";
  osl_canny_smooth_rothwell(image, kernel_, width_, k_size_, smooth_);

  if (verbose_)
    vcl_cerr << "computing x,y derivatives and norm of gradient\n";
  osl_canny_gradient(xsize_, ysize_, smooth_, dx_, dy_, grad_);

  if (verbose_)
    vcl_cerr << "doing sub-pixel interpolation\n";
  Sub_pixel_interpolation();

  if (verbose_) vcl_cerr << "assigning thresholds\n";
  Set_thresholds(); // ghist_ is computed here

  // If we don't want to maintain the strict measurement of the topology
  // (ie. we want to stop the junction regions becoming too extensive), we
  // fill in single pixel holes in the edge description.
  if ( !maintain_topology ) {
    vcl_cerr << "Filling holes\n";
    Fill_holes();
  }

  // Thin the edge image, though keep the original thick one
  if (verbose_) vcl_cerr << "thinning edges\n";
  Thin_edges();

  // Locate junctions in the edge image and joint the clusters together
  // as we have no confidence in the geometry around them.
  if (verbose_)
    vcl_cerr << "locating junctions in the edge image - ";
  Find_junctions();
  if (verbose_)
    vcl_cerr << xjunc_->size() << " junctions found\n";

  Find_junction_clusters();
  if (verbose_)
    vcl_cerr << vlist_->size() << " junction clusters found\n";

  // Finally do edge following to extract the edge data from the thin_ image
  if (verbose_) vcl_cerr << "doing final edge following\n";
  Follow_curves(edges);

  if (verbose_) vcl_cerr << "finished osl_edge_detector\n";
}

//-----------------------------------------------------------------------------
//:
// A procedure that performs sub-pixel interpolation for all edges greater
// than the threshold by parabolic fitting. Writes edges into the thresh_ image
// if they are maxima and above low_. This gives a good indication of the local
// edge strengths. Stores sub-pixel positions in dx_ and dy_, and set the
// orientations in theta_.
//
void osl_edge_detector::Sub_pixel_interpolation() {
  float h1=0.0,h2=0.0; // dummy initialisation values
  float k = 180.0f/float(vnl_math::pi);
  int orient;
  float theta,grad;
  float fraction,dnewx=0.0,dnewy=0.0; // dummy initialisation values

  // Add 1 to get rid of border effects.
  for (unsigned int x=width_+1; x+width_+1<xsize_; ++x)
  {
    float *g0 = grad_[x-1];
    float *g1 = grad_[x];
    float *g2 = grad_[x+1];
    float *dx = dx_[x];
    float *dy = dy_[x];

    for (unsigned int y=width_+1; y+width_+1<ysize_; ++y)
    {
      // First check that we have a potential edge
      if ( g1[y] > low_ ) {
        theta = k*(float)vcl_atan2(dy[y],dx[y]);

        // Now work out which direction wrt the eight-way
        // neighbours the edge normal points
        if ( theta >= 0.0 )
          orient = int(theta/45.0);
        else
          orient = int(theta/45.0+4);
        // if theta == 180.0 we will have orient = 4
        orient = orient%4;

        // And now compute the interpolated heights
        switch( orient ) {
        case 0:
          grad = dy[y]/dx[y];
          h1 = grad*g0[y-1] + (1 - grad)*g0[y];
          h2 = grad*g2[y+1] + (1 - grad)*g2[y];
          break;

        case 1:
          grad = dx[y]/dy[y];
          h1 = grad*g0[y-1] + (1 - grad)*g1[y-1];
          h2 = grad*g2[y+1] + (1 - grad)*g1[y+1];
          break;

        case 2:
          grad = -dx[y]/dy[y];
          h1 = grad*g2[y-1] + (1 - grad)*g1[y-1];
          h2 = grad*g0[y+1] + (1 - grad)*g1[y+1];
          break;

        case 3:
          grad = -dy[y]/dx[y];
          h1 = grad*g2[y-1] + (1 - grad)*g2[y];
          h2 = grad*g0[y+1] + (1 - grad)*g0[y];
          break;

        default:
          vcl_abort();
          //vcl_cerr << "*** ERROR ON SWITCH IN NMS ***\n";
        }

        // Do subpixel interpolation by fitting a parabola
        // along the NMS line and finding its peak
        fraction = (h1-h2)/(2*(h1-2*g1[y]+h2));
        switch( orient ) {
        case 0:
          dnewx = fraction;
          dnewy = dy[y]/dx[y]*fraction;
          break;

        case 1:
          dnewx = dx[y]/dy[y]*fraction;
          dnewy = fraction;
          break;

        case 2:
          dnewx = dx[y]/dy[y]*fraction;
          dnewy = fraction;
          break;

        case 3:
          dnewx = - fraction;
          dnewy = - dy[y]/dx[y]*fraction;
          break;

        default:
          vcl_abort();
          //vcl_cerr << "*** ERROR ON SWITCH IN NMS ***\n";
        }

        // Now store the edge data, re-use dx_[][] and dy_[][]
        // for sub-pixel locations (don't worry about the junk
        // that is already in them). Use any edgels that get
        // non-maximal suppression to bootstrap the image
        // thresholds. The >= is used rather than > for reasons
        // involving non-generic images. Should this be interpolated
        // height  = g1[y] + frac*(h2-h1)/4 ?
        if ( g1[y]>=h1 && g1[y]>=h2 && vcl_fabs(dnewx)<=0.5 && vcl_fabs(dnewy)<=0.5 )
        {
          if ( g1[y]*ALPHA > low_ )
            thresh_[x][y] = ALPHA * g1[y]; // Use a ALPHA% bound
          // thresh_ image starts off as being equal to low_
          // else
          //   thresh_[x][y] = low_;
          Thicken_threshold(x,y);
        }

        // + 0.5 is to account for targetjr display offset
        if ( (vcl_fabs(dnewx)<=0.5) && (vcl_fabs(dnewy)<=0.5) ) {
          dx[y] = x + dnewx + 0.5f;
          dy[y] = y + dnewy + 0.5f;
        }
        else {
          dx[y] = x + 0.5f;
          dy[y] = y + 0.5f;
        }
        theta_[x][y] = theta;
      }
      // For consistency assign these values even though the
      // edge is below strength.
      else {
        dx[y] = x + 0.5f;
        dy[y] = y + 0.5f;
      }
    }
  }

  // Clean up around the border to ensure consistency in the dx_ and dy_ values.
  for (unsigned int x=0; x<xsize_; ++x) {
    for (unsigned int y=0; y<=width_; ++y) {
      dx_[x][y] = x + 0.5f;
      dy_[x][y] = y + 0.5f;
    }
    for (int y=ysize_-width_-1; y<int(ysize_); ++y)
    {
      dx_[x][y] = x + 0.5f;
      dy_[x][y] = y + 0.5f;
    }
  }

  for (unsigned int y=width_+1; y+width_+1<ysize_; ++y)
  {
    for (unsigned int x=0; x<=width_; ++x)  {
      dx_[x][y] = x + 0.5f;
      dy_[x][y] = y + 0.5f;
    }
    for (int x=xsize_-width_-1; x<int(xsize_); ++x)
    {
      dx_[x][y] = x + 0.5f;
      dy_[x][y] = y + 0.5f;
    }
  }
}


//-----------------------------------------------------------------------------
//:
// Thickens the threshold image around each good pixel to take account for
// the smoothing kernel (almost a dilation with a square structuring element).
//
void osl_edge_detector::Thicken_threshold(int x, int y) {
  // Experimental change 13/4/95 by CAR
  int width = width_;
  //    int width = 0;

  for (int i=x-width; i<=x+width; ++i)
    for (int j=y-width; j<=y+width; ++j)  {

      dist_[i][j] = 0;
      if ( thresh_[i][j] != low_ )
        thresh_[i][j] = vcl_min(thresh_[x][y], thresh_[i][j]);
      else
        thresh_[i][j] = thresh_[x][y];
    }
}


//-----------------------------------------------------------------------------
//:
// Takes the thresh_ image that contains threshold values near to where
// non-maximal suppression succeeded, and zero elsewhere, and extend the
// values to all areas of the image. This is done using chamfer masks so that
// the final threshold assigned at any one point (ie. a point that was
// initially zero) is functionally dependent on the the strengths of the
// nearest good edges. At present we linearly interpolate between the two
// (approximately) closest edges.
//
// Try to do the same process using Delauney triangulation (CAR, March 1995), in
// an attempt to image the efficiency from a memory management point of view.
// However, the triangulation becomes so complex that the computation time
// becomes incredibly long. Therefore putting up with the Chamfer method for
// the moment.
//
// The histogram calculation was added to support
// edgel change detection-JLM May 1995
//
void osl_edge_detector::Set_thresholds()
{
  int** fdist = Make_int_image(xsize_,ysize_);
  int** bdist = Make_int_image(xsize_,ysize_);
  int**a1dist = Make_int_image(xsize_,ysize_);
  int**a2dist = Make_int_image(xsize_,ysize_);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(int const*const*, dist_), fdist, xsize_, ysize_);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(int const*const*, dist_), bdist, xsize_, ysize_);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(int const*const*, dist_),a1dist, xsize_, ysize_);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(int const*const*, dist_),a2dist, xsize_, ysize_);

  float** fth = Make_float_image(xsize_,ysize_);
  float** bth = Make_float_image(xsize_,ysize_);
  float**a1th = Make_float_image(xsize_,ysize_);
  float**a2th = Make_float_image(xsize_,ysize_);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const *const*, thresh_), fth, xsize_, ysize_);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const *const*, thresh_), bth, xsize_, ysize_);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const *const*, thresh_),a1th, xsize_, ysize_);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const *const*, thresh_),a2th, xsize_, ysize_);

  osl_chamfer_Forward (xsize_, ysize_, fdist, fth);
  osl_chamfer_Backward(xsize_, ysize_, bdist, bth);
  osl_chamfer_Alt1(xsize_, ysize_, a1dist, a1th);
  osl_chamfer_Alt2(xsize_, ysize_, a2dist, a2th);

  // The range of the effect of the smoothing kernel, including the scale factor
  // we have ignored up to now for the chamfer masks
  //    int range = 3*width_;
  //    float max_gradient = low_;  //Commented out May 1997- JLM
  //Replaced by global max_gradient_
  for (unsigned int x=0; x<xsize_; ++x) {
    for (unsigned int y=0; y<ysize_; ++y) {

      if ( thresh_[x][y] == low_ ) {

        // Determine the two closest edge points.
        int option = osl_Minimum4(fdist[x][y],
                                  bdist[x][y],
                                  a1dist[x][y],
                                  a2dist[x][y]);
        float num=1.0f; int den=1; // dummy initialisation values
        switch(option) {
        case 1:
        case 2:
          den = fdist[x][y]+bdist[x][y];
          num = bdist[x][y]*fth[x][y]+fdist[x][y]*bth[x][y];
          break;

        case 3:
        case 4:
          den = a1dist[x][y]+a2dist[x][y];
          num = a2dist[x][y]*a1th[x][y]+a1dist[x][y]*a2th[x][y];
          break;

        default:
          vcl_abort();
          //break;
        }
        if ( den != 0.0 )
          thresh_[x][y] = num / den;
        else if ( thresh_[x][y] <= low_ )
          thresh_[x][y] = low_;
      }

      if ( grad_[x][y]>thresh_[x][y] ) {
        //changed to max_gradient_, global - May 1997 JLM
        if (grad_[x][y]>max_gradient_)
          max_gradient_=grad_[x][y];
        thin_[x][y] = grad_[x][y];
      }
    }
  }
  // Noticed that all gradient values are used in edgel Strength Histogram - May 1997
  // So defer to actual edgel chain formation.
#if 0 // commented out
  if (gradient_histogram_)
        {
            ghist_ =
              new Histogram(histogram_resolution_, low_, max_gradient);
            for (x=0; x<xsize_; ++x)
                for (y=0; y<ysize_; ++y)
                  //ghist_->UpCount(grad_[x][y]); //All Pixels (Used since 1995)
                  ghist_->UpCount(thin_[x][y]);  //Just at edgels (First check
        }                                        //for significant differences)
#endif

  Free_int_image(fdist,xsize_);
  Free_int_image(bdist,xsize_);
  Free_int_image(a1dist,xsize_);
  Free_int_image(a2dist,xsize_);
  Free_float_image(fth,xsize_);
  Free_float_image(bth,xsize_);
  Free_float_image(a1th,xsize_);
  Free_float_image(a2th,xsize_);
}

struct osl_edge_detector_xyfloat {
  int x;
  int y;
  float thin;
};
static int compare(osl_edge_detector_xyfloat* xyf1, osl_edge_detector_xyfloat* xyf2)
{
  if (xyf1->thin < xyf2->thin)
    return -1;
  if (xyf1->thin == xyf2->thin)
    return 0;
  return 1;
}


//-----------------------------------------------------------------------------
//:
// Method to thin the image using the variation of Tsai-Fu thinning used
// by Van-Duc Nguyen in Geo-Calc. This relies on computing the genus of
// an edge location, and removing it if it is not a dangling chain as has
// genus zero. We also order the edges by strength and try to remove the weaker
// ones first. This accounts for non-maximal suppression, and does it in a
// topology preserving way. Note that we are creating a vcl_list with a large
// number of elements, and then sorting it - this is likely to be quite slow.
// An alternative implementation would be better.
//
void osl_edge_detector::Thin_edges()
{
  // Find all of the edgels with a strength > low_
  bool do_output = true;

  vcl_cerr << __FILE__ ": Fast Sort\n";
  osl_edge_detector_xyfloat* edgel_array = new osl_edge_detector_xyfloat[xsize_ * ysize_];
  int count = 1;     // count set to dummy, nonzero value
  while ( count!=0 ) //  Thin until no Pixels are removed
  {
    count = 0;
    int edgel_array_len = 0;
    for (unsigned int x=width_; x+width_<xsize_; ++x)
      for (unsigned int y=width_; y+width_<ysize_; ++y)
        if ( thin_[x][y] > thresh_[x][y] )
        {
          edgel_array[edgel_array_len].x = x;
          edgel_array[edgel_array_len].y = y;
          edgel_array[edgel_array_len].thin = thin_[x][y];
          edgel_array_len++;
        }

    // Now sort the list; this could be slow if we have a lot of potential.
    // edges - surely we have to do number of elements (not -1)?
    //      qsort(edgel_array, edgel_array_len-1, sizeof(osl_edge_detector_xyfloat), &compare);
    vcl_qsort(edgel_array,
              edgel_array_len,
              sizeof(osl_edge_detector_xyfloat),
              (int (*)(const void *, const void *))&compare);

    // To assist in setting the thresholds:
    if (  do_output && (edgel_array_len > 0) ) {

      vcl_cerr << "edgel strengths range from "
               << edgel_array[0].thin << " to "
               << edgel_array[edgel_array_len-1].thin << vcl_endl;
      do_output = false;
    }

    // Do the thinning taking the weakest edges first and works
    // up through the list strengthwise.
    for (int pos=0; pos<edgel_array_len; ++pos)
    {
      int x = edgel_array[pos].x;
      int y = edgel_array[pos].y;

      int a = ( thin_[x-1][y-1] > thresh_[x-1][y-1] ) ? 1 : 0;
      int b = ( thin_[x  ][y-1] > thresh_[x  ][y-1] ) ? 1 : 0;
      int c = ( thin_[x+1][y-1] > thresh_[x+1][y-1] ) ? 1 : 0;
      int d = ( thin_[x+1][y  ] > thresh_[x+1][y  ] ) ? 1 : 0;
      int e = ( thin_[x+1][y+1] > thresh_[x+1][y+1] ) ? 1 : 0;
      int f = ( thin_[x  ][y+1] > thresh_[x  ][y+1] ) ? 1 : 0;
      int g = ( thin_[x-1][y+1] > thresh_[x-1][y+1] ) ? 1 : 0;
      int h = ( thin_[x-1][y  ] > thresh_[x-1][y  ] ) ? 1 : 0;

      int genus = a+b+c+d+e+f+g+h;

      // Continue if the pixel is not dangling.
      if ( (genus!=1) && (genus!=8) ) {

        genus += h*a*b+b*c*d+d*e*f+f*g*h
               - a*b-b*c-c*d-d*e-e*f-f*g
               - g*h-h*a-h*b-b*d-d*f-f*h-1;

        // If the genus is zero delete the edge
        if ( genus == 0 ) {
          count++;
          thin_[x][y] = 0.0;
        }
      }
    }
  }

  delete [] edgel_array;
}


//-----------------------------------------------------------------------------
//:
// Finds all pixels that are surrounded by four edgels, but which are
// themselves not edgels. These `holes' cause the construction of complex
// topological descriptions. To simplify matters, we raise the thin_ value
// of the central pixel and so force it to be an edgel.
//
void osl_edge_detector::Fill_holes() {
  // Find all of the edgels with a strength <= thresh_
  float SMALL = 0.0001f;

  for (unsigned int x=width_; x+width_<xsize_; ++x)
    for (unsigned int y=width_; y+width_<ysize_; ++y)
      if ( thin_[x][y] <= thresh_[x][y] )
      {
        int count = 0;
        if ( thin_[x  ][y-1] > thresh_[x  ][y-1] ) count++;
        if ( thin_[x  ][y+1] > thresh_[x  ][y+1] ) count++;
        if ( thin_[x+1][y  ] > thresh_[x+1][y  ] ) count++;
        if ( thin_[x-1][y  ] > thresh_[x-1][y  ] ) count++;

        if ( count == 4 )
          thin_[x][y] = thresh_[x][y] + SMALL;
      }
}


//-----------------------------------------------------------------------------

// see osl_canny_ox.cxx
extern osl_Vertex *osl_find(vcl_list<osl_Vertex*> const *l, osl_Vertex const &v);

//:
// Follow all edgel chains that have pixel values above their corresponding
// threshold values (thin_[x][y] > thresh_[x][y]).
//
void osl_edge_detector::Follow_curves(vcl_list<osl_edge*> *edges)
{
  //  //Added May 1997 to restrict histogram to actual detected edgels -JLM
  //  if (gradient_histogram_)
  //    ghist_ = new Histogram(histogram_resolution_, low_, max_gradient_);

  vcl_list<int> xcoords,ycoords;
  vcl_list<float> grad;

  chain_no_ = 10;  // Must be set to a number >= 1

  // Find an edgel point and start to follow it.
  edges->clear();
  for (unsigned int x=width_; x+width_<xsize_; ++x)
  {
    float *thin = thin_[x];
    for (unsigned int y=width_; y+width_<ysize_; ++y)
    {
      if ( (thin[y]<=thresh_[x][y]) || junction_[x][y] )
        continue;

      // Set the following variable [what a pointless comment]
      chain_no_++;

      // clear lists before following
      xcoords.clear();
      ycoords.clear();
      grad.clear();

      Follow(x,y,&xcoords,&ycoords,&grad,0);

      // We may have picked up the edgel chain somewhere
      // away from its ends. Therefore, reverse the list
      // and try to follow again.
      xcoords.reverse();  ycoords.reverse();  grad.reverse();
      Follow(x,y,&xcoords,&ycoords,&grad,1);

      // Check that we have at least two endpoints to
      // the list, otherwise go to next loop
      if ( xcoords.size() < 2 )
        // vcl_cerr << "short list found in Final_follow\n";
        continue;

      int count=0; // isn't this just "count = grad.size()" ?
      for (vcl_list<float>::iterator i=grad.begin(); i!=grad.end(); ++i)
        count++;

      // If the count is less than two we cannot accept
      // the edgelchain. Smallest chain must either be `ee',
      // `je', `ej' or `jj' (for `e'dge and `j'unction)
      if ( count < 2 )
        continue;

      // Create an osl_edgel_chain
      osl_edgel_chain *dc = new osl_edgel_chain(count);
      float *px = dc->GetX();
      float *py = dc->GetY();
      float *pg = dc->GetGrad();
      float *pt = dc->GetTheta();

      // Write the points to the osl_edgel_chain
      while (count) {
        int tmpx = xcoords.front(); xcoords.pop_front();
        int tmpy = ycoords.front(); ycoords.pop_front();
        float val = grad.front(); grad.pop_front();
        count--;

        // If we are not at a junction use sub-pixel value.
        if ( val != jval_ ) {
          *(px++) = dx_[tmpx][tmpy] + xstart_;
          *(py++) = dy_[tmpx][tmpy] + ystart_;
          *(pg++) = val;
          //     if (ghist_)
          //       ghist_->UpCount(val); //Added edgel histogram here -May 1997
        }
        else {
          *(px++) = float(tmpx + xstart_);
          *(py++) = float(tmpy + ystart_);
          *(pg++) = 0.0f;   // Mark the gradient as zero at a junction
        }
        if (theta_[tmpx][tmpy] == DUMMYTHETA) {
          const float k = 180.0f/float(vnl_math::pi);
          theta_[tmpx][tmpy]  = k*(float)vcl_atan2(dy_[tmpx][y],dx_[tmpx][y]);
        }

        *(pt++) = theta_[tmpx][tmpy];
      }

      // Just check whether we have created a trivial edgechain
      // (can happen due to the presence of dummy points)
      if ( (dc->size()==2) &&
           (dc->GetX(0)==dc->GetX(1)) &&
           (dc->GetY(0)==dc->GetY(1)) ) {
        delete dc;
        continue;
      }

      else if ( dc->size() > 1 ) {
        // Create a edge for the image topology
        osl_Vertex *v1 = new osl_Vertex(dc->GetX(0),dc->GetY(0));
        v1->SetId(vertidcount_++);
        osl_Vertex *v2 = new osl_Vertex(dc->GetX(dc->size()-1),dc->GetY(dc->size()-1));
        v2->SetId(vertidcount_++);
        // Check whether each vertex is a junction
        osl_Vertex *V1 = osl_find(vlist_, *v1);
        osl_Vertex *V2 = osl_find(vlist_, *v2);

        // If neither are junctions we may have formed a single isolated
        // chain that should have common vertex endpoints.
        int single_chain = false;
        if ( !V1 && !V2 ) {
          float dx = dc->GetX(0) - dc->GetX(dc->size()-1);
          float dy = dc->GetY(0) - dc->GetY(dc->size()-1);
          if ( dx*dx+dy*dy<4 )  { //  ie. dist < 2 pixels it is closed
            osl_reorder_chain(dc);
            osl_IUDelete(v1);
            osl_IUDelete(v2);
            v1 = new osl_Vertex(dc->GetX(0),dc->GetY(0));
            v1->SetId(vertidcount_++);
            V1 = v1;  V2 = v1;
            single_chain = true;
          }
        }
        if ( !single_chain ) {
          if ( !V1 )
            V1 = v1;
          else
            osl_IUDelete (v1);
          if ( !V2 )
            V2 = v2;
          else
            osl_IUDelete(v2);
        }
        //edge = new osl_edge(V1,V2);

        // Note that the edge can start and end in the same place.
        // However, if this is so the DigitalCurve has positive length
        //dc->SetStart(dc->GetX(0), dc->GetY(0));
        //dc->SetEnd(dc->GetX(dc->size()-1), dc->GetY(dc->size()-1));

        //edge->SetCurve(dc);
        edges->push_front(new osl_edge(*dc, V1, V2));
        delete dc;
      }
    }
  }
}

//-----------------------------------------------------------------------------
//:
// Following routine looking for connectiveness of edgel chains, and
// accounts for single pixel gaps in the chains.
//
void osl_edge_detector::Follow(int x, int y,
                               vcl_list<int> *xc,
                               vcl_list<int> *yc,
                               vcl_list<float> *grad,
                               int reverse)
{
  // Make sure that we do not overun the border of the image
  assert( x>0 && y>0 );
  assert( (unsigned int)x+1<xsize_ );
  assert( (unsigned int)y+1<ysize_ );

  // Add the current point to the coordinate lists, and delete from
  // the edge image
  if (!reverse) {
    xc->push_front(x);
    yc->push_front(y);
    grad->push_front(thin_[x][y]);
  }
  thin_[x][y] = 0.0;

  // The order of traversal is (x axis to the right and y axis down) :
  //    5 0 4
  //    1 * 3
  //    6 2 7
  // Have to be careful with the ; in the following macros. Don't
  // emacs indent this function!
  if (false) { }

  // Now recursively look for connected eight-neighbours.
#define smoo(a, b) \
  else if ( (thin_[a][b]>thresh_[a][b]) && (junction_[a][b]==0) ) \
    Follow(a, b, xc,yc, grad, 0);
  smoo(x  , y-1)
  smoo(x-1, y  )
  smoo(x  , y+1)
  smoo(x+1, y  )
  smoo(x+1, y-1)
  smoo(x-1, y-1)
  smoo(x-1, y+1)
  smoo(x+1, y+1)
#undef smoo

  // Else see if there is a junction nearby, and record it. The chain_no_
  // variable is used to prevent the same junction being inserted at both
  // ends of the edgel chains when reversal occurs next to the junction
  // (in that case there will only be two stored points: the edge and the junction)
#define smoo(a, b) \
  else if ( junction_[a][b] && ((xc->size()>2) || (junction_[a][b]!=chain_no_)) ) { \
    xc->push_front(jx_[a][b]); \
    yc->push_front(jy_[a][b]); \
    grad->push_front(jval_); \
    junction_[a][b] = chain_no_; \
  }
  smoo(x  , y-1)
  smoo(x-1, y  )
  smoo(x  , y+1)
  smoo(x+1, y  )
  smoo(x+1, y-1)
  smoo(x-1, y-1)
  smoo(x-1, y+1)
  smoo(x+1, y+1)
#undef smoo
  else {
    // ? FIXME
  }
}


//-----------------------------------------------------------------------------
//
//: Searches for the junctions in the image.
//
void osl_edge_detector::Find_junctions() {
  // Reset the junction variables
  xjunc_->clear();
  yjunc_->clear();
  osl_canny_base_fill_raw_image(junction_, xsize_, ysize_, 0);

  for (unsigned int x=width_; x+width_<xsize_; ++x)
    for (unsigned int y=width_; y+width_<ysize_; ++y)
    {
      if ( thin_[x][y] <= thresh_[x][y] )
        continue;

      int count = 0;
      if ( thin_[x-1][y-1] > thresh_[x-1][y-1] )  count++;
      if ( thin_[x  ][y-1] > thresh_[x  ][y-1] )  count++;
      if ( thin_[x+1][y-1] > thresh_[x+1][y-1] )  count++;
      if ( thin_[x+1][y  ] > thresh_[x+1][y  ] )  count++;
      if ( thin_[x+1][y+1] > thresh_[x+1][y+1] )  count++;
      if ( thin_[x  ][y+1] > thresh_[x  ][y+1] )  count++;
      if ( thin_[x-1][y+1] > thresh_[x-1][y+1] )  count++;
      if ( thin_[x-1][y  ] > thresh_[x-1][y  ] )  count++;

      if ( count > 2 ) {
        xjunc_->push_front(x);
        yjunc_->push_front(y);
        junction_[x][y] = 1;
      }
    }
}


//-----------------------------------------------------------------------------
//
//: Locate junction clusters using the following method of hysteresis.
//
//
void osl_edge_detector::Find_junction_clusters() {
  vcl_list<int> xcoords,ycoords,xvertices,yvertices,xjunc,yjunc;

  // Find a junction and follow
  xvertices.clear();
  yvertices.clear();
  xjunc.clear();
  yjunc.clear();
  for (unsigned int x=width_; x+width_<xsize_; ++x)
  {
    for (unsigned int y=width_; y+width_<ysize_; ++y)
    {
      if ( junction_[x][y] ) {

        // Each cluster is written to (xcoords,ycooords)
        xcoords.clear();  ycoords.clear();
        Follow_junctions(x,y,&xcoords,&ycoords);

        // Find the `centre' of the cluster. Look at the method
        // Cluster_centre to see how `centre' is defined.
        int x0, y0;
        Cluster_centre(xcoords,ycoords,x0,y0);

        // Add both the junctions and the new cluster centre to
        // the main lists
        xvertices.push_front(x0);
        yvertices.push_front(y0);
        xjunc.insert(xjunc.begin(), xcoords.begin(), xcoords.end()); //xjunc.prepend(xcoords);
        yjunc.insert(yjunc.begin(), ycoords.begin(), ycoords.end()); //yjunc.prepend(ycoords);
      }
    }
  }

  // Reset the junction image - this is order dependent because
  // the cluster centres appear in both lists
  // xjunc.reset();  yjunc.reset();
  while ( xjunc.size() ) {
    junction_[xjunc.front()][yjunc.front()] = 1;
    xjunc.pop_front();
    yjunc.pop_front();
  }

  // Construct the list of junction cluster centres
  vlist_->clear();
  for (vcl_list<int>::iterator i=xvertices.begin(), j=yvertices.begin();
       i!=xvertices.end() && j!=yvertices.end();
       ++i, ++j) {
    //for (xvertices.reset(),yvertices.reset(); xvertices.next(),yvertices.next(); )  {

    osl_Vertex *v = new osl_Vertex( float((*i)/*xvertices.value()*/+xstart_),
                                    float((*j)/*yvertices.value()*/+ystart_));
    vlist_->push_front(v);
    junction_[(*i)/*xvertices.value()*/][(*j)/*yvertices.value()*/] = 2;
  }

  xvertices.clear();
  yvertices.clear();
}


//-----------------------------------------------------------------------------
//
//: Following routine looking for searching out junction clusters.
//
void osl_edge_detector::Follow_junctions(int x, int y, vcl_list<int> *xc, vcl_list<int> *yc)
{
  // Add the current junction to the coordinate lists, and delete from
  // the junction image
  xc->push_front(x);
  yc->push_front(y);
  junction_[x][y] = 0;

  // Now recursively look for connected eight-neighbours
  //    5 0 4
  //    1 * 3
  //    6 2 7
#define smoo(a, b) \
  if ( junction_[a][b] ) Follow_junctions(a,b, xc,yc);
  smoo(x  , y-1)
  smoo(x-1, y  )
  smoo(x  , y+1)
  smoo(x+1, y  )
  smoo(x+1, y-1)
  smoo(x-1, y-1)
  smoo(x-1, y+1)
  smoo(x+1, y+1)
#undef smoo
}


//-----------------------------------------------------------------------------
//
//: Finds which member of the lists lies closest to the centre of the list.
//
//
void osl_edge_detector::Cluster_centre(vcl_list<int> &xc,
                                       vcl_list<int> &yc,
                                       int &x0,
                                       int &y0)
{
  if ( xc.empty() )
    return;

#if 0 // commented out
  // First find the CofG
  double x=0.0,y=0.0;
  for (xc.reset(),yc.reset(); xc.next(),yc.next(); )
    {
      x += xc.value();  y += yc.value();
    }
  x /= xc.size();  y /= yc.size();

  // Now find the point closest to the CofG
  float dist,newdist;
  dist = xsize_*ysize_; // A number larger than the image size
  for (xc.reset(),yc.reset(); xc.next(),yc.next(); )
    if ( (newdist=hypot(x-xc.value(),y-yc.value())) < dist )
      {
        x0 = xc.value();  y0 = yc.value();
        dist = newdist;
      }
#endif

  typedef vcl_list<int>::iterator it;

  // Define the centre as the point with the highest gradient value.
  float grad = -1.0;  // Negative is smaller than the smallest norm of gradient
  for (it i=xc.begin(),j=yc.begin(); i!=xc.end() && j!=yc.end(); ++i, ++j)
    //xc.reset(),yc.reset(); xc.next(),yc.next(); )
    if ( grad_[(*i)/*xc.value()*/][(*j)/*yc.value()*/] > grad ) {
      grad = grad_[(*i)/*xc.value()*/][(*j)/*yc.value()*/];
      x0 = (*i);//xc.value();
      y0 = (*j);//yc.value();
    }

  // Set up the (jx_,jy_) arrays to point to the cluster centre
  for (it i=xc.begin(), j=yc.begin(); i!=xc.end() && j!=yc.end(); ++i, ++j) {
    //xc.reset(),yc.reset(); xc.next(),yc.next(); )  {
    jx_[(*i)/*xc.value()*/][(*j)/*yc.value()*/] = x0;
    jy_[(*i)/*xc.value()*/][(*j)/*yc.value()*/] = y0;
  }
}

//-----------------------------------------------------------------------------

