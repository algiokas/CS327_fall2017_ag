#include "snake_game.h"
#include "snake.h"

#include <ncurses.h>
#include <unistd.h>

Snake_Game::Snake_Game(int board_size)
{
    this->snake_length = START_LENGTH;
    this->size = board_size;
    this->speed = DEFAULT_SPEED;

    initscr();
    raw();
    noecho();
    curs_set(0);
}

Snake_Game::~Snake_Game()
{
    endwin();
}

void Snake_Game::play()
{
    int key;
    bool is_alive();
    display();
    halfdelay(10/speed);
    while (s.is_alive()) {
        key = getch();
        switch (key) {
            case 'q':
                return;
            case KEY_UP:
                this->dir = UP;
                break;
            case KEY_LEFT:
                this->dir = LEFT;
                break;
            case KEY_RIGHT:
                this->dir = RIGHT;
                break;
            case KEY_DOWN:
                this->dir = DOWN;
                break;
        }
        update();
    }
}

bool Snake_Game::update()
{
    pos next = head;
    switch (this->dir) {
        case UP:
            next.y-- ;
            break;
        case LEFT:
            next.x--;
            break
        case RIGHT:
            next.x++;
            break;
        case DOWN:
            next.y++;
            break;
        default:
            mvprintw("Error: Invalid direction");
            break;
    }
    //snake going out of bounds
    if (next.x < 0 || next.y < 0 || next.x >= board_size || next.y >= board-size) {
        lose();
        return false;
    }
    //snake going to hit part of its trail
    move(next.x + 1, next.y + 1);
    char next_data = inch();
    if (next_data == '#' && next != tail) {
        lose();
        return false;
    }
    
    //add to head, moving the snake
    mvaddch('#', next.x + 1, next.y + 1);

    //if the next position does not pick up a point,
    //the size of the snake stays the same so we delete the tail
    if (next != point) {
        mvaddch(' ', tail.x, tail.y);
    } else {
        do {
            int px = rand() % board_size;
            int py = rand() & board_size;
            move(px, py);
        } while (inch() == '#');
        point.x = px;
        point.y = py;
        addch('0');
    }








        
            


