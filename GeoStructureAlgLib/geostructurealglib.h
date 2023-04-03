#ifndef GEOSTRUCTUREALGLIB_H
#define GEOSTRUCTUREALGLIB_H

#include "geostructurealglib_global.h"

#include "GeoGraphBaseObject.h"
#include "GeoBlockBody.h"
#include "GeoCrossLineBody.h"
#include "GeoPointBody.h"
#include "GeoSurfaceBody.h"
#include "GeoGraphLink.h"
#include <QString>
 


struct GeoGraphBody {
	vector<CGeoBlockBody *> bodyObj;
	vector<CGeoSurfaceBody *> surfaceObj;
	vector<CGeoCrossLineBody *> lineObj;
	vector<CGeoPointBody *> pointObj;
	void deleteBodyObj(int id) {
		vector<CGeoBlockBody *> temp;
		for(int i = 0; i < bodyObj.size(); i ++) {
			if(bodyObj[i]->getID() == id) continue;
			temp.push_back(bodyObj[i]);

		}
		bodyObj = temp;
	}
	CGeoBlockBody * getBodyObjByID(int id) {
		for(int i = 0; i < bodyObj.size(); i ++) {
			if(bodyObj[i]->getID() == id) {
				return bodyObj[i];
			}
		}
		return NULL;
	}
	void deleteSurfaceObj(int id) {
		vector<CGeoSurfaceBody *> temp;
		for(int i = 0; i < surfaceObj.size(); i ++) {
			if(surfaceObj[i]->getID() == id) continue;
			temp.push_back(surfaceObj[i]);

		}
		surfaceObj = temp;
	}
	CGeoSurfaceBody * getSurfaceObjByID(int id) {
		for(int i = 0; i < surfaceObj.size(); i ++) {
			if(surfaceObj[i]->getID() == id) {
				return surfaceObj[i];
			}
		}
		return NULL;
	}
	// ������ʵ������ڹ�ϵ
	void updateSurfaceObjAdj() {
		vector<int> live;
		for(int i = 0; i < surfaceObj.size(); i ++) {
			if(surfaceObj[i]->getLive() == -1) {
				live.push_back(surfaceObj[i]->getID());
			}
		}
		for(int i = 0; i < live.size(); i ++) {
			deleteSurfaceObj(live[i]);
		}
		
		for(int i = 0; i < surfaceObj.size(); i ++) {
			CGeoSurfaceBody* obj1 = surfaceObj[i];
			for(int j = i + 1; j < surfaceObj.size(); j ++) {
				CGeoSurfaceBody* obj2 = surfaceObj[j];
				// �������������ͬ�����ߣ������������ཻ
				if(obj1->getCommonChildID(obj2) != -1) {
					obj1->addAdjsID(obj2->getID());
					obj2->addAdjsID(obj1->getID());
				}
			}
		}
		for(int i = 0; i < surfaceObj.size(); i ++) {
			vector<int> newAdj;
			
			for(int j = 0; j < surfaceObj[i]->getAdjsID().size(); j ++) {
				int adjID = surfaceObj[i]->getAdjsID()[j];
				if(getSurfaceObjByID(adjID) != NULL) {
					newAdj.push_back(adjID);
				}

			}	
			surfaceObj[i]->setAdjsID(newAdj);
		}
	}

	void deleteLineObj(int id) {
		vector<CGeoCrossLineBody *> temp;
		for(int i = 0; i < lineObj.size(); i ++) {
			if(lineObj[i]->getID() == id) continue;
			temp.push_back(lineObj[i]);

		}
		lineObj = temp;
	}
	CGeoCrossLineBody * getLineObjByID(int id) {
		for(int i = 0; i < lineObj.size(); i ++) {
			if(lineObj[i]->getID() == id) {
				return lineObj[i];
			}
		}
		return NULL;
	}
	void deletePointObj(int id) {
		vector<CGeoPointBody *> temp;
		for(int i = 0; i < pointObj.size(); i ++) {
			if(pointObj[i]->getID() == id) continue;
			temp.push_back(pointObj[i]);

		}
		pointObj = temp;
	}
	CGeoPointBody * getPointObjByID(int id) {
		for(int i = 0; i < pointObj.size(); i ++) {
			if(pointObj[i]->getID() == id) {
				return pointObj[i];
			}
		}
		return NULL;
	}
	
	// �ж���������Ԫ���Ƿ���ͬ
	bool isSameArr(vector<int> a, vector<int> b) {
		if(a.size() != b.size()) return false;
		sort(a.begin(), a.end());
		sort(b.begin(), b.end());
		for(int i = 0; i < a.size(); i ++) {
			if(a[i] != b[i]) return false;
		}
		return true;
	}

	int getLineIDByChildrenID(vector<int> childrenID) {
		for(int i = 0; i < lineObj.size(); i ++) {
			if(isSameArr(lineObj[i]->getChildrenID(), childrenID)) {
				return lineObj[i]->getID();
			} 
		}
		return -1;
	}
	vector<CGeoSurfaceBody *> getSurfaceObjByRealID(int id) {
		vector<CGeoSurfaceBody *> res;
		for(int i = 0; i < surfaceObj.size(); i ++) {
			if(surfaceObj[i]->getRealID() == id) {
				res.push_back(surfaceObj[i]);
			}
		}
		return res;
	}
};
class GEOSTRUCTUREALGLIB_EXPORT GeoStructureAlgLib
{
private:

	vector<pair<QString,vector<CPoint3> > >    m_vhSeed;
	vector<pair<QString,vector<CPoint3> > >    m_vfSeed;
	int                                         m_row;
	int											m_col;

	//��һά������֪ʶͼ��ʵ�����ͣ��顢���桢���ߣ������ߣ������㣨�����㣩���ڶ�ά��ʵ�����
	GeoGraphBody   	m_vGeoGraphBody;
	vector<CGeoGraphLink *>				m_vGeoGraphLink;
	
	vector<vector<int>> m_face2face;
private:

	//��ʵ��ʶ������ȡ
	void AbstractBlockBody();

	//ʵ��ָ�ͺϲ�����
	void BodySegandMerge();
public:
	void setHseed(vector<pair<QString,vector<CPoint3> > > seed) {m_vhSeed = seed;};
	void setFseed(vector<pair<QString,vector<CPoint3> > > seed) {m_vfSeed = seed;};
	void setFace2face(vector<vector<int>> face2face) {m_face2face = face2face;};
	vector<pair<QString,vector<CPoint3> > > getHseed() {return m_vhSeed;};
	vector<pair<QString,vector<CPoint3> > > getFseed() {return m_vfSeed;};
	/*
	��������������������֪�㷨�����빹��������ݣ��������֪ʶͼ������
	�������������������ݣ���һ���Ľ������� 0 - 1����������Χ�����������Ѿ����˹�һ������
	�������������ģ��֪ʶͼ������
	*/
	void geoStructCognition(vector<pair<QString,vector<CPoint3> > > & vhSeed,vector<pair<QString,vector<CPoint3> > > & vfSeed,int row,int col,float minz,float maxz, vector<vector<int>> face2face);

	//�޸Ĺ���֪ʶͼ������
	void updateGeoGraph(GeoGraphBody  &vBody,vector<CGeoGraphLink *> & vLink){m_vGeoGraphBody = vBody;m_vGeoGraphLink = vLink;};

	//��ȡ����֪ʶͼ������
	void getGeoGraph(GeoGraphBody  &vBody,vector<CGeoGraphLink *> & vLink){vBody = m_vGeoGraphBody;vLink = m_vGeoGraphLink;};

	/*
	�������������ݹ���֪ʶͼ��������������Ͻ���
	�������������������Ϊ�գ����Զ�������н��ߣ������Ϊ�գ������ָ������
	���������
	*/
	void regressionCrossLine(CGeoGraphBaseObject * line = NULL); 

	/*
	�������������ݹ���֪ʶͼ�������������������
	�������������������Ϊ�գ����Զ�����������棬�����Ϊ�գ������ָ������
	���������
	*/
	void regressionSurface(CGeoGraphBaseObject * suface = NULL);

	/*
	�������������ݹ���֪ʶͼ���������������ݽ�����Ϣ�����ɲ�����ģ��
	�������������������Ϊ�գ����Զ�����������棬�����Ϊ�գ������ָ������
	���������
	*/
	void regressionFrameSurface(CGeoGraphBaseObject * suface = NULL);
	int findID(int id, vector<int> ids) {
		for(int i = 0; i < ids.size(); i ++) {
			if(id == ids[i]) {
				return i;
			}
		}
		return -1;
	};
	vector<vector<int>> findFaces(vector<vector<int>> halfLines);
	void findBlockDFS(CGeoSurfaceBody* curr, vector<int>& facesID);
	int findTwinPoint(int id, vector<int> childrenPointsID2);

public:
	GeoStructureAlgLib();
	~GeoStructureAlgLib();

private:

};

#endif // GEOSTRUCTUREALGLIB_H
