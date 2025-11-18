#include "Shapes.h"

//add some constructors so that the forces can be referenced outside the header file

// ThrustShapeForce constructor
ThrustShapeForce::ThrustShapeForce(const ofVec3f & f)
	: force(f) { }

// RotationalShapeForce constructor
RotationalShapeForce::RotationalShapeForce(const float & r)
	: force(r) { }

// TurbulenceShapeForce constructor
TurbulenceShapeForce::TurbulenceShapeForce(const ofVec3f & min, const ofVec3f & max)
	: tmin(min)
	, tmax(max) { }
