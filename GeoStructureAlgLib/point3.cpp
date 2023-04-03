/*********************************************************************
    Copyright (C), 2010, 四川石油物探公司技术发展中心.
    FileName:  point3.cpp
    Author:    WangBo
    Version :  V1.0
    Date:      2010/10/14
    Description:     	// 模块描述
    other:              // 其他的描述
    Version:         	// 版本信息
                       V1.0
    Function List:   	// 主要函数及其功能
    History:         	// 历史修改记录
    <author>  <time>   <version >   <desc>  
***********************************************************************/

#include "point3.h"


//member function implementation of type float and double
template<>
bool CPoint3_tpl<float>::operator==(const CPoint3_tpl<float> & p) const
{
	return (fabs(m_data[0] - p.m_data[0]) <= std::numeric_limits<float>::epsilon())
		&& (fabs(m_data[1] - p.m_data[1]) <= std::numeric_limits<float>::epsilon())
		&& (fabs(m_data[2] - p.m_data[2]) <= std::numeric_limits<float>::epsilon());
}

template<>
bool CPoint3_tpl<double>::operator==(const CPoint3_tpl<double> & p) const
{
	return (fabs(m_data[0] - p.m_data[0]) <= std::numeric_limits<double>::epsilon())
		&& (fabs(m_data[1] - p.m_data[1]) <= std::numeric_limits<double>::epsilon())
		&& (fabs(m_data[2] - p.m_data[2]) <= std::numeric_limits<double>::epsilon());
}

template<>
CPoint3_tpl<float> & CPoint3_tpl<float>::operator/=(float rhs)
{
	assert(fabs(rhs) > std::numeric_limits<float>::epsilon());
	std::transform(begin(), end(), begin(), std::bind2nd(std::divides<float>(), rhs));
	return *this;
}

template<>
CPoint3_tpl<double> & CPoint3_tpl<double>::operator/=(double rhs)
{
	assert(fabs(rhs) > std::numeric_limits<double>::epsilon());
	std::transform(begin(), end(), begin(), std::bind2nd(std::divides<double>(), rhs));
	return *this;
}
