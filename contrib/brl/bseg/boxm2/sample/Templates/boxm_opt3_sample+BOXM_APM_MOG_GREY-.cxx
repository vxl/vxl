#include <boxm2/sample/boxm_opt3_sample.txx>

// DUMMY IMPLEMENTIATION of a static method of library boxm2_sample_algo, to avoid "undefined symbol" error in library boxm2_sample:
bool boxm_mog_grey_processor::update(bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gaussian_sphere<float,1> >,3> >&, float const&, float const&) { return false; }

BOXM2_OPT3_SAMPLE_INSTANTIATE(BOXM_APM_MOG_GREY);
