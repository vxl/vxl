FROM: debian:8
MAINTAINER: Alexander Leinoff <alexander-leinoff@uiowa.edu>

RUN apt-get update && apt-get install -y \
  build-essential
  cmake
  git

RUN mkdir -p /usr/src/vxl-build
WORKDIR /usr/src/

CMD /bin/bash
