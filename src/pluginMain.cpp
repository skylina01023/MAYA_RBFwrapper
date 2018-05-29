#include "RBFTransfomWrapper.h"
#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "RBFTransformWraper", "2017", "Any");

	status = plugin.registerNode("RBFTransformWrapper", RBFTransformWrapper::id, RBFTransformWrapper::creator, RBFTransformWrapper::initialize);

	return status;
}

MStatus uninitializePlugin( MObject obj )
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode(RBFTransformWrapper::id);

	return status;
}
