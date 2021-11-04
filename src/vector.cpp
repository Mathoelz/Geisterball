#include "vector.h"
#include <assert.h>
#include <math.h>

Vector::Vector( float x, float y, float z)
{
    this->X = x;
    this->Y = y;
    this->Z = z;
}

Vector::Vector()
{
    this->X = 0.0f;
    this->Y = 0.0f;
    this->Z = 0.0f;
}

/*+----+---------+  Skalarprodukt  +-----------+----+
| u * v =   Xu*Xv + Yu*Yv + Zu*Zv                   |
+----+------------+-----------+-------------------+*/
float Vector::dot(const Vector& v) const
{
    float value = 0.0f;
    value = this->X * v.X + this->Y * v.Y + this->Z * v.Z;
	return value;
}

/*+----+----------+  Kreuzprodukt  +-----------+----+
| u x v = |Xu| x |Xv| = |Yu*Zv - Zu*Yv|             |
|         |Yu| x |Yv| = |Zu*Xv - Xu*Zv|             |
|         |Zu| x |Zv| = |Xu*Yv - Yu*Xv|             |
+----+------------+-----------+-------------------+*/
Vector Vector::cross(const Vector& v) const
{
	Vector cross = Vector();
    cross.X = this->Y * v.Z - this->Z * v.Y;
    cross.Y = this->Z * v.X - this->X * v.Z;
    cross.Z = this->X * v.Y - this->Y * v.X;
    return cross;
}

/*+----+------------+  Addieren  +-------------+----+
| u + v =   |xu + xv|                               |
|           |yu + yv|                               |
|           |zu + zv|                               |
+----+------------+-----------+-------------------+*/
Vector Vector::operator+(const Vector& v) const
{
	Vector result = Vector();
    result.X = this->X + v.X;
    result.Y = this->Y + v.Y;
    result.Z = this->Z + v.Z;
    return result;
}

Vector Vector::operator-(const Vector& v) const
{
    Vector result = Vector();
    result.X = this->X - v.X;
    result.Y = this->Y - v.Y;
    result.Z = this->Z - v.Z;
    return result;
}

/*+----+----------+  Subtrahieren  +-----------+----+
| u + v =   |xu - xv|                               |
|           |yu - yv|                               |
|           |zu - zv|                               |
+----+------------+-----------+-------------------+*/
Vector Vector::operator*(float c) const
{
    Vector result = Vector();
    result.X = this->X * c;
    result.Y = this->Y * c;
    result.Z = this->Z * c;
    return result;
}

Vector Vector::operator-() const
{
	Vector result = Vector();
   // result.X = this->X * cos(180) - this->Y * sin(180);
    //result.Y = this->X * sin(180) - this->Y * cos(180);
    //result.Z = this->Z;
    result.X = this->X * -1;
    result.Y = this->Y * -1;
    result.Z = this->Z * -1;
    return result;
}

Vector& Vector::operator+=(const Vector& v)
{
    this->X += v.X;
    this->Y += v.Y;
    this->Z += v.Z;
    return *this;
}

/*+----+---------+  Normalisierung  +----------+----+
| norm = Xu/length + Yu/length + Zu/length          |
| norm = Xu*(1/length) +Yu*(1/length) + Zu*(1/length)
+----+------------+-----------+-------------------+*/
Vector& Vector::normalize()
{
    float temp = this->length();;
    this->X = 1 / temp * this->X;
    this->Y = 1 / temp * this->Y;
    this->Z = 1 / temp * this->Z;
    return *this;
}

/*+----+-----------+   Länge  +----------------+----+
| length = √(Xu^2 + Yu^2 + Zu^2)                    |
+----+------------+-----------+-------------------+*/
float Vector::length() const
{
    // Quadratwurzel aus der quadratischen Länge
    return sqrt(this->lengthSquared());
}

/*+----+-------+  Quadratische Länge  +--------+----+
| lengthSquared = Xu^2 + Yu^2 + Zu^2                |
+----+------------+-----------+-------------------+*/
float Vector::lengthSquared() const
{
    // x-y-z Variablen der Istanz Vector werden potenziert
    return pow(this->X, 2) + pow(this->Y, 2) + pow(this->Z, 2);
}

/*+----+-----------+  Reflektion  +------------+----+
| reflection = v - 2(normal * v) * normal           |
+----+------------+-----------+-------------------+*/
Vector Vector::reflection( const Vector& normal) const
{
    return *this - (normal * (2 * this->dot(normal)));
}

bool Vector::triangleIntersection( const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const
{
    double epsilon = 1e-5;
    // Ebene Dreieck
    Vector ab = b-a;
    Vector ac = c-a;
    Vector plane = ab.cross(ac).normalize();
    float abc = ab.cross(ac).length();
    float distance = plane.dot(a);

    //Strahlgleichung
    
    s = (distance - plane.dot(*this)) / plane.dot(d);
    if (s < 0) {
        return false;
    }
    Vector p = (*this) + d * s;

    // Dreieck abp
    Vector ap = p - a;
    Vector sizeABP = ab.cross(ap);
    float abp = sizeABP.length();

    // Dreieck acp
    Vector sizeACP = ac.cross(ap);
    float acp = sizeACP.length();

    // Dreieck bcp
    Vector bc = c - b;
    Vector bp = p - b;
    Vector sizeBCP = bc.cross(bp);
    float bcp = sizeBCP.length();

    // Schnitttest
    if (abc + epsilon >= abp + acp + bcp) {
            return true;
    }
    return false;
}
