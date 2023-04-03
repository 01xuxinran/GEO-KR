#include "GeoSurfaceBody.h"
#include "GeoCrossLineBody.h"


//断层子面只负责根据上下盘线重构断层子面
void CGeoSurfaceBody::CreateFaultFrameModel()
{
	if(m_type != Fault) return;
	
	if(m_vCrossLines.size()!=2) return;

	CGeoCrossLineBody * up_line = (CGeoCrossLineBody*)m_vCrossLines[0];
	CGeoCrossLineBody * down_line = (CGeoCrossLineBody*)m_vCrossLines[1];
}


//层位框架
void CGeoSurfaceBody::CreateHorizonFrameModel()
{
	
	vector<vector<vector<SCtrlPointInfo *>>> vGrid;

	vGrid.resize(m_row);
	for(int i = 0; i < m_row; i ++) vGrid[i].resize(m_col);
	int t = 9;

	////1.处理交线，顺时针方向为有效方向，将交线与网格点建立关联关系
	//for(int i = 0; i < m_vCrossLine.size(); i ++)
	//{
	//	CGeoCrossLineBody * t_line = (CGeoCrossLineBody*)m_vCrossLine[i];

	//  	vector<CGeoGraphBaseObject *> vPoint = t_line->getLineData();
	//	
	//	for(int j = 0;j < vPoint.size(); j ++)
	//	{
	//		CGeoPointBody * t_point = (CGeoPointBody *)vPoint[j];
	//		PointType t_type = t_point->getPointType();
	//		

	//		//判断行边上的点有效区域是左边还是右边，列边上的点是上边还是下边
	//		bool bd = false;
	//		
	//		SCtrlPointInfo * tp = new SCtrlPointInfo;
	//		tp->p = t_point->getPoint();
	//		tp->type = 1;

	//		SCtrlPointInfo * sp = new SCtrlPointInfo;	

	//		SSingleLinkInfo * link = new SSingleLinkInfo;
	//		link->p = tp;

	//		SSingleLinkInfo * link_twin = new SSingleLinkInfo;
	//		link_twin->p = sp;

	//		link->twin = link_twin;
	//		link_twin->twin = link;

	//		sp->p = tp->p;
	//		if(t_type == RowPoint)//行边上的点
	//		{				
	//			if(bd) //左边
	//			{
	//				sp->p.y() = (int)sp->p.y();		
	//				sp->left = link;
	//			}
	//			else
	//			{
	//				sp->p.y() = (int)(sp->p.y() + 1);
	//				sp->right = link;
	//			}
	//			vGrid[sp->p.x()][sp->p.y()].push_back(sp);
	//		}
	//		else if(t_type == ColPoint)//列边上的点
	//		{
	//			if(bd) //上边
	//			{
	//				sp->p.x() = (int)sp->p.x();		
	//				sp->up = link;
	//			}
	//			else
	//			{
	//				sp->p.x() = (int)(sp->p.x() + 1);
	//				sp->down = link;
	//			}
	//		}
	//		else //内部点
	//		{
	//			
	//		}
	//	}
	//}

	////2.构建内部网格点
	//for(int r = 0; r <m_row; r ++)
	//{
	//	for(int c = 0; c < m_col; c ++)
	//	{
	//		vector<SCtrlPointInfo *> &vPoint = vGrid[r][c];

	//		for(int i = 0; i < vPoint.size(); i ++)
	//		{
	//			SCtrlPointInfo * tp = vPoint[i];
	//			
	//			if(tp->up == NULL && r > 0) //上不封闭
	//			{

	//			}
	//			else if(tp->down == NULL && r < m_row - 1) //下不封闭
	//			{
	//				
	//			}
	//			else if(tp->left == NULL && c > 0) //左不封闭
	//			{

	//			}
	//			else if(tp->right == NULL && c < m_col - 1)//右不封闭
	//			{

	//			}
	//		}			
	//	}
	//}
	//
	//3.构建多边形
}

void CGeoSurfaceBody::CreateFrameModel( int row,int col )
{
	
	m_row = row;
	m_col = col;
	vector<vector<vector<SSingleLinkInfo>>> vGrid;
	
	vGrid.resize(row);
	for(int i = 0; i < row; i ++)vGrid[i].resize(col);

	//1.处理交线，顺时针方向为有效方向
	for(int i = 0; i < m_vCrossLines.size(); i ++)
	{
		CGeoCrossLineBody * t_line = (CGeoCrossLineBody*)m_vCrossLines[i];
 
		vector<CGeoGraphBaseObject *> vPoint = t_line->getLineData();

 
	}

	if(m_type == Fault)
	{
		//CreateFaultFrameModel();
	} else if(m_type == Horizon) {
		CreateHorizonFrameModel();
	}
}

CGeoSurfaceBody::CGeoSurfaceBody(GeoBodyType type): CGeoGraphBaseObject(type)
{
	m_twinFaceID = -1;
	dfsFlag = false;
	isBorder = false;
}



CGeoSurfaceBody::~CGeoSurfaceBody(void)
{
}


bool CGeoSurfaceBody::CrossSurface(CGeoSurfaceBody * t_surface)
{
	SurfaceType t_type = t_surface->getSurfaceType();
	SurfaceType s_type = m_type;
	// 工区边界上下、前后、左右不相交
	if(t_type == SurfaceType::Top && s_type == SurfaceType::Bottom) return false;
	if(s_type == SurfaceType::Top && t_type == SurfaceType::Bottom) return false;

	if(t_type == SurfaceType::Front && s_type == SurfaceType::Back) return false;
	if(s_type == SurfaceType::Front && t_type == SurfaceType::Back) return false;

	if(t_type == SurfaceType::Left && s_type == SurfaceType::Right) return false;
	if(s_type == SurfaceType::Left && t_type == SurfaceType::Right) return false;

	// 层位面\  断层面与上下边界面不相交
	if((s_type == SurfaceType::Horizon || s_type == SurfaceType::Fault) && (t_type == SurfaceType::Bottom || t_type == SurfaceType::Top)) return false;
	if((t_type == SurfaceType::Horizon || t_type == SurfaceType::Fault) && (s_type == SurfaceType::Bottom || s_type == SurfaceType::Top)) return false;
	
	// 层位和层位不相交
	if(s_type == SurfaceType::Horizon && t_type == SurfaceType::Horizon){
		return false;
	}

	// 判断空间子面相交有多种方法：1.空间包围盒判断；2.二维测线线段相交判断；3.空间曲线相交判断
	//1. 层位和层位相交
	//2. 层位和边界相交
	//
	//3. 层位和断层相交
	//包围盒判断相交
	/*if((t_type == Fault &&  s_type == Horizon) ||
		(s_type == Fault &&  t_type == Horizon)) 
	{
		BoundBox s_bound = this->getBoundBox();
		BoundBox t_bound = t_surface->getBoundBox();
		if(s_bound.isCrossByBoundBox(t_bound) || t_bound.isCrossByBoundBox(s_bound)) {
			return true;
		} else {
			return false;
		}
	}*/
	if(((t_type == Fault &&  s_type == Horizon) ||
		(s_type == Fault &&  t_type == Horizon)) && 
		!t_surface->isBorder && !isBorder) 
	{
		// 层位和断层的关系根据邻接矩阵来判定
		return true;
	} else {
		BoundBox s_bound = this->getBoundBox();
		BoundBox t_bound = t_surface->getBoundBox();
		if(s_bound.isCrossByBoundBox(t_bound) || t_bound.isCrossByBoundBox(s_bound)) {
			return true;
		} else {
			return false;
		}
	}
	
	return true;
}
