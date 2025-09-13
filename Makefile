all:
	cd src && $(MAKE)

format:
	clang-format -i src/*.[ch]

clean:
	cd src && $(MAKE) clean
