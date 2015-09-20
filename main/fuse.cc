#include <stdio.h>
#include <stdlib.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <mcu.h>


/* global variables */
fuse_t cksel0 = { "CKSEL0" };
fuse_t cksel1 = { "CKSEL1" };
fuse_t cksel2 = { "CKSEL2" };
fuse_t cksel3 = { "CKSEL3" };
fuse_t ckdiv8 = { "CKDIV8" };
fuse_t ckout = { "CKOUT" };
fuse_t sut0 = { "SUT0" };
fuse_t sut1 = { "SUT1" };
fuse_t bootrst = { "BOOTRST" };
fuse_t bootsz0 = { "BOOTSZ0" };
fuse_t bootsz1 = { "BOOTSZ1" };
fuse_t eesave = { "EESAVE" };
fuse_t wdton = { "WDTON" };
fuse_t spien = { "SPIEN" };
fuse_t jtagen = { "JTAGEN" };
fuse_t ocden = { "OCDEN" };
fuse_t bodlevel0 = { "BODLEVEL0" };
fuse_t bodlevel1 = { "BODLEVEL1" };
fuse_t bodlevel2 = { "BODLEVEL2" };
fuse_t reserved = { "RESERVED" };


/* global functions */
int fuse_read(const char* mcu, const char* programmer, const char* port, mcu_config_t* mcuc){
	char cmd[256];
	unsigned int low,
				  high,
				  ext;
	FILE* fp;


	printf("reading fuses... ");
	snprintf(cmd, 256, "avrdude -qq -p %s -c %s -P %s -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h", mcu, programmer, port);

	fp = popen(cmd, "r");

	if(fp == 0){
		printf("error - %s\n", strerror(errno));
		return -1;
	}

	if(fscanf(fp, "%x %x %x", &low, &high, &ext) != 3){
		printf("\033\[31merror\033\[0m - unhandled fuse byte format\n");
		fclose(fp);
		return -1;
	}

	fclose(fp);

	printf("\033\[32mdone\033\[0m (l: 0x%.2x, h: 0x%.2x, e: 0x%.2x)\n", low, high, ext);

	hex_to_fuse(low, mcuc->low);
	hex_to_fuse(high, mcuc->high);
	hex_to_fuse(ext, mcuc->ext);

	return 0;
}

int fuse_write(const char* mcu, const char* programmer, const char* port, mcu_config_t* mcuc){
	char cmd[256];


	printf("writing fuses... ");

	snprintf(cmd, 256, "avrdude -qq -p %s -c %s -P %s -U lfuse:w:0x%x:m -U hfuse:w:0x%x:m -U efuse:w:0x%x:m", mcu, programmer, port, fuse_to_hex(mcuc->low), fuse_to_hex(mcuc->high), fuse_to_hex(mcuc->ext));

	if(system(cmd) != 0){
		printf("\033\[31merror\033\[0m\n");
		return -1;
	}

	printf("\033\[32mdone\033\[0m (l: 0x%.2x, h: 0x%.2x, e: 0x%.2x)\n", fuse_to_hex(mcuc->low), fuse_to_hex(mcuc->high), fuse_to_hex(mcuc->ext));

	return 0;
}

void hex_to_fuse(unsigned int value, fuse_t** fuse){
	unsigned int i;


	for(i=0; fuse[i]!=0; i++)
		fuse[i]->value = (value & (0x1 << i)) >> i;
}

unsigned int fuse_to_hex(fuse_t** fuse){
	unsigned int i,
				 v = 0;


	for(i=0; fuse[i]!=0; i++)
		v |= (fuse[i]->value & 0x1) << i;

	return v;
}
