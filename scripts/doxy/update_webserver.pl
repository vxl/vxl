#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6
# If Windows barfs at line 3 here, you will need to run perl -x this_file.pl
# You can set up as a permanent file association using the following commands
#  >assoc .pl=PerlScript
#  >ftype PerlScript=C:\Perl\bin\Perl.exe -x "%1" %*


use Cwd;
use Getopt::Std;
use File::Basename;

#---------------------------------------------------------------------
#   update_webserver.pl -v vxlsrc -s script_dir -l ctrl_file [-o outputdir] -u -f
#  -u : Update cvs
#  -f : Force build
#---------------------------------------------------------------------

sub xec
{
    my $command = $_[0];
    print "exec [$command]\n";
    my $op = `$command`;
    print $op;
    if ( $? != 0 )
    {
        print "<$command> failed\n";
        exit(1);
    }
}

#-----------------------------------------------------------
#-----------------------------------------------------------
# Main
#-----------------------------------------------------------

my %options;
getopts('v:s:l:o:fu', \%options);

my $vxlsrc = $options{v} || "";
my $script_dir = $options{s} || "$vxlsrc/scripts/doxy";
my $ctrl_list = $options{l} || "$script_dir/data/library_list.txt";
my $doxydir = $options{o} || "$vxlsrc/Doxy";

$forcedflag="";
if (defined($options{f}))
{
  $forcedflag="-f";
}

$cvsupflag="";
if (defined($options{u}))
{
  $cvsupflag="-u";
}

if ($vxlsrc eq "")
{
  print "Update documentation files on webserver.\n";
    print "syntax is:\n";
    print "update_webserver.pl -v VXLSRC -s script_dir -l ctrl_file -o outputdir -u -f\n\n";
    print "  where VXLSRC is the path to the root of the vxl source tree.\n";
    print "  ie the directory which contains vcl,vxl,mul etc\n\n";
    print "  outputdir (default VXLSRC/Doxy) indicates where the documentation \n";
    print "  is to be placed.\n";
    print "  script_dir gives the location of the scripts\n";
    print "    default is VXLSRC/scripts/doxy\n";
    print "  ctrl_file is the control file . If not \n";
    print "  supplied, assumed to be script_dir/data/library_list.txt\n";
    print "  Other options:\n";
    print "  -u : Update cvs\n";
    print "  -f : Force build\n";
    exit(1);
}

# Check that a directory called $doxydir exists
if (! -e $doxydir)
{
  print "Creating $doxydir\n";
  mkdir $doxydir,0777 || die "Can't create directory $doxydir\n";
}

$doxyoutputdir = $doxydir . "/output";

# Check that a subdirectory called Doxy/output exists
if (! -e $doxyoutputdir)
{
  print "Creating $doxyoutputdir\n";
  mkdir $doxyoutputdir,0777 || die "Can't create directory $doxyoutputdir\n";
}


# Update the data directory
if ($cvsupflag)
{
  ($ctrl_list_file,$ctrl_list_dir) = fileparse($ctrl_list);

  chdir $ctrl_list_dir || die "Unable to chdir to $ctrl_list_dir\n";

  print "Checking for update of library lists etc.\n";
  $changes = `cvs -q up -d -A $ctrl_list_file`;

  if ($changes)
  {
    print "Lists have changed, so forcing rebuild of all documentation\n";
    $forcedflag="-f";
  }

  # Check out all relevant packages
  $cop_cmd = "$script_dir/checkout_packages.pl -v $vxlsrc -l $ctrl_list";
  xec("$cop_cmd > $doxyoutputdir/checkout_packages.out");
}


# Update all documentation
$params = "-v $vxlsrc -s $script_dir -o $doxydir -l $ctrl_list $forcedflag $cvsupflag";

xec("$script_dir/build_all_doc.pl $params > $doxyoutputdir/build_all_doc.out");
