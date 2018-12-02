/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	FileName:	mpVector.cpp
//	Author	:	Michael Y. Polyakov
//	email	:	myp@andrew.cmu.edu	or  mikepolyakov@hotmail.com
//	Website	:	www.angelfire.com/linux/myp
//	Date	:	7/16/2002
//	
//		Provides basic vector handling. 
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mpVector.h"

mpVector::mpVector(float xx, float yy, float zz) : 
	x(xx), y(yy), z(zz)
	{ }

mpVector::mpVector() : x(0), y(0), z(0)
	{ }
mpVector::mpVector(const mpVector& other) : x(other.x), y(other.y), z(other.z)
	{ }

mpVector& mpVector::Normalize()
{
	float length = sqrt(x*x + y*y + z*z);
	if(!length) return *this;
	x /= length;
	y /= length;
	z /= length;
	return *this;
}

float mpVector::Magnitude()
{
	return sqrt(x*x + y*y + z*z);
}

mpVector mpVector::Cross(const mpVector& other)
{
	return mpVector(y*other.z-z*other.y, z*other.x-x*other.z, x*other.y-y*other.x);
}

mpVector mpVector::operator - (mpVector v) 
{
	return mpVector(x - v.x, y - v.y, z - v.z);
}

mpVector mpVector::operator + (mpVector v)
{
	return mpVector(x + v.x, y + v.y, z + v.z);
}

float mpVector ::operator * (mpVector v)
{
	return x*v.x + y*v.y + z*v.z;
}
mpVector mpVector::operator - (float c)
{
	return mpVector(x-c, y-c, z-c);
}

mpVector mpVector::operator + (float c)
{
	return mpVector(x+c, y+c, z+c);
}

mpVector mpVector::operator / (float c)
{
	return mpVector(x/c, y/c, z/c);
}

mpVector mpVector::operator * (float c)
{
	return mpVector(x*c, y*c, z*c);
}

mpVector& mpVector::operator = (const mpVector& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

mpVector::operator mp4Vector() const
{
	return mp4Vector(*this);
}

#include <iostream>
mpVector::operator char*() const
{
	char *buffer = new char[300];
	size_t n = sprintf(buffer, "(%f %f %f)", x, y, z);
	return buffer;
}





mp4Vector::mp4Vector() : x(0), y(0), z(0), val(0)
{ }

mp4Vector::mp4Vector(float aa, float bb, float cc, float dd) :
	x(aa), y(bb), z(cc), val(dd)
{ }

mp4Vector::mp4Vector(const mp4Vector& other) : 
	x(other.x), y(other.y), z(other.z), val(other.val)
{ }

mp4Vector::mp4Vector(const mpVector& v, const float value) :
	x(v.x), y(v.x), z(v.z), val(value)
{ }

void mp4Vector::operator = (const mp4Vector& v)
{
	x = v.x; y = v.y; z = v.z;
	val = v.val;
}

mp4Vector::operator mpVector() const
{
	return mpVector(x, y, z);
}
