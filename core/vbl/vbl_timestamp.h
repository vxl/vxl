#ifndef vbl_timestamp_h
#define vbl_timestamp_h

// This is vxl/vbl/vbl_timestamp.h

//:
// \file
// \brief generates a timestamp.

//: class to generate a unique timestamp
class vbl_timestamp
{
public:

  //: Constructor
  vbl_timestamp();
  //: Destructor
  virtual ~vbl_timestamp();
  
  //: Get a new timestamp
  void touch();
  //: Get a new timestamp (incremented by 1 each time)
  unsigned long get_time_stamp() const { return timestamp_; };

  //: Returns true if t is older than the last timestamp
  bool older(vbl_timestamp const& t) const;
  //: Returns true if t is older than the last timestamp
  inline bool older(vbl_timestamp const* t) const { return older(*t); }

protected:
  //: last timestamp
  unsigned long timestamp_;

private:
  //: mark is incremented to give a unique timestamp 
  static unsigned long mark;

  //: get a new timestamp
  static unsigned long get_unique_timestamp();
};

#endif // vbl_timestamp_h
