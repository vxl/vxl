#ifndef rgrl_matcher_pseudo_int_3d_txx_
#define rgrl_matcher_pseudo_int_3d_txx_

#include "rgrl_matcher_pseudo_int_3d.h"
#include "rgrl_feature_face_region.h"
#include "rgrl_feature_trace_region.h"
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_macros.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_int_3.h>
#include <vnl/algo/vnl_svd.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vbl/vbl_bounding_box.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_cassert.h>

static const double max_response_value = 1.0e30;

//#define MY_DEBUG
#if defined ( MY_DEBUG )
#  include <vcl_iostream.h>
#  include <vcl_fstream.h>
#  include <vcl_sstream.h>
#  define DBG(x) x
#else
#  define DBG(x)
#endif

// convert pixel points to physical points
inline
void
pixel_to_physical( vnl_int_3    const& pixel_loc,
                   vnl_double_3      & point,
                   vnl_double_3 const& spacing_ratio )
{
  for ( unsigned i = 0; i < 3; ++i )
    point[ i ] = spacing_ratio[ i ] * double(pixel_loc[ i ]);
}

// convert physical points to pixel points
inline
void
physical_to_pixel( vnl_double_3 const& point,
                   vnl_int_3         & pixel_loc,
                   vnl_double_3 const& spacing_ratio )
{
  for ( unsigned i = 0; i < 3; ++i )
    pixel_loc[ i ] = (int) vnl_math_rnd( point[ i ] / spacing_ratio[ i ] );
}

// convert physical points to pixel points
inline
void
physical_to_pixel( vnl_double_3 const& point,
                   vnl_double_3      & pixel_loc,
                   vnl_double_3 const& spacing_ratio )
{
  for ( unsigned i = 0; i < 3; ++i )
    pixel_loc[ i ] = point[ i ] / spacing_ratio[ i ] ;
}

// check if the location is inside the mask and the image.
template <class PixelType>
inline
bool
pixel_in_range( vil3d_image_view< PixelType > const& image,
                rgrl_mask_sptr const& mask,
                vnl_int_3 const& location )
{
//    vnl_vector< double > loc_dbl( location.size() );
//    for ( unsigned i = 0; i < location.size(); ++i )
//      loc_dbl[ i ] = location[ i ];

  // mask is only 2D,
  // to be sure, check all dim
  if ( location[ 0 ] < 0 || location[ 0 ] > (int)image.ni()-1 ||
       location[ 1 ] < 0 || location[ 1 ] > (int)image.nj()-1 ||
       location[ 2 ] < 0 || location[ 2 ] > (int)image.nk()-1 )
  return false;

  if ( mask ) {
    // So far, 3D CT images can use one 2D mask image for each slices.
    static vnl_vector< double > loc_dbl( 2 );
    for ( unsigned i = 0; i < 2; ++i )
      loc_dbl[ i ] = double(location[ i ]);

//    vcl_cout << "mask pixel loc: " << loc_dbl << '\n';
    if (  !mask->inside( loc_dbl ) )
      return false;
  }
  return true;
}

template <class PixelType>
inline
bool
physical_in_range( vil3d_image_view< PixelType > const& image,
                   rgrl_mask_sptr const& mask,
                   vnl_double_3 const& location,
                   vnl_double_3 const& spacing_ratio )
{
  vnl_double_3 pixel_loc;
  physical_to_pixel( location, pixel_loc, spacing_ratio );
  // cannot just call pixel_in_range, because the coordinate here is double type

  // mask is only 2D,
  // to be sure, check all dim
  if ( pixel_loc[ 0 ] < 0 || pixel_loc[ 0 ] > (double)(image.ni()-1) ||
       pixel_loc[ 1 ] < 0 || pixel_loc[ 1 ] > (double)(image.nj()-1) ||
       pixel_loc[ 2 ] < 0 || pixel_loc[ 2 ] > (double)(image.nk()-1) )
  return false;

  if ( mask ) {
    // So far, 3D CT images can use one 2D mask image for each slices.
    static vnl_vector< double > loc_dbl( 2 );
    loc_dbl [0] = pixel_loc[0];
    loc_dbl [1] = pixel_loc[1];

    if (  !mask->inside( loc_dbl ) )
      return false;
  }
  return true;
}

template <class PixelType>
rgrl_matcher_pseudo_int_3d< PixelType > ::
rgrl_matcher_pseudo_int_3d( vil3d_image_view<PixelType> const& from_image,
                            vil3d_image_view<PixelType> const& to_image,
                            vnl_vector< double >        const& from_spacing_ratio,
                            vnl_vector< double >        const& to_spacing_ratio,
                            rgrl_evaluator_sptr                evaluator,
                            rgrl_mask_sptr                     mask )
  : from_image_( from_image ),
    to_image_( to_image ),
    mask_( mask ),
    evaluator_( evaluator ),
    from_spacing_ratio_( from_spacing_ratio ),
    to_spacing_ratio_( to_spacing_ratio )
{
  assert( from_spacing_ratio.size() == 3 );
  assert( to_spacing_ratio.size() == 3 );
}


template <class PixelType>
rgrl_match_set_sptr
rgrl_matcher_pseudo_int_3d< PixelType > ::
compute_matches( rgrl_feature_set const&    from_set,
                 rgrl_feature_set const&    to_set,
                 rgrl_view        const&    current_view,
                 rgrl_transformation const& current_xform,
                 rgrl_scale          const& current_scale ) const
{
  vcl_cerr << "compute_matches()" << vcl_endl;

  typedef vcl_vector<rgrl_feature_sptr> f_vector_type;
  typedef f_vector_type::iterator f_iterator_type;

  //  Build an empty match set
  rgrl_match_set_sptr matches_sptr = new rgrl_match_set( from_set.type(), to_set.type() );

  //  Get the from image features in the current view
  f_vector_type from = from_set.features_in_region( current_view.region() );

  //  Vector for mapped pixels
  rgrl_mapped_pixel_vector_type  mapped_pixels;

  //  Vectors for matched features and weights.
  f_vector_type matched_to_features;
  vcl_vector<double> match_weights;

  // reserve space
  matches_sptr->reserve( from.size() );
  // Match each feature...
   for ( f_iterator_type fitr = from.begin(); fitr != from.end(); ++fitr )
   {
     // Match by searching in the tangent space of the
     // transformed from image feature.  The match_weights are to be
     // treated later as similarity weights
     matched_to_features.clear();
     match_weights.clear();

     // Map the feature location using the current transformation
     rgrl_feature_sptr mapped_feature = (*fitr)->transform( current_xform );

     // if the location is not inside the valid region
     // set the weight = 0
     if ( !physical_in_range( to_image_, mask_, mapped_feature->location(), to_spacing_ratio_ ) ) {
       //  Make a dummy vector of intensity weights.
       // vcl_vector< double > dummy_intensity_weights( 0 ); //CT: not needed now
       // vcl_vector< double > match_weights( 0 );

       //  Add the feature and its matches and weights to the match set
       matches_sptr
         -> add_feature_matches_and_weights( *fitr, mapped_feature, matched_to_features,
                                             match_weights );
       DebugMacro(3, " skip match from: " << (*fitr)->location() << ", to: " << mapped_feature->location() << '\n' );
       continue;
     }

     // Map the intensities of the pixels in the from image
     // surrounding the from image feature.  Form a vector of pairs,
     // with each pair containing a mapped location and the
     // associated intensity.
     mapped_pixels.clear();

     DBG(
       if ( (*fitr)->is_type( rgrl_feature_trace_region::type_id() ) ) {
         vcl_cout << "\nfrom :\n" << (*fitr)->location()
                  << " normal: "
                  << rgrl_cast<rgrl_feature_trace_region *> ( *fitr )->normal_subspace().get_column(0)
                  << "\nto :\n" << mapped_feature->location()
                  << " normal: "
                  << rgrl_cast<rgrl_feature_trace_region *> ( mapped_feature )->normal_subspace().get_column(0)
                  << vcl_endl;
       }
       else if ( (*fitr)->is_type( rgrl_feature_face_region::type_id() ) ) {
         vcl_cout << "\nfrom :\n" << (*fitr)->location()
                  << " normal: "
                  << rgrl_cast<rgrl_feature_face_region *> ( *fitr )->normal()
                  << "\nto :\n" << mapped_feature->location()
                  << " normal: "
                  << rgrl_cast<rgrl_feature_face_region *> ( mapped_feature )->normal()
                  << vcl_endl;
       }
     );

     this -> map_region_intensities( current_xform, (*fitr), mapped_pixels );

     // if there is no mapped pixels in the valid region, no matcher is created
     if ( mapped_pixels.size() == 0 ) {
       //  Make a dummy vector of intensity weights.
       // vcl_vector< double > dummy_intensity_weights( 0 ); //CT: not needed now
       // vcl_vector< double > match_weights( 0 );

       //  Add the feature and its matches and weights to the match set
       matches_sptr
         -> add_feature_matches_and_weights( *fitr, mapped_feature, matched_to_features,
                                             match_weights );
       vcl_cout << " from point : " << (*fitr)->location()
                << " to point : " << mapped_feature->location()
                << " doesn't have proper matches\n" << vcl_endl;
       continue;
     }

     this -> slide_window( mapped_feature, mapped_pixels, current_scale,
                                  matched_to_features, match_weights );

     //  Make a dummy vector of intensity weights.
     //vcl_vector< double > dummy_intensity_weights( match_weights.size(), 1.0 );

     //  Add the feature and its matches and weights to the match set
     matches_sptr
       -> add_feature_matches_and_weights( *fitr, mapped_feature, matched_to_features, match_weights );
   }

  vcl_cout << " number of from points : " << matches_sptr->from_size() << vcl_endl;
  assert( matches_sptr->from_size() == from.size() );
  return matches_sptr;
}


template <class PixelType>
void
rgrl_matcher_pseudo_int_3d<PixelType> ::
map_region_intensities( rgrl_transformation      const& trans,
                        rgrl_feature_sptr               feature_sptr,
                        rgrl_mapped_pixel_vector_type & mapped_pixels) const
{
  if ( feature_sptr -> is_type( rgrl_feature_face_region::type_id() ) )
  {
    rgrl_feature_face_region * face_ptr =
      rgrl_cast<rgrl_feature_face_region *> ( feature_sptr );
    this -> map_region_intensities( face_ptr -> pixel_coordinates_ratio( from_spacing_ratio_.as_ref() ), trans,
            feature_sptr, mapped_pixels );
  }
  else
  {
    rgrl_feature_trace_region * trace_ptr =
    rgrl_cast<rgrl_feature_trace_region *> ( feature_sptr );
    this -> map_region_intensities( trace_ptr -> pixel_coordinates_ratio( from_spacing_ratio_.as_ref() ), trans,
            feature_sptr, mapped_pixels );
  }
}

// pixel_locations are neighboring pixels in "pixel coordinates".
template <class PixelType>
void
rgrl_matcher_pseudo_int_3d<PixelType> ::
map_region_intensities( vcl_vector< vnl_vector<int> > const& pixel_locations,
                        rgrl_transformation           const& trans,
                        rgrl_feature_sptr                    feature_sptr,
                        rgrl_mapped_pixel_vector_type      & mapped_pixels) const
{
  DebugMacro( 1, "   number of pixel coorindates: " << pixel_locations.size() << vcl_endl );
  // check
  if ( pixel_locations.empty() ) return;

  //unsigned dim = feature_sptr -> location() . size();
  assert ( feature_sptr -> location() . size() == 3 ); // so far vil3d force it to be 3D
  vnl_double_3 physical_loc;
  vnl_int_3    current_pixel_loc;
  //const unsigned int size = pixel_locations.size();

  // transform all the pixels and store their locations in a vector
  vbl_bounding_box<double,3> box;
  mapped_info   mapped_pt;
  mapped_pt.pixel_ = vnl_double_3( 0.0, 0.0, 0.0 );   // not used field
  vcl_vector< mapped_info >  direct_mapped_pts;
  direct_mapped_pts.reserve( pixel_locations.size() );
  for ( unsigned int i=0; i<pixel_locations.size(); ++i )
  {
    current_pixel_loc = pixel_locations[i];
    // Check if the location is inside the valid region
    if ( !pixel_in_range( from_image_, mask_, current_pixel_loc ) )
      continue;

    //  Copy the int pixel locations to doubles.  Yuck.
    pixel_to_physical( current_pixel_loc, physical_loc, from_spacing_ratio_ );

    // map the pixel, in the physical coordinates, and then convert
    // it to the pixel cooridinates.
    vnl_double_3 mapped_physical_pt;
    trans.map_location( physical_loc, mapped_physical_pt.as_ref().non_const() );
    // Check if the mapped location is inside the valid region
    if ( !physical_in_range( to_image_, mask_, mapped_physical_pt, to_spacing_ratio_ ) )
      continue;

    // store
    mapped_pt.physical_ = mapped_physical_pt;
    physical_to_pixel( mapped_physical_pt, mapped_pt.pixel_, to_spacing_ratio_ );
    // only use the first plane/channel
    mapped_pt.in_ = from_image_( current_pixel_loc[0], current_pixel_loc[1], current_pixel_loc[2], 0 );
    direct_mapped_pts.push_back( mapped_pt );

    // update bounding box
    box.update( mapped_pt.pixel_[0], mapped_pt.pixel_[1], mapped_pt.pixel_[2] );
  }

  // check for empty bounding box
  if ( box.empty() )   return;
  // the dimension of a smallest image that encasulate the bounding box
  vnl_int_3 origin, dim;
  origin[0] = (int)vcl_floor(box.xmin());
  origin[1] = (int)vcl_floor(box.ymin());
  origin[2] = (int)vcl_floor(box.zmin());
  dim[0] = 1+(int)vcl_ceil(box.xmax()) - (int)vcl_floor(box.xmin());
  dim[1] = 1+(int)vcl_ceil(box.ymax()) - (int)vcl_floor(box.ymin());
  dim[2] = 1+(int)vcl_ceil(box.zmax()) - (int)vcl_floor(box.zmin());

  DebugMacro( 1, "Origin: " << origin << " Dim: " << dim << vcl_endl; );
  // create a 3D image with this dim
  vbl_array_3d<double> wgted_sum( dim[0], dim[1], dim[2] ); // 1 plane
  vbl_array_3d<double> wgts( dim[0], dim[1], dim[2] ); // 1 plane
  wgted_sum.fill( 0.0 );
  wgts.fill( 0.0 );

  // Distribute each mapped point to their integer neighbors
  // The intensity of each integer point is a weighted sum of
  // closest points.
  // NOTE:
  // the weight could depend on either physical coordinate or
  // pixel coordinate.
  // The choice I made is to use pixel coordinate, because I want the weight
  // to have smooth transition between 1.0(coincide with the point) and 0.0
  // (one pixel away).
  for (typename vcl_vector<mapped_info>::const_iterator it=direct_mapped_pts.begin();
      it!=direct_mapped_pts.end(); ++it) {
    vnl_int_3 ceil, floor;
    vnl_double_3 diff_floor;
    for (unsigned i=0; i<3; i++) {
      floor[i] = (int)vcl_floor( it->pixel_[i] ) - origin[i];
      ceil [i] = (int)vcl_ceil ( it->pixel_[i] ) - origin[i];
      diff_floor[i] = it->pixel_[i] - origin[i] - floor[i];
    }

    double wgt;
    for (int i=floor[0]; i<=ceil[0]; i++)
      for (int j=floor[1]; j<=ceil[1]; j++)
        for (int k=floor[2]; k<=ceil[2]; k++) {
          // ceil(x) is not equivalent to floor(x)+1, try x=4,
          // or any integer pos
          wgt = vcl_abs( floor[0]+1-i-diff_floor(0) ) *
                vcl_abs( floor[1]+1-j-diff_floor(1) ) *
                vcl_abs( floor[2]+1-k-diff_floor(2) );
          wgts(i,j,k) += wgt;
          wgted_sum(i,j,k) += wgt* double(it->in_);
        }
  }

  // Count all pixels have positive weights
  mapped_pixels.reserve( pixel_locations.size() );
  rgrl_mapped_pixel_type  mapped_pixel;
  mapped_pixel . weight = 1.0;
  for (int i=0; i<dim[0]; ++i)
    for (int j=0; j<dim[1]; ++j)
      for (int k=0; k<dim[2]; ++k)
        if ( wgts(i,j,k) >= 1e-8 ) {  // artitrary threshold for numerical stability
          mapped_pixel.intensity = wgted_sum(i,j,k) / wgts(i,j,k);
          mapped_pixel.location[0] = i + origin[0];
          mapped_pixel.location[1] = j + origin[1];
          mapped_pixel.location[2] = k + origin[2];

          //DebugMacro(2, "mapped pixel loc: " << mapped_pixel.location << " intensity: " << mapped_pixel.intensity <<vcl_endl )
          mapped_pixels.push_back( mapped_pixel );
        }
  DebugMacro(1, "Total mapped pixels at integer locations: " << mapped_pixels.size() << vcl_endl );
}

inline
double
sub_pixel( vcl_vector< double > const& responses )
{
  assert( 0 ); // no need to use SVD
  assert( responses.size() == 3 );

  // let s be the similarity error, s = a r^2 + b r + c.
  // Use points index-1, index, index+1 to model the
  // parameters X = [a, b, c].
  vnl_matrix < double > A ( 3, 3 );
  vnl_matrix < double > S ( 3, 1 ) ;

  for ( unsigned i = 0; i < 3; ++i ) {
    // the middle point is at r = 0
    int r = i - 1;
    A( i, 0 ) = r * r;
    A( i, 1 ) = r;
    A( i, 2 ) = 1;
    S( i, 0 ) = responses[ i ];
  }

  vnl_svd< double > svd( A );
  vnl_matrix< double > inv = svd.inverse();
  vnl_matrix< double > X = inv * S;
  assert( X.columns() == 1 );

  // if it fit a line, instead of a parabola
  // then return the original best index
  if ( X[ 0 ][ 0 ] <= 1.0e-5 )
    return 0;

  // find r that minimizes s
  // ds = 2ar + b = 0
  // r = -b / 2a
  double best_index =  -X[ 1 ][ 0 ] / ( 2 * X[ 0 ][ 0 ] );

  DBG( vcl_cout << " best_index = " << best_index << '\n' ) ;

  assert( best_index <= 1 && best_index >= -1 );

  return best_index;
}

// slide the window in normal direction(s)
// to find the optimum response
template <class PixelType>
void
rgrl_matcher_pseudo_int_3d<PixelType> ::
slide_window(rgrl_feature_sptr         mapped_feature,
             rgrl_mapped_pixel_vector_type const & mapped_pixels,
             rgrl_scale                    const & current_scale,
             vcl_vector< rgrl_feature_sptr >     & matched_to_features,
             vcl_vector< double >                & match_weights ) const
{
  //  At this point, find the most similar region within the sliding window
  unsigned int dim = mapped_feature -> location().size();

  const double scale_multiplier = 4;   // magic number.  frown.

  DebugMacro(2, " geometric scale = " << current_scale.geometric_scale() << vcl_endl );

  vnl_matrix< double > normal_space;

  if ( mapped_feature -> is_type( rgrl_feature_face_region::type_id() ) )
  {
    rgrl_feature_face_region * face_ptr =
      rgrl_cast<rgrl_feature_face_region *> ( mapped_feature );
    normal_space.set_size( dim, 1 );
    normal_space.set_column ( 0, face_ptr -> normal() );
  }
  else // RGRL_TRACE_REGION
  {
    rgrl_feature_trace_region * trace_ptr =
    rgrl_cast<rgrl_feature_trace_region *> ( mapped_feature );
    normal_space = trace_ptr -> normal_subspace();
  }

  vnl_vector<double> match_location(3, 0.0);
  double min_response = 0.0;
  double second_derivative = 0.0;
  double max_length = scale_multiplier * current_scale.geometric_scale();
  if ( max_length < 1 ) max_length = 1;

  //  DO THE REST DEPENDING ON IF THE NORMAL SUBSPACE IS 1D or 2D.
  //  IN THE FUTURE, IF WE WANT TO JUMP TO N-D, THIS WILL NEED TO BE
  //  CHANGED, PERHAPS JUST BY ADDING EACH DIMENSION WE WANT.

  if ( normal_space . columns() == 1 )
  {
    vnl_double_3 physical_basis = normal_space.get_column(0);
    vnl_double_3 pixel_basis;
    physical_to_pixel( physical_basis, pixel_basis, to_spacing_ratio_ );

    // sample pixel(integer) locations
    vcl_vector< discrete_shift_node > discrete_offsets;
    // ASSUME known structure of the return offsets,
    // that is symmetric around origin
    // such as:  -4 -3 -2 -1 0 1 2 3 4
    sample_pixels_along_direction( discrete_offsets, pixel_basis, max_length );
    // This assertion should never fail,
    // unless geometric scale becomes too small
    assert( discrete_offsets.size() > 1 );
    DebugMacro(2, "  shift vector length: " << discrete_offsets.size() );

    // the shifts are symmetric around origin
    const int max_offset = (discrete_offsets.size()-1)/2;

    vcl_vector<double> responses( 2*max_offset+1, 0.0 );
    bool is_best_initialized = false;
    int best_offset = 0;

    // Don't favor the max_offset_range. sometimes the region is
    // homogeneous, the responses might have same value
    for ( int abs_offset = 0; abs_offset <= max_offset; ++abs_offset )
    {
      int offset = abs_offset;
      do {
        int i = offset + max_offset;
        responses[i] = this -> compute_response( mapped_pixels, discrete_offsets[i].shift_ );
        DebugMacro(2, " response at offset " << discrete_offsets[i].shift_
                   << " ( i = " << i << " ) : " << responses[ i ] << vcl_endl );

        // We don't want to use the responses of the offsets that shift
        // the box across the boundary.
        if ( (!is_best_initialized || responses[i] < min_response ) &&
             responses[ i ] != max_response_value )
        {
          is_best_initialized = true;
          min_response = responses[i];
          best_offset = offset;
        }
        offset = -offset;
      } while ( offset < 0 );
    }

    DebugMacro(2, " the best offset = " << discrete_offsets[best_offset+max_offset].shift_ << vcl_endl );
    if ( !is_best_initialized )
    {
      DebugMacro(1, "For mapped feature: " << mapped_feature->location()
                 << ", the slide window is invalid." << vcl_endl );
      return;
    }

    //  Evaluate the second derivative at the peak.  If the
    //  peak occurrence is on the boundary, compute the second
    //  derivative based on one step in from the boundary.
    //  Get back to the representation: origin is 0
    int deriv_loc = best_offset;
    if ( deriv_loc == -max_offset ) ++ deriv_loc;
    else if ( deriv_loc == max_offset ) -- deriv_loc;
    int index = deriv_loc + max_offset;
    DebugMacro(3, " the proper offset = " << deriv_loc << vcl_endl );

    // update the best matched location
    const vnl_int_3& best = discrete_offsets[best_offset+max_offset].shift_;
    match_location = mapped_feature->location();
    for (unsigned int i=0; i<3; i++)
      match_location[i] += double(best[i]);
    DebugMacro(2, "best match :\n" << match_location << vcl_endl );


    // Now compute the second derivative
    // Note that these descrete points are not evenly distributed.
    // Solution:
    // Use Taylor expansion on f(x0-d1), f(x0), and f(x0+d2)
    // It is easy to show that in order to get f''(x) = a1*f(x0-d1) + a2*f(x0) + a3*f(x0+d2)
    // a1 = 2/(d1*(d1+d2))
    // a2 = - 2/(d1*d2)
    // a3 = 2/(d2*(d1+d2))
    // If one neighbor's response is not valid, calculate the second
    // derivative value of the other neighbor
    if ( responses[ index - 1 ] == max_response_value )
      index ++;
    else if ( responses[ index + 1 ] == max_response_value )
      index--;

    double a1, a2, a3, d1, d2, sumd;
    //assert( responses[ index ] != max_response_value );
    if ( index > 0 && index+1 < (int)responses.size() &&
         responses[ index ] != max_response_value &&
         index + 1 <= 2*max_offset &&
         index - 1 >= -2*max_offset &&
         responses[ index + 1 ] != max_response_value &&
         responses[ index - 1 ] != max_response_value )
    {
      d2 = discrete_offsets[ index+1 ].step_ - discrete_offsets[ index ].step_;
      d1 = discrete_offsets[ index ].step_ - discrete_offsets[ index-1 ].step_;
      sumd = d1+d2;
      a1 = 2.0/(d1*sumd);
      a3 = 2.0/(d2*sumd);
      a2 = -2.0/(d1*d2);
      // take abs value, for it can be shifted for the boundary response,
      // or invalid points
      second_derivative = vcl_abs( a1*responses[ index-1 ] +
                                   a2*responses[ index ] +
                                   a3*responses[ index + 1] );
      DebugMacro(3, "  2nd Derivative(at " << index
                 << "): d1=" << d1 << " d2=" << d2
                 << "\n       a1=" << a1 <<" a2=" << a2
                 << " a3=" << a3
                 << "\n        res1 " << responses[ index-1 ]
                 << "  res2 " << responses[ index ]
                 << "  res3 " << responses[ index+1 ]
                 << "\n        deriv=" << second_derivative << vcl_endl ) ;
    }
    else
    {
      second_derivative = 0;
      DebugMacro(2, "index=" << index << ", max_offset=" << max_offset
                 << ", responses[index-1]=" << responses[index-1]
                 << ", responses[index+1]=" << responses[index+1] << '\n'
                 << "   neighbors' responses are not valid. Set the second_derivative = 0\n" );
    }
  }
  else if ( normal_space . columns() == 2 )
  {
    //int max_offset = int(max_length);
    vnl_vector<double> basis1 = normal_space . get_column(0);
    vnl_vector<double> basis2 = normal_space . get_column(1);

    DebugMacro(2, "normal basis :\n" << basis1 << " and " << basis2 << vcl_endl );

    // sample pixels along basis directions.
    vcl_vector< discrete_shift_node > offset1, offset2;
    // NOTE: the returned shift vector is symmetric around origin
    // for details, look at the face session(above)
    sample_pixels_along_direction( offset1, basis1, max_length );
    sample_pixels_along_direction( offset2, basis2, max_length );
    const int max_offset1 = (offset1.size()-1) / 2;
    const int max_offset2 = (offset2.size()-1) / 2;
    //vcl_vector<double> temp( 2*max_offset+1, 0.0 );
    //vcl_vector< vcl_vector<double> > responses( 2*max_offset+1, temp );
    vbl_array_2d<double> responses( 2*max_offset1+1, 2*max_offset2+1, 0.0 );
    bool is_best_initialized = false;
    int best_off1 = 0, best_off2 = 0;

    //  Find the offset along the basis direction giving the best
    //  response.

    for ( int off1 = -max_offset1, i=0; off1 <= max_offset1; ++off1, ++i )
    {
      for ( int off2 = -max_offset2, j=0; off2 <= max_offset2; ++off2, ++j )
      {
        responses(i,j) = this -> compute_response( mapped_pixels, offset1[i].shift_ + offset2[j].shift_ );

        if ( ( !is_best_initialized || responses(i,j) < min_response )
             && responses(i,j) != max_response_value )
        {
                is_best_initialized = true;
                min_response = responses(i,j);
                best_off1 = off1;
                best_off2 = off2;
        }
      }
    }
    if ( !is_best_initialized )
    {
      DebugMacro(1, "For mapped feature: " << mapped_feature->location()
                 << ", the slide window is invalid." << vcl_endl );
      return;
    }

    const vnl_int_3& best1 = offset1[best_off1+max_offset1].shift_;
    const vnl_int_3& best2 = offset2[best_off2+max_offset2].shift_;
    match_location = mapped_feature->location();
    for (unsigned int i=0; i<3; i++)
      match_location[i] += double(best1[i]) + double(best2[i]);

    // TODO
    // compute the second drivative
    assert( 0 );
#if 0 // commented out
    //  Evaluate the second derivative at the peak.  If the
    //  peak occurrence is on the boundary, compute the second
    //  derivative based on one step in from the boundary.

    int idx1 = 0, idx2 = 0;   // indices into the array of responses
    int deriv_loc1 = best_off1;
    if ( deriv_loc1 == -max_offset ) ++deriv_loc1;
    else if ( deriv_loc1 == max_offset ) --deriv_loc1;
    idx1 = deriv_loc1 + max_offset;
    idx2 = best_off2 + max_offset;

    // The best_offset so far is constrained on the discrete space.
    // Now we use a parabola to model the similarity error
    // (responses) and find the position of the minimum response.
    // Here I calculate sub_pixel in each dimension seperately just for
    // the convenience. Since it's only an approximation in one grid,
    // I assume this approximation is good enough.
    double sub_offset1;

    if ( best_off1 == max_offset || best_off1 == -max_offset )
  sub_offset1 = best_off1;
    else if ( responses[ idx1 - 1 ][ idx2 ] == max_response_value ||
        responses[ idx1 + 1 ][ idx2 ] == max_response_value )
    {
      sub_offset1 = idx1 - max_offset;
    }
    else
    {
      vcl_vector< double > responses_for_sub_pixel( 3 );
      responses_for_sub_pixel[ 0 ] = responses[ idx1 - 1 ][ idx2 ];
      responses_for_sub_pixel[ 1 ] = responses[ idx1 ][ idx2 ];
      responses_for_sub_pixel[ 2 ] = responses[ idx1 + 1 ][ idx2 ];
      sub_offset1 = sub_pixel( responses_for_sub_pixel ) + idx1 - max_offset;
      // the sub_pixel here is used only for interpolation
      // if it's outside
      if ( sub_offset1 < -max_offset ) sub_offset1 = -max_offset;
      if ( sub_offset1 > max_offset ) sub_offset1 = max_offset;
      DBG( vcl_cout << " sub_offset1 = " << sub_offset1 << " in [ "
                    << -max_offset << " , " << max_offset << " ] " << vcl_endl );
    }

    double second_d1 = vnl_math_abs( responses[ idx1-1 ][ idx2 ] + responses[ idx1+1 ][ idx2 ]
                                     - 2 * responses[ idx1 ][ idx2 ] );

    int deriv_loc2 = best_off2;
    if ( deriv_loc2 == -max_offset ) ++deriv_loc2;
    else if ( deriv_loc2 == max_offset ) --deriv_loc2;
    idx2 = deriv_loc2 + max_offset;
    idx1 = best_off1 + max_offset;
    double sub_offset2;
    if ( best_off2 == max_offset || best_off2 == -max_offset )
  sub_offset2 = best_off2;
    else if ( responses[ idx1 ][ idx2 - 1 ] == max_response_value ||
        responses[ idx1 ][ idx2 + 1 ] == max_response_value )
    {
      sub_offset2 = idx2 - max_offset;
    }
    else
    {
      vcl_vector< double > responses_for_sub_pixel( 3 );
      responses_for_sub_pixel[ 0 ] = responses[ idx1 ][ idx2 - 1 ];
      responses_for_sub_pixel[ 1 ] = responses[ idx1 ][ idx2 ];
      responses_for_sub_pixel[ 2 ] = responses[ idx1 ][ idx2 + 1 ];
      sub_offset2 = sub_pixel( responses_for_sub_pixel ) + idx2 - max_offset;
      if ( sub_offset2 < -max_offset ) sub_offset2 = -max_offset;
      if ( sub_offset2 > max_offset ) sub_offset2 = max_offset;
      DBG( vcl_cout << " sub_offset2 = " << sub_offset2 << " in [ "
                    << -max_offset << " , " << max_offset << " ] " << vcl_endl; );
    }

    double second_d2 = vnl_math_abs( responses[ idx1 ][ idx2-1 ] + responses[ idx1 ][ idx2+1 ]
                                     - 2 * responses[ idx1 ][ idx2 ] );

    second_derivative = vnl_math_min( second_d1, second_d2 );
    match_location = mapped_location + basis1 * sub_offset1 + basis2 * sub_offset2;
    DBG( vcl_cout << "best match :\n" << match_location << vcl_endl );
#endif // 0
  } else {
    vcl_cerr << "Code doesn't handle a normal subspace of greater than two dimenions.\n";
    assert( false );
  }
  matched_to_features . clear();
  match_weights . clear();
  rgrl_feature_sptr mf_ptr;
  if ( mapped_feature -> is_type( rgrl_feature_face_region::type_id() ) )
  {
    rgrl_feature_face_region * face_ptr =
    rgrl_cast<rgrl_feature_face_region *> ( mapped_feature );
    mf_ptr = new rgrl_feature_face_region( match_location, face_ptr -> normal() );
  } else
  {
    rgrl_feature_trace_region * trace_ptr =
    rgrl_cast<rgrl_feature_trace_region *> ( mapped_feature );
    mf_ptr = new rgrl_feature_trace_region( match_location, trace_ptr -> tangent() );
  }


  matched_to_features . push_back( mf_ptr );
  double weight = second_derivative / (1.0 + min_response);
  assert( weight >= 0.0 );

  DebugMacro(2, "second derivative: " << second_derivative
             << "\nmin_response: " << min_response << "\nweight : " << weight << vcl_endl );
  match_weights.push_back( weight );
}

template <class PixelType>
double
rgrl_matcher_pseudo_int_3d<PixelType> ::
compute_response( rgrl_mapped_pixel_vector_type const& mapped_pixels,
                  vnl_int_3                  const& shift ) const
{
  const unsigned size = mapped_pixels.size();

  //  Extract the intensities at the mapped locations.  Make sure
  //  they are inside the image.

  vcl_vector< double > a;
  vcl_vector< double > b;
  vcl_vector< double > weights;
  double intensity;
  vnl_int_3 loc;

  // reserve space
  a.reserve( size );
  b.reserve( size );
  weights.reserve( size );

  for ( unsigned i = 0; i < size; ++i )
  {
    loc = mapped_pixels[i].location + shift;
    // Check if the location is inside the valid region,
    // if not, we don't use the response of this shift
    if ( !pixel_in_range( to_image_, mask_, loc ) ) {
      DebugMacro(2, "out of range: " << loc << " ( shift: " << shift << " )\n" );
      return max_response_value;
    }

    // from mapped_pixels
    a.push_back( (double)(mapped_pixels[i].intensity) );
    // intensity on "to"(fixed) image
    intensity = to_image_( loc[0], loc[1], loc[2] );
    b.push_back( intensity );

    weights.push_back( mapped_pixels[i].weight );
  }

  //  call the response function
  double val = evaluator_->evaluate( a, b, weights );

  return val;
}

// ASSUME symmetric around origin
// such as:  -4 -3 -2 -1 0 1 2 3 4
template<class PixelType>
void
rgrl_matcher_pseudo_int_3d<PixelType>::
sample_pixels_along_direction( vcl_vector<discrete_shift_node>& two_dir_shifts,
                              vnl_double_3 dir,
                              double max_length ) const
{
  // make sure any element in normal vector/basis is less than 1.
  // Thus, divided by 2*mag, and double max_length;
  // I try to avoid directions like [1, 0, 0], in which 1 gives arise to problems.
  // The situation is different for 1 or 0.9999...
  // Therefore, I would rather have [0.5, 0, 0]
  dir /= 2.0 * dir.magnitude();
  max_length *= 2.0;

  DebugMacro(2, "normal basis :\n" << dir << vcl_endl );

  // the idea is to find the smallest delta length added to the current one,
  // in order to get to the nearest pixel location
  vnl_int_3 prev, current;
  double len;
  double abs_ele;
  vcl_vector<discrete_shift_node> locs;
  locs.reserve( int(2*max_length) );

  // init
  const double epsilon = 1.0/(100*max_length);  // 100 is arbitrary
  const double min_step_size = 1.0/(2*max_length);
  prev=vnl_int_3(0, 0, 0);
  locs.push_back( discrete_shift_node(prev, 0) );
  len = 1e-10; // has to be larger than zero, otherwise, vcl_ceil(0) = 0
  while ( len < max_length )
  {
    double delta_len, min_delta_len = 1e10;
    int min_index = 0;
    for (unsigned i=0; i<3; i++) {
      abs_ele = vcl_abs( dir[i] );
      // don't want to divide by too small a number
      if ( abs_ele < min_step_size )
        continue;

      // find the smallest step to next integer location
      delta_len = vcl_ceil( len * abs_ele ) / abs_ele - len;
      if ( delta_len < min_delta_len && delta_len > 0 ) //prevent delta_len==0
      {
        min_index = i;
        min_delta_len = delta_len;
      }
    }

    assert( min_delta_len > 0.0 );
    // keep it within max_length
    if ( len+min_delta_len > max_length )
      break;

    // find out the pixel location
    current = prev;
    if ( dir[min_index] < 0 )
      current[min_index] -= 1;
    else
      current[min_index] += 1;

    // store the length, instead of delta_len
    // It is required, as it is different case(left neighbor or right neighbor)
    // for positive leng and negative len
    locs.push_back( discrete_shift_node( current, (len+min_delta_len)/2 ) );
    prev = current;
    // update length
    // to prevent numerical rounding, e.g., 1.999999 instead of 2
    // add a small padding to make it larger
    len += min_delta_len+epsilon;
  }

  // copy
  two_dir_shifts.clear();
  two_dir_shifts.reserve( locs.size()*2-1 );
  // negative direction
  for (int i=locs.size()-1; i>0; i--)
    two_dir_shifts.push_back( - locs[i] );

  // positive direction, starting at origin
  for (unsigned int i=0; i<locs.size(); i++)
    two_dir_shifts.push_back( locs[i] );
}

#endif // rgrl_matcher_pseudo_int_3d_txx_
