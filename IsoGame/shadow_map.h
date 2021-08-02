#pragma once

#include "fbo.h"
#include "light.h"

struct ShadowMapper
{
	FBO fbo;
	Light light_source;
	glm::mat4 v, p;

	ShadowMapper() {}
	ShadowMapper(FBO fbo, Light light_source, glm::vec3 dir_pos, glm::vec3 up)
	{
		this->fbo = fbo;
		this->light_source = light_source;
		v = glm::lookAt(light_source.pos, dir_pos, up);
		p = glm::perspective(glm::radians(60.0f),1.0f,0.1f,1000.0f);
	}

	// Add an mesh to render on this FBO
	FBO * add_mesh(Mesh * m)
	{
		return fbo.add_mesh(m);
	}

	void render_shadowmap(VarHandle * model_handle, VarHandle * texture_handle)
	{
		fbo.binding_draw_meshes(model_handle, texture_handle);
	}

	void render_shadowmap(VarHandle * model_handle, VarHandle * texture_handle, VarHandle * view_handle, VarHandle *proj_handle)
	{
		view_handle->load(v);
		proj_handle->load(p);
		fbo.binding_draw_meshes(model_handle, texture_handle);
	}

	void load_mats(VarHandle * view_handle, VarHandle *proj_handle)
	{
		view_handle->load(v);
		proj_handle->load(p);
	}

	ShadowMapper * set_fbo(FBO fbo)
	{
		this->fbo = fbo;
		return this;
	}

	ShadowMapper * set_light_source(Light light_source)
	{
		this->light_source = light_source;
		return this;
	}

	ShadowMapper * set_mvp(glm::vec3 pos, glm::vec3 dir_pos, glm::vec3 up)
	{
		v = glm::lookAt(pos, dir_pos, up);
		p = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 1000.0f);
		return this;
	}

	ShadowMapper * set_mvp(glm::vec3 dir_pos, glm::vec3 up)
	{
		v = glm::lookAt(light_source.pos, dir_pos, up);
		p = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 1000.0f);
		return this;
	}

	glm::mat4 * get_view_mat4()
	{
		return &v;
	}

	glm::mat4 * get_proj_mat4()
	{
		return &p;
	}

	GLuint * get_depth()
	{
		return fbo.get_depth();
	}
	GLuint * get_color()
	{
		return fbo.get_tex();
	}
};