// This is oxl/osl/osl_canny_rothwell.cxx
#include <list>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include "osl_canny_rothwell.h"
//:
// \file

#include <osl/osl_canny_rothwell_params.h>
#include <osl/osl_kernel.h>
#include <osl/osl_canny_smooth.h>
#include <osl/osl_canny_gradient.h>
#include <vnl/vnl_math.h>

#include <vcl_compiler.h>
#include <cassert>

constexpr float DUMMYTHETA = 10000.0;

//-----------------------------------------------------------------------------

osl_canny_rothwell::osl_canny_rothwell(osl_canny_rothwell_params const &params)
  : osl_canny_base(params.sigma, params.low, params.high, params.verbose)
{
  // Determine the size of the largest convolution kernel
  range_ = params.range;
  gauss_tail_ = 0.01f;   // Canny uses 0.001
  width_ = int(sigma_*std::sqrt(2*std::log(1/gauss_tail_))+1); // round up to int
  w0_ = width_;
  k_size_ = 2*width_+ 1;
  kernel_ = new float[k_size_];

  xdang_ = new std::list<int>;
  ydang_ = new std::list<int>;
  xjunc_ = new std::list<int>;
  yjunc_ = new std::list<int>;
  vlist_ = new std::list<osl_Vertex*>;

  dummy_ = 1000.0;
  jval_ = 2000.0;
}

//-----------------------------------------------------------------------------

osl_canny_rothwell::~osl_canny_rothwell()
{
  osl_canny_base_free_raw_image(smooth_);
  osl_canny_base_free_raw_image(dx_);
  osl_canny_base_free_raw_image(dy_);
  osl_canny_base_free_raw_image(grad_);

  osl_canny_base_free_raw_image(thick_);
  osl_canny_base_free_raw_image(thin_);
  osl_canny_base_free_raw_image(theta_);

  osl_canny_base_free_raw_image(dangling_);
  osl_canny_base_free_raw_image(junction_);
  osl_canny_base_free_raw_image(jx_);
  osl_canny_base_free_raw_image(jy_);

  //no point vlist_->clear();
  delete vlist_;
  delete [] kernel_;
  delete xdang_;
  delete ydang_;
  delete xjunc_;
  delete yjunc_;
}


//-----------------------------------------------------------------------------

void osl_canny_rothwell::detect_edges(vil1_image const &image, std::list<osl_edge*> *edges, bool adaptive)
{
  assert(edges!=nullptr);

  xsize_  = image.height();
  ysize_  = image.width();
  xstart_ = 0;
  ystart_ = 0;

  if (verbose)
    std::cerr << "Doing Canny on image region "
             << xsize_ << " by " << ysize_ << std::endl
             << "Gaussian tail   = " << gauss_tail_ << std::endl
             << "Sigma           = " << sigma_ << std::endl
             << "Kernel size     = " << k_size_ << std::endl
             << "Upper threshold = " << high_ << std::endl
             << "Lower threshold = " << low_ << std::endl
             << "Smoothing range = " << range_ << std::endl << std::endl;

  smooth_   = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)nullptr);
  dx_       = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)nullptr);
  dy_       = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)nullptr);
  grad_     = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)nullptr);
  thick_    = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)nullptr);
  thin_     = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)nullptr);
  theta_    = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)nullptr);
  dangling_ = osl_canny_base_make_raw_image(xsize_, ysize_, (int*)nullptr);
  junction_ = osl_canny_base_make_raw_image(xsize_, ysize_, (int*)nullptr);
  jx_       = osl_canny_base_make_raw_image(xsize_, ysize_, (int*)nullptr);
  jy_       = osl_canny_base_make_raw_image(xsize_, ysize_, (int*)nullptr);

  osl_canny_base_fill_raw_image(theta_ ,xsize_, ysize_, DUMMYTHETA);
  osl_canny_base_fill_raw_image(smooth_,xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(dx_,    xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(dy_,    xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(grad_,  xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(thick_, xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(thin_,  xsize_, ysize_, 0.0f);

  // Do the traditional Canny parts
  if (verbose) std::cerr << "setting convolution kernel and zeroing images\n";
  osl_kernel_DOG(sigma_, kernel_, k_size_, width_);

  if (verbose) std::cerr << "smoothing the image\n";
  osl_canny_smooth_rothwell(image, kernel_, width_, k_size_, smooth_);

  if (verbose) std::cerr << "computing derivatives\n";
  osl_canny_gradient_central(xsize_, ysize_, smooth_, dx_, dy_, grad_);

  if (verbose) std::cerr << "doing non-maximal suppression\n";
  Non_maximal_suppression();

  // Thin the edge image, though keep the original thick one
  if (verbose) std::cerr << "thinning edges\n";
  osl_canny_base_copy_raw_image( thick_, thin_, xsize_, ysize_);
  Thin_edges();

  if (verbose) std::cerr << "doing hysteresis\n";
  Initial_hysteresis();

  if ( adaptive ) {
    // Do Canny around the remaining ends at smaller scales to improve
    // topology. We wish to do the adaptive Canny until the region of
    // influence is less than `range' pixels
    double min_sigma = range_ / std::sqrt(-2.0*std::log(gauss_tail_));
    if (verbose) std::cerr << "\nadaptive Canny with smoothing sigma bound = " << min_sigma << std::endl;

    // Try to fix single pixel breaks in the edgel chains
    if (verbose) std::cerr << "searching for dangling ends\n";
    Find_dangling_ends();
    if (verbose) std::cerr << xdang_->size() << " dangling edges found initially\n"
                          << "looking for single pixel breaks - ";
    Jump_single_breaks();
    Thin_edges();   // Must thin after jumping
    Find_dangling_ends();
    if (verbose) std::cerr << xdang_->size() << " dangling edges found after joining\n";

    while ( sigma_ > min_sigma ) {
      // Locate junctions in the edge image
      if (verbose) std::cerr << "computing current junction set";
      Find_junctions();

      if (verbose) std::cerr << "\nrunning adaptive Canny\n";
      Adaptive_Canny(image);

      // Repeat the thinning and pixel-jumping process
      if (verbose) std::cerr << "thinning edges - reprise\n";
      Thin_edges();

      Find_dangling_ends();
      if (verbose) std::cerr << xdang_->size() << " dangling edges found after scale reduction\n"
                            << "looking for single pixel breaks - ";
      Jump_single_breaks();
      Thin_edges();
      Find_dangling_ends();
      if (verbose) std::cerr << xdang_->size() << " dangling edges found after re-joining\n";
    }
  }

  // Locate junctions in the edge image
  if (verbose) std::cerr << "locating junctions in the edge image - ";
  Find_junctions();
  if (verbose) std::cerr << xjunc_->size() << " junctions found\n";
  Find_junction_clusters();
  if (verbose)  std::cerr << vlist_->size() << " junction clusters found\n";

  // Finally do edge following to extract the edge data from the thin_ image
  if (verbose) std::cerr << "doing final edge following\n";
  Final_hysteresis(edges);
  if (verbose) std::cerr << "finished Canny\n";
}


//-----------------------------------------------------------------------------
//
//:
// Non-maximally suppresses the output image by searching along the edge
// normal and checking that the test edge has a greater that the interpolated
// neighbours in the direction. We have also included sub-pixel interpolation
// of the peak position by parabolic fitting.  Writes edges into the thick_
// image.
//
void osl_canny_rothwell::Non_maximal_suppression()
{
  float h1=0,h2=0;
  auto k = float(vnl_math::deg_per_rad);

  // Add 1 to get rid of border effects
  for (unsigned int x=w0_; x+2+w0_<xsize_; ++x)  {
    float *g0 = grad_[x];
    float *g1 = grad_[x+1];
    float *g2 = grad_[x+2];
    float *dx = dx_[x+1];
    float *dy = dy_[x+1];

    for (unsigned int y=w0_; y+2+w0_<ysize_; ++y)  {
      // First check that we have an edge
      if ( g1[y+1] > low_ ) {
        double theta = k*std::atan2(dy[y+1],dx[y+1]);

        // Now work out which direction wrt the eight-way
        // neighbours the edge normal points
        int orient = int(theta/45.0+8) % 4;

        // And now compute the interpolated heights
        float grad;
        switch ( orient ) {
         case 0:
          grad = dy[y+1]/dx[y+1];
          h1 = grad*g0[y] + (1 - grad)*g0[y+1];
          h2 = grad*g2[y] + (1 - grad)*g2[y+1];
          break;
         case 1:
          grad = dx[y+1]/dy[y+1];
          h1 = grad*g0[y] + (1 - grad)*g1[y];
          h2 = grad*g2[y] + (1 - grad)*g1[y];
          break;
         case 2:
          grad = -dx[y+1]/dy[y+1];
          h1 = grad*g2[y] + (1 - grad)*g1[y];
          h2 = grad*g0[y] + (1 - grad)*g1[y];
          break;
         case 3:
          grad = -dy[y+1]/dx[y+1];
          h1 = grad*g2[y] + (1 - grad)*g2[y+1];
          h2 = grad*g0[y] + (1 - grad)*g0[y+1];
          break;
         default:
          std::cerr << "*** ERROR ON SWITCH IN NMS ***: orient="<< orient<< '\n';
          std::abort();
        }

        // If the edge is greater than h1 and h2 we are at a peak,
        // therefore do subpixel interpolation by fitting a parabola
        // along the NMS line and finding its peak
        if ( (g1[y+1]>h1) && (g1[y+1]>h2) )
        {
          float fraction = (h1-h2)/(2*(h1-2*g1[y+1]+h2));
          float newx=0.f, newy=0.f;
          switch ( orient ) {
           case 0:
            newx = x + fraction;
            newy = y + dy[y+1]/dx[y+1]*fraction;
            break;
           case 1:
            newx = x + dx[y+1]/dy[y+1]*fraction;
            newy = y + fraction;
            break;
           case 2:
            newx = x + dx[y+1]/dy[y+1]*fraction;
            newy = y + fraction;
            break;
           case 3:
            newx = x - fraction;
            newy = y - dy[y+1]/dx[y+1]*fraction;
            break;
           default:
            std::cerr<<"*** ERROR ON SWITCH IN NMS ***: orient="<< orient<< '\n';
            std::abort();
          }

          // Now store the edge data, re-use dx_[][] and dy_[][]
          // for sub-pixel locations (don't worry about the junk
          // that is already in them).
          // + 0.5 is to account for targetjr display offset
          thick_[x+1][y+1] = g1[y+1]; // Should this be interpolated height --
          dx[y+1] = newx + 1.5f;   // = g1[y+1] + frac*(h2-h1)/4 ?
          dy[y+1] = newy + 1.5f;
          theta_[x+1][y+1] = float(theta);
        }
      }
    }
  }
}


//-----------------------------------------------------------------------------
//
//:
// Hysteresis follows edgel chains that lie above the low_ threshold and
// have at least one edgel above the high_ threshold. Once we have followed,
// the good edgelchains are re-written to the thin_ image for further
// processing.
//
void osl_canny_rothwell::Initial_hysteresis()
{
  std::list<int> xcoords,ycoords;
  std::list<float> grad;
  std::list<osl_edgel_chain*> edges;
  float *thin,*px,*py,*pg;
  osl_edgel_chain *edgels;

  // Find a point above high_ and start to follow it.
  // First time round we are just trying to get rid of the weak dangling chains
  // and so we will record the good edges and then re-insert them in the thin_
  // image and follow a second time.
  edges.clear();
  for (unsigned int x=w0_; x+w0_<xsize_; ++x)  {
    thin = thin_[x];
    for (unsigned int y=w0_; y+w0_<ysize_; ++y)
      if ( thin[y]>high_ ) {
        Initial_follow(thin_, xsize_, ysize_, low_,
                       x,y,&xcoords,&ycoords,&grad);

        // Create an edge chain and add to the list
        edgels = new osl_edgel_chain(xcoords.size());
        px = edgels->GetX();
        py = edgels->GetY();
        pg = edgels->GetGrad();
        while ( xcoords.size() ) {
          *(px++) = float(xcoords.front()); xcoords.pop_front();
          *(py++) = float(ycoords.front()); ycoords.pop_front();
          *(pg++) = grad.front(); grad.pop_front();
        }
        edges.push_front(edgels);
      }
  }

  // Now re-create the thin_ image
  osl_canny_base_fill_raw_image(thin_, xsize_, ysize_, 0.0f);
  while (edges.size()) {

    edgels = edges.front(); edges.pop_front();
    px = edgels->GetX();
    py = edgels->GetY();
    pg = edgels->GetGrad();
    for (unsigned int i=0; i<edgels->size(); ++i)
      thin_[int(px[i])][int(py[i])] = pg[i];
    delete edgels;
  }
}


//-----------------------------------------------------------------------------

// see osl_canny_ox.cxx
extern osl_Vertex *osl_find(std::list<osl_Vertex*> const *l, osl_Vertex const &v);
extern osl_Vertex *osl_find(std::list<osl_Vertex*> const *l, float x, float y);

//:
// Hysteresis follows edgel chains that lie above the low_ threshold and
// have at least one edgel above the high_ threshold. Due to the Initial_hysteresis
// phase, all edges greater than low_ will be by default good and so have a member
// greater than high_.
//
void osl_canny_rothwell::Final_hysteresis(std::list<osl_edge*> *edges)
{
  std::list<int> xcoords,ycoords;
  std::list<float> grad;
  float *thin,*px,*py,*pg,*pt,val;

  chain_no_ = 10;  // Must be set to a number >= 1

  // Find a point above high_ and start to follow it (but not a dummy point).
  edges->clear();
  for (unsigned int x=w0_; x+w0_<xsize_; ++x)  {
    thin = thin_[x];
    for (unsigned int y=w0_; y+w0_<ysize_; ++y) {
      // Due to Initial_hysteresis we can follow everything > low_
      if ( thin[y]<=low_ || junction_[x][y] )
        continue;

      // Set up the following variable [what a pointless comment]
      chain_no_++;

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
        // std::cerr << "short list found in Final_follow\n";
        continue;

      // count the number of non-dummy edgels
      int count=0;
      for (const auto & i : grad)
        if ( i != dummy_ )
          count++;

      // If the count is less than two we cannot accept
      // the edgelchain. Smallest chain must either be `ee',
      // `je', `ej' or `jj' (for `e'dge and `j'unction)
      if ( count < 2 )
        continue;

      // Create an osl_edgel_chain
      auto *dc = new osl_edgel_chain(count);
      px = dc->GetX();     py = dc->GetY();
      pg = dc->GetGrad();  pt = dc->GetTheta();

      // Write the edgels and end points to the osl_edgel_chain
      //dc->SetStart(xcoords.front()+xstart_, ycoords.front()+ystart_);
      while (count) {
        int tmpx = xcoords.front(); xcoords.pop_front();
        int tmpy = ycoords.front(); ycoords.pop_front();
        val = grad.front(); grad.pop_front();
        if ( val != dummy_ ) {
          --count;
          // If we are not at a junction use sub-pixel value
          if ( val != jval_ ) {
            *(px++) = dx_[tmpx][tmpy] + xstart_;
            *(py++) = dy_[tmpx][tmpy] + ystart_;
            *(pg++) = val;
          }
          else {
            *(px++) = float(tmpx + xstart_);
            *(py++) = float(tmpy + ystart_);
            *(pg++) = 0.0f;   // Mark the gradient as zero at a junction
          }
          if (theta_[tmpx][tmpy] == DUMMYTHETA) {
            const auto k = float(vnl_math::deg_per_rad);
            float *dx = dx_[tmpx];
            float *dy = dy_[tmpx];

            // *** Bug fix, Samer Abdallah May 10, 1995:  next line was
            // theta_[tmpx][tmpy]  = k*std::atan2(dy[y],dx[y]);
            theta_[tmpx][tmpy]  = k*(float)std::atan2(dy[tmpy],dx[tmpy]);
          }

          *(pt++) = theta_[tmpx][tmpy];
        }
//      dc->SetEnd(tmpx+xstart_, tmpy+ystart_);
      }

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

        auto *v1 = new osl_Vertex(dc->GetX(0), dc->GetY(0));
        auto *v2 = new osl_Vertex(dc->GetX(dc->size()-1), dc->GetY(dc->size()-1));

        // Check whether each vertex is a junction
        osl_Vertex *V1=osl_find(vlist_, *v1);
        osl_Vertex *V2=osl_find(vlist_, *v2);

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
          { osl_IUDelete (v1); }
          if ( !V2 )
            V2 = v2;
          else
          { osl_IUDelete(v2); }
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
void osl_canny_rothwell::Thin_edges()
{
  // Now do the thinning. Do it twice: the first time to try to remove
  // dummy_ edges, and then other edges -- 0.001 turns <= to <
  float threshold=dummy_-0.001f;

  for (int i=0; i<2; threshold=low_,++i)
  {
    int count = 1;     // count set to nonzero value
    while (count) //  Thin until no Pixels are removed
    {
      count = 0;
      for (unsigned int x=w0_; x+w0_<xsize_; ++x)
        for (unsigned int y=w0_; y+w0_<ysize_; ++y)
        {
          if ( thin_[x][y] <= threshold )
            continue;

          int a = thin_[x-1][y-1] > low_ ? 1 : 0;
          int b = thin_[x  ][y-1] > low_ ? 1 : 0;
          int c = thin_[x+1][y-1] > low_ ? 1 : 0;
          int d = thin_[x+1][y  ] > low_ ? 1 : 0;
          int e = thin_[x+1][y+1] > low_ ? 1 : 0;
          int f = thin_[x  ][y+1] > low_ ? 1 : 0;
          int g = thin_[x-1][y+1] > low_ ? 1 : 0;
          int h = thin_[x-1][y  ] > low_ ? 1 : 0;

          int genus = a+b+c+d+e+f+g+h;

          // If the pixel is dangling, record, and go to next loop
          if ( genus == 1 )
            continue;

          genus += h*a*b+b*c*d+d*e*f+f*g*h-a*b-b*c-c*d-d*e-e*f-f*g
                 - g*h-h*a-h*b-b*d-d*f-f*h-1;

          // If the genus is zero delete the edge
          if ( genus == 0 ) {
            ++count;
            thin_[x][y] = 0.0;
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
void osl_canny_rothwell::Jump_single_breaks()
{
  // Take each dangling end in turn and determine whether there is
  // another edgel within the sixteen one-pixel-distant-neighbours.

  int x,y,i,j;
  float **t = thin_;
  // xdang_->reset();  ydang_->reset();

  while (xdang_->size()) {

    x = xdang_->front(); xdang_->pop_front();
    y = ydang_->front(); ydang_->pop_front();

    // This is messy for efficiency
    if      (thin_[x-1][y-1]) {i=-1; j=-1;}
    else if (thin_[x  ][y-1]) {i= 0; j=-1;}
    else if (thin_[x+1][y-1]) {i= 1; j=-1;}
    else if (thin_[x+1][y  ]) {i= 1; j= 0;}
    else if (thin_[x+1][y+1]) {i= 1; j= 1;}
    else if (thin_[x  ][y+1]) {i= 0; j= 1;}
    else if (thin_[x-1][y+1]) {i=-1; j= 1;}
    else if (thin_[x-1][y  ]) {i=-1; j= 0;}
    else { i = j = 0; } // Dummy declaration

    // If the entrant edgel is diagonal
    if ( i && j ) {

      if      (t[x-2*i][y-2*j]>low_) {
        t[x-i][y-j]=dummy_;thick_[x-i][y-j]=dummy_;}
      else if ((t[x+i][y-2*j]>low_)||(t[x  ][y-2*j]>low_)||(t[x-i][y-2*j]>low_)) {
        t[x  ][y-j]=dummy_;thick_[x  ][y-j]=dummy_;}
      else if ((t[x-2*i][y+j]>low_)||(t[x-2*i][y  ]>low_)||(t[x-2*i][y-j]>low_)) {
        t[x-i][y]=dummy_;thick_[x-i][y  ]=dummy_;}
      else if ((!(t[x+2*i][y  ]>low_))&&((t[x+2*i][y-j]>low_)||(t[x+2*i][y-2*j]>low_))) {
        t[x+i][y-j]=dummy_;thick_[x+i][y-j]=dummy_;}
      else if ((!(t[x  ][y+2*j]>low_))&&((t[x-i][y+2*j]>low_)||(t[x-2*i][y+2*j]>low_))) {
        t[x-i][y+j]=dummy_;thick_[x-i][y+j]=dummy_;}
    }

    // For entrant with i=0
    else if ( i == 0 ) {

      if ((t[x-1][y-2*j]>low_)||(t[x  ][y-2*j]>low_)||(t[x+1][y-2*j]>low_)) {
        t[x  ][y-j]=dummy_;thick_[x  ][y-j]=dummy_;}
      else if ((t[x+2][y-j]>low_)||(t[x+2][y-2*j]>low_)) {
        t[x+1][y-j]=dummy_;thick_[x+1][y-j]=dummy_;}
      else if ((t[x-2][y-j]>low_)||(t[x-2][y-2*j]>low_)) {
        t[x-1][y-j]=dummy_;thick_[x-1][y-j]=dummy_;}
      else if ((!(t[x-2][y+j]>low_))&&(t[x-2][y]>low_)) {
        t[x-1][y  ]=dummy_;thick_[x-1][y  ]=dummy_;}
      else if ((!(t[x+2][y+j]>low_))&&(t[x+2][y]>low_)) {
        t[x+1][y  ]=dummy_;thick_[x+1][y  ]=dummy_;}
    }
    // or finally for j=0

    // *** Bug fix, Samer Abdallah May 10, 1995:  next line was
    // else if ( i == 0 )
    else if ( j == 0 ) {

      if      ((t[x-2*i][y-1]>low_)||(t[x-2*i][y]>low_)||(t[x-2*i][y]>low_)) {
        t[x-i][y]=dummy_;thick_[x-i][y]=dummy_;}
      else if ((t[x-i][y+2]>low_)||(t[x-2*i][y+2]>low_)) {
        t[x-1][y+j]=dummy_;thick_[x-1][y+j]=dummy_;}
      else if ((t[x-i][y-2]>low_)||(t[x-2*i][y-2]>low_)) {
        t[x-1][y-j]=dummy_;thick_[x-1][y-j]=dummy_;}
      else if ((!(t[x+i][y-2]>low_))&&(t[x][y-2]>low_)) {
        t[x][y-1]=dummy_;thick_[x][y-1]=dummy_;}
      else if ((!(t[x+i][y+2]>low_))&&(t[x][y+2]>low_)) {
        t[x][y+1]=dummy_;thick_[x][y+1]=dummy_;}
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
void osl_canny_rothwell::Adaptive_Canny(vil1_image const &image)
{
  // Reset the smoothing kernel parameters by
  // halving the size of the smoothing sigma
  old_sigma_ = sigma_;  sigma_ /= 2.0f;
  old_width_ = width_;
  width_ = int(sigma_*std::sqrt(2*std::log(1.0/gauss_tail_))+1);
  old_k_size_ = k_size_;  k_size_ = 2*width_+ 1;
  delete kernel_; kernel_ = new float [k_size_];
  osl_kernel_DOG(sigma_, kernel_, k_size_, width_);

  // Define the new ROI to account for the domain of the old smoothing
  // kernel and for that of the new one
  int image_size = old_k_size_ + k_size_ - 1;
  int half_size = (image_size - 1)/2;

  if (verbose) std::cerr << "new image region "
                        << image_size << " by " << image_size << std::endl
                        << "Sigma           = " << sigma_ << std::endl
                        << "Kernel size     = " << k_size_ << std::endl;

  // Set up the new images
  float **dx   = osl_canny_base_make_raw_image(image_size,image_size, (float*)nullptr);
  float **dy   = osl_canny_base_make_raw_image(image_size,image_size, (float*)nullptr);
  float **grad = osl_canny_base_make_raw_image(image_size,image_size, (float*)nullptr);

  // For each dangling-end (X,Y), search for more edges at the reduced scale
  int count=0;
  if (verbose) std::cerr << "percentage of endings examined =   0";
  for (auto i=xdang_->begin(), j=ydang_->begin(); i!=xdang_->end() && j!=ydang_->end(); ++i, ++j)
  {
    //xdang_->reset(),ydang_->reset(); xdang_->next(),ydang_->next(); )  {

    int X = (*i)/*xdang_->value()*/, Y = (*j)/*ydang_->value()*/;
    int x0 = X-half_size,   y0 = Y - half_size;  // Region origin in image coords

    // Make sure that the region around the end is within the
    // bounds of the original image, and that the dangling end
    // is not a neighbour to a junction (if it is we recover the
    // topology more robustly by growing something else towards
    // it, rather than the other way round)
    if ( !Junction_neighbour(junction_, X, Y) &&
         (x0>=0) && ((unsigned int)x0+image_size<=xsize_) &&
         (y0>=0) && ((unsigned int)y0+image_size<=ysize_) )
    {
      // Compute the new image intensity gradients
      osl_canny_smooth_rothwell_adaptive(image, x0,y0,image_size, kernel_, width_, k_size_, dx,dy,grad);

      // Delete the effects of the thick_ image - we don't want to
      // locate edges in the same places as before
      Subtract_thick(x0,y0,image_size,grad);

      // Now, if possible, grow the edgelchain out from (X,Y) in a
      // one dimensional direction up to the boundary of the old kernel
      int newx,newy;
      X -= x0;  Y -= y0;   // Do a coordinate shift; centre in local coords
      for (int ii=1; ii<=old_k_size_ && Dangling_end(X+x0,Y+y0)==1; ++ii)
      {
        // Find the eight-way neighbour with the strongest edge
        newx = X;  newy = Y;
        Best_eight_way(X,Y,grad,&newx,&newy);
        // If no new edge has been found we should break
        if ( (newx==X) && (newy==Y) )
          break;
        // Else record a dummy edgel
        else {
          thin_[newx+x0][newy+y0] = dummy_;  thick_[newx+x0][newy+y0] = dummy_;
          X = newx;  Y = newy;
        }
      }
    }
    if (verbose) std::fprintf(stderr,"\b\b\b%3d", int(10*((++count)*10/xdang_->size())));
  }
  if (verbose)   std::cerr << std::endl;

  // Remove the image arrays
  osl_canny_base_free_raw_image(dx);
  osl_canny_base_free_raw_image(dy);
  osl_canny_base_free_raw_image(grad);
}


//-----------------------------------------------------------------------------
//
//: Wherever the thick_ image has an edge marked set the new gradient value to zero.
void osl_canny_rothwell::Subtract_thick(int x0, int y0, int image_size, float **grad)
{
  for (int x=width_; x<image_size-width_; ++x)
    for (int y=width_; y<image_size-width_; ++y)
      if ( thick_[x+x0][y+y0] > low_ )
        grad[x][y] = 0.0f;
}


//-----------------------------------------------------------------------------
//
//: Returns the eight-way neighbour with the strongest contrast change.
//
//fsm: No, this routine returns the last of the eight neighbours with contrast
//change greater than 'low_'. Probably a bug. FIXME
void osl_canny_rothwell::Best_eight_way(int x, int y, float **grad, int *xnew, int *ynew)
{
  float max = low_;
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
void osl_canny_rothwell::Find_dangling_ends()
{
  // Reset the dangling ends
  xdang_->clear();
  ydang_->clear();
  osl_canny_base_fill_raw_image(dangling_, xsize_, ysize_, 0);

  for (unsigned int x=w0_; x+w0_<xsize_; ++x)
    for (unsigned int y=w0_; y+w0_<ysize_; ++y)
    {
      if (Dangling_end(x,y) == 1) {
        xdang_->push_front(x);
        ydang_->push_front(y);
        dangling_[x][y] = 1;
      }
    }
}


//-----------------------------------------------------------------------------
//
//: Tests whether a point is a dangling end and return 1 in that case.
//  Otherwise the return value could be 0 or between 2 and 8.
//
int osl_canny_rothwell::Dangling_end(int x, int y)
{
  if ( thin_[x][y] <= low_ )
    return 0;

  int a = thin_[x-1][y-1] > low_ ? 1 : 0;
  int b = thin_[x  ][y-1] > low_ ? 1 : 0;
  int c = thin_[x+1][y-1] > low_ ? 1 : 0;
  int d = thin_[x+1][y  ] > low_ ? 1 : 0;
  int e = thin_[x+1][y+1] > low_ ? 1 : 0;
  int f = thin_[x  ][y+1] > low_ ? 1 : 0;
  int g = thin_[x-1][y+1] > low_ ? 1 : 0;
  int h = thin_[x-1][y  ] > low_ ? 1 : 0;

  return a+b+c+d+e+f+g+h;
}


//-----------------------------------------------------------------------------
//
//: Searches for the junctions in the image.
//
void osl_canny_rothwell::Find_junctions()
{
  // Reset the junction variables
  xjunc_->clear();
  yjunc_->clear();
  osl_canny_base_fill_raw_image(junction_, xsize_, ysize_, 0);

  for (unsigned int x=w0_; x+w0_<xsize_; ++x)
    for (unsigned int y=w0_; y+w0_<ysize_; ++y)
    {
      if (Dangling_end(x,y) > 2) {
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
void osl_canny_rothwell::Find_junction_clusters()
{
  std::list<int> xcoords,ycoords,xvertices,yvertices,xjunc,yjunc;
  // Find a junction and follow
  xvertices.clear();  yvertices.clear();
  xjunc.clear();      yjunc.clear();
  for (unsigned int x=w0_; x+w0_<xsize_; ++x)
    for (unsigned int y=w0_; y+w0_<ysize_; ++y)
      if ( junction_[x][y] ) {

        // Each cluster is written to (xcoords,ycooords)
        xcoords.clear();  ycoords.clear();
        Follow_junctions(junction_, x,y,&xcoords,&ycoords);

        // Find the `centre' of the cluster. This is defined as the
        // junction closest to the centre of gravity of the cluster
        int x0,y0;
        Cluster_centre_of_gravity(jx_, jy_, xcoords,ycoords,x0,y0);

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
    junction_[xjunc.front()][yjunc.front()] = 1;
    xjunc.pop_front();
    yjunc.pop_front();
  }

  // Construct the list of junction cluster centres
  vlist_->clear();
  for (auto i=xvertices.begin(), j=yvertices.begin();
       i!=xvertices.end() && j!=yvertices.end();
       ++i, ++j) {
    //for (xvertices.reset(),yvertices.reset(); xvertices.next(),yvertices.next(); )  {

    auto *v = new osl_Vertex( float((*i)/*xvertices.value()*/+xstart_),
                                    float((*j)/*yvertices.value()*/+ystart_));
    vlist_->push_front(v);
    junction_[(*i)/*xvertices.value()*/][(*j)/*yvertices.value()*/] = 2;
  }

  xvertices.clear();
  yvertices.clear();
}
