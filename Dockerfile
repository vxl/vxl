FROM debian:8
MAINTAINER Alexander Leinoff <alexander-leinoff@uiowa.edu>

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  git

#RUN sudo sudo setenforce 0

#RUN useradd -m travis

RUN mkdir -p /home/travis/build/vxl

WORKDIR /home/travie/build/vxl

CMD /bin/bash  
