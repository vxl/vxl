#! /bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl -w
#line 6

#
# fsm@robots.ox.ac.uk
#

while (<>) {
  if (m/^\@([a-zA-Z_0-9\:]+)\s*$/) {
    $orig = $1; $orig =~ s/\s//g;
    $name = $orig; $name =~ s/\:\:/_/g;
    print "// $orig\n";
    print "#ifndef vcl_$name\n";
    print "#define vcl_$name std::$orig\n";
    print "#endif\n";
  } else {
    print;
  }
}
