#pragma once
#include "BindCage.h"
#include <maya/MPxNode.h>

class RBFTransformWrapper : public MPxNode
{
public:
	RBFTransformWrapper() {}
	~RBFTransformWrapper() override {}
	MStatus compute(const MPlug&,
		MDataBlock&) override;
	static void* creator() { return new RBFTransformWrapper; }
	static MStatus initialize();

	static MObject ainWidth;
	static MObject ainCage;
	static MObject ainCageMatrix;
	static MObject ainBindMesh;
	static MObject ainBindMeshMatrix;
	static MObject aoutMesh;

	static MTypeId id;
private:
	double m_preWidth{0.3};
	MStatus initBindCage(MDataBlock&);

	skylina::app::BindCage m_bindCage;
};