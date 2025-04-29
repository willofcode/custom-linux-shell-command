# Compiler
CC = gcc

# Targets
all: memviewlinux memviewmacos

memviewlinux: memview.c
	$(CC) -o memviewlinux memview.c

memviewmacos: memviewmacos.c
	$(CC) -o memviewmacos memviewupdate.c

# Clean build files
clean:
	rm -f memviewlinux memviewmacos

