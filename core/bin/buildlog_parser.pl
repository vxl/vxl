#! /bin/sh
# -*- perl -*-
exec perl -I $IUEROOT/vxl/bin -x $0 ${1+"$@"}
#!perl
#line 6

#
# crossge@crd.ge.com
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
$gmake_enteringdirectory= "^g?make\\[[0-9]+\\]: Entering directory \\\`([^\\\']+)\\\'\\\s*\$";
$gmake_leavingdirectory = "^g?make\\[[0-9]+\\]: Leaving directory \\\`([^\\\']+)\\\'\\\s*\$";
$gmake_errorindirectory = "^g?make\\[[0-9]+\\]: \\*\\*\\* \\[";

############
# gcc-2.95

# g++ ... -o something.o
# gcc ... -o something.o
# gcc ... -o something.so
$gcc_compilingcpp= "^g\\\+\\\+\\s.*\\s\\-o\\s([^\\s]+\\.o)";
$gcc_compilingc  = "^gcc\\s.*\\s\\-o\\s([^\\s]+\\.o)";
$gcc_linkingso   = "^gcc\\s.*\\s\\-o\\s([^\\s]+\\.so)";
$gcc_compilewarning= "[^:]+:[0-9]+: warning";

############
# FreeBSD

# c++ ... -o something.o
# gcc ... -o something.o
# gcc ... -o something.so
$freebsd_compilingcpp= "^c\\\+\\\+\\s.*\\s\\-o\\s([^\\s]+\\.o)";
$freebsd_compilingc  = "^gcc\\s.*\\s\\-o\\s([^\\s]+\\.o)";
$freebsd_linkingso   = "^gcc\\s.*\\s\\-o\\s([^\\s]+\\.so)";
$freebsd_compilewarning= "[^:]+:[0-9]+: warning";

##############
# SGI compiler
# CC ... -o something.o
# cc ... -o something.o
# CC ... -o something.so
#  "rpoly.c", line 111: warning(1194): floating-point value does not fit in
#  ld: WARNING 127: 
$sgi_compilingcpp= "^CC\\s.*\\s\\-o\\s([^\\s]+\\.o)";
$sgi_compilingc  = "^cc\\s.*\\s\\-o\\s([^\\s]+\\.o)";
$sgi_linkingso   = "^CC\\s.*\\s\\-o\\s([^\\s]+\\.so)";
$sgi_linkwarning = "ld: WARNING [0-9]+:";
$sgi_compilewarning= "^\\\"[^\\\"]+\\\", line [0-9]+: warning";

########
# SunPro
$sun_compilingcpp= "^CC\\s.*\\s\\-o\\s([^\\s]+\\.o)";
$sun_compilingc  = "^cc\\s.*\\s\\-o\\s([^\\s]+\\.o)";
$sun_linkingso   = "^CC\\s.*\\s\\-o\\s([^\\s]+\\.so)";
$sun_compilewarning= "^\\\"[^\\\"]+\\\", line [0-9]+: Warning";

############### 
# status flags

open(INFO,"-");

$starttime="";
$endtime="";
$currentline="";
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

if( $uname[1]=~ /julia/)
  {
    $machine= "julia";

    $compilingcpp= $sgi_compilingcpp;
    $compilingc=$sgi_compilingc;
    $linkingso=$sgi_linkingso;
    $compilewarning=$sgi_compilewarning;
  }
elsif(( $uname[1]=~ /^imogen/) | 
      ( $uname[1]=~ /^shadow/) | 
      ( $uname[1]=~ /^viola/) | 
      ( $uname[1]=~ /^volumnia/) | 
      ( $uname[1]=~ /^maxcreek/))
  {
    $machine= $uname[1];
    $machine=~ s/\..*//;

    $compilingcpp= $gcc_compilingcpp;
    $compilingc=$gcc_compilingc;
    $linkingso=$gcc_linkingso;
    $compilewarning=$gcc_compilewarning;
  }
elsif(( $uname[1]=~ /pre/) |
      ( $uname[1]=~ /lomme/))
  {
    $machine= $uname[1];
    $machine=~ s/\..*//;

    $compilingcpp= $freebsd_compilingcpp;
    $compilingc=$freebsd_compilingc;
    $linkingso=$freebsd_linkingso;
    $compilewarning=$freebsd_compilewarning;
  }
elsif( $uname[1]=~ /kate/)
  {
    $machine= "kate";

    $compilingcpp= $sun_compilingcpp;
    $compilingc=$sun_compilingc;
    $linkingso=$sun_linkingso;
    $compilewarning=$sun_compilewarning;
  }
else
  {
    print "ERROR\n";
    exit(1);
  }

#print "Machine: $machine\n";
#print "Date   : $date\n";

############
# main loop

while( $in=<INFO>)
  {
    # patch together multi-lines
    if( $in=~ s/\\\s*$//)
      {
	$currentline= $currentline.$in;
      }
    else
      {
	# no multi-line
	$currentline= $currentline.$in;
	$currentlineweb= webify_string($currentline);
	
	if( $currentline=~ /$gmake_enteringdirectory/)
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
	elsif( $currentline=~ /$gmake_leavingdirectory/)
	  {
	    $currentlineweb="<font color=\"555500\">$currentlineweb</font>";
	    pop( @current);
	    $index= $allbuilds{$current[$#current]};
	  }
	elsif( $currentline=~ /$compilingcpp/)
	  {
	    $buildfilenumber[$index]++;
	    $currentlineweb="<font color=blue>$currentlineweb</font>";
	  }
	elsif( $currentline=~ /$compilingc/)
	  {
	    $buildfilenumber[$index]++;
	    $currentlineweb="<font color=blue>$currentlineweb</font>";
	  }
	elsif( $currentline=~ /$linkingso/)
	  {
#	    print "so file: $1\n";
	  }
	elsif( $currentline=~ /$compilewarning/)
	  {
	    $buildwarnings[$index]++;
	    $currentlineweb="<font color=\"AA0000\">$currentlineweb</font>";
	  }
	elsif( $currentline=~ /$gmake_errorindirectory/)
	  {
	    $builderrors[$index]++;
	    $currentlineweb="<font color=red>$currentlineweb</font>";
	  }
	elsif( $currentline=~ /^Beginning TargetJr make:(.*)$/)
	  {
	    @thisdate= ParseDate( $1);
	    $starttime= UnixDate( @thisdate, "%a %b %e %H:%M:%S %z %Y");
	  }
	elsif( $currentline=~ /Done TargetJr make:(.*)$/)
	  {
	    @thisdate= ParseDate( $1);
	    $endtime= UnixDate( @thisdate, "%a %b %e %H:%M:%S %z %Y");
	  }

	if( $index)
	  {
	    $buildlogs[$index].= $currentlineweb;
	    $fullbuildlog.= $fullbuildlog;
	  }

	$currentline= "";
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
#	RunMeasurement( "BuildFileNumber", ($buildfilenumber[$index]+0));

	# start time
#	RunMeasurement( "BuildStartTime", $starttime);

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

