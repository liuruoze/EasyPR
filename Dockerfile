FROM thiagofalcao/opencv3
ADD https://cmake.org/files/v3.8/cmake-3.8.0-Linux-x86_64.sh /usr/local/
RUN cd  /usr/local && sh cmake-3.8.0-Linux-x86_64.sh --skip-license --exclude-subdir && rm -f cmake-3.8.0-Linux-x86_64.sh
ADD . /data/easypr
RUN cd /data/easypr && ./build.sh
