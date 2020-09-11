#include "Vec2.h"

Vec2::Vec2(float x, float y)
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
float Vec2::winding(const Vec2 otherVector)
{
  return (X * otherVector.Y - Y * otherVector.X);
}


float Vec2::dot(const Vec2 otherVector)
{
  return X * otherVector.X + Y * otherVector.Y;
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
