FROM gcc:latest as build

RUN apt-get update && \
	apt-get install -y \
		libboost-dev libboost-coroutine-dev libboost-program-options-dev \
		libssl-dev cmake

ADD ./load_balancer/ app/load_balancer
ADD ./CMakeLists.txt app/CMakeLists.txt

WORKDIR /app/build

RUN cmake .. && \
	cmake --build .

FROM ubuntu:latest

WORKDIR /app

COPY --from=build /app/build/load_balancer .

CMD ["./load_balancer"]
