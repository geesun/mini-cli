CROSS_COMPILE=/ppc/usr/bin/ppc_4xx-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdum



CFLAGS=

CFLAGS+=

LD_PATH=
LD_FLAGS=


OS=linux
ARCH=ppc

KERNEL_BUILD := /home/geesun/src/uni_tiger4/eldk/ppc_4xx/usr/src/linux
