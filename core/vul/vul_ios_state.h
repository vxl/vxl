// This is core/vul/vul_ios_state.h
#ifndef vul_ios_state_h_
#define vul_ios_state_h_
//:
// \file
// \brief saves and restores stream state
// \author Ian Scott, Imorphics.
// \date   03 April 2007
//
// Modifications are subject to the VXL license.
//
// Copied from http://www.boost.org/boost/io/ios_state.hpp
//
//  Original:
//  Copyright 2002, 2005 Daryle Walker.  Use, modification, and distribution
//  are subject to the Boost Software License, Version 1.0.  (See accompanying
//  file LICENSE_1_0.txt or a copy at <http://www.boost.org/LICENSE_1_0.txt>.)
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <ios>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Use RAII to save and restore precision and other state on an iostream
class vul_ios_state_saver
{
public:
  explicit vul_ios_state_saver(std::ios_base & s)
    : stream_(s)
    , flags_(s.flags())
    , precision_(s.precision())
    , width_(s.width())
  {}
  ~vul_ios_state_saver() { this->restore(); }

  void
  restore()
  {
    stream_.width(width_);
    stream_.precision(precision_);
    stream_.flags(flags_);
  }

private:
  std::ios_base & stream_;
  const std::ios::fmtflags flags_;
  const std::streamsize precision_;
  const std::streamsize width_;
};

#endif
