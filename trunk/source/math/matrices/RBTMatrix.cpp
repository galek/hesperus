/***
 * hesperus: RBTMatrix.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "RBTMatrix.h"

#include <cassert>
#include <cmath>

#include <source/exceptions/Exception.h>
#include <source/math/Constants.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
RBTMatrix::RBTMatrix()
{
	reset_to_zeros();
}

//#################### STATIC FACTORY METHODS ####################
/**
Constructs a rigid-body transformation matrix from an axis, an angle and a translation vector.

@param axis			The rotation axis
@param angle		The rotation angle (in radians, NOT degrees)
@param translation	The translation vector
*/
RBTMatrix_Ptr RBTMatrix::from_axis_angle_translation(Vector3d axis, double angle, const Vector3d& translation)
{
	if(fabs(axis.length_squared() - 1) > SMALL_EPSILON)
	{
		if(axis.length_squared() > EPSILON*EPSILON) axis.normalize();
		else throw Exception("RBTMatrix::from_axis_angle_translation: Can't rotate about a zero-length axis");
	}

	RBTMatrix_Ptr ret(new RBTMatrix);
	RBTMatrix& mat = *ret;

	double c = cos(angle);
	double s = sin(angle);
	double t = 1 - c;

	const double& x = axis.x;
	const double& y = axis.y;
	const double& z = axis.z;

	// See p.62 of "Mathematics for 3D Game Programming & Computer Graphics" (Eric Lengyel).
	mat(0,0) = t*x*x + c;		mat(0,1) = t*x*y - s*z;		mat(0,2) = t*x*z + s*y;		mat(0,3) = translation.x;
	mat(1,0) = t*x*y + s*z;		mat(1,1) = t*y*y + c;		mat(1,2) = t*y*z - s*x;		mat(1,3) = translation.y;
	mat(2,0) = t*x*z - s*y;		mat(2,1) = t*y*z + s*x;		mat(2,2) = t*z*z + c;		mat(2,3) = translation.z;

	return ret;
}

RBTMatrix_Ptr RBTMatrix::identity()
{
	RBTMatrix_Ptr ret(new RBTMatrix);
	RBTMatrix& mat = *ret;
	mat(0,0) = mat(1,1) = mat(2,2) = 1;
	return ret;
}

RBTMatrix_Ptr RBTMatrix::zeros()
{
	return RBTMatrix_Ptr(new RBTMatrix);
}

//#################### PUBLIC OPERATORS ####################
double& RBTMatrix::operator()(int i, int j)
{
	assert(0 <= i && i < 3 && 0 <= j && j < 4);
	return m[i][j];
}

double RBTMatrix::operator()(int i, int j) const
{
	assert(0 <= i && i < 3 && 0 <= j && j < 4);
	return m[i][j];
}

//#################### PUBLIC METHODS ####################
void RBTMatrix::add_scaled(const RBTMatrix_CPtr& mat, double scale)
{
	for(int i=0; i<3; ++i)
		for(int j=0; j<4; ++j)
		{
			m[i][j] += (*mat)(i,j) * scale;
		}
}

/**
Applies the rigid-body transform matrix to a 3D *point* (note: w = 1 for a point,
but 0 for a vector - this function is only suitable for the former).

@param p	The point to which to apply the RBT
@return		The transformed point
*/
Vector3d RBTMatrix::apply_to_point(const Vector3d& p) const
{
	return Vector3d(
		m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3],
		m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3],
		m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3]
	);
}

/**
Applies the rigid-body transform matrix to a 3D *vector* (compare to apply_to_point above).

@param v	The (free) vector to which to apply the RBT
@return		The transformed vector
*/
Vector3d RBTMatrix::apply_to_vector(const Vector3d& v) const
{
	return Vector3d(
		m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
		m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
		m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
	);
}

RBTMatrix_Ptr RBTMatrix::copy(const RBTMatrix_CPtr& rhs)
{
	return RBTMatrix_Ptr(new RBTMatrix(*rhs));
}

RBTMatrix_Ptr RBTMatrix::inverse() const
{
	/*
	[ux vx wx tx] -1   ( [1 0 0 tx]   [ux vx wx 0] ) -1
	[uy vy wy ty]      ( [0 1 0 ty]   [uy vy wy 0] )
	[uz vz wz tz]    = ( [0 0 1 tz] * [uz vz wz 0] )
	[ 0  0  0  1]      ( [0 0 0  1]   [ 0  0  0 1] )

		               [ux vx wx 0] -1   [1 0 0 tx] -1
			           [uy vy wy 0]      [0 1 0 ty]
				     = [ux vz wz 0]    * [0 0 1 tz]
					   [ 0  0  0 1]      [0 0 0  1]

	                   [ux uy uz 0]   [1 0 0 -tx]
		               [vx vy vz 0]   [0 1 0 -ty]
			         = [wx wy wz 0] * [0 0 1 -tz]
				       [ 0  0  0 1]   [0 0 0  1 ]

	                   [ux uy uz -ux*tx-uy*ty-uz*tz]
		               [vx vy vz -vx*tx-vy*ty-vz*tz]
			         = [wx wy wz -wx*tx-wy*ty-wz*tz]
				       [ 0  0  0          1        ]

	                   [ux uy uz -dot(u,t)]
		               [vx vy vz -dot(v,t)]
			         = [wx wy wz -dot(w,t)]
				       [ 0  0  0     1    ]
	*/

	Vector3d u(m[0][0], m[1][0], m[2][0]);
	Vector3d v(m[0][1], m[1][1], m[2][1]);
	Vector3d w(m[0][2], m[1][2], m[2][2]);
	Vector3d t(m[0][3], m[1][3], m[2][3]);

	RBTMatrix_Ptr inv(new RBTMatrix);
	RBTMatrix& i = *inv;

	i(0,0) = u.x;	i(0,1) = u.y;	i(0,2) = u.z;	i(0,3) = -u.dot(t);
	i(1,0) = v.x;	i(1,1) = v.y;	i(1,2) = v.z;	i(1,3) = -v.dot(t);
	i(2,0) = w.x;	i(2,1) = w.y;	i(2,2) = w.z;	i(2,3) = -w.dot(t);

	return inv;
}

std::vector<double> RBTMatrix::rep() const
{
	std::vector<double> vec(16);
	vec[0] = m[0][0];	vec[1] = m[1][0];	vec[2] = m[2][0];	vec[3] = 0;
	vec[4] = m[0][1];	vec[5] = m[1][1];	vec[6] = m[2][1];	vec[7] = 0;
	vec[8] = m[0][2];	vec[9] = m[1][2];	vec[10] = m[2][2];	vec[11] = 0;
	vec[12] = m[0][3];	vec[13] = m[1][3];	vec[14] = m[2][3];	vec[15] = 1;
	return vec;
}

void RBTMatrix::reset_to_zeros()
{
	for(int i=0; i<3; ++i)
		for(int j=0; j<4; ++j)
			m[i][j] = 0;
}

//#################### GLOBAL OPERATORS ####################
RBTMatrix_Ptr& operator+=(RBTMatrix_Ptr& lhs, const RBTMatrix_CPtr& rhs)
{
	for(int i=0; i<3; ++i)
		for(int j=0; j<4; ++j)
		{
			(*lhs)(i,j) += (*rhs)(i,j);
		}

	return lhs;
}

RBTMatrix_Ptr& operator*=(RBTMatrix_Ptr& lhs, const RBTMatrix_CPtr& rhs)
{
	// FIXME: It should be possible to optimize this.
	lhs = lhs * rhs;
	return lhs;
}

RBTMatrix_Ptr& operator*=(RBTMatrix_Ptr& lhs, double scale)
{
	for(int i=0; i<3; ++i)
		for(int j=0; j<4; ++j)
		{
			(*lhs)(i,j) *= scale;
		}

	return lhs;
}

RBTMatrix_Ptr operator*(const RBTMatrix_CPtr& lhs, const RBTMatrix_CPtr& rhs)
{
	RBTMatrix_Ptr ret = RBTMatrix::zeros();

	const RBTMatrix& lmat = *lhs;
	const RBTMatrix& rmat = *rhs;
	RBTMatrix& mat = *ret;

	for(int i=0; i<3; ++i)
		for(int j=0; j<4; ++j)
			for(int k=0; k<3; ++k)
			{
				mat(i,j) += lmat(i,k) * rmat(k,j);
			}

	for(int i=0; i<3; ++i)
	{
		mat(i,3) += lmat(i,3);
	}

	return ret;
}

RBTMatrix_Ptr operator*(const RBTMatrix_CPtr& lhs, double scale)
{
	RBTMatrix_Ptr copy(new RBTMatrix(*lhs));
	copy *= scale;
	return copy;
}

RBTMatrix_Ptr operator*(double scale, const RBTMatrix_CPtr& rhs)
{
	RBTMatrix_Ptr copy(new RBTMatrix(*rhs));
	copy *= scale;
	return copy;
}

}
