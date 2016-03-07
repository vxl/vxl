FROM debian:8
MAINTAINER Alexander Leinoff <alexander-leinoff@uiowa.edu>

RUN apt-get update && apt-get install -y \
  build-essential \
#  clang \
  cmake \
  git


RUN useradd -m travis

RUN mkdir -p /home/travis/build/vxl

WORKDIR /home/travis/build/vxl

CMD /bin/bash  
