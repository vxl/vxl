// This is core/vidl/vidl_v4l_istream.cxx
//:
// \file
// \author Brendan McCane
// \date   16 Mar 2006
//
//-----------------------------------------------------------------------------

#include <cstdio>
#include "vidl_v4l_istream.h"
#include "vidl_v4l_params.h"
#include "vidl_pixel_format.h"
#include "vidl_frame.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Destructor
vidl_v4l_istream::~vidl_v4l_istream()
{
    close();
}

//: open a device
bool vidl_v4l_istream::open(const std::string &device_name)
{
    close();
    frame_number_ = 0;

    fd_ = ::open(device_name.c_str(), O_RDWR);

    if (fd_==-1)
        std::perror("problem with v4l");

    if (-1 == ioctl (fd_, VIDIOCGCAP, &vc)) {
        std::perror ("VIDIOCGCAP");
        return false;
    }
    std::cout << "name = " << vc.name << std::endl
             << "type = " << vc.type << std::endl
             << "channels = " << vc.channels << std::endl
             << "maxwidth = " << vc.maxwidth << std::endl
             << "maxheight = " << vc.maxheight << std::endl;

    if (-1 == ioctl (fd_, VIDIOCGWIN, &vw)) {
        std::perror ("VIDIOCGWIN");
        return false;
    }
    defaults_.ni_ = vw.width;
    defaults_.nj_ = vw.height;

    if (-1 == ioctl (fd_, VIDIOCGPICT, &vp)) {
        std::perror ("VIDIOCGPICT");
        return false;
    }

    defaults_.brightness_ = vp.brightness;
    defaults_.hue_ = vp.hue;
    defaults_.colour_ = vp.colour;
    defaults_.contrast_ = vp.contrast;
    defaults_.whiteness_ = vp.whiteness;
    defaults_.depth_ = vp.depth;
    defaults_.pixel_format_ = vidl_v4l_params::v4lpf_to_vidlpf(vp.palette);
    if (defaults_.pixel_format_ ==  VIDL_PIXEL_FORMAT_UNKNOWN)
        defaults_.pixel_format_ = VIDL_PIXEL_FORMAT_YUV_420P;

    if (-1 == ioctl (fd_, VIDIOCGMBUF, &vm)) {
        std::perror ("VIDIOCGMBUF");
        return false;
    }
    // for the moment just use one mmapped frame
    buf = mmap(0, vm.size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_,0);
    if (buf==(unsigned char *)-1)
    {
        std::perror("problem with mmap");
        close();
        return false;
    }

    // this looks a bit redundant (and it is), but some other things
    // are also set in set_params besides the actual driver controls
    bool sp_ret = set_params(defaults_);
    std::cerr << "sp_ret = " << sp_ret << std::endl;
    // serious problems if we can't set the params to the defaults
    if (!sp_ret)
    {
        std::cerr << "Problem in constructor, can't set camera to default\n"
                 << "Trying to continue anyway ...\n";
    }

    return sp_ret;
}

//: return true if the stream is open for reading
bool vidl_v4l_istream::is_open() const
{
    return fd_>0;
}

//: return true if the stream is in a valid state
bool vidl_v4l_istream::is_valid() const
{
    return is_open();
}

//: close the stream
void vidl_v4l_istream::close()
{
    ::close(fd_);
    fd_ = -1;
}

//: set the parameters for this stream.
// If all the parameters are set as requested, return true; else return false
bool vidl_v4l_istream::set_params(const vidl_v4l_params &p)
{
    bool success=true;

    params_ = p;

    vw.width = p.ni_;
    vw.height = p.nj_;
    if (-1 == ioctl (fd_, VIDIOCSWIN, &vw)) {
        std::cerr << "VIDIOCSWIN\n";
        std::perror ("VIDIOCGWIN");
        return false;
    }
    if (-1 == ioctl (fd_, VIDIOCGWIN, &vw)) {
        std::cerr << "VIDIOCGWIN\n";
        std::perror ("VIDIOCGWIN");
        return false;
    }
    if ((vw.width!=p.ni_)||(vw.height!=p.nj_))
    {
        std::cerr << "width not equal to default\n";
        success = false;
    }

    vp.brightness = p.brightness_;
    vp.hue =  p.hue_;
    vp.colour = p.colour_;
    vp.contrast = p.contrast_;
    vp.whiteness = p.whiteness_;
    vp.depth = p.depth_;
    vp.palette = vidl_v4l_params::vidlpf_to_v4lpf(p.pixel_format_);
    if (-1 == ioctl (fd_, VIDIOCSPICT, &vp)) {
        std::cerr << "VIDIOCSPICT\n";
        std::perror ("VIDIOCSPICT");
        return false;
    }
    if (-1 == ioctl (fd_, VIDIOCGPICT, &vp)) {
        std::cerr << "VIDIOCGPICT\n";
        std::perror ("VIDIOCGPICT");
        return false;
    }

    if (vp.brightness != p.brightness_)
    {
        success = false;
        std::cerr << "vp.brightness = " << vp.brightness << std::endl
                 << "p.brightness = " << p.brightness_ << std::endl;
    }
    if (vp.hue != p.hue_)
    {
        success = false;
        std::cerr << "vp.hue = " << vp.hue << std::endl
                 << "p.hue = " << p.hue_ << std::endl;
    }
    if (vp.colour != p.colour_)
    {
        success = false;
        std::cerr << "vp.colour = " << vp.colour << std::endl
                 << "p.colour = " << p.colour_ << std::endl;
    }
    if (vp.contrast != p.contrast_)
    {
        success = false;
        std::cerr << "vp.contrast = " << vp.contrast << std::endl
                 << "p.contrast = " << p.contrast_ << std::endl;
    }
    if (vp.whiteness != p.whiteness_)
    {
        success = false;
        std::cerr << "vp.whiteness = " << vp.whiteness << std::endl
                 << "p.whiteness = " << p.whiteness_ << std::endl;
    }
    if (vp.depth != p.depth_)
    {
        success = false;
        std::cerr << "vp.depth = " << vp.depth << std::endl
                 << "p.depth = " << p.depth_ << std::endl;
    }
    if (vp.palette != vidl_v4l_params::vidlpf_to_v4lpf(p.pixel_format_))
    {
        success = false;
        std::cerr << "vp.palette = " << vp.palette << std::endl
                 << "p.pixel_format = " << p.pixel_format_ << std::endl;
    }

    mm.width = vw.width;
    mm.height = vw.height;
    mm.format = vp.palette;
    mm.frame = 0;

    cur_frame_ = new vidl_shared_frame(
        buf, vw.width, vw.height, vidl_v4l_params::v4lpf_to_vidlpf(vp.palette));

    std::cerr << "success = " << success << std::endl;
    return success;
}

//: advance to the next frame (but don't acquire an image)?
// I'm quite confused by this description - I don't even know what it means.
// I'm using the vidl_dc1394_istream as a model here which clearly
// advances and acquires - contrary to the first sentence description
bool vidl_v4l_istream::advance()
{
    if (ioctl(fd_, VIDIOCMCAPTURE, &mm)<0) {
        std::perror("VIDIOCMCAPTURE");
        return false;
    }

    int i = -1;

    // shouldn't have to loop here, but apparently we do. Something to
    // do with interrupts.
    while (i < 0) {
        i = ioctl(fd_, VIDIOCSYNC, &mm.frame);
        if (i < 0 && errno == EINTR){
            std::perror("VIDIOCSYNC problem");
            continue;
        }
        if (i < 0) {
            std::perror("VIDIOCSYNC");
            // You may want to exit here, because something has gone
            // pretty badly wrong...
            return false;
        }
        break;
    }

    frame_number_++;

    return true;
}

//: Read the next frame from the stream
//  Not sure why this one is needed ...
vidl_frame_sptr vidl_v4l_istream::read_frame()
{
    if (advance())
        return current_frame();
    return NULL;
}


//: Return the width of each frame
unsigned int vidl_v4l_istream::width() const
{
  if (!is_open())
    return 0;
  return cur_frame_->ni();
}


//: Return the height of each frame
unsigned int vidl_v4l_istream::height() const
{
  if (!is_open())
    return 0;
  return cur_frame_->nj();
}


//: Return the pixel format
vidl_pixel_format vidl_v4l_istream::format() const
{
  if (!is_open())
    return VIDL_PIXEL_FORMAT_UNKNOWN;
  return cur_frame_->pixel_format();
}


//: Return the frame rate (FPS, 0.0 if unspecified)
double vidl_v4l_istream::frame_rate() const
{
  // TODO return a frame rate here if v4l can be
  // configured for a constant frame rate
  return 0.0;
}
