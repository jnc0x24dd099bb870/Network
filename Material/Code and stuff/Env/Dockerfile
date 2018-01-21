FROM ubuntu:14.04
RUN apt-get update
RUN  apt-get upgrade -y
RUN  apt-get -y install gcc mono-mcs
RUN  apt-get -y install libpcap0.8-dev
COPY . /home
WORKDIR /home
RUN  gcc test.c -o test 
EXPOSE 8667 8990 23423 
