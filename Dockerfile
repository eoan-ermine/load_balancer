FROM gcc:latest as build

RUN apt-get update && \
	apt-get install -y \
		libboost-dev libboost-coroutine-dev libboost-program-options-dev \
		cmake

ADD ./src/ app/src
ADD ./include/ app/include
ADD ./CMakeLists.txt app/CMakeLists.txt


WORKDIR /app/build

RUN cmake .. && \
	cmake --build .

FROM ubuntu:latest

WORKDIR /app

COPY --from=build /app/build/load_balancer .

CMD ["./load_balancer"]
