#pragma once

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>

#include <SDL/SDL_image.h>

#include <cstdint>
#include <vector>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <functional>
#include <algorithm>

#include "font.hpp"
#include "widget_text.hpp"

#include "interface.hpp"
#include "terrain.hpp"
#include "collision_detection.hpp"

extern float projectionMatrix_[16];
extern james::font font_;
extern int viewport_width_;
extern int viewport_height_;
extern double pixel_ratio_;


namespace james {


	class player {

	public:

		player(interface& inter, collision_detection& colli) :
			interface_(inter),
			collision_detection_(colli)
		{
			if (!shaderCreated_) {
				create_shader();
			}

         collision_detection_.register_coin_collect(std::bind(&player::coin_collect, this));
			collision_detection_.register_death(std::bind(&player::death, this));
			collision_detection_.register_completion(std::bind(&player::completion, this));

		};

		~player() {
			if (textureId_) {
				glDeleteTextures(1, &textureId_);
				textureId_ = 0;
			}

			if (vertexId_) {
				glDeleteBuffers(1, &vertexId_);
				vertexId_ = 0;
			}
		};

		auto get_xpos() const { return xpos; }
		auto get_ypos() const { return ypos; }
		auto get_dead() const { return dead; }
		auto get_completed() const { return completed; }

		void set_xpos(double x) { xpos = x; }
		void set_ypos(double y) { ypos = y; }
		void soft_reset() {
			set_xpos(180); set_ypos(60);
			dead = 0;
			text_dead_->set_text("");
		}
		void hard_reset() {
			set_xpos(180); set_ypos(60);
			dead = 0;
			text_dead_->set_text("");
			completed = 0;
			text_completed_->set_text("");
			if (!text_max_score_) {
				text_max_score_ = new widget_text(&font_, "new high score: " + std::to_string(score), { 0, int(viewport_height_ * pixel_ratio_ - 60), int(viewport_width_ * pixel_ratio_), int(viewport_height_ * pixel_ratio_ - 20) }, widget_text::align::CENTER, { 1, 1, 0.5 }, { 0, 0, 0 });
			}
			if (score > max_score) {
				max_score = score;
				text_max_score_->set_text("new high score: " + std::to_string(max_score));
			}

			score = 0;
			text_score_->set_text("score: " + std::to_string(score));
		}

		void move() {

			if (interface_.keyw_ == true && collision_detection_.collision_down(ypos, ypos - 0.5, xpos, 60, imheight) == ypos) { yvel = 255; }

			if (yvel > 0) { yvel -= weight; }
			if (yvel < 0) { yvel = 0; }

			if (!dead && !completed) {
			auto tempxvd = xvel_dir_;

         if (interface_.keya_) xvel_dir_ = -1;
			if (interface_.keyd_) {
				xvel_dir_ = 1;
				if (interface_.keya_) xvel_dir_ = 0;
			}
			else if (!interface_.keya_) xvel_dir_ = 0;
			xvel_dir_old_ = (!xvel_dir_) * xvel_dir_old_ + xvel_dir_;
			if (xvel_dir_old_ == 1 && xvel != 0) {
				xvel += M_PI / 9;
				if (xvel > 0) xvel = 0;
			}
			if (xvel_dir_old_ == -1 && xvel != M_PI) {
				xvel -= M_PI / 9;
				if (xvel < -M_PI) xvel = -M_PI;
			}

				if (xvel_dir_ != 0) {
					rock += rock_dir_ * mspeed / 100;
					if (rock >= 0.15) {
						rock = 0.15;
						rock_dir_ = -1;
					} else if (rock <= -0.15) {
						rock = -0.15;
						rock_dir_ = 1;
					}
				} else rock /= 2;
				if (tempxvd == xvel_dir_) run++;
				else {
					run = 0; mspeed = 5;
				}
			} else {
				rock += 0.3;
				xvel = 0;
			}

			if (run % static_cast<int>(mspeed*mspeed) == 0 && mspeed <= 10) mspeed++;

			if (yvel > 0 && collision_detection_.collision_up(ypos, ypos + 0.5, xpos, 60, imheight) == ypos) { yvel = 0; }

			auto[xnew, ynew] = collision_detection_.detect_collision(xpos, ypos, xpos + (1-(dead || completed)) * mspeed * (interface_.keyd_ - interface_.keya_), ypos - (1-completed)*(gravity - (1-dead) * yvel/weight), 60, imheight);
			if (xvel_dir_ == 1 && collision_detection_.collision_right(xpos, xpos + 0.5, ypos, 60, imheight) == xpos) { run = 0; mspeed = 5; }
			if (xvel_dir_ == -1 && collision_detection_.collision_left(xpos, xpos - 0.5, ypos, 60, imheight) == xpos) { run = 0; mspeed = 5; }
			set_xpos(xnew);
			set_ypos(ynew);

		}

		void coin_collect() {
			score += 100;

			std::string s = "score: " + std::to_string(score);

			if (!text_score_) {
				text_score_ = new widget_text(&font_, s, { 0, int(viewport_height_ * pixel_ratio_ - 60), 300, int(viewport_height_ * pixel_ratio_ - 20) }, widget_text::align::CENTER, { 1, 1, 0.5 }, { 0, 0, 0 });
			} else {
				text_score_->set_text(s);
				if (text_max_score_) {
					text_max_score_->set_text("high score: " + std::to_string(max_score));
				}
			}
		}

		void death() {
			if (!dead) {
				dead = 1;
				score = std::max(0, int(score - 500));

				text_score_->set_text("score: " + std::to_string(score));

				std::string a = "You died. Double click to respawn.";

				if (!text_dead_) {
					text_dead_ = new widget_text(&font_, a, { int(viewport_width_ * pixel_ratio_) - 750, int(viewport_height_ * pixel_ratio_ - 60), int(viewport_width_ * pixel_ratio_), int(viewport_height_ * pixel_ratio_ - 20) }, widget_text::align::CENTER, { 1, 0, 0 }, { 0, 0, 0 });
				} else {
					text_dead_->set_text(a);
				}
			}
		}

		void completion() {
			if (!completed) {
				completed = 1;

				text_score_->set_text("score: " + std::to_string(score));

				std::string a = "You won. Double click to play again.";

				if (!text_completed_) {
					text_completed_ = new widget_text(&font_, a, { int(viewport_width_ * pixel_ratio_) - 750, int(viewport_height_ * pixel_ratio_ - 60), int(viewport_width_ * pixel_ratio_), int(viewport_height_ * pixel_ratio_ - 20) }, widget_text::align::CENTER, { 0, 1, 0 }, { 0, 0, 0 });
				} else {
					text_completed_->set_text(a);
				}
			}
		}

		void load(const std::string url) {
			url_ = url;

			request_handle_ = emscripten_async_wget2_data(url_.c_str(), "GET", "", this, EM_FALSE,

				[](unsigned int handle, void * user_data, void * data, unsigned int data_size) {
				auto handler = reinterpret_cast<player *>(user_data);
				handler->onload(handle, data, data_size);
			},

				[](unsigned int handle, void * user_data, int error_code, const char * error_msg) {
				auto handler = reinterpret_cast<player *>(user_data);
				handler->onerror(handle, error_code, error_msg);
			},

				[](unsigned int handle, void * user_data, int number_of_bytes, int total_bytes) {
				auto handler = reinterpret_cast<player *>(user_data);
				handler->onprogress(handle, number_of_bytes, total_bytes);
			}
			);

		};

		void addbox(float x1, float y1, float x2, float y2, float X1, float Y1, float X2, float Y2) {

			boxno_ += 6;

			float w = x2 / 2;
			float h = y2 / 2;

			// setup the vertex data

			//note: flipped

			v.push_back(x1 - w);
			v.push_back(y1 - h);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1);
			v.push_back(Y1 + Y2);


			v.push_back(x1 + w);
			v.push_back(y1 - h);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1 + X2);
			v.push_back(Y1 + Y2);


			v.push_back(x1 - w);
			v.push_back(y1 + h);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1);
			v.push_back(Y1);


			v.push_back(x1 + w);
			v.push_back(y1 - h);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1 + X2);
			v.push_back(Y1 + Y2);


			v.push_back(x1 - w);
			v.push_back(y1 + h);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1);
			v.push_back(Y1);


			v.push_back(x1 + w);
			v.push_back(y1 + h);
			v.push_back(0.0);
			v.push_back(1.0);

			v.push_back(X1 + X2);
			v.push_back(Y1);
		}

		void load(const std::uint8_t * data, int width, int height) {

			if (!textureId_) {

				// setup the texture

				glGenTextures(1, &textureId_);

				glBindTexture(GL_TEXTURE_2D, textureId_);

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


			} else {
				glBindTexture(GL_TEXTURE_2D, textureId_);
			}

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			glBindTexture(GL_TEXTURE_2D, 0);
		};


		void display() {

			glUseProgram(program_);

			glUniformMatrix4fv(uniformProj_, 1, GL_FALSE, projectionMatrix_);

			glUniform4f(uniformOffset_, 180 + 30, ypos + imheight / 2, 0.0, 0.0);
			glUniform2f(uniformScale_, std::max(1.0 - rock/100, 0.0), std::max(1.0 - rock / 100, 0.0));
			glUniform4f(uniformRot_, 0.0, xvel, rock, 0.0);

			glUniform1i(attribSample_, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
			glEnableVertexAttribArray(attribPosition_);
			glEnableVertexAttribArray(attribTexcoords_);

			glVertexAttribPointer(attribPosition_, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (const GLvoid *)0);
			glVertexAttribPointer(attribTexcoords_, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (const GLvoid *)(4 * sizeof(GLfloat)));

			glBindTexture(GL_TEXTURE_2D, textureId_);
			glDrawArrays(GL_TRIANGLES, 0, boxno_);

         if (text_score_) text_score_->display();
			if (text_max_score_) text_max_score_->display();
			if (text_dead_) text_dead_->display();
			if (text_completed_) text_completed_->display();
		};

	private:

		void onload(unsigned int handle, void * data, unsigned int data_size) {

			printf("got data size: %u\n", data_size);

			emscripten_run_preload_plugins_data((char *)data, data_size, "png", this,

				[](void * user_data, const char * file) {
				auto handler = reinterpret_cast<player *>(user_data);
				handler->ondecoded(file);
			},

				[](void * user_data) {
				auto handler = reinterpret_cast<player *>(user_data);
				handler->ondecodederror();
			}
			);
		};

		void ondecoded(const char * file) {
			printf("ondecoded: %s\n", file);

			SDL_Surface * image = IMG_Load(file);

			if (!image) {
				printf("IMG_Load of: %s failed with: %s\n", file, IMG_GetError());
				return;
			}

			load((std::uint8_t *)image->pixels, image->w, image->h);

			SDL_FreeSurface(image);


			addbox(0, 0, 60, imheight, 0, 0, 1, 1);


			// store the vertex on the gpu

			glGenBuffers(1, &vertexId_);
			glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
			glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);

			request_handle_ = 0;
		};


		void ondecodederror() {
			printf("ondecodederror\n");
			request_handle_ = 0;
		};

		void onerror(unsigned int handle, int error_code, const char * error_msg) {
			printf("failed to get fonts with error code: %d msg: %s\n", error_code, error_msg);

			request_handle_ = 0;
		};

		void onprogress(unsigned int handle, int number_of_bytes, int total_bytes) {
			printf("received: %d total is: %d\n", number_of_bytes, total_bytes);
		}


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
					vec4 t = rotation * position; \
					gl_Position = proj * ((a_scale * t) + offset); \
					v_texCoord = vec4(a_texCoord.x, a_texCoord.y, 0.0, 1.0); \
				} \
				";

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

		interface &interface_;
		collision_detection &collision_detection_;

		double xpos = 180;
		double ypos = 60;
		double mspeed = 5;
		int run = 0;
		double rock = 0;
		int rock_dir_ = 1;

		double weight = 8.1743;
		double gravity = 8.1743;
		double yvel = 0;

		unsigned int level = 0;
		unsigned int imheight = 60.0 * 454.0 / 322.0;

		unsigned int score = 0;
		unsigned int max_score = 0;
		bool dead = false;
		bool completed = false;

		double xvel = 0;
		int xvel_dir_ = 1;
		int xvel_dir_old_ = 1;

      widget_text* text_score_{nullptr};
		widget_text* text_max_score_{ nullptr };
		widget_text* text_dead_{ nullptr };
		widget_text* text_completed_{ nullptr };

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

	}; // class player

	bool player::shaderCreated_ = false;

	GLuint player::program_ = 0;

	GLuint player::vertexShader_ = 0;
	GLuint player::fragmentShader_ = 0;

	GLint player::attribPosition_ = 0;
	GLint player::attribTexcoords_ = 0;
	GLint player::attribSample_ = 0;

	GLint player::uniformProj_ = 0;
	GLint player::uniformOffset_ = 0;
	GLint player::uniformScale_ = 0;
	GLint player::uniformRot_ = 0;

} // namespace
