#include "geomodeltest.h"
#include <fstream>
#include <cmath>
GeoModelTest::GeoModelTest(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	Test();
}

GeoModelTest::~GeoModelTest()
{

}

void GeoModelTest::Test()
{

	//1. 创建种子数据
	vector<pair<QString,vector<CPoint3> > > vSeed;
	vector<pair<QString,vector<CPoint3> > > vfSeed;

	// 导入解释数据
	loadData(vSeed, vfSeed);
	// 归一化种子数据
	normalizeData(vSeed, vfSeed);
	GeoStructureAlgLib *t_lib = new GeoStructureAlgLib;

	// 面与面之间的先验信息：邻接矩阵
	vector<vector<int>> face2face = readMatrixFile("C:\\desktop\\GeoStructureAlgLib 1008\\face2face.xls");
	t_lib->geoStructCognition(vSeed,vfSeed,11,11,0,1, face2face);

}
double GeoModelTest::normalizeNum(double n, double minN, double maxN) {
	return (n - minN) / (maxN - minN);
}
void GeoModelTest::normalizeData(vector<pair<QString,vector<CPoint3> > > &vSeed, vector<pair<QString,vector<CPoint3> > > &vfSeed)
{
	BoundBox boundBox;
	for (int i = 0; i < vSeed.size(); i ++)
	{
		for(int j = 0; j < vSeed[i].second.size(); j ++)
		{
			CPoint3 p = vSeed[i].second[j];
			boundBox.xMin = min(boundBox.xMin, p.x());
			boundBox.yMin = min(boundBox.yMin, p.y());
			boundBox.zMin = min(boundBox.zMin, p.z());
			boundBox.xMax = max(boundBox.xMax, p.x());
			boundBox.yMax = max(boundBox.yMax, p.y());
			boundBox.zMax = max(boundBox.zMax, p.z());
			
		}
	}
	for (int i = 0; i < vfSeed.size(); i ++)
	{
		for(int j = 0; j < vfSeed[i].second.size(); j ++) 
		{
			CPoint3 p = vfSeed[i].second[j];
			boundBox.xMin = min(boundBox.xMin, p.x());
			boundBox.yMin = min(boundBox.yMin, p.y());
			boundBox.zMin = min(boundBox.zMin, p.z());
			boundBox.xMax = max(boundBox.xMax, p.x());
			boundBox.yMax = max(boundBox.yMax, p.y());
			boundBox.zMax = max(boundBox.zMax, p.z());
			
		}
	}
	// z轴上下预留距离
	boundBox.zMin = boundBox.zMin - 100;
	boundBox.zMax = boundBox.zMax + 100;
	for (int i = 0; i < vSeed.size(); i ++)
	{
		for(int j = 0; j < vSeed[i].second.size(); j ++) 
		{
			CPoint3 p = vSeed[i].second[j];
			double xi = normalizeNum(p.x(), boundBox.xMin, boundBox.xMax);
			double yi = normalizeNum(p.y(), boundBox.yMin, boundBox.yMax);
			double zi = normalizeNum(p.z(), boundBox.zMin, boundBox.zMax);
			vSeed[i].second[j] = CPoint3(xi, yi, zi);
		}
	}
	for (int i = 0; i < vfSeed.size(); i ++)
	{
		for(int j = 0; j < vfSeed[i].second.size(); j ++) 
		{
			CPoint3 p = vfSeed[i].second[j];
			double xi = normalizeNum(p.x(), boundBox.xMin, boundBox.xMax);
			double yi = normalizeNum(p.y(), boundBox.yMin, boundBox.yMax);
			double zi = normalizeNum(p.z(), boundBox.zMin, boundBox.zMax);
			vfSeed[i].second[j] = CPoint3(xi, yi, zi);
		}
	}
	m_rootBoxBound = boundBox;
}

// 加载本地解释数据
void GeoModelTest::loadData(vector<pair<QString,vector<CPoint3> > > &vSeed, vector<pair<QString,vector<CPoint3> > > &vfSeed) 
{
	string rootPath = "C:\\desktop\\GeoStructureAlgLib 1008\\ZBN\\xyz\\";
	string fileType = "txt";
	vector<string> horFileName;
	horFileName.push_back("T1");
	
	vector<string> faultFileName;
	faultFileName.push_back("F1");
	// 导入层位种子数据
	for(int i = 0; i < horFileName.size(); i ++) {
		vector<CPoint3> horData = readFile(rootPath + horFileName[i] + "." + fileType);
		pair<QString,vector<CPoint3> > hor;
		hor.first = "h";
		hor.second = horData;
		vSeed.push_back(hor);
	}
	// 导入断层种子数据
	for(int i = 0; i < faultFileName.size(); i ++) {
		vector<CPoint3> faultData = readFile(rootPath + faultFileName[i] + "." + fileType);
		pair<QString,vector<CPoint3> > fault;
		fault.first = "f";
		fault.second = faultData;
		vfSeed.push_back(fault);
	}

}
// 点云文件读取
vector<CPoint3> GeoModelTest::readFile(string filePath)
{
	ifstream inFile(filePath, ios::in);
	vector<CPoint3> output;
	while (!inFile.eof()) {
		CPoint3 p;
		inFile >> p.z()
			>> p.x()
			>> p.y();
		output.push_back(p);
	}
	inFile.close();
	return output;
}
vector<string> split(const string& s, const string& seperator) {
	vector<string> result;
	unsigned int posBegin = 0;
	unsigned int posSeperator = s.find(seperator);
	
	while (posSeperator != s.size()) {
		result.push_back(s.substr(posBegin, posSeperator - posBegin));// 
		posBegin = posSeperator + seperator.size(); // 分隔符的下一个元素
		posSeperator = s.find(seperator, posBegin);
	}
	if (posBegin != s.length()) // 指向最后一个元素，加进来
		result.push_back(s.substr(posBegin));
	
	return result;
}
// 矩阵文件读取
vector<vector<int>> GeoModelTest::readMatrixFile(string filePath)
{
	ifstream infile;
	infile.open(filePath);   
	assert(infile.is_open());   
	string s;
	vector<vector<int>> output;
	int row = 0;
	while (getline(infile, s))
	{
		int col = 0;

		if(row == 0) 
		{
			row ++;
			continue;
		}
		vector<int> temp;

		for(int i = 0; i < s.size(); i ++) {
			if(s[i] == '\t') {
				col = 1;
			}
			if(s[i] != '\t' && col == 1) {
				temp.push_back(int(s[i]) - 48);
			}
		}
		output.push_back(temp);
	}
	infile.close();
	return output;
}
