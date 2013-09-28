//----------------------------------------------
// Description: 
// 
// Author : Andrew Copland.
//----------------------------------------------

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "sdl.h"

#include "VertexArray.h"

class Geometry {
public:
	struct MeshData {
		MeshData(const AttributeSet attribs, const int size=0);
		~MeshData();

		VertexArray *mVerts;
		std::vector<Uint32> mIndices;
	};

	static MeshData* CreateGrid(const float width, const float depth, const Uint32 vertsWide, const Uint32 vertsDeep);
	
	static MeshData* CreateBox(const float width, const float height, const float depth);

	static MeshData* CreateSphere(const float radius, const int sliceCount, const int stackCount);

	static MeshData* CreateCylinder(const float bottomRadius, const float topRadius, const float height, const int sliceCount, const int stackCount);

private:
	static void BuildCylinderTopCap(const float topRadius, const float height, const Sint32 sliceCount, MeshData *ret);
	static void BuildCylinderBottomCap(const float bottomRadius, const float height, const Sint32 sliceCount, MeshData *ret) ;
};

#endif // _GEOMETRY_H_