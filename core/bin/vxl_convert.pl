#! /bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

# Purpose: Pipe a list of source files through the TargetJr-to-vxl
#          conversion filters and replace if necessary.
#
# usage: vxl_convert.pl [options] [file1 [file2 [...]]]
#
# options: -n     dry run (print commands, but don't execute).
#          -diff  show diffs.

# Author: Dave Cooper
# Modification: fsm. various.

#--------------------------------------------------------------------------------

# global variables

# where we are
my $IUELOCALROOT=$ENV{'VXLROOT'};
$IUELOCALROOT =~ s+[/\\]core[/\\]?$++ if $IUELOCALROOT;
$IUELOCALROOT = $ENV{'IUELOCALROOT'} unless $IUELOCALROOT;
$IUELOCALROOT = $ENV{'IUEROOT'} unless $IUELOCALROOT;
$IUELOCALROOT || die "Cannot find \$IUELOCALROOT";
die "Cannot find vxl_filter.pl" unless -f "$IUELOCALROOT/core/bin/vxl_filter.pl";

#
my $dry_run = 0;

#
my $show_diffs = 0;

$mv = "mv";
$rm = "rm -f";
$perl= "perl";

# find a suitable 'diff' command :
$diff = "diff";
if (-f ($diff = "/bin/diff")) { }
elsif (-f ($diff = "/usr/bin/diff")) { }
elsif (-f ($diff = "C:/cygnus/CYGWIN~1/H-I586~1/bin/diff.exe")) {
  $mv = "rename";
  $rm = "del";
  chop($perl = `bash -c 'type -p perl'`);
  $perl =~ s!^/cygdrive/([a-z])/!$1:/!;
# print STDERR "PERL=[$perl]";
}
else { die "no diff found\n"; }

if ($#ARGV>=0 && defined($ARGV[0])) {
  exit &main(@ARGV);
} else {
  print STDERR "no arguments\n";
  exit 0;
}

#--------------------------------------------------------------------------------

# handy indirection for dry runs.
sub shell {
  my ($arg) = @_;
  #print STDERR "[shell: $arg";
  # system("echo " . $arg);
  $ok = system($arg) >> 8;
  die "help [$arg]" if $ok != 0;
  #print STDERR "]\n";

#  if ($dry_run) { print STDERR "$arg\n"; }
#  else { print STDERR `$arg`;
}

# returns 1 if the two given files differ and 0 otherwise
sub files_differ {
  my ($a, $b) = @_;

  # a lot of file stats here...
  die "no such file : '$a'\n" unless -f $a;
  die "no such file : '$b'\n" unless -f $b;

  open(A, "$a") || die;
  open(B, "$b") || die;
  do {
      $aline = <A>;
      $bline = <B>;
      if (!defined($aline) && !defined($bline)) {
        close A; close B; return 0;
      }
      goto fail if (!defined($aline) || !defined($bline));
      if ($aline ne $bline) {
          # print STDERR "$aline >> $bline\n";
          goto fail;
      }
  } until eof A;
  goto fail if ! eof B;
  close A;
  close B;
  return 0;
fail:
  close A;
  close B;
  return 1;
}

# entry point
sub main {
  # parse command line :
  my @options = ();
  my @files = ();
  my (@argv) = @_;
  foreach my $arg (@argv) {
    if ($arg =~ m/^\-.+$/) {
      #print STDERR "option: $arg\n";
      if ($arg eq "-n") { $dry_run = 1; }
      elsif ($arg eq "-diff") { $show_diffs = 1; }
      else { push @options, $arg; }
    }
    elsif (-f $arg) {
      #print STDERR "file: $arg\n";
      if ($arg =~ m/\.bak$/ || $arg =~ m/\.orig$/ ||
          $arg =~ m/\~$/) { print STDERR "ignoring '$arg'\n"; }
      else { push @files, $arg; }
    }
    else {
      print STDERR "no such file : $arg\n";
    }
  }
  if ($#options == -1) {
    print STDERR "no options given, consider using -vcl\n";
  }

  # for each file, $f :
  foreach my $f (@files) {
    # report
    print STDERR "examining ", $f, "... ";

    # filter, passing arguments through and redirecting stdout to $f.filt
    &shell("$perl -x $IUELOCALROOT/core/bin/vxl_filter.pl < $f @options > $f.filt");

    # compare and replace if changed
    if (&files_differ("$f", "$f.filt")) {
        print STDERR "*changed*";
        if ($show_diffs) {
            print STDERR "\n========== diffs for $f ==========\n";
            &shell("$diff $f $f.filt || true");
        }
        if ($dry_run) {
            &shell("$rm $f.filt"); # clean up.
        }
        else {
            if (! -f "$f.orig") {
                &shell("$mv $f $f.orig");  # back up.
            } else {
                unlink($f) || die "cannot unlink $f ";
            }
            &shell("$mv $f.filt $f"); # replace.
        }
    }
    else {
        print STDERR "no change";
        &shell("rm -f $f.filt"); # clean up
    }

    #
    print STDERR "\n";
  }
  return 0;
}
