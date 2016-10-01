///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKVector.h
///
/// Description :	Vector Utilities
///----------------------------------------------------------------------

#include "../Common/RK.h"
#include "RKVector.h"
#include "RKMatrix.h"
#include "../Common/RKMacros.h"
namespace RKUtils
{
	const RKVector RKVector::Red(1, 0, 0, 1);
	const RKVector RKVector::Green(0, 1, 0, 1);
	const RKVector RKVector::Blue(0, 0, 1, 1);
	const RKVector RKVector::White(1, 1, 1, 1);
	const RKVector RKVector::Yellow(1, 1, 0, 1);

	const RKVector RKVector::Right(1, 0, 0, 0);
	const RKVector RKVector::Up(0, 1, 0, 0);
	const RKVector RKVector::Forward(0, 0, 1, 0);

	const RKVector RKVector::Zero(0, 0, 0, 0);
	const RKVector RKVector::One(1, 1, 1, 1);

	const RKVector RKVector::InvalidValue(
		std::numeric_limits<float>::infinity()
		, std::numeric_limits<float>::infinity()
		, std::numeric_limits<float>::infinity()
		, 0);

	const RKVector2 RKVector2::InvalidValue(
		std::numeric_limits<float>::infinity()
		, std::numeric_limits<float>::infinity()
		);


	// added by frank.kyoshi.liu <frank.liu@gameloft.com>
	// to support matrix manipulation for cross product
	// this func returns the matrix representation of u x in the cross production of : u x some
	RKMatrix RKVector::GetCrossProductMatrix(const RKVector& u)
	{
		RKMatrix cross_product_mat = { 0.0f, u.z, -u.y, 0.0f,
			-u.z, 0.0f, u.x, 0.0f,
			u.y, -u.x, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		return cross_product_mat;
	}

	/*
	because there countless perpendicular vectors to this given vector
	we just use the dot product
	assume the given vector is <vx, vy, vz> and its perpendicular vector to be <px, py, pz>
	we have vx*px + vy*py + vz*pz = 0
	1. we find the smallest component of v, zero out this component
	2. if x is the smallest, set py :== -vz and pz = vy
	so we have <0, -vz, vy>
	vx*px + vy*py + vz*pz = 0*px -vz*vy + vy*vz = 0
	*/
	void RKVector::GetOrthonormalBase(RKVector& u_axis, const RKVector& given_vector, RKVector& w_axis)
	{
		RKVector given_vector_modified(given_vector.x > 0.0f ? given_vector.x : -given_vector.x,
			given_vector.y > 0.0f ? given_vector.y : -given_vector.y,
			given_vector.z > 0.0f ? given_vector.z : -given_vector.z
			);

		char smallest_component_indx = 0;
		float smallest_component = given_vector_modified.x;

		if (given_vector_modified.y < smallest_component)
		{
			smallest_component_indx = 1;

			smallest_component = given_vector_modified.y;
		}

		if (given_vector_modified.z < smallest_component)
		{
			smallest_component_indx = 2;
		}

		RKVector perpendicular_vector_to_given_vector(0.0f, 0.0f, 0.0f, 0.0f);
		switch (smallest_component_indx)
		{
		case 0: // x
			perpendicular_vector_to_given_vector.x = 0.0f;
			perpendicular_vector_to_given_vector.y = -given_vector.z;
			perpendicular_vector_to_given_vector.z = given_vector.y;
			break;

		case 1: // y
			perpendicular_vector_to_given_vector.x = -given_vector.z;
			perpendicular_vector_to_given_vector.y = 0.0f;
			perpendicular_vector_to_given_vector.z = given_vector.x;
			break;

		case 2: // z
			perpendicular_vector_to_given_vector.x = -given_vector.y;
			perpendicular_vector_to_given_vector.y = given_vector.x;
			perpendicular_vector_to_given_vector.z = 0.0f;
			break;
		}

		u_axis = given_vector.Cross(perpendicular_vector_to_given_vector);
		u_axis.Normalize();

		w_axis = given_vector.Cross(u_axis);
		w_axis.Normalize();
	}

	RKVector2 RKVector2::GetPerpendicularVector(const RKVector2& v)
	{
		return RKVector2(-v.y, v.x);
	}
}