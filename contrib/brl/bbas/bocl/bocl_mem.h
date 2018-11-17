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
//   Andrew Miller - 30 Nov 2010 - moved to BOCL, added string, void* member var
// \endverbatim

#include <string>
#include <iostream>
#include <cstddef>
#include <iosfwd>
#include "bocl_cl.h"
#include "bocl_utils.h"
#include "bocl_kernel.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//makes a bocl_mem a sptr
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

#define MEM_SUCCESS 1
#define MEM_FAILURE 0

//: High level wrapper of a cl_mem object (which always corresponds to some void* cpp buffer).
//  a bocl_mem object is responsible for freeing the cl_mem buffer but NOT THE CPU buffer.
class bocl_mem : public vbl_ref_count
{
 public:

  //: constructor that takes the context to start with
  bocl_mem(const cl_context& context, void* buffer, unsigned num_bytes, std::string id);
  ~bocl_mem() override;

  //: creates the memory for buffer (create from command queue as welll)
  bool create_buffer(const cl_mem_flags& flags);
  bool create_buffer(const cl_mem_flags& flags, cl_command_queue& queue);

  //: create an image buffer
  bool create_image_buffer(const cl_mem_flags& flags, const cl_image_format* format,
                           std::size_t width, std::size_t height);

  //: releases buffer memory
  bool release_memory();

  //: read/write to buffer (copies memory from cpu_buf to gpu buf)
  bool write_to_buffer(const cl_command_queue& cmd_queue, bool blocking=true);
  bool read_to_buffer(const cl_command_queue& cmd_queue, bool blocking=true);

  //: read/write buffers of arbitrary size (smaller than existing gpu mem)
  bool write_to_gpu_mem(const cl_command_queue& cmd_queue, void* buff, std::size_t size, bool blocking=true);
  bool read_from_gpu_mem(const cl_command_queue& cmd_queue, void* buff, std::size_t size, bool blocking=true);

  //: write to buffer asynchronously
  bool write_to_buffer_async(const cl_command_queue& cmd_queue);
  bool finish_write_to_buffer(const cl_command_queue& cmd_queue);

  //: map/unmap
  void* enqueue_map(const cl_command_queue& cmd_queue);
  bool enqueue_unmap(const cl_command_queue& cmd_queue, void* mapped_ptr);

  //: fill gpu buffer with value (shouldn't use any CPU mem
  template<class T>
  bool fill(const cl_command_queue& cmd_queue, T val, std::string type_string, bool blocking=false);

  //: zeros out GPU buffer
  bool zero_gpu_buffer(const cl_command_queue& cmd_queue, bool blocking=false) {
    return this->fill(cmd_queue, (cl_uint) 0, "uint", blocking);
  }

  //: initializes GPU buffer with a constant value
  bool init_gpu_buffer(void const* init_val, std::size_t value_size, cl_command_queue& cmd_queue);

  //: returns a reference to the buffer
  cl_mem& buffer()        { return buffer_; }

  //: returns a reference to the cpu buffer
  void* cpu_buffer()      { return cpu_buf_; }
  void set_cpu_buffer(void* buff) { cpu_buf_ = buff; }

  //: returns number of bytes in buffer
  std::size_t num_bytes()  const { return num_bytes_; }

  //: returns id
  std::string id()         const { return id_; }

  //: set buffer used when a clCreateGLBuffer is called..
  bool set_gl_buffer(cl_mem buff) { buffer_ = buff; is_gl_=true; return true; }

  //: time spend writing or reading
  float exec_time();

 private:

  //: OpenCL buffer
  cl_mem buffer_;

  //: pointer to the corresponding CPU buffer
  void* cpu_buf_;
  bool  delete_cpu_;
  cl_command_queue* queue_;

  //: number of bytes this buffer points to
  std::size_t num_bytes_;

  //: OpenCL context (reference)
  const cl_context& context_;

  //: cl event object identifies read/write with this particular buffer
  cl_event event_;

  //: string identifier for error messages
  std::string id_;

  //: event for profiling info
  cl_event ceEvent_;

  //: signifies if this object wraps a GL object
  bool is_gl_;

  //compile kernels and cache
  bocl_kernel* get_set_kernel(cl_device_id dev_id, cl_context context, const std::string& type="float");

  //map keeps track of all kernels compiled and cached
  static std::map<std::string, bocl_kernel*> set_kernels_;
};


//Templated fill method
template<class T>
bool bocl_mem::fill(const cl_command_queue& cmd_queue, T val, std::string type_string, bool blocking)
{
  //buffer length
  cl_uint len = static_cast<cl_uint>(this->num_bytes_) / static_cast<unsigned>(sizeof(val));
  std::size_t lThreads[2] = {64,1};
  std::size_t gThreads[2] = {RoundUp(static_cast<size_t>(len), static_cast<int>(lThreads[0])), 1};

  //get command queue info
  cl_device_id dev_id;
  cl_context context;
  /* cl_int status = */ clGetCommandQueueInfo(cmd_queue, CL_QUEUE_DEVICE,
                                              sizeof(dev_id), &dev_id, nullptr);
  /* cl_int status = */ clGetCommandQueueInfo(cmd_queue, CL_QUEUE_CONTEXT,
                                              sizeof(context), &context, nullptr);

  //grab kernel
  bocl_kernel* fillKernel = this->get_set_kernel(dev_id, context, type_string);
  bocl_mem valMem(this->context_, &val, sizeof(val), "fill value");
  valMem.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  bocl_mem lenMem(this->context_, &len, sizeof(len), "buffer length");
  lenMem.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //set args, execute
  fillKernel->set_arg(this);
  fillKernel->set_arg(&valMem);
  fillKernel->set_arg(&lenMem);
  bool good = fillKernel->execute(cmd_queue, 2, lThreads, gThreads);
  if (blocking)
    clFinish(cmd_queue);
  fillKernel->clear_args();
  return good;
}


//: Smart_Pointer typedef for boxm2_block
typedef vbl_smart_ptr<bocl_mem> bocl_mem_sptr;

//: output stream
std::ostream& operator <<(std::ostream &s, bocl_mem& scene);

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, bocl_mem const& scene);
void vsl_b_write(vsl_b_ostream& os, const bocl_mem* &p);
void vsl_b_write(vsl_b_ostream& os, bocl_mem_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bocl_mem_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, bocl_mem &scene);
void vsl_b_read(vsl_b_istream& is, bocl_mem* p);
void vsl_b_read(vsl_b_istream& is, bocl_mem_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bocl_mem_sptr const& sptr);

#endif
