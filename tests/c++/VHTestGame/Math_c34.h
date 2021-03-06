#ifndef MATH_C34_H_               
#define MATH_C34_H_                    

#include <math.h>

#include "Coord2d.h"
#include "Rectangle_c34.h"


inline int compare(double a, double b)
{
	if(a - EPSILON > b)
		return 1;
	if(a + EPSILON < b)
		return -1;
	return 0;
}

inline int random(int max)
{
	return rand() % max;
}

inline bool roll(int perc)
{
	return perc > random(100);
}

template <typename T> 
T clamp(T val, T min, T max)
{
	if(compare(val, min) == -1)
		return min;
	else if(compare(val, max) == 1)
		return max;
	else
		return val;
}

template <typename T> 
bool pointInRect(const Coord2d<T>& p, const Rectangle_c34<T>& r)
{
	if(p[X] >= r.getCoord(X) && p[X] < r.getCoord(X) + r.getSize(X)){
		if(p[Y] >= r.getCoord(Y) && p[Y] < r.getCoord(Y) + r.getSize(Y))
			return true;
	}
	return false;
}

template <typename T> 
bool rectInRect(const Rectangle_c34<T>& r1, const Rectangle_c34<T>& r2)
{
	Coord2d<T> point;

	// Test r1 points vs r2 rectangle
	point[X] = r1.getCoord(X);
	point[Y] = r1.getCoord(Y);
	if(pointInRect(point, r2))
		return true;
	
	point[X] = r1.getCoord(X) + r1.getSize(X);
	if(pointInRect(point, r2))
		return true;

	point[Y] = r1.getCoord(Y) + r1.getSize(Y);
	if(pointInRect(point, r2))
		return true;
	
	point[X] = r1.getCoord(X);
	if(pointInRect(point, r2))
		return true;

	// Test r2 points vs r1 rectangle
	point[X] = r2.getCoord(X);
	point[Y] = r2.getCoord(Y);
	if(pointInRect(point, r1))
		return true;
	
	point[X] = r2.getCoord(X) + r2.getSize(X);
	if(pointInRect(point, r1))
		return true;

	point[Y] = r2.getCoord(Y) + r2.getSize(Y);
	if(pointInRect(point, r1))
		return true;
	
	point[X] = r2.getCoord(X);
	if(pointInRect(point, r1))
		return true;

	return false;
}

#endif
     