#include "GeoSurfaceBody.h"
#include "GeoCrossLineBody.h"


//�ϲ�����ֻ����������������ع��ϲ�����
void CGeoSurfaceBody::CreateFaultFrameModel()
{
	if(m_type != Fault) return;
	
	if(m_vCrossLines.size()!=2) return;

	CGeoCrossLineBody * up_line = (CGeoCrossLineBody*)m_vCrossLines[0];
	CGeoCrossLineBody * down_line = (CGeoCrossLineBody*)m_vCrossLines[1];
}


//��λ���
void CGeoSurfaceBody::CreateHorizonFrameModel()
{
	
	vector<vector<vector<SCtrlPointInfo *>>> vGrid;

	vGrid.resize(m_row);
	for(int i = 0; i < m_row; i ++) vGrid[i].resize(m_col);
	int t = 9;

	////1.�����ߣ�˳ʱ�뷽��Ϊ��Ч���򣬽�����������㽨��������ϵ
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

	//		//�ж��б��ϵĵ���Ч��������߻����ұߣ��б��ϵĵ����ϱ߻����±�
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
	//		if(t_type == RowPoint)//�б��ϵĵ�
	//		{				
	//			if(bd) //���
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
	//		else if(t_type == ColPoint)//�б��ϵĵ�
	//		{
	//			if(bd) //�ϱ�
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
	//		else //�ڲ���
	//		{
	//			
	//		}
	//	}
	//}

	////2.�����ڲ������
	//for(int r = 0; r <m_row; r ++)
	//{
	//	for(int c = 0; c < m_col; c ++)
	//	{
	//		vector<SCtrlPointInfo *> &vPoint = vGrid[r][c];

	//		for(int i = 0; i < vPoint.size(); i ++)
	//		{
	//			SCtrlPointInfo * tp = vPoint[i];
	//			
	//			if(tp->up == NULL && r > 0) //�ϲ����
	//			{

	//			}
	//			else if(tp->down == NULL && r < m_row - 1) //�²����
	//			{
	//				
	//			}
	//			else if(tp->left == NULL && c > 0) //�󲻷��
	//			{

	//			}
	//			else if(tp->right == NULL && c < m_col - 1)//�Ҳ����
	//			{

	//			}
	//		}			
	//	}
	//}
	//
	//3.���������
}

void CGeoSurfaceBody::CreateFrameModel( int row,int col )
{
	
	m_row = row;
	m_col = col;
	vector<vector<vector<SSingleLinkInfo>>> vGrid;
	
	vGrid.resize(row);
	for(int i = 0; i < row; i ++)vGrid[i].resize(col);

	//1.�����ߣ�˳ʱ�뷽��Ϊ��Ч����
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
	// �����߽����¡�ǰ�����Ҳ��ཻ
	if(t_type == SurfaceType::Top && s_type == SurfaceType::Bottom) return false;
	if(s_type == SurfaceType::Top && t_type == SurfaceType::Bottom) return false;

	if(t_type == SurfaceType::Front && s_type == SurfaceType::Back) return false;
	if(s_type == SurfaceType::Front && t_type == SurfaceType::Back) return false;

	if(t_type == SurfaceType::Left && s_type == SurfaceType::Right) return false;
	if(s_type == SurfaceType::Left && t_type == SurfaceType::Right) return false;

	// ��λ��\  �ϲ��������±߽��治�ཻ
	if((s_type == SurfaceType::Horizon || s_type == SurfaceType::Fault) && (t_type == SurfaceType::Bottom || t_type == SurfaceType::Top)) return false;
	if((t_type == SurfaceType::Horizon || t_type == SurfaceType::Fault) && (s_type == SurfaceType::Bottom || s_type == SurfaceType::Top)) return false;
	
	// ��λ�Ͳ�λ���ཻ
	if(s_type == SurfaceType::Horizon && t_type == SurfaceType::Horizon){
		return false;
	}

	// �жϿռ������ཻ�ж��ַ�����1.�ռ��Χ���жϣ�2.��ά�����߶��ཻ�жϣ�3.�ռ������ཻ�ж�
	//1. ��λ�Ͳ�λ�ཻ
	//2. ��λ�ͱ߽��ཻ
	//
	//3. ��λ�Ͷϲ��ཻ
	//��Χ���ж��ཻ
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
		// ��λ�Ͷϲ�Ĺ�ϵ�����ڽӾ������ж�
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
