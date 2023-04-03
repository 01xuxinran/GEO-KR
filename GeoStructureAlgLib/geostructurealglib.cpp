#include "geostructurealglib.h"
#include "GeoBlockBody.h"
#include "GeoSurfaceBody.h"
#include "GeoCrossLineBody.h"
#include <map>
#include <list>
#include <stack>
//链表定义及遍历
typedef list<int>* ListPtr;
#define forIt(ListPtr) for(auto it=ListPtr->begin();it!=ListPtr->end();++it)
//vector的find方法重写
int findP(int id, vector<int> arr) {
	for(int i = 0; i < arr.size(); i ++) {
		if(arr[i] == id) {
			return i;
		}
	}
	return -1;
}
/*
功能描述：构造拓扑认知算法，输入构造解释数据，输出构造知识图谱网络
输入参数：构造解释数据，工区范围，种子数据已经做了归一化处理
输出参数：构造模型知识图谱网络
*/
void GeoStructureAlgLib::geoStructCognition( vector<pair<QString,vector<CPoint3> > > & vhSeed,vector<pair<QString,vector<CPoint3> > > & vfSeed,int row,int col,float minz,float maxz, vector<vector<int>> face2face)
{
	// 设置层位和断层的解释数据
	setHseed(vhSeed);
	setFseed(vfSeed);
	setFace2face(face2face);
 	//1. 创建6边界子面
	double borderXmin = 0;
	double borderXmax = 1;
	double borderYmin = 0;
	double borderYmax = 1;
	double borderZmin = minz;
	double borderZmax = maxz;

	// 工区边界面队列
	vector<CGeoSurfaceBody* > frameQue;

	// 层位面沉积队列
	vector<CGeoSurfaceBody* > horQue;

	// 断层面队列
	vector<CGeoSurfaceBody* > faultQue;


	for(int i = 0; i < 6; i ++)
	{
		CGeoSurfaceBody * t_obj = new CGeoSurfaceBody;

		frameQue.push_back(t_obj);

		// 设置工区面包围盒

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
		// 设置工区的时间
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

	//2. 根据断层和层位数构建子面实体
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
		

		// 设置子面包围盒
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

		// 设置子面包围盒
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


		// 添加虚拟层位作为断层的上下边界
		// 虚拟下边界
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
		// 虚拟上边界
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

	// 层位排序，深度由大到小
	for(int i = 0; i < horQue.size(); i ++)
	{
		CGeoSurfaceBody* temp = horQue[i];//采用中间变量temp，否则会覆盖
		for( int j = i - 1;j >= 0; --j)
		{
			if(temp->getBoundBox().zMin < horQue[j]->getBoundBox().zMin)
			{
				horQue[j +1] = horQue[j];//后移
				horQue[j] = temp;
			}
			else
			{
				break;
			}
		}

	}
	// 设置层位时间顺序，从下到上递增
	for(int i = 0; i < horQue.size(); i ++) {
		horQue[i]->setHTimeRecord(i);
		horQue[i]->setFTimeRecord(-1);
	}

	// 设置断层面时间顺序，从左到右递增
	for(int i = 0; i < faultQue.size(); i ++) {
		faultQue[i]->setFTimeRecord(i);
		faultQue[i]->setHTimeRecord(-1);
	}
	//3. 识别面与面相交关系

	// 面与面相交事件，具有时空顺序，首先是边界面与边界面相交、层位面沉积、边界面与层位面相交、层位面与断层面相交

	for(int i = 0; i < m_vGeoGraphBody.surfaceObj.size(); i ++)
	{
		CGeoSurfaceBody * t_obj = (CGeoSurfaceBody *)m_vGeoGraphBody.surfaceObj[i];
		for(int j = i + 1; j <m_vGeoGraphBody.surfaceObj.size(); j ++)
		{
			CGeoSurfaceBody * s_obj = (CGeoSurfaceBody *)m_vGeoGraphBody.surfaceObj[j];
			// 孪生面不会相交
			if(t_obj->getTwinFaceID() == s_obj->getID() && s_obj->getTwinFaceID() == t_obj->getID()) {
				continue;
			}
			//4. 根据相交创建交线和交点实体
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

			// 面邻接关系
			s_obj->addCrossSurface(t_obj);
			t_obj->addCrossSurface(s_obj);
			s_obj->addAdjsID(t_obj->getID());
			t_obj->addAdjsID(s_obj->getID());

			//层位和断层相交，在初期只生成一条交线，后期交点合并+子面生成后，根据断层事件生成多余交线			
			if((t_obj->getSurfaceType() == Fault &&  s_obj->getSurfaceType() == Horizon) ||
			   (s_obj->getSurfaceType() == Fault &&  t_obj->getSurfaceType() == Horizon))
			{
				CGeoCrossLineBody * line1 = new CGeoCrossLineBody;

				// 交线自身属性
				line1->setID(m_vGeoGraphBody.lineObj.size());
				line1->setLineType(faultline);

				// 交线父级关系
				line1->addParentsID(s_obj->getID());
				line1->addParentsID(t_obj->getID());

				// 面父级关系
				s_obj->addCrossLine(line1);
				t_obj->addCrossLine(line1);

				s_obj->addChildrenID(line1->getID());
				t_obj->addChildrenID(line1->getID());

				m_vGeoGraphBody.lineObj.push_back(line1);

			} 
			//边界面与边界面相交，生成一条交线
			else if(t_obj->getSurfaceType() != Fault && t_obj->getSurfaceType() != Horizon &&  s_obj->getSurfaceType() != Horizon &&  s_obj->getSurfaceType() != Fault){
				
				CGeoCrossLineBody * line = new CGeoCrossLineBody;

				// 交线自身属性
				line->setID(m_vGeoGraphBody.lineObj.size());
				line->setLineType(frameline);

				// 交线父级关系
				line->addParentsID(s_obj->getID());
				line->addParentsID(t_obj->getID());

				// 面父级关系
				s_obj->addCrossLine(line);
				t_obj->addCrossLine(line);

				s_obj->addChildrenID(line->getID());
				t_obj->addChildrenID(line->getID());

				m_vGeoGraphBody.lineObj.push_back(line);

			} 
			// 层位与边界面相交，生成一条交线，并且会将边界面分割为两个子面
			else if((t_obj->getSurfaceType() == Horizon 
				&& s_obj->getSurfaceType() != Horizon 
				&& s_obj->getSurfaceType() != Fault)
				|| (s_obj->getSurfaceType() == Horizon 
				&& t_obj->getSurfaceType() != Horizon 
				&& t_obj->getSurfaceType() != Fault)) 
			{
				CGeoCrossLineBody * line = new CGeoCrossLineBody;
				// 交线自身属性
				line->setID(m_vGeoGraphBody.lineObj.size());
				line->setLineType(frameline);

				// 交线父级关系
				line->addParentsID(s_obj->getID());
				line->addParentsID(t_obj->getID());

				// 面父级关系
				s_obj->addCrossLine(line);
				t_obj->addCrossLine(line);

				s_obj->addChildrenID(line->getID());
				t_obj->addChildrenID(line->getID());

				m_vGeoGraphBody.lineObj.push_back(line);
			}
			// 断层与边界相交，生成一条交线
			else if((t_obj->getSurfaceType() == Fault 
				&& s_obj->getSurfaceType() != Horizon 
				&& s_obj->getSurfaceType() != Fault)
				|| (s_obj->getSurfaceType() == Fault 
				&& t_obj->getSurfaceType() != Horizon 
				&& t_obj->getSurfaceType() != Fault)) 
			{
				CGeoCrossLineBody * line = new CGeoCrossLineBody;

				// 交线自身属性
				line->setID(m_vGeoGraphBody.lineObj.size());
				line->setLineType(frameline);

				// 交线父级关系
				line->addParentsID(s_obj->getID());
				line->addParentsID(t_obj->getID());

				// 面父级关系
				s_obj->addCrossLine(line);
				t_obj->addCrossLine(line);

				s_obj->addChildrenID(line->getID());
				t_obj->addChildrenID(line->getID());

				m_vGeoGraphBody.lineObj.push_back(line);
			}
		}
	}
	
	//5. 估计交点，估计交线

	// 线的邻接关系
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

					// 两条孪生边无法相交
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
	
	//交点合并
	// 添加点，设置每个点的父级交线集合
	for(int i = 0; i < m_vGeoGraphBody.lineObj.size(); i ++) {
		CGeoCrossLineBody* line1 = m_vGeoGraphBody.lineObj[i];
		
		vector<CGeoPointBody*> subsetPoints; // 一条线上有几个点集
		for(int j = 0; j < m_vGeoGraphBody.lineObj.size(); j ++) {
			if(i == j) continue;
			CGeoCrossLineBody* line2 = m_vGeoGraphBody.lineObj[j];
			
			if(line1->isConnected(line2->getID(), line1->getAdjsID())) {
				if(subsetPoints.size() == 0) {
					CGeoPointBody * p1 = new CGeoPointBody;
					// 点自身属性
					p1->setID(m_vGeoGraphBody.pointObj.size());
					p1->setPointType(FrameCrossPoint);
					m_vGeoGraphBody.pointObj.push_back(p1);

					// 点父级关系
					p1->addParentsID(line1->getID());
					p1->addParentsID(line2->getID());

					// 交线子级关系
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
						// 点自身属性
						p1->setID(m_vGeoGraphBody.pointObj.size());
						p1->setPointType(FrameCrossPoint);
						m_vGeoGraphBody.pointObj.push_back(p1);

						// 点父级关系
						p1->addParentsID(line1->getID());
						p1->addParentsID(line2->getID());

						// 交线子级关系
						line1->addChildrenID(p1->getID());
						line2->addChildrenID(p1->getID());

						subsetPoints.push_back(p1);
					}
				}
			}
		}
	}

	// 合并交点,如果两个点的父级交线集合相同，则为同一个点，需要合并

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
				
				// 点父级关系
				vector<int> parentsID = p1->getParentsID();
				// 交线子级关系
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
	// 是否需要垃圾回收
	m_vGeoGraphBody.pointObj = newPointObjSet;

	// 点面关系
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
	// 将孪生面上的点设置孪生点
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
	//6. 根据交线进行子面分割
	// 进行交线分割,top和bottom两个边界面不会被分割，则分为边界面分割、层位面分割、断层面分割
	//6-1. 边界面分割：主要是left\right\front\back边界面被层位面分割，可以根据层位沉积时序对层位面与边界相交的四个点进行排序分割交线
	for(int i = 0; i < m_vGeoGraphBody.lineObj.size(); i ++) {
		CGeoCrossLineBody* line = m_vGeoGraphBody.lineObj[i];
		if(line->getChildrenID().size() == 2) continue;
		vector<CGeoPointBody*> pointQue;
		for(int j = 0; j < line->getChildrenID().size(); j ++) {
			int pID = line->getChildrenID()[j];
			CGeoPointBody* p = m_vGeoGraphBody.getPointObjByID(pID);
			// 该点所在层位的时空顺序
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
		// 根据时间顺序对点集排序，从小到大
		for(int j = 0; j < pointQue.size(); j ++)
		{
			CGeoPointBody* temp = pointQue[j];//采用中间变量temp，否则会覆盖
			for( int k = j - 1;k >= 0; --k)
			{
				if(temp->getHTimeRecord() < pointQue[k]->getHTimeRecord())
				{
					pointQue[k +1] = pointQue[k];//后移
					pointQue[k] = temp;
				}
				else if(temp->getHTimeRecord() == pointQue[k]->getHTimeRecord()) {
					if(temp->getFTimeRecord() < pointQue[k]->getFTimeRecord()) {
						pointQue[k +1] = pointQue[k];//后移
						pointQue[k] = temp;
					}
				} 
				else {
					break;
				}
				
			}
		}
		// 线拆分
		CGeoCrossLineBody* preLine = NULL;
		
		for(int j = 0; j < pointQue.size() - 1; j ++) {
			CGeoCrossLineBody* segLine = new CGeoCrossLineBody;
			segLine->setID(m_vGeoGraphBody.lineObj.size());
			segLine->setLineType(line->getLineType());
			CGeoPointBody* p1 = pointQue[j];
			CGeoPointBody* p2 = pointQue[j + 1];
			
			// 如果两点为孪生点，则两点之间断开
			if(p1->isTwinPoint(p2->getID())|| p2->isTwinPoint(p1->getID())) {
				continue;
			}
			// 设置线与线相邻
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

			// 设置线与面父级
			segLine->setParentsID(line->getParentsID());
			// 设置面的线子级
			for(int m = 0; m < line->getParentsID().size(); m ++) {
				
				CGeoSurfaceBody* face = m_vGeoGraphBody.getSurfaceObjByID(line->getParentsID()[m]);
				face->deleteChildrenID(line->getID());
				face->addChildrenID(segLine->getID());
				
			}
			// 设置点的线父级
			p1->deleteParentsID(line->getID());
			p2->deleteParentsID(line->getID());
			p1->addParentsID(segLine->getID());
			p2->addParentsID(segLine->getID());

			// 设置线与点子级
			segLine->addChildrenID(p1->getID());
			segLine->addChildrenID(p2->getID());
			m_vGeoGraphBody.lineObj.push_back(segLine);
		}
		// 删除已经被分段的线实体
		//m_vGeoGraphBody.deleteLineObj(line->getID());
	}

	// 6.2 子面分割
	int facesNum = m_vGeoGraphBody.surfaceObj.size();
	for(int i = 0; i < facesNum; i ++)
	{
		CGeoSurfaceBody * surface = (CGeoSurfaceBody *)m_vGeoGraphBody.surfaceObj[i];
		// 该面所有线
		vector<int> linesID = surface->getChildrenID();
		// 获取该面所有点
		vector<int> pointsID;

		vector<vector<int>> halfLines;
			
		// 构造半边
		for(int j = 0; j < linesID.size(); j ++) {
			CGeoCrossLineBody* line = m_vGeoGraphBody.getLineObjByID(linesID[j]);
			vector<int> halfLine1 = line->getChildrenID();
			vector<int> halfLine2 = vector<int>(halfLine1);
			reverse(halfLine2.begin(), halfLine2.end());
			halfLines.push_back(halfLine1);
			halfLines.push_back(halfLine2);
		}
		// 寻找子面环
		vector<vector<int>> facesID = findFaces(halfLines);

		// 当前面的所有子面的id
		vector<int> segFacesID;
		// 构建子面上下级关系
		for(int m = 0; m < facesID.size(); m ++) {
			CGeoSurfaceBody* segFace = new CGeoSurfaceBody;
			segFace->setID(m_vGeoGraphBody.surfaceObj.size());
			segFace->setSurfacetype(surface->getSurfaceType());
			// 构建子面父面关系
			segFace->setCollectID(surface->getID());
			segFacesID.push_back(segFace->getID());
			for(int n = 0; n < facesID[m].size(); n ++) {
				// 构建子面子级关系

				vector<int> curLineChildrenID;
				int p1 = facesID[m][n];

				int p2;
				if(n == facesID[m].size() - 1) {
					p2 = facesID[m][0];
				} else {
					p2 = facesID[m][n + 1];
				}

					// 点属于哪个子面
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

				// 更新线的父面关系
				CGeoCrossLineBody* curLine = m_vGeoGraphBody.getLineObjByID(childLineID);
				curLine->deleteParentsID(surface->getID());
				curLine->addParentsID(segFace->getID());

			}
			m_vGeoGraphBody.surfaceObj.push_back(segFace);
			surface->setLive(-1);
		}
	}
	m_vGeoGraphBody.updateSurfaceObjAdj();


	// 寻找子面的孪生面，并设置对应关系
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
	
	
	//7. 块识别和提取
	AbstractBlockBody();

	// 孪生子面、子线、子点合并
	//BodySegandMerge();

	// 8 生成理论模型
	regressionFrameSurface();
}
// 寻找孪生兄弟点
int GeoStructureAlgLib::findTwinPoint(int childrenPointsID1, vector<int> childrenPointsID2) {
	for(int i = 0; i < childrenPointsID2.size(); i ++) {
		int res = findP(childrenPointsID1, m_vGeoGraphBody.getPointObjByID(childrenPointsID2[i])->getTwinPointID());
		if(res != -1) {
			return res;
		}
	}
	return -1;
}
// 深度遍历寻找环
void dfs(int start, int v, vector<bool>* m_onStack, vector<bool>* m_marked, vector<ListPtr>* m_adj, vector<int> m_cycle, vector<int>* m_father, vector<vector<int>> & res)
{
	m_onStack->at(v) = true;//入栈标记
	m_marked->at(v) = true;

	forIt(m_adj->at(v)) {
		int ind = *it;
		if(m_father->at(v) == ind) continue;

		if(m_cycle.size() != 0) return;
		if(!m_onStack->at(*it)) {//孩子未被标记，继续叠罗汉
			m_father->at(*it) = v;
			dfs(start, *it,m_onStack, m_marked, m_adj, m_cycle, m_father, res);
		}else if(start == *it){//已被标记，且还在栈里，必然有环
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
// 判断是否是同一个环
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
// 判断是否是子集
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
// 寻找子面
vector<vector<int>> GeoStructureAlgLib::findFaces(vector<vector<int>> halfLines) {
	vector<bool>* m_marked = nullptr;//标记数组
	vector<int>* m_father = nullptr;//记录父节点
	vector<bool>* m_onStack = nullptr;//是否在栈内
	vector<int> m_cycle;//记录环

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

	
	int V = pointArr.size(); // 点个数
	int E = halfLines.size(); // 边个数
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
		// 求出所有的环
		if(m_marked->at(i)) continue;
		dfs(i,i,m_onStack, m_marked, m_adj, m_cycle, m_father, res);

		for(int j = 0; j < res.size(); j ++) {
			vector<int> c1 = res[j];

			for(int k = j + 1; k < res.size(); k ++) {
				vector<int> c2 = res[k];
				// 一对逆时针和顺时针的环合并为一个
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
	
	// id转换
	for(int i = 0; i < finalFaces.size(); i ++) {
		for(int j = 0; j < finalFaces[i].size(); j ++) {
			finalFaces[i][j] = pointArr[finalFaces[i][j]];
		}
	}

	return finalFaces;

}
//实体分割和合并推理，根据第一层基础知识推到第二层知识，生成层位子面和断层子面
void GeoStructureAlgLib::BodySegandMerge()
{

}

/*
功能描述：第二层推理，根据构造知识图谱网络引导，利用底层实体（子面、交线和交点实体），推到出块实体
输入参数： 
输出参数：
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
		// 设置块的子集面
		block->setChildrenID(facesID);
		// 设置面的父级块
		for(int j = 0; j < facesID.size(); j ++) {
			CGeoSurfaceBody* f = m_vGeoGraphBody.getSurfaceObjByID(facesID[j]);
			f->addParentsID(block->getID());
		}
		m_vGeoGraphBody.bodyObj.push_back(block);

	}
	// 设置块与块的关系,子面为孪生面，则两个块相邻
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

	// 对所有孪生面合并，对虚拟边界面进行合并
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

	//// 重置块-面矩阵
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
/* 深度遍历寻找子面连通集合                                                                     */
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
功能描述：根据构造知识图谱网络引导，拟合子面
输入参数：如果输入参数为空，则自动拟合所有子面，如果不为空，则拟合指定子面
输出参数：
*/
void GeoStructureAlgLib::regressionSurface( CGeoGraphBaseObject * suface /*= NULL*/ )
{
	
}

void GeoStructureAlgLib::regressionFrameSurface( CGeoGraphBaseObject * suface /*= NULL*/ )
{
	//第一步：根据边界生成层面框架网格模型
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
