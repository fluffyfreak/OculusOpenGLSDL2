//----------------------------------------------
// Description: 
// 
// Author : Andrew Copland.
//----------------------------------------------
#include "Geometry.h"

Geometry::MeshData::MeshData(const AttributeSet attribs, const int size) {
	mVerts = new VertexArray(attribs, size);
	mIndices.reserve(size*3*2);
}
Geometry::MeshData::~MeshData() {
	delete mVerts;
	mIndices.clear();
}

Geometry::MeshData* Geometry::CreateGrid(const float width, const float depth, const Uint32 vertsWide, const Uint32 vertsDeep)
{
	MeshData *ret = new MeshData(ATTRIB_POSITION | ATTRIB_DIFFUSE, vertsWide*vertsDeep);

	const float  halfWidth = width*0.5f;
	const float  halfDepth = width*0.5f;

	const float  dx = width/(vertsDeep - 1);
	const float  dz = depth/(vertsWide - 1);

	const float  du = 1.0f/(vertsDeep - 1);
	const float  dv = 1.0f/(vertsWide - 1);

	for (Uint32 i = 0; i < vertsWide; i++) {
		const float z = halfDepth - i*dz;
		for (Uint32 j = 0; j < vertsDeep; j++) {
			const float x = -halfWidth + j*dx;
			ret->mVerts->Add(glm::vec3(x, 0, z), glm::vec4(1,0,0,1));
		}
	}
	for (Uint32 i = 0; i < vertsWide-1; i++) {
		for (Uint32 j = 0; j < vertsDeep-1; j++) {
			ret->mIndices.push_back(i*vertsDeep+j);
			ret->mIndices.push_back(i*vertsDeep+j+1);
			ret->mIndices.push_back((i+1)*vertsDeep+j);

			ret->mIndices.push_back((i+1)*vertsDeep+j);
			ret->mIndices.push_back(i*vertsDeep+j+1);
			ret->mIndices.push_back((i+1)*vertsDeep+j+1);
		}
	}

	return ret;
}

Geometry::MeshData* Geometry::CreateBox(const float width, const float height, const float depth) 
{
    MeshData *ret = new MeshData(ATTRIB_POSITION | ATTRIB_NORMAL | ATTRIB_UV0, 24);
            
    const float w2 = 0.5f*width;
    const float h2 = 0.5f*height;
    const float d2 = 0.5f*depth;
    // front
    ret->mVerts->Add(glm::vec3(-w2, -h2, -d2), glm::vec3(0, 0, -1), glm::vec2(0, 1));
    ret->mVerts->Add(glm::vec3(-w2, +h2, -d2), glm::vec3(0, 0, -1), glm::vec2(0, 0));
    ret->mVerts->Add(glm::vec3(+w2, +h2, -d2), glm::vec3(0, 0, -1), glm::vec2(1, 0));
    ret->mVerts->Add(glm::vec3(+w2, -h2, -d2), glm::vec3(0, 0, -1), glm::vec2(1, 1));
    // back
    ret->mVerts->Add(glm::vec3(-w2, -h2, +d2), glm::vec3(0, 0, 1), glm::vec2(1, 1));
    ret->mVerts->Add(glm::vec3(+w2, -h2, +d2), glm::vec3(0, 0, 1), glm::vec2(0, 1));
    ret->mVerts->Add(glm::vec3(+w2, +h2, +d2), glm::vec3(0, 0, 1), glm::vec2(0, 0));
    ret->mVerts->Add(glm::vec3(-w2, +h2, +d2), glm::vec3(0, 0, 1), glm::vec2(1, 0));
    // top
    ret->mVerts->Add(glm::vec3(-w2, +h2, -d2), glm::vec3(0, 1, 0), glm::vec2(0, 1));
    ret->mVerts->Add(glm::vec3(-w2, +h2, +d2), glm::vec3(0, 1, 0), glm::vec2(0, 0));
    ret->mVerts->Add(glm::vec3(+w2, +h2, +d2), glm::vec3(0, 1, 0), glm::vec2(1, 0));
    ret->mVerts->Add(glm::vec3(+w2, +h2, -d2), glm::vec3(0, 1, 0), glm::vec2(1, 1));
    // bottom
    ret->mVerts->Add(glm::vec3(-w2, -h2, -d2), glm::vec3(0, -1, 0), glm::vec2(1, 1));
    ret->mVerts->Add(glm::vec3(+w2, -h2, -d2), glm::vec3(0, -1, 0), glm::vec2(0, 1));
    ret->mVerts->Add(glm::vec3(+w2, -h2, +d2), glm::vec3(0, -1, 0), glm::vec2(0, 0));
    ret->mVerts->Add(glm::vec3(-w2, -h2, +d2), glm::vec3(0, -1, 0), glm::vec2(1, 0));
    // left
    ret->mVerts->Add(glm::vec3(-w2, -h2, +d2), glm::vec3(-1, 0, 0), glm::vec2(0, 1));
    ret->mVerts->Add(glm::vec3(-w2, +h2, +d2), glm::vec3(-1, 0, 0), glm::vec2(0, 0));
    ret->mVerts->Add(glm::vec3(-w2, +h2, -d2), glm::vec3(-1, 0, 0), glm::vec2(1, 0));
    ret->mVerts->Add(glm::vec3(-w2, -h2, -d2), glm::vec3(-1, 0, 0), glm::vec2(1, 1));
    // right
    ret->mVerts->Add(glm::vec3(+w2, -h2, -d2), glm::vec3(1, 0, 0), glm::vec2(0, 1));
    ret->mVerts->Add(glm::vec3(+w2, +h2, -d2), glm::vec3(1, 0, 0), glm::vec2(0, 0));
    ret->mVerts->Add(glm::vec3(+w2, +h2, +d2), glm::vec3(1, 0, 0), glm::vec2(1, 0));
    ret->mVerts->Add(glm::vec3(+w2, -h2, +d2), glm::vec3(1, 0, 0), glm::vec2(1, 1));

	static const Uint32 indices[] = {
        0,1,2,0,2,3,
        4,5,6,4,6,7,
        8,9,10,8,10,11,
        12,13,14,12,14,15,
        16,17,18,16,18,19,
        20,21,22,20,22,23
    };
	static const Uint32 indicesSize = sizeof(indices) / sizeof(Uint32);
	ret->mIndices.insert(ret->mIndices.end(), &indices[0], &indices[indicesSize]);

    return ret;
}

Geometry::MeshData* Geometry::CreateSphere(const float radius, const int sliceCount, const int stackCount) 
{
    MeshData *ret = new MeshData(ATTRIB_POSITION | ATTRIB_NORMAL | ATTRIB_UV0, 2 + (sliceCount * stackCount));

	// Generate the vertices - add north pole, fill in the middle then add south pole.
    ret->mVerts->Add(glm::vec3(0,radius,0), glm::vec3(0,1,0), glm::vec2(0,0));
    const float phiStep = M_PI/stackCount;
    const float thetaStep = 2.0f*M_PI/sliceCount;
            
    for (int i = 1; i <= stackCount-1; i++) {
        const float phi = i*phiStep;
        for (int j = 0; j <= sliceCount; j++) {
            const float theta = j*thetaStep;
            const glm::vec3 p(  (radius*sin(phi)*cos(theta)),
								(radius*cos(phi)),
								(radius* sin(phi)*sin(theta)) );
                    
            const glm::vec3 n = glm::normalize(p);
            const glm::vec2 uv(theta/(M_PI*2), phi / M_PI);
            ret->mVerts->Add(p, n, uv);
        }
    }
	ret->mVerts->Add(glm::vec3(0,-radius,0), glm::vec3(0,-1,0), glm::vec2(0,1));

	// generate the indices
    for (Uint32 i = 1; i <= sliceCount; i++) {
        ret->mIndices.push_back(0);
        ret->mIndices.push_back(i+1);
        ret->mIndices.push_back(i);
    }
    Uint32 baseIndex = 1;
    Uint32 ringVertexCount = sliceCount + 1;
    for (Uint32 i = 0; i < stackCount-2; i++) {
        for (Uint32 j = 0; j < sliceCount; j++) {
            ret->mIndices.push_back(baseIndex + i*ringVertexCount + j);
            ret->mIndices.push_back(baseIndex + i*ringVertexCount + j+1);
            ret->mIndices.push_back(baseIndex + (i+1)*ringVertexCount + j);

            ret->mIndices.push_back(baseIndex + (i+1)*ringVertexCount + j);
            ret->mIndices.push_back(baseIndex + i*ringVertexCount + j+1);
            ret->mIndices.push_back(baseIndex + (i+1)*ringVertexCount + j + 1);
        }
    }
    Uint32 southPoleIndex = ret->mVerts->position.size() - 1;
    baseIndex = southPoleIndex - ringVertexCount;
    for (Uint32 i = 0; i < sliceCount; i++) {
        ret->mIndices.push_back(southPoleIndex);
        ret->mIndices.push_back(baseIndex+i);
        ret->mIndices.push_back(baseIndex+i+1);
    }

    return ret;
}

Geometry::MeshData* Geometry::CreateCylinder(const float bottomRadius, const float topRadius, const float height, const int sliceCount, const int stackCount) 
{
    MeshData *ret = new MeshData(ATTRIB_POSITION | ATTRIB_NORMAL | ATTRIB_UV0);

    const float stackHeight = height/stackCount;
    const float radiusStep = (topRadius - bottomRadius)/stackCount;
    const Sint32 ringCount = stackCount + 1;

    for (Sint32 i = 0; i < ringCount; i++) {
        const float y = -0.5f*height + i*stackHeight;
        const float r = bottomRadius + i*radiusStep;
        const float dTheta = 2.0f*M_PI/sliceCount;
        for (Sint32 j = 0; j <= sliceCount; j++) {

            const float c = cos(j*dTheta);
            const float s = sin(j*dTheta);

            glm::vec3 v(r*c, y, r*s);
            glm::vec2 uv((float)j/sliceCount, 1.0f - (float)i/stackCount);
            glm::vec3 t(-s, 0.0f, c);
                    
            const float dr = bottomRadius - topRadius;
            glm::vec3 bitangent(dr*c, -height, dr*s);

            glm::vec3 n = glm::normalize(glm::cross(t, bitangent));
            ret->mVerts->Add(v, n, uv);

        }
    }
    const Sint32 ringVertexCount = sliceCount + 1;
    for (Sint32 i = 0; i < stackCount; i++) {
        for (Sint32 j = 0; j < sliceCount; j++) {
            ret->mIndices.push_back(i*ringVertexCount + j);
            ret->mIndices.push_back((i+1)*ringVertexCount + j);
            ret->mIndices.push_back((i+1)*ringVertexCount + j + 1);

            ret->mIndices.push_back(i*ringVertexCount + j);
            ret->mIndices.push_back((i+1)*ringVertexCount + j + 1);
            ret->mIndices.push_back(i*ringVertexCount + j + 1);
        }
    }
    BuildCylinderTopCap(topRadius, height, sliceCount, ret);
    BuildCylinderBottomCap(bottomRadius, height, sliceCount, ret);
    return ret;
}

void Geometry::BuildCylinderTopCap(const float topRadius, const float height, const Sint32 sliceCount, MeshData *ret) 
{
	const Sint32 baseIndex = ret->mVerts->position.size();
            
    const float y = 0.5f*height;
    const float dTheta = 2.0f*M_PI/sliceCount;

    for (int i = 0; i <= sliceCount; i++) {
        const float x = topRadius*cos(i*dTheta);
        const float z = topRadius*sin(i*dTheta);

        const float u = x/height + 0.5f;
        const float v = z/height + 0.5f;
        ret->mVerts->Add(glm::vec3(x,y,z), glm::vec3(0, 1, 0), glm::vec2(u, v));
    }
    ret->mVerts->Add( glm::vec3(0,y,0), glm::vec3(0, 1, 0), glm::vec2(0.5f, 0.5f));
	const Sint32 centerIndex = ret->mVerts->position.size() - 1;
    for (int i = 0; i < sliceCount; i++) {
        ret->mIndices.push_back(centerIndex);
        ret->mIndices.push_back(baseIndex + i + 1);
        ret->mIndices.push_back(baseIndex + i);
    }
}

void Geometry::BuildCylinderBottomCap(const float bottomRadius, const float height, const Sint32 sliceCount, MeshData *ret) 
{
    const Sint32 baseIndex = ret->mVerts->position.size();

    const float y = -0.5f * height;
    const float dTheta = 2.0f * M_PI / sliceCount;

    for (int i = 0; i <= sliceCount; i++) {
        const float x = bottomRadius * cos(i * dTheta);
        const float z = bottomRadius * sin(i * dTheta);

        const float u = x / height + 0.5f;
        const float v = z / height + 0.5f;
		ret->mVerts->Add(glm::vec3(x,y,z), glm::vec3(0, -1, 0), glm::vec2(u, v));
    }
	ret->mVerts->Add( glm::vec3(0,y,0), glm::vec3(0, -1, 0), glm::vec2(0.5f, 0.5f));
    const Sint32 centerIndex = ret->mVerts->position.size() - 1;
    for (int i = 0; i < sliceCount; i++) {
        ret->mIndices.push_back(centerIndex);
        ret->mIndices.push_back(baseIndex + i );
        ret->mIndices.push_back(baseIndex + i + 1);
    }
}
