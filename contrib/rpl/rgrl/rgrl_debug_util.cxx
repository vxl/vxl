#include "rgrl_debug_util.h"
//:
// \file
// \brief Utility functions/classes to help debug registration
//
// Current implementations only consider featured-based registration.
// For the view-based registration, it should be fairly easy to add
// the support for views.
// \author Gehua Yang
// \date Aug 2004

#include <rgrl/rgrl_feature_based_registration.h>
#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_match_set.h>

#include <vcl_cstdio.h>
#include <vcl_fstream.h>

//: observer to view transformations at each iteration of feature-based registration engine
void
rgrl_debug_feature_iteration_print::
execute(const rgrl_object* caller, const rgrl_event & event )
{
  const rgrl_feature_based_registration* reg_engine =
    dynamic_cast<const rgrl_feature_based_registration*>(caller);
  if ( !reg_engine ) {
    vcl_cerr << "WARNING: " << __FILE__ << "(line " << __LINE__ << ")\n"
             << "         This iteration observer only works with rgrl_feature_based_registrion engine.\n";
    return;
  }
  vcl_cerr << "WARNING: rgrl_debug_feature_iteration_print::execute(): event "
           << event.name() << " is not used\n";

  // current stage
  vcl_cout <<"Current stage = " << reg_engine->current_stage() << vcl_endl;
  // current ieration of this stage
  vcl_cout <<"Current iteration = " << reg_engine->iterations_at_current_stage() << vcl_endl;
  // current transformation
  const rgrl_transformation_sptr& trans_sptr = reg_engine->current_transformation();
  if ( !trans_sptr ) {
    vcl_cout << "Current transformation is invalid!\n";
    return;
  } else {
    vcl_cout << "Current transformation =\n";
    trans_sptr->write( vcl_cout );
    vcl_cout << vcl_endl;
  }
}

//: constructor
rgrl_debug_feature_iteration_save_matches::
rgrl_debug_feature_iteration_save_matches( const vcl_string& path,
                                           const vcl_string& prefix,
                                           const rgrl_mask_sptr& from_roi )
  : path_(path), file_prefix_(prefix), from_roi_sptr_(from_roi)
{
}


void
rgrl_debug_feature_iteration_save_matches::
execute(const rgrl_object* caller, const rgrl_event & event )
{
  static char stage_buffer[31], iter_buffer[31];
  const rgrl_feature_based_registration* reg_engine =
    dynamic_cast<const rgrl_feature_based_registration*>(caller);
  if ( !reg_engine ) {
    vcl_cerr << "WARNING: " << __FILE__ << "(line " << __LINE__ << ")\n"
             << "         This iteration observer only works with rgrl_feature_based_registrion engine.\n";
    return;
  }
  vcl_cerr << "WARNING: rgrl_debug_feature_iteration_save_matches::execute(): event "
           << event.name() << " is not used\n";

  // current stage
  const int stage = reg_engine->current_stage();
  vcl_sprintf( stage_buffer, "%02d", stage );
  vcl_cout <<"Current stage = " << stage << vcl_endl;
  // current ieration of this stage
  const int iteration = reg_engine->iterations_at_current_stage();
  vcl_sprintf( iter_buffer, "%02d", iteration );
  vcl_cout <<"Current iteration = " << iteration << vcl_endl;

  // current transformation
  // compose file name for storing transformation
  vcl_string xform_name = path_ + '/' + file_prefix_ + '_' + stage_buffer + '_' + iter_buffer + ".xform";
  const rgrl_transformation_sptr& trans_sptr = reg_engine->current_transformation();
  if ( !trans_sptr )  {
    vcl_cout << "Current transformation is invalid!\n";
    return;
  } else {
    vcl_cout << "Trying to store transformation to " << xform_name << " ........\n";
    vcl_ofstream ofs( xform_name.c_str() );
    if ( !ofs ) {
      vcl_cerr << "ERROR: Cannot open file to write: " << xform_name << vcl_endl;
      return;
    }
    trans_sptr->write( ofs );
    ofs.close();
  }

  // Output current match sets
  //
  typedef rgrl_match_set::from_iterator  from_iter;
  typedef from_iter::to_iterator         to_iter;

  // compose file name for storing matches
  vcl_string matches_name = path_ + '/' + file_prefix_ + '_' + stage_buffer + '_' + iter_buffer + ".matches";
  vcl_cout << "Trying to store matches to " << matches_name << " ........\n";
  vcl_ofstream mofs( matches_name.c_str() );
  if ( !mofs ) {
    vcl_cerr << "ERROR: Cannot open file to write: " << matches_name << vcl_endl;
    return;
  }

  const rgrl_set_of<rgrl_match_set_sptr>& match_sets = reg_engine->current_match_sets();
  // output dim
  const unsigned dim = match_sets[0]->from_begin().from_feature()->location().size();
  mofs << dim << vcl_endl;

  for ( unsigned ms=0; ms < match_sets.size(); ++ms ) {
    const rgrl_match_set_sptr& match_set = match_sets[ms];
    //  for each from image feature being matched
    for ( from_iter fitr = match_set->from_begin();
         fitr != match_set->from_end(); ++fitr ){
      // skip empty ones
      if ( fitr.size() == 0 )  continue;

      // check roi
      if ( from_roi_sptr_ && !from_roi_sptr_->inside( fitr.from_feature()->location() ) )
        continue;

      const rgrl_feature_sptr& from_feature = fitr.from_feature();
      const rgrl_feature_sptr& mapped_from = fitr.mapped_from_feature();
      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        //  for each match with a "to" image feature
        const rgrl_feature_sptr& to_feature = titr.to_feature();
        double error = titr.to_feature()->geometric_error( *mapped_from );
        double cum_wgt = titr.cumulative_weight();
        double sig_wgt = titr.signature_weight();

        // now output in the format of:
        // [from loc] [to loc] wgt error [mapped_to loc]
        mofs << from_feature->location() << " \t" << to_feature->location() << " \t"
             << sig_wgt << ' ' << cum_wgt << " \t" << error << " \t\t" << mapped_from->location() << vcl_endl;
      }
    }
  }
}
