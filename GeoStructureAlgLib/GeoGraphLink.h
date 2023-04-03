#pragma once
#include "geographbaseobject.h"
class CGeoGraphLink :	public CGeoGraphBaseObject
{
private:

	CGeoGraphBaseObject*              m_FirstBody;
	CGeoGraphBaseObject*              m_SecondBody;

public:
	CGeoGraphLink(GeoBodyType type);
	~CGeoGraphLink(void);
};

