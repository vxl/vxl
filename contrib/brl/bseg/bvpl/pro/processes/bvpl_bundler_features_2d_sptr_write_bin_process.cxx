//this is /contrib/bvpl/pro/bvpl_bundler_features_write_bin_process.cxx
#include<bprb/bprb_func_process.h>
//:
// \file
// \brief A process for writing bvpl_bundler_features_2d_sptr to disk.
//
// \author Brandon A. Mayer
// \date Sept  15, 2011
// \verbatim
//  Modifications
//
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <bvpl/util/bvpl_bundler_features_2d.h>
#include <bvpl/util/io/bvpl_bundler_features_2d_io.h>

#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>

namespace bvpl_bundler_features_2d_write_bin_process_globals
{
    const unsigned n_inputs_ = 2;
    const unsigned n_outputs_ = 0;
}//end bvpl_bundler_features_2d_write_bin_process_globals


bool bvpl_bundler_features_2d_write_bin_process_cons( bprb_func_process& pro )
{
    using namespace bvpl_bundler_features_2d_write_bin_process_globals;

    vcl_vector<vcl_string> input_types_(n_inputs_);

    unsigned i = 0;
    input_types_[i++] = "bvpl_bundler_features_2d_sptr";//the smart pointer
    input_types_[i++] = "vcl_string";//filename

    if (!pro.set_input_types(input_types_))
    {
        vcl_cerr << "----ERROR---- bvpl_bundler_features_2d_write_bin_process_cons\n"
                 << "\tCOULD NOT SET INPUT TYPES.\n"
                 << __FILE__ << '\n'
                 << __LINE__ << '\n' << vcl_flush;
        return false;
    }

    return true;
}//end bvpl_bundler_features_2d_write_bin_process_cons

bool bvpl_bundler_features_2d_write_bin_process( bprb_func_process& pro )
{
    using namespace bvpl_bundler_features_2d_write_bin_process_globals;

    if ( pro.n_inputs() != n_inputs_ )
    {
        vcl_cerr << pro.name()
                 << " bvpl_bundler_features_2d_compute_process: NUMBER OF INPUTS SHOULD BE: "
                 << n_inputs_ << vcl_endl
                 << "File: " << __FILE__ << vcl_endl
                 << "Line: " << __LINE__ << vcl_endl;
        return false;
    }

    //get inputs
    unsigned i = 0;
    bvpl_bundler_features_2d_sptr
        bundler_features_sptr           = pro.get_input<bvpl_bundler_features_2d_sptr>(i++);
    vcl_string filename                 = pro.get_input<vcl_string>(i++);

    vsl_b_ofstream of(filename);
    vsl_b_write(of,bundler_features_sptr);

    return true;

}//end bvpl_bundler_features_2d_write_bin_process
