#pragma once

#include "d3dUtility.h"

class DragonXVector3 : public D3DXVECTOR3
{
public:
	DragonXVector3():D3DXVECTOR3()
	{
	}
	DragonXVector3(D3DXVECTOR3 v3):D3DXVECTOR3(v3)
	{
	}
	DragonXVector3(real x,real y,real z):D3DXVECTOR3(x,y,z)
	{
	}
	inline void operator+=(const DragonXVector3& v)
	{
		x+=v.x;
		y+=v.y;
		z+=v.z;
	}
	inline void operator+=(const D3DXVECTOR3& v)
	{
		x+=v.x;
		y+=v.y;
		z+=v.z;
	}
	inline DragonXVector3 operator+(const DragonXVector3& v) const
	{
		return DragonXVector3(x+v.x,y+v.y,z+v.z);
	}
	inline D3DXVECTOR3 operator+(const D3DXVECTOR3& v) const
	{
		return D3DXVECTOR3(x+v.x,y+v.y,z+v.z);
	}
	inline void operator-=(const DragonXVector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	inline void operator-=(const D3DXVECTOR3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	inline DragonXVector3 operator-(const DragonXVector3& v) const
	{
		return DragonXVector3(x-v.x,y-v.y,z-v.z);
	}
	inline D3DXVECTOR3 operator-(const D3DXVECTOR3& v) const
	{
		return D3DXVECTOR3(x-v.x,y-v.y,z-v.z);
	}
	inline void operator*=(const real value)
	{
		x *= value;
		y *= value;
		z *= value;
	}
	inline DragonXVector3 operator*(const real value) const
	{
		return DragonXVector3(x*value, y*value, z*value);
	}
	inline DragonXVector3 componentProductDrX(const DragonXVector3 &v) const
	{
		return DragonXVector3(x*v.x,y*v.y,z*v.z);
	}
	inline void componentProductUpdate(const DragonXVector3 &v)
	{
		x*=v.x;
		y*=v.y;
		z*=v.z;
	}
	inline D3DXVECTOR3 componentProductDX(const DragonXVector3 &v) const
	{
		return D3DXVECTOR3(x*v.x,y*v.y,z*v.z);
	}
	inline real operator*(const DragonXVector3 &v)	const	//dot product
	{
		return D3DXVec3Dot(this,&v);
	}
	inline real operator*(const D3DXVECTOR3 &v)const	//dot product with dx9 v3
	{
		return D3DXVec3Dot(this,&v);
	}
	inline DragonXVector3 operator%(const DragonXVector3 &vector)const //cross product of 2 dragonxv3s
	{
		DragonXVector3 v;
		D3DXVec3Cross(&v,this,&vector);
		return v;
	}
	inline D3DXVECTOR3 operator%(const D3DXVECTOR3 &vector)const //cross product of 2 dragonxv3s
	{
		D3DXVECTOR3 v;
		D3DXVec3Cross(&v,this,&vector);
		return v;
	}
	inline void Normalize()
	{
		D3DXVec3Normalize(this,this);
	}
	inline real GetMagnitude()const//returns the magnitude more processor intensive then get squared mag
	{
		return D3DXVec3Length(this);
	}
	inline real GetMagSquared() const // returns squared mag less processor intensive than get mag
	{
		return D3DXVec3LengthSq(this);
	}
	/*inline real operator[](unsigned i) const
	{
	if (i == 0) return x;
	if (i == 1) return y;
	return z;
	}
	inline real& operator[](unsigned i)
	{
	if (i == 0) return x;
	if (i == 1) return y;
	return z;
	}*/
	inline void clear()
	{
		x=0.0f;
		y=0.0f;
		z=0.0f;
	}
	inline void invert()
	{
		x*=-1.0f;
		y*=-1.0f;
		z*=-1.0f;
	}
	inline void truncate(real max)
	{
		if (GetMagSquared() > max*max)
		{
			Normalize();
			x *= max;
			y *= max;
			z *= max;
		}
	}
	/** Checks if the two vectors have identical components. */
	inline bool operator==(const DragonXVector3& other) const
	{
		return x == other.x &&
			y == other.y &&
			z == other.z;
	}

	/** Checks if the two vectors have non-identical components. */
	inline bool operator!=(const DragonXVector3& other) const
	{
		return !(*this == other);
	}

	/**
	* Checks if this vector is component-by-component less than
	* the other.
	*
	* @note This does not behave like a single-value comparison:
	* !(a < b) does not imply (b >= a).
	*/
	inline bool operator<(const DragonXVector3& other) const
	{
		return x < other.x && y < other.y && z < other.z;
	}

	/**
	* Checks if this vector is component-by-component less than
	* the other.
	*
	* @note This does not behave like a single-value comparison:
	* !(a < b) does not imply (b >= a).
	*/
	inline bool operator>(const DragonXVector3& other) const
	{
		return x > other.x && y > other.y && z > other.z;
	}

	/**
	* Checks if this vector is component-by-component less than
	* the other.
	*
	* @note This does not behave like a single-value comparison:
	* !(a <= b) does not imply (b > a).
	*/
	inline bool operator<=(const DragonXVector3& other) const
	{
		return x <= other.x && y <= other.y && z <= other.z;
	}

	/**
	* Checks if this vector is component-by-component less than
	* the other.
	*
	* @note This does not behave like a single-value comparison:
	* !(a <= b) does not imply (b > a).
	*/
	inline bool operator>=(const DragonXVector3& other) const
	{
		return x >= other.x && y >= other.y && z >= other.z;
	}
};

class DragonXQuaternion : public D3DXQUATERNION
{
public:
	DragonXQuaternion():D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f)
	{
	}
	DragonXQuaternion(real x,real y,real z, real w):D3DXQUATERNION(x,y,z,w)
	{
	}
	DragonXQuaternion(D3DXQUATERNION q):D3DXQUATERNION(q)
	{
	}
	inline void normalize()
	{
		D3DXQuaternionNormalize(this,this);
	}
	inline void operator*=(const DragonXQuaternion &q)
	{
		D3DXQuaternionMultiply(this,this,&q);
	}
	inline void addScaledVectorDrX(const DragonXVector3& vector,real scale)
	{
		D3DXQUATERNION q(vector.x * scale,
			vector.y * scale,
			vector.z * scale,0.0f);
		q *= *this;
		w += q.w * ((real)0.5);
		x += q.x * ((real)0.5);
		y += q.y * ((real)0.5);
		z += q.z * ((real)0.5);
	}
	inline void rotateByVector(const DragonXVector3& v)
	{
		DragonXQuaternion q(v.x,v.y,v.z,0.0f);
		(*this)*= q;
	}
	inline DragonXVector3 Forward()
	{
		return DragonXVector3( 2 * (x * z + w * y),
			2 * (y * z - w * x),
			1 - 2 * (x * x + y * y));
	}
	inline DragonXVector3 Up()
	{
		return DragonXVector3( 2 * (x * y - w * z),
			1 - 2 * (x * x + z * z),
			2 * (y * z + w * x));
	}

	inline DragonXVector3 Right()
	{
		return DragonXVector3( 1 - 2 * (y * y + z * z),
			2 * (x * y + w * z),
			2 * (x * z - w * y));
	}
};

class DragonXMatrix : public D3DXMATRIX
{
public:
	DragonXMatrix():D3DXMATRIX(1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f)
	{
	}
	DragonXMatrix(D3DXMATRIX m):D3DXMATRIX(m)
	{
	}
	inline void setDiagonal(real a,real b, real c)
	{
		_11=a;
		_22=b;
		_33=c;
	}
	inline void setDiagonal(DragonXVector3 v)
	{
		setDiagonal(v.x,v.y,v.z);
	}
	inline DragonXVector3 operator*(const DragonXVector3 &v) const
	{
		return DragonXVector3(
			v.x*_11+v.y*_12+v.z*_13+_14,
			v.x*_21+v.y*_22+v.z*_23+_24,
			v.x*_31+v.y*_32+v.z*_33+_34);
	}
	inline DragonXVector3 transform(const DragonXVector3 &v) const
	{
		return (*this)*v;
	}
	inline real getDeterminant()const
	{
		return D3DXMatrixDeterminant(this);
	}
	inline DragonXMatrix operator*(const DragonXMatrix &x) const
	{
		DragonXMatrix m;
		D3DXMatrixMultiply(&m,this,&x);
		return m;
	}
	inline DragonXMatrix getInverse() const
	{
		DragonXMatrix m;
		return *D3DXMatrixInverse(&m,NULL,this);
	}
	inline DragonXVector3 TransformTranspose(const DragonXVector3 &vector) const
	{
		return DragonXVector3(
			vector.x * _11 + vector.y * _21 + vector.z * _31,
			vector.x * _12 + vector.y * _22 + vector.z * _32,
			vector.x * _13 + vector.y * _23 + vector.z * _33);
	}
	inline DragonXVector3 TransformDirection(const DragonXVector3 &v)const
	{
		return DragonXVector3(v.x*_11+v.y*_12+v.z*_13,v.x*_21+v.y*_22+v.z*_23,v.x*_31+v.y*_32+v.z*_33);
	}
	inline DragonXVector3 TransformInverseDirection(const DragonXVector3 &v) const
	{
		return DragonXVector3(v.x*_11+v.y*_21+v.z*_31,v.x*_12+v.y*_22+v.z*_32,v.x*_13+v.y*_23+v.z*_33);
	}
	inline DragonXVector3 TransformInverse(const DragonXVector3 &v) const
	{
		DragonXVector3 tmp = v;
		tmp.x-=_14;
		tmp.y-=_24;
		tmp.z-=_34;
		return DragonXVector3(tmp.x*_11+tmp.y*_21+tmp.z*_31,tmp.x*_12+tmp.y*_22+tmp.z*_32,tmp.x*_13+tmp.y*_23+tmp.z*_33);
	}
	inline void setComponents(const DragonXVector3 &compOne, const DragonXVector3 &compTwo, const DragonXVector3 &compThree)
	{
		_11 = compOne.x;
		_12 = compTwo.x;
		_13 = compThree.x;
		_21 = compOne.y;
		_22 = compTwo.y;
		_23 = compThree.y;
		_31 = compOne.z;
		_32 = compTwo.z;
		_33 = compThree.z;
	}
	inline DragonXVector3 getAxisVector(unsigned idx) const
	{
		return DragonXVector3(m[0][idx],m[1][idx],m[2][idx]);
	}
	/**
	* Sets the matrix to be a skew symmetric matrix based on
	* the given vector. The skew symmetric matrix is the equivalent
	* of the cross product. So if a,b are vectors. a x b = A_s b
	* where A_s is the skew symmetric form of a.
	*/
	inline void setSkewSymmetric(const DragonXVector3 vector)
	{
		_11 = _22 = _33 = 0.0f;
		_12 = -vector.z;
		_13 = vector.y;
		_21 = vector.z;
		_23 = -vector.x;
		_31 = -vector.y;
		_32 = vector.x;
	}
	/**
	* Sets the matrix to be the transpose of the given matrix.
	*
	* @param m The matrix to transpose and use to set this.
	*/
	inline void setTranspose(const DragonXMatrix &M)
	{
		_11 = M._11;
		_12 = M._21;
		_13 = M._31;
		_21 = M._12;
		_22 = M._22;
		_23 = M._32;
		_31 = M._13;
		_32 = M._23;
		_33 = M._33;
	}

	/** Returns a new matrix containing the transpose of this matrix. */
	inline DragonXMatrix transpose() const
	{
		DragonXMatrix result;
		result.setTranspose(*this);
		return result;
	}
};
