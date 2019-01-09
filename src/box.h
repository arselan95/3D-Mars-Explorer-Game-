#ifndef _BOX_H_
#define _BOX_H_

#include <assert.h>
#include "vector3.h"
#include "ray.h"
#include "ofMain.h"
/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

/*Made some changes to this, added some functions, and changes some other functions to const*/



class Box 
{
  public:
    Box() { }
    Box(const Vector3 &min, const Vector3 &max) 
	{
      assert(min < max);
      parameters[0] = min;
      parameters[1] = max;
    }
    // (t0, t1) is the interval for valid hits
    bool intersect(const Ray &, float t0, float t1) const;
	bool contains(const ofPoint& p);			   //Determine if point contained in the box
    // corners
    Vector3 parameters[2];
	Vector3 min() const { return parameters[0]; }  //I made this const
	Vector3 max() const { return parameters[1]; }  //I made this const also
	float volume();								   //Method to calculate box volume
	Vector3 center() const;						   //Return the center point of the box
};

#endif // _BOX_H_
