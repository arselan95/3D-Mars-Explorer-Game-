#include "vector3.h"
#include "ray.h"
#include "box.h"
  
/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

/*Again, I have added some methods, and made 2 others const*/

bool Box::intersect(const Ray &r, float t0, float t1) const 
{
  float tmin, tmax, tymin, tymax, tzmin, tzmax;

  tmin = (parameters[r.sign[0]].x() - r.origin.x()) * r.inv_direction.x();
  tmax = (parameters[1-r.sign[0]].x() - r.origin.x()) * r.inv_direction.x();
  tymin = (parameters[r.sign[1]].y() - r.origin.y()) * r.inv_direction.y();
  tymax = (parameters[1-r.sign[1]].y() - r.origin.y()) * r.inv_direction.y();
  if ( (tmin > tymax) || (tymin > tmax) ) 
    return false;
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;
  tzmin = (parameters[r.sign[2]].z() - r.origin.z()) * r.inv_direction.z();
  tzmax = (parameters[1-r.sign[2]].z() - r.origin.z()) * r.inv_direction.z();
  if ( (tmin > tzmax) || (tzmin > tmax) ) 
    return false;
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  return ( (tmin < t1) && (tmax > t0) );
}


//I added this, returns whether or not an ofPoint is inside a box
//Simple bounds checking procedure
bool Box::contains(const ofPoint& p)
{
	return p.x <= this->max().x() && p.x >= this->min().x() &&
		p.y <= this->max().y() && p.y >= this->min().y() &&
		p.z <= this->max().z() && p.z >= this->min().z();
}

//Return the volume of the box
float Box::volume()
{
	float x = abs(max().x() - min().x());	//Length
	float y = abs(max().y() - min().y());   //Height
	float z = abs(max().z() - min().z());   //Width
	return x * y * z;   // length * height * width
}

//Return the center point of the box
Vector3 Box::center() const
{
	return (max() - min()) / 2;  //Midpoint of corner vectors
}