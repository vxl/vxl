#ifndef vbl_ref_count_h
#define vbl_ref_count_h

class vbl_ref_count
{
public:

  vbl_ref_count();
  virtual ~vbl_ref_count();
  
  // public methods
  // memory management
  void ref() { protected_++; }

  void unref()
	{ protected_--; if(protected_ <= 0) delete this;}

  int get_references() const
	{ return protected_ ; }

  bool is_referenced() const
	{ return (protected_ > 0); }

protected:
  int protected_;		 // reference count

private:
  static int verbosity_;
};

#endif
