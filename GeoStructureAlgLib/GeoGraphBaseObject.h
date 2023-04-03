#pragma once
#include <vector>
#include "point3.h"
#include <qstring.h>
#include "geostructurealglib_global.h"
#include<algorithm>
using namespace std;


enum GeoBodyType{BLOCKBODY = 0, SURFACEBODY, CROSSLINEBODY, CROSSPOINTBODY};

class GEOSTRUCTUREALGLIB_EXPORT CGeoGraphBaseObject
{
protected:

	//实体编号
	int                            m_id;
	// 父级实体id
	vector<int> m_parentsID;

	// 子级实体id
	vector<int> m_childrenID;

	// 邻接实体id
	vector<int> m_adjsID;

	//实体名称
	QString                        m_name;

	//实体对象
	GeoBodyType                    m_bodytype;

	// 生成时间
	double m_hTimeRecord;

	double m_fTimeRecord;

public:

	void setID(int id){m_id = id;};
	int getID() {return m_id;};
	void setName(QString name){m_name = name;};
	QString getName() {return m_name;};
	void setType(GeoBodyType type){m_bodytype = type;}	


	void setParentsID(vector<int> ids) {m_parentsID = ids;};
	vector<int> getParentsID(){return m_parentsID;};
	void addParentsID(int id) {
		for(int i = 0; i < m_parentsID.size(); i ++) {
			if(m_parentsID[i] == id) return;
		}
		m_parentsID.push_back(id);
	};
	void clearParentsID() {m_parentsID.clear();};
	void deleteParentsID(int id) {
		vector<int> temp;
		for(int i = 0; i < m_parentsID.size(); i ++) {
			if(m_parentsID[i] == id) continue;
			temp.push_back(m_parentsID[i]);
		}
		m_parentsID = temp;
	};
	void mergeParentsID(vector<int> ids) {
		for(int i = 0; i < ids.size(); i ++) {
			addParentsID(ids[i]);
		}
	};
	void setChildrenID(vector<int> ids) {m_childrenID = ids;};
	vector<int> getChildrenID(){return m_childrenID;};
	void addChildrenID(int id) {
		for(int i = 0; i < m_childrenID.size(); i ++) {
			if(m_childrenID[i] == id) return;
		}
		m_childrenID.push_back(id);
	};
	void clearChildrenID() {m_childrenID.clear();};
	void deleteChildrenID(int id) {
		vector<int> temp;
		for(int i = 0; i < m_childrenID.size(); i ++) {
			if(m_childrenID[i] == id) continue;
			temp.push_back(m_childrenID[i]);
		}
		m_childrenID = temp;
	};
	void mergeChildrenID(vector<int> ids) {
		for(int i = 0; i < ids.size(); i ++) {
			addChildrenID(ids[i]);
		}
	};
	int getCommonChildID(CGeoGraphBaseObject* t_obj) {
		for(int i = 0; i < m_childrenID.size(); i ++) {
			for(int j = 0; j < t_obj->getChildrenID().size(); j ++) {
				if(m_childrenID[i] == t_obj->getChildrenID()[j]) {
					return m_childrenID[i];
				}
			}
		}
		return -1;
	};


	void setAdjsID(vector<int> ids) {m_adjsID = ids;};
	vector<int> getAdjsID(){return m_adjsID;};
	void addAdjsID(int id) {
		for(int i = 0; i < m_adjsID.size(); i ++) {
			if(m_adjsID[i] == id) return;
		}
		m_adjsID.push_back(id);
	};
	void clearAdjsID() {m_adjsID.clear();};
	bool isConnected(int id, vector<int> ids) {
		for(int i = 0; i < ids.size(); i ++) {
			if(ids[i] == id) return true;
		}
		return false;
	}
	void deleteAdjsID(int id) {
		vector<int> temp;
		for(int i = 0; i < m_adjsID.size(); i ++) {
			if(m_adjsID[i] == id) continue;
			temp.push_back(m_adjsID[i]);
		}
		m_adjsID = temp;
	}
	void mergeAdjsID(vector<int> adjs) {
		for(int i = 0; i < adjs.size(); i ++) {
			addAdjsID(adjs[i]);
		}
	};
	// 判断两个集合是否为同一个集合
	bool compareTwoCollect(vector<int> c1, vector<int> c2) {
		if(c1.size() != c2.size()) return false;
		sort(c1.begin(), c1.end());
		sort(c2.begin(), c2.end());
		for(int i = 0; i < c1.size(); i ++) {
			if(c1[i] != c2[i]) return false;
		}
		return true;
	};
	double getHTimeRecord() {return m_hTimeRecord;};
	void setHTimeRecord(double time) {m_hTimeRecord = time;};
	double getFTimeRecord() {return m_fTimeRecord;};
	void setFTimeRecord(double time) {m_fTimeRecord = time;};

	void mergeObj(CGeoGraphBaseObject* obj) {
		mergeAdjsID(obj->getAdjsID());
		mergeChildrenID(obj->getChildrenID());
		mergeParentsID(obj->getParentsID());
	};
	
	
public:
	CGeoGraphBaseObject(GeoBodyType type);
	~CGeoGraphBaseObject(void);
};

