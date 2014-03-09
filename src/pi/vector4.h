#pragma once

template <typename T>
class vector4 {
public:
	float x,y,z,w;

	vector4() : x(0.0), y(0.0), z(0.0), w(0.0) {}
	vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
	vector4(T xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
};

typedef vector4<float>  vector4f;
typedef vector4<double> vector4d;
