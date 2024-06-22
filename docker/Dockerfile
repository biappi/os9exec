FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y cmake

COPY . /app
WORKDIR /app
RUN cmake .
RUN make
