#include <iostream>
#include "bocl_buffer_mgr.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bocl_buffer_mgr* bocl_buffer_mgr::instance_=NULL;

bocl_buffer_mgr* bocl_buffer_mgr::instance()
{
  if (instance_ == NULL)
    instance_= new bocl_buffer_mgr();
  return instance_;
}

bool bocl_buffer_mgr::create_read_buffer(const cl_context& context,
                                         std::string name,
                                         void* data,
                                         unsigned size)
{
  bocl_buffer* buf = new bocl_buffer(context);
  if (!buf->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size, data)) {
    std::cout << "bocl_buffer_mgr::create_read_buffer -- clCreateBuffer failed for " << name << std::endl;
    return false;
  }
  buffers_[name]=buf;
  return true;
}

bool bocl_buffer_mgr::create_write_buffer(const cl_context& context,
                                          std::string name,
                                          void* data,
                                          unsigned size)
{
  bocl_buffer* buf = new bocl_buffer(context);
  if (!buf->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, size, data)){
    std::cout << "bocl_buffer_mgr::create_write_buffer -- clCreateBuffer failed for " << name << std::endl;
    return false;
  }
  buffers_[name]=buf;
  return true;
}

bool bocl_buffer_mgr::create_image2D(cl_context context, std::string name, cl_mem_flags flags,
                                     const cl_image_format * format, std::size_t width,
                                     std::size_t height, std::size_t row_pitch, void * data)
{
  bocl_buffer* buf = new bocl_buffer(context);
  if (!buf->create_image2D(flags,format,width,height,row_pitch,data)){
    std::cout << "bocl_buffer_mgr::create_write_buffer -- clCreateBuffer failed for " << name << std::endl;
    return false;
  }
  buffers_[name]=buf;
  return true;
}

bocl_buffer* bocl_buffer_mgr::get_buffer(std::string name)
{
  if (buffers_.find(name) != buffers_.end())
    return buffers_[name];
  else
    return 0;
}

bool bocl_buffer_mgr::set_buffer(const cl_context& context, std::string name, cl_mem buffer)
{
  bocl_buffer* buf = new bocl_buffer(context);
  return true; // was: return buf->set_mem(buffer);
}

bool bocl_buffer_mgr::enqueue_read_buffer(const cl_command_queue& queue, std::string name, cl_bool block_read,
                                          std::size_t  offset,std::size_t  cnb, void*  data, cl_uint num_events,
                                          const cl_event* ev1, cl_event *ev2)
{
  cl_int status = clEnqueueReadBuffer(queue,buffers_[name]->mem(), block_read,
                                      offset, cnb, data, num_events, ev1, ev2);

  return check_val(status,CL_SUCCESS,"clCreateBuffer failed for "+ name);
}

bool bocl_buffer_mgr::enqueue_write_buffer(const cl_command_queue& queue, std::string name,
                                           cl_bool block_write,
                                           std::size_t  offset          /* offset */,
                                           std::size_t  cnb             /* cb */,
                                           const void*  data           /* ptr */,
                                           cl_uint num_events          /* num_events_in_wait_list */,
                                           const cl_event *  ev1       /* event_wait_list */,
                                           cl_event *   ev2)
{
  cl_int status = clEnqueueWriteBuffer(queue,buffers_[name]->mem(),block_write,
                                       offset,cnb,data,num_events,ev1,ev2);

  return check_val(status,CL_SUCCESS,"enqueue_write_buffer failed.");
}

bool bocl_buffer_mgr::release_buffer(std::string name)
{
  if (buffers_.find(name) != buffers_.end()) {
    buffers_[name]->release_memory();
    return true;
  }
  std::cout << "clReleaseMemObject failed for " << name << std::endl;
  return false;
}

bool bocl_buffer_mgr::release_buffers()
{
  std::map<std::string, bocl_buffer*>::const_iterator it=buffers_.begin();
  while (it != buffers_.end()) {
    if (!it->second->release_memory())
      return false;
    ++it;
  }
  return true;
}
