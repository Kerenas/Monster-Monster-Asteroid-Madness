include env
.PHONY: docker run
SHELL=/bin/bash -o pipefail

ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

PUID := $(shell id -u)
PGID := $(shell id -g)
PWD := $(shell pwd)



all: docker

clean:
	podman rmi ${TAGGED_IMAGE} || true
	podman rmi ${IMAGE}:latest || true

superclean: clean
	podman image prune -f

docker: Dockerfile
	echo ${TAGGED_IMAGE}
	echo ---
	podman build -t ${TAGGED_IMAGE} -t ${IMAGE}:latest \
		--build-arg LC_ALL="en_US.UTF-8" \
		-v $(PWD)/external:/build/external \
		-v $(PWD)/patch:/build/patch \
		-v $(PWD)/script:/build/script \
		-v $(PWD)/env:/build/env \
		.
	rm -f ${TARBALL}
	podman save -o ${TARBALL} ${TAGGED_IMAGE}
