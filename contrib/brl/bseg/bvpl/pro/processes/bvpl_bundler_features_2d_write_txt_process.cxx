//this is /contrib/bm/bseg/bvpl/pro/bvpl_bundler_features_2d_compute_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for writing a bvpl_bundler_features_2d_sptr to txt.
//
// \author Brandon A. Mayer
// \date Sept  9, 2011
// \verbatim
//  Modifications
//
// \endverbatim
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <bvpl/util/bvpl_bundler_features_2d.h>

namespace bvpl_bundler_features_2d_write_txt_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}//end bvpl_bundler_features_2d_write_txt_process_globals

bool bvpl_bundler_features_2d_write_txt_process_cons( bprb_func_process& pro )
{
  using namespace bvpl_bundler_features_2d_write_txt_process_globals;

  std::vector<std::string> input_types_(n_inputs_);

  unsigned i = 0;
  input_types_[i++] = "bvpl_bundler_features_2d_sptr";
  input_types_[i++] = "vcl_string";//filename

  if ( !pro.set_input_types(input_types_) )
  {
    std::cerr << "----ERROR---- bvpl_bundler_features_2d_write_txt_process_cons\n"
             << "\tCOULD NOT SET INPUT TYPES.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n' << std::flush;
    return false;
  }

  return true;
}//end bvpl_bundler_features_2d_write_txt_process_cons

bool bvpl_bundler_features_2d_write_txt_process( bprb_func_process& pro )
{
  using namespace bvpl_bundler_features_2d_write_txt_process_globals;

  if ( pro.n_inputs() != n_inputs_ )
  {
    std::cerr << pro.name()
             << " bvpl_bundler_features_2d_write_txt_process: NUMBER OF INPUTS SHOULD BE: "
             << n_inputs_ << std::endl;
    return false;
  }

  //get inputs
  unsigned i = 0;
  bvpl_bundler_features_2d_sptr bundler_features_sptr = pro.get_input<bvpl_bundler_features_2d_sptr>(i++);
  std::string filename = pro.get_input<std::string>(i++);

  bundler_features_sptr->write_txt(filename);

  return true;
}//end bvpl_bundler_featureS_2d_write_txt_process
