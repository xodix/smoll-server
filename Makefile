all:
	gcc -Os -nostdlib -static \
		-fno-asynchronous-unwind-tables \
		-fno-stack-protector \
		-fno-ident \
		-ffunction-sections -fdata-sections \
		-Wl,-n \
		-Wl,--nmagic \
		-Wl,--build-id=none \
		-s main.c -o server