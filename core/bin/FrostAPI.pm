#
# crossge@crd.ge.com
#

package FrostAPI;

use MIME::Base64;
use IO::Socket;

use vars qw( $frosttype $frostsocket );

# globals
$frosttype= "";
$frostsocket= "STDOUT";


#StartFrost( "icehouse", "vxl", "vxluser", "");
#DefineGauge("Errors", "Errors found in build or test cases", "numeric/integer");
#DefineGauge("Warnings", "Warnings found in build or test cases", "numeric/integer");
#DefineGauge("TimeStarted", "Time an operation started", "text/string");
#DefineGauge("TimeElapsed", "Elapsed time for operation", "numeric/integer");
#DefineGauge("FileCount", "Count of files", "numeric/integer");

#DefineTest("BuildLibrary", "Library build results", "Warnings", "Errors", "TimeStarted", "TimeElapsed");
#DefineTest("BuildDirectory", "Directory build results", "Warnings", "Errors", "FileCount");

#StartDefineTestGroup("vxl", "vxl");
#AddTest("BuildLibrary");
#EndDefineTestGroup();

#StartRunTestGroup("vxl");
#StartRunTestGroup("vxl/vnl");
#StartRunTest("BuildDirectory");
#RunMeasurement( "Errors", 5);
#EndRunTest();
#EndRunTestGroup();
#EndRunTestGroup();

#EndFrost();

sub StartRunTestGroup
  {
    my ($name,$date)= @_;

    if (!$date)
      {
        $date= localtime;
      }

    print $frostsocket "<TestGroupRun Name=\"$name\">\n";
    print $frostsocket "  <StartDateTime>$date</StartDateTime>\n";
  }

sub StartRunTest
  {
    my ($name,$date)= @_;

    if (!$date)
      {
        $date= localtime;
      }

    print $frostsocket "<TestRun Name=\"$name\">\n";
    print $frostsocket "  <StartDateTime>$date</StartDateTime>\n";
  }

sub RunMeasurement
  {
    my ($name,$value)= @_;

    print $frostsocket "<Measurement Gauge=\"$name\">\n";
    print $frostsocket "  <Value>".$value."</Value>\n";
    print $frostsocket "</Measurement>\n";
  }

sub RunMeasurementBase64
  {
    my ($name,$value)= @_;

    print $frostsocket "<Measurement Gauge=\"$name\">\n";
    print $frostsocket "  <Value Encoding=\"base64\">\n".MIME::Base64::encode($value)."\n</Value>\n";
    print $frostsocket "</Measurement>\n";
  }

sub EndRunTest
  {
    my ($success)= @_;

    if ( !$success)
      {
        $success= "t";
      }
    elsif ($success eq "1")
      {
        $success= "t";
      }
    elsif ($success eq "0")
      {
        $success= "f";
      }

    print $frostsocket "  <Passed>$success</Passed>\n";
    print $frostsocket "</TestRun>\n";
  }

sub SetMachine
  {
    my ($name)= @_;

    print $frostsocket "<SetMachine Name=\"$name\"/>\n";
  }

sub EndRunTestGroup
  {
    my ($date)= @_;

    if (!$date)
      {
        $date= localtime;
      }

    print $frostsocket "  <EndDateTime>$date</EndDateTime>\n";
    print $frostsocket "</TestGroupRun>\n";
  }

sub StartFrost
  {
    my ($host,$database,$username,$password)= @_;

    if ( $host eq "FILE")
      {
        $frostsocket= "";
        open( $frostsocket, ">$database");
        $frosttype= "FILE";
      }
    else
      {
        $frostsocket = IO::Socket::INET->new( PeerAddr => $host, PeerPort => 4444, Proto => "tcp", Type => SOCK_STREAM, TimeOut => 10 )
          or die $@;

        print $frostsocket "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        print $frostsocket "<!DOCTYPE FrostImmediate SYSTEM \"http://pragmatic.crd.ge.com/blezek/xml/FrostImmediate.dtd\">\n";
        print $frostsocket "<FrostImmediate Database=\"$database\" Server=\"$host\" Username=\"$username\" Password=\"$password\">\n\n";

        StartSchema();
      }
  }

sub EndFrost
  {
    if ( $frosttype eq "FILE")
      {
        close( $frostsocket);
      }
    else
      {
        EndSchema();

        print $frostsocket "</FrostImmediate>\n";

        my $in= <$frostsocket>;

        if ($in!~ /<Status>true<\/Status>/i)
          {
            print "Error from server: $in\n";
          }
      }
  }

sub EndSchema
  {
    print $frostsocket "</Schema>\n";
  }

sub StartSchema
  {
    print $frostsocket "<Schema>\n";
  }

sub DefineGauge
  {
    my ($name,$description,$type)= @_;

    print $frostsocket "<Gauge Name=\"$name\" Type=\"$type\" PartSpecific=\"f\">\n";
    print $frostsocket "  <Description>$description</Description>\n";
    print $frostsocket "</Gauge>\n\n";
  }

sub DefineTest
  {
    my ($name,$description)= @_;
    shift;
    shift;
    my (@gauges)= @_;
    my $i;

    print $frostsocket "<Test Name=\"$name\">\n";
    print $frostsocket "   <Description>$description</Description>\n";

    for ($i=0; $i<= $#gauges; $i++)
      {
        print $frostsocket "   <Gauge Name=\"$gauges[$i]\"/>\n";
      }

    print $frostsocket "</Test>\n\n";
  }

sub DefineMachine
  {
    my ($name,$location,$architecture)= @_;

    print $frostsocket "<Machine Name=\"$name\" Location=\"$location\" Architecture=\"$architecture\"/>\n\n";
  }

sub StartDefineTestGroup
  {
    my ($name,$description)=@_;

    print $frostsocket "<TestGroup Name=\"$name\" Cardinality=\"1\">\n";
    print $frostsocket "  <Description>$description</Description>\n";
  }

sub EndDefineTestGroup
  {
    print $frostsocket "</TestGroup>\n\n";
  }

sub AddTest
  {
    my ($name)= @_;

    print $frostsocket "  <Test Name=\"$name\"/>\n";
  }
