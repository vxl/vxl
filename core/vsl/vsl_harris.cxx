#include "vsl_harris.h"
#include <vsl/vsl_roi_window.h>
#include <vsl/vsl_convolve.h>
#include "droid.h"

#include <vcl/vcl_climits.h> // CHAR_BIT
#include <vcl/vcl_cmath.h>
#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_fstream.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_functional.h>

#include <vbl/vbl_printf.h>

#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_copy.h>

//--------------------------------------------------------------

vsl_harris::vsl_harris(vsl_harris_params const & params) 
  : vsl_harris_params(params) 
  , _params(*this)
{ 
}

vsl_harris::~vsl_harris() {
  uninit_module();
}

//------------------------------------------------------------

//: initialization of buffers.
void vsl_harris::init_module (vil_image image) {
  // store size :
  image_h = image.height ();
  image_w = image.width ();
  
  /* set up window. */
  window_str.row_start_index = 0;
  window_str.col_start_index = 0;
  window_str.row_end_index = image_h-1;
  window_str.col_end_index = image_w-1;
  
  // set up response images etc.
  //no longer:
  // we have to explicitly ref() and unref() the buffers
  // because we want to refer to them as vil_memory_image_of<T>s and not generic images.
  image_ptr            = new vil_byte_buffer   (image_w, image_h); //image_ptr->ref();
  image_gradx_ptr      = new vil_int_buffer    (image_w, image_h); //image_gradx_ptr->ref();
  image_grady_ptr      = new vil_int_buffer    (image_w, image_h); //image_grady_ptr->ref();
  image_fxx_ptr        = new vil_float_buffer  (image_w, image_h); //image_fxx_ptr->ref();
  image_fxy_ptr        = new vil_float_buffer  (image_w, image_h); //image_fxy_ptr->ref();
  image_fyy_ptr        = new vil_float_buffer  (image_w, image_h); //image_fyy_ptr->ref();
  image_cornerness_ptr = new vil_float_buffer  (image_w, image_h); //image_cornerness_ptr->ref();
  image_cornermax_ptr  = new vil_bool_buffer   (image_w, image_h); //image_cornermax_ptr->ref();

  // copy input image to buffer.
  if (image.planes()             ==1 && 
      image.components()         ==1 && 
      image.bits_per_component() ==CHAR_BIT &&
      image.component_format()==VIL_COMPONENT_FORMAT_UNSIGNED_INT) {
    // byte greyscale
    vil_copy(image, *image_ptr);
  }
  else if (image.planes()             ==1 && 
	   image.components()         ==3 && 
	   image.bits_per_component() ==CHAR_BIT &&
	   image.component_format()==VIL_COMPONENT_FORMAT_UNSIGNED_INT) {
    // byte rgb
    vcl_vector<vil_byte> buf(3*image_w);
    for (unsigned j=0; j<image_h; ++j) {
      image.get_section(buf.begin(), 0, j, image_w, 1);
      for (unsigned i=0; i<image_w; ++i)
	(*image_ptr)[j][i] = (unsigned(buf[3*i+0]) + unsigned(buf[3*i+1]) + unsigned(buf[3*i+2]))/3;
    }
  }
  else
    assert(false/* implement for your image type as needed */);
}

void vsl_harris::uninit_module() {
#define macro(ptr) { delete ptr; ptr=0; }
  macro(image_ptr);
  macro(image_fxx_ptr);
  macro(image_fxy_ptr);
  macro(image_fyy_ptr);
  macro(image_gradx_ptr);
  macro(image_grady_ptr);
  macro(image_cornerness_ptr);
  macro(image_cornermax_ptr);
#undef macro
}

//-----------------------------------------------------------------------------

void vsl_harris::compute(vil_image image) {
  // set up bitmaps etc :
  this->init_module(image);
  
  // compute cornerness map :
  if (verbose)
    cerr << "Doing harris on image region "
	 << image_h << " by " << image_w << endl
	 << "Maximum no of corners                     = " << _params.corner_count_max << endl
	 << "Gaussian sigma                            = " << _params.gauss_sigma << endl
	 << "Expected ratio lowest/max corner strength = " << _params.relative_minimum << endl
	 << "Auto-correlation scale factor             = " << _params.scale_factor << endl
	 << "Computing cornerness operator response...." << flush;
  compute_response(); // sets corner_max
  if (verbose)
    cerr << "  done" << endl;

  // do the relevant harris flavours :
  int final;

  if (_params.adaptive) {
    do_adaptive(); 
    // cryptic comment: OK, now thresholding on thresh_lo will get only the goodies.
    final = dr_store_corners (0.0);
  }
  else {
    double corner_min = _params.relative_minimum * corner_max;
    do_non_adaptive(&corner_min);
    final = dr_store_corners (corner_min);
  }
  cerr << "vsl_harris: Final corner count " << final << endl;
}


//--------------------------------------------------------------------------------

//: compute the cornerness map and its maximum value.
void vsl_harris::compute_response() {
  if (verbose)
    cerr << " gradient" << flush;
  window_str.row_start_index += 2;
  window_str.col_start_index += 2;
  window_str.row_end_index   -= 2;
  window_str.col_end_index   -= 2;
  droid::compute_gradx_grady (&window_str,
			      image_ptr,
			      image_gradx_ptr,
			      image_grady_ptr);

  if (verbose)
    cerr << " fxx,fxy,fyy" << flush;
  window_str.row_start_index += 2;
  window_str.col_start_index += 2;
  window_str.row_end_index   -= 2;
  window_str.col_end_index   -= 2;
  droid::compute_fxx_fxy_fyy (&window_str,
			      image_gradx_ptr,
			      image_grady_ptr,
			      image_fxx_ptr,
			      image_fxy_ptr,
			      image_fyy_ptr);
  
  if (verbose)
    cerr << " convolution" << flush;

  // create smoothing kernel
  vsl_1d_half_kernel<double> gauss_mask;
  vsl_create_gaussian (double(_params.gauss_sigma), &gauss_mask);

  // use as temporary. *** overwrites current cornerness map ***.
  vil_float_buffer *tmp = image_cornerness_ptr;
  // we don't need this.
  vsl_roi_window dummy_window_str;
  if (_params.pab_emulate) {
    vsl_convolve(&window_str, &dummy_window_str, &gauss_mask, image_fxx_ptr, tmp);// This was 
    vsl_convolve(&window_str, &dummy_window_str, &gauss_mask, image_fxy_ptr, tmp);// probably
    vsl_convolve(&window_str, &window_str,       &gauss_mask, image_fyy_ptr, tmp);// <- a bug.
  }
  else {
    vsl_convolve(&window_str, &dummy_window_str, &gauss_mask, image_fxx_ptr, tmp);
    vsl_convolve(&window_str, &dummy_window_str, &gauss_mask, image_fxy_ptr, tmp);
    vsl_convolve(&window_str, &dummy_window_str, &gauss_mask, image_fyy_ptr, tmp);
  }
  
  if (verbose)
    cerr << " cornerness" << flush;
  corner_max = droid::compute_cornerness (&window_str,
					  image_fxx_ptr,image_fxy_ptr,image_fyy_ptr,
					  _params.scale_factor,
					  image_cornerness_ptr);

}

//--------------------------------------------------------------------------------

//: internal
void vsl_harris::do_non_adaptive(double* corner_min) {
  int maxima_count = droid::find_corner_maxima (*corner_min,
						&window_str,
						image_cornerness_ptr,
						image_cornermax_ptr);

  // iterate if not enough corners.
  
  if (verbose) 
    cerr << "Found " << maxima_count << " corners\n";
  
  if (maxima_count < (float) _params.corner_count_max * 0.9) {
    for (int i=0 ; i<10 && maxima_count < (float) _params.corner_count_max * 0.9; i++) {
      _params.relative_minimum *= 0.5;
      *corner_min = _params.relative_minimum * corner_max;
      if (verbose) 
	cerr << "Found " << maxima_count
	     << "... iterating with relmin = " << _params.relative_minimum
	     << endl;
      maxima_count = droid::find_corner_maxima (*corner_min,
						&window_str,
						image_cornerness_ptr,
						image_cornermax_ptr);
    }
  }
  
  // too many corners - reset parameters to get max number.

  if (maxima_count > _params.corner_count_max) {
    *corner_min = droid::compute_corner_min (*corner_min, 
					    corner_max,
					    _params.corner_count_max,
					    &window_str,
					    image_cornerness_ptr,
					    image_cornermax_ptr);
    
    _params.relative_minimum = *corner_min / corner_max;
    if (verbose) 
      cerr << "vsl_harris: Too many: " << maxima_count
	   << "... iterating with relmin = " << _params.relative_minimum
	   << endl;
  }
}

//--------------------------------------------------------------------------------

//: internal
void vsl_harris::do_adaptive() {
  if (verbose)
    cerr << "No. of corners before density thresholding= " << _params.corner_count_max << endl;

  double corner_min = _params.relative_minimum * corner_max;
  int maxima_count = droid::find_corner_maxima (corner_min,
						&window_str,
						image_cornerness_ptr,
						image_cornermax_ptr);
  cerr << "harris: " << maxima_count << " corners with response above " << corner_min << endl;

  // Store all corners in an array.
  int TILE_WIDTH = _params.adaptive_window_size; // 32
  int row_min = window_str.row_start_index;
  int col_min = window_str.col_start_index;
  int row_max = window_str.row_end_index;
  int col_max = window_str.col_end_index;

  int n_tiles_x = (int)ceil((double)(col_max-col_min) / TILE_WIDTH);
  int n_tiles_y = (int)ceil((double)(row_max-row_min) / TILE_WIDTH);

  double TILE_AREA = TILE_WIDTH*TILE_WIDTH;

  int IDEAL_NUM_PER_TILE = _params.corner_count_max / (n_tiles_x*n_tiles_y);

  if (_params.corner_count_max == 0)
    IDEAL_NUM_PER_TILE = _params.corner_count_low;

  vbl_printf(cerr, "Tiles %d x %d, NUM_PER_TILE %d\n", n_tiles_x, n_tiles_y, IDEAL_NUM_PER_TILE);
  
  vcl_vector<double> cornerness(maxima_count, 0.0);

  for(int tile_y = 0; tile_y < n_tiles_y; ++tile_y) {
    int window_row_start_index = tile_y * TILE_WIDTH+row_min;
    int window_row_end_index = vcl_min(window_row_start_index+TILE_WIDTH, row_max);
    
    for(int tile_x = 0; tile_x < n_tiles_x; ++tile_x) {
      int window_col_start_index = tile_x * TILE_WIDTH+col_min;
      int window_col_end_index = vcl_min(window_col_start_index+TILE_WIDTH, col_max);

      // get corner strengths in this tile :
      vil_memory_image_of<bool>        &corner_present  = *image_cornermax_ptr;
      vil_memory_image_of<float> const &corner_strength = *image_cornerness_ptr;
      int n = 0;
      for(int row = window_row_start_index; row < window_row_end_index; row++)
	for(int col = window_col_start_index; col < window_col_end_index; col++)
	  if (corner_present[row][col])
	    cornerness[n++] = corner_strength[row][col];
      vbl_printf(cerr, "%3d ", n);

      //
      double THIS_TILE_AREA = 
	(window_row_end_index-window_row_start_index) *
	(window_col_end_index-window_col_start_index);

      int NUM_PER_TILE = (int) ( IDEAL_NUM_PER_TILE * (THIS_TILE_AREA/TILE_AREA) );

      if (n > NUM_PER_TILE) {
	// Sort corners to get thresholds
	vcl_sort(cornerness.begin(), cornerness.begin()+n);
	double thresh = cornerness[n-1-NUM_PER_TILE];

	// Zap corners over thresh
	for(int row = window_row_start_index; row < window_row_end_index; row++)
	  for(int col = window_col_start_index; col < window_col_end_index; col++)
	    if (corner_present[row][col])
	      if (corner_strength[row][col] < thresh)
		corner_present[row][col] = false;
      }
    }
    cerr << endl;
  }
}

//-----------------------------------------------------------------------------

int vsl_harris::dr_store_corners (float corner_min) {
  cx.clear();
  cy.clear();

  for (int row = window_str.row_start_index; row < window_str.row_end_index; row++)
    for (int col = window_str.col_start_index; col < window_str.col_end_index; col++)
      if ((*image_cornermax_ptr) [row][col] && (*image_cornerness_ptr) [row][col] > corner_min) {
	double x, y;
	if (droid::compute_subpixel_max (image_cornerness_ptr, row, col, x,y, _params.pab_emulate)) {
	  cx.push_back(col_start_index+x);
	  cy.push_back(row_start_index+y);
	}
      }
  
  assert(cx.size() == cy.size());
  return cx.size();
}

void vsl_harris::get_corners(vcl_vector<float> &corx, vcl_vector<float> &cory) const {
  assert(cx.size() == cy.size());
  // ?? should we append, and not assign ??
  corx = cx;
  cory = cy;
}

//: convenience method
void vsl_harris::save_corners(char const *filename) const {
  ofstream f(filename);
  assert(f);

  assert(cx.size() == cy.size());
  unsigned n=cx.size();
  for (unsigned i=0; i<n; ++i)
    f << cx[i] << ' ' << cy[i] << endl;
  
  f.close();
}

//-----------------------------------------------------------------------------
