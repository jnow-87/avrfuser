#ifndef FUSE_H
#define FUSE_H


/* types */
typedef struct{
	const char *name;
	int value;
	unsigned int ui_x,
				 ui_y;
} fuse_t;

struct mcu_config_t{
	const char *name;

	fuse_t **low,
		   **high,
		   **ext;
};

typedef mcu_config_t mcu_config_t;


/* external variables */
extern fuse_t cksel0,
			  cksel1,
			  cksel2,
			  cksel3,
			  ckdiv8,
			  ckout,
			  sut0,
			  sut1,
			  bootrst,
			  bootsz0,
			  bootsz1,
			  eesave,
			  wdton,
			  spien,
			  jtagen,
			  ocden,
			  bodlevel0,
			  bodlevel1,
			  bodlevel2,
			  dwen,
			  rstdisbl,
			  reserved;


/* prototypes */
int fuse_read(const char *mcu, const char *programmer, const char *port, mcu_config_t *mcuc);
int fuse_write(const char *mcu, const char *programmer, const char *port, mcu_config_t *mcuc);
void hex_to_fuse(unsigned int value, fuse_t **fuse);
unsigned int fuse_to_hex(fuse_t **fuse);


#endif // FUSE_H
