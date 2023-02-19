# load_balancer

Simple load balancer written with Boost.Asio.

HTTP GET requests forwarding to HTTP(S) targets is supported.

## Requirements

* **C++17**: Robust support for most language features.
* **Boost**: Boost.Beast, Boost.Asio, Boost.Coroutine and Boost.ProgramOptions.
* **LibSSL**: HTTPS support

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
                --target_host wikipedia.org --target_port 443
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
                            --target_host wikipedia.org --target_port 443
        ports:
            - 80:80

```

## Developer guide

### Requirements

* **clang-format** (optional) — formatting
* **clang-tidy** (optional) — linting

### Usage

Format code using clang-format:

```
make format
```

Run clang-tidy over project codebase:

```
make tidy
```
