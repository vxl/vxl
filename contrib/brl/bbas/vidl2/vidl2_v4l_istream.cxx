// This is brl/bbas/vidl2/vidl2_v4l_istream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Brendan McCane
// \date   16 Mar 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_v4l_istream.h"
#include "vidl2_v4l_params.h"
#include "vidl2_pixel_format.h"
#include "vidl2_frame.h"

//: Destructor
vidl2_v4l_istream::~vidl2_v4l_istream()
{
    close();
}

//: open a device
bool vidl2_v4l_istream::open(const vcl_string &device_name)
{
    close();
    frame_number_ = 0;

    fd_ = ::open(device_name.c_str(), O_RDWR);

    if (fd_==-1)
        perror("problem with v4l");

    if (-1 == ioctl (fd_, VIDIOCGCAP, &vc)) {
        perror ("VIDIOCGCAP");
        return false;
    }
    vcl_cout << "name = " << vc.name << vcl_endl
             << "type = " << vc.type << vcl_endl
             << "channels = " << vc.channels << vcl_endl
             << "maxwidth = " << vc.maxwidth << vcl_endl
             << "maxheight = " << vc.maxheight << vcl_endl;

    if (-1 == ioctl (fd_, VIDIOCGWIN, &vw)) {
        perror ("VIDIOCGWIN");
        return false;
    }
    defaults_.ni_ = vw.width;
    defaults_.nj_ = vw.height;

    if (-1 == ioctl (fd_, VIDIOCGPICT, &vp)) {
        perror ("VIDIOCGPICT");
        return false;
    }

    defaults_.brightness_ = vp.brightness;
    defaults_.hue_ = vp.hue;
    defaults_.colour_ = vp.colour;
    defaults_.contrast_ = vp.contrast;
    defaults_.whiteness_ = vp.whiteness;
    defaults_.depth_ = vp.depth;
    defaults_.pixel_format_ = vidl2_v4l_params::v4lpf_to_vidl2pf(vp.palette);
    if (defaults_.pixel_format_ ==  VIDL2_PIXEL_FORMAT_UNKNOWN)
        defaults_.pixel_format_ = VIDL2_PIXEL_FORMAT_YUV_420P;

    if (-1 == ioctl (fd_, VIDIOCGMBUF, &vm)) {
        perror ("VIDIOCGMBUF");
        return false;
    }
    // for the moment just use one mmapped frame
    buf = mmap(0, vm.size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_,0);
    if (buf==(unsigned char *)-1)
    {
        perror("problem with mmap");
        close();
        return false;
    }

    // this looks a bit redundant (and it is), but some other things
    // are also set in set_params besides the actual driver controls
    bool sp_ret = set_params(defaults_);
    vcl_cerr << "sp_ret = " << sp_ret << vcl_endl;
    // serious problems if we can't set the params to the defaults
    if (!sp_ret)
    {
        vcl_cerr << "Problem in constructor, can't set camera to default\n"
                 << "Trying to continue anyway ...\n";
    }

    return sp_ret;
}

//: return true if the stream is open for reading
bool vidl2_v4l_istream::is_open() const
{
    return fd_>0;
}

//: return true if the stream is in a valid state
bool vidl2_v4l_istream::is_valid() const
{
    return is_open();
}

//: close the stream
void vidl2_v4l_istream::close()
{
    ::close(fd_);
    fd_ = -1;
}

//: set the parameters for this stream.
// If all the parameters are set as requested, return true; else return false
bool vidl2_v4l_istream::set_params(const vidl2_v4l_params &p)
{
    bool success=true;

    params_ = p;

    vw.width = p.ni_;
    vw.height = p.nj_;
    if (-1 == ioctl (fd_, VIDIOCSWIN, &vw)) {
        vcl_cerr << "VIDIOCSWIN\n";
        perror ("VIDIOCGWIN");
        return false;
    }
    if (-1 == ioctl (fd_, VIDIOCGWIN, &vw)) {
        vcl_cerr << "VIDIOCGWIN\n";
        perror ("VIDIOCGWIN");
        return false;
    }
    if ((vw.width!=p.ni_)||(vw.height!=p.nj_))
    {
        vcl_cerr << "width not equal to default\n";
        success = false;
    }

    vp.brightness = p.brightness_;
    vp.hue =  p.hue_;
    vp.colour = p.colour_;
    vp.contrast = p.contrast_;
    vp.whiteness = p.whiteness_;
    vp.depth = p.depth_;
    vp.palette = vidl2_v4l_params::vidl2pf_to_v4lpf(p.pixel_format_);
    if (-1 == ioctl (fd_, VIDIOCSPICT, &vp)) {
        vcl_cerr << "VIDIOCSPICT\n";
        perror ("VIDIOCSPICT");
        return false;
    }
    if (-1 == ioctl (fd_, VIDIOCGPICT, &vp)) {
        vcl_cerr << "VIDIOCGPICT\n";
        perror ("VIDIOCGPICT");
        return false;
    }

    if (vp.brightness != p.brightness_)
    {
        success = false;
        vcl_cerr << "vp.brightness = " << vp.brightness << vcl_endl
                 << "p.brightness = " << p.brightness_ << vcl_endl;
    }
    if (vp.hue != p.hue_)
    {
        success = false;
        vcl_cerr << "vp.hue = " << vp.hue << vcl_endl
                 << "p.hue = " << p.hue_ << vcl_endl;
    }
    if (vp.colour != p.colour_)
    {
        success = false;
        vcl_cerr << "vp.colour = " << vp.colour << vcl_endl
                 << "p.colour = " << p.colour_ << vcl_endl;
    }
    if (vp.contrast != p.contrast_)
    {
        success = false;
        vcl_cerr << "vp.contrast = " << vp.contrast << vcl_endl
                 << "p.contrast = " << p.contrast_ << vcl_endl;
    }
    if (vp.whiteness != p.whiteness_)
    {
        success = false;
        vcl_cerr << "vp.whiteness = " << vp.whiteness << vcl_endl
                 << "p.whiteness = " << p.whiteness_ << vcl_endl;
    }
    if (vp.depth != p.depth_)
    {
        success = false;
        vcl_cerr << "vp.depth = " << vp.depth << vcl_endl
                 << "p.depth = " << p.depth_ << vcl_endl;
    }
    if (vp.palette != vidl2_v4l_params::vidl2pf_to_v4lpf(p.pixel_format_))
    {
        success = false;
        vcl_cerr << "vp.palette = " << vp.palette << vcl_endl
                 << "p.pixel_format = " << p.pixel_format_ << vcl_endl;
    }

    mm.width = vw.width;
    mm.height = vw.height;
    mm.format = vp.palette;
    mm.frame = 0;

    cur_frame_ = new vidl2_shared_frame(
        buf, vw.width, vw.height, vidl2_v4l_params::v4lpf_to_vidl2pf(vp.palette));

    vcl_cerr << "success = " << success << vcl_endl;
    return success;
}

//: advance to the next frame (but don't acquire an image)?
// I'm quite confused by this description - I don't even know what it means.
// I'm using the vidl2_dc1394_istream as a model here which clearly
// advances and acquires - contrary to the first sentence description
bool vidl2_v4l_istream::advance()
{
    if (ioctl(fd_, VIDIOCMCAPTURE, &mm)<0) {
        perror("VIDIOCMCAPTURE");
        return false;
    }

    int i = -1;

    // shouldn't have to loop here, but apparently we do. Something to
    // do with interrupts.
    while (i < 0) {
        i = ioctl(fd_, VIDIOCSYNC, &mm.frame);
        if (i < 0 && errno == EINTR){
            perror("VIDIOCSYNC problem");
            continue;
        }
        if (i < 0) {
            perror("VIDIOCSYNC");
            /* You may want to exit here, because something has gone
               pretty badly wrong... */
            return false;
        }
        break;
    }

    frame_number_++;

    return true;
}

//: Read the next frame from the stream
//  Not sure why this one is needed ...
vidl2_frame_sptr vidl2_v4l_istream::read_frame()
{
    if (advance())
        return current_frame();
    return NULL;
}
