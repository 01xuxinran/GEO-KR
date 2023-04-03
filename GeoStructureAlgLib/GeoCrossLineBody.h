#pragma once
#include "geographbaseobject.h"
#include "GeoPointBody.h"

//边界线、断层线
enum  CrossLineType{frameline = 0,faultline,borderline};

class CGeoCrossLineBody :public CGeoGraphBaseObject
{
private:
	CrossLineType                       m_type;

	//端点实体
	CGeoPointBody *               m_End_A;
	CGeoPointBody *               m_End_B;

	//交线数据
	vector<CGeoGraphBaseObject *>      m_vPoint;

	vector<CGeoCrossLineBody*> m_vCrossLine;

	int m_twinLineID;
	
public:
	void setTwinLineID(int id) {m_twinLineID=id;};
	int getTwinLineID() {return m_twinLineID;};
	CrossLineType getLineType(){return m_type;};
	void setLineType(CrossLineType type) {m_type = type;};
	vector<CGeoGraphBaseObject *> getLineData(){return m_vPoint;};
	pair<CGeoPointBody *, CGeoPointBody *> getEndPointBody(){
		return make_pair(m_End_A, m_End_B);
	};
	void setEndPointBody(CGeoPointBody * A, CGeoPointBody * B){m_End_A = A; m_End_B = B;};
	
	vector<CGeoCrossLineBody *> getCrossLine() {return m_vCrossLine;};
	void setCrossLine(vector<CGeoCrossLineBody *> lines) {m_vCrossLine = lines;};
	void addCrossLine(CGeoCrossLineBody *line) {m_vCrossLine.push_back(line);};
	CGeoCrossLineBody* getCrossLineByID(int id) {
		for(int i = 0; i < m_vCrossLine.size(); i ++) {
			if(m_vCrossLine[i]->getID() == id) { 
				return m_vCrossLine[i];
			}

		}
		return NULL;
	};
	// 根据子节点id获取伴侣节点
	int getWifeLineIDByChildID(int childID) {
		int me = m_id;
		vector<int> wifesID = m_adjsID;

		for(int i = 0; i < wifesID.size(); i ++) {
			CGeoCrossLineBody* wife =  getCrossLineByID(wifesID[i]);
			if(isConnected(childID, wife->getChildrenID())) {
				return wife->getID();
			}
		}
		return -1;

	};

public:
	CGeoCrossLineBody(GeoBodyType type = CROSSLINEBODY);
	~CGeoCrossLineBody(void);
};

