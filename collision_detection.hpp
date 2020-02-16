#pragma once

#include <cmath>
#include "terrain.hpp"
#include <tuple>
#include <algorithm>
#include <functional>

namespace james {

	class collision_detection {

	public:

		collision_detection()
		{

		}

		~collision_detection() {

		}

		void register_coin_collect(std::function< void()> cb) { coin_collect_cb_ = cb; };

		void register_death(std::function< void()> cb) { death_cb_ = cb; };
		void register_completion(std::function< void()> cb) { completion_cb_ = cb; };

		void set_terrain(terrain* terr) { terrain_ = terr; }

		std::tuple<double, double> detect_collision(const double xstart, const double ystart, const double xend, const double yend, const double width, const double height) {
			int steps = std::max(100, static_cast<int>(xend - xstart + 3));
			steps = std::max(steps, static_cast<int>(yend - ystart + 3));
			double xcurrent = xstart;
			double ycurrent = ystart;
			double xinc = (xend - xstart) / steps;
			double yinc = (yend - ystart) / steps;

			if (xinc > 0) {
				if (yinc > 0) {
					for (int i = 0; i < steps; i++) {
						xcurrent = collision_right(xcurrent, xcurrent + xinc, ycurrent, width, height);
						ycurrent = collision_up(ycurrent, ycurrent + yinc, xcurrent, width, height);
					}
				} else {
					for (int i = 0; i < steps; i++) {
						xcurrent = collision_right(xcurrent, xcurrent + xinc, ycurrent, width, height);
						ycurrent = collision_down(ycurrent, ycurrent + yinc, xcurrent, width, height);
					}
				}
			} else {
				if (yinc > 0) {
					for (int i = 0; i < steps; i++) {
						xcurrent = collision_left(xcurrent, xcurrent + xinc, ycurrent, width, height);
						ycurrent = collision_up(ycurrent, ycurrent + yinc, xcurrent, width, height);
					}
				} else {
					for (int i = 0; i < steps; i++) {
						xcurrent = collision_left(xcurrent, xcurrent + xinc, ycurrent, width, height);
						ycurrent = collision_down(ycurrent, ycurrent + yinc, xcurrent, width, height);
					}
				}
			}

			//std::tie(xcurrent, ycurrent) = detect_collision_step(xcurrent, ycurrent, xcurrent + xinc, ycurrent +yinc, width, height);
			//xcurrent = xreturn;
			//ycurrent = yreturn;

			return { xcurrent, ycurrent };
		}

		double collision_right(const double xstart, const double xend, const double y, const double width, const double height) {
			bool dead = false;
			for (int i = 0; i < height; i += 60) {
				auto temp = terrain_->get_box(xend + width, y + i);
				if (temp == 13) {
					terrain_->remove_coin(xend + width, y + i);
					coin_collect_cb_();
					continue;
				}
				if (temp == 111 || temp == 40) {	continue; }
				if (temp == 57) { completion_cb_(); continue; }
				if (temp == 37) { dead = true; continue; }
				if (temp) {	return std::floor(xend); }
			}
			auto temp = terrain_->get_box(xend + width, y + height - 0.000000001);
			if (temp == 13) {
				terrain_->remove_coin(xend + width, y + height);
				coin_collect_cb_();
				return xend;
			}
			else if (temp == 111 || temp == 52) {	return xend; }
			else if (temp) {	return std::floor(xend); }
			else if (temp == 57) { completion_cb_(); }
			else if (temp == 37) { dead = true; }
			else if (dead || temp == 37) { death_cb_(); }
			if (dead) { death_cb_(); }
			return xend;
		}

		double collision_up(const double ystart, const double yend, const double x, const double width, const double height) {
			bool dead = false;
			for (int i = 0; i < width; i += 60) {
				auto temp = terrain_->get_box(x + i, yend + height);
				if (temp == 13) {
					terrain_->remove_coin(x + i, yend + height);
					coin_collect_cb_();
					continue;
				}
				if (temp == 111 || temp == 52 || temp == 40) { continue; }
				if (temp == 57) { completion_cb_(); continue; }
				if (temp == 37) { dead = true; continue; }
				if (temp) {	return std::floor(yend); }
			}
			auto temp = terrain_->get_box(x + width - 0.000000001, yend + height);
			if (temp == 13) {
				terrain_->remove_coin(x + width, yend + height);
				coin_collect_cb_();
				return yend;
			}
			else if (temp == 111 || temp == 52) {	return yend; }
			else if (temp) {	return std::floor(yend); }
			else if (temp == 57) { completion_cb_(); }
			else if (temp == 37) { dead = true; }
			else if (dead || temp == 37) { death_cb_(); }
			if (dead) { death_cb_(); }
			return yend;
		}

		double collision_left(const double xstart, const double xend, const double y, const double width, const double height) {
			bool dead = false;
			for (int i = 0; i < height; i += 60) {
				auto temp = terrain_->get_box(xend, y + i);
				if (temp == 13) {
					terrain_->remove_coin(xend, y + i);
					coin_collect_cb_();
					continue;
				}
				if (temp == 111 || temp == 52) {	continue; }
				if (temp == 57) { completion_cb_(); continue; }
				if (temp == 37) { dead = true; continue; }
				if (temp) {	return std::ceil(xend);	}
			}
			auto temp = terrain_->get_box(xend, y + height - 0.000000001);
			if (temp == 13) {
				terrain_->remove_coin(xend, y + height);
				coin_collect_cb_();
			}
			else if (temp == 111 || temp == 52) {	return xend; }
			else if (temp) {	return std::ceil(xend); }
			else if (temp == 57) { completion_cb_(); }
			else if (temp == 37) { dead = true; }
			else if (dead || temp == 37) { death_cb_(); }
			if (dead) { death_cb_(); }
			return xend;
		}

		double collision_down(const double ystart, const double yend, const double x, const double width, const double heigth) {
			bool dead = false;
			for (int i = 0; i < width; i += 60) {
				auto temp = terrain_->get_box(x + i, yend);
				if (temp == 13) {
					terrain_->remove_coin(x + i, yend);
					coin_collect_cb_();
					continue;
				}
				if (temp == 52 || temp == 40) { continue;	}
				if (temp == 57) { completion_cb_(); continue; }
				if (temp == 37) { dead = true; continue; }
				if (temp) {	return std::ceil(yend);	}
			}
			auto temp = terrain_->get_box(x + width - 0.000000001, yend);
			if (temp == 13) {
				terrain_->remove_coin(x + width, yend);
				coin_collect_cb_();
			}
			else if (temp == 52 || temp == 40) { return yend;	}
			else if (temp == 57) { completion_cb_(); }
			else if (temp == 37) { dead = true; }
			else if (temp) {	return std::ceil(yend);	}
			if (dead) { death_cb_(); }
			return yend;
		}
		
		/*
		std::tuple<double, double> detect_collision_step(const double xstart, const double ystart, const double xend, const double yend, const double width, const double height) {
			
			//std::cout << "coll" << std::endl;

			double xnew = xend;
			double ynew = yend;

			//////bodged fix with -1s in up and right directions *****************************************
			
			bool a = std::floor((xend + width) / 60) - std::floor((xstart + width) / 60) > 0;
			bool b = (((xend + width) / 60 - std::floor((xend + width) / 60) == 0) && xend - xstart > 0);

			if (a != b) { //x block shift right detected
				for (int i = 0; i < height; i += 50) {
					if (terrain_->get_box(xend + width, yend + i) != 0) {
						std::cout << "hit" << "    " << xnew << std::endl;
						xnew = 60 * std::floor((xend + width) / 60) - width;
						std::cout << "hit" << "    " << xnew << std::endl;
					}
				}
				if (terrain_->get_box(xend + width, yend + height) != 0) {
					xnew = 60 * std::floor((xend + width) / 60) - width;
					//std::cout << "hit" << "    " << xnew << std::endl;
				}
				std::cout << "collide" << std::endl;
			} else if (std::floor(xend/60) - std::floor(xstart/60) < 0 || (((xend) / 60 - std::floor((xend) / 60) == 0) && xend - xstart < 0)) { //x block shift left detected
				for (int i = 0; i < height; i += 50) {
					if (terrain_->get_box(xend, yend + i) != 0) {
						std::cout << "hit" << "    " << xnew << std::endl;

						xnew = 60 * (std::floor(xend / 60) + 1);
						std::cout << "hit" << "    " << xnew << std::endl;

					}
				}
				if (terrain_->get_box(xend, yend + height) != 0) {
					xnew = 60 * (std::floor(xend / 60) + 1);
				}
			}
			a = std::floor((yend + height) / 60) - std::floor((ystart + height) / 60) > 0;
			b = (((yend + height) / 60 - std::floor((yend + height) / 60) == 0) && yend - ystart > 0);

			if (a != b) { //x block shift right detected
				for (int i = 0; i < width; i += 50) {
					if (terrain_->get_box(xnew + i, yend + height) != 0) {
						ynew = 60 * std::floor((yend + height) / 60) - height;
					}
				}
				if (terrain_->get_box(xnew + width, yend + height) != 0) {
					ynew = 60 * std::floor((yend + height) / 60) - height;
				}
			} else if (std::floor(yend/60) - std::floor(ystart/60) < 0 || (((yend) / 60 - std::floor((yend) / 60) == 0) && yend - ystart < 0)) { //y block shift down detected
				for (int i = 0; i < width; i += 50) {
					if (terrain_->get_box(xnew + i, yend) != 0) {
						ynew = 60 * (std::floor(yend / 60) + 1);
					}
				}
				if (terrain_->get_box(xnew + width, yend) != 0) {
					ynew = 60 * (std::floor(yend / 60) + 1);
				}
			}
			//std::cout << xnew << "     " << ynew << std::endl;
			return { xnew, ynew };
			



		}*/

	private:
		terrain *terrain_;

      std::function< void ()> coin_collect_cb_;
		std::function< void()> death_cb_;
		std::function< void()> completion_cb_;
	};

}
