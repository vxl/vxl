CLOUD COMPUTING USING VXL

Apache Hadoop is an open source project that enables map/reduce style job distribution
and file system operations on large computer clusters:
http://hadoop.apache.org/

1) BHDFS

bhdfs is a collection of classes that enable C++ processes to work with files saved on large computer clusters.
The file system on the computer cluster has to be managed by Apache Hadoop's File System Manager  (HDFS)
http://hadoop.apache.org/hdfs/

HDFS is implemented in Java, there is a wrapper around it called libdhfs written in C++ to enable C++ programs to
interact with it.

bhdfs is meant to use libdhfs for io operations of VXL classes. bhdfs_manager class creates a singleton that holds
file system handle of HDFS. bhdfs_fstream handles file io by grabbing the manager instance and reading/writing chunks
of data using the handle. Specialized readers and writes can be written using/inheriting from bhdfs_fstream.

For example there is a stream class
bhdfs_vil_stream.h
that implements vil_stream interface so that it can directly be used in vil load/save operations.

2) MAPPER EXECUTABLES as PYTHON SCRIPTS

Hadoop has a streaming library:
http://hadoop.apache.org/common/docs/current/streaming.html

This library, enables PYTHON scripts to be used as executables which will be distributed onto the nodes of the cluster.

bhdfs/pro directory contains processes that enable HDFS io operations for some classes as well as some file system operations
(like create directories, copy files, etc.) so that PYTHON process pipeline of VXL can operate on the cluster.

3) GETTING BHDFS TO BUILD

BHDFS will only build on LINUX for now.

- install hadoop and the necessary java packages:
http://www.michael-noll.com/tutorials/running-hadoop-on-ubuntu-linux-single-node-cluster/

- libdhfs requires the environment variable CLASSPATH to be set
 one must get the classpath from hadoop:
 TMP=$(/usr/local/hadoop/bin/hadoop classpath)
   and then edit /etc/environment to contain the following:
 CLASSPATH=$CLASSPATH:$TMP
   e.g. echo $CLASSPATH >> /etc/environment

- restart the computer for environment variable to take effect

- make sure LD_LIBRARY_PATH contains the path to libmawt.so
   e.g. to the path /usr/lib/jvm/java-6-sun/jre/lib/amd64
  on ubuntu, this is done by creating a .conf file in the folder:
   /etc/ld.so.conf.d
  add the path to that file. run the command:
   ldconfig

- make sure you have FindHadoop.cmake in vxl/contrib/brl/bmods/
- ccmake vxl and make sure the following paths are set:
JAVA_AWT_INCLUDE_PATH            /usr/lib/jvm/java-6-sun/include
JAVA_AWT_LIBRARY                 /usr/lib/jvm/java-6-sun/jre/lib/amd64/libjawt.so
JAVA_INCLUDE_PATH                /usr/lib/jvm/java-6-sun/include
JAVA_INCLUDE_PATH2               /usr/lib/jvm/java-6-sun/include/linux
JAVA_JVM_LIBRARY                 /usr/lib/jvm/java-6-sun/jre/lib/amd64/server/libjvm.so
JNI_INCLUDE_PATH                 /usr/lib/jvm/java-6-sun-1.6.0.26/include

- cd release/contrib/brl/bbas/bhdfs/tests
- make
- start hadoop e.g. /usr/local/hadoop/bin/start-all.sh
- run the tests in /bhdfs/tests
  e.g.
  ./bhdfs_test_all test_hadoop
  ./bhdfs_test_all test_manager
  ./bhdfs_test_all test_fstream
  ./bhdfs_test_all test_vil_save_load_image

- build boxm2_batch
- cd ${VXL_ROOT}/contrib/brl/bbas/bhdfs/notes and run the test Python script as a hadoop mapper (test script imports boxm2_batch)
  ./test_mapper
- look for a message like the following for a successful job completion:
11/12/20 12:32:17 INFO streaming.StreamJob: Job complete: job_201112200829_0015
11/12/20 12:32:17 INFO streaming.StreamJob: Output: /user/hduser/test_mapper/out
- look for a file of the sort:
   view_00_offsets.txt
  to be created in the current directory, if so, congrats, you're done!

