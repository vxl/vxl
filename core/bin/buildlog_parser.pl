#! /bin/sh
# -*- perl -*-
exec perl -I $VXLROOT/core/bin -x $0 ${1+"$@"}
#!perl
#line 6

#
# crossge@crd.ge.com
#
# Modifications:
# 13 May 2001 - Peter Vanroose - several minor fixes:
#      unified search strings for different compilers;
#      consequently removed specific computer names;
#      ad-hoc fix for bug in ParseDate which does not recognise "DST" in date;
#      simplified expressions, e.g. by replacing $currentline with $_;
#      added removal of $IUEROOT absolute path (which is irrelevant).
# 21 May 2001 - Amitha Perera - fixed to work on FreeBSD (temp
#      filenames should not have $s in them in perl, because interpolation
#      happens in the most awkward places.
# 17 Jun 2001 - Peter Vanroose - added test failures as errors,
#      but now ignoring "error in subdirs" messages.
# 18 Jun 2001 - Peter Vanroose - added hyperlinks to warnings.
#

# global modules from CPAN
use Date::Manip qw(ParseDate UnixDate);
use IO::Socket;
use Time::Local;

# local module
use FrostAPI;

# see if we have compress
#   if so, we output to a tmpfile first, then compress it, MIME it and then output it
#   otherwise we just output it to STDOUT
$tmpfilename1= "/tmp/buildlog_parser_${$}_1.tmp";
$tmpfilename2= "/tmp/buildlog_parser_${$}_2.tmp.gz";

if ( -f "/bin/compress" )
  {
    $compress= "/bin/compress -c $tmpfilename1 > $tmpfilename2";
  }

if ( -f "/usr/local/bin/gzip" )
  {
    $compress= "/usr/local/bin/gzip -c $tmpfilename1 > $tmpfilename2";
  }

if ( -f "/bin/gzip" )
  {
    $compress= "/bin/gzip -c $tmpfilename1 > $tmpfilename2";
  }

if ( -f "/freeware/bin/gnu-tools/gzip" )
  {
    $compress= "/freeware/bin/gnu-tools/gzip -c -n -q -9 $tmpfilename1 > $tmpfilename2";
  }

if ( -f "/tmp/iup_opt/bin/gzip" )
  {
    $compress= "/tmp/iup_opt/bin/gzip -c $tmpfilename1 > $tmpfilename2";
  }

# get current date and time
$date= localtime;
# patch for daylight saving time (DST) which is not correctly recognised:
$date =~ s/( ?DST )/ /;
if ($1) {
  $date =~ m/\s(\d\d):/; $hr = $1 - 1; $date =~ s/\s\d\d:/ $hr:/;
}

############
# gmake 3.79

# make[2]: Entering directory `/a/directory/a/directory'
# make[6]: ***
$gmake_enteringdirectory= q/^g?make\[\d+\]: Entering directory \`(.+?)\'\s*$/;
$gmake_leavingdirectory = q/^g?make\[\d+\]: Leaving directory \`(.+?)\'\s*$/;
$gmake_errorindirectory = q/^g?make(\[\d+\])?: \*\*\* \[(x?all|subdirs|.*recurse-subdirs\])/;
$gmake_errorintest = q/^g?make(\[\d+\])?: \*\*\* \[.*\.out\]/;
$gmake_errortestfail = q/Test Summary:.*\*\*\*/;
$gmake_error_segfault = q/^g?make(\[\d+\])?: \[.*\.out\]\s+Error\s+139/;
$gmake_error_assert = q/:\d+: failed assertion /;
$gmake_error = q/^g?make(\[\d+\])?: \*\*\* \[/;

############
# gcc-2.95

# g++ ... -o something.o
# gcc ... -o something.o
# gcc ... -o something.so

############
# FreeBSD

# c++ ... -o something.o
# gcc ... -o something.o
# gcc ... -o something.so

##############
# SGI compiler
# SunPro
# CC ... -o something.o
# cc ... -o something.o
# CC ... -o something.so
#  "rpoly.c", line 111: warning(1194): floating-point value does not fit in
#  ld: WARNING 127:

$compilingcpp   = q/^[cg]?[+cC][+cC]\s.*\s\-o\s(\S+\.o)\b/;
$compilingc     = q/^g?cc\s.*\s\-o\s(\S+\.o)\b/;
$linkingso      = q/^[cg]?[+Cc][+Cc]\s.*\s\-o\s(\S+\.s[ol])\b/;
$compilewarning = q/^[^:]+:?(, line )?\d+: [Ww]arning\b/;
$sgi_linkwarning= q/ld: WARNING \d+:/;
#$sgi_compilewarning = q/^\".+?\", line \d+: warning/;
$linkwarning    = $sgi_linkwarning;
$testsummary    = q/ Test Summary: /;
$iueroot = $ENV{"IUEROOT"};

###############
# status flags

open(INFO,"-");

$starttime="";
$endtime="";
@current= {};
$index= 0;
$nextindex= 0;
$htmlerrorlink= 0;
$htmlwarninglink= 0;

### build stats
%allbuilds = ();
@buildname= [];
@buildfilenumber = [];
@buildwarnings = [];
@builderrors = [];
@buildparent= [];
@buildchildrenwarnings = [];
@buildchildrenerrors   = [];
@buildlogs= [];
$fullbuildlog="<a href=\"\#ERRORLINK$htmlerrorlink\"><font size=-1 color=\"CC3333\">Jump to first error</font></a><br>\n";
$fullbuildlog.="<a href=\"\#WARNINGLINK$htmlwarninglink\"><font size=-1 color=\"773333\">Jump to first warning</font></a><br>\n";
#%buildtime = ();

########################
# get the current machine name ...
@uname = split(/\s/,<INFO>);
$machine = $uname[1];
$machine =~ s/\..*//;

#print "Machine: $machine\n";
#print "Date   : $date\n";

############
# main loop

while ( <INFO>)
  {
    # patch together multi-lines
    $_ .= <INFO> while ( s/\\\s*$//);
    s/$iueroot\/?/\//g if ( $iueroot);

    $currentlineweb= webify_string($_);

    if ( m/$gmake_enteringdirectory/)
      {
        $currentlineweb="<font color=\"555500\">$currentlineweb</font>";

        push( @current, $1);

        if ( !exists( $allbuilds{$1}))
          {
            $nextindex=$nextindex+1;
            $allbuilds{$1}= $nextindex;
            $index= $nextindex;
            $buildname[$index]= $1;
          }
        else
          {
            $index= $allbuilds{$1};
          }
      }
    elsif ( m/$gmake_leavingdirectory/)
      {
        $currentlineweb="<font color=\"555500\">$currentlineweb</font>";
        pop( @current);
        $index= $allbuilds{$current[$#current]};
      }
    elsif ( m/$compilingcpp/)
      {
        $buildfilenumber[$index]++;
        $currentlineweb="<font color=blue>$currentlineweb</font>";
      }
    elsif ( m/$compilingc/)
      {
        $buildfilenumber[$index]++;
        $currentlineweb="<font color=blue>$currentlineweb</font>";
      }
    elsif ( m/$linkingso/)
      {
#       print "so file: $1\n";
      }
    elsif ( m/$compilewarning/ || m/$linkwarning/)
      {
        $buildwarnings[$index]++;
        $currentlineweb="<a name=\"WARNINGLINK$htmlwarninglink\">" .$currentlineweb;
        $currentlineweb="<font color=\"AA0000\">$currentlineweb</font>";
        $htmlwarninglink++;
        $t_1link="\&nbsp\;<a href=\"\#WARNINGLINK" . ($htmlwarninglink-2);
        $t10link="\&nbsp\;<a href=\"\#WARNINGLINK" . ($htmlwarninglink+9);
        $t_10link="\&nbsp\;<a href=\"\#WARNINGLINK" . ($htmlwarninglink-11);
        $t100link="\&nbsp\;<a href=\"\#WARNINGLINK" . ($htmlwarninglink+99);
        $t_100link="\&nbsp\;<a href=\"\#WARNINGLINK" . ($htmlwarninglink-101);
        $fnt="<font size=-1 color=\"773333\">"; $efnt="</font></a>\n";
        $currentlineweb.="<a href=\"\#WARNINGLINK$htmlwarninglink\">$fnt Jump to next warning$efnt";
        $currentlineweb.="$t_1link\">$fnt -1$efnt" if ($htmlwarninglink > 1);
        $currentlineweb.="$t10link\">+$fnt 10$efnt";
        $currentlineweb.="$t_10link\">-$fnt 10$efnt" if ($htmlwarninglink > 10);
        $currentlineweb.="$t100link\">+$fnt 100$efnt";
        $currentlineweb.="$t_100link\">-$fnt 100$efnt" if ($htmlwarninglink > 100);
        $currentlineweb.="<br>\n";
      }
    elsif ( m/$gmake_errorintest/ || m/$gmake_errortestfail/)
      {
        $builderrors[$index]++;
        $currentlineweb="<a name=\"ERRORLINK$htmlerrorlink\">" .$currentlineweb;
        $currentlineweb.="<font color=red>$currentlineweb</font>";
        $htmlerrorlink++;
        $currentlineweb.="<a href=\"\#ERRORLINK$htmlerrorlink\"><font size=-1 color=\"CC3333\">Jump to next error</font></a><br>\n";
      }
    elsif ( m/$gmake_errorindirectory/)
      {
        $buildwarnings[$index]++;
        $currentlineweb="<font color=\"AA0000\">$currentlineweb</font>";
      }
    elsif ( m/$gmake_error_segfault/ || m/$gmake_error_assert/ || m/$gmake_error/)
      {
        $builderrors[$index]++;
        $currentlineweb="<a name=\"ERRORLINK$htmlerrorlink\">" .$currentlineweb;
        $currentlineweb.="<font color=red>$currentlineweb</font>";
        $htmlerrorlink++;
        $currentlineweb.="<a href=\"\#ERRORLINK$htmlerrorlink\"><font size=-1 color=\"CC3333\">Jump to next error</font></a><br>\n";
      }
    elsif ( m/$testsummary/)
      {
        $currentlineweb="<font color=\"00AA00\">$currentlineweb</font>";
      }
    elsif ( m/Beginning TargetJr make:\s*(.*)$/)
      {
        $f = $1; $f =~ s/( ?DST )/ /;
        if ($1) {
          $f =~ m/\s(\d\d):/; $hr = $1 - 1; $f =~ s/\s\d\d:/ $hr:/;
          $currentlineweb =~ s/ ?DST / /;
        }
        @thisdate= ParseDate( $f);
        $starttime= UnixDate( @thisdate, "%a %b %e %H:%M:%S %z %Y") unless ($starttime);
      }
    elsif ( m/Done TargetJr make:\s*(.*)$/)
      {
        $f = $1; $f =~ s/( ?DST )/ /;
        if ($1) {
          $f =~ m/\s(\d\d):/; $hr = $1 - 1; $f =~ s/\s\d\d:/ $hr:/;
          $currentlineweb =~ s/ ?DST / /;
        }
        @thisdate= ParseDate( $f);
        $endtime= UnixDate( @thisdate, "%a %b %e %H:%M:%S %z %Y");
      }

    if ( $index)
      {
        $buildlogs[$index].= $currentlineweb;
      }

    $fullbuildlog.= $currentlineweb;
  }

$t_1link="\&nbsp\;<a href=\"\#WARNINGLINK" . ($htmlwarninglink-1);
$t_10link="\&nbsp\;<a href=\"\#WARNINGLINK" . ($htmlwarninglink-10);
$t_100link="\&nbsp\;<a href=\"\#WARNINGLINK" . ($htmlwarninglink-100);
$fnt="<font size=-1 color=\"773333\">"; $efnt="</font></a>\n";
$fullbuildlog.="<a name=\"ERRORLINK$htmlerrorlink\"><a href=\"\#ERRORLINK0\">"
             . "<font size=-1 color=\"CC3333\">No more errors.</font></a><br>\n"
             . "<a name=\"WARNINGLINK$htmlwarninglink\"><a href=\"\#WARNINGLINK0\">"
             . "$fnt Jump to first warning$efnt";
$fullbuildlog.="$t_1link\">$fnt -1$efnt" if ($htmlwarninglink > 0);
$fullbuildlog.="$t_10link\">-$fnt 10$efnt" if ($htmlwarninglink > 9);
$fullbuildlog.="$t_100link\">-$fnt 100$efnt" if ($htmlwarninglink > 99);
$fullbuildlog.="<br>\n";

##############################################
# find the common sub-directory for each build

$subdir=-1;

for $build ( keys %allbuilds )
  {
    if ( $subdir== -1)
      {
        $subdir= $build;
      }
    else
      {
        ($subdir)=(($subdir."_FOO_".$build) =~ /^(.*).*_FOO_\1.*$/);
      }
  }

#############################################################################
# and remove the sub-directory (and leading and following /s) from each build

%allbuildsnew= ();

for $build ( keys %allbuilds )
  {
    $buildold= $build;
    $build=~ s/^$subdir\/*//;
    $build=~ s/\/$//;

    $allbuildsnew{$build}= $allbuilds{$buildold};
    $buildname[$allbuilds{$buildold}]= $build;
  }

%allbuilds= %allbuildsnew;

#######################
# compute the hierarchy

for $buildit1 ( keys %allbuilds )
  {
    # remove the last subdirectory from this build
    $index  = $allbuilds{$buildit1};
    $buildit1=~ s/\/*[^\/]+$//;

    for $buildit2 ( keys %allbuilds )
      {
        if ( $buildit1 eq $buildit2)
          {
            $buildparent[$index]= $allbuilds{$buildit2};
          }
      }
  }

########################################
# propogate errors from children upwards
$toterrors= 0;
$totwarnings= 0;
$totfiles= 0;
$totstarttime= $starttime;

for $build ( keys %allbuilds )
  {
    if (( $build!~ /\//) & ( $build!~ /^\s*$/))
      {
        $errors= 0;
        $warnings= 0;

        for $buildit ( keys %allbuilds )
          {
            if ( $buildit =~ /^$build/)
              {
                $errors+= $builderrors[$allbuilds{$buildit}];
                $warnings+= $buildwarnings[$allbuilds{$buildit}];
                $files+= $buildfilenumber[$allbuilds{$buildit}];
              }
          }

        $builderrors[$allbuilds{$build}]= $errors;
        $buildwarnings[$allbuilds{$build}]= $warnings;
        $toterrors+= $errors;
        $totwarnings+= $warnings;
        $totfiles+= $files;
      }
  }

($ds, $Ms, $ys, $hs,$ms,$ss)= UnixDate( ParseDate( $starttime), "%d", "%m", "%y", "%H", "%M", "%S");
($de, $Me, $ye, $he,$me,$se)= UnixDate( ParseDate( $endtime)  , "%d", "%m", "%y", "%H", "%M", "%S");

#print STDERR "Start time = $starttime\n";
#print STDERR "End time   = $endtime\n";

$Ms --;  $Me --;
$as= timelocal( $ss, $ms, $hs, $ds, $Ms, $ys);
$ae= timelocal( $se, $me, $he, $de, $Me, $ye);

$tottime= int(($ae-$as)/60);

##################
### OUTPUT THE XML

#StartFrost( "icehouse", "vxl", "vxluser", "");


if ( $compress ne "")
  {
    FrostAPI::StartFrost( "FILE", $tmpfilename1);
  }


FrostAPI::SetMachine( $machine);

FrostAPI::StartRunTestGroup( "root", $date);
FrostAPI::StartRunTest( "BuildStats", $date);
FrostAPI::RunMeasurement( "Errors", $toterrors);
FrostAPI::RunMeasurement( "Warnings", $totwarnings);
FrostAPI::RunMeasurement( "FileCount", $totfiles);
FrostAPI::RunMeasurement( "TimeStarted", $totstarttime);
FrostAPI::RunMeasurement( "TimeElapsed", $tottime);
FrostAPI::RunMeasurementBase64( "BuildLog", $fullbuildlog);

if ( $toterrors>0)
  {
    FrostAPI::EndRunTest( "f");
  }
else
  {
    FrostAPI::EndRunTest( "t");
  }


FrostAPI::EndRunTestGroup();


$headindex= $allbuilds{""};
@roots= get_children( $headindex);

for ( $i=0; $i<= $#roots; $i++)
  {
    XMLruntestgroup( $roots[$i]);
  }

if ( $compress ne "")
  {
    FrostAPI::EndFrost();

    system( $compress);

    open( CP, $tmpfilename2);

    $buffer= "";

    while ( read( CP, $c, 1))
      {
        $buffer.= $c;
      }

    print MIME::Base64::encode( $buffer);

    unlink( $tmpfilename1);
    unlink( $tmpfilename2);
  }

#print webify_string("hello\nthis is \"a\" test")."\n";

###################
####################
#####################
######################
######## sub-routines
######################
#####################
####################
###################

sub webify_string
  {
    my ($string)= @_;
    my $i;
    my $n= "";
    my $c;

    for ( $i= 0; $i<= length($string); $i++)
      {
        $c= substr($string,$i,1);

        if ( $c eq "\r")
          {
            $c= "";
          }
        elsif ( $c eq "\n")
          {
            $c= "<br>\n";
          }
#        elsif ( $c eq  "\"")
#          {
#            $c= "&quot;";
#          }
        elsif ( $c eq  "&")
          {
            $c= "&amp;";
          }
        elsif ( $c eq  "<")
          {
            $c= "&lt;";
          }
        elsif ( $c eq  ">")
          {
            $c= "&gt;";
          }

        $n.= $c;
      }

    return $n;
  }


sub get_depth
  {
    my ($index)= @_;
    my $i= 0;


    while ( $buildparent[$index]!= $index)
      {
        $i++;
        $index= $buildparent[$index];
      }

    return $i;
  }

sub get_children
  {
    ############################
    # return an indexes children
    my ($index)= @_;
    my $i;
    my @children;

    @pr= @buildparent;

    for ( $i= 0; $i<= $#pr; $i++)
      {
        if (( $pr[$i]== $index) &
           ( $i!= $index))
          {
            push( @children, $i);
          }
      }

    return @children;
  }


sub display
  {
    ######################
    # display some results
    my $i;
    my $j;

    for $i ( keys %allbuilds )
      {
        print "name = $i ".$allbuilds{$i}." Parent = "
          .$buildparent[$allbuilds{$i}]." Warnings = "
            .($buildwarnings[$allbuilds{$i}])." Errors = "
              .($builderrors[$allbuilds{$i}])." Files = "
                .($buildfilenumber[$allbuilds{$i}])." ";

        @children= get_children( $allbuilds{$i});
        print "Children = ";
        for ( $j= 0; $j<= $#children; $j++)
          {
            print $children[$j]." ";
          }
        print  "\n";
      }
  }


########################################
# output the gauges and XML header stuff

sub XMLruntestgroup
  {
    my ($index)= @_;
    my $name= $buildname[$index];
    my @children;
    my $i;

    FrostAPI::StartRunTestGroup( $name, $date);

      {
        if ( $name=~ /\//)
          {
            FrostAPI::StartRunTest( "BuildDirectory", $date);
          }
        else
          {
            FrostAPI::StartRunTest( "BuildLibrary", $date);
          }

        # errors
        FrostAPI::RunMeasurement( "Errors",($builderrors[$index]+0));

        # warnings
        FrostAPI::RunMeasurement( "Warnings", ($buildwarnings[$index]+0));

        # build log
#        if ( $buildlogs[$index] ne "")
#          {
#            FrostAPI::RunMeasurementBase64( "BuildLog", $buildlogs[$index]);
#          }

        # file count
#       RunMeasurement( "BuildFileNumber", ($buildfilenumber[$index]+0));

        # start time
#       RunMeasurement( "BuildStartTime", $starttime);

        if ( $builderrors[$index]== 0)
          {
            FrostAPI::EndRunTest("t");
          }
        else
          {
            FrostAPI::EndRunTest("f");
          }
      }

    @children= get_children( $index);

    for ( $i=0; $i<=$#children; $i++)
      {
        XMLruntestgroup( $children[$i]);
      }

    FrostAPI::EndRunTestGroup( $date);
  }
