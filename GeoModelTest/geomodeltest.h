#ifndef GEOMODELTEST_H
#define GEOMODELTEST_H

#include <QtWidgets/QMainWindow>
#include "ui_geomodeltest.h"
#include "geostructurealglib.h"

#include "point3.h"
class GeoModelTest : public QMainWindow
{
	Q_OBJECT

private:
	void Test();
public:
	GeoModelTest(QWidget *parent = 0);
	~GeoModelTest();
public:
	BoundBox m_rootBoxBound;
	vector<CPoint3> readFile(string filePath);
	vector<vector<int>> readMatrixFile(string filePath);
	void loadData(vector<pair<QString,vector<CPoint3> > > &vSeed, vector<pair<QString,vector<CPoint3> > > &vfSeed);
	double normalizeNum(double n, double minN, double maxN);
	void normalizeData(vector<pair<QString,vector<CPoint3> > > &vSeed, vector<pair<QString,vector<CPoint3> > > &vfSeed);

	void draw();
	void densityRender(
		vector<vector<int>> & pixelMatrix, 
		int timeStart, 
		int timeEnd, 
		int depthStart, 
		int depthEnd, 
		vector<vector<float>> & vData, 
		vector<float> & vTraceDepth, 
		float timeInterval, 
		float gain);
	float getMean(vector<float> x);
	void seisGrayRender(vector<vector<int>> & pixelMatrix, 
		float timeStart, float timeEnd, 
		float depthStart, float depthEnd, 
		vector<vector<float>> & vData, vector<float> & vTraceDepth, 
		float timeInterval, float gain, float maxValue, 
		bool grayFlag, bool lineFlag, bool polar,int _OptimizeNuber);
	float getRelevant(vector<float> x, vector<float> y);
private:
	Ui::GeoModelTestClass ui;
};

#endif // GEOMODELTEST_H
