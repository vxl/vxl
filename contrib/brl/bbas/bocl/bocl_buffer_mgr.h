#ifndef bocl_buffer_mgr_h_
#define bocl_buffer_mgr_h_

#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_cstddef.h> // for std::size_t
#include "bocl_cl.h"
#include "bocl_buffer.h"

#include "bocl_utils.h"

#define BOCL_BUFFER_MGR bocl_buffer_mgr::instance()

class bocl_buffer_mgr
{
 public:
  static bocl_buffer_mgr* instance();

  bool create_read_buffer(const cl_context& context, vcl_string name, void* data, unsigned size);

  bool create_write_buffer(const cl_context& context, vcl_string name, void* data, unsigned size);

  bool create_image2D(cl_context context, vcl_string name, cl_mem_flags,
                      const cl_image_format * format, vcl_size_t width,
                      vcl_size_t height, vcl_size_t row_pitch, void * data);

  bocl_buffer* get_buffer(vcl_string name);

  bool set_buffer(const cl_context& context, vcl_string name, cl_mem buffer);

  bool enqueue_write_buffer(const cl_command_queue& queue, vcl_string name, cl_bool block_write,
                            vcl_size_t offset, vcl_size_t cnb, const void* data, cl_uint num_events,
                            const cl_event* ev1, cl_event* ev2);

  bool enqueue_read_buffer(const cl_command_queue& queue, vcl_string name, cl_bool block_read,
                           vcl_size_t offset, vcl_size_t cnb, void* data, cl_uint num_events,
                           const cl_event* ev1, cl_event *ev2);

  bool release_buffer(vcl_string name);

  bool release_buffers();

  int buffer_cnt() const { return buffers_.size(); }

 private:
  static bocl_buffer_mgr* instance_;
  bocl_buffer_mgr() {}
  vcl_map<vcl_string, bocl_buffer*> buffers_;
};

#endif
