//class for two-dimensional vectors using floats
class Vec2
{
  public:
    float X;
    float Y;
    Vec2(float x, float y);
    Vec2();
    float winding(const Vec2 otherVector);
    float dot(const Vec2 otherVector);
    Vec2 operator+(const Vec2 otherVector);
    Vec2 operator-(const Vec2 otherVector);
};
