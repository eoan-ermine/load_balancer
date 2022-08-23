# load_balancer

Simple load balancer written with Boost.Asio. **Only GET requests are supported so far**.

## Requirements

* **C++17**: Robust support for most language features.
* **Boost**: Boost.Beast, Boost.Asio, Boost.Coroutine and Boost.ProgramOptions.

## Building

```bash
mkdir build && cd build
cmake ..
cmake -build .
```

## Usage

The code below will start a server listening on address `127.0.0.1:80` and proxying incoming requests to example.com or wikipedia.org, choosing using the round robin algorithm.

```bash
./load_balancer --host 127.0.0.1 --port 80 \
                --target_host example.com --target_port 80 \
                --target_host wikipedia.org --target_port 80
```

### With docker-compose

```yml
version: "3.2"
services:
    balancer:
        image: ghcr.io/patriotrossii/load_balancer:latest
        command: >
            ./load_balancer --host 0.0.0.0 --port 80
                            --target_host example.com --target_port 80
                            --target_host wikipedia.org --target_port 80
        ports:
            - 80:80

```
