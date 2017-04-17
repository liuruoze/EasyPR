FROM thiagofalcao/opencv3

ADD . /data/easypr
RUN cd /data/easypr && ./build
