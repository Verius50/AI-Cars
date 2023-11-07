#include <iostream>
#include <SFML/Graphics.hpp>
#pragma once

using namespace sf;
class RaceTrack
{
public:
	Vector2f startPos = { 100, 300 };
	//int startAngle = 0;
	std::string name = "RaceTrack1";
	VertexArray lines;
	RaceTrack() { lines.setPrimitiveType(Lines); }
	void save();
	int read(int number);
	void clear() {
		startPos = { 100, 300 };
		lines.clear();
		name = "RaceTrack1";
	}
	void deleteRace(int number);
};

