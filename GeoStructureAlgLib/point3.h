// point3.h
// 电子科技大学通信学院229教研室
// 作者:漆星剑
// 最后修改: 2007.4.3
//	Last:	2007.4.10	double == double
///WangBo   2010/10/14

#ifndef _POINT3_H_PHANTOM_
#define _POINT3_H_PHANTOM_
#pragma warning (disable : 4996)

#include <vector>
#include <cmath>
#include <cstring>
#include <cassert>
#include <limits>
#include <algorithm>
#include <numeric>
#include <functional>

using namespace std;

/////////////////////////////////////////////////////
//	class CPoint3
//	This class is one implemention for storage of 3D coordinate.
//	This class could also be used as a 3D vector(向量)
//	In order to use opengl vertex array, this class only contain xyz value,
//	and all other parts are isolated by micro __HAVE__MORE__.

template<class T>
class CPoint3_tpl
{
public:
	typedef size_t size_type;
	typedef size_t difference_type;
	typedef T & reference;
	typedef const T & const_reference;
	typedef T value_type;
    typedef const T const_value_type;
    typedef T * iterator;
	typedef const T * const_iterator;

public:
	CPoint3_tpl()
	{
		memset(m_data, 0, sizeof(m_data));
	}

	CPoint3_tpl(value_type x, value_type y, value_type z = value_type(0))
	{
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
	}

	explicit CPoint3_tpl(const_value_type * pnt)
	{
		assert(pnt != NULL);
		memcpy(m_data, pnt, sizeof(m_data));
	}

	CPoint3_tpl(const CPoint3_tpl<value_type> & rhs)
	{
		std::copy(rhs.begin(), rhs.end(), begin());
	}

	~CPoint3_tpl(){}

	CPoint3_tpl<value_type> & operator=(const CPoint3_tpl<value_type> & rhs)
	{
		std::copy(rhs.begin(), rhs.end(), begin());
		return *this;
	}

	template<typename F>
	CPoint3_tpl<T> & From(const CPoint3_tpl<F> & rhs)
	{
		copy(rhs.begin(), rhs.end(), begin());
		return *this;
	}

	
/*
	template<typename F>
	static const CPoint3_tpl<T> From(const CPoint3_tpl<F> & rhs)
	{
		return CPoint3_tpl<T>().From(rhs);
	}*/


public:
	size_t size() const {return Data_Size;}
	iterator begin(){return m_data;}
	const_iterator begin() const {return m_data;}
	iterator end(){return m_data + Data_Size;}
	const_iterator end() const {return m_data + Data_Size;}

	value_type * Data()//Return an array containing the coordinate(x, y, z, w).
	{
		return m_data;
	}

	const_value_type * Data() const
	{
		return m_data;
	}

	reference operator[](int k)//return m_data[k % 4]
	{
		return m_data[k % Data_Size];
	}

	const_reference operator[](int k) const
	{
		return m_data[k % Data_Size];
	}

	reference x(){return m_data[0];}
	const_reference x() const{return m_data[0];}
	reference y(){return m_data[1];}
	const_reference y() const{return m_data[1];}
	reference z(){return m_data[2];}
	const_reference z() const{return m_data[2];}

public:
	double Length() const//Consider (x, y, z) as a vector, and return the length of the vector
	{
		return (double)sqrt(DotMult(*this, *this));
	}

	bool operator==(const CPoint3_tpl<value_type> & rhs) const
	{
		return (m_data[0] == rhs.m_data[0]) && (m_data[1] == rhs.m_data[1]) && (m_data[2] == rhs.m_data[2]);
	}

	bool operator<(const CPoint3_tpl<value_type> &rhs) const
	{
		return std::equal(begin(), end(), rhs.begin(), less_equal<value_type>());
	}

	bool operator<=(const CPoint3_tpl<value_type> &rhs) const
	{
		return std::equal(begin(), end(), rhs.begin(), less_equal<value_type>());
	}

	bool operator>(const CPoint3_tpl<value_type> &rhs) const
	{
		return std::equal(begin(), end(), rhs.begin(), greater<value_type>());
	}

	bool operator>=(const CPoint3_tpl<value_type> &rhs) const
	{
		return std::equal(begin(), end(), rhs.begin(), greater_equal<value_type>());
	}
	CPoint3_tpl<T> & operator+=(const CPoint3_tpl<T> & rhs)
	{
		std::transform(rhs.begin(), rhs.end(), begin(), begin(), std::plus<T>());
		return *this;
	}

	//added by hy
	template<typename F>
	CPoint3_tpl<T> & operator+=(const CPoint3_tpl<F>& rhs)
	{
		std::transform(rhs.begin(), rhs.end(), begin(), begin(), std::plus<T>());
		return *this;
	}

	template<typename F>
	CPoint3_tpl<T> & operator-=(const CPoint3_tpl<F> & rhs)
	{
		transform(begin(), end(), rhs.begin(), begin(), std::minus<T>());
		return *this;
	}

	CPoint3_tpl<T> & operator*=(T rhs)
	{
		std::transform(begin(), end(), begin(), std::bind2nd(std::multiplies<T>(), rhs));
		return *this;
	}

	CPoint3_tpl<T> & operator/=(T rhs)
	{
		assert(rhs != T(0));
		std::transform(begin(), end(), begin(), std::bind2nd(std::divides<T>(), rhs));
		return *this;
	}

protected:
	enum _etagInit{Data_Size = 3};
	value_type		m_data[Data_Size];//Stored in the format of (x, y, z, w).
};

#ifdef __POINT3__USE__FLOAT__
typedef CPoint3_tpl<float>		CPoint3;
#else
typedef CPoint3_tpl<double>		CPoint3;
#endif//__POINT3__USE__FLOAT__
typedef CPoint3_tpl<long>		CPoint;
typedef std::vector<CPoint3>			CPointVector;//a series of CPoint3, could indicate a line or circle.

//Math functions of CPoint3_tpl
template<typename T>
const CPoint3_tpl<T> operator+(const CPoint3_tpl<T> & lhs)
{
	return lhs;
}

template<typename T>
const CPoint3_tpl<T> operator-(const CPoint3_tpl<T> & lhs)
{
	CPoint3_tpl<T> r;
	std::transform(lhs.begin(), lhs.end(), r.begin(), std::negate<T>());
	return r;
}

template<typename T, typename F>
const CPoint3_tpl<T> operator+(const CPoint3_tpl<T> & lhs, const CPoint3_tpl<F> & rhs)
{
	return CPoint3_tpl<T>(lhs) += rhs;
}

template<typename T, typename F>
const CPoint3_tpl<T> operator-(const CPoint3_tpl<T> & lhs, const CPoint3_tpl<F> & rhs)
{
	return CPoint3_tpl<T>(lhs) -= rhs;
}

template<typename T>
const CPoint3_tpl<T> operator*(const CPoint3_tpl<T> & lhs, T rhs)
{
	return CPoint3_tpl<T>(lhs) *= rhs;
}

template<typename T>
const CPoint3_tpl<T> operator*(T lhs, const CPoint3_tpl<T> & rhs)
{
	return CPoint3_tpl<T>(rhs) *= lhs;
}

template<typename T>
const CPoint3_tpl<T> operator/(const CPoint3_tpl<T> & lhs, T rhs)
{
	return CPoint3_tpl<T>(lhs) /= rhs;
}

template<class T>
const T Distance(const CPoint3_tpl<T> & lhs, const CPoint3_tpl<T> & rhs)
{
	return (CPoint3_tpl<T>(lhs) -= rhs).Length();
}

template<class T>
const T DotMult(const CPoint3_tpl<T> & lhs, const CPoint3_tpl<T> & rhs)
{
	return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), T(0));
}

template<class T>
const CPoint3_tpl<T> CrossMult(const CPoint3_tpl<T> & lhs, const CPoint3_tpl<T> & rhs)
{
	CPoint3_tpl<T> p;
	p.x() = lhs.y() * rhs.z() - rhs.y() * lhs.z();
	p.y() = rhs.x() * lhs.z() - lhs.x() * rhs.z();
	p.z() = lhs.x() * rhs.y() - lhs.y() * rhs.x();
	return p;
}

typedef CPoint3_tpl<int>       CdpPoint;

#endif//_POINT3_H_PHANTOM_
