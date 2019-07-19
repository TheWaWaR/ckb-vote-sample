all: vote

all-via-docker:
	docker run --rm -it -v `pwd`:/code xxuejie/riscv-gnu-toolchain-rv64imac bash -c "cd /code && make"

vote: vote.c
	riscv64-unknown-elf-gcc -nostartfiles -Os vote.c -o vote -Wl,-static -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
	riscv64-unknown-elf-strip -R .comment -R .riscv.attributes -R .shstrtab vote -o vote_strip
	chown 1000:1000 vote vote_strip

clean:
	rm -rf vote vote_strip
