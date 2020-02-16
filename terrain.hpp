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

#include "coins.hpp"

extern float projectionMatrix_[16];

namespace james {

	class terrain {

	public:

		terrain() {

			if (!shaderCreated_) {
				create_shader();
			}

			boxmap_.resize(400);
			for (int i = 0; i < boxmap_.size(); i++) {
				boxmap_[i].resize(75);
				boxmap_[i][0] = 2;
				for (int j = 1; j < boxmap_[i].size(); j++) {
					boxmap_[i][j] = 0;
				}
			}

			for (int i = 1; i < 15; i++) { boxmap_[0][i] = 132; }

			boxmap_[5][1] = 13;

			boxmap_[10][1] = 3;
			boxmap_[10][2] = 3;
			boxmap_[11][4] = 3;
			boxmap_[13][6] = 3;
			boxmap_[18][9] = 3;
			boxmap_[21][13] = 3;
			boxmap_[24][14] = 13;
			boxmap_[25][14] = 13;
			boxmap_[26][14] = 13;

			for (int i = 20; i < 30; i++) {
				boxmap_[i][4] = 13;
			}

			boxmap_[0][0] = 14;
			boxmap_[34][4] = 3;
			boxmap_[35][4] = 111;
			boxmap_[36][4] = 111;
			boxmap_[37][4] = 111;
			boxmap_[38][4] = 3;

			boxmap_[35][9] = 3;
			boxmap_[36][8] = 3;
			boxmap_[37][8] = 111;
			boxmap_[38][8] = 111;
			boxmap_[39][8] = 111;
			boxmap_[40][8] = 111;
			boxmap_[41][8] = 111;
			boxmap_[42][8] = 111;

			for (int i = 1; i < 10; i++) { boxmap_[43][i] = 3; }

			/*boxmap_[51][1] = 37;
			boxmap_[52][1] = 37;
			boxmap_[58][1] = 37;
			boxmap_[59][1] = 37;
			boxmap_[60][1] = 37;
			boxmap_[61][1] = 37;*/

			boxmap_[52][1] = 37;
			boxmap_[59][1] = 37;
			boxmap_[60][1] = 37;

			for (int i = 3; i < 22; i++) {
				boxmap_[73][i] = 3;
				boxmap_[77][i] = 3;
			}
			boxmap_[73][1] = 40;
			boxmap_[77][1] = 52;
			for (int i = 1; i < 6; i++) {
				boxmap_[74][i * 4] = 111;
				boxmap_[75][i * 4] = 111;
				boxmap_[76][i * 4] = 111;
				boxmap_[74][i * 4 + 2] = 13;
				boxmap_[75][i * 4 + 2] = 13;
				boxmap_[76][i * 4 + 2] = 13;
			}

			boxmap_[84][1] = 128;
			boxmap_[85][1] = 103;
			boxmap_[86][1] = 103;
			boxmap_[87][1] = 103;
			boxmap_[88][1] = 103;
			boxmap_[89][1] = 103;
			boxmap_[90][1] = 103;
			boxmap_[91][1] = 104;
			boxmap_[85][2] = 128;
			boxmap_[86][2] = 103;
			boxmap_[87][2] = 103;
			boxmap_[88][2] = 103;
			boxmap_[89][2] = 103;
			boxmap_[90][2] = 104;
			boxmap_[86][3] = 128;
			boxmap_[87][3] = 103;
			boxmap_[88][3] = 103;
			boxmap_[89][3] = 104;
			boxmap_[87][4] = 128;
			boxmap_[88][4] = 104;

			boxmap_[81][11] = 13;
			boxmap_[94][11] = 13;
			boxmap_[82][10] = 13;
			boxmap_[83][10] = 13;
			boxmap_[84][10] = 13;
			boxmap_[85][10] = 13;
			boxmap_[86][10] = 13;
			boxmap_[87][10] = 13;
			boxmap_[88][10] = 13;
			boxmap_[89][10] = 13;
			boxmap_[90][10] = 13;
			boxmap_[91][10] = 13;
			boxmap_[92][10] = 13;
			boxmap_[93][10] = 13;
			boxmap_[83][9] = 13;
			boxmap_[84][9] = 13;
			boxmap_[85][9] = 13;
			boxmap_[86][9] = 13;
			boxmap_[87][9] = 13;
			boxmap_[88][9] = 13;
			boxmap_[89][9] = 13;
			boxmap_[90][9] = 13;
			boxmap_[91][9] = 13;
			boxmap_[92][9] = 13;
			boxmap_[84][8] = 13;
			boxmap_[85][8] = 13;
			boxmap_[86][8] = 13;
			boxmap_[87][8] = 13;
			boxmap_[88][8] = 13;
			boxmap_[89][8] = 13;
			boxmap_[90][8] = 13;
			boxmap_[91][8] = 13;
			boxmap_[85][7] = 13;
			boxmap_[86][7] = 13;
			boxmap_[87][7] = 13;
			boxmap_[88][7] = 13;
			boxmap_[89][7] = 13;
			boxmap_[90][7] = 13;
			boxmap_[86][6] = 13;
			boxmap_[87][6] = 13;
			boxmap_[88][6] = 13;
			boxmap_[89][6] = 13;
			boxmap_[87][5] = 13;
			boxmap_[88][5] = 13;

			boxmap_[93][5] = 37;
			boxmap_[93][4] = 3;

			for (int i = 100; i < 105; i++) { boxmap_[i][1] = 37; boxmap_[i][7] = 13; }
			for (int i = 105; i < 120; i++) { boxmap_[i][1] = 37; boxmap_[i][10] = 13; }
			for (int i = 120; i < 125; i++) { boxmap_[i][1] = 37; boxmap_[i][7] = 13; }

			boxmap_[99][2] = 110;
			boxmap_[102][4] = 110;
			boxmap_[105][6] = 110;
			boxmap_[107][3] = 110;
			boxmap_[110][5] = 110;
			boxmap_[111][9] = 110;
			boxmap_[116][6] = 110;
			boxmap_[120][3] = 110;
			boxmap_[123][3] = 110;

			for (int i = 130; i < 142; i++) {
				for (int j = 1; j < 7; j++) {
					if ((i + j) % 2 == 0)
						boxmap_[i][j] = 13;
				}
			}

			boxmap_[148][1] = 57;
			boxmap_[148][2] = 45;
			boxmap_[147][3] = 3;
			boxmap_[148][3] = 3;
			boxmap_[149][3] = 3;
			boxmap_[146][4] = 3;
			boxmap_[147][4] = 13;
			boxmap_[148][4] = 13;
			boxmap_[149][4] = 13;
			boxmap_[150][4] = 3;

			for (int i = 3; i < 15; i+=2) { boxmap_[155][i] = 3; }

			boxmap_[159][1] = 37; boxmap_[159][2] = 13;
			boxmap_[160][1] = 37; boxmap_[160][2] = 13;
			boxmap_[163][1] = 37; boxmap_[163][2] = 13;
			boxmap_[164][1] = 37; boxmap_[164][3] = 13;
			boxmap_[165][1] = 37; boxmap_[165][3] = 13;
			boxmap_[166][1] = 37; boxmap_[166][2] = 13;
			boxmap_[169][1] = 37; boxmap_[169][2] = 13;
			boxmap_[170][1] = 37; boxmap_[170][3] = 13;
			boxmap_[171][1] = 37; boxmap_[171][3] = 13;
			boxmap_[172][1] = 37; boxmap_[172][2] = 13;

			for (int i = 2; i < 7; i++) { for (int j = 174; j < 179; j++) { boxmap_[j][i] = 13; } }

			for (int i = 1; i < 15; i++) { boxmap_[180][i] = 132; }
			boxmap_[180][15] = 37;

		};

		void reinitialize() {
			boxmap_.resize(400);
			for (int i = 0; i < boxmap_.size(); i++) {
				boxmap_[i].resize(75);
				boxmap_[i][0] = 2;
				for (int j = 1; j < boxmap_[i].size(); j++) {
					boxmap_[i][j] = 0;
				}
			}

			for (int i = 1; i < 15; i++) { boxmap_[0][i] = 132; }

			boxmap_[5][1] = 13;
			/*
			boxmap_[6][1] = 111;
			boxmap_[5][2] = 111;
			boxmap_[6][3] = 111;
			boxmap_[5][4] = 111;
			boxmap_[6][5] = 111;
			boxmap_[5][6] = 111;
			*/

			boxmap_[10][1] = 3;
			boxmap_[10][2] = 3;
			boxmap_[11][4] = 3;
			boxmap_[13][6] = 3;
			boxmap_[18][9] = 3;
			boxmap_[21][13] = 3;
			boxmap_[24][14] = 13;
			boxmap_[25][14] = 13;
			boxmap_[26][14] = 13;

			for (int i = 20; i < 30; i++) {
				boxmap_[i][4] = 13;
			}

			boxmap_[0][0] = 14;
			boxmap_[34][4] = 3;
			boxmap_[35][4] = 111;
			boxmap_[36][4] = 111;
			boxmap_[37][4] = 111;
			boxmap_[38][4] = 3;

			boxmap_[35][9] = 3;
			boxmap_[36][8] = 3;
			boxmap_[37][8] = 111;
			boxmap_[38][8] = 111;
			boxmap_[39][8] = 111;
			boxmap_[40][8] = 111;
			boxmap_[41][8] = 111;
			boxmap_[42][8] = 111;

			for (int i = 1; i < 10; i++) { boxmap_[43][i] = 3; }

			/*boxmap_[51][1] = 37;
			boxmap_[52][1] = 37;
			boxmap_[58][1] = 37;
			boxmap_[59][1] = 37;
			boxmap_[60][1] = 37;
			boxmap_[61][1] = 37;*/

			boxmap_[52][1] = 37;
			boxmap_[59][1] = 37;
			boxmap_[60][1] = 37;

			for (int i = 3; i < 22; i++) {
				boxmap_[73][i] = 3;
				boxmap_[77][i] = 3;
			}
			boxmap_[73][1] = 40;
			boxmap_[77][1] = 52;
			for (int i = 1; i < 6; i++) {
				boxmap_[74][i * 4] = 111;
				boxmap_[75][i * 4] = 111;
				boxmap_[76][i * 4] = 111;
				boxmap_[74][i * 4 + 2] = 13;
				boxmap_[75][i * 4 + 2] = 13;
				boxmap_[76][i * 4 + 2] = 13;
			}

			boxmap_[84][1] = 128;
			boxmap_[85][1] = 103;
			boxmap_[86][1] = 103;
			boxmap_[87][1] = 103;
			boxmap_[88][1] = 103;
			boxmap_[89][1] = 103;
			boxmap_[90][1] = 103;
			boxmap_[91][1] = 104;
			boxmap_[85][2] = 128;
			boxmap_[86][2] = 103;
			boxmap_[87][2] = 103;
			boxmap_[88][2] = 103;
			boxmap_[89][2] = 103;
			boxmap_[90][2] = 104;
			boxmap_[86][3] = 128;
			boxmap_[87][3] = 103;
			boxmap_[88][3] = 103;
			boxmap_[89][3] = 104;
			boxmap_[87][4] = 128;
			boxmap_[88][4] = 104;

			boxmap_[81][11] = 13;
			boxmap_[94][11] = 13;
			boxmap_[82][10] = 13;
			boxmap_[83][10] = 13;
			boxmap_[84][10] = 13;
			boxmap_[85][10] = 13;
			boxmap_[86][10] = 13;
			boxmap_[87][10] = 13;
			boxmap_[88][10] = 13;
			boxmap_[89][10] = 13;
			boxmap_[90][10] = 13;
			boxmap_[91][10] = 13;
			boxmap_[92][10] = 13;
			boxmap_[93][10] = 13;
			boxmap_[83][9] = 13;
			boxmap_[84][9] = 13;
			boxmap_[85][9] = 13;
			boxmap_[86][9] = 13;
			boxmap_[87][9] = 13;
			boxmap_[88][9] = 13;
			boxmap_[89][9] = 13;
			boxmap_[90][9] = 13;
			boxmap_[91][9] = 13;
			boxmap_[92][9] = 13;
			boxmap_[84][8] = 13;
			boxmap_[85][8] = 13;
			boxmap_[86][8] = 13;
			boxmap_[87][8] = 13;
			boxmap_[88][8] = 13;
			boxmap_[89][8] = 13;
			boxmap_[90][8] = 13;
			boxmap_[91][8] = 13;
			boxmap_[85][7] = 13;
			boxmap_[86][7] = 13;
			boxmap_[87][7] = 13;
			boxmap_[88][7] = 13;
			boxmap_[89][7] = 13;
			boxmap_[90][7] = 13;
			boxmap_[86][6] = 13;
			boxmap_[87][6] = 13;
			boxmap_[88][6] = 13;
			boxmap_[89][6] = 13;
			boxmap_[87][5] = 13;
			boxmap_[88][5] = 13;

			boxmap_[93][5] = 37;
			boxmap_[93][4] = 3;

			for (int i = 100; i < 105; i++) { boxmap_[i][1] = 37; boxmap_[i][7] = 13; }
			for (int i = 105; i < 120; i++) { boxmap_[i][1] = 37; boxmap_[i][10] = 13; }
			for (int i = 120; i < 125; i++) { boxmap_[i][1] = 37; boxmap_[i][7] = 13; }

			boxmap_[99][2] = 110;
			boxmap_[102][4] = 110;
			boxmap_[105][6] = 110;
			boxmap_[107][3] = 110;
			boxmap_[110][5] = 110;
			boxmap_[111][9] = 110;
			boxmap_[116][6] = 110;
			boxmap_[120][3] = 110;
			boxmap_[123][3] = 110;

			for (int i = 130; i < 142; i++) {
				for (int j = 1; j < 7; j++) {
					if ((i + j) % 2 == 0)
						boxmap_[i][j] = 13;
				}
			}

			boxmap_[148][1] = 57;
			boxmap_[148][2] = 45;
			boxmap_[147][3] = 3;
			boxmap_[148][3] = 3;
			boxmap_[149][3] = 3;
			boxmap_[146][4] = 3;
			boxmap_[147][4] = 13;
			boxmap_[148][4] = 13;
			boxmap_[149][4] = 13;
			boxmap_[150][4] = 3;

			for (int i = 3; i < 15; i += 2) { boxmap_[155][i] = 3; }

			boxmap_[159][1] = 37; boxmap_[159][2] = 13;
			boxmap_[160][1] = 37; boxmap_[160][2] = 13;
			boxmap_[163][1] = 37; boxmap_[163][2] = 13;
			boxmap_[164][1] = 37; boxmap_[164][3] = 13;
			boxmap_[165][1] = 37; boxmap_[165][3] = 13;
			boxmap_[166][1] = 37; boxmap_[166][2] = 13;
			boxmap_[169][1] = 37; boxmap_[169][2] = 13;
			boxmap_[170][1] = 37; boxmap_[170][3] = 13;
			boxmap_[171][1] = 37; boxmap_[171][3] = 13;
			boxmap_[172][1] = 37; boxmap_[172][2] = 13;

			for (int i = 2; i < 7; i++) { for (int j = 174; j < 179; j++) { boxmap_[j][i] = 13; } }

			for (int i = 1; i < 15; i++) { boxmap_[180][i] = 132; }
			boxmap_[180][15] = 37;


			coins_.reinitialize();

		}

		~terrain() {
			if (textureId_) {
				glDeleteTextures(1, &textureId_);
				textureId_ = 0;
			}

			if (vertexId_) {
				glDeleteBuffers(1, &vertexId_);
				vertexId_ = 0;
			}
		};

		auto get_box(double x, double y) {
			return(boxmap_[std::floor(x / 60)][std::floor(y / 60)]);
		}

		void remove_coin(double x, double y) {
			coins_.remove_coin(std::floor(x / 60), std::floor(y / 60));
			boxmap_[std::floor(x / 60)][std::floor(y / 60)] = 0;
		}

		void load(const std::string url) {
			url_ = url;

			request_handle_ = emscripten_async_wget2_data(url_.c_str(), "GET", "", this, EM_FALSE,

				[](unsigned int handle, void * user_data, void * data, unsigned int data_size) {
				auto handler = reinterpret_cast<terrain *>(user_data);
				handler->onload(handle, data, data_size);
			},

				[](unsigned int handle, void * user_data, int error_code, const char * error_msg) {
				auto handler = reinterpret_cast<terrain *>(user_data);
				handler->onerror(handle, error_code, error_msg);
			},

				[](unsigned int handle, void * user_data, int number_of_bytes, int total_bytes) {
				auto handler = reinterpret_cast<terrain *>(user_data);
				handler->onprogress(handle, number_of_bytes, total_bytes);
			}
			);

		};

		void addbox(float x1, float y1, float x2, float y2, float X1, float Y1, float X2, float Y2) {

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

			coins_.set_texture(textureId_);

		};


		void display(float xPos, float yPos) {

			
			coins_.display(xPos, yPos);

			glUseProgram(program_);

			glUniformMatrix4fv(uniformProj_, 1, GL_FALSE, projectionMatrix_);

			glUniform4f(uniformOffset_, xPos, yPos, 0.0, 0.0);
			glUniform2f(uniformScale_, 1.0, 1.0);
			glUniform4f(uniformRot_, 0.0, 0.0, 0.0, 0.0);

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

		void onload(unsigned int handle, void * data, unsigned int data_size) {

			printf("got data size: %u\n", data_size);

			emscripten_run_preload_plugins_data((char *)data, data_size, "png", this,

				[](void * user_data, const char * file) {
				auto handler = reinterpret_cast<terrain *>(user_data);
				handler->ondecoded(file);
			},

				[](void * user_data) {
				auto handler = reinterpret_cast<terrain *>(user_data);
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

			for (int i = 0; i < boxmap_.size(); i++) {
				for (int j = 0; j < boxmap_[i].size(); j++) {
					if (boxmap_[i][j] != 0) {
						if (boxmap_[i][j] != 13) {
							addbox(i * 60, j * 60, 60, 60, (boxmap_[i][j] % 12) * 1.0 / 12.7, (boxmap_[i][j] - (boxmap_[i][j] % 12)) / 12 * 1.0 / 13.0, 1.0 / 12.8, 1.0 / 13.0);
						} else {
							coins_.addbox(i * 60, j * 60, 60, 60, (boxmap_[i][j] % 12) * 1.0 / 12.7, (boxmap_[i][j] - (boxmap_[i][j] % 12)) / 12 * 1.0 / 13.0, 1.0 / 12.8, 1.0 / 13.0);
						}
					}	
				}
			}

			coins_.bind();

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
					mat4 a_scale = mat4(scale.x, 0.0, 0.0, 0.0, 0.0, scale.y, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0); \
					gl_Position = proj * ((a_scale * position) + offset); \
					v_texCoord = vec4(a_texCoord.x, a_texCoord.y, 0.0, 1.0); \
				} \
				";
			//					mat4 rotation = mat4(cos(rot.z), sin(rot.z), 0, 0,  -sin(rot.z), cos(rot.z), 0, 0,  0, 0, 1, 0,  0, 0, 0, 1); \
			//					vec4 t = rotation * position; \


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
		std::vector<std::vector<std::uint8_t>> boxmap_;

		coins coins_;

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

	}; // class terrain

	bool terrain::shaderCreated_ = false;

	GLuint terrain::program_ = 0;

	GLuint terrain::vertexShader_ = 0;
	GLuint terrain::fragmentShader_ = 0;

	GLint terrain::attribPosition_ = 0;
	GLint terrain::attribTexcoords_ = 0;
	GLint terrain::attribSample_ = 0;

	GLint terrain::uniformProj_ = 0;
	GLint terrain::uniformOffset_ = 0;
	GLint terrain::uniformScale_ = 0;
	GLint terrain::uniformRot_ = 0;

} // namespace
