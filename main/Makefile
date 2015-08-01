bin-y := avrfuser

avrfuser-y := main.o fuse.o mcu.o atmega1284p.o

$(loc_built_tree)/main.o: $(loc_built_tree)/mcu.hash.h

avrfuser-cppflags :=  -I"$(loc_built_tree)/"
avrfuser-ldflags := -lcurses
