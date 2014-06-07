mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

main_c := main.c
main_obj := main.o
lib_c := $(current_dir).c
lib_obj := $(current_dir).o
executable := $(current_dir)

run: link
	./$(executable)

link: compile
	LIB_PATH=/usr/local/Cellar/libusb/1.0.18/lib gcc -o $(executable) $(main_obj) $(lib_obj) -lusb-1.0

compile:
	gcc -o $(lib_obj) -c $(lib_c) -I/usr/local/Cellar/libusb/1.0.18/include/libusb-1.0
	gcc -o $(main_obj) -c $(main_c)
