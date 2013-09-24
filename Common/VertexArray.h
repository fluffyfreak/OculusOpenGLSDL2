// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _VERTEXARRAY_H
#define _VERTEXARRAY_H

#include <vector>
#include <glm/glm.hpp>

//allowed minimum of GL_MAX_VERTEX_ATTRIBS is 8 on ES2
//XXX could implement separate position2D, position3D
enum VertexAttrib {
	ATTRIB_POSITION  = (1u << 0),
	ATTRIB_NORMAL    = (1u << 1),
	ATTRIB_DIFFUSE   = (1u << 2),
	ATTRIB_UV0       = (1u << 3),
	//ATTRIB_UV1       = (1u << 4),
	//ATTRIB_TANGENT   = (1u << 5),
	//ATTRIB_BITANGENT = (1u << 6),
	//ATTRIB_CUSTOM?
};

typedef unsigned int AttributeSet;

/*
 * VertexArray is a multi-purpose vertex container. Users specify
 * the attributes they intend to use and then add vertices. Renderers
 * do whatever they need to do with regards to the attribute set.
 * This is not optimized for high performance drawing, but okay for simple
 * cases.
 */
class VertexArray {
public:
	//specify attributes to be used, additionally reserve space for vertices
	VertexArray(AttributeSet attribs, int size=0);
	virtual ~VertexArray();

	//check presence of an attribute
	virtual bool HasAttrib(VertexAttrib v) const;
	virtual unsigned int GetNumVerts() const;
	virtual AttributeSet GetAttributeSet() const { return m_attribs; }

	//removes vertices, does not deallocate space
	virtual void Clear();

	// don't mix these
	virtual void Add(const glm::vec3 &v);
	virtual void Add(const glm::vec3 &v, const glm::vec4 &c);
	virtual void Add(const glm::vec3 &v, const glm::vec4 &c, const glm::vec3 &normal);
	virtual void Add(const glm::vec3 &v, const glm::vec4 &c, const glm::vec2 &uv);
	virtual void Add(const glm::vec3 &v, const glm::vec2 &uv);
	virtual void Add(const glm::vec3 &v, const glm::vec3 &normal, const glm::vec2 &uv);
	//virtual void Reserve(unsigned int howmuch)

	//could make these private, but it is nice to be able to
	//add attributes separately...
	std::vector<glm::vec3> position;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec4> diffuse;
	std::vector<glm::vec2> uv0;

private:
	int m_attribs;
};

#endif
