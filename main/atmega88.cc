#include <fuse.h>


fuse_t *atmega88_low[] = {
	&cksel0,
	&cksel1,
	&cksel2,
	&cksel3,
	&sut0,
	&sut1,
	&ckout,
	&ckdiv8,
	0
};

fuse_t *atmega88_high[] = {
	&bodlevel0,
	&bodlevel1,
	&bodlevel2,
	&eesave,
	&wdton,
	&spien,
	&dwen,
	&rstdisbl,
	0
};

fuse_t *atmega88_ext[] = {
	&bootrst,
	&bootsz0,
	&bootsz1,
	&reserved,
	&reserved,
	&reserved,
	&reserved,
	&reserved,
	0
};
