/* hello4.c
 *	purpose	show how to use erase, time, and draw for animation
 */
#include	<stdio.h>
#include	<curses.h>
#include        <unistd.h>

int main(void)
{
	int	i;

	initscr();
	   clear();
	   for(i=0; i<LINES; i++ ){
		move( i, i+i );
		if ( i%2 == 1 ) 
			standout();
		addstr("Hello, world");
		if ( i%2 == 1 ) 
			standend();
		refresh();
		sleep(1);
		move(i,i+i);			/* move back	*/
		addstr("             ");	/* erase line	*/
	   }
	endwin();
        return 0;
}
