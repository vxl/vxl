#ifndef vil_region_finder_txx_
#define vil_region_finder_txx_

#include "vil_region_finder.h"

// ---------------------------------------------------------------------------
//                                                                 constructor

template <class pix_type, class predicate_type>
vil_region_finder<pix_type, predicate_type>::
vil_region_finder( image_view const& in_image,
                   vil_region_finder_connectivity conn )
  : image_( in_image ),
    processed_( image_.ni(), image_.nj(), image_.nplanes() )
{
  init( conn );
}


// ---------------------------------------------------------------------------
//                                                                        init

template <class pix_type, class predicate_type>
void
vil_region_finder<pix_type, predicate_type>::
init( vil_region_finder_connectivity conn )
{
  static int const nbrs4_delta[4][2] = { { 1, 0}, { 0,-1}, {-1, 0}, { 0, 1} };
  static int const nbrs8_delta[8][2] = { { 1, 0}, { 1,-1}, { 0,-1}, {-1,-1},
                                         {-1, 0}, {-1, 1}, { 0, 1}, { 1, 1} };
  processed_.fill( false );
  switch ( conn ) {
   case vil_region_finder_4_conn:
    num_nbrs_ = 4;
    nbr_delta_ = nbrs4_delta;
    break;
   case vil_region_finder_8_conn:
    num_nbrs_ = 8;
    nbr_delta_ = nbrs8_delta;
    break;
  }
}


// ---------------------------------------------------------------------------
//                                                             same int region

template <class pix_type, class predicate_type>
void
vil_region_finder<pix_type, predicate_type>::
same_int_region( unsigned i, unsigned j,
                 vcl_vector<unsigned>& ri,
                 vcl_vector<unsigned>& rj )
{
  // early stop if this pixel has already been processed
  if ( processed_(i,j) )
    return;

  // otherwise, add this pixel's coordinates and process neighbours
  // with the same intensity
  pix_type p = image_(i,j);
  processed_(i,j) = true;
  ri.push_back( i );
  rj.push_back( j );

  for ( unsigned c = 0; c < num_nbrs_; ++c ) {
    unsigned nbr_i = i + nbr_delta_[c][0];
    unsigned nbr_j = j + nbr_delta_[c][1];
    if (nbr_i < image_.ni() &&
        nbr_j < image_.nj() &&
        predi_( image_( nbr_i, nbr_j ),  p ) ) {
      same_int_region( nbr_i, nbr_j, p, ri, rj );
    }
  }
}


// ---------------------------------------------------------------------------
//                                                             same int region

template <class pix_type, class predicate_type>
void
vil_region_finder<pix_type, predicate_type>::
same_int_region( unsigned i, unsigned j, pix_type p,
                 vcl_vector<unsigned>& ri,
                 vcl_vector<unsigned>& rj )
{
  // early stop if this pixel has already been processed
  if ( processed_(i,j) )
    return;

  // otherwise, add this pixel's coordinates and process neighbours
  // with the same intensity
  processed_(i,j) = true;
  ri.push_back( i );
  rj.push_back( j );

  for ( unsigned c = 0; c < num_nbrs_; ++c ) {
    unsigned nbr_i = i + nbr_delta_[c][0];
    unsigned nbr_j = j + nbr_delta_[c][1];
    if (nbr_i < image_.ni() &&
        nbr_j < image_.nj() &&
        predi_( image_( nbr_i, nbr_j ),  p ) ) {
      same_int_region( nbr_i, nbr_j, p, ri, rj );
    }
  }
}


// ---------------------------------------------------------------------------
//                                                                       image

template <class pix_type, class predicate_type>
typename vil_region_finder<pix_type, predicate_type>::image_view const&
vil_region_finder<pix_type, predicate_type>::
image() const
{
  return image_;
}

#endif // vil_region_finder_txx_
