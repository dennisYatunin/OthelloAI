#ifndef __COMMON_H__
#define __COMMON_H__

enum Side {
    WHITE, BLACK, EMPTY
};

static inline Side operator!(Side side) {return static_cast<Side>(BLACK - side);}

class Move {

public:
    int x, y;
    Move(int x, int y) {
        this->x = x;
        this->y = y;
    }
    ~Move() {}

    int getX() { return x; }
    int getY() { return y; }

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
};

#endif
