CROSS_COMPILE=/arm-ecos/usr/bin/arm-elf-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdum


ECOS_ROOT=/home/geesun/ecos

CFLAGS=-O1 -fdata-sections -ffunction-sections -ffreestanding  -Wno-uninitialized -nostdlib -fno-inline -mthumb-interwork -mthumb -mcpu=arm9

CFLAGS+=-I$(ECOS_ROOT)/include

LD_PATH=
LD_FLAGS=


OS=ecos
ARCH=arm

