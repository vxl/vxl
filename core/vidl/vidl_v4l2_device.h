// This is core/vidl/vidl_v4l2_device.h
#ifndef vidl_v4l2_device_h_
#define vidl_v4l2_device_h_
//:
// \file
// \brief A class for handling a video device
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//   15 Apr 2009 Created (A. Garrido)
//\endverbatim

#include <vector>
#include <string>
#include <iosfwd>
#include <iostream>
#include "vidl_v4l2_control.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
extern "C" {
//#include <asm/types.h>          /* for videodev2.h */
#include <sys/time.h>
#include <linux/videodev2.h>
};

//: A class for handle a video device input
// This class is not finished.
// I am thinking about adding controls, that is, a number of user-settable controls such as
// brightness, saturation and so on, but different devices will have different controls available.
// So, I am thinking about new classes...
class vidl_v4l2_input
{
  struct v4l2_input input_;
  vidl_v4l2_input (const struct v4l2_input& inp) { input_=inp; }
  friend class vidl_v4l2_device;
 public:
  //: Return name of input
  std::string name() const { return std::string((const char*) input_.name); }
  //: Return if the input uses a tuner (RF modulator)
  bool is_tuner() const { return input_.type==V4L2_INPUT_TYPE_TUNER; }
};

//: A class for handle a video device.
// This class is designed to be vidl independent, although it is very easy to create a vidl istream from it.
// It has not been fully tested, although it has been proven to capture images
// from multiple webcams with a single buffer (default) and has worked properly.
// Generally, the steps to be made for a capture are as follows:
// -# <b>Select input</b>
// -# <b>Select format</b>
// -# <b>Select number of buffers</b>.
// -# <b>Start capturing</b>
// -# <b>Stop capturing</b>
// When a device is opened, an input and format are selected by default (see vidl_v4l2_device::try_formats function).
// Then if you start capturing, you will <b>probably</b> get 640x480 frames,  from input 0,
// using just one buffer with an unknown format (the first one valid for the driver).
// \see vidl_v4l2_devices
class vidl_v4l2_device
{

  int fd;

  struct buffer {
    void *                  start; // vidl_frame_sptr??
    struct v4l2_buffer buf;
    //size_t                  length;
  };
  unsigned int pre_nbuffers; // Number of buffers to create
  struct buffer * buffers;
  unsigned int n_buffers;
  int last_buffer; // last read buffer to enqueue again (-1 if none)
  struct v4l2_format fmt; // width=height=0 indicates not established
  double frame_rate;

  std::string dev_name_;
  std::string card_name_;
  mutable std::string last_error;
  bool capturing; // see start_capturing
  std::vector<vidl_v4l2_input> inputs_;
  std::vector<vidl_v4l2_control *> controls_;
  void update_controls(); // must be called after input change

  // Reset the device to an initial state
  // It was public, but it is not useful for users of device, so it is moved to private
  void reset();

  bool open(); // return true if successful
  bool initialize_device(); //  return true if successful
  bool init_mmap(int reqbuf);
  bool uninit_mmap ();
  // Close device. If other functions are called, it would be automatically re-opened.
  bool close();

  bool is_open() const { return fd!=-1; }
  bool is_prepared_for_capturing() const  { return buffers!=0; }
  bool good() const { return last_error.size()==0; }

  // non-valid functions
  vidl_v4l2_device(const vidl_v4l2_device&);
  vidl_v4l2_device& operator= (const vidl_v4l2_device&);

 public:

  //: Constructor
  // After construction, device is closed (device is not busy).
  // You can use device as a boolean expression to test if device is ok.
  // \param file device name (for example, "/dev/video")
  // \param file device name (for example, "/dev/video")
  vidl_v4l2_device(const char *file);
  ~vidl_v4l2_device();

  //: Name of the associated file device (same as constructor)
  std::string device_file() const { return dev_name_; }
  //: Friendly name of the device.
  std::string card_name() const { return card_name_; }
  //: Number of inputs in device
  unsigned int n_inputs() const { return inputs_.size(); }
  //: Inputs been used (0 to ninputs-1)
  // if equal to ninputs, indicates unknown
  // \see ninputs
  unsigned int current_input() const;
  //: Return input number i (0 .. ninputs-1)
  const vidl_v4l2_input& input(int i) const { return inputs_.at(i);}

  //: Select input i
  // \return if successful
  bool set_input(unsigned int i);

  //: Select a new format.
  // Device try automatically different formats (\see try_formats()). Then this function could be ignored if you don't mind the format. Usually, user wants a concrete pixel format or, simply, change width and height.
  // \param fourcode A four character code defined in v4l2 (see v4l2 specification and vidl_pixel_format.h) indicating pixel encoding
  // \param width can be changed by drivers to the closest possible value
  // \param height can be changed by drivers to the closest possible value
  // \param fps frame rate to try (not garanteed).
  // \return if successful
  // \see format_is_set
  bool set_v4l2_format(unsigned int fourcode, int width, int height,double fps=0.0);

  //: Try several formats implemented in vidl
  // \param width can be changed by drivers to the closest possible value
  // \param height can be changed by drivers to the closest possible value
  // \param fps frame rate to try (not garanteed).
  // \return if successful

  bool try_formats(int width= 640, int height= 480);

  //: Return if the format is set.
  // Normally, a format is automatically selected or user call set_v4l2_format. User can use this function to know if a format is selected before calling start_capturing.
  // \see set_v4l2_format
  bool format_is_set() const { return fmt.fmt.pix.width!=0; }

  //: Get pixel format of type of compression
  // \return the four character code which is being used by driver(see v4l2 specification and vidl_pixel_format.h) or 0 if not set
  // \see set_v4l2_format
  unsigned int  get_v4l2_format() const {
               return (fmt.fmt.pix.width!=0)?
                      fmt.fmt.pix.pixelformat:0;
  }

  //: Return Image width in pixels.
  // \note You can use this function to know the width selected by driver after calling set_v4l2_format
  // \see set_v4l2_format
  int get_width() const { return fmt.fmt.pix.width; }
  //: Return Image height in pixels.
  // \note You can use this function to know the height selected by driver after calling set_v4l2_format
  // \see set_v4l2_format
  int get_height() const { return fmt.fmt.pix.height; }

  //: Return Current frame rate.
  // \see set_v4l2_format
  double get_frame_rate() const { return frame_rate; }

  // ----------------- Methods associated to controls -------------------

  //: Reset controls to default
  void reset_controls() ;

  //: Get number of controls
  // \return the number of detected controls (control not disabled and not inactive).
  int n_controls() const { return controls_.size(); }
  //: Get control
  // The user must downcast the pointer -depending on type- to use all functionality.
  // \param i indicates the control to be extracted, from 0 to n_controls-1
  // \return pointer to control
  // \see n_controls
  vidl_v4l2_control * get_control(int i) const { return controls_[i]; }
  //: Get control from driver id
  // The user must downcast the pointer -depending on type- to use all functionality.
  // \param id is control ID from v4l2 specification. For example, V4L2_CID_BRIGHTNESS
  // \return pointer to control or 0 if does not exist
  vidl_v4l2_control * get_control_id(int id) const
    { for (int i=0;i<n_controls();++i) { if (controls_[i]->id()==id) return controls_[i]; } return 0;}

  //: Get control from driver id
  // \param id is control ID from v4l2 specification. For example, V4L2_CID_BRIGHTNESS
  // \return pointer to control. Null if doesn't exist or is not of type V4L2_CTRL_TYPE_INTEGER
  vidl_v4l2_control_integer * get_control_integer_id( int id) const
  {
    vidl_v4l2_control *pc= get_control_id(id);
    return pc ? (pc->type()==V4L2_CTRL_TYPE_INTEGER ?
                 dynamic_cast<vidl_v4l2_control_integer *>(pc) : 0 ) : 0;
  }

  //: Get control from driver id
  // \param id is control ID from v4l2 specification. For example, V4L2_CID_BRIGHTNESS
  // \return pointer to control. Null if doesn't exist or is not of type V4L2_CTRL_TYPE_MENU
  vidl_v4l2_control_menu * get_control_menu_id( int id) const
  {
    vidl_v4l2_control *pc= get_control_id(id);
    return pc ? (pc->type()==V4L2_CTRL_TYPE_MENU ?
                 dynamic_cast<vidl_v4l2_control_menu *>(pc) : 0 ) : 0;
  }

  //: Get control from driver id
  // \param id is control ID from v4l2 specification. For example, V4L2_CID_BRIGHTNESS
  // \return pointer to control. Null if doesn't exist or is not of type V4L2_CTRL_TYPE_BOOLEAN
  vidl_v4l2_control_boolean * get_control_boolean_id( int id) const
  {
    vidl_v4l2_control *pc= get_control_id(id);
    return pc ? (pc->type()==V4L2_CTRL_TYPE_BOOLEAN ?
                 dynamic_cast<vidl_v4l2_control_boolean *>(pc) : 0 ) : 0;
  }

  //: Get control from driver id
  // \param id is control ID from v4l2 specification. For example, V4L2_CID_BRIGHTNESS
  // \return pointer to control. Null if doesn't exist or is not of type V4L2_CTRL_TYPE_BUTTON
  vidl_v4l2_control_button * get_control_button_id( int id) const
  {
    vidl_v4l2_control *pc= get_control_id(id);
    return pc ? (pc->type()==V4L2_CTRL_TYPE_BUTTON ?
                 dynamic_cast<vidl_v4l2_control_button *>(pc) : 0 ) : 0;
  }

  // ----------------- End methods associated to controls -------------------

  //: Start capturing
  // \return if successful
  bool start_capturing ();
  //: Return if device is capturing
  bool is_capturing() const { return capturing; }
  //: Stop capturing
  // \return if successful
  bool stop_capturing();
  //: Read next frame
  // \return if successful
  bool read_frame();

  // ----------------- Methods associated to buffers -------------------

  //: Set numbers of buffers
  // \return if successful
  bool set_number_of_buffers(unsigned int nb);
  //: Get numbers of buffers
  unsigned int get_number_of_buffers() const { return pre_nbuffers; }

  //: Return the last used buffer
  // \pre device is capturing
  void * current_buffer() const {
    return (last_buffer==-1)?0:buffers[last_buffer].start;
  }

  //: Return length of the last used buffer
  // \pre device is capturing
  unsigned int current_buffer_length() const { // return __u32
    return (last_buffer==-1)?0:buffers[last_buffer].buf.length;
  }

  //: Return buffer number i
  // \pre device is capturing
  // \see get_number_of_buffers
  void *ibuffer(unsigned int i) const {
    return buffers && i < n_buffers ? buffers[i].start : 0;
  }

  // Return number in sequence associated to last frame, as indicated by driver
  unsigned int sequence() const { // return __u32
    if (last_buffer==-1) std::cerr << "UPS\n";
    return (last_buffer==-1)?0:buffers[last_buffer].buf.sequence;
  }

  // ----------------- End methods associated to buffers -------------------

  //: Return if driver has time associated to the captured frame
  // \pre device is capturing
  bool time_available() const {
    return (last_buffer==-1)?false:(buffers[last_buffer].buf.flags&V4L2_BUF_FLAG_TIMECODE);
  }

  //: Time from last frame
  // \see time_available
  v4l2_timecode time() const {
    return (last_buffer==-1)?v4l2_timecode():buffers[last_buffer].buf.timecode;
  }

  // --------   Next functions indicate if the device is in a bad state: not usable.

  //: Cast to bool is true if video device is working ok
  explicit operator bool () const
  { return (last_error.size()==0)? true : false; }

  //: Return false if video device is not working ok
  bool operator!() const
  { return (last_error.size()==0)? false : true; }

  //: Return last error if device is in a bad state. Empty if ok
  std::string get_error() const { return last_error; }

  //-------------------------------------------------------
  // reference counting if used through sptr
 public:

  //: Increment reference count
  void ref() { ref_count_++; }

  //: Decrement reference count
  void unref(){
    assert (ref_count_ >0);
    ref_count_--;
    if (ref_count_==0)
    {
      delete this;
    }
  }

  //: Number of objects referring to this data
  int ref_count() const { return ref_count_; }

 private:
  int ref_count_;
};


std::ostream &
operator << (std::ostream &os, const vidl_v4l2_device & dev);


#endif // vidl_v4l2_device_h_
