build:
	cl65 -t apple2 -C apple2-system.cfg -Wl -D__EXEHDR__=0 main.c

wav: build
	c2t -2 main,2000 main.wav

clean:
	rm -f main main.wav
