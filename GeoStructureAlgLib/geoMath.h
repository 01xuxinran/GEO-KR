#pragma once
class geoMath
{
public:
	geoMath(void);
	~geoMath(void);
	struct Line {
		double x1;
		double y1;
		double x2;
		double y2;
	};

	bool isLineIntersection(const Line &l1, const Line &l2);
	
};

