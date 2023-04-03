#include "geostructurealglib.h"
#include "GeoBlockBody.h"
#include "GeoSurfaceBody.h"
#include "GeoCrossLineBody.h"
#include <map>
#include <list>
#include <stack>
//�����弰����
typedef list<int>* ListPtr;
#define forIt(ListPtr) for(auto it=ListPtr->begin();it!=ListPtr->end();++it)
//vector��find������д
int findP(int id, vector<int> arr) {
	for(int i = 0; i < arr.size(); i ++) {
		if(arr[i] == id) {
			return i;
		}
	}
	return -1;
}
/*
��������������������֪�㷨�����빹��������ݣ��������֪ʶͼ������
�������������������ݣ�������Χ�����������Ѿ����˹�һ������
�������������ģ��֪ʶͼ������
*/
void GeoStructureAlgLib::geoStructCognition( vector<pair<QString,vector<CPoint3> > > & vhSeed,vector<pair<QString,vector<CPoint3> > > & vfSeed,int row,int col,float minz,float maxz, vector<vector<int>> face2face)
{
	// ���ò�λ�Ͷϲ�Ľ�������
	setHseed(vhSeed);
	setFseed(vfSeed);
	setFace2face(face2face);
 	//1. ����6�߽�����
	double borderXmin = 0;
	double borderXmax = 1;
	double borderYmin = 0;
	double borderYmax = 1;
	double borderZmin = minz;
	double borderZmax = maxz;

	// �����߽������
	vector<CGeoSurfaceBody* > frameQue;

	// ��λ���������
	vector<CGeoSurfaceBody* > horQue;

	// �ϲ������
	vector<CGeoSurfaceBody* > faultQue;


	for(int i = 0; i < 6; i ++)
	{
		CGeoSurfaceBody * t_obj = new CGeoSurfaceBody;

		frameQue.push_back(t_obj);

		// ���ù������Χ��

		BoundBox boundBox;
		boundBox.xMin = borderXmin;
		boundBox.yMin = borderYmin;
		boundBox.zMin = borderZmin;
		boundBox.xMax = borderXmax;
		boundBox.yMax = borderYmax;
		boundBox.zMax = borderZmax;

		if(i == 0) {
			// top
			boundBox.zMin = borderZmax;
		} else if(i == 1) {
			// bottom
			boundBox.zMax = borderZmin;
		} else if(i == 2) {
			// left
			boundBox.xMax = borderXmin;
		} else if(i == 3) {
			// right
			boundBox.xMin = borderXmax;
		} else if(i == 4) {
			// front
			boundBox.yMin = borderYmax;
		} else if(i == 5) {
			// back
			boundBox.yMax = borderYmin;
		}  

		boundBox.setVertices();
		t_obj->setBoundBox(boundBox);
		t_obj->setSurfacetype((SurfaceType)i);
		t_obj->setID(i);
		// ���ù�����ʱ��
		if(t_obj->getSurfaceType() == Top) {
			t_obj->setHTimeRecord(9999);
			t_obj->setFTimeRecord(-1);
		} else if(t_obj->getSurfaceType() == Bottom) {
			t_obj->setHTimeRecord(-9999);
			t_obj->setFTimeRecord(-1);

		} else if(t_obj->getSurfaceType() == Left) {
			t_obj->setFTimeRecord(-9999);
			t_obj->setHTimeRecord(-1);

		} else if(t_obj->getSurfaceType() == Right) {
			t_obj->setFTimeRecord(9999);
			t_obj->setHTimeRecord(-1);
		}
		m_vGeoGraphBody.surfaceObj.push_back(t_obj);

	}

	//2. ���ݶϲ�Ͳ�λ����������ʵ��
	for(int i = 0; i < vhSeed.size(); i ++)
	{
		int hLen = vhSeed.size();
		CGeoSurfaceBody * t_obj = new CGeoSurfaceBody;
		CGeoSurfaceBody * twin_obj = new CGeoSurfaceBody;
		t_obj->setName(vhSeed[i].first);
		twin_obj->setName(vhSeed[i].first);

		t_obj->setFace2faceID(i);
		twin_obj->setFace2faceID(i);

		horQue.push_back(t_obj);
		horQue.push_back(twin_obj);
		

		// ���������Χ��
		BoundBox boundBox;

		for(int j = 0; j < vhSeed[i].second.size(); j ++) {
			CPoint3 p = vhSeed[i].second[j];
			boundBox.xMin = min(boundBox.xMin, p.x());
			boundBox.yMin = min(boundBox.yMin, p.y());
			boundBox.zMin = min(boundBox.zMin, p.z());
			boundBox.xMax = max(boundBox.xMax, p.x());
			boundBox.yMax = max(boundBox.yMax, p.y());
			boundBox.zMax = max(boundBox.zMax, p.z());
		}
		boundBox.xMin = 0;
		boundBox.yMin = 0;
		boundBox.xMax = 1;
		boundBox.yMax = 1;

		boundBox.zMin = (double) 0.8 * (i + 1) / hLen;
		boundBox.zMax = (double) 0.8 * (i + 1) / hLen;
		boundBox.setVertices();

		t_obj->setSeed(vhSeed[i].second);
		t_obj->setSurfacetype(SurfaceType::Horizon);
		t_obj->setBoundBox(boundBox);
		t_obj->setID(m_vGeoGraphBody.surfaceObj.size());
		m_vGeoGraphBody.surfaceObj.push_back(t_obj);

		twin_obj->setSeed(vhSeed[i].second);
		twin_obj->setSurfacetype(SurfaceType::Horizon);
		twin_obj->setBoundBox(boundBox);
		twin_obj->setID(m_vGeoGraphBody.surfaceObj.size());
		m_vGeoGraphBody.surfaceObj.push_back(twin_obj);

		t_obj->setTwinFaceID(twin_obj->getID());
		twin_obj->setTwinFaceID(t_obj->getID());

		t_obj->setRealID(m_vGeoGraphBody.surfaceObj.size() / 2);
		twin_obj->setRealID(m_vGeoGraphBody.surfaceObj.size() / 2);
	}

	for(int i = 0; i < vfSeed.size(); i ++)
	{
		int fLen = vfSeed.size();
		CGeoSurfaceBody * t_obj = new CGeoSurfaceBody;
		CGeoSurfaceBody * twin_obj = new CGeoSurfaceBody;
		t_obj->setSurfacetype(SurfaceType::Fault);
		t_obj->setName(vfSeed[i].first);
		twin_obj->setName(vfSeed[i].first);

		t_obj->setFace2faceID(vhSeed.size() + i);
		twin_obj->setFace2faceID(vhSeed.size() + i);

		faultQue.push_back(t_obj);
		twin_obj->setSurfacetype(SurfaceType::Fault);
		faultQue.push_back(twin_obj);

		// ���������Χ��
		BoundBox boundBox;

		for(int j = 0; j < vfSeed[i].second.size(); j ++) {
			CPoint3 p = vfSeed[i].second[j];
			boundBox.xMin = min(boundBox.xMin, p.x());
			boundBox.yMin = min(boundBox.yMin, p.y());
			boundBox.zMin = min(boundBox.zMin, p.z());
			boundBox.xMax = max(boundBox.xMax, p.x());
			boundBox.yMax = max(boundBox.yMax, p.y());
			boundBox.zMax = max(boundBox.zMax, p.z());
		}
		boundBox.xMin = (double) 0.8 * (i + 1) / fLen;
		boundBox.xMax = (double) 0.8 * (i + 1) / fLen;
		boundBox.yMin = 0;
		boundBox.yMax = 1;
		boundBox.zMin = 0.1;
		boundBox.zMax = 0.9;
		boundBox.setVertices();

		t_obj->setBoundBox(boundBox);
		t_obj->setID(m_vGeoGraphBody.surfaceObj.size());

		m_vGeoGraphBody.surfaceObj.push_back(t_obj);

		twin_obj->setBoundBox(boundBox);
		twin_obj->setID(m_vGeoGraphBody.surfaceObj.size());

		m_vGeoGraphBody.surfaceObj.push_back(twin_obj);

		t_obj->setTwinFaceID(twin_obj->getID());
		twin_obj->setTwinFaceID(t_obj->getID());

		t_obj->setRealID(m_vGeoGraphBody.surfaceObj.size() / 2);
		twin_obj->setRealID(m_vGeoGraphBody.surfaceObj.size() / 2);


		// ��������λ��Ϊ�ϲ�����±߽�
		// �����±߽�
		CGeoSurfaceBody * h_obj = new CGeoSurfaceBody;
		horQue.push_back(h_obj);

		BoundBox boundBoxH;

		boundBoxH.xMin = boundBox.xMin;
		boundBoxH.yMin = boundBox.yMin;
		boundBoxH.xMax = boundBox.xMax;
		boundBoxH.yMax = boundBox.yMax;

		boundBoxH.zMin = boundBox.zMin;
		boundBoxH.zMax = boundBox.zMin;
		boundBoxH.setVertices();

		h_obj->setSurfacetype(SurfaceType::Horizon);
		h_obj->setBoundBox(boundBoxH);
		h_obj->setID(m_vGeoGraphBody.surfaceObj.size());
		h_obj->isBorder = true;
		m_vGeoGraphBody.surfaceObj.push_back(h_obj);
		// �����ϱ߽�
		CGeoSurfaceBody * h_obj2 = new CGeoSurfaceBody;
		horQue.push_back(h_obj2);

		BoundBox boundBoxH2;

		boundBoxH2.xMin = boundBox.xMin;
		boundBoxH2.yMin = boundBox.yMin;
		boundBoxH2.xMax = boundBox.xMax;
		boundBoxH2.yMax = boundBox.yMax;

		boundBoxH2.zMin = boundBox.zMax;
		boundBoxH2.zMax = boundBox.zMax;
		boundBoxH2.setVertices();

		h_obj2->setSurfacetype(SurfaceType::Horizon);
		h_obj2->setBoundBox(boundBoxH2);
		h_obj2->setID(m_vGeoGraphBody.surfaceObj.size());
		h_obj2->isBorder = true;

		m_vGeoGraphBody.surfaceObj.push_back(h_obj2);

	}

	// ��λ��������ɴ�С
	for(int i = 0; i < horQue.size(); i ++)
	{
		CGeoSurfaceBody* temp = horQue[i];//�����м����temp������Ḳ��
		for( int j = i - 1;j >= 0; --j)
		{
			if(temp->getBoundBox().zMin < horQue[j]->getBoundBox().zMin)
			{
				horQue[j +1] = horQue[j];//����
				horQue[j] = temp;
			}
			else
			{
				break;
			}
		}

	}
	// ���ò�λʱ��˳�򣬴��µ��ϵ���
	for(int i = 0; i < horQue.size(); i ++) {
		horQue[i]->setHTimeRecord(i);
		horQue[i]->setFTimeRecord(-1);
	}

	// ���öϲ���ʱ��˳�򣬴����ҵ���
	for(int i = 0; i < faultQue.size(); i ++) {
		faultQue[i]->setFTimeRecord(i);
		faultQue[i]->setHTimeRecord(-1);
	}
	//3. ʶ���������ཻ��ϵ

	// �������ཻ�¼�������ʱ��˳�������Ǳ߽�����߽����ཻ����λ��������߽������λ���ཻ����λ����ϲ����ཻ

	for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++)
	{
		CGeoSurfaceBody * t_obj = (CGeoSurfaceBody *)m_vGeoGraphBody.surfaceObj[i];
		for(int j = i + 1; j <m_vGeoGraphBody.surfaceObj.size(); j ++)
		{
			CGeoSurfaceBody * s_obj = (CGeoSurfaceBody *)m_vGeoGraphBody.surfaceObj[j];
			// �����治���ཻ
			if(t_obj->getTwinFaceID() == s_obj->getID() && s_obj->getTwinFaceID() == t_obj->getID()) {
				continue;
			}
			//4. �����ཻ�������ߺͽ���ʵ��
			if((t_obj->getSurfaceType() == Fault &&  s_obj->getSurfaceType() == Horizon) ||
				(s_obj->getSurfaceType() == Fault &&  t_obj->getSurfaceType() == Horizon)) 
			{
				int ind1 = t_obj->getFace2faceID();
				int ind2 = s_obj->getFace2faceID();
				if(ind1 >= 0 && ind2 >= 0) {
					int isCrossFlag = face2face[ind1][ind2];
					if(isCrossFlag == 0) continue;
				} 
				
			}
			if(!t_obj->CrossSurface(s_obj))  continue;

			// ���ڽӹ�ϵ
			s_obj->addCrossSurface(t_obj);
			t_obj->addCrossSurface(s_obj);
			s_obj->addAdjsID(t_obj->getID());
			t_obj->addAdjsID(s_obj->getID());

			//��λ�Ͷϲ��ཻ���ڳ���ֻ����һ�����ߣ����ڽ���ϲ�+�������ɺ󣬸��ݶϲ��¼����ɶ��ཻ��			
			if((t_obj->getSurfaceType() == Fault &&  s_obj->getSurfaceType() == Horizon) ||
			   (s_obj->getSurfaceType() == Fault &&  t_obj->getSurfaceType() == Horizon))
			{
				CGeoCrossLineBody * line1 = new CGeoCrossLineBody;

				// ������������
				line1->setID(m_vGeoGraphBody.lineObj.size());
				line1->setLineType(faultline);

				// ���߸�����ϵ
				line1->addParentsID(s_obj->getID());
				line1->addParentsID(t_obj->getID());

				// �游����ϵ
				s_obj->addCrossLine(line1);
				t_obj->addCrossLine(line1);

				s_obj->addChildrenID(line1->getID());
				t_obj->addChildrenID(line1->getID());

				m_vGeoGraphBody.lineObj.push_back(line1);

			} 
			//�߽�����߽����ཻ������һ������
			else if(t_obj->getSurfaceType() != Fault && t_obj->getSurfaceType() != Horizon &&  s_obj->getSurfaceType() != Horizon &&  s_obj->getSurfaceType() != Fault){
				
				CGeoCrossLineBody * line = new CGeoCrossLineBody;

				// ������������
				line->setID(m_vGeoGraphBody.lineObj.size());
				line->setLineType(frameline);

				// ���߸�����ϵ
				line->addParentsID(s_obj->getID());
				line->addParentsID(t_obj->getID());

				// �游����ϵ
				s_obj->addCrossLine(line);
				t_obj->addCrossLine(line);

				s_obj->addChildrenID(line->getID());
				t_obj->addChildrenID(line->getID());

				m_vGeoGraphBody.lineObj.push_back(line);

			} 
			// ��λ��߽����ཻ������һ�����ߣ����һὫ�߽���ָ�Ϊ��������
			else if((t_obj->getSurfaceType() == Horizon 
				&& s_obj->getSurfaceType() != Horizon 
				&& s_obj->getSurfaceType() != Fault)
				|| (s_obj->getSurfaceType() == Horizon 
				&& t_obj->getSurfaceType() != Horizon 
				&& t_obj->getSurfaceType() != Fault)) 
			{
				CGeoCrossLineBody * line = new CGeoCrossLineBody;
				// ������������
				line->setID(m_vGeoGraphBody.lineObj.size());
				line->setLineType(frameline);

				// ���߸�����ϵ
				line->addParentsID(s_obj->getID());
				line->addParentsID(t_obj->getID());

				// �游����ϵ
				s_obj->addCrossLine(line);
				t_obj->addCrossLine(line);

				s_obj->addChildrenID(line->getID());
				t_obj->addChildrenID(line->getID());

				m_vGeoGraphBody.lineObj.push_back(line);
			}
			// �ϲ���߽��ཻ������һ������
			else if((t_obj->getSurfaceType() == Fault 
				&& s_obj->getSurfaceType() != Horizon 
				&& s_obj->getSurfaceType() != Fault)
				|| (s_obj->getSurfaceType() == Fault 
				&& t_obj->getSurfaceType() != Horizon 
				&& t_obj->getSurfaceType() != Fault)) 
			{
				CGeoCrossLineBody * line = new CGeoCrossLineBody;

				// ������������
				line->setID(m_vGeoGraphBody.lineObj.size());
				line->setLineType(frameline);

				// ���߸�����ϵ
				line->addParentsID(s_obj->getID());
				line->addParentsID(t_obj->getID());

				// �游����ϵ
				s_obj->addCrossLine(line);
				t_obj->addCrossLine(line);

				s_obj->addChildrenID(line->getID());
				t_obj->addChildrenID(line->getID());

				m_vGeoGraphBody.lineObj.push_back(line);
			}
		}
	}
	
	//5. ���ƽ��㣬���ƽ���

	// �ߵ��ڽӹ�ϵ
	for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++) {
		CGeoSurfaceBody* curFace = m_vGeoGraphBody.surfaceObj[i];
		int curFaceID = curFace->getID();
		vector<int> childLinesID = curFace->getChildrenID();
		for(int j = 0; j < childLinesID.size(); j ++) {
			CGeoCrossLineBody* line1 = m_vGeoGraphBody.getLineObjByID(childLinesID[j]);
			int line1ID = childLinesID[j];
			int wife1ID = curFace->getWifeSurfaceIDByChildID(line1ID);
			if(wife1ID == -1) continue;
			CGeoSurfaceBody* wife1Surface = m_vGeoGraphBody.getSurfaceObjByID(wife1ID);
			for(int k = j + 1; k < childLinesID.size(); k ++) {
				CGeoCrossLineBody* line2 = m_vGeoGraphBody.getLineObjByID(childLinesID[k]);
				int line2ID = childLinesID[k];
				int wife2ID = curFace->getWifeSurfaceIDByChildID(line2ID);
				if(wife2ID == -1) continue;
				CGeoSurfaceBody* wife2Surface = m_vGeoGraphBody.getSurfaceObjByID(wife2ID);

				if(wife1Surface->isConnected(wife2ID, wife1Surface->getAdjsID())) {

					// �����������޷��ཻ
					if(wife1Surface->getTwinFaceID() == wife2Surface->getID() && 
						wife2Surface->getTwinFaceID() == wife1Surface->getID()) {
							continue;
					}
					line1->addAdjsID(line2ID);
					line2->addAdjsID(line1ID);

					line1->addCrossLine(line2);
					line2->addCrossLine(line1);
				}
			}
		}
	}
	
	//����ϲ�
	// ��ӵ㣬����ÿ����ĸ������߼���
	for(int i = 0; i < m_vGeoGraphBody.lineObj.size(); i ++) {
		CGeoCrossLineBody* line1 = m_vGeoGraphBody.lineObj[i];
		
		vector<CGeoPointBody*> subsetPoints; // һ�������м����㼯
		for(int j = 0; j < m_vGeoGraphBody.lineObj.size(); j ++) {
			if(i == j) continue;
			CGeoCrossLineBody* line2 = m_vGeoGraphBody.lineObj[j];
			
			if(line1->isConnected(line2->getID(), line1->getAdjsID())) {
				if(subsetPoints.size() == 0) {
					CGeoPointBody * p1 = new CGeoPointBody;
					// ����������
					p1->setID(m_vGeoGraphBody.pointObj.size());
					p1->setPointType(FrameCrossPoint);
					m_vGeoGraphBody.pointObj.push_back(p1);

					// �㸸����ϵ
					p1->addParentsID(line1->getID());
					p1->addParentsID(line2->getID());

					// �����Ӽ���ϵ
					line1->addChildrenID(p1->getID());
					line2->addChildrenID(p1->getID());

					subsetPoints.push_back(p1);
				} else {
					bool findFlag = false;
					for(int k = 0; k < subsetPoints.size(); k ++) {
						int wife1ID = line1->getWifeLineIDByChildID(subsetPoints[k]->getID());
						if(wife1ID == -1) continue;
						if(line2->isConnected(wife1ID, line2->getAdjsID())) {
							subsetPoints[k]->addParentsID(line2->getID());
							line2->addChildrenID(subsetPoints[k]->getID());
							findFlag = true;
						} 
					}
					if(!findFlag) {
						CGeoPointBody * p1 = new CGeoPointBody;
						// ����������
						p1->setID(m_vGeoGraphBody.pointObj.size());
						p1->setPointType(FrameCrossPoint);
						m_vGeoGraphBody.pointObj.push_back(p1);

						// �㸸����ϵ
						p1->addParentsID(line1->getID());
						p1->addParentsID(line2->getID());

						// �����Ӽ���ϵ
						line1->addChildrenID(p1->getID());
						line2->addChildrenID(p1->getID());

						subsetPoints.push_back(p1);
					}
				}
			}
		}
	}

	// �ϲ�����,���������ĸ������߼�����ͬ����Ϊͬһ���㣬��Ҫ�ϲ�

	vector<CGeoPointBody*> newPointObjSet;
	for(int i = 0; i < m_vGeoGraphBody.pointObj.size(); i ++) {
		CGeoPointBody* p1 = m_vGeoGraphBody.pointObj[i];
		int p1ID = p1->getID();

		if(i == 0) {
			newPointObjSet.push_back(p1);
			continue;
		}
		bool findSameFlag = false;
		for(int j = 0; j < newPointObjSet.size(); j ++) {
			CGeoPointBody* p2 = newPointObjSet[j];
			int p2ID = p2->getID();

			if(p1->compareTwoCollect(p1->getParentsID(), p2->getParentsID())) {
				
				// �㸸����ϵ
				vector<int> parentsID = p1->getParentsID();
				// �����Ӽ���ϵ
				for(int k = 0; k < parentsID.size(); k ++) {
					CGeoCrossLineBody* line = m_vGeoGraphBody.getLineObjByID(parentsID[k]);
					vector<int> newChildrenID;
					for(int m = 0; m < line->getChildrenID().size(); m ++) {
						int cID = line->getChildrenID()[m];
						
						if(cID != p1ID) {
							newChildrenID.push_back(cID);
						}
					}
					
					line->setChildrenID(newChildrenID);
				}
				findSameFlag = true;
				break;
			} 
		}
		if(!findSameFlag) {
			newPointObjSet.push_back(p1);
		}
		
	}
	// �Ƿ���Ҫ��������
	m_vGeoGraphBody.pointObj = newPointObjSet;

	// �����ϵ
	for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++) {
		CGeoSurfaceBody* face = m_vGeoGraphBody.surfaceObj[i];
		vector<int> linesID = face->getChildrenID();
		for(int j = 0; j < linesID.size(); j ++) {
			CGeoCrossLineBody* line = m_vGeoGraphBody.getLineObjByID(linesID[j]);
			vector<int> pointsID = line->getChildrenID();
			for(int k = 0; k < pointsID.size(); k ++) {
				face->addChildrenPoint(pointsID[k]);
				CGeoPointBody* p = m_vGeoGraphBody.getPointObjByID(pointsID[k]);
				p->addParentFacesID(face->getID());
			}
		}
	}
	// ���������ϵĵ�����������
	for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++) {
		CGeoSurfaceBody* face = m_vGeoGraphBody.surfaceObj[i];
		int twinFaceID = face->getTwinFaceID();
		if(twinFaceID != -1) {
			CGeoSurfaceBody* twinFace = m_vGeoGraphBody.getSurfaceObjByID(twinFaceID);
			vector<int> facePointsID = face->getChildrenPoint();
			vector<int> twinPointsID = twinFace->getChildrenPoint();

			for(int m = 0; m < facePointsID.size(); m ++) {
				CGeoPointBody* p1 = m_vGeoGraphBody.getPointObjByID(facePointsID[m]);
				vector<int> parentFacesID1;
				for(int u = 0; u < p1->getParentFacesID().size(); u ++) {
					if(p1->getParentFacesID()[u] != face->getID()) {
						parentFacesID1.push_back(p1->getParentFacesID()[u]);
					}
				}

				for(int n = 0; n < twinPointsID.size(); n ++) {
					CGeoPointBody* p2 = m_vGeoGraphBody.getPointObjByID(twinPointsID[n]);
					vector<int> parentFacesID2;
					for(int v = 0; v < p2->getParentFacesID().size(); v ++) {
						if(p2->getParentFacesID()[v] != twinFace->getID()) {
							parentFacesID2.push_back(p2->getParentFacesID()[v]);
						}
					}
					if(m_vGeoGraphBody.isSameArr(parentFacesID1, parentFacesID2)) {
						bool borderFlag = false;
						for(int w = 0; w < parentFacesID1.size(); w ++) {
							if(m_vGeoGraphBody.getSurfaceObjByID(parentFacesID1[w])->isBorder) {
								borderFlag = true;
								break;
							}
						}
						if(!borderFlag) {
							p1->addTwinPointID(p2->getID());
							p2->addTwinPointID(p1->getID());
						}
					}
				}
			}
		}		
	}
	//6. ���ݽ��߽�������ָ�
	// ���н��߷ָ�,top��bottom�����߽��治�ᱻ�ָ���Ϊ�߽���ָ��λ��ָ�ϲ���ָ�
	//6-1. �߽���ָ��Ҫ��left\right\front\back�߽��汻��λ��ָ���Ը��ݲ�λ����ʱ��Բ�λ����߽��ཻ���ĸ����������ָ��
	for(int i = 0; i < m_vGeoGraphBody.lineObj.size(); i ++) {
		CGeoCrossLineBody* line = m_vGeoGraphBody.lineObj[i];
		if(line->getChildrenID().size() == 2) continue;
		vector<CGeoPointBody*> pointQue;
		for(int j = 0; j < line->getChildrenID().size(); j ++) {
			int pID = line->getChildrenID()[j];
			CGeoPointBody* p = m_vGeoGraphBody.getPointObjByID(pID);
			// �õ����ڲ�λ��ʱ��˳��
			vector<int> parentLinesID = m_vGeoGraphBody.getPointObjByID(pID)->getParentsID();
			for(int k = 0; k < parentLinesID.size(); k ++) {
				if(parentLinesID[k] == line->getID()) continue;
				vector<int> parentSurfacesID = m_vGeoGraphBody.getLineObjByID(parentLinesID[k])->getParentsID();
				for(int m = 0; m < parentSurfacesID.size(); m ++) {
					CGeoSurfaceBody* parentSurface = m_vGeoGraphBody.getSurfaceObjByID(parentSurfacesID[m]);
					
					if(parentSurface->getSurfaceType() == Horizon) {
						p->setHTimeRecord(parentSurface->getHTimeRecord());
					} else if(parentSurface->getSurfaceType() == Fault) {
						p->setFTimeRecord(parentSurface->getFTimeRecord());
					} else if(parentSurface->getSurfaceType() == Top) {
						p->setHTimeRecord(9999);
					} else if(parentSurface->getSurfaceType() == Bottom) {
						p->setHTimeRecord(-9999);
					} else if(parentSurface->getSurfaceType() == Left) {
						p->setFTimeRecord(-9999);
					} else if(parentSurface->getSurfaceType() == Right) {
						p->setFTimeRecord(9999);
					} 
				}
			}
			pointQue.push_back(p);
		}
		// ����ʱ��˳��Ե㼯���򣬴�С����
		for(int j = 0; j < pointQue.size(); j ++)
		{
			CGeoPointBody* temp = pointQue[j];//�����м����temp������Ḳ��
			for( int k = j - 1;k >= 0; --k)
			{
				if(temp->getHTimeRecord() < pointQue[k]->getHTimeRecord())
				{
					pointQue[k +1] = pointQue[k];//����
					pointQue[k] = temp;
				}
				else if(temp->getHTimeRecord() == pointQue[k]->getHTimeRecord()) {
					if(temp->getFTimeRecord() < pointQue[k]->getFTimeRecord()) {
						pointQue[k +1] = pointQue[k];//����
						pointQue[k] = temp;
					}
				} 
				else {
					break;
				}
				
			}
		}
		// �߲��
		CGeoCrossLineBody* preLine = NULL;
		
		for(int j = 0; j < pointQue.size() - 1; j ++) {
			CGeoCrossLineBody* segLine = new CGeoCrossLineBody;
			segLine->setID(m_vGeoGraphBody.lineObj.size());
			segLine->setLineType(line->getLineType());
			CGeoPointBody* p1 = pointQue[j];
			CGeoPointBody* p2 = pointQue[j + 1];
			
			// �������Ϊ�����㣬������֮��Ͽ�
			if(p1->isTwinPoint(p2->getID())|| p2->isTwinPoint(p1->getID())) {
				continue;
			}
			// ��������������
			for(int m = 0; m < p1->getParentsID().size(); m ++) {
				if(p1->getParentsID()[m] == line->getID()) continue;
				segLine->addAdjsID(p1->getParentsID()[m]);
				CGeoCrossLineBody* segLineAdj = m_vGeoGraphBody.getLineObjByID(p1->getParentsID()[m]);
				segLineAdj->addAdjsID(segLine->getID());
				segLineAdj->deleteAdjsID(line->getID());

			}
			
			for(int m = 0; m < p2->getParentsID().size(); m ++) {
				if(p2->getParentsID()[m] == line->getID()) continue;
				segLine->addAdjsID(p2->getParentsID()[m]);
				CGeoCrossLineBody* segLineAdj = m_vGeoGraphBody.getLineObjByID(p2->getParentsID()[m]);
				segLineAdj->addAdjsID(segLine->getID());
				segLineAdj->deleteAdjsID(line->getID());
			}
			if(preLine != NULL) {
				preLine->addAdjsID(segLine->getID());
			}
			preLine = segLine;

			// ���������游��
			segLine->setParentsID(line->getParentsID());
			// ����������Ӽ�
			for(int m = 0; m < line->getParentsID().size(); m ++) {
				
				CGeoSurfaceBody* face = m_vGeoGraphBody.getSurfaceObjByID(line->getParentsID()[m]);
				face->deleteChildrenID(line->getID());
				face->addChildrenID(segLine->getID());
				
			}
			// ���õ���߸���
			p1->deleteParentsID(line->getID());
			p2->deleteParentsID(line->getID());
			p1->addParentsID(segLine->getID());
			p2->addParentsID(segLine->getID());

			// ����������Ӽ�
			segLine->addChildrenID(p1->getID());
			segLine->addChildrenID(p2->getID());
			m_vGeoGraphBody.lineObj.push_back(segLine);
		}
		// ɾ���Ѿ����ֶε���ʵ��
		//m_vGeoGraphBody.deleteLineObj(line->getID());
	}

	// 6.2 ����ָ�
	int facesNum = m_vGeoGraphBody.surfaceObj.size();
	for(int i = 0; i < facesNum; i ++)
	{
		CGeoSurfaceBody * surface = (CGeoSurfaceBody *)m_vGeoGraphBody.surfaceObj[i];
		// ����������
		vector<int> linesID = surface->getChildrenID();
		// ��ȡ�������е�
		vector<int> pointsID;

		vector<vector<int>> halfLines;
			
		// ������
		for(int j = 0; j < linesID.size(); j ++) {
			CGeoCrossLineBody* line = m_vGeoGraphBody.getLineObjByID(linesID[j]);
			vector<int> halfLine1 = line->getChildrenID();
			vector<int> halfLine2 = vector<int>(halfLine1);
			reverse(halfLine2.begin(), halfLine2.end());
			halfLines.push_back(halfLine1);
			halfLines.push_back(halfLine2);
		}
		// Ѱ�����滷
		vector<vector<int>> facesID = findFaces(halfLines);

		// ��ǰ������������id
		vector<int> segFacesID;
		// �����������¼���ϵ
		for(int m = 0; m < facesID.size(); m ++) {
			CGeoSurfaceBody* segFace = new CGeoSurfaceBody;
			segFace->setID(m_vGeoGraphBody.surfaceObj.size());
			segFace->setSurfacetype(surface->getSurfaceType());
			// �������游���ϵ
			segFace->setCollectID(surface->getID());
			segFacesID.push_back(segFace->getID());
			for(int n = 0; n < facesID[m].size(); n ++) {
				// ���������Ӽ���ϵ

				vector<int> curLineChildrenID;
				int p1 = facesID[m][n];

				int p2;
				if(n == facesID[m].size() - 1) {
					p2 = facesID[m][0];
				} else {
					p2 = facesID[m][n + 1];
				}

					// �������ĸ�����
				m_vGeoGraphBody.getPointObjByID(p1)->addParentFacesID(segFace->getID());
				m_vGeoGraphBody.getPointObjByID(p2)->addParentFacesID(segFace->getID());
				m_vGeoGraphBody.getPointObjByID(p1)->deleteParentFacesID(surface->getID());
				m_vGeoGraphBody.getPointObjByID(p2)->deleteParentFacesID(surface->getID());

				segFace->addChildrenPoint(p1);
				segFace->addChildrenPoint(p2);

				curLineChildrenID.push_back(p1);
				curLineChildrenID.push_back(p2);

				int childLineID = m_vGeoGraphBody.getLineIDByChildrenID(curLineChildrenID);
				segFace->addChildrenID(childLineID);
					
				segFace->setTwinFaceID(0);

				// �����ߵĸ����ϵ
				CGeoCrossLineBody* curLine = m_vGeoGraphBody.getLineObjByID(childLineID);
				curLine->deleteParentsID(surface->getID());
				curLine->addParentsID(segFace->getID());

			}
			m_vGeoGraphBody.surfaceObj.push_back(segFace);
			surface->setLive(-1);
		}
	}
	m_vGeoGraphBody.updateSurfaceObjAdj();


	// Ѱ������������棬�����ö�Ӧ��ϵ
	for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++) {
		CGeoSurfaceBody* face1 = m_vGeoGraphBody.surfaceObj[i];
		if(face1->getTwinFaceID() == 0) {
			vector<int> childrenPointsID1 = face1->getChildrenPoint();
			for(int j = i + 1; j < m_vGeoGraphBody.surfaceObj.size(); j ++) {
				CGeoSurfaceBody* face2 = m_vGeoGraphBody.surfaceObj[j];
				if(face2->getTwinFaceID() == 0) {
					vector<int> childrenPointsID2 = face2->getChildrenPoint();
					bool findFlag = true;
					for(int m = 0; m < childrenPointsID1.size(); m ++) {
						int res = findTwinPoint(childrenPointsID1[m], childrenPointsID2);
						if(res == -1) {
							findFlag = false;
						}
					}
					if(findFlag) {
						face1->setTwinFaceID(face2->getID());
						face2->setTwinFaceID(face1->getID());
						break;
					}
				}
			}
		}
	}
	
	
	//7. ��ʶ�����ȡ
	AbstractBlockBody();

	// �������桢���ߡ��ӵ�ϲ�
	//BodySegandMerge();

	// 8 ��������ģ��
	regressionFrameSurface();
}
// Ѱ�������ֵܵ�
int GeoStructureAlgLib::findTwinPoint(int childrenPointsID1, vector<int> childrenPointsID2) {
	for(int i = 0; i < childrenPointsID2.size(); i ++) {
		int res = findP(childrenPointsID1, m_vGeoGraphBody.getPointObjByID(childrenPointsID2[i])->getTwinPointID());
		if(res != -1) {
			return res;
		}
	}
	return -1;
}
// ��ȱ���Ѱ�һ�
void dfs(int start, int v, vector<bool>* m_onStack, vector<bool>* m_marked, vector<ListPtr>* m_adj, vector<int> m_cycle, vector<int>* m_father, vector<vector<int>> & res)
{
	m_onStack->at(v) = true;//��ջ���
	m_marked->at(v) = true;

	forIt(m_adj->at(v)) {
		int ind = *it;
		if(m_father->at(v) == ind) continue;

		if(m_cycle.size() != 0) return;
		if(!m_onStack->at(*it)) {//����δ����ǣ��������޺�
			m_father->at(*it) = v;
			dfs(start, *it,m_onStack, m_marked, m_adj, m_cycle, m_father, res);
		}else if(start == *it){//�ѱ���ǣ��һ���ջ���Ȼ�л�
			m_cycle.clear();
			m_cycle.push_back(*it);

			for (int i = v; i != *it; i=m_father->at(i)) {
				m_cycle.push_back(i);
			}
			vector<int> temp;
			temp.assign(m_cycle.begin(), m_cycle.end());
			res.push_back(temp);
			int u = 0;
		}
	}
	m_onStack->at(v) = false;
}
// �ж��Ƿ���ͬһ����
bool isSameCircel(vector<int> c1, vector<int> c2) {
	if(c1.size() != c2.size()) {
		return false;
	}
	for(int i = 0; i < c1.size() - 1; i ++) {
		if(c1[i + 1] != c2[c2.size() - 1 - i]) {
			return false;
		}
	}
	return true;
}
// �ж��Ƿ����Ӽ�
bool isContain(vector<int> c1, vector<int> c2) {
	if(c1.size() == 0 || c2.size() == 0) {
		return false;
	}
	if(c1.size() == c2.size()) {
		sort(c1.begin(), c1.end());
		sort(c2.begin(), c2.end());
		for(int i = 0; i < c1.size(); i ++) {
			if(c1[i] != c2[i]) {
				return false;
			}

		}
		return true;
	}
		if(c1.size() > c2.size()) {
			for(int i = 0; i < c2.size() - 1; i ++) {
				if(!findP(c2[i], c1)) {
					return false;
				}
			}
		} else {
			return false;
		}
		
		return true;
}
// Ѱ������
vector<vector<int>> GeoStructureAlgLib::findFaces(vector<vector<int>> halfLines) {
	vector<bool>* m_marked = nullptr;//�������
	vector<int>* m_father = nullptr;//��¼���ڵ�
	vector<bool>* m_onStack = nullptr;//�Ƿ���ջ��
	vector<int> m_cycle;//��¼��

	map<int,int> pointMap;

	vector<int> pointArr;
	for(int i = 0; i < halfLines.size(); i ++) {
		for(int j = 0; j < 2; j ++) {
			int p = halfLines[i][j];
			if(i == 0) {
				pointArr.push_back(p);
				continue;
			}
			if(findP(p, pointArr) != -1) {
				continue;
			} else {
				pointArr.push_back(p);
			}
		}
	}
	for(int i = 0; i < pointArr.size(); i ++) {

		pointMap.insert(pair<int,int> (pointArr[i],i));
	}

	
	int V = pointArr.size(); // �����
	int E = halfLines.size(); // �߸���
	m_marked = new vector<bool>(V, false);
	m_father = new vector<int>(V, -1);
	m_onStack = new vector<bool>(V, false);


	vector<ListPtr>* m_adj = new vector<ListPtr>(V, nullptr);


	for (int i = 0; i < V; i++)
	{
		m_adj->at(i) = new list<int>(0);
	}

	for (int i = 0; i < E; i++)
	{
		(*m_adj)[pointMap[halfLines[i][0]]]->push_front(pointMap[halfLines[i][1]]);
	}


	
	vector<vector<int>> tempFaces;
	for (int i = 0; i < V; ++i) {
		vector<vector<int>> res;
		// ������еĻ�
		if(m_marked->at(i)) continue;
		dfs(i,i,m_onStack, m_marked, m_adj, m_cycle, m_father, res);

		for(int j = 0; j < res.size(); j ++) {
			vector<int> c1 = res[j];

			for(int k = j + 1; k < res.size(); k ++) {
				vector<int> c2 = res[k];
				// һ����ʱ���˳ʱ��Ļ��ϲ�Ϊһ��
				if(isSameCircel(c1, c2)) {
					res[j].clear();
					break;
				} 
			}
		}
		for(int j = 0; j < res.size(); j ++) {
			if(res[j].size() == 0) {
				continue;
			} 
			tempFaces.push_back(res[j]);
		}

	}
	vector<vector<int>> finalFaces;
	for(int i = 0; i < tempFaces.size(); i ++) {
		if(tempFaces[i].size() == 0) {
			continue;
		} 
		finalFaces.push_back(tempFaces[i]);
	}
	
	// idת��
	for(int i = 0; i < finalFaces.size(); i ++) {
		for(int j = 0; j < finalFaces[i].size(); j ++) {
			finalFaces[i][j] = pointArr[finalFaces[i][j]];
		}
	}

	return finalFaces;

}
//ʵ��ָ�ͺϲ��������ݵ�һ�����֪ʶ�Ƶ��ڶ���֪ʶ�����ɲ�λ����Ͷϲ�����
void GeoStructureAlgLib::BodySegandMerge()
{

}

/*
�����������ڶ����������ݹ���֪ʶͼ���������������õײ�ʵ�壨���桢���ߺͽ���ʵ�壩���Ƶ�����ʵ��
��������� 
���������
*/
void GeoStructureAlgLib::AbstractBlockBody()
{
	for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++)
	{

		CGeoSurfaceBody * face = (CGeoSurfaceBody *)m_vGeoGraphBody.surfaceObj[i];
		if(face->dfsFlag) continue;
		vector<int> facesID;
		findBlockDFS(face, facesID);
		CGeoBlockBody* block = new CGeoBlockBody;
		block->setID(m_vGeoGraphBody.bodyObj.size());
		// ���ÿ���Ӽ���
		block->setChildrenID(facesID);
		// ������ĸ�����
		for(int j = 0; j < facesID.size(); j ++) {
			CGeoSurfaceBody* f = m_vGeoGraphBody.getSurfaceObjByID(facesID[j]);
			f->addParentsID(block->getID());
		}
		m_vGeoGraphBody.bodyObj.push_back(block);

	}
	// ���ÿ����Ĺ�ϵ,����Ϊ�����棬������������
	for(int i = 0; i < m_vGeoGraphBody.bodyObj.size(); i ++) {
		CGeoBlockBody* b1 = m_vGeoGraphBody.bodyObj[i];
		vector<int> facesID1 = b1->getChildrenID();
		for(int j = i + 1; j < m_vGeoGraphBody.bodyObj.size(); j ++) {
			CGeoBlockBody* b2 = m_vGeoGraphBody.bodyObj[j];
			vector<int> facesID2 = b2->getChildrenID();
			for(int m = 0; m < facesID1.size(); m ++) {
				CGeoSurfaceBody* f1 = m_vGeoGraphBody.getSurfaceObjByID(facesID1[m]);
				for(int n = 0; n < facesID2.size(); n ++) {
					CGeoSurfaceBody* f2 = m_vGeoGraphBody.getSurfaceObjByID(facesID2[n]);
					if(f1->getTwinFaceID() == f2->getID() && f2->getTwinFaceID() == f1->getID()) {
						b1->addAdjsID(b2->getID());
						b2->addAdjsID(b1->getID());
					}
				}
			}
		}
	}

	// ������������ϲ���������߽�����кϲ�
	//vector<CGeoSurfaceBody*> realFace;
	//for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++) {
	//	CGeoSurfaceBody* face = m_vGeoGraphBody.surfaceObj[i];

	//	int twinFaceID = face->getTwinFaceID();

	//	if(twinFaceID != 0) {
	//		CGeoSurfaceBody* twinFace = m_vGeoGraphBody.getSurfaceObjByID(twinFaceID);
	//		face->mergeObj(twinFace);
	//		twinFace->setTwinFaceID(0);
	//		realFace.push_back(face);
	//	}
	//	if(face->getSurfaceType() == Top 
	//		|| face->getSurfaceType() == Left
	//		|| face->getSurfaceType() == Right
	//		|| face->getSurfaceType() == Bottom
	//		|| face->getSurfaceType() == Back
	//		|| face->getSurfaceType() == Front
	//		|| face->getSurfaceType() == Fault
	//		) {
	//		realFace.push_back(face);
	//	}
	//}

	//// ���ÿ�-�����
	//for(int i = 0; i < m_vGeoGraphBody.bodyObj.size(); i ++) {
	//	m_vGeoGraphBody.bodyObj[i]->clearChildrenID();
	//}
	//for(int i = 0; i < realFace.size(); i ++) {
	//	vector<int> blocksID = realFace[i]->getParentsID();
	//	for(int j = 0; j < blocksID.size(); j ++) {
	//		int bID = blocksID[j];
	//		CGeoBlockBody* block = m_vGeoGraphBody.getBodyObjByID(bID);
	//		block->addChildrenID(realFace[i]->getID());
	//	}
	//}
	//m_vGeoGraphBody.surfaceObj = realFace;
}
/************************************************************************/
/* ��ȱ���Ѱ��������ͨ����                                                                     */
/************************************************************************/
void GeoStructureAlgLib::findBlockDFS(CGeoSurfaceBody* curr, vector<int>& facesID) {
	if(curr->dfsFlag) return;
	facesID.push_back(curr->getID());
	curr->dfsFlag = true;
	vector<int> adjs = curr->getAdjsID();
	for(int i = 0; i < adjs.size(); i ++) {
		CGeoSurfaceBody* adjFace = m_vGeoGraphBody.getSurfaceObjByID(adjs[i]);
		findBlockDFS(adjFace, facesID);
	}
}
/*
�������������ݹ���֪ʶͼ�������������������
�������������������Ϊ�գ����Զ�����������棬�����Ϊ�գ������ָ������
���������
*/
void GeoStructureAlgLib::regressionSurface( CGeoGraphBaseObject * suface /*= NULL*/ )
{
	
}

void GeoStructureAlgLib::regressionFrameSurface( CGeoGraphBaseObject * suface /*= NULL*/ )
{
	//��һ�������ݱ߽����ɲ���������ģ��
	int row = 100;
	int col = 100;
	for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++) {
		CGeoSurfaceBody* face = m_vGeoGraphBody.surfaceObj[i];
		face->CreateFrameModel(row, col);
	}



}

GeoStructureAlgLib::GeoStructureAlgLib()
{

}

GeoStructureAlgLib::~GeoStructureAlgLib()
{

}
