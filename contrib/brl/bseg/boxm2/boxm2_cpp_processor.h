#ifndef bomx2_cpp_processor_h
#define bomx2_cpp_processor_h
//:
// \file
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_processor.h>
#include <brdb/brdb_value_sptr.h>
#include <vcl_vector.h>

class boxm2_cpp_processor: public boxm2_processor
{
 public:
    boxm2_cpp_processor() {}
    ~boxm2_cpp_processor() {}

    virtual bool  init();
    virtual bool  run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    virtual bool  finish();

    //: sets the scene this processor will work on
    bool set_scene(boxm2_scene* scene) { scene_ = scene; return true; }

 protected:
    //: scene that this processor is operating on
    boxm2_scene* scene_;
};

#endif
