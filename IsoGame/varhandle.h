#pragma once

#include "opengl.h"
#include "glm.h"

#include <stdio.h>

#include <vector>

#define MAT4_HANDLE 0
#define VEC3_HANDLE 1
#define FLOAT_HANDLE 2
#define GLUINT_HANDLE 3
#define INT_HANDLE 4
#define NO_HANDLE 5

typedef GLuint VarHandleID;

struct VarHandle
{
private:
	const char * var_name;
	VarHandleID handle;
	glm::mat4 * data_m;
	glm::vec3 * data_v;
	GLfloat * data_f;
	GLuint * data_i;
	int * data_ii;
	int handle_type = NO_HANDLE;

	const char * TAG = "VarHandle";

public:

	VarHandle() {}
	VarHandle(const char * var_name_)
	{
		var_name = var_name_;
		handle_type = NO_HANDLE;
	}
	VarHandle(const char * var_name_, glm::mat4 * data)
	{
		var_name = var_name_;
		data_m = data;
		handle_type = MAT4_HANDLE;
	}
	VarHandle(const char * var_name_, glm::vec3 * data)
	{
		var_name = var_name_;
		data_v = data;
		handle_type = VEC3_HANDLE;
	}
	VarHandle(const char * var_name_, GLfloat * data)
	{
		var_name = var_name_;
		data_f = data;
		handle_type = FLOAT_HANDLE;
	}
	VarHandle(const char * var_name_, GLuint * data)
	{
		var_name = var_name_;
		data_i = data;
		handle_type = GLUINT_HANDLE;
	}
	VarHandle(const char * var_name_, int * data)
	{
		var_name = var_name_;
		data_ii = data;
		handle_type = INT_HANDLE;
	}

	void init(GLuint program)
	{
		handle = glGetUniformLocation(program, var_name);
		printf("[%-11s] Linkning variable %s -> GLSL PROG: %i\n", TAG, var_name, program);
	}

	void load()
	{
		if (handle_type != NO_HANDLE)
		{
			if (handle_type == MAT4_HANDLE)
			{
				glm::mat4 mat = *data_m;
				glUniformMatrix4fv(handle, 1, GL_FALSE, &mat[0][0]);
			}
			else if (handle_type == VEC3_HANDLE)
				glUniform3f(handle, data_v->x, data_v->y, data_v->z);
			else if (handle_type == FLOAT_HANDLE)
				glUniform1f(handle, *data_f);
			else if (handle_type == GLUINT_HANDLE)
				glUniform1i(handle, *data_i);
			else if (handle_type == INT_HANDLE)
				glUniform1i(handle, *data_ii);
		}
	}
	void load(glm::mat4 data)
	{
		glUniformMatrix4fv(handle, 1, GL_FALSE, &data[0][0]);
	}
	void load(glm::vec3 data)
	{
		glUniform3f(handle, data.x, data.y, data.z);
	}
	void load(GLfloat data)
	{
		glUniform1f(handle, data);
	}
	void load(GLuint data)
	{
		glUniform1i(handle, data);
	}
	void load(int data)
	{
		glUniform1i(handle, data);
	}

	glm::mat4 * get_handle_data_mat4()
	{
		return data_m;
	}

	glm::vec3 * get_handle_data_ec3()
	{
		return data_v;
	}

	VarHandleID get_handle_id()
	{
		return handle;
	}

	const char * get_handle_name()
	{
		return var_name;
	}
};

struct MVPHandles
{
	VarHandle * model, * view, * projection;

	MVPHandles() {}
	MVPHandles(VarHandle * model, VarHandle * view, VarHandle *projection)
	{
		this->model = model;
		this->view = view;
		this->projection = projection;
	}
};


struct VarHandleManager
{
private:
	std::vector<VarHandle> handles;

public:
	void add_handle(VarHandle var_handle)
	{
		handles.push_back(var_handle);
	}

	void load_handles()
	{
		for (VarHandle v : handles)
			v.load();
	}

	void link_handles(GLuint prog)
	{
		for (VarHandle v : handles)
			v.init(prog);
	}

	VarHandle get_last_handle()
	{
		return handles[handles.size() - 1];
	}
};