# #!/bin/sh
# exec perl -x $0 $*
#!perl -w

# Module: doc
# Purpose: Generate makefile documentation from makefiles.
# Author: Andrew W. Fitzgibbon, Oxford RRG
# Modifications:
#    180298 AWF Initial version

print "

			 IUE Makefiles
                         -------------

This documentation for IUE makefiles is automatically generated from
the central makefiles.  It describes the variables used to control the
build process, divided into four categories:

Target
  A make target, such as all, cpp-File.C etc.

Option:
  Something that an ordinary user might want to specify
  on the command line for any invocation of make.


Variable:
  Something that is specified in a user-level makefile

Select:
  Flags which are set in order to access non-default functionality,
  such as OpenGL, SQL etc.


Config:
  A variable that is set at configuration time based
  on the operating system, locally installed software
  versions etc.

Internal:
  Variables used only in the driver makefiles themselves.

The following files in
    ".`pwd`.
"Were used to make this document
    @ARGV

";


@tags = ("Target", "Option", "Variable", "Select", "Config", "Internal");

foreach $tag (@tags) {
    print "================================= $tag =================================";
    print "\n";
    foreach $file (@ARGV) {
	open(FILE, $file);
	$printing = 0;
	while (<FILE>) {
	    if (!m/^#/) {
		print "\n" if $printing;
		$printing = 0;
	    }
	    if ($printing) {
		s/^#+ //;
		print "     $_";
	    }
	    if (m/^# ${tag}: (.*)$/) {
		$printing = 1;
		print "$1\n";
	    }
	}
	close(FILE);
    }
}
