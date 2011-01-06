#ifndef boxm2_cpp_process_base_h
#define boxm2_cpp_process_base_h

#include <boxm2/boxm2_process.h>
#include <boxm2/io/boxm2_cache.h>
class boxm2_cpp_process_base : public boxm2_process
{
public:
    boxm2_cpp_process_base() :  cache_(0) {}
    virtual bool set_cache(boxm2_cache* cache)   { cache_ = cache;  return true;} 

protected:

    //: opencl cache to get block mems
    boxm2_cache* cache_;
};

#endif
