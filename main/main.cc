#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <curses.h>
#include <version.h>
#include <mcu.h>
#include <mcu.hash.h>


/* macros */
#define FUSEBITS_X_BASE		2
#define FUSEBITS_Y_BASE		0
#define FUSEBYTES_X_BASE	2
#define FUSEBYTES_Y_BASE	(FUSEBITS_Y_BASE + nlines + 6)
#define	ELEMENT_LEN			20

#define USAGE(pname){ \
	unsigned int i; \
	\
	\
	printf("usage: %s <mcu> <programmer> <port>\n\n" \
		   "    <mcu>\n" \
		   , pname); \
	\
	for(i=mcu_config::MIN_HASH_VALUE; i<=mcu_config::MAX_HASH_VALUE; i++){ \
		if(mcu_config::wordlist[i].name[0] != 0) \
			printf("        %s\n", mcu_config::wordlist[i].name); \
	} \
	\
	printf("\n" \
		   "    <programmer>   check $avrdude -c ?\n" \
		   "    <port>         port that the programmer is connected to the host, e.g. usb\n" \
	); \
	\
	printf("\nversion:\n" VERSION "\n"); \
}


/* static prototypes */
static int ui(mcu_config_t *mcuc);


int main(int argc, char **argv, char **env){
char *programmer,
	 *port,
	 *mcu;
	mcu_config_t *mcuc;


	/* check arguments */
	if(argc < 4 || (argc > 2 && strcmp(argv[1], "-h") == 0)){
		USAGE(argv[0]);
		return 1;
	}

	mcu = argv[1];
	programmer = argv[2];
	port = argv[3];

	/* get mcu config */
	mcuc = (mcu_config_t*)mcu_config::lookup(mcu, strlen(mcu));

	if(mcuc == 0){
		printf("unknown MCU \"%s\"\n", mcu);
		return 1;
	}

	/* main */
	if(fuse_read(mcu, programmer, port, mcuc) != 0)
		return 1;

	if(ui(mcuc) == 1){
		if(fuse_write(mcu, programmer, port, mcuc) != 0)
			return 1;
	}

	return 0;
}


/* local functions */
int ui(mcu_config_t *mcuc){
	int r = 0;
	unsigned int i,
				 j,
				 x,
				 y,
				 c = 0,
				 nel = 0,
				 nlines,
				 ncols,
				 line = 0,
				 col = 0;
	fuse_t **fuse_lst;


	/* init ncurses */
	setlocale(LC_ALL, "");

	initscr();
	noecho();
	keypad(stdscr, true);

	/* calculate number of lines and columns for fuse bits */
	for(i=0; mcuc->low[i]!=0; i++, nel++);
	for(i=0; mcuc->high[i]!=0; i++, nel++);
	for(i=0; mcuc->ext[i]!=0; i++, nel++);

	if((unsigned int)COLS < ELEMENT_LEN + FUSEBITS_X_BASE)
		goto err;

	ncols = (COLS - FUSEBITS_X_BASE) / ELEMENT_LEN;
	nlines = nel / ncols + (nel % ncols ? 1 : 0);
	ncols = nel / nlines + (nel % nlines ? 1 : 0);

	if((unsigned int)LINES < FUSEBYTES_Y_BASE + 4)
		goto err;

	/* create list of fuses */
	fuse_lst = new fuse_t*[nel];

	i = 0;
	for(j=0; mcuc->low[j]!=0; j++, i++)	fuse_lst[i] = mcuc->low[j];
	for(j=0; mcuc->high[j]!=0; j++, i++)	fuse_lst[i] = mcuc->high[j];
	for(j=0; mcuc->ext[j]!=0; j++, i++)	fuse_lst[i] = mcuc->ext[j];

	/* display static ui elements */
	attron(A_BOLD | A_UNDERLINE);
	mvprintw(FUSEBITS_Y_BASE, 0, "fuse bits");
	mvprintw(FUSEBYTES_Y_BASE, 0, "fuse bytes");
	attroff(A_BOLD | A_UNDERLINE);

	mvprintw(FUSEBITS_Y_BASE + nlines + 3, FUSEBITS_X_BASE + 2, "[x] - unprogrammed (1)");
	mvprintw(FUSEBITS_Y_BASE + nlines + 4, FUSEBITS_X_BASE + 2, "[ ] - programmed (0)");
	mvprintw(FUSEBYTES_Y_BASE + 4, ncols * ELEMENT_LEN / 2 - 10, "press 'q' to quit");

	x = FUSEBITS_X_BASE;

	for(i=0; i<ncols; i++){
		y = FUSEBITS_Y_BASE + 2;

		for(j=0; j<nlines; j++){
			if(i * nlines + j < nel)
				mvprintw(y, x, "[%c] %-16s", (fuse_lst[i * nlines + j]->value ? 'x' : ' '), fuse_lst[i * nlines + j]->name);
			y++;
		}

		x += ELEMENT_LEN;
	}

	/* user input */
	while(!r){
		x = FUSEBITS_X_BASE + ELEMENT_LEN * col;
		y = FUSEBITS_Y_BASE + 2 + line;
		c = col * nlines + line;

		attron(A_BOLD | A_STANDOUT);
		mvprintw(y, x, "[%c]", (fuse_lst[c]->value ? 'x' : ' '));
		attroff(A_BOLD | A_STANDOUT);
		wrefresh(stdscr);

		mvprintw(FUSEBYTES_Y_BASE + 2, FUSEBYTES_X_BASE, "lfuse: 0x%.2x  hfuse: 0x%.2x  efuse: 0x%.2x", fuse_to_hex(mcuc->low), fuse_to_hex(mcuc->high), fuse_to_hex(mcuc->ext));

		switch(getch()){
		case 27:
		case 'q':	// quit ui and ask if to flash fuses
			mvprintw(nlines / 2 - 1, ncols * ELEMENT_LEN / 2 - 10, "                      ");
			attron(A_BLINK);
			mvprintw(nlines / 2 + 0, ncols * ELEMENT_LEN / 2 - 10, " program fuses? [y/n] ");
			attroff(A_BLINK);
			mvprintw(nlines / 2 + 1, ncols * ELEMENT_LEN / 2 - 10, "                      ");
			wrefresh(stdscr);

			switch(getch()){
			case 'y':
				r = 1;
				break;

			default:
				r = 2;
				break;
			};

			break;

		case ' ':	// toggle fuse bit
			fuse_lst[c]->value ^= 1;
			break;

		case 258:	// DOWN
			if(col * nlines + line + 1 < nel){
				if(line < nlines - 1)
					line++;
				else{
					col++;
					line = 0;
				}
			}
			break;

		case 259:	// UP
			if(line > 0)
				line--;
			else if(col > 0){
				col--;
				line = nlines - 1;
			}
			break;

		case 260:	// LEFT
			if(col > 0)
				col--;
			break;

		case 261:	// RIGHT
			if((col + 1) * nlines + line < nel)
					col++;
			break;
		};

		mvprintw(y, x, "[%c]", (fuse_lst[c]->value ? 'x' : ' '));
	}

	delete [] fuse_lst;

	/* stop ncurses */
	endwin();

	return r;

err:
	/* stop ncurses */
	endwin();
	printf("error, unable to render user interface, probably the window is too small\n");

	return -1;
}
