WD = $(shell pwd)

OS9_ROOT := $(WD)/dd
DOCKER := $(shell which docker || which podman)
BUILD := $(shell $(DOCKER) build -f docker/Dockerfile --target build -q .)

.PHONY: build/os9exec
build/os9exec:
	mkdir -p build
	$(DOCKER) run --rm -v $(WD):/work -w /work/build $(BUILD) sh -c "cmake .. && make"

.PHONY: run
run: build/os9exec
	$(DOCKER) run --rm -it -v $(WD):/work -v $(OS9_ROOT):/dd -w /work $(BUILD) /work/build/os9exec
