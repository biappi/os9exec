FROM ubuntu:latest as build

RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get install -y cmake:i386

FROM build as app

COPY . /app
WORKDIR /app
RUN cmake .
RUN make
ENTRYPOINT [ "docker/startup.sh" ]
