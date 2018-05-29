#include "BindCage.h"
#include <cmath>
#include <numeric>
#include <maya/MFnMesh.h>
#include <maya/MQuaternion.h>
#include <maya/MTransformationMatrix.h>

namespace skylina {
namespace app {

auto gaussFunc = [](double x, double a, double c) {return a*std::exp((-1.0*x*x) / 2.0*c*c); };

BindCage::BindCage(MObject& cage,  MObject& bindObj, const MMatrix& cageWorldMat, const MMatrix& bObjWorldMat, double width):m_width(width)
{
	reset(cage, bindObj, cageWorldMat, bObjWorldMat);
}

void BindCage::reset( MObject& cage,  MObject& bindObj, const MMatrix& cageWorldMat, const MMatrix& bObjWorldMat)
{
	MFnMesh meshFn(cage);
	meshFn.getVertexNormals(false, m_bindCageNrms, MSpace::kObject);
	meshFn.getPoints(m_bindCagePnts, MSpace::kObject);

	MFnMesh bMeshFn(bindObj);
	bMeshFn.getPoints(m_bindObjectPnts, MSpace::kObject);

	int a = m_bindObjectPnts.length();
	/*MMatrix bobjMatInverse = bObjWorldMat.inverse();
	for (unsigned i = 0; i < m_bindObjectPnts.length(); i++) {
	m_bindObjectPnts[i] *= bobjMatInverse*cageWorldMat;
	}*/

	accessTransform(m_bindCagePnts, m_bindCageNrms, m_bindTransform);
	caculateWeight(false, m_bindCagePnts, m_bindObjectPnts, cageWorldMat, bObjWorldMat);
}

void BindCage::accessTransform(const MPointArray& pnts, const MFloatVectorArray& nrms, stdMatrixAry& transMatrics) noexcept
{
	if (transMatrics.size() != pnts.length())
		transMatrics.resize(pnts.length());
	for (std::size_t i = 0; i < transMatrics.size(); ++i) {
		MTransformationMatrix matrixFn;
		matrixFn.setTranslation(pnts[i], MSpace::kTransform);
		matrixFn.setRotatePivot(pnts[i], MSpace::kTransform,false);
		MQuaternion qua(MVector(0.0, 0.0, 0.0), nrms[i]);
		matrixFn.setRotationQuaternion(qua.x, qua.y, qua.z, qua.w);
		transMatrics[i] = matrixFn.asMatrix();
	}
}

void BindCage::caculateWeight(bool isWorldSpacePnts, const MPointArray& cagePnts, const MPointArray& bObjPnts , const MMatrix& cageWorldMat, const MMatrix& bObjWorldMat) noexcept
{
	double centerLength = 0.0;
	for (unsigned i = 0; i < cagePnts.length(); ++i) {
		MPoint cagePnt = cagePnts[i];
		if (!isWorldSpacePnts)
			cagePnt *= cageWorldMat;
		for (unsigned p = 0; p < bObjPnts.length(); ++p) {
			MPoint bObjPnt = bObjPnts[p];
			if (!isWorldSpacePnts)
				bObjPnt *= bObjWorldMat;
			centerLength += bObjPnt.distanceTo(cagePnt);
		}
	}
	centerLength /= (cagePnts.length()*bObjPnts.length());

	m_bindWeight.resize(bObjPnts.length());
	for (unsigned i = 0; i < m_bindWeight.size(); i++) {
		m_bindWeight[i].resize(cagePnts.length());
		MPoint bObjPnt = bObjPnts[i];
		if (!isWorldSpacePnts)
			bObjPnt *= bObjWorldMat;
		for (unsigned p = 0; p < m_bindWeight[i].size(); p++) {
			MPoint cagePnt = cagePnts[p];
			if (!isWorldSpacePnts)
				cagePnt *= cageWorldMat;
			m_bindWeight[i][p] = gaussFunc(bObjPnt.distanceTo(cagePnt) / centerLength, 1.0, m_width);
		}
		double weightSum = std::accumulate(m_bindWeight[i].cbegin(), m_bindWeight[i].cend(), 0.0);
		for (auto& weight : m_bindWeight[i]) {
			weight /= weightSum;
		}
	}
}

MStatus BindCage::compute(const MObject& cage, MPointArray& pnts)
{
	if (pnts.length() != m_bindObjectPnts.length())
		pnts.setLength(m_bindObjectPnts.length());
	MPointArray cagePnts;
	MFloatVectorArray cageNrms;
	MFnMesh meshFn(cage);
	meshFn.getVertexNormals(false, cageNrms, MSpace::kObject);
	meshFn.getPoints(cagePnts, MSpace::kObject);

	stdMatrixAry transMatrics;
	accessTransform(cagePnts, cageNrms, transMatrics);

	for (unsigned i = 0; i < pnts.length(); i++) {
		pnts[i] = MPoint();
		for (unsigned p = 0; p < m_bindWeight[i].size(); p++) {
			pnts[i] += m_bindWeight[i][p] * (m_bindObjectPnts[i] * m_bindTransform[p].inverse()*transMatrics[p]);
		}
	}
	return MS::kSuccess;
}

}//namespace app
}//namespace skylina