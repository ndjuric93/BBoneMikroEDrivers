# Makefile for driver
# Derived from:
# http://www.opensourceforu.com/2010/12/writing-your-first-linux-driver/
# with some settings from Robert Nelson's BBB kernel build script
# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq (${KERNELRELEASE},)
	obj-m := module.o
# Otherwise we were called directly from the command line.
# Invoke the kernel build system.
else
	KERNEL_SOURCE := ${HOME}/Development/bb-kernel/KERNEL/
	PWD := $(shell pwd)
# Linux kernel 4.4 (which has cape manager support)
	CC=${HOME}/Development/bb-kernel/dl/gcc-linaro-arm-linux-gnueabihf-4.7-2013.04-20130415_linux/bin/arm-linux-gnueabihf-
	BUILD=bone14
	CORES=4
	image=zImage
	PUBLIC_DRIVER_PWD=~/Development/test_drivers/compiled
default:
# Trigger kernel build for this module
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} -j${CORES} ARCH=arm \
	LOCALVERSION=-${BUILD} CROSS_COMPILE=${CC} ${address} \
	${image} modules
# copy result to public folder
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif
