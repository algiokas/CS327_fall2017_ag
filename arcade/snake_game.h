#ifndef SNAKE_GAME_H

#define SNAKE_GAME_H

const int START_LENGTH = 3;
const int DEFAULT_SPEED = 2;

enum direction = {UP, LEFT, RIGHT, DOWN};

struct pos {
    int x;
    int y;

    bool operator==(const pos& lhs, const pos&rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    bool operator!=(const pos& lhs, const pos&rhs)
    {
        return !(lhs == rhs);
    }

}

class Snake_Game
{
public:
    Snake_Game(int board_size);
    ~Snake_Game();

    void play();

private:
    int snake_length; //length of snake
    int board_size; //width/height of square board
    int speed; //game updates every 1s/speed

    direction dir; //the direction the snake will move on next update

    pos head; //the location of the head of the snake
    pos tail; //the location of the tail of the snake
    pos point; //location of next point

    void display();
    void update();
    void lose();
}:
    
