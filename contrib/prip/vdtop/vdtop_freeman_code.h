// This is contrib/prip/vmap/vdtop_freeman_code.h
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
  	_code = arg ;
  }
  
	vdtop_freeman_code (const vdtop_freeman_code & arg)
    : _code(arg._code)
  {
  }
  
	vdtop_freeman_code & operator= (vdtop_freeman_code arg)
  {
    _code = arg._code ;
    return *this ;
  }
  
	bool operator == (vdtop_freeman_code arg) const
  {
    return _code == arg._code ;
  }
  
	bool operator != (vdtop_freeman_code arg) const
  {
    return _code != arg._code ;
  }
  
	bool is_valid() const
  {
    return _code <= 7 ;
  }
  
  void set_not_valid()
  {
  	_code=255 ;
  }
  
	int di() const
  {
    return _di[_code] ;
  }
	
  int dj() const 
  {
    return _dj[_code] ;
  }
	
  vdtop_freeman_code & operator++()
  {
    ++_code ;
		_code%=8 ;
    return *this ;
  }
	
	vdtop_freeman_code & operator+= (int i)
  {
    _code+=i ;
		_code%=8 ;
    return *this ;
  }
  
	
  vdtop_freeman_code & operator--()
  {
    --_code ;
		_code%=8 ;
    return *this ;
  }
	
  vdtop_freeman_code operator-()
  {
    return _opposite[_code] ;
  }
  vcl_pair<vdtop_freeman_code,vdtop_freeman_code> reduction(vdtop_freeman_code arg)
  {
    vxl_byte m =  _reduction[_code][arg._code];
    return vcl_pair<vdtop_freeman_code,vdtop_freeman_code>(m&0x0F,(m&0xF0) >> 4) ;
  }
  
	/*bool adjacent(vdtop_freeman_code arg)
  {
    return (abs(arg._code-_code)<=1 || abs(arg._code-_code)==7) ;
  }
  bool strictly_adjacent(vdtop_freeman_code arg)
  {
    return (abs(arg._code-_code)==1 || abs(arg._code-_code)==7) ;
  }
	*/
	
  bool parallel(vdtop_freeman_code arg)
    {
      return _normalized[_code]==_normalized[arg._code] ;
    }
	
  bool orthogonal_to(vdtop_freeman_code arg)
    {
      return _are_orthogonal[_code][arg._code] ;
    }
  vdtop_freeman_code orthogonal()
    {
      return vdtop_freeman_code(_orthogonal[_code]) ;
    }
  void normalise()
    {
      _code = _normalized[_code] ;
    }
  vxl_byte mask ()
    {
      return _mask [_code] ;
    }
  bool is_between (vdtop_freeman_code arg1, vdtop_freeman_code arg2) 
    {
      if (arg1._code>arg2._code)
      {
				return _code > arg1._code || _code< arg2._code ;
      }
      return _code > arg1._code && _code< arg2._code ;
    }  
  
	vxl_byte code()
  {
		return _code ;
  }
	
  bool connection4() const
    {
      return _code  == 0 || _code == 2 || _code == 4 || _code == 6 ;
    }
private :
	
  vxl_byte _code  ;
	static const vxl_byte _reduction [8][8] ;
  static const vxl_byte _normalized[8] ;
	static const vxl_byte _opposite[8] ;
	static const vxl_byte _orthogonal[8] ;
	static const bool _are_orthogonal[8][8] ;
	static const vxl_byte _difcd[3][3] ;
	static const vxl_byte _mask [8] ;
	static const int _di[8] ;
	static const int _dj[8] ;

} ;

#endif
