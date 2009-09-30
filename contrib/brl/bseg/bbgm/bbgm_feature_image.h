// This is brl/bseg/bbgm/bbgm_feature_image.h
#ifndef bbgm_feature_image_h_
#define bbgm_feature_image_h_

//:
// \file
// \brief A templated image of features with associated probabilities
// \author J.L. Mundy
// \date May 3, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_loader.h>

//: base class for images of features
class bbgm_feature_image_base : public vbl_ref_count
{
  public:
    virtual ~bbgm_feature_image_base(){}

    //: Binary save self to stream.
    virtual void b_write(vsl_b_ostream &os) const=0;

    //: Binary load self from stream.
    virtual void b_read(vsl_b_istream &is)=0;

    virtual vcl_string is_a() const=0;

    virtual bbgm_feature_image_base* clone() const = 0;
    
    virtual unsigned int ni() const = 0;
    virtual unsigned int nj() const = 0;
};


template<class f_type_>
class bbgm_feature_image : public bbgm_feature_image_base
{
 public:
  //: Constructor
  bbgm_feature_image<f_type_>(){};
  bbgm_feature_image<f_type_>(unsigned int ni, unsigned int nj): data_(nj,ni){}
  bbgm_feature_image<f_type_>(unsigned int ni, unsigned int nj,
                        const f_type_& feature) : data_(nj,ni,feature) {}


  //: Return the width of the image
  unsigned int ni() const { return data_.cols(); }

  //: Return the height
  unsigned int nj() const { return data_.rows(); }

  //: resize to ni x nj
  // If already correct size, this function returns quickly
  void set_size(unsigned ni, unsigned nj) { data_.resize(nj,ni); }

  //: Read only access to the features
  const f_type_& operator() (unsigned int i, unsigned int j) const
  { return data_(j,i); }

  //: Access to the features
  f_type_& operator() (unsigned int i, unsigned int j)
  { return data_(j,i); }

  //: Set the distribution at (i,j) to a copy of d
  void set(unsigned int i, unsigned int j, const f_type_& d)
  { data_(j,i) = d; }

  //: An iterator over the distribution in the image
  class iterator
  {
    public:
      iterator(f_type_* ptr) : ptr_(ptr) {}
      iterator(const iterator& other) : ptr_(other.ptr_) {}
      void operator++() { ++ptr_; }
      f_type_& operator*(){ return *ptr_; }
      f_type_* operator -> () { return ptr_; }
      bool operator==(const iterator& other) { return ptr_ == other.ptr_; }
      bool operator!=(const iterator& other) { return ptr_ != other.ptr_; }

    private:
      f_type_* ptr_;
  };

  class const_iterator
  {
    public:
      const_iterator(const f_type_* ptr) : ptr_(ptr) {}
      const_iterator(const const_iterator& other) : ptr_(other.ptr_) {}
      void operator++() { ++ptr_; }
      const f_type_& operator*(){ return *ptr_; }
      const f_type_* operator -> () { return ptr_; }
      bool operator==(const const_iterator& other) { return ptr_ == other.ptr_; }
      bool operator!=(const const_iterator& other) { return ptr_ != other.ptr_; }

    private:
      const f_type_* ptr_;
  };

  //: Return an iterator to the first element
  iterator begin() { return iterator(data_[0]); }
  //: Return an iterator to one past the last element
  iterator end() { return iterator(data_[0]+data_.size()); }
  //: Return a const iterator to the first element
  const_iterator begin() const { return const_iterator(data_[0]); }
  //: Return a const iterator to one past the last element
  const_iterator end() const { return const_iterator(data_[0]+data_.size()); }

  //===========================================================================
  // Binary I/O Methods

  //: Return a string name
  // \note this is probably not portable
  virtual vcl_string is_a() const;

  virtual bbgm_feature_image_base* clone() const;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

 private:
  //: the data
   vbl_array_2d<f_type_ > data_;

};

template <class f_type_>
void vsl_print_summary(vcl_ostream& os,
                       const bbgm_feature_image<f_type_> & b)
{ os << "not yet implemented for bbgm_feature_image\n";}


//: Binary save bbgm_feature_image
template <class f_type_>
void vsl_b_write(vsl_b_ostream &os, const bbgm_feature_image<f_type_>& b)
{
  b.b_write(os);
}

//: Binary load bbgm_feature_image
template <class f_type_>
void vsl_b_read(vsl_b_istream &is, bbgm_feature_image<f_type_>& b)
{
  bbgm_feature_image<f_type_> temp;
  temp.b_read(is);
  b = temp;
}


//: Add an instance to the binary loader
void vsl_add_to_binary_loader(bbgm_feature_image_base const& b);


#endif // bbgm_feature_image_h_
