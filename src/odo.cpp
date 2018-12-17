#include<iostream>
#include<algorithm>
#include<tuple>
#include<vector>
#include<array>
#include<iterator>
#include<cmath>
#include<fstream>
#include<unordered_map>
#include<utility>
#include<functional>
#include<set>
#include<numeric>
#include<SDL2/SDL.h>
#include<chrono>

#define REVISION "ODO_1.0"
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PI 3.14159265359

using namespace std;

template<typename T> T distance(T a, T b, T c, T d){
  return sqrt(pow(a - c, 2) + pow(b - d, 2));
}

template<typename S, typename C> void add_circle(int x, int y, double r,
									 C&& c, S& s){
	int r_squared = int(pow(r, double(2)));
	int r_Z = int(r);
  for(int i = -r_Z; i < r_Z; ++i){
    for(int j = -r_Z; j < r_Z; ++j){
      if(pow(i, 2) + pow(j, 2) < r_squared){
        if(x + i > 0 && y + j > 0 && y + j < s.size() && x + i < s[0].size()){
          s[y + j][x + i] = c;
        }
      }
    }
  }
}

template<typename S, typename C> void add_line(int x1, int y1, int x2, int y2,
								   C&& c, S& s){
	auto plot = [&s, c](int loc_x, int loc_y){
	  if(int(loc_y) < s.size() && int(loc_x) < s[0].size() && int(loc_y > 0) && int(loc_x) > 0){
		  s[int(loc_y)][int(loc_x)] = c;
		}
	};
	if(x1 > x2){
	  swap(y1, y2); swap(x1, x2);
	}
	if(abs(x1 - x2) == 0) return;
  double dy = (double(y1) - double(y2)) / (double(x1) - double(x2));
  int y = y1;
  double yerr = 0;
  for(int x = x1; x < x2; ++x){
    plot(x, y);
    yerr += abs(dy);
    while(yerr > 1.0){
      y += (y2 > y1) ? 1 : -1;
      plot(x, y);
      yerr -= 1.0;
    }
  }
}

template<typename R> struct Node{
	array<R, 3> orthogonal;
	array<R, 3> original;
	array<R, 2> theta;
	bool exit = false;
	bool on = true;
	bool occupied = false;
	const array<R, 3>& ortho(array<R, 2> in_theta){
	  orthogonal = original;
    theta[0] += in_theta[0]; theta[1] += in_theta[1];
    orthogonal[0] = original[0] * cos(theta[1]) - original[2] * sin(theta[1]);
    orthogonal[2] = original[0] * sin(theta[1]) + original[2] * cos(theta[1]);    
		return orthogonal;
	}
};

struct Color_ptr{
	Uint8 * ptr = nullptr;
	void operator=(const tuple<char, char, char>& color){
		ptr[0] = get<0>(color);
		ptr[1] = get<1>(color);
		ptr[2] = get<2>(color);
	}
	Color_ptr(Uint8 * p) : ptr(p) {}
};

struct Row{
		size_t loc_y;
		SDL_Surface * surface;
		size_t w = 0;

		Color_ptr operator[](size_t loc_x){
					int bpp = surface -> format -> BytesPerPixel;
      	  return Color_ptr((Uint8*)surface -> pixels + loc_y * surface->pitch + loc_x * bpp);
		}

		size_t size(){
			return w;
		}

		Row(size_t y0, SDL_Surface * s, size_t width) : loc_y(y0), surface(s), w(width) {}
};

struct SDL_surface_wrap{
	SDL_Surface * surf = nullptr;
	size_t x, y;

	Row operator[](size_t loc_y){
		return Row(loc_y, surf, x);
	}

	size_t size(){
		return y;
	}

	SDL_surface_wrap(size_t x0, size_t y0, SDL_Surface * surface) {
		x = x0; y = y0; surf = surface;
	}

};

template<typename R> struct Puzzle{
	vector<Node<R>> nodes = {};
	vector<vector<bool>> connections;
	typedef tuple<char, char, char> color;
	typedef vector<vector<color>> screen;
	SDL_surface_wrap canvas;
	int occupied = 0;
	vector<int> p;
	
	vector<int> path(){
	  cout << __PRETTY_FUNCTION__ << endl;
	  auto valid = [this](auto begin, auto end){
	  	if(!nodes[*begin].exit) return false;
	    auto prev = begin++;
	    for(; begin != end; ++begin){
	      if(!connections[*begin][*prev]) return false;
	      prev = begin;
	    }
	    if(*prev != occupied) return false;
	    return true;
	  };
	  vector<vector<int>> known(nodes.size(), vector<int>{});
	  size_t min_length = nodes.size();
		auto result = path(vector<bool>(nodes.size(), false), occupied, known, min_length, 0);
		if(valid(get<1>(result).begin() , get<1>(result).end())){
		  cout << "result validated" << endl;
		}else{
		  cout << "invalid result" << endl;
		}
		if(get<0>(result)) return get<1>(result);
		return vector<int>{};
	}

	tuple<bool, vector<int>> path(const vector<bool>& passed,
	                              const int back, vector<vector<int>>& known,
	                              size_t& min_length,
	                              size_t curr_length){
	  if(curr_length > min_length) return make_tuple(false, vector<int>{});
		if(nodes[back].exit) return make_tuple(true, vector<int>{back});
		if(known[back].size() != 0) return make_tuple(true, known[back]);
		tuple<bool, vector<int>> selected = make_tuple(false, vector<int>{});
		for(size_t i(0); i < connections[back].size(); ++i){
			if(!passed[i] && connections[back][i] && nodes[i].on){
				vector<bool> next_passed = passed; next_passed[i] = true;
				selected = min(path(next_passed, i, known, min_length, curr_length + 1), selected,
						   [](auto&& a, auto&& b){
						      if(!get<0>(a)) return false;
						      if(!get<0>(b)) return true;
						      return (get<1>(a).size() < get<1>(b).size());});
		    if(get<0>(selected)) min_length = min(get<1>(selected).size(), min_length + curr_length);
			}
		}
		get<1>(selected).push_back(back);
		if(get<0>(selected)) known[back] = get<1>(selected);
		return selected;
	}

	void draw_to_canvas(array<R, 2> theta, vector<int> to_eliminate){
	  const unsigned c_x = canvas[0].size() / 2;
	  const unsigned c_y = canvas.size() / 2;
		enum state{NORMAL, OCCUPIED, OFF, EXIT, PATH};
	  array<color, 5> state_color = {color{255, 255, 255},
	                                 color{255, 255, 0},
	                                 color{0, 0, 0},
	                                 color{0, 0, 255},
	                                 color{0, 255, 255}};
		for(size_t i(0); i < canvas.size(); ++i){
			for(size_t j(0); j < canvas[0].size(); ++j){
				canvas[i][j] = make_tuple(0, 0, 0);
			}
		}
		for(int& i : to_eliminate) nodes[i].on = false;
		for(auto& i : nodes){
			i.ortho(theta);
		}
		vector<int> sorted(nodes.size()); iota(sorted.begin(), sorted.end(), 0);
		sort(sorted.begin(), sorted.end(), [this](int i1, int i2){
			return nodes[i1].orthogonal[2] > nodes[i2].orthogonal[2];
		});
		vector<state> states(nodes.size(), NORMAL);
	  for(int& i : p) states[i] = PATH;
		for(size_t i(0); i < nodes.size(); ++i){
			if(!nodes[i].on){
				states[i] = OFF;
			}else{
				if(nodes[i].exit) states[i] = EXIT;
			}
		}
		states[occupied] = OCCUPIED;
		for(size_t i(0); i < nodes.size(); ++i){
			if(nodes[i].on){
				for(size_t j(0); j < nodes.size(); ++j){
					if(connections[i][j] && nodes[j].on){
					  auto color = state_color[NORMAL]; //terrible if statement - I'm not using comparison operators for extensibility
					  if((states[i] == PATH && (states[j] == PATH || states[j] == OCCUPIED || states[j] == EXIT)) || 
					      (states[j] == PATH && (states[i] == PATH || states[i] == OCCUPIED || states[i] == EXIT))){
					    color = state_color[PATH];
					  }
					  if((states[i] == OCCUPIED && states[j] == EXIT) || (states[j] == OCCUPIED && states[i] == EXIT)) color = state_color[PATH];
						add_line(nodes[i].orthogonal[0] + c_x, nodes[i].orthogonal[1] + c_y,
								  nodes[j].orthogonal[0] + c_x, nodes[j].orthogonal[1] + c_y,
								  color, canvas);
					}
				}
			}
		}
		for(size_t i(0); i < nodes.size(); ++i){
			if(nodes[sorted[i]].on){
				  add_circle(nodes[sorted[i]].orthogonal[0] + c_x,
							nodes[sorted[i]].orthogonal[1] + c_y, R(8),
							state_color[states[sorted[i]]], canvas);
			}
		}
	}

	void rotate(array<R, 2> theta){
		for(auto& i : nodes) i.ortho(theta);
	}

	void del_node(int x, int y){
    size_t sel = 0;
    R sel_dist = distance(nodes[sel].orthogonal[0] + R(canvas[0].size() / 2), nodes[sel].orthogonal[1] + R(canvas.size() / 2), R(x), R(y));
    cout << "init_dist :: " << sel_dist << endl;
    for(size_t i(0); i < nodes.size(); ++i){
      if(nodes[i].on){
        R i_dist = distance(nodes[i].orthogonal[0] + R(canvas[0].size() / 2), nodes[i].orthogonal[1] + R(canvas.size() / 2), R(x), R(y));
        if(i_dist < sel_dist){
          sel = i; sel_dist = i_dist;
        }
      }
    }
    cout << "sel_dist :: " << sel_dist << endl;
    cout << sel << endl;
    if(sel == occupied) return;
    if(nodes[sel].exit) return;
    nodes[sel].on = false;
		p = path();
		cout << "called from del_node ... path() :: ";
		for(auto i : p){
		  cout << i << " ";
		}
		cout << endl;
		if(p.size() > 1){
		  nodes[p.back()].occupied = false;
		  p.pop_back();
		  nodes[p.back()].occupied = true;
		  occupied = p.back();
		}
		cout << "del_node result ... path() :: ";
		for(auto i : p){
		  cout << i << " ";
		}
		cout << endl;
		draw_to_canvas({0, 0}, {});
	}

	Puzzle(){}

	template<typename In> Puzzle(In& file, size_t x, size_t y, SDL_Surface * surf) : canvas(x, y, surf) {
		int offset = min(x, y) / 2;
		string rev = ""; file >> rev;
		if(rev != REVISION) throw "IncorrectRevisionHeader";
		size_t node_count = 0; file >> node_count;
		connections = vector<vector<bool>>(node_count, vector<bool>(node_count, false));
		nodes = vector<Node<R>>(node_count);
		size_t index = 0;
		for(auto& i : nodes){
			file >> i.original[0]; i.original[0] -= offset;
			file >> i.original[1]; i.original[1] -= offset;
			file >> i.original[2]; i.original[2] -= offset;
			file >> i.exit;
			file >> i.occupied;
			i.ortho({0, 0});
			if(i.occupied) occupied = index;
			++index;
		}
		for(size_t i(0); i < node_count; ++i){
			for(size_t j(0); j < node_count; ++j){
				bool stupid_temp = false;
				file >> stupid_temp;
				connections[i][j] = stupid_temp;
			}
		}
		p = path();
		cout << "path() :: ";
		for(auto i : p){
			cout << i << " ";
		}
		cout << endl;
	}
};

int main(){
	string fname; cin >> fname;
	fstream file(fname);
	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr;
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		cout << "SDL could not initialize! SDL_Error :: " <<  SDL_GetError() << endl;
	}
	else{
		window = SDL_CreateWindow("odo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if(window == nullptr){
			cout << "Window could not be created! SDL_Error :: " << SDL_GetError() << endl;
		}
		else{
			screenSurface = SDL_GetWindowSurface(window);
			Puzzle<double> puz(file, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface);
			SDL_Event event;
			int mx = SCREEN_WIDTH / 2;
			int my = SCREEN_HEIGHT / 2;
			int mx_ = mx;
			int my_ = my;
			bool left = false;
			size_t left_count(0);
			while(event.type != SDL_QUIT){
			  SDL_PumpEvents();
			  mx_ = mx; my_ = my;
			  if(SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT)){
			    if(left){
			      puz.draw_to_canvas({-double(my_ - my) / 1000, -double(mx - mx_) / 1000}, {});
			      if((my_ - my) != 0 && (mx - mx_) != 0) ++left_count;
			    }else{
			      left = true;
			      ++left_count;
			    }
			  }else{
			    left = false;
			    if(left_count <= 1 && left_count > 0){
			      puz.del_node(mx, my);
			    }
			    left_count = 0;
			  }
			  SDL_UpdateWindowSurface(window);
			  SDL_PollEvent(&event);
			}
		}
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
