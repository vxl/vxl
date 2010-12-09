#ifndef boxm2_cpp_render_process_h
#define boxm2_cpp_render_process_h

#include <boxm2/boxm2_cpp_process_base.h>

class boxm2_cpp_render_process : public boxm2_cpp_process_base
{
  public: 
    virtual bool init(); 
    virtual bool execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output); 
};

#endif
