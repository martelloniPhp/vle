

#ifndef VLE_UTILS_STATE_HPP
#define VLE_UTILS_STATE_HPP

#include <iostream>
#include <vector>
//#include <pair>
#include <utility>

namespace vle { namespace utils {






struct Position
{
	int i;
	int j;
};


class State 
{

	 
	 
	 public:
	static std::vector<State *> states;
	static int n;
	//State () {}
	//~State () = default;
	 //std::vector<std::pair<int, int>> voisinage  = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};  
	 //std::vector<std::pair<int, int>> voisinage = {{-3,0},{-2,-2},{-1,-1},{-1,0},{-1,1},{-2,2},{0,-3},{0,-1},{0,1},{0,3},{2,-2},{1,-1},{1,0},{1,1},{2,2},{3,0}};
	 std::vector<std::pair<int, int>> voisinage = {{-2,0},{-1,-1},{-1,0},{-1,1},{0,-2},{0,-1},{0,1},{0,2},{1,-1},{1,0},{1,1},{2,0}};
	//std::vector<std::pair<int, int>> voisinage;
	 State (bool act=true) {lastMasse=0;elapsed=0;activity = act;}
	 State (Position pos,bool act=true) { p = pos;states.emplace_back(this); n++;lastMasse=0;activity = act;}
	 State (Position pos, std::string v,bool act=true) { p = pos; val = v;states.emplace_back(this);n++;lastMasse=0;activity = act;}
	 State (int i, int j, std::string v,bool act=true) { p.i=i; p.j=j;  val=v; states.emplace_back(this);n++;lastMasse=0;activity = act;}
	 State (int i, int j, std::string v,float t, float m,bool act=true) { p.i=i; p.j=j;  val=v; states.emplace_back(this);n++; temperature=t; masse=m; lastMasse=m;activity = act;}
	 ~State () = default;
	 
	 void setState(int i, int j, std::string v) 
		 {
			 p.i=i; p.j=j ;val=v;
		 } 
	 void setState(int i, int j, std::string v, float t, float m) 
		 {
			 p.i=i; p.j=j ;val=v; temperature=t; masse=m; lastMasse=m; 
		 }
		  void setState(int i, int j, std::string v, float t, float m, int e) 
		 {
			 p.i=i; p.j=j ;val=v; temperature=t; masse=m; lastMasse=m; elapsed = e;
		 }
	  void setTemperature(float t)
		 {
			 temperature = t;
		 }
	  float getTemperature ()
	  {
		  return temperature;
	  }
	  
	  void setMasse(float m)
	  {
		  lastMasse = masse;
		  masse = m;
	  }
	  
	  float getMasse()
	  {
		  return masse;
	  }
	   float getlastMasse()
	  {
		  return lastMasse;
	  }
	  
	  bool getActivity()
	  {
		  return activity;
		}
		
		void setActivity(bool act)
		{
			activity = act;
		}
	 
	Position getPosition() {return p;} 
	void setPosition(Position pos) {p = pos;} 
	std::string getVal() {return val;}
	void setVal(std::string v) {val=v;}
	void setElapsed(int e) {elapsed = e;}
	int getElapsed(){ return elapsed;}
	static int nb();
	int nbs() {return n;}
	 
	 protected:
	 
	 Position p;
	 std::string val; 
	 float temperature;
	 float masse;
	 float lastMasse;
	 int elapsed;
	 bool activity;
	 
	 
	 
	
	
};

}} // namespace vle utils

#endif
