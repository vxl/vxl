// This is prip/vdtop/vdtop_freeman_code.h
#ifndef vdtop_freeman_code_h_
#define vdtop_freeman_code_h_
//:
// \file
// \brief Freeman code manipulation.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vxl_config.h>
#include <vcl_utility.h>

//: Freeman code class.
class vdtop_freeman_code
{
 public :
  vdtop_freeman_code (vxl_byte arg=0)
  {
    code_ = arg ;
  }

  vdtop_freeman_code (const vdtop_freeman_code & arg) : code_(arg.code_) {}

  vdtop_freeman_code & operator= (vdtop_freeman_code arg)
  {
    code_ = arg.code_ ;
    return *this ;
  }

  bool operator == (vdtop_freeman_code arg) const
  {
    return code_ == arg.code_ ;
  }

  bool operator != (vdtop_freeman_code arg) const
  {
    return code_ != arg.code_ ;
  }

  bool is_valid() const
  {
    return code_ <= 7 ;
  }

  void set_not_valid()
  {
    code_=255 ;
  }

  int di() const
  {
    return di_[code_] ;
  }

  int dj() const
  {
    return dj_[code_] ;
  }

  vdtop_freeman_code & operator++()
  {
    ++code_ ;
    code_%=8 ;
    return *this ;
  }

  vdtop_freeman_code & operator+= (int i)
  {
    code_+=i ;
    code_%=8 ;
    return *this ;
  }

  vdtop_freeman_code & operator--()
  {
    --code_ ;
    code_%=8 ;
    return *this ;
  }

  vdtop_freeman_code operator-()
  {
    return opposite_[code_] ;
  }
  vcl_pair<vdtop_freeman_code,vdtop_freeman_code> reduction(vdtop_freeman_code arg)
  {
    vxl_byte m =  reduction_[code_][arg.code_];
    return vcl_pair<vdtop_freeman_code,vdtop_freeman_code>(m&0x0F,(m&0xF0) >> 4) ;
  }
#if 0
  bool adjacent(vdtop_freeman_code arg)
  {
    return vcl_abs(arg.code_-code_)<=1 || vcl_abs(arg.code_-code_)==7 ;
  }
  bool strictly_adjacent(vdtop_freeman_code arg)
  {
    return vcl_abs(arg.code_-code_)==1 || vcl_abs(arg.code_-code_)==7 ;
  }
#endif // 0

  bool parallel(vdtop_freeman_code arg)
  {
    return normalized_[code_]==normalized_[arg.code_] ;
  }

  bool orthogonal_to(vdtop_freeman_code arg)
  {
    return are_orthogonal_[code_][arg.code_] ;
  }
  vdtop_freeman_code orthogonal()
  {
    return vdtop_freeman_code(orthogonal_[code_]) ;
  }
  void normalise()
  {
    code_ = normalized_[code_] ;
  }
  vxl_byte mask ()
  {
    return mask_ [code_] ;
  }
  bool is_between (vdtop_freeman_code arg1, vdtop_freeman_code arg2)
  {
    if (arg1.code_>arg2.code_)
    {
      return code_ > arg1.code_ || code_< arg2.code_ ;
    }
    return code_ > arg1.code_ && code_< arg2.code_ ;
  }

  vxl_byte code()
  {
    return code_ ;
  }

  bool connection4() const
  {
    return code_ == 0 || code_ == 2 || code_ == 4 || code_ == 6 ;
  }
 private :

  vxl_byte code_ ;
  static const vxl_byte reduction_ [8][8] ;
  static const vxl_byte normalized_[8] ;
  static const vxl_byte opposite_[8] ;
  static const vxl_byte orthogonal_[8] ;
  static const bool are_orthogonal_[8][8] ;
  static const vxl_byte difcd_[3][3] ;
  static const vxl_byte mask_ [8] ;
  static const int di_[8] ;
  static const int dj_[8] ;
};

#endif
