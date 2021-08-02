#pragma once

#include "glm.h"
#include "opengl.h"
#include "mesh.h"


namespace
{

}
// An FBO ID
typedef GLuint FBOID;

// This is an FBO struct.
// Holds the properties: FBO, texture, dimensions, meshes to render, render mesh for texture
struct FBO
{
protected:
	FBOID id;
	GLuint tex, depth;

	int w = 0, h = 0;

	std::vector<Mesh*> meshes;

	Mesh * render_mesh;

	const char * TAG = "FBO";

	// Creates a FBO and its render texture and depth buffer
	void get_frame_buffer(GLuint * FramebufferName, GLuint * colorTexture, GLuint *depthTexture)
	{
		printf("[%-11s] Creating FBO:\n", TAG);

		glGenFramebuffersEXT(1, FramebufferName);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, *FramebufferName);

		//RGBA8 2D texture, 24 bit depth texture
		glGenTextures(1, colorTexture);
		glBindTexture(GL_TEXTURE_2D, *colorTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//NULL means reserve texture memory, but texels are undefined
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		//Attach 2D texture to this FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *colorTexture, 0);

		//RGBA8 2D texture, 24 bit depth texture
		glGenTextures(1, depthTexture);
		glBindTexture(GL_TEXTURE_2D, *depthTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//NULL means reserve texture memory, but texels are undefined
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, *depthTexture, 0);

		GLuint depthBuffer;
		// The depth buffer
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		//Attach depth buffer to FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		//Does the GPU support current FBO configuration?
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			printf("[%-11s]    FBO good.\n", TAG);
			break;
		default:
			printf("[%-11s]    Error creating FBO.\n", TAG);
		}
		
		glFlush();

		printf("[%-11s]    FBO    -> %i\n", TAG, id);
		printf("[%-11s]    Tex ID -> %i\n", TAG, tex);
		//printf("[%-11s]           -> %i\n", TAG,);

	}

public:

	FBO() {}

	// Frame size
	FBO(int w, int h)
	{
		this->w = w; this->h = h;
		get_frame_buffer(&id, &tex, &depth);
	}
	// Frame size
	FBO(glm::vec2 window_size)
	{
		this->w = window_size.x; this->h = window_size.y;
		get_frame_buffer(&id, &tex, &depth);
	}

	// Set the render mesh for the texture
	void set_render_mesh(Mesh * mesh)
	{
		render_mesh = mesh;
	}

	// Binds FBO for render
	void bind()
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);
		//glClearDepth(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//glDisable(GL_TEXTURE_2D);
		//glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
	}

	// Unbinds all FBOs
	static void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}	

	// Gets texture for this FBO
	GLuint * get_tex()
	{
		return &tex;
	}

	// Gets depth buffer for this FBO
	GLuint * get_depth()
	{
		return &depth;
	}

	// Gets FBO ID
	FBOID get_fboid()
	{
		return id;
	}

	// Add an mesh to render on this FBO
	FBO * add_mesh(Mesh * m)
	{
		meshes.push_back(m);
		return this;
	}

	// Just draws the meshes for this FBO
	void draw_meshes(VarHandle * model, VarHandle * tex)
	{
		for (Mesh * m : meshes)
			m->draw(0, model, tex);
	}

	// Renders meshes to this FBO
	void binding_draw_meshes(VarHandle * model, VarHandle * tex)
	{
		bind();
		for (Mesh * m : meshes)
			m->draw(0, model, tex);
		unbind();
	}

	// Load this FBOs texture to the shader
	void activate_texture(VarHandle * handle)
	{
		handle->load(tex);
		glActiveTexture(GL_TEXTURE0 + tex);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

	// Unload this FBOs texture from the shader
	void deactivate_texture()
	{
		glActiveTexture(GL_TEXTURE0 + tex);
		glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
	}

	// Draw the texture on the render mesh (make sure ortho is used)
	void draw_render_mesh(VarHandle * model_handle, VarHandle * texture_handle)
	{
		activate_texture(texture_handle);
		render_mesh->draw(0, model_handle, nullptr);
		deactivate_texture();
	}

};

// Manages a list of FBOs
struct FBOManager
{
private:
	std::map<FBOID, FBO> fbos;

public:

	FBOManager() {}

	// Add an FBO to the list and returns its FBOID
	FBOID add_fbo(glm::vec2 size, Mesh * render_mesh)
	{
		FBO fbo = FBO(size);
		fbo.set_render_mesh(render_mesh);
		fbos.insert({ fbo.get_fboid(), fbo });
		return fbo.get_fboid();
	}

	// Get the pointer to an FBO in the list
	FBO * get_fbo(FBOID id)
	{
		return &fbos[id];
	}

};

//void get3DTexture(GLuint * tex, int w, int h, int d, GLuint * data)
//{
//	//create and bind texture
//	glGenTextures(1, tex);
//	glBindTexture(GL_TEXTURE_2D_ARRAY, *tex);
//
//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, w, h, d, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//
//	// set texture wrapping to GL_REPEAT (default wrapping method)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	for (int i = 0; i < d; ++i)
//		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
//
//}