#ifndef boxm_io_scene_base_h_
#define boxm_io_scene_base_h_
//:
// \file
#include <boxm/boxm_scene_base.h>

//: Binary write boxm scene to stream
void vsl_b_write(vsl_b_ostream & os, boxm_scene_base_sptr const& scene_base);


//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream & is, boxm_scene_base_sptr &scene_base);


//: Binary write boxm scene to stream
void vsl_b_write(vsl_b_ostream & os, boxm_scene_base const& scene_base);


//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream & is, boxm_scene_base &scene_base);

#endif
