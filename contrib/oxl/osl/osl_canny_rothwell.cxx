
//:
//  \file

#include "osl_canny_rothwell.h"
#include <osl/osl_canny_rothwell_params.h>
#include <osl/osl_kernel.h>
#include <osl/osl_canny_smooth.h>
#include <osl/osl_canny_gradient.h>
#include <vnl/vnl_math.h>

#include <vcl_list.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

const float DUMMYTHETA = 10000.0;

//-----------------------------------------------------------------------------

osl_canny_rothwell::osl_canny_rothwell(osl_canny_rothwell_params const &params)
  : osl_canny_base(params.sigma, params.low, params.high, params.verbose)
{
  // Determine the size of the largest convolution kernel
  _range = params.range;
  _gauss_tail = 0.01f;   // Canny uses 0.001
  _width = int(_sigma*vcl_sqrt(2*vcl_log(1/_gauss_tail))+1); // round up to int
  _w0 = _width;
  _k_size = 2*_width+ 1;
  _kernel = new float[_k_size];

  _xdang = new vcl_list<int>;
  _ydang = new vcl_list<int>;
  _xjunc = new vcl_list<int>;
  _yjunc = new vcl_list<int>;
  _vlist = new vcl_list<osl_Vertex*>;

  _dummy = 1000.0;
  _jval = 2000.0;
}

//-----------------------------------------------------------------------------

osl_canny_rothwell::~osl_canny_rothwell() {
  osl_canny_base_free_raw_image(_smooth);
  osl_canny_base_free_raw_image(_dx);
  osl_canny_base_free_raw_image(_dy);
  osl_canny_base_free_raw_image(_grad);

  osl_canny_base_free_raw_image(_thick);
  osl_canny_base_free_raw_image(_thin);
  osl_canny_base_free_raw_image(_theta);

  osl_canny_base_free_raw_image(_dangling);
  osl_canny_base_free_raw_image(_junction);
  osl_canny_base_free_raw_image(_jx);
  osl_canny_base_free_raw_image(_jy);

  //no point _vlist->clear();
  delete _vlist;
  delete [] _kernel;
  delete _xdang;
  delete _ydang;
  delete _xjunc;
  delete _yjunc;
}


//-----------------------------------------------------------------------------

void osl_canny_rothwell::detect_edges(vil_image const &image, vcl_list<osl_edge*> *edges, bool adaptive)
{
  assert(edges!=0);

  _xsize  = image.height();
  _ysize  = image.width();
  _xstart = 0;
  _ystart = 0;

  if (verbose)
    vcl_cerr << "Doing Canny on image region "
         << _xsize << " by " << _ysize << vcl_endl
         << "Gaussian tail   = " << _gauss_tail << vcl_endl
         << "Sigma           = " << _sigma << vcl_endl
         << "Kernel size     = " << _k_size << vcl_endl
         << "Upper threshold = " << _high << vcl_endl
         << "Lower threshold = " << _low << vcl_endl
         << "Smoothing range = " << _range << vcl_endl << vcl_endl;

  _smooth   = osl_canny_base_make_raw_image(_xsize, _ysize, (float*)0);
  _dx       = osl_canny_base_make_raw_image(_xsize, _ysize, (float*)0);
  _dy       = osl_canny_base_make_raw_image(_xsize, _ysize, (float*)0);
  _grad     = osl_canny_base_make_raw_image(_xsize, _ysize, (float*)0);
  _thick    = osl_canny_base_make_raw_image(_xsize, _ysize, (float*)0);
  _thin     = osl_canny_base_make_raw_image(_xsize, _ysize, (float*)0);
  _theta    = osl_canny_base_make_raw_image(_xsize, _ysize, (float*)0);
  _dangling = osl_canny_base_make_raw_image(_xsize, _ysize, (int*)0);
  _junction = osl_canny_base_make_raw_image(_xsize, _ysize, (int*)0);
  _jx       = osl_canny_base_make_raw_image(_xsize, _ysize, (int*)0);
  _jy       = osl_canny_base_make_raw_image(_xsize, _ysize, (int*)0);

  osl_canny_base_fill_raw_image(_theta ,_xsize, _ysize, DUMMYTHETA);
  osl_canny_base_fill_raw_image(_smooth,_xsize, _ysize, 0.0f);
  osl_canny_base_fill_raw_image(_dx,    _xsize, _ysize, 0.0f);
  osl_canny_base_fill_raw_image(_dy,    _xsize, _ysize, 0.0f);
  osl_canny_base_fill_raw_image(_grad,  _xsize, _ysize, 0.0f);
  osl_canny_base_fill_raw_image(_thick, _xsize, _ysize, 0.0f);
  osl_canny_base_fill_raw_image(_thin,  _xsize, _ysize, 0.0f);

  // Do the traditional Canny parts
  if (verbose) vcl_cerr << "setting convolution kernel and zeroing images\n";
  osl_kernel_DOG(_sigma, _kernel, _k_size, _width);

  if (verbose) vcl_cerr << "smoothing the image\n";
  osl_canny_smooth_rothwell(image, _kernel, _width, _k_size, _smooth);

  if (verbose) vcl_cerr << "computing derivatives\n";
  osl_canny_gradient_central(_xsize, _ysize, _smooth, _dx, _dy, _grad);

  if (verbose) vcl_cerr << "doing non-maximal supression\n";
  Non_maximal_supression();

  // Thin the edge image, though keep the original thick one
  if (verbose) vcl_cerr << "thinning edges\n";
  osl_canny_base_copy_raw_image(VCL_OVERLOAD_CAST(float const*const*, _thick),
                                VCL_OVERLOAD_CAST(float *const*, _thin), _xsize, _ysize);
  Thin_edges();

  if (verbose) vcl_cerr << "doing hysteresis\n";
  Initial_hysteresis();

  if ( adaptive ) {
    // Do Canny around the remaining ends at smaller scales to improve
    // topology. We wish to do the adaptive Canny until the region of
    // influence is less than `range' pixels
    float min_sigma = _range / vcl_sqrt(-2.0*vcl_log(_gauss_tail));
    if (verbose) vcl_cerr << "\nadaptive Canny with smoothing sigma bound = " << min_sigma << vcl_endl;

    // Try to fix single pixel breaks in the edgel chains
    if (verbose) vcl_cerr << "searching for dangling ends\n";
    Find_dangling_ends();
    if (verbose) vcl_cerr << _xdang->size() << " dangling edges found initially\n"
                      << "looking for single pixel breaks - ";
    Jump_single_breaks();
    Thin_edges();   // Must thin after jumping
    Find_dangling_ends();
    if (verbose) vcl_cerr << _xdang->size() << " dangling edges found after joining\n";

    while ( _sigma > min_sigma ) {
      // Locate junctions in the edge image
      if (verbose) vcl_cerr << "computing current junction set";
      Find_junctions();

      if (verbose) vcl_cerr << "\nrunning adaptive Canny\n";
      Adaptive_Canny(image);

      // Repeat the thinning and pixel-jumping process
      if (verbose) vcl_cerr << "thinning edges - reprise\n";
      Thin_edges();

      Find_dangling_ends();
      if (verbose) vcl_cerr << _xdang->size() << " dangling edges found after scale reduction\n"
                        << "looking for single pixel breaks - ";
      Jump_single_breaks();
      Thin_edges();
      Find_dangling_ends();
      if (verbose) vcl_cerr << _xdang->size() << " dangling edges found after re-joining\n";
    }
  }

  // Locate junctions in the edge image
  if (verbose) vcl_cerr << "locating junctions in the edge image - ";
  Find_junctions();
  if (verbose) vcl_cerr << _xjunc->size() << " junctions found\n";
  Find_junction_clusters();
  if (verbose)  vcl_cerr << _vlist->size() << " junction clusters found\n";

  // Finally do edge following to extract the edge data from the _thin image
  if (verbose) vcl_cerr << "doing final edge following\n";
  Final_hysteresis(edges);
  if (verbose) vcl_cerr << "finished Canny\n";
}


//-----------------------------------------------------------------------------
//
//:
// Non-maximally supresses the output image by searching along the edge
// normal and checking that the test edge has a greater that the interpolated
// neighbours in the direction. We have also included sub-pixel interpolation
// of the peak position by parabolic fitting.  Writes edges into the _thick
// image.
//
void osl_canny_rothwell::Non_maximal_supression() {
  float h1,h2;
  float k = 180.0f/float(vnl_math::pi);
  int orient;
  float theta,grad;
  float fraction,newx,newy;

  // Add 1 to get rid of border effects
  for (int x=_w0+1; x<_xsize-_w0-1; ++x)  {
    float *g0 = _grad[x-1];
    float *g1 = _grad[x];
    float *g2 = _grad[x+1];
    float *dx = _dx[x];
    float *dy = _dy[x];

    for (int y=_w0+1; y<_ysize-_w0-1; ++y)  {
      // First check that we have an edge
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
          //h1 = h2 = 0.0;  // Dummy values
          //vcl_cerr << "*** ERROR ON SWITCH IN NMS ***\n";
        }

        // If the edge is greater than h1 and h2 we are at a peak,
        // therefore do subpixel interpolation by fitting a parabola
        // along the NMS line and finding its peak
        if ( (g1[y]>h1) && (g1[y]>h2) ) {
          fraction = (h1-h2)/(2.0*(h1-2.0*g1[y]+h2));
          switch( orient ) {
          case 0:
            newx = x + fraction;
            newy = y + dy[y]/dx[y]*fraction;
            break;

          case 1:
            newx = x + dx[y]/dy[y]*fraction;
            newy = y + fraction;
            break;

          case 2:
            newx = x + dx[y]/dy[y]*fraction;
            newy = y + fraction;
            break;

          case 3:
            newx = x - fraction;
            newy = y - dy[y]/dx[y]*fraction;
            break;

          default:
            vcl_abort();
            //newx = newy = 0.0; // Dummy values
            //vcl_cerr << "*** ERROR ON SWITCH IN NMS ***\n";
          }

          // Now store the edge data, re-use _dx[][] and _dy[][]
          // for sub-pixel locations (don't worry about the junk
          // that is already in them).
          // + 0.5 is to account for targetjr display offset
          _thick[x][y] = g1[y]; // Should this be interpolated height --
          dx[y] = newx + 0.5;   // = g1[y] + frac*(h2-h1)/4 ?
          dy[y] = newy + 0.5;
          _theta[x][y] = theta;
        }
      }
    }
  }
}


//-----------------------------------------------------------------------------
//
//:
// Hysteresis follows edgel chains that lie above the _low threshold and
// have at least one edgel above the _high threshold. Once we have followed,
// the good edgelchains are re-writted to the _thin image for further
// processing.
//
void osl_canny_rothwell::Initial_hysteresis() {

  vcl_list<int> xcoords,ycoords;
  vcl_list<float> grad;
  vcl_list<osl_edgel_chain*> edges;
  float *thin,*px,*py,*pg;
  osl_edgel_chain *edgels;

  // Find a point above _high and start to follow it.
  // First time round we are just trying to get rid of the weak dangling chains
  // and so we will record the good edges and then re-insert them in the _thin
  // image and follow a second time.
  edges.clear();
  for (int x=_w0; x<_xsize-_w0; ++x)  {
    thin = _thin[x];
    for (int y=_w0; y<_ysize-_w0; ++y)
      if ( thin[y]>_high ) {
        Initial_follow(_thin, _xsize, _ysize, _low,
                       x,y,&xcoords,&ycoords,&grad);

        // Create an edge chain and add to the list
        edgels = new osl_edgel_chain(xcoords.size());
        px = edgels->GetX();
        py = edgels->GetY();
        pg = edgels->GetGrad();
        while ( xcoords.size() ) {
          *(px++) = xcoords.front(); xcoords.pop_front();
          *(py++) = ycoords.front(); ycoords.pop_front();
          *(pg++) = grad.front(); grad.pop_front();
        }
        edges.push_front(edgels);
      }
  }

  // Now re-create the _thin image
  osl_canny_base_fill_raw_image(_thin, _xsize, _ysize, 0.0f);
  while (edges.size()) {

    edgels = edges.front(); edges.pop_front();
    px = edgels->GetX();
    py = edgels->GetY();
    pg = edgels->GetGrad();
    for (int i=0; i<edgels->size(); ++i)
      _thin[int(px[i])][int(py[i])] = pg[i];
    delete edgels;
  }
}


//-----------------------------------------------------------------------------

// see osl_canny_ox.cxx
extern osl_Vertex *osl_find(vcl_list<osl_Vertex*> const *l, osl_Vertex const &v);
extern osl_Vertex *osl_find(vcl_list<osl_Vertex*> const *l, float x, float y);

//:
// Hysteresis follows edgel chains that lie above the _low threshold and
// have at least one edgel above the _high threshold. Due to the Initial_hysteresis
// phase, all edges greater than _low will be by default good and so have a member
// greater than _high.
//
void osl_canny_rothwell::Final_hysteresis(vcl_list<osl_edge*> *edges) {
  vcl_list<int> xcoords,ycoords;
  vcl_list<float> grad;
  float *thin,*px,*py,*pg,*pt,val;

  _chain_no = 10;  // Must be set to a number >= 1

  // Find a point above _high and start to follow it (but not a dummy point).
  edges->clear();
  for (int x=_w0; x<_xsize-_w0; ++x)  {
    thin = _thin[x];
    for (int y=_w0; y<_ysize-_w0; ++y) {
      // Due to Initial_hysteresis we can follow everything > _low
      if ( thin[y]<=_low || _junction[x][y] )
        continue;

      // Set up the following variable [what a pointless comment]
      _chain_no++;

      // clear the lists
      xcoords.clear();
      ycoords.clear();
      grad.clear();

      // follow in one direction [? fsm]
      Final_follow(x,y, &xcoords,&ycoords,&grad, 0);

      // We may have picked up the edgel chain somewhere
      // away from its ends. Therefore, reverse the list
      // and try to follow again.
      xcoords.reverse();
      ycoords.reverse();
      grad.reverse();
      Final_follow(x,y,&xcoords,&ycoords,&grad,1);

      // Check that we have at least two endpoints to
      // the list, otherwise go to next loop
      if ( xcoords.size() < 2 )
        // vcl_cerr << "short list found in Final_follow\n";
        continue;

      // count the number of non-dummy edgels
      int count=0;
      for (vcl_list<float>::iterator i=grad.begin(); i!=grad.end(); ++i)
        if ( (*i) != _dummy )
          count++;

      // If the count is less than two we cannot accept
      // the edgelchain. Smallest chain must either be `ee',
      // `je', `ej' or `jj' (for `e'dge and `j'unction)
      if ( count < 2 )
        continue;

      // Create a osl_edgel_chain
      osl_edgel_chain *dc = new osl_edgel_chain(count);
      px = dc->GetX();     py = dc->GetY();
      pg = dc->GetGrad();  pt = dc->GetTheta();

      // Write the edgels and end points to the osl_edgel_chain
      //dc->SetStart(xcoords.front()+_xstart, ycoords.front()+_ystart);
      int tmpx=0, tmpy=0;// dummy initialization, as count is always > 0.
      while (count) {
        tmpx = xcoords.front(); xcoords.pop_front();
        tmpy = ycoords.front(); ycoords.pop_front();
        val = grad.front(); grad.pop_front();
        if ( val != _dummy ) {
          --count;
          // If we are not at a junction use sub-pixel value
          if ( val != _jval ) {
            *(px++) = _dx[tmpx][tmpy] + _xstart;
            *(py++) = _dy[tmpx][tmpy] + _ystart;
            *(pg++) = val;
          }
          else {
            *(px++) = tmpx + _xstart;
            *(py++) = tmpy + _ystart;
            *(pg++) = 0.0;   // Mark the gradient as zero at a junction
          }
          if (_theta[tmpx][tmpy] == DUMMYTHETA) {
            const float k = 180.0f/float(vnl_math::pi);
            float *dx = _dx[tmpx];
            float *dy = _dy[tmpx];

            // *** Bug fix, Samer Abdallah 5/10/95:  next line was
            // _theta[tmpx][tmpy]  = k*vcl_atan2(dy[y],dx[y]);
            _theta[tmpx][tmpy]  = k*vcl_atan2(dy[tmpy],dx[tmpy]);
          }

          *(pt++) = _theta[tmpx][tmpy];
        }
      }
      //dc->SetEnd(tmpx+_xstart, tmpy+_ystart);

      // Just check whether we have created a trivial edgechain
      // (can happen due to the presence of dummy points)
      if ( (dc->size()==2) &&
           (dc->GetX(0)==dc->GetX(1)) &&
           (dc->GetY(0)==dc->GetY(1)) ) {
        delete dc; // osl_IUDelete(dc);
        continue;
      }

      else if ( dc->size() > 1 ) {
        // Create an edge for the image topology

        osl_Vertex *v1 = new osl_Vertex(dc->GetX(0), dc->GetY(0));
        osl_Vertex *v2 = new osl_Vertex(dc->GetX(dc->size()-1), dc->GetY(dc->size()-1));

        // Check whether each vertex is a junction
        osl_Vertex *V1=osl_find(_vlist, *v1);
        osl_Vertex *V2=osl_find(_vlist, *v2);

        // If neither are junctions we may have formed a single isolated
        // chain that should have common vertex endpoints.
        bool single_chain = false;
        if ( !V1 && !V2 ) {
          // compute difference (dx, dy) between endpoints.
          float dx = dc->GetX(0) - dc->GetX(dc->size()-1);
          float dy = dc->GetY(0) - dc->GetY(dc->size()-1);
          if ( dx*dx+dy*dy < 4 ) { //  if dist < 2 pixels, it is closed
            V1 = v1;
            V2 = v1;
            osl_IUDelete(v2);
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

        // Note that the edge can start and end in the same place.
        // However, if this is so the DigitalCurve has positive length
        //dc->SetStart(dc->GetX(0), dc->GetY(0));
        //dc->SetEnd(dc->GetX(dc->size()-1), dc->GetY(dc->size()-1));

        edges->push_front(new osl_edge(*dc, V1, V2));
        delete dc;
      }

    } // end of y-loop
  } // end of x-loop
}


//-----------------------------------------------------------------------------
//
//:
// Method to thin the image using the variation of Tsai-Fu thinning used
// by Van-Duc Nguyen in Geo-Calc. This relies on computing the genus of
// an edge location, and removing it if it is not a dangling chain as has
// genus zero.
//
void osl_canny_rothwell::Thin_edges() {
  int i,count;
  float threshold;

  // Now do the thinning. Do it twice: the first time to try to remove
  // _dummy edges, and then other edges -- 0.001 turns <= to <

  for (threshold=_dummy-0.001,i=0; i<2; threshold=_low,i++)  {

    count = 1;     // count set to dummy value
    while ( count )  { //  Thin until no Pixels are removed

      count = 0;
      for (int x=_w0; x<_xsize-_w0; ++x)
        for (int y=_w0; y<_ysize-_w0; ++y)  {

          if ( _thin[x][y] <= threshold )
            continue;

          int a,b,c,d,e,f,g,h;
          if ( _thin[x-1][y-1] > _low )  a = 1; else a = 0;
          if ( _thin[x  ][y-1] > _low )  b = 1; else b = 0;
          if ( _thin[x+1][y-1] > _low )  c = 1; else c = 0;
          if ( _thin[x+1][y  ] > _low )  d = 1; else d = 0;
          if ( _thin[x+1][y+1] > _low )  e = 1; else e = 0;
          if ( _thin[x  ][y+1] > _low )  f = 1; else f = 0;
          if ( _thin[x-1][y+1] > _low )  g = 1; else g = 0;
          if ( _thin[x-1][y  ] > _low )  h = 1; else h = 0;

          int genus = a+b+c+d+e+f+g+h;

          // If the pixel is dangling, record, and go to next loop
          if ( genus == 1 )
            continue;

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
}


//-----------------------------------------------------------------------------
//
//: Searches for single pixel breaks near dangling ends and patches them up.
// This is destructive on the dangling lists.
//
void osl_canny_rothwell::Jump_single_breaks() {

  // Take each dangling end in turn and determine whether there is
  // another edgel within the sixteen one-pixel-distant-neighbours.

  int x,y,i,j;
  float **t = _thin;
  // _xdang->reset();  _ydang->reset();

  while (_xdang->size()) {

    x = _xdang->front(); _xdang->pop_front();
    y = _ydang->front(); _ydang->pop_front();

    // This is messy for efficiency
    if      (_thin[x-1][y-1]) {i=-1; j=-1;}
    else if (_thin[x  ][y-1]) {i= 0; j=-1;}
    else if (_thin[x+1][y-1]) {i= 1; j=-1;}
    else if (_thin[x+1][y  ]) {i= 1; j= 0;}
    else if (_thin[x+1][y+1]) {i= 1; j= 1;}
    else if (_thin[x  ][y+1]) {i= 0; j= 1;}
    else if (_thin[x-1][y+1]) {i=-1; j= 1;}
    else if (_thin[x-1][y  ]) {i=-1; j= 0;}
    else { i = j = 0; } // Dummy declaration

    // If the entrant edgel is diagonal
    if ( i && j ) {

      if      (t[x-2*i][y-2*j]>_low) {
        t[x-i][y-j]=_dummy;_thick[x-i][y-j]=_dummy;}
      else if ((t[x+i][y-2*j]>_low)||(t[x  ][y-2*j]>_low)||(t[x-i][y-2*j]>_low)) {
        t[x  ][y-j]=_dummy;_thick[x  ][y-j]=_dummy;}
      else if ((t[x-2*i][y+j]>_low)||(t[x-2*i][y  ]>_low)||(t[x-2*i][y-j]>_low)) {
        t[x-i][y]=_dummy;_thick[x-i][y  ]=_dummy;}
      else if ((!(t[x+2*i][y  ]>_low))&&((t[x+2*i][y-j]>_low)||(t[x+2*i][y-2*j]>_low))) {
        t[x+i][y-j]=_dummy;_thick[x+i][y-j]=_dummy;}
      else if ((!(t[x  ][y+2*j]>_low))&&((t[x-i][y+2*j]>_low)||(t[x-2*i][y+2*j]>_low))) {
        t[x-i][y+j]=_dummy;_thick[x-i][y+j]=_dummy;}
    }

    // For entrant with i=0
    else if ( i == 0 ) {

      if ((t[x-1][y-2*j]>_low)||(t[x  ][y-2*j]>_low)||(t[x+1][y-2*j]>_low)) {
        t[x  ][y-j]=_dummy;_thick[x  ][y-j]=_dummy;}
      else if ((t[x+2][y-j]>_low)||(t[x+2][y-2*j]>_low)) {
        t[x+1][y-j]=_dummy;_thick[x+1][y-j]=_dummy;}
      else if ((t[x-2][y-j]>_low)||(t[x-2][y-2*j]>_low)) {
        t[x-1][y-j]=_dummy;_thick[x-1][y-j]=_dummy;}
      else if ((!(t[x-2][y+j]>_low))&&(t[x-2][y]>_low)) {
        t[x-1][y  ]=_dummy;_thick[x-1][y  ]=_dummy;}
      else if ((!(t[x+2][y+j]>_low))&&(t[x+2][y]>_low)) {
        t[x+1][y  ]=_dummy;_thick[x+1][y  ]=_dummy;}
    }
    // or finally for j=0

    // *** Bug fix, Samer Abdallah 5/10/95:  next line was
    // else if ( i == 0 )
    else if ( j == 0 ) {

      if      ((t[x-2*i][y-1]>_low)||(t[x-2*i][y]>_low)||(t[x-2*i][y]>_low)) {
        t[x-i][y]=_dummy;_thick[x-i][y]=_dummy;}
      else if ((t[x-i][y+2]>_low)||(t[x-2*i][y+2]>_low)) {
        t[x-1][y+j]=_dummy;_thick[x-1][y+j]=_dummy;}
      else if ((t[x-i][y-2]>_low)||(t[x-2*i][y-2]>_low)) {
        t[x-1][y-j]=_dummy;_thick[x-1][y-j]=_dummy;}
      else if ((!(t[x+i][y-2]>_low))&&(t[x][y-2]>_low)) {
        t[x][y-1]=_dummy;_thick[x][y-1]=_dummy;}
      else if ((!(t[x+i][y+2]>_low))&&(t[x][y+2]>_low)) {
        t[x][y+1]=_dummy;_thick[x][y+1]=_dummy;}
    }
  }
}


//-----------------------------------------------------------------------------
//
//: Searches for high contrast changes in the vicinity of dangling ends.
// This is done by halving the previously used Canny sigma to reduce the effects of
// smoothing near corners. Ultimately the kernel size will be scaled down until
// its radius of influence is only two pixels; at that stage pixel-jumping should
// fix any problems.
//
void osl_canny_rothwell::Adaptive_Canny(vil_image const &image) {
  // Reset the smoothing kernel parameters by
  // halfing the size of the smoothing sigma
  _old_sigma = _sigma;  _sigma /= 2.0;
  _old_width = _width;
  _width = int(_sigma*vcl_sqrt(2*vcl_log(1.0/_gauss_tail))+1);
  _old_k_size = _k_size;  _k_size = 2*_width+ 1;
  delete _kernel; _kernel = new float [_k_size];
  osl_kernel_DOG(_sigma, _kernel, _k_size, _width);

  // Define the new ROI to account for the domain of the old smoothing
  // kernel and for that of the new one
  int image_size = _old_k_size + _k_size - 1;
  int half_size = (image_size - 1)/2;

  if (verbose) vcl_cerr << "new image region "
                    << image_size << " by " << image_size << vcl_endl
                    << "Sigma           = " << _sigma << vcl_endl
                    << "Kernel size     = " << _k_size << vcl_endl;

  // Set up the new images
  float **dx   = osl_canny_base_make_raw_image(image_size,image_size, (float*)0);
  float **dy   = osl_canny_base_make_raw_image(image_size,image_size, (float*)0);
  float **grad = osl_canny_base_make_raw_image(image_size,image_size, (float*)0);

  // For each dangling-end (X,Y), search for more edges at the reduced scale
  int count=0;
  if (verbose) vcl_cerr << "percentage of endings examined =   0";
  typedef vcl_list<int>::iterator it;
  for (it i=_xdang->begin(), j=_ydang->begin(); i!=_xdang->end() && j!=_ydang->end(); ++i, ++j) {
    //_xdang->reset(),_ydang->reset(); _xdang->next(),_ydang->next(); )  {

    int X = (*i)/*_xdang->value()*/, Y = (*j)/*_ydang->value()*/;
    int x0 = X-half_size,   y0 = Y - half_size;  // Region origin in image coords

    // Make sure that the region around the end is within the
    // bounds of the original image, and that the dangling end
    // is not a neighbour to a junction (if it is we recover the
    // topology more robustly by growing something else towards
    // it, rather than the other way round)
    if ( !Junction_neighbour(_junction, X, Y) && (x0>=0) && (x0+image_size<=_xsize) &&
         (y0>=0) && (y0+image_size<=_ysize) )
      {
        // Compute the new image intensity gradients
        osl_canny_smooth_rothwell_adaptive(image, x0,y0,image_size, _kernel, _width, _k_size, dx,dy,grad);

        // Delete the effects of the _thick image - we don't want to
        // locate edges in the same places as before
        Subtract_thick(x0,y0,image_size,grad);

        // Now, if possible, grow the edgelchain out from (X,Y) in a
        // one dimensional direction up to the boundary of the old kernel
        int newx,newy;
        X -= x0;  Y -= y0;   // Do a coordinate shift; centre in local coords
        for (int ii=1; (ii<=_old_k_size) && Dangling_end(X+x0,Y+y0); ++ii)  {

          // Find the eight-way neighbour with the strongest edge
          newx = X;  newy = Y;
          Best_eight_way(X,Y,grad,&newx,&newy);
          // If no new edge has been found we should break
          if ( (newx==X) && (newy==Y) )
            break;
          // Else record a dummy edgel
          else {
            _thin[newx+x0][newy+y0] = _dummy;  _thick[newx+x0][newy+y0] = _dummy;
            X = newx;  Y = newy;
          }
        }
      }
    if (verbose) vcl_fprintf(stderr,"\b\b\b%3ld", 10*((++count)*10/_xdang->size()));
  }
  if (verbose)   vcl_cerr << vcl_endl;

  // Remove the image arrays
  osl_canny_base_free_raw_image(dx);
  osl_canny_base_free_raw_image(dy);
  osl_canny_base_free_raw_image(grad);
}


//-----------------------------------------------------------------------------
//
//: Wherever the _thick image has an edge marked set the new gradient value to zero.
void osl_canny_rothwell::Subtract_thick(int x0, int y0, int image_size, float **grad)
{
  for (int x=_width; x<image_size-_width; ++x)
    for (int y=_width; y<image_size-_width; ++y)
      if ( _thick[x+x0][y+y0] > _low )
        grad[x][y] = 0.0f;
}


//-----------------------------------------------------------------------------
//
//: Returns the eight-way neighbour with the strongest contrast change.
//
//fsm: No, this routine returns the last of the eight neighbours with contrast
//change greater than '_low'. Probably a bug. FIXME
void osl_canny_rothwell::Best_eight_way(int x, int y, float **grad, int *xnew, int *ynew)
{
  float max = _low;
  if ( grad[x-1][y-1] > max ) { *xnew = x-1;  *ynew = y-1; }
  if ( grad[x  ][y-1] > max ) { *xnew = x;    *ynew = y-1; }
  if ( grad[x+1][y-1] > max ) { *xnew = x+1;  *ynew = y-1; }
  if ( grad[x+1][y  ] > max ) { *xnew = x+1;  *ynew = y;   }
  if ( grad[x+1][y+1] > max ) { *xnew = x+1;  *ynew = y+1; }
  if ( grad[x  ][y+1] > max ) { *xnew = x;    *ynew = y+1; }
  if ( grad[x-1][y+1] > max ) { *xnew = x-1;  *ynew = y+1; }
  if ( grad[x-1][y  ] > max ) { *xnew = x-1;  *ynew = y;   }

  // Zero all of those places tested
  for (int j=y-1; j<=y+1; ++j)
    for (int i=x-1; i<=x+1; ++i)
      grad[i][j] = 0.0;
}


//-----------------------------------------------------------------------------
//
//: Searches for the junctions in the image.
//
void osl_canny_rothwell::Find_dangling_ends() {
  // Reset the dangling ends
  _xdang->clear();
  _ydang->clear();
  osl_canny_base_fill_raw_image(_dangling, _xsize, _ysize, 0);

  for (int x=_w0; x<_xsize-_w0; ++x) {
    for (int y=_w0; y<_ysize-_w0; ++y) {

      if ( _thin[x][y] <= _low )
        continue;

      int a,b,c,d,e,f,g,h;
      if ( _thin[x-1][y-1] > _low )  a = 1; else a = 0;
      if ( _thin[x  ][y-1] > _low )  b = 1; else b = 0;
      if ( _thin[x+1][y-1] > _low )  c = 1; else c = 0;
      if ( _thin[x+1][y  ] > _low )  d = 1; else d = 0;
      if ( _thin[x+1][y+1] > _low )  e = 1; else e = 0;
      if ( _thin[x  ][y+1] > _low )  f = 1; else f = 0;
      if ( _thin[x-1][y+1] > _low )  g = 1; else g = 0;
      if ( _thin[x-1][y  ] > _low )  h = 1; else h = 0;

      if ( (a+b+c+d+e+f+g+h) == 1 ) {
        _xdang->push_front(x);
        _ydang->push_front(y);
        _dangling[x][y] = 1;
      }
    }
  }
}


//-----------------------------------------------------------------------------
//
//: Tests whether a points is a dangling end.
//
int osl_canny_rothwell::Dangling_end(int x, int y)
{
  if ( _thin[x][y] <= _low )
    return 0;

  int a,b,c,d,e,f,g,h;
  if ( _thin[x-1][y-1] > _low )  a = 1; else a = 0;
  if ( _thin[x  ][y-1] > _low )  b = 1; else b = 0;
  if ( _thin[x+1][y-1] > _low )  c = 1; else c = 0;
  if ( _thin[x+1][y  ] > _low )  d = 1; else d = 0;
  if ( _thin[x+1][y+1] > _low )  e = 1; else e = 0;
  if ( _thin[x  ][y+1] > _low )  f = 1; else f = 0;
  if ( _thin[x-1][y+1] > _low )  g = 1; else g = 0;
  if ( _thin[x-1][y  ] > _low )  h = 1; else h = 0;

  if ( (a+b+c+d+e+f+g+h) == 1 )
    return 1;
  else
    return 0;
}


//-----------------------------------------------------------------------------
//
//: Searches for the junctions in the image.
//
void osl_canny_rothwell::Find_junctions() {

  int a,b,c,d,e,f,g,h;

  // Reset the junction variables
  _xjunc->clear();
  _yjunc->clear();
  osl_canny_base_fill_raw_image(_junction, _xsize, _ysize, 0);

  for (int x=_w0; x<_xsize-_w0; ++x)
    for (int y=_w0; y<_ysize-_w0; ++y)  {

      if ( _thin[x][y] <= _low )
        continue;

      if ( _thin[x-1][y-1] > _low )  a = 1; else a = 0;
      if ( _thin[x  ][y-1] > _low )  b = 1; else b = 0;
      if ( _thin[x+1][y-1] > _low )  c = 1; else c = 0;
      if ( _thin[x+1][y  ] > _low )  d = 1; else d = 0;
      if ( _thin[x+1][y+1] > _low )  e = 1; else e = 0;
      if ( _thin[x  ][y+1] > _low )  f = 1; else f = 0;
      if ( _thin[x-1][y+1] > _low )  g = 1; else g = 0;
      if ( _thin[x-1][y  ] > _low )  h = 1; else h = 0;

      if ( (a+b+c+d+e+f+g+h) > 2 ) {
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
void osl_canny_rothwell::Find_junction_clusters() {

  vcl_list<int> xcoords,ycoords,xvertices,yvertices,xjunc,yjunc;
  int x,y,x0,y0;

  // Find a junction and follow
  xvertices.clear();  yvertices.clear();
  xjunc.clear();      yjunc.clear();
  for (x=_w0; x<_xsize-_w0; ++x)
    for (y=_w0; y<_ysize-_w0; ++y)
      if ( _junction[x][y] ) {

        // Each cluster is written to (xcoords,ycooords)
        xcoords.clear();  ycoords.clear();
        Follow_junctions(_junction, x,y,&xcoords,&ycoords);

        // Find the `centre' of the cluster. This is defined as the
        // junction closest to the centre of gravity of the cluser
        Cluster_centre_of_gravity(_jx, _jy, xcoords,ycoords,x0,y0);

        // Add both the junctions and the new cluster centre to
        // the main lists
        xvertices.push_front(x0);
        yvertices.push_front(y0);
        xjunc.insert(xjunc.begin(), xcoords.begin(), xcoords.end()); //xjunc.prepend(xcoords);
        yjunc.insert(yjunc.begin(), ycoords.begin(), ycoords.end()); //yjunc.prepend(ycoords);
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
