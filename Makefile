start_addr := 6000

build:
	cl65 -t apple2 -C apple2.cfg --start-addr 0x$(start_addr) -Wl -D__EXEHDR__=0 main.c

wav: build
	c2t -bc8 main,$(start_addr) main.wav

clean:
	rm -f main main.wav
