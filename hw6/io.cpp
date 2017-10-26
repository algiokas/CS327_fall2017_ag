#include "io.h"
#include "Floor.h"

#include <cstdlib>
#include <iostream>

#include <ncurses.h>


bool io_init_terminal()
{
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
	return true;
}

void io_reset_terminal()
{
	endwin();
}

void io_handle_input(Floor *f)
{
	int key;

	do {
		switch (key = std::cin.get()) {
		case '7':
		case 'y':
		case KEY_HOME:
			move_pc(d, 7);
			break;
		case '8':
		case 'k':
		case KEY_UP:
			move_pc(d, 8);
			break;
		case '9':
		case 'u':
		case KEY_PPAGE:
			move_pc(d, 9);
			break;
		case '6':
		case 'l':
		case KEY_RIGHT:
			fail_code = move_pc(d, 6);
			break;
		case '3':
		case 'n':
		case KEY_NPAGE:
			fail_code = move_pc(d, 3);
			break;
		case '2':
		case 'j':
		case KEY_DOWN:
			fail_code = move_pc(d, 2);
			break;
		case '1':
		case 'b':
		case KEY_END:
			fail_code = move_pc(d, 1);
			break;
		case '4':
		case 'h':
		case KEY_LEFT:
			fail_code = move_pc(d, 4);
			break;
		case '5':
		case ' ':
		case '.':
		case KEY_B2:
			fail_code = 0;
			break;
		case '>':
			fail_code = move_pc(d, '>');
			break;
		case '<':
			fail_code = move_pc(d, '<');
			break;
		case 'Q':
			d->quit = 1;
			fail_code = 0;
			break;
		case 'T':
			/* New command.  Display the distances for tunnelers.             */
			io_display_tunnel(d);
			fail_code = 1;
			break;
		case 'D':
			/* New command.  Display the distances for non-tunnelers.         */
			io_display_distance(d);
			fail_code = 1;
			break;
		case 'H':
			/* New command.  Display the hardnesses.                          */
			io_display_hardness(d);
			fail_code = 1;
			break;
		case 's':
			/* New command.  Return to normal display after displaying some   *
			* special screen.                                                */
			io_display(d);
			fail_code = 1;
			break;
		case 'L':
			fail_code = 1;
			break;
		case 'g':
			/* Teleport the PC to a random place in the dungeon.              */
			io_teleport_pc(d);
			fail_code = 0;
			break;
		case 'm':
			io_list_monsters(d);
			fail_code = 1;
			break;
		case 'q':
			/* Demonstrate use of the message queue.  You can use this for *
			* printf()-style debugging (though gdb is probably a better   *
			* option.  Not that it matterrs, but using this command will  *
			* waste a turn.  Set fail_code to 1 and you should be able to *
			* figure out why I did it that way.                           */
			io_queue_message("This is the first message.");
			io_queue_message("Since there are multiple messages, "
				"you will see \"more\" prompts.");
			io_queue_message("You can use any key to advance through messages.");
			io_queue_message("Normal gameplay will not resume until the queue "
				"is empty.");
			io_queue_message("Long lines will be truncated, not wrapped.");
			io_queue_message("io_queue_message() is variadic and handles "
				"all printf() conversion specifiers.");
			io_queue_message("Did you see %s?", "what I did there");
			io_queue_message("When the last message is displayed, there will "
				"be no \"more\" prompt.");
			io_queue_message("Have fun!  And happy printing!");
			fail_code = 0;
			break;
		default:
			/* Also not in the spec.  It's not always easy to figure out what *
			* key code corresponds with a given keystroke.  Print out any    *
			* unhandled key here.  Not only does it give a visual error      *
			* indicator, but it also gives an integer value that can be used *
			* for that key in this (or other) switch statements.  Printed in *
			* octal, with the leading zero, because ncurses.h lists codes in *
			* octal, thus allowing us to do reverse lookups.  If a key has a *
			* name defined in the header, you can use the name here, else    *
			* you can directly use the octal value.                          */
			mvprintw(0, 0, "Unbound key: %#o ", key);
			fail_code = 1;
		}
	} while (fail_code);
}


