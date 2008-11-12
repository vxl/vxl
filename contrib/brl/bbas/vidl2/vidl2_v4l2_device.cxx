//:
// \file
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     15 Apr 2009 Created (A. Garrido)
//\endverbatim

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
};

#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include "vidl2_pixel_format.h"
#include "vidl2_v4l2_device.h"
#include "v4l2_pixel_format.h"

// ----------------- local functions ---------------
namespace {
  inline int xioctl (int fd, int request, void * arg)
  {
    int r;
    do { r = ioctl(fd, request, arg); }
    while (-1 == r && EINTR == errno);
    return r;
  }
}
// --------------- end local functions -------------------


void vidl2_v4l2_device::update_controls()
{
  for (int i=0;i<controls_.size();++i) delete controls_[i]; 
  controls_.clear();

  struct v4l2_queryctrl ctrl;
  for (int indice = V4L2_CID_BASE;indice < V4L2_CID_LASTP1;indice++) {
    ctrl.id= indice;
    if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &ctrl)) { // error ignored
        vidl2_v4l2_control *pc= vidl2_v4l2_control::new_control(ctrl, fd);
        if (pc) controls_.push_back(pc);
    }
  }

  for (int indice = V4L2_CID_PRIVATE_BASE;;indice++) {
    ctrl.id= indice;
    if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &ctrl)) {// error ignored
      vidl2_v4l2_control *pc= vidl2_v4l2_control::new_control(ctrl, fd);
      if (pc) controls_.push_back(pc);
    } else  break;
  }
}




vidl2_v4l2_device::vidl2_v4l2_device(const char *file)
{
  pre_nbuffers= 4;
  ref_count_ = 0;
  dev_name_= file;
  fd= -1;
  buffers= NULL;
  n_buffers= 0;
  capturing= false;
  last_error="";

  if (!open()) {
    vcl_cerr << "Error creating device: " << last_error << vcl_endl;
    return;
  }
  if (!initialize_device()) {
    vcl_cerr << "Error initializing device: " << last_error << vcl_endl;
    close();
    return;
  }

  // Now we should consider all possibilities

  struct v4l2_input inp;

  //vcl_cerr << "Looking for inputs..." << fd << vcl_endl;
  for (inp.index=0;-1!=xioctl(fd,VIDIOC_ENUMINPUT,&inp); inp.index++) {
    //vcl_cerr << "Inserting input...\n";
    inputs_.push_back(vidl2_v4l2_input(inp));
  }

  // fmt.fmt.pix.width = 0;
  // fmt.fmt.pix.height =0;

  try_formats();
  update_controls();
}

vidl2_v4l2_device::~vidl2_v4l2_device()
{
  if (is_open()) {
    if (capturing)
      stop_capturing();
    if (buffers)
      uninit_mmap();
    close();
  }
  for (int i=0;i<controls_.size();++i) delete controls_[i]; 
}

void vidl2_v4l2_device::reset()
{
  if (is_open()) {
    if (capturing)
      stop_capturing();
    if (buffers)
      uninit_mmap();
    close();
  }
  last_error="";
  if (!open()) {
    vcl_cerr << "Error creating device: " << last_error << vcl_endl;
    return;
  }
  if (!initialize_device()) {
    vcl_cerr << "Error initializing device: " << last_error << vcl_endl;
    close();
    return;
  }

  try_formats();
  // inputs already updated
  update_controls();

}


bool vidl2_v4l2_device::open()
{
  if (is_open()) close(); // ?????

  struct stat st;

  if (-1 == stat (dev_name_.c_str(), &st)) {
    vcl_ostringstream f;
    f << "Cannot identify " << dev_name_ << ": " << vcl_strerror(errno);
    last_error=f.str();

    return false; //exit (EXIT_FAILURE);
  }

  if (!S_ISCHR (st.st_mode)) {
    vcl_ostringstream f;
    f << dev_name_ << "is not a valid video device";
    last_error=f.str();
    return false; //exit (EXIT_FAILURE);
  }

  fd = ::open (dev_name_.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

  if (-1 == fd) {
    vcl_ostringstream f;
    f << "Cannot open " << dev_name_ << ": "<< vcl_strerror(errno);
    last_error=f.str();
    return false; //exit (EXIT_FAILURE);
  }
  return true;
}

bool vidl2_v4l2_device::initialize_device()
{
  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  struct v4l2_format fmt;
  unsigned int min;

  if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) {
    vcl_ostringstream f;

    if (EINVAL == errno) {
      f << dev_name_ << " is not a valid V4L2 video device";
    }
    else
      f << "v4l2_device ->  Error in VIDIOC_QUERYCAP";
    close();
    last_error=f.str();
    return false;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    vcl_ostringstream f;
    f << dev_name_ << " is not a valid video capture device";
    close();
    last_error=f.str();
    return false;
  }

  if (!(cap.capabilities & V4L2_CAP_STREAMING)) { // Right now, only MMAP method
    vcl_ostringstream f;
    f << dev_name_ << " does not support streaming i/o";
    close();
    last_error=f.str();
    return false;
  }

  card_name_= (const char *)cap.card;
  return true;
}

bool vidl2_v4l2_device::try_formats()
{
  // change order
  // better select formats implemented in vidl2

  if (set_v4l2_format(V4L2_PIX_FMT_BGR24,640,480)) return true;
  if (set_v4l2_format(V4L2_PIX_FMT_BGR32,640,480)) return true;
  if (set_v4l2_format(V4L2_PIX_FMT_RGB565,640,480)) return true;
  if (set_v4l2_format(V4L2_PIX_FMT_RGB555,640,480)) return true;

  if (set_v4l2_format(V4L2_PIX_FMT_YUYV,640,480)) return true;
  if (set_v4l2_format(V4L2_PIX_FMT_UYVY,640,480)) return true;

  if (set_v4l2_format(V4L2_PIX_FMT_YUV422P,640,480)) return true;
  if (set_v4l2_format(V4L2_PIX_FMT_YVU420,640,480)) return true;
  if (set_v4l2_format(V4L2_PIX_FMT_YUV420,640,480)) return true;
  if (set_v4l2_format(V4L2_PIX_FMT_YUV411P,640,480)) return true;
  if (set_v4l2_format(V4L2_PIX_FMT_YVU410,640,480)) return true;

  if (set_v4l2_format(V4L2_PIX_FMT_GREY,640,480)) return true;

  // add other formats...

  fmt.fmt.pix.width = 0; // not success
  fmt.fmt.pix.height =0;

  return false;
}


// Width and height could be changed by driver
bool vidl2_v4l2_device::set_v4l2_format(int fourcode, int width, int height)
{
  fmt.fmt.pix.width = 0;
  fmt.fmt.pix.height= 0;

  if (is_open()) {
    if (capturing)
      stop_capturing();
    if (buffers)
      uninit_mmap();
    vcl_memset(&fmt, 0, sizeof(fmt));

    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = width;
    fmt.fmt.pix.height      = height;
    fmt.fmt.pix.pixelformat = fourcode;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED; // add to parameters?

    if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt)) {
      fmt.fmt.pix.width = 0;
      fmt.fmt.pix.height =0;
      return false;
    }

    if (init_mmap(pre_nbuffers)) // add to parameters?;
      return true;
    else {
      fmt.fmt.pix.width = 0;
      fmt.fmt.pix.height =0;
      return false;
    }
  }
  else return false;
}


bool vidl2_v4l2_device::init_mmap(int reqbuf)
{
  if (!format_is_set())
    if (!try_formats()) return false;

  struct v4l2_requestbuffers req;

  vcl_memset (&req, 0, sizeof (req));

  req.count               = reqbuf;
  req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory              = V4L2_MEMORY_MMAP;

  if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      vcl_ostringstream f;
      f << dev_name_ << " does not support memory mapping";
      last_error=f.str();
      return false;
    } else {
      last_error = "v4l2_device -> VIDEOC_REQBUFS";
      return false;
    }
  }

  if (req.count < 1) {
    vcl_ostringstream f;
    f<< "Insufficient buffer memory on " << dev_name_ ;
    last_error=f.str();
    return false;
  }

  buffers = (struct buffer *) vcl_calloc (req.count, sizeof (*buffers));

  if (!buffers) {
    last_error= "Out of memory reserving buffers";
    return false;
  }

  for (n_buffers = 0; n_buffers < req.count; ++n_buffers) { // n_buffers is member
    //struct v4l2_buffer buf;

    //vcl_memset (&buf, 0, sizeof (buf));
    vcl_memset(&(buffers[n_buffers].buf), 0, sizeof(struct v4l2_buffer) );

    buffers[n_buffers].buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffers[n_buffers].buf.memory      = V4L2_MEMORY_MMAP;
    buffers[n_buffers].buf.index       = n_buffers;

    if (-1 == xioctl (fd, VIDIOC_QUERYBUF, /*&buf*/&buffers[n_buffers].buf)) {
      last_error= "v4l2_device -> VIDIOC_QUERYBUF";
      vcl_free (buffers); buffers=NULL;
      return false;
    }

    //buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start =
            mmap (NULL /* start anywhere */,
                  buffers[n_buffers].buf.length,
                  PROT_READ | PROT_WRITE /* required */,
                  MAP_SHARED /* recommended */,
                  fd, buffers[n_buffers].buf.m.offset);

    if (MAP_FAILED == buffers[n_buffers].start) {
      last_error= "v4l2_device -> mmap";
      vcl_free (buffers); buffers=NULL;
      return false;
    }
  }
  last_buffer= -1;
  return true;
}


bool vidl2_v4l2_device::set_number_of_buffers(unsigned int nb){
  if (nb==0) return false;
  if (pre_nbuffers==nb) return true;
  pre_nbuffers= nb;
  if (capturing) stop_capturing();
  if (buffers) {
    uninit_mmap();
    return init_mmap(pre_nbuffers);
  }
  return true;
}


bool vidl2_v4l2_device::start_capturing ()
{
  if (capturing) return true;
  if (!buffers)
    if (!init_mmap(pre_nbuffers))
      return false;

  enum v4l2_buf_type type;

  for (unsigned int i = 0; i < n_buffers; ++i) {
    struct v4l2_buffer buf;
    vcl_memset (&buf, 0, sizeof (buf));

    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = i;

    if (-1 == xioctl (fd, VIDIOC_QBUF, &buf)){
      last_error= "v4l2_device -> VIDIOC_QBUF";
      return false;
    }
  }

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (-1 == xioctl (fd, VIDIOC_STREAMON, &type)){
    last_error= "v4l2_device -> VIDIOC_STREAMON";
    return false;
  }
  capturing= true;
  last_buffer= -1;
  return true;
}


bool vidl2_v4l2_device::read_frame()
{
  if (!capturing) return false;

  if (last_buffer!=-1)  // enqueue again the last read buffer
    if (-1 == xioctl (fd, VIDIOC_QBUF, &(buffers[last_buffer].buf) ) ) {
      last_error= "read_frame: VIDIOC_QBUF";
      return false;
    }

  struct v4l2_buffer buf;
  bool completed= false;
  do {
    fd_set fds;
    struct timeval tv;
    int r;

    FD_ZERO (&fds);
    FD_SET (fd, &fds);

    /* Timeout. */
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    r = select (fd + 1, &fds, NULL, NULL, &tv);
    if (-1 == r) {
      if (EINTR == errno)
        continue;

      last_error= "read_frame: error in select";
      return false;
    }
    if (0 == r) {
      last_error= "read_frame: select timeout";
      return false;
    }

    vcl_memset (&buf, 0, sizeof (buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
      if (errno!= EAGAIN) { // if EAGAIN -> iterate
        last_error= "read_frame: VIDIOC_DQBUF";
        return false;
      }
    }
    else completed= true;
  } while (!completed);

  buffers[buf.index].buf= buf;
  last_buffer=buf.index;

  return true;
}

bool vidl2_v4l2_device::stop_capturing()
{
  if (!capturing) return true;

  enum v4l2_buf_type type;

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type)){
    last_error= "v4l2_device -> VIDIOC_STREAMOFF";
    return false;
  }
  capturing= false;
  last_buffer= -1;
  return true;
}


bool vidl2_v4l2_device::uninit_mmap ()
{
  for (unsigned int i = 0; i < n_buffers; ++i)
    if (-1 == munmap (buffers[i].start, buffers[i].buf.length)) {
      last_error= "v4l2_device -> munmap";
      return false;
    }
  vcl_free (buffers);
  buffers= NULL;
  n_buffers=0;
}

bool vidl2_v4l2_device::close()
{
  if (-1 == ::close (fd)) {
    last_error= "Error closing device";
    return false; //     errno_exit ("close");
  }
  fd = -1;
  return true;
}

int vidl2_v4l2_device::current_input() const
{
  if (!is_open())
    return n_inputs();

  if (n_inputs()==0) return 0;

  int index;
  if (-1==xioctl(fd,VIDIOC_G_INPUT,&index)) {
    last_error= "error getting current input (VIDIOC_G_INPUT)";
    return n_inputs();
  }
  return index;
}

bool vidl2_v4l2_device::set_input(int i)
{
  if (current_input()==i)
    return true;
  if (!is_open() || i<0 || i>=n_inputs())
    return false;

    if (capturing)
      stop_capturing();
    if (buffers)
      uninit_mmap();

    if (-1==xioctl(fd,VIDIOC_S_INPUT,&i))
      return false;

  // fmt.fmt.pix.width = 0; // format unknown
  //fmt.fmt.pix.height =0;
  try_formats();
  update_controls();

  return true;
}


vcl_ostream &
operator << (vcl_ostream &os, const vidl2_v4l2_device & dev)
{
  os << dev.device_file() << " -> " <<  dev.card_name()<< vcl_endl
     << "  " << dev.n_inputs() << " inputs:"<< vcl_endl;
  for (int j=0;j<dev.n_inputs();++j){
    os << "    " <<  j << ": " << dev.input(j).name();
    if (dev.input(j).is_tuner())
      os << " is tuner" << vcl_endl;
    else
      os << " is camera" << vcl_endl;
  }
  os << "      Current input: " << dev.current_input() << vcl_endl
     << "      Current format: " << v4l2_to_vidl2(dev.get_v4l2_format())
     << " width: "<< dev.get_width()<< " height: " << dev.get_height() << vcl_endl;
}

