// This is core/vul/vul_timestamp.h
#ifndef vul_timestamp_h
#define vul_timestamp_h
//:
// \file
// \brief generates a timestamp.
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

//: class to generate a unique timestamp
class vul_timestamp
{
  //: mark is incremented to give a unique timestamp
  static unsigned long mark;

 public:

  //: Constructor
  vul_timestamp() { this->touch(); }
  //: Destructor
  virtual ~vul_timestamp() {}

  //: Get a new timestamp
  void touch() { timestamp_ = get_unique_timestamp(); }
  //: Get a new timestamp (incremented by 1 each time)
  unsigned long get_time_stamp() const { return timestamp_; }

  //: Returns true if t is older than the last timestamp
  bool older(vul_timestamp const& t)const{return timestamp_<t.get_time_stamp();}
  //: Returns true if t is older than the last timestamp
  inline bool older(vul_timestamp const* t) const { return older(*t); }

 protected:
  //: last timestamp
  unsigned long timestamp_;

 private:
  //: get a new timestamp
  static unsigned long get_unique_timestamp() { return mark++; }
};

#endif // vul_timestamp_h
