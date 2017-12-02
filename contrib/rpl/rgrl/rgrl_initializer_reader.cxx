#include "rgrl_initializer_reader.h"
//:
// \file
// \author Gehua Yang

#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_trans_reader.h>
#include <rgrl/rgrl_util.h>
#include <rgrl/rgrl_scale.h>


rgrl_initializer_reader::
rgrl_initializer_reader(std::istream& istr,
                        rgrl_mask_sptr             const& from_image_roi,
                        rgrl_mask_sptr             const& to_image_roi,
                        rgrl_scale_sptr            const& prior_scale,
                        rgrl_estimator_sptr        const& estimator,
                        unsigned int                      resolution )
  : xform_index_(0),
    from_image_roi_( from_image_roi ),
    to_image_roi_( to_image_roi ),
    prior_scale_( prior_scale ),
    estimator_( estimator ),
    res_( resolution )
{
  // ASSUMING 2D
  //
  // here read in the number of initializations.
  int num=-1;

  istr>> num;
  if ( !istr || num < 0 ) {
    WarningMacro( "Cannot read in the number of initializations." );
    return;
  }

  // go through inits
  for ( int i=0; i<num; ++i ) {

    // read in transformation
    init_record one;
    rgrl_mask_box region(2), global_region(2);
    bool is_global_region_set = false;

    rgrl_transformation_sptr xform = rgrl_trans_reader::read( istr );
    if ( !istr || !xform ) {
      WarningMacro( "Cannot parse transformation" );
      return;
    }
    one.xform_ = xform;

    while (true)
    {
      std::streampos pos;
      std::string tag_str;

      // skip any empty lines
      rgrl_util_skip_empty_lines( istr );

      // store current reading position
      pos = istr.tellg();
      std::getline( istr, tag_str );

      if ( tag_str.find( "REGION" ) == 0 ) {

        vnl_vector<double> x0(2), x1(2);
        istr >> x0 >> x1;
        if ( !istr ) {
          WarningMacro( "Cannot parse region" );
          return;
        }

        region.set_x0( x0 );
        region.set_x1( x1 );
      }
      else if ( tag_str.find( "GLOBAL_REGION" ) == 0 )
      {
        vnl_vector<double> x0(2), x1(2);
        istr >> x0 >> x1;
        if ( !istr ) {
          WarningMacro( "Cannot parse region" );
          return;
        }

        global_region.set_x0( x0 );
        global_region.set_x1( x1 );
        is_global_region_set = true;
      }
      else if ( tag_str.find( "GEOMETRIC_SCALE" ) == 0 )
      {
        double scale=-1.0;
        istr >> scale;
        if ( !istr ) {
          WarningMacro( "Cannot parse scale" );
          return;
        }
        one.scale_ = new rgrl_scale;
        one.scale_->set_geometric_scale( scale, rgrl_scale::prior );
      }
      else
      {
        // Maybe it is the beginning of next transformation
        // back to the beginning of the tag line
        istr.seekg( pos );

        // break the loop
        break;
      }
    }

    // estimate global region
    if ( !is_global_region_set ) {
      global_region =
        rgrl_util_estimate_global_region(from_image_roi_,
                                         to_image_roi_,
                                         from_image_roi_->bounding_box(),
                                         *one.xform_);
    }

    // should use general prior_scale?
    if ( !one.scale_ )
      one.scale_ = prior_scale_;

    //inverse transformation
    rgrl_transformation_sptr inverse_xform;
    if ( xform->is_invertible() )
      inverse_xform = xform->inverse_transform();

    // setup view
    one.view_ = new rgrl_view( from_image_roi_,
                               to_image_roi_,
                               region,
                               global_region,
                               estimator_,
                               one.xform_,
                               res_,
                               inverse_xform );

    // store
    init_records_.push_back( one );
  }
}

bool
rgrl_initializer_reader::
next_initial( rgrl_view_sptr           & view,
              rgrl_scale_sptr          & prior_scale )
{
  if ( xform_index_ >= init_records_.size())
    return false;

  init_record const& one = init_records_[xform_index_];
  //view = new rgrl_view( from_image_roi_,
  //                      to_image_roi_,
  //                      init_view_->region(),
  //                      init_view_->global_region(),
  //                      init_view_->xform_estimator(),
  //                      xforms_[xform_index_],
  //                      init_view_->resolution() );
  view = one.view_;
  prior_scale = rgrl_initializer::enforce_prior_scale( one.scale_ );
  xform_index_++;

  return true;
}

int
rgrl_initializer_reader::
size() const
{
  return int( init_records_.size() );
}
