#! /bin/sh
# -*- perl -*-
exec perl -I $IUEROOT/vxl/bin -x $0 ${1+"$@"}
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
#

# global modules from CPAN
use Date::Manip qw(ParseDate UnixDate);
use IO::Socket;
use Time::Local;

# local module
use FrostAPI;

$date= localtime;

############
# gmake 3.79

# make[2]: Entering directory `/a/directory/a/directory'
# make[6]: ***
$gmake_enteringdirectory= q/^g?make\[\d+\]: Entering directory \`(.+?)\'\s*$/;
$gmake_leavingdirectory = q/^g?make\[\d+\]: Leaving directory \`(.+?)\'\s*$/;
$gmake_errorindirectory = q/^g?make\[\d+\]: \*\*\* \[/;

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

$compilingcpp   = q/^[cg]?[+cC][+cC]\s.*\s\-o\s(\S+\.o)/;
$compilingc     = q/^g?cc\s.*\s\-o\s(\S+\.o)/;
$linkingso      = q/^[cg]?[+Cc][+Cc]\s.*\s\-o\s(\S+\.so)/;
$compilewarning = q/[^:]+:\d+: [Ww]arning/;
$sgi_linkwarning= q/ld: WARNING \d+:/;
#$sgi_compilewarning = q/^\".+?\", line \d+: warning/;
$linkwarning    = $sgi_linkwarning;
$iueroot = $ENV{"IUEROOT"};

###############
# status flags

open(INFO,"-");

$starttime="";
$endtime="";
@current= {};
$index= 0;
$nextindex= 0;

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
$fullbuildlog= "";
#%buildtime = ();

########################
# get the current machine ...
@uname = split(/\s/,<INFO>);
$machine = $uname[1];
$machine =~ s/\..*//;

#print "Machine: $machine\n";
#print "Date   : $date\n";

############
# main loop

while( $in=<INFO>)
  {
    $_ = "$in";
    # patch together multi-lines
    $_ .= <INFO> while( s/\\\s*$//);
    s/$iueroot//g if( $iueroot);

    $currentlineweb= webify_string($_);

    if( m/$gmake_enteringdirectory/)
      {
        $currentlineweb="<font color=\"555500\">$currentlineweb</font>";

        push( @current, $1);

        if( !exists( $allbuilds{$1}))
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
    elsif( m/$gmake_leavingdirectory/)
      {
        $currentlineweb="<font color=\"555500\">$currentlineweb</font>";
        pop( @current);
        $index= $allbuilds{$current[$#current]};
      }
    elsif( m/$compilingcpp/)
      {
        $buildfilenumber[$index]++;
        $currentlineweb="<font color=blue>$currentlineweb</font>";
      }
    elsif( m/$compilingc/)
      {
        $buildfilenumber[$index]++;
        $currentlineweb="<font color=blue>$currentlineweb</font>";
      }
    elsif( m/$linkingso/)
      {
#       print "so file: $1\n";
      }
    elsif( m/$compilewarning/)
      {
        $buildwarnings[$index]++;
        $currentlineweb="<font color=\"AA0000\">$currentlineweb</font>";
      }
    elsif( m/$linkwarning/)
      {
      }
    elsif( m/$gmake_errorindirectory/)
      {
        $builderrors[$index]++;
        $currentlineweb="<font color=red>$currentlineweb</font>";
      }
    elsif( m/Beginning TargetJr make:\s*(.*)$/)
      {
        $f = $1; $f =~ s/ ?DST / /;
        @thisdate= ParseDate( $f);
        $starttime= UnixDate( @thisdate, "%a %b %e %H:%M:%S %z %Y") unless ($starttime);
      }
    elsif( m/Done TargetJr make:\s*(.*)$/)
      {
        $f = $1; $f =~ s/ ?DST / /;
        @thisdate= ParseDate( $f);
        $endtime= UnixDate( @thisdate, "%a %b %e %H:%M:%S %z %Y");
      }

    if( $index)
      {
        $buildlogs[$index].= $currentlineweb;
        $fullbuildlog.= $fullbuildlog;
      }
  }

##############################################
# find the common sub-directory for each build

$subdir=-1;

for $build ( keys %allbuilds )
  {
    if( $subdir== -1)
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
# compute the heirarchy

for $buildit1 ( keys %allbuilds )
  {
    # remove the last subdirectory from this build
    $index  = $allbuilds{$buildit1};
    $buildit1=~ s/\/*[^\/]+$//;

    for $buildit2 ( keys %allbuilds )
      {
        if( $buildit1 eq $buildit2)
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

#print "Start time = $starttime\n";
#print "End time   = $endtime\n";

$as= timelocal( $ss, $ms, $hs, $ds, $Ms, $ys);
$ae= timelocal( $se, $me, $he, $de, $Me, $ye);

$tottime= int(($ae-$as)/60);

##################
### OUTPUT THE XML

#StartFrost( "icehouse", "vxl", "vxluser", "");
SetMachine( $machine);

StartRunTestGroup( "root", $date);
StartRunTest( "BuildStats", $date);
RunMeasurement( "Errors", $toterrors);
RunMeasurement( "Warnings", $totwarnings);
RunMeasurement( "FileCount", $totfiles);
RunMeasurement( "TimeStarted", $totstarttime);
RunMeasurement( "TimeElapsed", $tottime);
#RunMeasurementBase64( "BuildLog", $fullbuildlog);

if( $toterrors>0)
  {
    EndRunTest( "f");
  }
else
  {
    EndRunTest( "t");
  }

EndRunTestGroup();


$headindex= $allbuilds{""};
@roots= get_children( $headindex);

for( $i=0; $i<= $#roots; $i++)
  {
    XMLruntestgroup( $roots[$i]);
  }

#EndFrost();

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

    for( $i= 0; $i<= length($string); $i++)
      {
        $c= substr($string,$i,1);

        if ( $c eq "\n")
          {
            $c= "<br>\n";
          }
        elsif( $c eq  "\"")
          {
            $c= "&quot";
          }
        elsif( $c eq  "&")
          {
            $c= "&amp";
          }
        elsif( $c eq  "<")
          {
            $c= "&lt";
          }
        elsif( $c eq  ">")
          {
            $c= "&gt";
          }

        $n.= $c;
      }

    return $n;
  }


sub get_depth
  {
    my ($index)= @_;
    my $i= 0;


    while( $buildparent[$index]!= $index)
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

    for( $i= 0; $i<= $#pr; $i++)
      {
        if(( $pr[$i]== $index) &
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
        for( $j= 0; $j<= $#children; $j++)
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

    StartRunTestGroup( $name, $date);

      {
        if( $name=~ /\//)
          {
            StartRunTest( "BuildDirectory", $date);
          }
        else
          {
            StartRunTest( "BuildLibrary", $date);
          }

        # errors
        RunMeasurement( "Errors",($builderrors[$index]+0));

        # warnings
        RunMeasurement( "Warnings", ($buildwarnings[$index]+0));

        if ( $buildlogs[$index] ne "")
          {
            RunMeasurementBase64( "BuildLog", $buildlogs[$index]);
          }

        # file count
#       RunMeasurement( "BuildFileNumber", ($buildfilenumber[$index]+0));

        # start time
#       RunMeasurement( "BuildStartTime", $starttime);

        if( $builderrors[$index]== 0)
          {
            EndRunTest("t");
          }
        else
          {
            EndRunTest("f");
          }
      }

    @children= get_children( $index);

    for( $i=0; $i<=$#children; $i++)
      {
        XMLruntestgroup( $children[$i]);
      }

    EndRunTestGroup( $date);
  }

