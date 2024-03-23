FROM ubuntu:latest AS DEV

RUN apt-get update && apt-get install -y \
    qtbase5-dev \
    qt5-qmake \
    qtbase5-dev-tools \
    libqt5sql5 \
    build-essential \
    cmake \
    librange-v3-dev \
    pkg-config \
    zip \
    unzip

RUN wget -qO vcpkg.tar.gz https://github.com/microsoft/vcpkg/archive/master.tar.gz


COPY ./main.cpp /app/main.cpp
COPY ./CMakeLists.txt /app/CMakeLists.txt

WORKDIR /app

RUN cmake ./CMakeLists.txt && make

CMD ["./ExpensoApp"]