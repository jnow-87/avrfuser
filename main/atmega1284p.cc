#include <fuse.h>


fuse_t *atmega1284p_low[] = {
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

fuse_t *atmega1284p_high[] = {
	&bootrst,
	&bootsz0,
	&bootsz1,
	&eesave,
	&wdton,
	&spien,
	&jtagen,
	&ocden,
	0
};

fuse_t *atmega1284p_ext[] = {
	&bodlevel0,
	&bodlevel1,
	&bodlevel2,
	&reserved,
	&reserved,
	&reserved,
	&reserved,
	&reserved,
	0
};
