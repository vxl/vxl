//this is /brl/bpro/core/vil_pro/processes/vil_set_float_image_pixel_process.cxx
#include<bprb/bprb_func_process.h>
#include<brdb/brdb_value.h>

#include<bbas_pro/bbas_1d_array_float.h>

#include<vil/vil_image_view.h>
#include<vil/vil_image_view_base.h>

namespace vil_set_float_image_pixel_process_globals
{
    constexpr unsigned int n_inputs_ = 4;
    constexpr unsigned int n_outputs_ = 0;
}

bool vil_set_float_image_pixel_process_cons( bprb_func_process& pro )
{
    using namespace vil_set_float_image_pixel_process_globals;

    std::vector<std::string> input_types_(n_inputs_);

    unsigned i = 0;
    input_types_[i++] = "vil_image_view_base_sptr";
    input_types_[i++] = "unsigned";//i
    input_types_[i++] = "unsigned";//j
    input_types_[i++] = "bbas_1d_array_float_sptr";//pixel array (or scalar)

    return pro.set_input_types(input_types_);
}

bool vil_set_float_image_pixel_process( bprb_func_process& pro )
{
    using namespace vil_set_float_image_pixel_process_globals;

    if (pro.n_inputs() != n_inputs_) {
        std::cout << "vil_set_float_image_pixel_process: The number of inputs should be " << n_inputs_ << std::endl;
        return false;
    }

    unsigned i = 0;
    vil_image_view_base_sptr
        base_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
    auto x    = pro.get_input<unsigned>(i++);
    auto y    = pro.get_input<unsigned>(i++);
    bbas_1d_array_float_sptr
        pixel_array_sptr = pro.get_input<bbas_1d_array_float_sptr>(i++);

    if ( x > base_sptr->ni() || y > base_sptr->nj() )
    {
        std::cerr << "-----ERROR-----\n"
                 << "In vil_set_float_image_pixel_process\n"
                 << "Target (" << x << ", "<< y << ") out of bounds.\n";
        return false;
    }

    if ( pixel_array_sptr->data_array.size() != base_sptr->nplanes() )
    {
        std::cerr << "-----ERROR-----\n"
                 << "In vil_set_float_image_pixel_process\n"
                 << "Pixel array and Image dimensions don't match.\n";
        return false;
    }

    auto* img_ptr =
        dynamic_cast<vil_image_view<float>*>(base_sptr.as_pointer());

    if ( !img_ptr )
    {
        std::cerr << "-----ERROR-----\n"
                 << "In vil_set_float_image_pixel_process\n"
                 << "Couldn't cast to vil_image_view<float>*\n";
        return false;
    }

    for ( unsigned k = 0; k < pixel_array_sptr->data_array.size(); ++k )
        (*img_ptr)(x,y,k) = pixel_array_sptr->data_array[k];

    return true;
}
