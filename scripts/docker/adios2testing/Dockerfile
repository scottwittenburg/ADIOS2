FROM centos:centos7.3.1611

RUN yum -y update && \
    yum -y install \
        bzip2-devel.x86_64 \
    	curl \
    	git \
        gcc \
        gcc-c++ \
        gcc-gfortran \
        gmake \
        hdf5-devel.x86_64 \
        numpy.x86_64 \
        python-devel.x86_64 \    
        https://github.com/openhpc/ohpc/releases/download/v1.3.GA/ohpc-release-1.3-1.el7.x86_64.rpm

RUN yum -y update && \
    yum -y install \
        ohpc-base \
        openmpi-gnu7-ohpc \
        phdf5-gnu7-openmpi-ohpc \
        netcdf-gnu7-openmpi-ohpc

WORKDIR /home/adios2

RUN curl -O https://cmake.org/files/v3.8/cmake-3.8.2.tar.gz ; \
    tar -zxvf cmake-3.8.2.tar.gz

WORKDIR cmake-3.8.2

RUN ./bootstrap ; make ; make install

WORKDIR /home/adios2

RUN rm cmake-3.8.2.tar.gz && \
    mkdir -p /home/adios2/hidecirclesource
