#include "Vec2.h"
#include <math.h>

Vec2::Vec2(int x, int y)
{
  X = x;
  Y = y;
}
Vec2::Vec2()
{
  X = 0.0f;
  Y = 0.0f;
}

//returns the z component of the cross product: thisVector X otherVector
int Vec2::winding(const Vec2 otherVector) const
{
  return (X * otherVector.Y - Y * otherVector.X);
}


int Vec2::dot(const Vec2 otherVector) const
{
  return X * otherVector.X + Y * otherVector.Y;
}

float Vec2::magnitude() const
{
  return sqrt(X*X + Y*Y);
}

float Vec2::angleBetween(const Vec2 otherVector) const
{
  return acos(dot(otherVector) / (magnitude() * otherVector.magnitude())); //in radians
}

Vec2 Vec2::operator+(const Vec2 otherVector)
{
  Vec2 result;
  result.X = X + otherVector.X;
  result.Y = Y + otherVector.Y;
  return result;
}

Vec2 Vec2::operator-(const Vec2 otherVector)
{
  Vec2 result;
  result.X = X - otherVector.X;
  result.Y = Y - otherVector.Y;
  return result;
}
