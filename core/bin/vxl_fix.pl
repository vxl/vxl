#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

# Purpose: Pipe a list of source files through the TargetJr-to-vxl
#          conversion filters and replace if necessary.

# Author: Dave Cooper
# Modification: fsm. various.

# find out where we are :
my $IUELOCALROOT=$ENV{'IUELOCALROOT'};


# handy indirection for dry runs.
sub shell {
  my ($arg) = @_;
  #  print STDERR "(shell) $arg\n";
  print STDERR `$arg`;
}

# parse command line :
my @options = ();
my @files = ();
foreach my $arg (@ARGV) {
  if ($arg =~ m/^\-.+$/) { 
    #print STDERR "option: $arg\n";
    push @options, $arg; 
  }
  elsif (-f $arg) { 
    #print STDERR "file: $arg\n";
    push @files, $arg; 
  }
  else {
    print STDERR "no such file : $arg\n";
  }
}
if ($#options == -1) {
  print STDERR "no options given, consider using -vcl\n";
  exit 0;
}

# for each file, $f :
foreach my $f (@files) {
  # report
  print STDERR "examining ", $f, "... ";
  
  # filter, passing arguments through
  &shell("cat $f | $IUELOCALROOT/vxl/bin/vxl_filter.pl @options > $f.filt");
  
  # compare and replace if changed
  &shell("if diff $f $f.filt >/dev/null; then echo \"no change\"; rm -f $f.filt; else echo \"change\"; mv $f $f.bak; mv $f.filt $f; fi");
}
