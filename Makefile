.PHONY: format tidy

IMAGE_NAME ?= load_balancer
PROJECT_NAMESPACE ?= patriotrossii
REGISTRY_NAME ?= ghcr.io

PROJECT_IMAGE ?= $(REGISTRY_NAME)/$(PROJECT_NAMESPACE)/$(IMAGE_NAME)

format:
	find ./ -iname *.hpp -o -iname *.cpp | xargs clang-format -i

tidy:
	cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=on
	clang-tidy -p build/ -header-filter=. `find ./ -iname *.cpp`

docker:
	docker build -t $(IMAGE_NAME):latest .

upload:
	docker tag $(IMAGE_NAME):latest $(PROJECT_IMAGE):latest
	docker push $(PROJECT_IMAGE):latest