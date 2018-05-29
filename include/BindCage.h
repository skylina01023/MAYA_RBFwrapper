#pragma once
#include <vector>
#include <maya/MFloatVectorArray.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPointArray.h>

using stdMatrixAry = std::vector<MMatrix>;
using stdDoubleAry = std::vector<double>;

namespace skylina {
namespace app {

class BindCage
{
public:
	BindCage() = default;
	BindCage(MObject&, MObject&,const MMatrix&,const MMatrix&,double =0.3);
	void reset( MObject&,  MObject&, const MMatrix&, const MMatrix&);
	~BindCage(){}

	void setWidth(double width, const MMatrix& cageWorldMat, const MMatrix& bObjWorldMat) {
		m_width = width;
		caculateWeight(false, m_bindCagePnts, m_bindObjectPnts, cageWorldMat, bObjWorldMat);
	}
	bool isValid() { return m_bindCagePnts.length() != 0 && m_bindObjectPnts.length() != 0; }
	MStatus compute(const MObject&, MPointArray&);
private:
	void accessTransform(const MPointArray&,const MFloatVectorArray&,stdMatrixAry&) noexcept;
	void caculateWeight(bool ,const MPointArray&, const MPointArray&, const MMatrix&, const MMatrix&) noexcept;
private:
	double m_width;
	MPointArray m_bindCagePnts;
	MFloatVectorArray m_bindCageNrms;
	MPointArray m_bindObjectPnts;
	stdMatrixAry m_bindTransform;
	std::vector<stdDoubleAry> m_bindWeight;
};

} //namespace app
} //namespace skylina