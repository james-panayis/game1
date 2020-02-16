#pragma once

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>

#include <SDL/SDL_image.h>

#include <cstdint>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <map>

extern float projectionMatrix_[16];

namespace james {

	class coins {

	public:

		coins() {

			if (!shaderCreated_) {
				create_shader();
			}

		};

		~coins() {
			if (vertexId_) {
				glDeleteBuffers(1, &vertexId_);
				vertexId_ = 0;
			}
		};

		void remove_coin(int x, int y) {

         auto it = db.find({x,y});
			if (it == db.end()) {  return; }
			int offset = it->second * 36;

			for (int i = 0; i < 36; i+=6) {
				v[offset + i + 4] = v[offset + i];
				v[offset + i] = 0;
			}
			glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
			glBufferSubData(GL_ARRAY_BUFFER, offset * 4, 36 * 4, (char *)v.data() + offset * 4);
		}

		void reinitialize() {
			for (int j = 0; j < boxno_ * 6; j += 36) {
				if (v[j] == 0) {
					for (int i = 0; i < 36; i += 6) {
						v[j + i] = v[j + i + 4];
						v[j + i + 4] = 1 / 12.7;
						i += 6;
						v[j + i] = v[j + i + 4];
						v[j + i + 4] = 1 / 12.7 + 1/12.74;
					}
				}
			}
			bind();
		}

		void set_texture(GLuint newtex) { textureId_ = newtex; }

		void bind() {
			glGenBuffers(1, &vertexId_);
			glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
			glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);

			request_handle_ = 0;
		}

		void addbox(float x1, float y1, float x2, float y2, float X1, float Y1, float X2, float Y2) {

			db[{x1/60, y1/60}] = boxno_ / 6;
			boxno_ += 6;

			// setup the vertex data

			//note: flipped

			v.push_back(x1);
			v.push_back(y1);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1);
			v.push_back(Y1 + Y2);


			v.push_back(x1 + x2);
			v.push_back(y1);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1 + X2);
			v.push_back(Y1 + Y2);


			v.push_back(x1);
			v.push_back(y1 + y2);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1);
			v.push_back(Y1);


			v.push_back(x1 + x2);
			v.push_back(y1);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1 + X2);
			v.push_back(Y1 + Y2);


			v.push_back(x1);
			v.push_back(y1 + y2);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1);
			v.push_back(Y1);


			v.push_back(x1 + x2);
			v.push_back(y1 + y2);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1 + X2);
			v.push_back(Y1);
		}


		void display(float xPos, float yPos) {

			rot += 0.12;

			glUseProgram(program_);

			glUniformMatrix4fv(uniformProj_, 1, GL_FALSE, projectionMatrix_);

			glUniform4f(uniformOffset_, xPos, yPos, 0.0, 0.0);
			glUniform2f(uniformScale_, 1.0, 1.0);
			glUniform4f(uniformRot_, 0.0, rot, 0.0, 0.0);

			glUniform1i(attribSample_, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
			glEnableVertexAttribArray(attribPosition_);
			glEnableVertexAttribArray(attribTexcoords_);

			glVertexAttribPointer(attribPosition_, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (const GLvoid *)0);
			glVertexAttribPointer(attribTexcoords_, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (const GLvoid *)(4 * sizeof(GLfloat)));

			glBindTexture(GL_TEXTURE_2D, textureId_);
			glDrawArrays(GL_TRIANGLES, 0, boxno_);
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			//glDisableVertexAttribArray(attribPosition_);
			//glDisableVertexAttribArray(attribTexcoords_);
			//glUseProgram(0);
		};

	private:

		bool create_shader()
		{
			GLint status;

			// build the vertex shader

			std::string prog = "\
				attribute vec4 position; \
				attribute vec2 a_texCoord; \
				uniform mat4 proj; \
				uniform vec4 offset; \
				uniform vec2 scale; \
				uniform vec4 rot; \
				varying vec4 v_texCoord; \
				void main() \
				{ \
					float sin_x = sin(rot.x); \
					float cos_x = cos(rot.x); \
					float sin_y = sin(rot.y); \
					float cos_y = cos(rot.y); \
					float sin_z = sin(rot.z); \
					float cos_z = cos(rot.z); \
					mat4 a_scale = mat4(scale.x, 0.0, 0.0, 0.0, 0.0, scale.y, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0); \
					mat4 rotation = mat4( cos_y * cos_z, cos_y * sin_z, -sin_y, 0, cos_z * sin_x * sin_y - cos_x * sin_z, cos_x * cos_z + sin_x * sin_y * sin_z, cos_y * sin_x, 0, cos_x * cos_z * sin_y + sin_x * sin_z, cos_x * sin_y * sin_z - cos_z * sin_x, cos_x * cos_y, 0, 0, 0, 0, 1 ); \
					vec4 tpos = vec4(0.0, 0.0, 0.0, 0.0); \
					int txi = 0; \
					if (a_texCoord.x < 0.1) { tpos.x = -30.0; txi = 30; } \
					else { tpos.x = +30.0; txi = -30; } \
					vec4 t = (rotation * tpos) + position + vec4(txi, 0.0, 0.0, 0.0); \
					gl_Position = proj * ((a_scale * t) + offset); \
					v_texCoord = vec4(a_texCoord.x, a_texCoord.y, 0.0, 1.0); \
				} \
				";
			//					mat4 rotation = mat4(cos(rot.z), sin(rot.z), 0, 0,  -sin(rot.z), cos(rot.z), 0, 0,  0, 0, 1, 0,  0, 0, 0, 1); \
			//					vec4 t = rotation * position; \

			//gl_VertexID
			const char * c_str = prog.c_str();

			vertexShader_ = glCreateShader(GL_VERTEX_SHADER);

			glShaderSource(vertexShader_, 1, &c_str, NULL);
			glCompileShader(vertexShader_);

			glGetShaderiv(vertexShader_, GL_COMPILE_STATUS, &status);

			if (status == GL_FALSE) { std::cout << "did not compile\n"; check_compiled(vertexShader_); return false; }

			// build the fragment shader

			prog = "\
				varying highp vec4 v_texCoord; \
				uniform sampler2D s_texture; \
				void main() \
				{ \
				  gl_FragColor = texture2DProj(s_texture, v_texCoord.xyw); \
				} \
				";

			c_str = prog.c_str();

			fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);

			glShaderSource(fragmentShader_, 1, &c_str, NULL);
			glCompileShader(fragmentShader_);

			glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &status);

			if (status == GL_FALSE) { std::cout << "did not compile\n"; check_compiled(fragmentShader_); return false; }

			// link the program and store the entry points

			program_ = glCreateProgram();

			glAttachShader(program_, vertexShader_);
			glAttachShader(program_, fragmentShader_);

			glLinkProgram(program_);

			glGetProgramiv(program_, GL_LINK_STATUS, &status);

			if (status == GL_FALSE) { std::cout << "did not link\n"; return false; }

			attribPosition_ = glGetAttribLocation(program_, "position");
			attribTexcoords_ = glGetAttribLocation(program_, "a_texCoord");
			attribSample_ = glGetUniformLocation(program_, "s_texture");
			uniformProj_ = glGetUniformLocation(program_, "proj");
			uniformOffset_ = glGetUniformLocation(program_, "offset");
			uniformScale_ = glGetUniformLocation(program_, "scale");
			uniformRot_ = glGetUniformLocation(program_, "rot");

//			glUniform1i(attribSample_, 0); // try having this here rather than for each texture

			shaderCreated_ = true;

			std::cout << "Shaders created\n";
			return true;
		};


		static int check_compiled(GLuint shader) {

			GLint success = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (success == GL_FALSE) {
				GLint max_len = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_len);

				GLchar err_log[max_len];
				glGetShaderInfoLog(shader, max_len, &max_len, &err_log[0]);
				glDeleteShader(shader);

				std::cout << "Shader compilation failed: " << err_log << std::endl;
			}

			return success;
		};


		static int check_linked(GLuint program) {

			GLint success = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &success);

			if (success == GL_FALSE) {
				GLint max_len = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_len);

				GLchar err_log[max_len];
				glGetProgramInfoLog(program, max_len, &max_len, &err_log[0]);

				std::cout << "Program linking failed: " << err_log << std::endl;
			}

			return success;
		}


		GLuint textureId_ = 0; // Store the opengl texture id
		GLuint vertexId_ = 0;  // Store the opengl vertex id

		std::string url_;

		int request_handle_{ 0 };
		std::vector<float> v;
		int boxno_ = 0;
		double rot = 0;

		std::map<std::pair<int, int>, int> db;

		// Shader setings

		static bool shaderCreated_;

		static GLuint program_;

		static GLuint vertexShader_;
		static GLuint fragmentShader_;

		static GLint  attribPosition_;
		static GLint  attribTexcoords_;
		static GLint  attribSample_;

		static GLint  uniformProj_;
		static GLint  uniformOffset_;
		static GLint  uniformScale_;
		static GLint  uniformRot_;

	}; // class coins

	bool coins::shaderCreated_ = false;

	GLuint coins::program_ = 0;

	GLuint coins::vertexShader_ = 0;
	GLuint coins::fragmentShader_ = 0;

	GLint coins::attribPosition_ = 0;
	GLint coins::attribTexcoords_ = 0;
	GLint coins::attribSample_ = 0;

	GLint coins::uniformProj_ = 0;
	GLint coins::uniformOffset_ = 0;
	GLint coins::uniformScale_ = 0;
	GLint coins::uniformRot_ = 0;

} // namespace
