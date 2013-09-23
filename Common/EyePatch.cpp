//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------

#include "EyePatch.h"

//static 
const float EyePatch::g_vertex_buffer_data[3][4][3] =
{	
	// centre eye
	{	{ -1.0f, -1.0f, 0.0f, },
		{  1.0f, -1.0f, 0.0f, },
		{  1.0f,  1.0f, 0.0f, },
		{ -1.0f,  1.0f, 0.0f, }, },

	// left eye
	{	{ -1.0f, -1.0f, 0.0f, },
		{  0.0f, -1.0f, 0.0f, },
		{  0.0f,  1.0f, 0.0f, },
		{ -1.0f,  1.0f, 0.0f, }, },

	// right eye
	{	{  0.0f, -1.0f, 0.0f, },
		{  1.0f, -1.0f, 0.0f, },
		{  1.0f,  1.0f, 0.0f, },
		{  0.0f,  1.0f, 0.0f, }, },
};

//static 
const float EyePatch::g_uv_buffer_data[3][4][2] = 
{
	// center eye
	{	{ 0.0f, 0.0f, },
		{ 1.0f, 0.0f, },
		{ 1.0f, 1.0f, },
		{ 0.0f, 1.0f, }, },

	// left eye
	{	{ 0.0f, 0.0f, },
		{ 0.5f, 0.0f, },
		{ 0.5f, 1.0f, },
		{ 0.0f, 1.0f, }, },

	// right eye
	{	{ 0.5f, 0.0f, },
		{ 1.0f, 0.0f, },
		{ 1.0f, 1.0f, },
		{ 0.5f, 1.0f, }, }
};

// various patches for eyes.
void EyePatch::Setup(OVR::Util::Render::StereoEye eye)
{
	// load up the eye quad.
	glGenVertexArrays(1, &m_vertex_arrays);
	glBindVertexArray(m_vertex_arrays);

	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data[eye]), &g_vertex_buffer_data[eye][0][0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data[eye]), &g_uv_buffer_data[eye][0][0], GL_STATIC_DRAW);
}

void EyePatch::Render()
{
	// render the quad for the eye patch on Oculus display.
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
	glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);  

    glDrawArrays(GL_QUADS, 0, 4);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void EyePatch::Cleanup()
{
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteBuffers(1, &m_uv_buffer);
	glDeleteVertexArrays(1, &m_vertex_arrays);
}
