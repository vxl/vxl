#include "bocl_buffer_mgr.h"
#include <vcl_iostream.h>

bocl_buffer_mgr* bocl_buffer_mgr::instance_=NULL;

bocl_buffer_mgr* bocl_buffer_mgr::instance()
{
  if (instance_ == NULL)
    instance_= new bocl_buffer_mgr();
  return instance_;
}

bool bocl_buffer_mgr::create_read_buffer(const cl_context& context,
                                         vcl_string name,
                                         void* data,
                                         unsigned size)
{
  bocl_buffer* buf = new bocl_buffer(context);
  if (!buf->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size, data)) {
    vcl_cout << "bocl_buffer_mgr::create_read_buffer -- clCreateBuffer failed for " << name << vcl_endl;
    return false;
  }
  buffers_[name]=buf;
  return true;
}

bool bocl_buffer_mgr::create_write_buffer(const cl_context& context,
                                          vcl_string name,
                                          void* data,
                                          unsigned size)
{
  bocl_buffer* buf = new bocl_buffer(context);
  if (!buf->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, size, data)){
    vcl_cout << "bocl_buffer_mgr::create_write_buffer -- clCreateBuffer failed for " << name << vcl_endl;
    return false;
  }
  buffers_[name]=buf;
  return true;
}

bool bocl_buffer_mgr::create_image2D(cl_context context, vcl_string name, cl_mem_flags flags,
                                     const cl_image_format * format, vcl_size_t width,
                                     vcl_size_t height, vcl_size_t row_pitch, void * data)
{
  bocl_buffer* buf = new bocl_buffer(context);
  if (!buf->create_image2D(flags,format,width,height,row_pitch,data)){
    vcl_cout << "bocl_buffer_mgr::create_write_buffer -- clCreateBuffer failed for " << name << vcl_endl;
    return false;
  }
  buffers_[name]=buf;
  return true;
}

bocl_buffer* bocl_buffer_mgr::get_buffer(vcl_string name)
{
  if (buffers_.find(name) != buffers_.end())
    return buffers_[name];
  else
    return 0;
}

bool bocl_buffer_mgr::set_buffer(const cl_context& context, vcl_string name, cl_mem buffer)
{
  bocl_buffer* buf = new bocl_buffer(context);
  return true; // was: return buf->set_mem(buffer);
}

bool bocl_buffer_mgr::enqueue_read_buffer(const cl_command_queue& queue, vcl_string name, cl_bool block_read,
                                          vcl_size_t  offset,vcl_size_t  cnb, void*  data, cl_uint num_events,
                                          const cl_event* ev1, cl_event *ev2)
{
  cl_int status = clEnqueueReadBuffer(queue,buffers_[name]->mem(), block_read,
                                      offset, cnb, data, num_events, ev1, ev2);

  return check_val(status,CL_SUCCESS,"clCreateBuffer failed for "+ name);
}

bool bocl_buffer_mgr::enqueue_write_buffer(const cl_command_queue& queue, vcl_string name,
                                           cl_bool block_write,
                                           vcl_size_t  offset          /* offset */,
                                           vcl_size_t  cnb             /* cb */,
                                           const void*  data           /* ptr */,
                                           cl_uint num_events          /* num_events_in_wait_list */,
                                           const cl_event *  ev1       /* event_wait_list */,
                                           cl_event *   ev2)
{
  cl_int status = clEnqueueWriteBuffer(queue,buffers_[name]->mem(),block_write,
                                       offset,cnb,data,num_events,ev1,ev2);

  return check_val(status,CL_SUCCESS,"enqueue_write_buffer failed.");
}

bool bocl_buffer_mgr::release_buffer(vcl_string name)
{
  if (buffers_.find(name) != buffers_.end()) {
    buffers_[name]->release_memory();
    return true;
  }
  vcl_cout << "clReleaseMemObject failed for " << name << vcl_endl;
  return false;
}

bool bocl_buffer_mgr::release_buffers()
{
  vcl_map<vcl_string, bocl_buffer*>::const_iterator it=buffers_.begin();
  while (it != buffers_.end()) {
    if (!it->second->release_memory())
      return false;
    ++it;
  }
  return true;
}

