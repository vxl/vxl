#ifndef rgrl_matcher_pseudo_txx_
#define rgrl_matcher_pseudo_txx_

#include "rgrl_matcher_pseudo.h"
#include <rgrl/rgrl_feature_face_region.h>
#include <rgrl/rgrl_feature_trace_region.h>
#include <rgrl/rgrl_feature_point_region.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_macros.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <vil/vil_bilin_interp.h>
#include <vcl_cassert.h>

static const unsigned int verbose_ = 2;
static const double max_response_value = 1.0e30;


template <class PixelType>
inline
bool
in_range( vil_image_view< PixelType > const& image,
          rgrl_mask_sptr const& mask,
          vnl_double_2 const& location )
{
  if ( mask && !mask->inside( location.as_ref() ) )
      return false;

  if ( location[ 0 ] < 0 || location[ 0 ] > image.ni()-1 ||
       location[ 1 ] < 0 || location[ 1 ] > image.nj()-1 )
    return false;

  return true;
}

template <class PixelType>
inline
bool
in_range( vil_image_view< PixelType > const& image,
          rgrl_mask_sptr const& mask,
          vnl_vector< double > const& location )
{
  if ( mask && !mask->inside( location ) )
      return false;

  return true;
}

template <class PixelType>
rgrl_matcher_pseudo< PixelType > ::
rgrl_matcher_pseudo( vil_image_view<PixelType> from_image,
         vil_image_view<PixelType> to_image,
         rgrl_evaluator_sptr      evaluator,
         rgrl_mask_sptr from_mask,
         rgrl_mask_sptr to_mask )
  : from_image_( from_image ),
    to_image_( to_image ),
    from_mask_( from_mask ),
    to_mask_ ( to_mask ),
    evaluator_( evaluator )
{}


template <class PixelType>
rgrl_match_set_sptr
rgrl_matcher_pseudo< PixelType > ::
compute_matches( rgrl_feature_set const&    from_set,
     rgrl_feature_set const&    to_set,
     rgrl_view const&           current_view,
     rgrl_transformation const& current_xform,
     rgrl_scale const&          current_scale )
{
  typedef vcl_vector<rgrl_feature_sptr> f_vector_type;
  typedef f_vector_type::iterator f_iterator_type;

  //  Build an empty match set
  rgrl_match_set_sptr matches_sptr = new rgrl_match_set( from_set.type(), to_set.type() );

  //  Get the from image features in the current view
  f_vector_type from = from_set.features_in_region( current_view.region() );
  DebugMacro (1,  " compute_matches : from.size() = " << from.size() << '\n' );
  DebugMacro_abv(1,"geometric scale = " << current_scale.geometric_scale()<< '\n' );

  //  Vector for mapped pixels
  rgrl_mapped_pixel_vector_type  mapped_pixels;
  mapped_pixels.reserve( from.size() );

  //  Vectors for matched features and weights.
  f_vector_type matched_to_features;
  vcl_vector<double> match_weights;

  // Match each feature...
  for ( f_iterator_type fitr = from.begin(); fitr != from.end(); ++fitr )
  {
    // Match by searching in the tangent space of the
    // transformed from image feature.  The match_weights are to be
    // treated later as similarity weights
    matched_to_features.clear();
    match_weights.clear();

    // for debug purpose
    // double x = (*fitr)->location()[0];
    // double y = (*fitr)->location()[1];

    // Map the feature location using the current transformation
    rgrl_feature_sptr mapped_feature = (*fitr)->transform( current_xform );

    { // Begin debugging
      if ( (*fitr)->is_type( rgrl_feature_trace_region::type_id() ) )
        DebugMacro_abv(1, "\n\nfrom :\n" << (*fitr)->location()
                       << " normal: "
                       << rgrl_cast<rgrl_feature_trace_region *> ( *fitr )->normal_subspace().get_column(0)
                       << "\nmapped :\n" << mapped_feature->location()
                       << rgrl_cast<rgrl_feature_trace_region *> ( mapped_feature )->normal_subspace().get_column(0) <<'\n' );
      else if ( (*fitr)->is_type( rgrl_feature_face_region::type_id() ) )
        DebugMacro_abv(1, "\n\nfrom :\n" << (*fitr)->location()
                       << " normal: "
                       << rgrl_cast<rgrl_feature_face_region *> ( *fitr )->normal()
                       << "\nmapped :\n" << mapped_feature->location()
                       << " normal: "
                       << rgrl_cast<rgrl_feature_face_region *> ( mapped_feature )->normal()<<'\n');
      else if ( (*fitr)->is_type( rgrl_feature_point_region::type_id() ) )
        DebugMacro_abv(1, "\n\nfrom :\n" << (*fitr)->location()
                       << "\nmapped :\n" << mapped_feature->location()<<'\n');
    } // End debugging

    // Check if the mapped feature is inside the valid region.
    // If the location is not inside the valid region
    // set the weight = 0
    if ( !in_range( to_image_, to_mask_, mapped_feature->location() ) ) {
      //  Make a dummy vector of intensity weights.
      // vcl_vector< double > dummy_intensity_weights( 0 ) //CT: not needed now;
      vcl_vector< double > match_weights( 0 );

      //  Add the feature and its matches and weights to the match set
      matches_sptr
        -> add_feature_matches_and_weights( *fitr, mapped_feature, matched_to_features,
                                            match_weights );

     DebugMacro_abv(1, " skip match from: " << (*fitr)->location()
                    << ", to: " << mapped_feature->location() << '\n' );

      continue;
    }

    // Map the intensities of the pixels in the from image
    // surrounding the from image feature.  Form a vector of pairs,
    // with each pair containing a mapped location and the
    // associated intensity.
    mapped_pixels.clear();

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
      DebugMacro(3, " from point : " << (*fitr)->location()
                   << " to point : " << mapped_feature->location() << " doesn't have proper matches\n");
      continue;
    }

    this -> match_mapped_region( mapped_feature, mapped_pixels, current_scale,
         matched_to_features, match_weights );

    //  Make a dummy vector of intensity weights.
    // vcl_vector< double > dummy_intensity_weights( match_weights.size(), 1.0 );

    //  Add the feature and its matches and weights to the match set
    matches_sptr -> add_feature_matches_and_weights( *fitr, mapped_feature, matched_to_features,
            match_weights );
  }

  DebugMacro(3, "matches set from size = " << matches_sptr->from_size() <<'\n');

  typedef rgrl_match_set::from_iterator       from_iter;
  typedef from_iter::to_iterator              to_iter;
  for ( from_iter fitr = matches_sptr->from_begin(); fitr !=  matches_sptr->from_end(); ++fitr ) {
    if ( fitr.size() == 0 )  continue;

    rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
    for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
      rgrl_feature_sptr to = titr.to_feature();
      DebugMacro(3, mapped_from->location()[0]<<' '<<mapped_from->location()[1]<<' '<<
            to->location()[0]<<' '<<to->location()[1]<<'\n');
    }
  }

  return matches_sptr;
}


template <class PixelType>
void
rgrl_matcher_pseudo<PixelType> ::
map_region_intensities( rgrl_transformation      const& trans,
                        rgrl_feature_sptr               feature_sptr,
                        rgrl_mapped_pixel_vector_type & mapped_pixels) const
{
  static vnl_vector<double> spacing_ratio( 2, 1.0 );

  if ( feature_sptr -> is_type( rgrl_feature_face_region::type_id() ) )
  {
    rgrl_feature_face_region * face_ptr =
        rgrl_cast<rgrl_feature_face_region *> ( feature_sptr );
    this -> map_region_intensities( face_ptr -> pixel_coordinates_ratio( spacing_ratio ), trans,
            feature_sptr, mapped_pixels );
  }
  else if ( feature_sptr -> is_type( rgrl_feature_trace_region::type_id() ))
  {
    rgrl_feature_trace_region * trace_ptr =
      rgrl_cast<rgrl_feature_trace_region *> ( feature_sptr );
    this -> map_region_intensities( trace_ptr -> pixel_coordinates_ratio( spacing_ratio ), trans,
                                    feature_sptr, mapped_pixels );
  }
  else // Assuming feature_point_region
  {
    rgrl_feature_point_region * point_ptr =
      rgrl_cast<rgrl_feature_point_region *> ( feature_sptr );
    this -> map_region_intensities( point_ptr -> pixel_coordinates_ratio( spacing_ratio ), trans,
                                    feature_sptr, mapped_pixels );
  }
}


template <class PixelType>
void
rgrl_matcher_pseudo<PixelType> ::
map_region_intensities( vcl_vector< vnl_vector<int> > const& pixel_locations,
                        rgrl_transformation           const& trans,
                        rgrl_feature_sptr                    feature_sptr,
                        rgrl_mapped_pixel_vector_type   & mapped_pixels) const
{
  DebugMacro(3,"   number of points around the from point: " << pixel_locations.size() <<'\n');
  // check # of pixels
  if ( pixel_locations.empty() )  return;

  unsigned dim = feature_sptr -> location() . size();
  assert ( dim == 2 ); // so far vil force it to be 2D
  vnl_double_2 pixel_loc_dbl;
  rgrl_mapped_pixel_type  mapped_pixel;
  mapped_pixel . weight = 1.0;

  // reserve space
  mapped_pixels.reserve( pixel_locations.size() );

  double intensity;
  for ( unsigned int i=0; i<pixel_locations.size(); ++i )
  {
    //  Copy the int pixel locations to doubles.  Yuck.
    for ( unsigned int j=0; j<dim; ++j )  pixel_loc_dbl[j] = pixel_locations[i][j];
    // Check if the location is inside the valid region
    if ( !in_range( from_image_, from_mask_, pixel_loc_dbl ) )
      continue;

    trans.map_location( pixel_loc_dbl.as_ref(), mapped_pixel.location.as_ref().non_const() );
    // Check if the location is inside the valid region
    if ( !in_range( to_image_, to_mask_, mapped_pixel.location ) )
      continue;

    //  Extract the intensity.  This is where we need ITK.
    // only work for one plane so far
    assert ( from_image_.nplanes() == 1 );

    intensity = from_image_( pixel_locations[i][0], pixel_locations[i][1] );
    //PixelType intensity; //  =  SOMETHING from ITK
    mapped_pixel . intensity = intensity; // trans . map_intensity( pixel_loc_dbl, intensity );
    mapped_pixels . push_back( mapped_pixel );
  }
}

//  inline
//  double
//  est_sub_offset( vnl_matrix< double > const& A, vnl_matrix< double > const& S )
//  {
//    // A S = X, where X = [ a b c ]
//    vnl_svd< double > svd( A );
//    vnl_matrix< double > inv = svd.inverse();
//    vnl_matrix< double > X = inv * S;
//    assert( X.columns() == 1 );

//    // if it fit a line, instead of a parabola
//    // then return the original best index
//    if ( X[ 0 ][ 0 ] == 0 )
//      return 0.0;

//    // find r that minimizes s
//    // ds = 2ar + b = 0
//    // r = -b / 2a
//    return - X[ 1 ][ 0 ] / ( 2 * X[ 0 ][ 0 ] ) ;
//  }

//  inline
//  vnl_vector< double >
//  sub_pixel_2d( vcl_vector< vcl_vector< double > > const& responses,
//                int idx1, int idx2 )
//  {
//    assert( responses.size() >= 3 );
//    assert( responses[ 0 ].size() >= 3 );
//    for ( unsigned i = 1; i < responses.size(); ++i ) {
//      assert( responses[ i ].size() == responses[ 0 ].size() );
//    }

//    if ( idx1 == 0 ) idx1 = 1;
//    if ( idx2 == 0 ) idx2 = 1;

//    if ( idx1 == (int)responses.size() - 1 )
//      idx1 = responses.size() - 2;
//    if ( idx2 == (int)responses.size() - 1 )
//      idx2 = responses.size() - 2;

//    // In 2D, I treat it as fitting a parabola in each direction ( d1
//    // and d2 ), since we use the curvature along both direction to
//    // approximate the paincipal curvature anyway.

//    // let s be the similarity error, s = a r^2 + b r + c.
//    // Use points index-1, index, index+1 to model the
//    // parameters X = [a, b, c].
//    vnl_matrix < double > A ( 3, 3 );
//    vnl_matrix< double > S1 ( 3, 1 ) ;
//    vnl_matrix< double > S2 ( 3, 1 ) ;

//    for ( int r = -1; r <= 1; ++r ) {
//      A( r+1, 0 ) = r * r;
//      A( r+1, 1 ) = r;
//      A( r+1, 2 ) = 1;
//      S1( r+1, 0 ) = responses[ idx1 + r][ idx2 ];
//      S2( r+1, 0 ) = responses[ idx1 ][ idx2 + r ];
//    }

//    vnl_vector< double > best_index( 2 );
//    best_index[ 0 ] = est_sub_offset( A, S1 ) + idx1;
//    best_index[ 1 ] = est_sub_offset( A, S2 ) + idx2;

//    return best_index;
//  }

inline
double
sub_pixel( vcl_vector< double > const& responses )
{
  assert( responses.size() == 3 );

  // let s be the similarity error, s = a r^2 + b r + c.
  // Use points index-1, index, index+1 to model the
  // parameters X = [a, b, c].
  vnl_matrix < double > A ( 3, 3 );
  vnl_matrix< double > S ( 3, 1 ) ;

  for ( unsigned i = 0; i < 3; ++i ) {
    int r = i - 1;
    A( r+1, 0 ) = r * r;
    A( r+1, 1 ) = r;
    A( r+1, 2 ) = 1;
    S( r+1, 0 ) = responses[ i ];
  }

  vnl_svd< double > svd( A );
  vnl_matrix< double > inv = svd.inverse();
  vnl_matrix< double > X = inv * S;
  assert( X.columns() == 1 );

  // if it fit a line, instead of a parabola
  // then return the original best index
  if ( X[ 0 ][ 0 ] <= 1.0e-5 ) {
    return 0;
  }

  // find r that minimizes s
  // ds = 2ar + b = 0
  // r = -b / 2a
  double best_index =  -X[ 1 ][ 0 ] / ( 2 * X[ 0 ][ 0 ] );

  assert( best_index <= 1 && best_index >= -1 );

  return best_index;
}

template <class PixelType>
void
rgrl_matcher_pseudo<PixelType> ::
match_mapped_region( rgrl_feature_sptr                     mapped_feature,
                     rgrl_mapped_pixel_vector_type const & mapped_pixels,
                     rgrl_scale                    const & current_scale,
                     vcl_vector< rgrl_feature_sptr >     & matched_to_features,
                     vcl_vector< double >                & match_weights ) const
{
  //  At this point, find the most similar feature within the given
  //  scale.
  unsigned int dim = mapped_feature -> location() . size();

  const double scale_multiplier = 4;   // magic number.  frown.

  vnl_matrix< double > normal_space;

  if ( mapped_feature -> is_type( rgrl_feature_face_region::type_id() ) )
  {
    rgrl_feature_face_region * face_ptr =
    rgrl_cast<rgrl_feature_face_region *> ( mapped_feature );
    normal_space = vnl_matrix< double > ( dim, 1 );
    normal_space . set_column ( 0, face_ptr -> normal() );
  }
  else if ( mapped_feature->is_type( rgrl_feature_trace_region::type_id()) ) // RGRL_FEATURE_TRACE_REGION
  {
    rgrl_feature_trace_region * trace_ptr =
    rgrl_cast<rgrl_feature_trace_region *> ( mapped_feature );
    normal_space = trace_ptr -> normal_subspace();
  }
  else { // RGRL_FEATURE_POINT_REGION
    normal_space = vnl_matrix<double>(2,2,vnl_matrix_identity);
  }

  vnl_double_2 match_location;
  double min_response = 0.0;
  double second_derivative = 0.0;
  int max_offset = vnl_math_rnd( scale_multiplier * current_scale . geometric_scale() );
  if ( max_offset == 0 ) max_offset = 1;

  //  DO THE REST DEPENDING ON IF THE NORMAL SUBSPACE IS 1D or 2D.
  //  IN THE FUTURE, IF WE WANT TO JUMP TO N-D, THIS WILL NEED TO BE
  //  CHANGED, PERHAPS JUST BY ADDING EACH DIMENSION WE WANT.

  // 3D case should use rgrl_matcher_pseudo_3d
  //assert( normal_space.columns() == 1 );
  if ( normal_space.columns() == 1 ) {
    vnl_double_2 basis = normal_space.get_column(0);

    DebugMacro_abv(2, "normal basis :\n" << basis << '\n');

    vcl_vector<double> responses( 2*max_offset+1, 0.0 );
    bool is_best_initialized = false;
    int best_offset = 0;

    //  Find the offset along the basis direction giving the best
    //  response.

    vnl_double_2 mapped_location = mapped_feature -> location();

    // Don't favor the max_offset_range. sometimes the region is
    // homogeneous, the responses might have same value
    for ( int abs_offset = 0; abs_offset <= max_offset; ++abs_offset )
    {
      int offset = abs_offset;
      do {
        int i = offset + max_offset;
        responses[i] = this -> compute_response( mapped_location, mapped_pixels, basis * double(offset) );

        DebugMacro_abv(2," response at offset " << offset
                       << " ( i = " << i << " ) : " << responses[ i ] <<'\n' );

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

    DebugMacro_abv(2," the best offset = " << best_offset << '\n' );

    assert( is_best_initialized );

    //  Evaluate the second derivative at the peak.  If the
    //  peak occurrence is on the boundary, compute the second
    //  derivative based on one step in from the boundary.
    int deriv_loc = best_offset;
    if ( deriv_loc == -max_offset ) ++ deriv_loc;
    else if ( deriv_loc == max_offset ) -- deriv_loc;
    int index = deriv_loc + max_offset;

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
        // derivative value of the other neighbor and sub_pixel is not necessary.
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
//        if ( sub_offset + best_offset < -max_offset ) best_offset = -max_offset;
//        if ( sub_offset + best_offset > max_offset ) best_offset = max_offset;
        }
      }

    match_location = mapped_location + basis * ( best_offset + sub_offset );

    DebugMacro_abv(2,"best match :\n" << match_location << '\n' );

    // Here I use the second derivative at index to approximate the
    // second derivative at sub_pixel.
    // We need at least three valid values for calculating second derivatives.
    //
    if ( index >0 && index+1 < (int)responses.size() &&
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
      DebugMacro_abv(2, "index=" << index << ", max_offset="
                     << max_offset << ", responses[index-1]=" << responses[index-1]
                     << ", responses[index+1]=" << responses[index+1] << '\n'
                     << "   neighbors' responses are not valid. Set the second_derivative = 0\n");
    }
  }
  else if ( normal_space.columns() == 2 ) { //For feature_point_region
    vnl_vector<double> basis1 = normal_space . get_column(0);
    vnl_vector<double> basis2 = normal_space . get_column(1);

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
      DebugMacro_abv(2, " sub_offset1 = " << sub_offset1 << " in [ "
                     << -max_offset << " , " << max_offset << " ]\n");
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

      DebugMacro_abv(2, " sub_offset2 = " << sub_offset2 << " in [ "
                     << -max_offset << " , " << max_offset << " ]\n");
    }

    double second_d2 = vnl_math_abs( responses[ idx1 ][ idx2-1 ] + responses[ idx1 ][ idx2+1 ]
                                     - 2 * responses[ idx1 ][ idx2 ] );

    second_derivative = vnl_math_min( second_d1, second_d2 );
    match_location = mapped_location + basis1 * sub_offset1 + basis2 * sub_offset2;
    DebugMacro_abv(1, "best match :\n" << match_location << '\n' );
  }
  else {
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
  else if (  mapped_feature -> is_type( rgrl_feature_trace_region::type_id() ) )
  {
    rgrl_feature_trace_region * trace_ptr =
      rgrl_cast<rgrl_feature_trace_region *> ( mapped_feature );
    mf_ptr = new rgrl_feature_trace_region( match_location, trace_ptr -> tangent() );
  }
  else { //rgrl_feature_point_region
    mf_ptr = new rgrl_feature_point_region( match_location );
  }

  matched_to_features.push_back( mf_ptr );
  double weight = second_derivative / (1.0 + min_response);
#if 0 // other options for weights
  double epsilon = 1e-16;
  double weight = min_response/(second_derivative + epsilon);
  double weight = 1;
#endif // 0

  match_weights.push_back( weight );
  DebugMacro(3, "Signature error for best match :\n" << weight
             <<", second_derivative = "<< second_derivative
             <<" min_response = "<<min_response
             <<" old wgt = "<<second_derivative / (1.0 + min_response)<<'\n');
}

template <class PixelType>
double
rgrl_matcher_pseudo<PixelType> ::
compute_response( vnl_double_2                  const& mapped_location,
                  rgrl_mapped_pixel_vector_type const& mapped_pixels,
                  vnl_double_2                  const& shift ) const
{
  //  Extract the intensities at the mapped locations.  Make sure
  //  they are inside the image.

  vcl_vector< double > a;
  vcl_vector< double > b;
  vcl_vector< double > weights;
  double intensity;

  // reserve space
  a.reserve( mapped_pixels.size() );
  b.reserve( mapped_pixels.size() );
  weights.reserve( mapped_pixels.size() );
  for ( unsigned i = 0; i < mapped_pixels.size(); ++i ) {
    vnl_double_2 location = mapped_pixels[i].location + shift;
    //vcl_cout << " position : " << mapped_pixels[ i ].location << "  shift " << shift << vcl_endl;
    // Check if the location is inside the valid region
    if ( !in_range( to_image_, to_mask_, location ) )
      return max_response_value;

    intensity = vil_bilin_interp(to_image_,  location[0], location[1] );
    a.push_back( (double)(mapped_pixels[i].intensity) );
    b.push_back( intensity );
    weights.push_back( mapped_pixels[i].weight );
  }

  //  call the response function
  return evaluator_->evaluate( a, b, weights );
}

#endif // rgrl_matcher_pseudo_txx_
