#include "RBFTransfomWrapper.h"
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>

MObject RBFTransformWrapper::ainWidth;
MObject RBFTransformWrapper::ainCage;
MObject RBFTransformWrapper::ainCageMatrix;
MObject RBFTransformWrapper::ainBindMesh;
MObject RBFTransformWrapper::ainBindMeshMatrix;
MObject RBFTransformWrapper::aoutMesh;
MTypeId RBFTransformWrapper::id(0x000564da);

MStatus RBFTransformWrapper::initialize()
{
	MStatus status;
	MFnNumericAttribute numAttr;
	ainWidth = numAttr.create("width", "wi", MFnNumericData::kDouble,0.3);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MFnTypedAttribute typeAttr;
	ainCage = typeAttr.create("inCage", "ica", MFnData::kMesh, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	ainBindMesh = typeAttr.create("inBindMesh", "ibm", MFnData::kMesh, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	aoutMesh = typeAttr.create("outMesh", "om", MFnData::kMesh, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MFnMatrixAttribute matAttr;
	ainCageMatrix = matAttr.create("inCageMatrix", "icam", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	ainBindMeshMatrix = matAttr.create("inBindMeshMatrix", "ibmm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	CHECK_MSTATUS(addAttribute(ainWidth));
	CHECK_MSTATUS(addAttribute(ainCage));
	CHECK_MSTATUS(addAttribute(ainBindMesh));
	CHECK_MSTATUS(addAttribute(aoutMesh));
	CHECK_MSTATUS(addAttribute(ainCageMatrix));
	CHECK_MSTATUS(addAttribute(ainBindMeshMatrix));
	CHECK_MSTATUS(attributeAffects(ainWidth, aoutMesh));
	CHECK_MSTATUS(attributeAffects(ainCage, aoutMesh));
	CHECK_MSTATUS(attributeAffects(ainBindMesh, aoutMesh));
	CHECK_MSTATUS(attributeAffects(ainCageMatrix, aoutMesh));
	CHECK_MSTATUS(attributeAffects(ainBindMeshMatrix, aoutMesh));

	return MS::kSuccess;
}

MStatus RBFTransformWrapper::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug == aoutMesh) {
		if (!m_bindCage.isValid())
			CHECK_MSTATUS_AND_RETURN_IT(initBindCage(data));
		MStatus status;
		double& width = data.inputValue(ainWidth).asDouble();
		if (m_preWidth != width) {
			m_preWidth = width;
			MMatrix& cageMatrix = data.inputValue(ainCageMatrix, &status).asMatrix();
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MMatrix& bObjMatrix = data.inputValue(ainBindMeshMatrix, &status).asMatrix();
			CHECK_MSTATUS_AND_RETURN_IT(status);
			m_bindCage.setWidth(width, cageMatrix, bObjMatrix);
		}
		MObject& cageMesh = data.inputValue(ainCage).asMesh();
		MPointArray pnts;
		CHECK_MSTATUS_AND_RETURN_IT(m_bindCage.compute(cageMesh, pnts));
		MObject bobjMesh(data.inputValue(ainBindMesh).asMesh());
		MFnMesh meshFn(bobjMesh);
		if (meshFn.numVertices() != pnts.length()) {
			MGlobal::displayError("wrong input bind mesh!");
			return MS::kFailure;
		}
		meshFn.setPoints(pnts);
		data.outputValue(aoutMesh).set(bobjMesh);
		data.setClean(plug);
		return MS::kSuccess;
	}
	else
		return MS::kNotImplemented;
}

MStatus RBFTransformWrapper::initBindCage(MDataBlock& data)
{
	MStatus status;
	MObject& cageMesh = data.inputValue(ainCage, &status).asMesh();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MObject& bObjMesh = data.inputValue(ainBindMesh, &status).asMesh();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MMatrix& cageMatrix = data.inputValue(ainCageMatrix, &status).asMatrix();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MMatrix& bObjMatrix = data.inputValue(ainBindMeshMatrix, &status).asMatrix();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	m_bindCage.reset(cageMesh, bObjMesh, cageMatrix, bObjMatrix);

	return MS::kSuccess;
}