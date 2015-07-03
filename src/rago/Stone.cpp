/**
  * Implementation of the functions of the class Stone
  *
  * @author Nicolas David and Sylvain Palominos
  *
  **/

#include "Stone.hpp"

#include "define.hpp"

using namespace rago;
using namespace cv;

Stone::Stone()
{
    this->x = 0;
    this->y = 0;
    this->value = PLAYER_NONE;
}

Stone::Stone(int p, int x, int y)
{
  setDraw(x,y,p);
}


Stone::~Stone()
{
}

void Stone::setDraw(int x, int y, int value)
{
    this->x = x;
    this->y = y;
    this->value = value;
}

void Stone::setPlayer(int i)
{
    value=i;
}


int Stone::getPlayer()
{
    return value;
}

int Stone::getX()
{
  return x;
}

int Stone::getY()
{
  return y;
}

