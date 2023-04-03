#pragma once
#include "geographbaseobject.h"
#include "GeoCrossLineBody.h"
#include "GeoPointBody.h"

#include <limits>



//��������
enum SurfaceType{Top = 0,Bottom,Left,Right,Front,Back,Fault,Horizon};

//���߽ṹ
typedef struct _tagSingleLinkInfo
{
	void	   * p;
	_tagSingleLinkInfo * twin;
	_tagSingleLinkInfo(){twin = NULL;p = NULL;}
}SSingleLinkInfo;

//���Ƶ���Ϣ
typedef struct _tagCtrlPointInfo
{
	//����
	CPoint3       p;
	//���ͣ�0������㣬1���������
	char          type;

	_tagSingleLinkInfo * up;
	_tagSingleLinkInfo * down;
	_tagSingleLinkInfo * left;
	_tagSingleLinkInfo * right;

	_tagCtrlPointInfo(){up = NULL;down = NULL;left = NULL;right = NULL;type = 0;}
}SCtrlPointInfo;
struct BoundBox {
	double xMin;
	double yMin;
	double zMin;
	double xMax;
	double yMax;
	double zMax;

	CPoint3 v1;
	CPoint3 v2;
	CPoint3 v3;
	CPoint3 v4;
	CPoint3 v5;
	CPoint3 v6;
	CPoint3 v7;
	CPoint3 v8;

	BoundBox() : xMin(numeric_limits<double>::max()), yMin(numeric_limits<double>::max()), zMin(numeric_limits<double>::max())
		, xMax(numeric_limits<double>::min()), yMax(numeric_limits<double>::min()), zMax(numeric_limits<double>::min())
		, v1(CPoint3(xMin, yMin, zMax)), v2(CPoint3(xMax, yMin, zMax))
		, v3(CPoint3(xMax, yMin, zMin)), v4(CPoint3(xMin, yMin, zMin))
		, v5(CPoint3(xMin, yMax, zMax)), v6(CPoint3(xMax, yMax, zMax))
		, v7(CPoint3(xMax, yMax, zMin)), v8(CPoint3(xMin, yMax, zMin)) {}


	BoundBox(double xi, double yi, double zi, double xa, double ya, double za) : xMin(xi), yMin(yi), zMin(zi), xMax(xa), yMax(ya), zMax(za)
		, v1(CPoint3(xMin, yMin, zMax)), v2(CPoint3(xMax, yMin, zMax))
		, v3(CPoint3(xMax, yMin, zMin)), v4(CPoint3(xMin, yMin, zMin))
		, v5(CPoint3(xMin, yMax, zMax)), v6(CPoint3(xMax, yMax, zMax))
		, v7(CPoint3(xMax, yMax, zMin)), v8(CPoint3(xMin, yMax, zMin)) {}
	void setVertices() {
		v1 = CPoint3(xMin, yMin, zMax);
		v2 = CPoint3(xMax, yMin, zMax);
		v3 = CPoint3(xMax, yMin, zMin);
		v4 = CPoint3(xMin, yMin, zMin);
		v5 = CPoint3(xMin, yMax, zMax);
		v6 = CPoint3(xMax, yMax, zMax);
		v7 = CPoint3(xMax, yMax, zMin);
		v8 = CPoint3(xMin, yMax, zMin);
	}
	vector<CPoint3> getVertices() {
		vector<CPoint3> vVertices;
		vVertices.push_back(v1);
		vVertices.push_back(v2);
		vVertices.push_back(v3);
		vVertices.push_back(v4);
		vVertices.push_back(v5);
		vVertices.push_back(v6);
		vVertices.push_back(v7);
		vVertices.push_back(v8);
		return vVertices;
	}
	bool isCrossByBoundBox(BoundBox t_boundBox) {
		double AminX = xMin;
		double AmaxX = xMax;
		double AminY = yMin;
		double AmaxY = yMax;
		double AminZ = zMin;
		double AmaxZ = zMax;

		double BminX = t_boundBox.xMin;
		double BmaxX = t_boundBox.xMax;
		double BminY = t_boundBox.yMin;
		double BmaxY = t_boundBox.yMax;
		double BminZ = t_boundBox.zMin;
		double BmaxZ = t_boundBox.zMax;
		if(AminX<=BmaxX && AmaxX >= BminX
				&& AminY<=BmaxY && AmaxY >= BminY
				&& AminZ<=BmaxZ && AmaxZ >= BminZ) {
			return true;
		}
		return false;
	}
	bool isPointInBoundBox(CPoint3 p) {

		if(p.x() >= xMin && p.x() <= xMax 
			&& p.y() >= yMin && p.y() <= yMax
			&& p.z() >= zMin && p.z() <= zMax) {
				return true;
		} else {
			return false;
		}
	}
};


class CGeoSurfaceBody :	public CGeoGraphBaseObject
{
public:
	bool dfsFlag;
	bool isBorder;

private:
	//��������
	SurfaceType              m_type;

	int m_live;
	//������������id����Ӧ�������ݼ���id��
	int                      m_collectID;

	int                      m_row;
	int                      m_col;

	// ��Χ��
	BoundBox              m_boundBox;
	//���н���
	vector<CGeoCrossLineBody *>  m_vCrossLines;


	// �����ཻ����ʵ��
	vector<CGeoSurfaceBody *> m_vCrossSurface;

	// ������������
	vector<CPoint3> m_vSeed;
	//���ģ�͵����
	vector<CGeoPointBody *>  m_vFramePoint;

	//���ģ�������ζ���
	vector<vector<CGeoPointBody *> > m_vFrameTriangle;

	int m_twinFaceID;
	
	vector<int> m_childrenPoint;
	
	int m_realID;

	int m_face2faceID;
	

private:
	
	//�ع��ϲ�����
	void CreateFaultFrameModel();

	//�ع���λ����
	void CreateHorizonFrameModel();

	//�ع��߽�����
	void CreateBoundaryFrameModel();

public:
	int getFace2faceID() {return m_face2faceID;};
	void setFace2faceID(int id) {m_face2faceID = id;};
	int getTwinFaceID() {return m_twinFaceID;};
	void setTwinFaceID(int id) {m_twinFaceID = id;};
	vector<CPoint3> getSeed() {return m_vSeed;};
	void setSeed(vector<CPoint3> seed) {m_vSeed = seed;};
	int getLive() {return m_live;};
	void setLive(int live) {m_live = live;};
	int getRealID() {return m_realID;};
	void setRealID(int id) {m_realID = id;};

	vector<CGeoSurfaceBody *> getCrossSurface() {return m_vCrossSurface;};
	void setCrossSurface(vector<CGeoSurfaceBody *> faces) {m_vCrossSurface = faces;};
	void addCrossSurface(CGeoSurfaceBody *face) {m_vCrossSurface.push_back(face);};

	CGeoSurfaceBody* getCrossSurfaceByID(int id) {
		for(int i = 0; i < m_vCrossSurface.size(); i ++) {
			if(m_vCrossSurface[i]->getID() == id) { return m_vCrossSurface[i];}
		
		}
		return NULL;
	
	}
	void setSurfacetype(SurfaceType type){m_type = type;}

	SurfaceType getSurfaceType(){return m_type;}
	

	void setBoundBox(BoundBox boundBox) {
		m_boundBox = boundBox;
	}

	BoundBox getBoundBox() {
		return m_boundBox;
	}


	//���ɿ��ģ��
	void CreateFrameModel(int row,int col);

	//�ж����������Ƿ��ཻ������ཻ����ƽ���
	bool CrossSurface(CGeoSurfaceBody * t_surface);

	//���ӽ���ʵ��
	void addCrossLine(CGeoCrossLineBody * line){m_vCrossLines.push_back(line);};

	void setCrossLines(vector<CGeoCrossLineBody *> lines) {m_vCrossLines = lines;};
	vector<CGeoCrossLineBody *> getCrossLines(){return m_vCrossLines;};
	// �����ӽڵ�id��ȡ���½ڵ�
	int getWifeSurfaceIDByChildID(int childID) {
		int me = m_id;
		vector<int> wifesID = m_adjsID;

		for(int i = 0; i < wifesID.size(); i ++) {
			CGeoSurfaceBody* wife =  getCrossSurfaceByID(wifesID[i]);
			if(isConnected(childID, wife->getChildrenID())) {
				return wife->getID();
			}
		}
		return -1;

	};
	// ��������ĸ��漯��id
	int getCollectID(){
		return m_collectID;
	};
	void setCollectID(int id) {m_collectID = id;};

	vector<int> getChildrenPoint(){return m_childrenPoint;}
	void setChildrenPoint(vector<int> ids) {m_childrenPoint = ids;};
	void addChildrenPoint(int id) {
		for(int i = 0; i < m_childrenPoint.size(); i ++) {
			if(m_childrenPoint[i] == id) return;
		}
		m_childrenPoint.push_back(id);
	};
	void deleteChildrenPoint(int id) {
		vector<int> res;
		for(int i = 0; i < m_childrenPoint.size(); i ++) {
			if(m_childrenPoint[i] != id) {
				res.push_back(m_childrenPoint[i]);
			}
		}
		m_childrenPoint = res;
	};
public:
	CGeoSurfaceBody(GeoBodyType type = SURFACEBODY );
	~CGeoSurfaceBody(void);
};

