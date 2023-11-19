name := tron
start_addr := 2000

default: dsk

build: clean
	mkdir -p out/
	cl65 -t apple2 --start-addr 0x$(start_addr) -Wl -D__EXEHDR__=0 $$(find src/ -name '*.c') -o out/$(name)

wav: build
	c2t -bc8 out/$(name),$(start_addr) out/$(name).wav

dsk: build
	cp "disk-images/ProDOS 8.dsk" out/$(name).dsk
	for f in launcher sysutil fastcopy basic; do ac -d out/$(name).dsk $$f.system; done
	ac -p out/$(name).dsk $(name).system sys 0x$(start_addr) < out/$(name)

clean:
	rm -rf out/
