obj-m += enable_ccr.o

ifeq "$(shell uname -m)" "aarch64"
CPUTOUSE = -D __aarch64__
endif

ifeq "$(shell uname -m)" "armv7l"
EXTRACONDITIONS = -mfpu=neon
CPUTOUSE = -D __armv7l__
endif

ifeq "$(shell uname -m)" "x86_64"
CPUTOUSE = -D __x8664__
MEASURETIME = cycles
endif


all:
	make -C /usr/src/linux-headers-$(shell uname -r)/ M=$(PWD) modules

clean:
	make -C /usr/src/linux-headers-$(shell uname -r)/ M=$(PWD) clean

install: all
	@echo "Enabling access to performance counters; ideally, you should know what you are doing" 
	sudo insmod enable_ccr.ko
