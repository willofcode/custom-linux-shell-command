# Compiler
CC = gcc

# Targets
all: memviewlinux memviewmacos

memviewlinux: memviewubuntu.c
	$(CC) -o memview1 memviewubuntu.c

memviewmacos: memviewmacos.c
	$(CC) -o memview2 memviewupdate.c

# Clean build files
clean:
	rm -f memview1 memview2

