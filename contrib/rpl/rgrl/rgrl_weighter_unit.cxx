//:
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_weighter_unit.h"

#include "rgrl_match_set.h"

rgrl_weighter_unit::
rgrl_weighter_unit()
{
}


void
rgrl_weighter_unit::
compute_weights( rgrl_scale const&  /*scales*/,
                 rgrl_match_set&    match_set ) const
{
  typedef rgrl_match_set::from_iterator from_iter;
  typedef from_iter::to_iterator        to_iter;

  //  for each from image feature being matched
  for( from_iter fitr = match_set.from_begin();
       fitr != match_set.from_end(); ++fitr ) {
    for( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
      titr.set_geometric_weight( 1.0 );
      titr.set_signature_weight( 1.0 );
      titr.set_cumulative_weight( 1.0 );
    }
  }
}
