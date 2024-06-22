FROM ubuntu:latest

RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get install -y cmake:i386

COPY . /app
WORKDIR /app
RUN cmake .
RUN make
ENTRYPOINT [ "docker/startup.sh" ]
