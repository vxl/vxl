#ifndef rgrl_matcher_pseudo_3d_txx_
#define rgrl_matcher_pseudo_3d_txx_

#include "rgrl_matcher_pseudo_3d.h"
#include <rgrl/rgrl_feature_face_region.h>
#include <rgrl/rgrl_feature_trace_region.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_match_set.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_int_3.h>
#include <vnl/algo/vnl_svd.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vcl_cassert.h>

static const double max_response_value = 1.0e30;

#define DEBUG
#if defined ( DEBUG )
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
                   vnl_double_3 const&  spacing_ratio )
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

  if ( mask ) {
    // So far, 3D CT images can use one 2D mask image for each slices.
    static vnl_vector< double > loc_dbl( 2 );
    for ( unsigned i = 0; i < 2; ++i )
      loc_dbl[ i ] = double(location[ i ]);

//    vcl_cout << "mask pixel loc: " << loc_dbl << '\n';
    if (  !mask->inside( loc_dbl ) )
      return false;
  }

  // Even though we have checked mask, the location might still be
  // invalid in image range. Because the mask truncates the double
  // to int. For example, if image range is [0, 1023] the
  // location is at 1023.5, the mask truncates it to 1023. But
  // 1023.5 is not valid for interpolation.

  if ( location[ 0 ] < 0 || location[ 0 ] > (int)image.ni()-1 ||
       location[ 1 ] < 0 || location[ 1 ] > (int)image.nj()-1 ||
       location[ 2 ] < 0 || location[ 2 ] > (int)image.nk()-1 )
    return false;
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
  vnl_int_3 pixel_loc;
  physical_to_pixel( location, pixel_loc, spacing_ratio );
  return pixel_in_range( image, mask, pixel_loc );
}

template <class PixelType>
rgrl_matcher_pseudo_3d< PixelType > ::
rgrl_matcher_pseudo_3d( vil3d_image_view<PixelType> const& from_image,
                        vil3d_image_view<PixelType> const& to_image,
                        vnl_vector< double > const& from_spacing_ratio,
                        vnl_vector< double > const& to_spacing_ratio,
                        rgrl_evaluator_sptr      evaluator,
                        rgrl_mask_sptr mask )
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
rgrl_matcher_pseudo_3d< PixelType > ::
compute_matches( rgrl_feature_set const&    from_set,
                 rgrl_feature_set const&    to_set,
                 rgrl_view const&           current_view,
                 rgrl_transformation const& current_xform,
                 rgrl_scale const&          current_scale ) const
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
  DBG( unsigned a=0; );
  for ( f_iterator_type fitr = from.begin(); fitr != from.end(); ++fitr )
  {
    DBG(
      vcl_cout << "***feature " << a << '\n';
      ++a;
      );
    // Match by searching in the tangent space of the
    // transformed from image feature.  The match_weights are to be
    // treated later as similarity weights
    matched_to_features.clear();
    match_weights.clear();

    // Map the feature location using the current transformation
    rgrl_feature_sptr mapped_feature = (*fitr)->transform( current_xform );

    // if the location is not inside the valid region
    // set the weight = 0
    if ( !physical_in_range( to_image_, mask_, mapped_feature->location(), to_spacing_ratio_ ) )
    {
      //  Make a dummy vector of intensity weights.
      // vcl_vector< double > dummy_intensity_weights( 0 ); //CT: not needed now
      vcl_vector< double > match_weights( 0 );

      //  Add the feature and its matches and weights to the match set
      matches_sptr
        -> add_feature_matches_and_weights( *fitr, mapped_feature, matched_to_features,
                                            match_weights );
      DBG( vcl_cout << " skip match from: " << (*fitr)->location() << ", to: " << mapped_feature->location() << '\n' );
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
      vcl_vector< double > match_weights( 0 );

      //  Add the feature and its matches and weights to the match set
      matches_sptr
        -> add_feature_matches_and_weights( *fitr, mapped_feature, matched_to_features,
                                            match_weights );
      vcl_cout << " from point : " << (*fitr)->location()
               << " to point : " << mapped_feature->location()
               << " doesn't have proper matches\n" << vcl_endl;
      continue;
    }

    this -> match_mapped_region( mapped_feature, mapped_pixels, current_scale,
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
rgrl_matcher_pseudo_3d<PixelType> ::
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
rgrl_matcher_pseudo_3d<PixelType> ::
map_region_intensities( vcl_vector< vnl_vector<int> > const& pixel_locations,
                        rgrl_transformation           const& trans,
                        rgrl_feature_sptr                    feature_sptr,
                        rgrl_mapped_pixel_vector_type      & mapped_pixels) const
{
  DBG( vcl_cout << "   number of pixel coorindates: " << pixel_locations.size() << vcl_endl );
  // check # of pixels
  if ( pixel_locations.empty() )  return;

  unsigned dim = feature_sptr -> location() . size();
  assert ( dim == 3 ); // so far vil3d force it to be 3D
  vnl_double_3 physical_loc;
  vnl_int_3    current_pixel_loc;
  rgrl_mapped_pixel_type  mapped_pixel;
  mapped_pixel . weight = 1.0;

  double intensity;
  // reserve space
  mapped_pixels.reserve( pixel_locations.size() );
  for ( unsigned int i=0; i<pixel_locations.size(); ++i )
  {
    current_pixel_loc = pixel_locations[i];
    // Check if the location is inside the valid region
    if ( !pixel_in_range( from_image_, mask_, current_pixel_loc ) )
      continue;

//  //  Copy the int pixel locations to doubles.  Yuck.
//  for ( unsigned j=0; j<dim; ++j )  physical_loc_dbl[j] = pixel_locations[i][j];
    pixel_to_physical( current_pixel_loc, physical_loc, from_spacing_ratio_ );

    // map the pixel, in the physical coordinates, and then convert
    // it to the pixel cooridinates.
    vnl_double_3 mapped_pt;
    trans.map_location( physical_loc, mapped_pt.as_ref().non_const() );
    // Check if the mapped location is inside the valid region
    if ( !physical_in_range( to_image_, mask_, mapped_pt, to_spacing_ratio_ ) )
      continue;

    physical_to_pixel( mapped_pt, mapped_pixel.location, to_spacing_ratio_ );
    //  Extract the intensity.  This is where we need ITK.
    // only work for one plane so far

    // only use the first plane/channel
    intensity = from_image_( current_pixel_loc[0], current_pixel_loc[1], current_pixel_loc[2], 0 );
    //PixelType intensity; //  =  SOMETHING from ITK
    mapped_pixel . intensity = trans . map_intensity( physical_loc.as_ref(), intensity );
    mapped_pixels . push_back( mapped_pixel );
  }
}

inline
double
sub_pixel( vcl_vector< double > const& responses )
{
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

template <class PixelType>
void
rgrl_matcher_pseudo_3d<PixelType> ::
match_mapped_region( rgrl_feature_sptr         mapped_feature,
         rgrl_mapped_pixel_vector_type const & mapped_pixels,
         rgrl_scale                    const & current_scale,
         vcl_vector< rgrl_feature_sptr >     & matched_to_features,
         vcl_vector< double >                & match_weights ) const
{
  //  At this point, find the most similar feature within the given
  //  scale.
  unsigned int dim = mapped_feature -> location().size();

  const double scale_multiplier = 4;   // magic number.  frown.

  DBG( vcl_cout << " geometric scale = " << current_scale.geometric_scale() << vcl_endl );

  vnl_matrix< double > normal_space;

  if ( mapped_feature -> is_type( rgrl_feature_face_region::type_id() ) )
  {
    rgrl_feature_face_region * face_ptr =
      rgrl_cast<rgrl_feature_face_region *> ( mapped_feature );
    normal_space = vnl_matrix< double > ( dim, 1 );
    normal_space . set_column ( 0, face_ptr -> normal() );
  }
  else // RGRL_TRACE_REGION
  {
    rgrl_feature_trace_region * trace_ptr =
    rgrl_cast<rgrl_feature_trace_region *> ( mapped_feature );
    normal_space = trace_ptr -> normal_subspace();
  }

  vnl_vector<double> match_location;
  double min_response = 0.0;
  double second_derivative = 0.0;
  int max_offset = vnl_math_rnd( scale_multiplier * current_scale.geometric_scale() );
  if ( max_offset == 0 ) max_offset = 1;

  //  DO THE REST DEPENDING ON IF THE NORMAL SUBSPACE IS 1D or 2D.
  //  IN THE FUTURE, IF WE WANT TO JUMP TO N-D, THIS WILL NEED TO BE
  //  CHANGED, PERHAPS JUST BY ADDING EACH DIMENSION WE WANT.

  if ( normal_space . columns() == 1 )
  {
    vnl_vector<double> basis = normal_space.get_column(0);

    DBG( vcl_cout << "normal basis :\n" << basis << vcl_endl );

    vcl_vector<double> responses( 2*max_offset+1, 0.0 );
    bool is_best_initialized = false;
    int best_offset = 0;

    //  Find the offset along the basis direction giving the best
    //  response.

    vnl_vector<double> mapped_location = mapped_feature -> location();

    // Don't favor the max_offset_range. sometimes the region is
    // homogeneous, the responses might have same value
    for ( int abs_offset = 0; abs_offset <= max_offset; ++abs_offset )
    {
      int offset = abs_offset;
      do {
        int i = offset + max_offset;
        responses[i] = this -> compute_response( mapped_location, mapped_pixels, basis * offset );
        DBG( vcl_cout << " response at offset " << offset
                      << " ( i = " << i << " ) : " << responses[ i ] << vcl_endl
        );

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

    DBG( vcl_cout << " the best offset = " << best_offset << vcl_endl );
    assert( is_best_initialized );

    //  Evaluate the second derivative at the peak.  If the
    //  peak occurrence is on the boundary, compute the second
    //  derivative based on one step in from the boundary.
    int deriv_loc = best_offset;
    if ( deriv_loc == -max_offset ) ++ deriv_loc;
    else if ( deriv_loc == max_offset ) -- deriv_loc;
    int index = deriv_loc + max_offset;
    DBG( vcl_cout << " the proper offset = " << deriv_loc << vcl_endl );

    // The best_offset so far is constrained on the discrete space.
    // Now we use a parabola to model the similarity error
    // (responses) and find the position of the minimum response.

    // If the best offset is at the (+/-)max_offset, no need to
    // calculate the sub_offset.
    double sub_offset = 0;
    if ( best_offset != max_offset &&
        best_offset != -max_offset )
      {
        // If one neighbor's response is not valid, calculate the second
        // derivative value of the other neighbor
        if ( responses[ index - 1 ] == max_response_value )
          index ++;
        else if ( responses[ index + 1 ] == max_response_value )
          index--;
        else
        {
          vcl_vector< double > responses_for_sub_pixel( 3 );
          responses_for_sub_pixel[ 0 ] = responses[ index - 1 ];
          responses_for_sub_pixel[ 1 ] = responses[ index ];
          responses_for_sub_pixel[ 2 ] = responses[ index + 1 ];
          sub_offset = sub_pixel( responses_for_sub_pixel );
          assert( sub_offset + best_offset >= -max_offset );
          assert( sub_offset + best_offset <= max_offset );
//            if ( sub_offset + best_offset < -max_offset ) best_offset = -max_offset;
//            if ( sub_offset + best_offset > max_offset ) best_offset = max_offset;
        }
      }

    match_location = mapped_location + basis * ( best_offset + sub_offset );

    // here I don't calculate the second derivative at sub_pixel,
    // but the second derivative at index to approximate it.
    //
    DBG( vcl_cout << "best match :\n" << match_location << vcl_endl );

    // assert( responses[ index ] != max_response_value );

    if ( index >0 && index+1 < responses.size() &&
         responses[ index ] != max_response_value &&
         index + 1 <= 2*max_offset &&
         index - 1 >= -2*max_offset &&
         responses[ index + 1 ] != max_response_value &&
         responses[ index - 1 ] != max_response_value )
      second_derivative = vnl_math_abs( responses[ index-1 ] + responses[ index+1 ]
                                        - 2 * responses[ index ] ); // should be positive
    // If one neighbor's response is not valid, calculate the second
    // derivative value of the other neighbor
    else {
      second_derivative = 0;
      DBG( vcl_cout << "index=" << index << ", max_offset="
                    << max_offset << ", responses[index-1]=" << responses[index-1]
                    << ", responses[index+1]=" << responses[index+1] << '\n'
                    << "   neighbors' responses are not valid. Set the second_derivative = 0\n" );
    }
  }

  else if ( normal_space . columns() == 2 )
  {
    vnl_vector<double> basis1 = normal_space . get_column(0);
    vnl_vector<double> basis2 = normal_space . get_column(1);

    DBG( vcl_cout << "normal basis :\n" << basis1 << " and " << basis2 << vcl_endl );
    vcl_vector<double> temp( 2*max_offset+1, 0.0 );
    vcl_vector< vcl_vector<double> > responses( 2*max_offset+1, temp );

    bool is_best_initialized = false;
    int best_off1 = 0, best_off2 = 0;

    //  Find the offset along the basis direction giving the best
    //  response.

    vnl_vector<double> mapped_location = mapped_feature -> location();
    for ( int off1 = -max_offset, i=0; off1 <= max_offset; ++off1, ++i )
      for ( int off2 = -max_offset, j=0; off2 <= max_offset; ++off2, ++j )
      {
        responses[i][j] = this -> compute_response( mapped_location, mapped_pixels,
          basis1 * off1 + basis2 * off2 );

        if ( ( !is_best_initialized || responses[i][j] < min_response )
             && responses[i][j] != max_response_value )
        {
          is_best_initialized = true;
          min_response = responses[i][j];
          best_off1 = off1;
          best_off2 = off2;
        }
      }

    assert( is_best_initialized );

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
                    << -max_offset << " , " << max_offset << " ]" << vcl_endl );
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
                    << -max_offset << " , " << max_offset << " ]" << vcl_endl; );
    }

    double second_d2 = vnl_math_abs( responses[ idx1 ][ idx2-1 ] + responses[ idx1 ][ idx2+1 ]
                                     - 2 * responses[ idx1 ][ idx2 ] );

    second_derivative = vnl_math_min( second_d1, second_d2 );
    match_location = mapped_location + basis1 * sub_offset1 + basis2 * sub_offset2;
    DBG( vcl_cout << "best match :\n" << match_location << vcl_endl );
  }
  else
  {
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
  }
  else
  {
    rgrl_feature_trace_region * trace_ptr =
    rgrl_cast<rgrl_feature_trace_region *> ( mapped_feature );
    mf_ptr = new rgrl_feature_trace_region( match_location, trace_ptr -> tangent() );
  }
  matched_to_features . push_back( mf_ptr );
  double weight = second_derivative / (1.0 + min_response);

  DBG( vcl_cout << "second derivative: " << second_derivative
                << "\nmin_response: " << min_response << "\nweight : " << weight << vcl_endl );
  match_weights.push_back( weight );
}

template <class PixelType>
double
rgrl_matcher_pseudo_3d<PixelType> ::
compute_response( vnl_double_3                  const& mapped_location,
                  rgrl_mapped_pixel_vector_type const& mapped_pixels,
                  vnl_double_3                  const& shift ) const
{
  const unsigned size = mapped_pixels.size();

  //  Extract the intensities at the mapped locations.  Make sure
  //  they are inside the image.

  vcl_vector< double > a;
  vcl_vector< double > b;
  vcl_vector< double > weights;
  double intensity;
  vnl_double_3 mapped_physical, loc, loc_in_double_pixel;

  // reserve space
  a.reserve( size );
  b.reserve( size );
  weights.reserve( size );

  for ( unsigned i = 0; i < size; ++i )
  {
    pixel_to_physical( mapped_pixels[i].location, mapped_physical, to_spacing_ratio_ );
    loc = mapped_physical + shift;
    // Check if the location is inside the valid region,
    // if not, we don't use the response of this shift
    if ( !physical_in_range( to_image_, mask_, loc, to_spacing_ratio_ ) ) {
      DBG( vnl_double_3 tmp;
           physical_to_pixel( loc, tmp, to_spacing_ratio_ );
           vcl_cout << "out of range: " << tmp << " ( " << loc << " )\n" );
      return max_response_value;
    }

    physical_to_pixel( loc, loc_in_double_pixel, to_spacing_ratio_ );
    intensity = vil3d_trilin_interp_safe( loc_in_double_pixel[0],
                                          loc_in_double_pixel[1],
                                          loc_in_double_pixel[2],
                                          to_image_.origin_ptr(),
                                          to_image_.ni(),
                                          to_image_.nj(),
                                          to_image_.nk(),
                                          to_image_.istep(),
                                          to_image_.jstep(),
                                          to_image_.kstep() );

    a.push_back( (double)(mapped_pixels[i].intensity) );
    b.push_back( intensity );

    weights.push_back( mapped_pixels[i].weight );
  }

  //  call the response function
  double val = evaluator_->evaluate( a, b, weights );

  return val;
}

#endif // rgrl_matcher_pseudo_3d_txx_
