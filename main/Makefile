bin-y := avrfuser

avrfuser-y := main.o fuse.o mcu.o atmega1284p.o atmega88.o

$(loc_build_tree)/main.o: $(loc_build_tree)/mcu.hash.h

avrfuser-cppflags :=  -I"$(loc_build_tree)/"
avrfuser-ldlibs := -lcurses
