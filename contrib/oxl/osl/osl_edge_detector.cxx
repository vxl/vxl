#include "osl_edge_detector.h"

//:
//  \file

#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_list.h>
//#include <vcl_memory.h>
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
  _gradient_histogram = false; //Do we need to compute a histogram?
  _histogram_resolution = 15; // The number of buckets

 _width = int(_sigma*vcl_sqrt(2*vcl_log(1.0/_gauss_tail))+1); // round up to int
  _k_size = 2*_width+ 1;
  _kernel = new float[_k_size];
  _max_gradient = _low;
  _xjunc = new vcl_list<int>;
  _yjunc = new vcl_list<int>;
  _vlist = new vcl_list<osl_Vertex*>;

  _jval = 2000.0;

  _vertidcount = 0;
}

//-----------------------------------------------------------------------------

osl_edge_detector::~osl_edge_detector() {
  Free_float_image(_dx,_xsize);
  Free_float_image(_dy,_xsize);
  Free_float_image(_grad,_xsize);

  Free_float_image(_thin,_xsize);
  Free_float_image(_theta,_xsize);
  Free_float_image(_thresh,_xsize);

  Free_int_image(_dist,_xsize);
  Free_int_image(_jx,_xsize);
  Free_int_image(_jy,_xsize);
  Free_int_image(_junction,_xsize);

  //no point _vlist->clear();
  delete _vlist;
  delete [] _kernel;
  delete _xjunc;
  delete _yjunc;
}


//-----------------------------------------------------------------------------

void osl_edge_detector::detect_edges(vil_image const &image,
                                     vcl_list<osl_edge*> *edges,
                                     bool maintain_topology)
{
  assert(edges!=0);

  //
  _xsize = image.height();
  _ysize = image.width();
  _xstart = 0;
  _ystart = 0;

  //vcl_cerr << "_xstart = " << _xstart << " _ystart = " << _ystart << vcl_endl;
  //vcl_cerr << "_xsize = " << _xsize << " _ysize = " << _ysize << vcl_endl;

  _dx = Make_float_image(_xsize,_ysize);
  _dy = Make_float_image(_xsize,_ysize);
  _grad = Make_float_image(_xsize,_ysize);
  _smooth = Make_float_image(_xsize,_ysize);

  _thin = Make_float_image(_xsize,_ysize);
  _thresh = Make_float_image(_xsize,_ysize);
  _theta = Make_float_image(_xsize,_ysize);

  _dist = Make_int_image(_xsize,_ysize);
  _junction = Make_int_image(_xsize,_ysize);
  _jx = Make_int_image(_xsize,_ysize);
  _jy = Make_int_image(_xsize,_ysize);

  if (_verbose)
    vcl_cerr << "Doing canny on image region "
         << _xsize << " by " << _ysize << vcl_endl
         << "Gaussian tail = " << _gauss_tail << vcl_endl
         << "Sigma         = " << _sigma << vcl_endl
         << "Kernel size   = " << _k_size << vcl_endl
         << "Threshold     = " << _low << vcl_endl;

  if (_verbose) vcl_cerr << "setting convolution kernel and zeroing images\n";
  osl_kernel_DOG(_sigma, _kernel, _k_size, _width);

  osl_canny_base_fill_raw_image(_thin, _xsize, _ysize, 0.0f);
  osl_canny_base_fill_raw_image(_thresh, _xsize, _ysize, _low);
  osl_canny_base_fill_raw_image(_theta, _xsize, _ysize, DUMMYTHETA);

  // A suitably large value (FAR) - perhaps should be more. This represents the
  // distance of a point to the nearest edgel
  osl_canny_base_fill_raw_image(_dist, _xsize, _ysize, FAR);

  // Do the traditional Canny parts, and use non-maximal supression to
  // set the thresholds.
  if (_verbose) vcl_cerr << "smoothing the image\n";
  osl_canny_smooth_rothwell(image, _kernel, _width, _k_size, _smooth);

  if (_verbose) vcl_cerr << "computing x,y derivatives and norm of gradient\n";
  osl_canny_gradient(_xsize, _ysize, _smooth, _dx, _dy, _grad);

  if (_verbose) vcl_cerr << "doing sub-pixel interpolation\n";
  Sub_pixel_interpolation();

  if (_verbose) vcl_cerr << "assigning thresholds\n";
  Set_thresholds(); // _ghist is computed here

  // If we don't want to maintain the strict measurement of the topology
  // (ie. we want to stop the junction regions becoming too extensive), we
  // fill in single pixel holes in the edge description.
  if ( !maintain_topology ) {
    vcl_cerr << "Filling holes\n";
    Fill_holes();
  }

  // Thin the edge image, though keep the original thick one
  if (_verbose) vcl_cerr << "thinning edges\n";
  Thin_edges();

  // Locate junctions in the edge image and joint the clusters together
  // as we have no confidence in the geometry around them.
  if (_verbose) vcl_cerr << "locating junctions in the edge image - ";
  Find_junctions();
  if (_verbose) vcl_cerr << _xjunc->size() << " junctions found\n";

  Find_junction_clusters();
  if (_verbose) vcl_cerr << _vlist->size() << " junction clusters found\n";

  // Finally do edge following to extract the edge data from the _thin image
  if (_verbose) vcl_cerr << "doing final edge following\n";
  Follow_curves(edges);

  if (_verbose) vcl_cerr << "finished osl_edge_detector\n";
}

//-----------------------------------------------------------------------------
//:
// A procedure that performs sub-pixel interpolation for all edges greater
// than the threshold by parabolic fitting. Writes edges into the _thresh image
// if they are maxima and above _low. This gives a good indication of the local
// edge strengths. Stores sub-pixel positions in _dx and _dy, and set the
// orientations in _theta.
//
void osl_edge_detector::Sub_pixel_interpolation() {
  float h1=0.0,h2=0.0; // dummy initialisation values
  float k = 180.0f/float(vnl_math::pi);
  int orient;
  float theta,grad;
  float fraction,dnewx=0.0,dnewy=0.0; // dummy initialisation values

  // Add 1 to get rid of border effects.
  for (int x=_width+1; x<_xsize-_width-1; ++x)  {
    float *g0 = _grad[x-1];
    float *g1 = _grad[x];
    float *g2 = _grad[x+1];
    float *dx = _dx[x];
    float *dy = _dy[x];

    for (int y=_width+1; y<_ysize-_width-1; ++y)  {
      // First check that we have a potential edge
      if ( g1[y] > _low ) {
        theta = k*vcl_atan2(dy[y],dx[y]);

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
        fraction = (h1-h2)/(2.0*(h1-2.0*g1[y]+h2));
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

        // Now store the edge data, re-use _dx[][] and _dy[][]
        // for sub-pixel locations (don't worry about the junk
        // that is already in them). Use any edgels that get
        // non-maximal suppression to bootstrap the image
        // thresholds. The >= is used rather than > for reasons
        // involving non-generic images. Should this be interpolated
        // height  = g1[y] + frac*(h2-h1)/4 ?
        if ( (g1[y]>=h1) && (g1[y]>=h2) && (vcl_fabs(dnewx)<=0.5) && (vcl_fabs(dnewy)<=0.5) ) {

          if ( g1[y]*ALPHA > _low )
            _thresh[x][y] = ALPHA * g1[y]; // Use a ALPHA% bound
          // _thresh image starts off as being equal to _low
          // else
          //   _thresh[x][y] = _low;
          Thicken_threshold(x,y);
        }

        // + 0.5 is to account for targetjr display offset
        if ( (vcl_fabs(dnewx)<=0.5) && (vcl_fabs(dnewy)<=0.5) ) {
          dx[y] = x + dnewx + 0.5;
          dy[y] = y + dnewy + 0.5;
        }
        else {
          dx[y] = x + 0.5;
          dy[y] = y + 0.5;
        }
        _theta[x][y] = theta;
      }
      // For consistency assign these values even though the
      // edge is below strength.
      else {
        dx[y] = x + 0.5;
        dy[y] = y + 0.5;
      }
    }
  }

  // Clean up around the border to ensure consistency in the _dx and _dy values.
  for (int x=0; x<_xsize; ++x) {
    for (int y=0; y<=_width; ++y) {
      _dx[x][y] = x + 0.5;
      _dy[x][y] = y + 0.5;
    }
    for (int y=_ysize-_width-1; y<_ysize; ++y) {
      _dx[x][y] = x + 0.5;
      _dy[x][y] = y + 0.5;
    }
  }

  for (int y=_width+1; y<_ysize-_width-1; ++y) {
    for (int x=0; x<=_width; ++x)  {
      _dx[x][y] = x + 0.5;
      _dy[x][y] = y + 0.5;
    }
    for (int x=_xsize-_width-1; x<_xsize; ++x) {
      _dx[x][y] = x + 0.5;
      _dy[x][y] = y + 0.5;
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
  int width = _width;
  //    int width = 0;

  for (int i=x-width; i<=x+width; ++i)
    for (int j=y-width; j<=y+width; ++j)  {

      _dist[i][j] = 0;
      if ( _thresh[i][j] != _low )
        _thresh[i][j] = vcl_min(_thresh[x][y], _thresh[i][j]);
      else
        _thresh[i][j] = _thresh[x][y];
    }
}


//-----------------------------------------------------------------------------
//:
// Takes the _thresh image that contains threshold values near to where
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
// becomes incredibably long. Therefore putting up with the Chamfer method for
// the moment.
//
// The histogram calculation was added to support
// edgel change detection-JLM May 1995
//
void osl_edge_detector::Set_thresholds()
{
  int **fdist,**bdist,**a1dist,**a2dist;
  fdist = Make_int_image(_xsize,_ysize);
  bdist = Make_int_image(_xsize,_ysize);
  a1dist = Make_int_image(_xsize,_ysize);
  a2dist = Make_int_image(_xsize,_ysize);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(int const*const*, _dist), fdist, _xsize, _ysize);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(int const*const*, _dist), bdist, _xsize, _ysize);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(int const*const*, _dist), a1dist, _xsize, _ysize);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(int const*const*, _dist), a2dist, _xsize, _ysize);

  float **fth,**bth,**a1th,**a2th;
  fth = Make_float_image(_xsize,_ysize);
  bth = Make_float_image(_xsize,_ysize);
  a1th = Make_float_image(_xsize,_ysize);
  a2th = Make_float_image(_xsize,_ysize);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const *const*, _thresh), fth, _xsize, _ysize);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const *const*, _thresh), bth, _xsize, _ysize);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const *const*, _thresh), a1th, _xsize, _ysize);
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const *const*, _thresh), a2th, _xsize, _ysize);

  osl_chamfer_Forward (_xsize, _ysize, fdist, fth);
  osl_chamfer_Backward(_xsize, _ysize, bdist, bth);
  osl_chamfer_Alt1(_xsize, _ysize, a1dist, a1th);
  osl_chamfer_Alt2(_xsize, _ysize, a2dist, a2th);

  // The range of the effect of the smoothing kernel, including the scale factor
  // we have ignored up to now for the chamfer masks
  //    int range = 3*_width;
  //    float max_gradient = _low;  //Commented out May 1997- JLM
  //Replaced by global _max_gradient
  for (int x=0; x<_xsize; ++x) {
    for (int y=0; y<_ysize; ++y) {

      if ( _thresh[x][y] == _low ) {

        // Determine the two closest edge points.
        int option = osl_Minimum4(fdist[x][y],
                                  bdist[x][y],
                                  a1dist[x][y],
                                  a2dist[x][y]);
        float num=1.0,den=1.0; // dummy initialisation values
        switch(option) {
        case 1:
        case 2:
          den = (fdist[x][y]+bdist[x][y]);
          num = (bdist[x][y]*fth[x][y]+fdist[x][y]*bth[x][y]);
          break;

        case 3:
        case 4:
          den = (a1dist[x][y]+a2dist[x][y]);
          num = (a2dist[x][y]*a1th[x][y]+a1dist[x][y]*a2th[x][y]);
          break;

        default:
          vcl_abort();
          //break;
        }
        if ( den != 0.0 )
          _thresh[x][y] = num / den;
        else if ( _thresh[x][y] <= _low )
          _thresh[x][y] = _low;
      }

      if ( _grad[x][y]>_thresh[x][y] ) {
        //changed to _max_gradient, global - May 1997 JLM
        if (_grad[x][y]>_max_gradient)
          _max_gradient=_grad[x][y];
        _thin[x][y] = _grad[x][y];
      }
    }
  }
  // Noticed that all gradient values are used in edgel Strength Histogram - May 1997
  // So defer to actual edgel chain formation.
#if 0 // commented out
  if (_gradient_histogram)
        {
            _ghist =
              new Histogram(_histogram_resolution, _low, max_gradient);
            for (x=0; x<_xsize; ++x)
                for (y=0; y<_ysize; ++y)
                  //_ghist->UpCount(_grad[x][y]); //All Pixels (Used since 1995)
                  _ghist->UpCount(_thin[x][y]);  //Just at edgels (First check
        }                                        //for significant differences)
#endif

  Free_int_image(fdist,_xsize);
  Free_int_image(bdist,_xsize);
  Free_int_image(a1dist,_xsize);
  Free_int_image(a2dist,_xsize);
  Free_float_image(fth,_xsize);
  Free_float_image(bth,_xsize);
  Free_float_image(a1th,_xsize);
  Free_float_image(a2th,_xsize);
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
// ones first. This accounts for non-maximal supression, and does it in a
// topology preserving way. Note that we are creating a vcl_list with a large
// number of elements, and then sorting it - this is likely to be quite slow.
// An alternative implementation would be better.
//
void osl_edge_detector::Thin_edges() {

  // Find all of the edgels with a strength > _low
  int x,y,a,b,c,d,e,f,g,h,genus,count;
  bool do_output = true;

  vcl_cerr << __FILE__ ": Fast Sort" << vcl_endl;
  osl_edge_detector_xyfloat* edgel_array = new osl_edge_detector_xyfloat[_xsize * _ysize];
  int edgel_array_len = 0;
  int pos = 0;
  count = 1;     // count set to dummy value
  while ( count )  { //  Thin until no Pixels are removed

    count = 0;
    edgel_array_len = 0;
    for (x=_width; x<_xsize-_width; ++x)
      for (y=_width; y<_ysize-_width; ++y)
        if ( _thin[x][y] > _thresh[x][y] ) {

          edgel_array[edgel_array_len].x = x;
          edgel_array[edgel_array_len].y = y;
          edgel_array[edgel_array_len].thin = _thin[x][y];
          edgel_array_len++;
        }

    // Now sort the list; this could be slow if we have a lot of potential.
    // edges - surely we have to do number of elements (not -1)?
    //      qsort(edgel_array, edgel_array_len-1, sizeof(osl_edge_detector_xyfloat), &compare);
    qsort(edgel_array,
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
    for (pos=0; pos<edgel_array_len; ++pos)  {

      x = edgel_array[pos].x;
      y = edgel_array[pos].y;

      if ( _thin[x-1][y-1] > _thresh[x-1][y-1] )  a = 1; else a = 0;
      if ( _thin[x  ][y-1] > _thresh[x  ][y-1] )  b = 1; else b = 0;
      if ( _thin[x+1][y-1] > _thresh[x+1][y-1] )  c = 1; else c = 0;
      if ( _thin[x+1][y  ] > _thresh[x+1][y  ] )  d = 1; else d = 0;
      if ( _thin[x+1][y+1] > _thresh[x+1][y+1] )  e = 1; else e = 0;
      if ( _thin[x  ][y+1] > _thresh[x  ][y+1] )  f = 1; else f = 0;
      if ( _thin[x-1][y+1] > _thresh[x-1][y+1] )  g = 1; else g = 0;
      if ( _thin[x-1][y  ] > _thresh[x-1][y  ] )  h = 1; else h = 0;

      genus = a+b+c+d+e+f+g+h;

      // Continue if the pixel is not dangling.
      if ( (genus!=1) && (genus!=8) ) {

        genus += h*a*b+b*c*d+d*e*f+f*g*h-a*b-b*c-c*d-d*e-e*f-f*g
          - g*h-h*a-h*b-b*d-d*f-f*h-1;

        // If the genus is zero delete the edge
        if ( genus == 0 ) {
          count++;
          _thin[x][y] = 0.0;
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
// topological descriptions. To simplify matters, we raise the _thin value
// of the central pixel and so force it to be an edgel.
//
void osl_edge_detector::Fill_holes() {
  // Find all of the edgels with a strength <= _thresh
  float SMALL = 0.0001f;

  for (int x=_width; x<_xsize-_width; ++x)
    for (int y=_width; y<_ysize-_width; ++y)
      if ( _thin[x][y] <= _thresh[x][y] ) {

        int count = 0;
        if ( _thin[x  ][y-1] > _thresh[x  ][y-1] ) count++;
        if ( _thin[x  ][y+1] > _thresh[x  ][y+1] ) count++;
        if ( _thin[x+1][y  ] > _thresh[x+1][y  ] ) count++;
        if ( _thin[x-1][y  ] > _thresh[x-1][y  ] ) count++;

        if ( count == 4 )
          _thin[x][y] = _thresh[x][y] + SMALL;
      }
}


//-----------------------------------------------------------------------------

// see osl_canny_ox.cxx
extern osl_Vertex *osl_find(vcl_list<osl_Vertex*> const *l, osl_Vertex const &v);

//:
// Follow all edgel chains that have pixel values above their corresponding
// threshold values (_thin[x][y] > _thresh[x][y]).
//
void osl_edge_detector::Follow_curves(vcl_list<osl_edge*> *edges)
{
  //  //Added May 1997 to restrict histogram to actual detected edgels -JLM
  //  if (_gradient_histogram)
  //    _ghist = new Histogram(_histogram_resolution, _low, _max_gradient);

  vcl_list<int> xcoords,ycoords;
  vcl_list<float> grad;

  _chain_no = 10;  // Must be set to a number >= 1

  // Find an edgel point and start to follow it.
  edges->clear();
  for (int x=_width; x<_xsize-_width; ++x)  {
    float *thin = _thin[x];
    for (int y=_width; y<_ysize-_width; ++y) {
      if ( (thin[y]<=_thresh[x][y]) || _junction[x][y] )
        continue;

      // Set the following variable [what a pointless comment]
      _chain_no++;

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
        if ( val != _jval ) {
          *(px++) = _dx[tmpx][tmpy] + _xstart;
          *(py++) = _dy[tmpx][tmpy] + _ystart;
          *(pg++) = val;
          //     if (_ghist)
          //       _ghist->UpCount(val); //Added edgel histogram here -May 1997
        }
        else {
          *(px++) = tmpx + _xstart;
          *(py++) = tmpy + _ystart;
          *(pg++) = 0.0;   // Mark the gradient as zero at a junction
        }
        if (_theta[tmpx][tmpy] == DUMMYTHETA) {
          const float k = 180.0f/float(vnl_math::pi);
          _theta[tmpx][tmpy]  = k*vcl_atan2(_dy[tmpx][y],_dx[tmpx][y]);
        }

        *(pt++) = _theta[tmpx][tmpy];
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
        v1->SetId(_vertidcount++);
        osl_Vertex *v2 = new osl_Vertex(dc->GetX(dc->size()-1),dc->GetY(dc->size()-1));
        v2->SetId(_vertidcount++);
        // Check whether each vertex is a junction
        osl_Vertex *V1 = osl_find(_vlist, *v1);
        osl_Vertex *V2 = osl_find(_vlist, *v2);

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
            v1->SetId(_vertidcount++);
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
  if ( (x<=0) || (x>=_xsize-1) || (y<=0) || (y>=_ysize-1) )
    return;

  // Add the current point to the coordinate lists, and delete from
  // the edge image
  if (!reverse) {
    xc->push_front(x);
    yc->push_front(y);
    grad->push_front(_thin[x][y]);
  }
  _thin[x][y] = 0.0;

  // The order of traversal is (x axis to the right and y axis down) :
  //    5 0 4
  //    1 * 3
  //    6 2 7
  // Have to be careful with the ; in the following macros. Don't
  // emacs indent this function!
  if (false) { }

  // Now recursively look for connected eight-neighbours.
#define smoo(a, b) \
  else if ( (_thin[a][b]>_thresh[a][b]) && (_junction[a][b]==0) ) \
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

  // Else see if there is a junction nearby, and record it. The _chain_no
  // variable is used to prevent the same junction being inserted at both
  // ends of the edgel chains when reversal occurs next to the junction
  // (in that case there will only be two stored points: the edge and the junction)
#define smoo(a, b) \
  else if ( _junction[a][b] && ((xc->size()>2) || (_junction[a][b]!=_chain_no)) ) { \
    xc->push_front(_jx[a][b]); \
    yc->push_front(_jy[a][b]); \
    grad->push_front(_jval); \
    _junction[a][b] = _chain_no; \
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
  _xjunc->clear();
  _yjunc->clear();
  osl_canny_base_fill_raw_image(_junction, _xsize, _ysize, 0);

  for (int x=_width; x<_xsize-_width; ++x)
    for (int y=_width; y<_ysize-_width; ++y)  {
      if ( _thin[x  ][y  ] <= _thresh[x  ][y] )
        continue;

      int count = 0;
      if ( _thin[x-1][y-1] > _thresh[x-1][y-1] )  count++;
      if ( _thin[x  ][y-1] > _thresh[x  ][y-1] )  count++;
      if ( _thin[x+1][y-1] > _thresh[x+1][y-1] )  count++;
      if ( _thin[x+1][y  ] > _thresh[x+1][y  ] )  count++;
      if ( _thin[x+1][y+1] > _thresh[x+1][y+1] )  count++;
      if ( _thin[x  ][y+1] > _thresh[x  ][y+1] )  count++;
      if ( _thin[x-1][y+1] > _thresh[x-1][y+1] )  count++;
      if ( _thin[x-1][y  ] > _thresh[x-1][y  ] )  count++;

      if ( count > 2 ) {
        _xjunc->push_front(x);
        _yjunc->push_front(y);
        _junction[x][y] = 1;
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
  for (int x=_width; x<_xsize-_width; ++x) {
    for (int y=_width; y<_ysize-_width; ++y) {
      if ( _junction[x][y] ) {

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
    _junction[xjunc.front()][yjunc.front()] = 1;
    xjunc.pop_front();
    yjunc.pop_front();
  }

  // Construct the list of junction cluster centres
  _vlist->clear();
  for (vcl_list<int>::iterator i=xvertices.begin(), j=yvertices.begin();
       i!=xvertices.end() && j!=yvertices.end();
       ++i, ++j) {
    //for (xvertices.reset(),yvertices.reset(); xvertices.next(),yvertices.next(); )  {

    osl_Vertex *v = new osl_Vertex( (*i)/*xvertices.value()*/+_xstart,
                                    (*j)/*yvertices.value()*/+_ystart);
    _vlist->push_front(v);
    _junction[(*i)/*xvertices.value()*/][(*j)/*yvertices.value()*/] = 2;
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
  _junction[x][y] = 0;

  // Now recursively look for connected eight-neighbours
  //    5 0 4
  //    1 * 3
  //    6 2 7
#define smoo(a, b) \
  if ( _junction[a][b] ) Follow_junctions(a,b, xc,yc);
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
  dist = _xsize*_ysize; // A number larger than the image size
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
    if ( _grad[(*i)/*xc.value()*/][(*j)/*yc.value()*/] > grad ) {
      grad = _grad[(*i)/*xc.value()*/][(*j)/*yc.value()*/];
      x0 = (*i);//xc.value();
      y0 = (*j);//yc.value();
    }

  // Set up the (_jx,_jy) arrays to point to the cluster centre
  for (it i=xc.begin(), j=yc.begin(); i!=xc.end() && j!=yc.end(); ++i, ++j) {
    //xc.reset(),yc.reset(); xc.next(),yc.next(); )  {
    _jx[(*i)/*xc.value()*/][(*j)/*yc.value()*/] = x0;
    _jy[(*i)/*xc.value()*/][(*j)/*yc.value()*/] = y0;
  }
}

//-----------------------------------------------------------------------------

