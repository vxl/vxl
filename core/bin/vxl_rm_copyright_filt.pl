#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

#========================================================
# remove GE copyright notice
#========================================================

$startpatt = "<begin copyright notice>";
$endpatt = "<end copyright notice>";
$begin = 0;

while (<>)
{
  if ( /$startpatt/ )
  {
    $begin = 1;
    next;
  }

  if ( /$endpatt/ )
  {
    $begin = 0;
    next;
  }

  if ( $begin )
  {
    next;
  }
  else
  {
    print;
  }
}

#========================================================
