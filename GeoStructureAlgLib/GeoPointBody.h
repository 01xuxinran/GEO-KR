#pragma once
#include "geographbaseobject.h"

//边界点类型,端点、边界点、断层交点
enum PointType{EndPoint = 0,FrameCrossPoint,inCrossPoint,borderPoint};

class CGeoPointBody :public CGeoGraphBaseObject
{
private:
	CPoint3                        m_p;

	PointType                      m_type;

	vector<int> m_parentFacesID;

	vector<int> m_twinPointID;

public:
	PointType getPointType(){return m_type;};
	void setPointType(PointType type) {m_type = type;};
	CPoint3 getPoint(){return m_p;}
	void setPoint(CPoint3 p) {m_p = p;};
	vector<int> getParentFacesID(){return m_parentFacesID;}
	void setParentFacesID(vector<int> ids) {m_parentFacesID = ids;};
	void addParentFacesID(int id) {
		for(int i = 0; i < m_parentFacesID.size(); i ++) {
			if(m_parentFacesID[i] == id) return;
		}
		m_parentFacesID.push_back(id);
	};
	void deleteParentFacesID(int id) {
		vector<int> res;
		for(int i = 0; i < m_parentFacesID.size(); i ++) {
			if(m_parentFacesID[i] != id) {
				res.push_back(m_parentFacesID[i]);
			}
		}
		m_parentFacesID = res;
	};
	vector<int> getTwinPointID() {
		return m_twinPointID;
	};
	void setTwinPointID(vector<int> ids) {
		m_twinPointID = ids;
	};
	void addTwinPointID(int id) {
		for(int i = 0; i < m_twinPointID.size(); i ++) {
			if(m_twinPointID[i] == id) return;
		}
		m_twinPointID.push_back(id);
	};
	bool isTwinPoint(int p2ID) {
		for(int i = 0; i < m_twinPointID.size(); i ++) {
			if(m_twinPointID[i] == p2ID) return true;
		}
		return false;
	}
public:
	CGeoPointBody(GeoBodyType type = CROSSPOINTBODY);
	~CGeoPointBody(void);
};

