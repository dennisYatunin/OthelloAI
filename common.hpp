#ifndef __COMMON_H__
#define __COMMON_H__

enum Side {
    WHITE, BLACK
};

static inline Side operator!(Side s) {return static_cast<Side>(!static_cast<bool>(s));}

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
