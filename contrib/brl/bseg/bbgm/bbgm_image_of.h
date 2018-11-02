// This is brl/bseg/bbgm/bbgm_image_of.h
#ifndef bbgm_image_of_h_
#define bbgm_image_of_h_
//:
// \file
// \brief An image of distributions (templated)
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date January 27, 2006

#include <iostream>
#include <typeinfo>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_loader.h>

//: base class for images of distributions
class bbgm_image_base : public vbl_ref_count
{
  public:
    ~bbgm_image_base() override= default;

    //: return the type_info for the distribution type
    virtual const std::type_info& dist_typeid() const=0;

    //: Binary save self to stream.
    virtual void b_write(vsl_b_ostream &os) const=0;

    //: Binary load self from stream.
    virtual void b_read(vsl_b_istream &is)=0;

    virtual std::string is_a() const=0;

    virtual bbgm_image_base* clone() const = 0;
};


//: An image of distributions
template<class dist>
class bbgm_image_of : public bbgm_image_base
{
 public:
  //: Constructor
  bbgm_image_of<dist>() = default;
  bbgm_image_of<dist>(unsigned int ni, unsigned int nj,
                      const dist& model) : data_(nj,ni,model) {}

  //: return the type_info for the distribution type
  const std::type_info& dist_typeid() const override { return typeid(dist); }

  //: Return the width of the image
  unsigned int ni() const { return data_.cols(); }

  //: Return the height
  unsigned int nj() const { return data_.rows(); }

  //: resize to ni x nj
  // If already correct size, this function returns quickly
  void set_size(unsigned ni, unsigned nj) { data_.resize(nj,ni); }

  //: Read only access to the distributions
  const dist& operator() (unsigned int i, unsigned int j) const
  { return data_(j,i); }

  //: Access to the distributions
  dist& operator() (unsigned int i, unsigned int j)
  { return data_(j,i); }

  //: Set the distribution at (i,j) to a copy of d
  void set(unsigned int i, unsigned int j, const dist& d)
  { data_(j,i) = d; }

  //: An iterator over the distribution in the image
  class iterator
  {
    public:
      iterator(dist* ptr) : ptr_(ptr) {}
      iterator(const iterator& other) : ptr_(other.ptr_) {}
      iterator& operator++() { ++ptr_; return *this; }
      dist& operator*(){ return *ptr_; }
      dist* operator -> () { return ptr_; }
      bool operator==(const iterator& other) { return ptr_ == other.ptr_; }
      bool operator!=(const iterator& other) { return ptr_ != other.ptr_; }

    private:
      dist* ptr_;
  };

  class const_iterator
  {
    public:
      const_iterator(const dist* ptr) : ptr_(ptr) {}
      const_iterator(const const_iterator& other) : ptr_(other.ptr_) {}
      const_iterator& operator++() { ++ptr_; return *this; }
      const dist& operator*(){ return *ptr_; }
      const dist* operator -> () { return ptr_; }
      bool operator==(const const_iterator& other) { return ptr_ == other.ptr_; }
      bool operator!=(const const_iterator& other) { return ptr_ != other.ptr_; }

    private:
      const dist* ptr_;
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
  std::string is_a() const override;

  bbgm_image_base* clone() const override;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const override;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is) override;

 private:
  //: the data
   vbl_array_2d<dist> data_;
};


//: Add an instance to the binary loader
void vsl_add_to_binary_loader(bbgm_image_base const& b);


#endif // bbgm_image_of_h_
