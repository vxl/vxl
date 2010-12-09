#ifndef bocl_mem_h_
#define bocl_mem_h_
//:
// \file
// \brief  A wrapper class for cl_mem buffers for OpenCL
// \author Gamze Tunali gtunali@brown.edu
// \date  October 20, 2010
//
// \verbatim
//  Modifications
//    Andrew Miller - 30 Nov 2010 - moved to BOCL, added string, void* member var
// \endverbatim

#include "bocl_cl.h"
#include "bocl_utils.h"
#include <vcl_string.h>
#include <vcl_cstddef.h>

#define MEM_SUCCESS 1
#define MEM_FAILURE 0


//: High level wrapper of a cl_mem object (which always corresponds to some 
//  void* cpp buffer).  a bocl_mem object is responsible for freeing the 
//  cl_mem buffer but NOT THE CPU buffer.  
class bocl_mem
{
  public:
   
    //: constructor that takes the context to start with
    bocl_mem(const cl_context& context, void* buffer, unsigned num_bytes, vcl_string id);

    //: creates the memory for buffer
    bool create_buffer(const cl_mem_flags& flags);

    //: releases buffer memory
    bool release_memory();
    
    //: write to buffer (copies memory from cpu_buf to gpu buf)
    bool write_to_buffer(const cl_command_queue cmdQueue);
    
    //; wread from buffer (copies mem from gpu buf to cpu buf)
    bool read_to_buffer(const cl_command_queue cmdQueue);

    //: returns a reference to the buffer
    cl_mem& buffer()        { return buffer_; }
    
    //: returns a reference to teh cpu buffer
    void* cpu_buffer()      { return cpu_buf_; }
    
    //: returns number of bytes in buffer
    vcl_size_t num_bytes()  { return num_bytes_; }
    
    //: returns id
    vcl_string id()         { return id_; }

  private:
  
    //: OpenCL buffer
    cl_mem buffer_;
    
    //: pointer to the corresponding CPU buffer
    void* cpu_buf_; 
    
    //: number of bytes this buffer points to
    vcl_size_t num_bytes_;
    
    //: OpenCL context (reference)
    const cl_context& context_;
    
    //: string identifier for error messages 
    vcl_string id_; 

};

#endif
