all:
	make -C src
	make -C sample

clean:
	make clean -C src
	make clean -C sample
