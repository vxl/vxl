#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

#======================================================
# install tiff package (default configuration)

#======================================================

$make="make";
$configparams = "--noninteractive";

$prefix="/freeware";

# Hack config script to fix installation prefix
if (! -f "configure.orig" ) {
  rename("configure", "configure.orig") || die "I want to be run from the tiff dir\n";
}
open(CONFIGURE_orig, "configure.orig")  || die "I want to be run from the tiff dir\n";
open(CONFIGURE, ">configure")  || die "cannot write to ./configure\n";
while (<CONFIGURE_orig>) {
  s!DIR_BIN=/usr/local!DIR_BIN=$prefix!;
  s!DIR_LIB=/usr/local!DIR_LIB=$prefix!;
  s!DIR_INC=/usr/local!DIR_INC=$prefix!;
  s!DIR_MAN=/usr/local!DIR_MAN=$prefix!;
  s!/usr/local/man!$prefix/man!;

  print CONFIGURE;
}
close(CONFIGURE);
close(CONFIGURE_orig);

xec("echo yes | sh ./configure $configparams");
xec("$make");
xec("$make install");

#======================================================


#======================================================
# execute command and check for errors

sub xec
{
  $command = $_[0];
  print "xec \"$command\"\n";
  $op = `$command`;
  print $op;
  if  ($? != 0 )
  {
    print "command <$command> failed\n";
    die;
  }
}

