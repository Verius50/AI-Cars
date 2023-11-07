#include <iostream>
#include <vector>
#include <fstream>
#include "SFML/Graphics.hpp"
#include "AI-Car.h"
#include "RaceTrack.h"


#define viewSizeX 1280
#define viewSizeY 720


using namespace sf;

RaceTrack tempRace;
VertexArray carlines(Lines);	//lines in 5 directions from cars, to find car position
Texture SelectRaceBackTX, carTX, dotTX;		//textures from files folder
Sprite SelectRaceBack, * dot;	//dots - carlines collision points
float dots[10]; size_t nm = 200;	//nm - number of cars
bool pressed = 0, anypressed = 0, rd = 0, mrk = 0;	//rd - read weights from file, mrk - show car markers
Font font;	//arial.tff
Text description("With Artificial intelligence!", font);

class Car
{
private:
	float speed = 0.0f;
public:
	float score = 0;
	bool alive = 1;
	Sprite car;
	void controlSpeed(int move)	
	{
		if (alive)
		{
			if (move == 1){		//move forvard
				speed += 0.1f;
				//score += 0.5f;
				score += speed;
			}
			if (move == 2) {	//move back
				speed -= 0.1f;
				//score -= 0.5f;
				score += speed * 1.5;
			}
			if (speed < 0.03f && speed > -0.03f)
				speed = 0;
			if (speed > 5)
				speed = 5;
			if (speed < -5)
				speed = -5;
			if (speed != 0)
			{
				car.move(sin((car.getRotation() * 3.141592) / 180) * speed, -cos((car.getRotation() * 3.141592) / 180) * speed);	//move in direction
				if (speed > 0)
					speed -= 0.03f;
				else
					speed += 0.03f;
				if (move == 3) {
					car.rotate(4);
					//speed -= 0.1f;
					//score += 0.5f;
					score += speed * 1.5;
				}
				if (move == 4){
					car.rotate(-4); 
					//speed -= 0.1f;
					//score += 0.5f;
					score += speed * 1.5;
				}
				//score += speed / 10.0;
			}
			else if (move == 0 || move > 2 ) alive = 0;	 //it never moves again
			if (score < 0) alive = 0;
		}
	}
};
	Car* cars;
	AICar* AI;
	void paintrace();


	bool startMenu()
	{
		RenderWindow menuwin(VideoMode(800, 600), "AI-Race Menu");
		bool selectingRace = 0; int racesNum = 0;
		Text header("RACE", font, 80), nmtext("", font, 30), rdtext("", font, 30), mrktext("", font, 30);
		std::vector<Text> list{
			Text("        Start", font, 50),
				Text("Make a new race", font, 50),
				Text("    Contact us", font, 50) };
		description.setString("With Artificial intelligence!");
		std::vector<FloatRect> textsboxes;	textsboxes.resize(6);
		Texture TrashTX; TrashTX.loadFromFile("Trash.png");
		std::vector<Sprite> trashes;
		SelectRaceBack.setPosition(200, 75);

		header.setStyle(Text::Bold);	header.setPosition(290, 30);	header.setFillColor(Color::Black);
		header.setOutlineThickness(3);	header.setOutlineColor(Color::Red);

		description.setPosition(270, 130);	description.setFillColor(Color(128, 128, 128));

		nmtext.setPosition(560, 15);	nmtext.setStyle(Text::Bold);	nmtext.setFillColor(Color::Black);

		rdtext.setPosition(20, 20);		rdtext.setStyle(Text::Bold);	rdtext.setFillColor(Color::Red);	rdtext.setOutlineThickness(1);

		mrktext.setPosition(20, 120);	mrktext.setStyle(Text::Bold);	mrktext.setFillColor(Color::Red);	mrktext.setOutlineThickness(1);

		for (size_t i = 0; i < 3; i++)
		{
			list[i].setStyle(Text::Bold);
			list[i].setPosition(225, 180 + i * 80);
			list[i].setFillColor(Color(0, 255, 0));
			list[i].setOutlineThickness(2);
			list[i].setOutlineColor(Color::Black);
			textsboxes[i] = list[i].getGlobalBounds();
		}

		while (menuwin.isOpen())
		{
			Event event;
			while (menuwin.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					menuwin.close();
					return 0;
				}
			}
			menuwin.clear(Color(195, 195, 195));
			menuwin.draw(header);
			menuwin.draw(nmtext);
			menuwin.draw(rdtext);
			menuwin.draw(mrktext);
			menuwin.draw(description);


			nmtext.setString("Cars in \npopulation: " + std::to_string(nm));

			for (size_t i = 0; i < 3; i++)
			{
				if (!selectingRace)
					menuwin.draw(list[i]);
				else { 
					menuwin.draw(SelectRaceBack); 
					for (int j = 3; j < list.size(); j++)
					{
						textsboxes[j + 3] = list[j].getGlobalBounds();
						if (textsboxes[j + 3].contains(Vector2f(Mouse::getPosition(menuwin)))) {
							list[j].setOutlineThickness(1);
							list[j].setOutlineColor(Color::Green);
							if (Mouse::isButtonPressed(Mouse::Left) && !pressed)
							{
								tempRace.read(j - 2);
								event.type = Event::Closed;
								menuwin.close();
								return 1;
							}
						}
						else list[j].setOutlineThickness(0);
						trashes[j - 3].setPosition(list[j].getPosition() + Vector2f{320, 15});
						if (trashes[j - 3].getGlobalBounds().contains(Vector2f(Mouse::getPosition(menuwin))))
						{
							trashes[j - 3].setColor(Color::Red);
							if (Mouse::isButtonPressed(Mouse::Left) && !pressed)
							{
								tempRace.deleteRace(j - 2);
								trashes.erase(trashes.begin());
								list.erase(list.begin() + j);
								j--;
							}
						}
						else
							trashes[j - 3].setColor(Color::White);

						menuwin.draw(trashes[j - 3]);
						menuwin.draw(list[j]);
					}
				}
				if (textsboxes[i].contains(Vector2f(Mouse::getPosition(menuwin))))
				{
					list[i].setOutlineColor(Color::Yellow);

					if (Mouse::isButtonPressed(Mouse::Left) && !pressed)
					{
						switch (i)
						{
						case 0:
							selectingRace = 1;
							racesNum = tempRace.read(50);
							for (int j = 0; j < racesNum; j++)
							{
								tempRace.read(j + 1);
								if (list.size() < 4 + j) {
									list.push_back(Text(std::to_string(j + 1) + ". " + tempRace.name, font, 50));
									textsboxes.push_back(list[list.size() - 1].getGlobalBounds());
									trashes.push_back(Sprite(TrashTX));
								}
								{
									list[3 + j] = Text(std::to_string(j + 1) + ". " + tempRace.name, font, 50);
									list[3 + j].setPosition(Vector2f(220, 118 + 50 * j));
								}
							}
							pressed = 1;
							break;
						case 1:	
							if (!selectingRace)
							{
								tempRace.clear();
								paintrace();
							}
							break;
						case 2:
							break;
						}
						pressed = 1;
					}

				}
				else list[i].setOutlineColor(Color::Black);

			}
			if (!rd) {
				rdtext.setFillColor(Color::Red);
				rdtext.setString("   Ideal cars\nwill not be used.");
			}
			else {
				rdtext.setFillColor(Color::Green);
				rdtext.setString("   Ideal cars\n will be used!");
			}
			if (!mrk) {
				mrktext.setFillColor(Color::Red);
				mrktext.setString("   Car markers\nwill not be shown.");
			}
			else {
				mrktext.setFillColor(Color::Green);
				mrktext.setString("   Car markers\n will be shown!");
			}
			textsboxes[3] = rdtext.getGlobalBounds();
			textsboxes[4] = mrktext.getGlobalBounds();
			textsboxes[5] = SelectRaceBack.getGlobalBounds();
			if (textsboxes[3].contains(Vector2f(Mouse::getPosition(menuwin)))) {
				rdtext.setOutlineColor(Color::Yellow);
				if (Mouse::isButtonPressed(Mouse::Left) && !pressed)
					rd = !rd;
			}
			else rdtext.setOutlineColor(Color::Black);
			if (textsboxes[4].contains(Vector2f(Mouse::getPosition(menuwin))))
			{
				mrktext.setOutlineColor(Color::Yellow);
				if (Mouse::isButtonPressed(Mouse::Left) && !pressed)
					mrk = !mrk;
			}
			else mrktext.setOutlineColor(Color::Black);

			if (!textsboxes[5].contains(Vector2f(Mouse::getPosition(menuwin))))
				selectingRace = 0;

			anypressed = 0;
			if (Keyboard::isKeyPressed(Keyboard::BackSpace) && !pressed)
			{
				nm /= 10;
				pressed = 1;
			}

			for (size_t i = 0; i < 10; i++)
				if (Keyboard::isKeyPressed(Keyboard::Key(26 + i)) || Mouse::isButtonPressed(Mouse::Left) || Keyboard::isKeyPressed(Keyboard::BackSpace))
				{
					anypressed = 1;
					if (!pressed)
					{
						if (!Mouse::isButtonPressed(Mouse::Left))
							nm = nm * 10 + i;
						pressed = 1;
						break;
					}
				}
			if (!anypressed)
				pressed = 0;

			menuwin.display();

		}
	}
	
	void paintrace()
	{
		int paintmode = 0; bool modeselecting = 0, pressed = 0;
		Text header("Make your own race!", font, 23), list[3] = {
			Text(" Paint wall\n     lines", font, 18),
			Text("Set the start\n   position", font , 18),
			Text("Track name: ", font , 20) };
		Texture StartPosTX; StartPosTX.loadFromFile("StartPos.png");
		Sprite StartPos(StartPosTX); StartPos.setPosition(tempRace.startPos);
		StartPos.setOrigin({ 15, 30 });
		FloatRect textsboxes[4];
		header.setStyle(Text::Bold);
		header.setPosition(270, 10);
		header.setFillColor(Color::Black);
		header.setOutlineThickness(3);
		header.setOutlineColor(Color::Red);
		description.setString("Select paint mode:");
		description.setPosition(7, 5);
		description.setCharacterSize(17);
		for (size_t i = 0; i < 2; i++) {
			list[i].setStyle(Text::Bold);
			if (i < 2)	list[i].setPosition(15, 40 + i * 50);
			list[i].setFillColor(Color(0, 255, 0));
			list[i].setOutlineThickness(1);
			list[i].setOutlineColor(Color::Black);
			textsboxes[i] = list[i].getGlobalBounds();
		}	
		list[2].setString(tempRace.name);
		list[2].setPosition({ 550, 15 });
		RenderWindow paintwin(VideoMode(800, 600), "AI-Race Paint");
		paintwin.setFramerateLimit(60);
		while (paintwin.isOpen())
		{
			Event event;
			while (paintwin.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					paintwin.close();
					tempRace.save();
					return;
				}
			}
			paintwin.clear(Color(195, 195, 195));
			paintwin.draw(header);
			paintwin.draw(description);
			paintwin.draw(list[2]);
			modeselecting = 0;
			/*for (size_t i = 0; i < 2; i++)
			{
				if (i != paintmode)
					list[i].setOutlineColor(Color::Black);
				paintwin.draw(list[i]);
				textsboxes[i] = list[i].getGlobalBounds();
				if (textsboxes[i].contains(Vector2f(Mouse::getPosition(paintwin))))
				{
					if (Mouse::isButtonPressed(Mouse::Left))
						paintmode = i;
					modeselecting = 1;
				}
			}*/
			list[paintmode].setOutlineColor(Color::Yellow);
			if (StartPos.getGlobalBounds().contains(Vector2f(Mouse::getPosition(paintwin)))) {
				if (Mouse::isButtonPressed(Mouse::Left)){
					StartPos.setPosition((Vector2f(Mouse::getPosition(paintwin))));
					tempRace.startPos = StartPos.getPosition();
				}
			}
			else
				if (Mouse::isButtonPressed(Mouse::Left) && !modeselecting)
			{
				tempRace.lines.resize(tempRace.lines.getVertexCount() + 2);
				tempRace.lines[tempRace.lines.getVertexCount() - 2].position = Vector2f(Mouse::getPosition(paintwin));
				while (Mouse::isButtonPressed(Mouse::Left))
				{
					tempRace.lines[tempRace.lines.getVertexCount() - 1].position = Vector2f(Mouse::getPosition(paintwin));

					paintwin.clear(Color(195, 195, 195));
					paintwin.draw(StartPos);
					paintwin.draw(tempRace.lines);
					paintwin.display();
				}
			}
			if (!pressed)
			{
				for (int i = 0; i < 28; i++)

					if (Keyboard::isKeyPressed(Keyboard::Key(i))) {
						pressed = 1;
						//std::cout <<'\n' << i + '0' << ' ' << char(i + '0');
						tempRace.name += i + 'a';
						list[2].setString(tempRace.name);
					}
				if (Keyboard::isKeyPressed(Keyboard::BackSpace)) {
					if (tempRace.name.size() > 0)
						tempRace.name.pop_back();
					list[2].setString(tempRace.name);
					pressed = 1;
				}
			}
			if (pressed)
			{
				pressed = 0;
				for (int i = 0; i < 28; i++)
					if (Keyboard::isKeyPressed(Keyboard::Key(i)) || Keyboard::isKeyPressed(Keyboard::BackSpace))
						pressed = 1;
			}
			paintwin.draw(tempRace.lines);
			paintwin.draw(StartPos);
			paintwin.display();
		}
	}
	bool cross(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int dotnum) {
						//do 2 lines intersect?
		float n, lengt, dist1, dist2;
		if (y2 - y1 != 0) {  // a(y)
			float q = (x2 - x1) / (y1 - y2);
			float sn = (x3 - x4) + (y3 - y4) * q; if (!sn) { return 0; }  // c(x) + c(y)*q
			float fn = (x3 - x1) + (y3 - y1) * q;   // b(x) + b(y)*q
			n = fn / sn;
		}
		else {
			if (!(y3 - y4)) { return 0; }  // b(y)
			n = (y3 - y1) / (y3 - y4);   // c(y)/b(y)
		}
		dots[dotnum * 2] = x3 + (x4 - x3) * n;  // x3 + (-b(x))*n
		dots[dotnum * 2 + 1] = y3 + (y4 - y3) * n;  // y3 +(-b(y))*n
		lengt = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
		dist1 = sqrt((x1 - dots[dotnum * 2]) * (x1 - dots[dotnum * 2]) + (y1 - dots[dotnum * 2 + 1]) * (y1 - dots[dotnum * 2 + 1]));
		dist2 = sqrt((x2 - dots[dotnum * 2]) * (x2 - dots[dotnum * 2]) + (y2 - dots[dotnum * 2 + 1]) * (y2 - dots[dotnum * 2 + 1]));
		if (lengt > dist1 + dist2 + 1 || lengt < dist1 + dist2 - 1)
			return 0;
		if (x3 < x4) {
			if (dots[dotnum * 2] < x3 || dots[dotnum * 2] > x4)
				return 0;
		}
		else if (dots[dotnum * 2] < x4 || dots[dotnum * 2] > x3)
			return 0;
		if (y3 < y4) {
			if (dots[dotnum * 2 + 1] < y3 || dots[dotnum * 2 + 1] > y4)
				return 0;
		}
		else if (dots[dotnum * 2 + 1] < y4 || dots[dotnum * 2 + 1] > y3)
			return 0; 
		return 1;
	}

	int main()
	{
		View mainView({ 200, 300 }, { viewSizeX, viewSizeY });
		Vector2f viewMover = { 0, 0 };

		SelectRaceBackTX.loadFromFile("SelectRace.png");
		carTX.loadFromFile("car.png");
		font.loadFromFile("arial.ttf");
		dotTX.loadFromFile("dot.png");
		SelectRaceBack.setTexture(SelectRaceBackTX);

		description.setCharacterSize(20);
		description.setStyle(Text::Bold);
		description.setFillColor(Color::Red);
		
		double data[5];
		size_t aliveNum, maxscore, maxscoreNum, timer, generationNum;
		
		while (startMenu())
		{
			maxscore = 0; maxscoreNum = 0; timer = 0; generationNum = 1;  aliveNum = nm;
			cars = new Car[nm];
			AI = new AICar[nm];
			dot = new Sprite[nm * 5];
			carlines.resize(10 * (nm + 1));

			for (size_t i = 0; i < nm * 5; i++)
			{
				dot[i].setTexture(dotTX);
				dot[i].setOrigin(5, 5);
			}
			srand(time(0));
			for (size_t i = 0; i < nm; i++)
			{
				if (rd)
					AI[i].InitBestWeights("bestWeights.txt");
				else AI[i].InitRand();		//initialize AI
				cars[i].car.setTexture(carTX);
				cars[i].car.setOrigin(162, 256);
				cars[i].car.setScale(0.05f, 0.05f);
				cars[i].car.setPosition(tempRace.startPos);
			}

			RenderWindow window(VideoMode(1280, 720), "AI-Race!");
			window.setFramerateLimit(60);
			window.setView(mainView);
			while (window.isOpen())
			{
				Event event;
				while (window.pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
						window.close();
				}
				
				window.clear(Color(195, 195, 195));
				//window.draw(background);
				timer++;
				aliveNum = 0;
				maxscoreNum = 0; maxscore = 0;
				for (size_t i = 0; i < nm; i++)
				{
					
					if (cars[i].score > maxscore)
					{
						maxscore = cars[i].score + 1;
						maxscoreNum = i;
						timer = 0;
					}
					if (cars[i].alive)
					{
						aliveNum++;
						window.draw(cars[i].car);
								//direct carlines from cars to 5 directions
						carlines[i * 10].position = cars[i].car.getPosition();
						carlines[i * 10 + 1].position = cars[i].car.getPosition() + Vector2f(sin((cars[i].car.getRotation() * 3.141592) / 180) * 500,
							-cos((cars[i].car.getRotation() * 3.141592) / 180) * 500);
						carlines[i * 10 + 2].position = cars[i].car.getPosition();
						carlines[i * 10 + 3].position = cars[i].car.getPosition() + Vector2f(-cos((cars[i].car.getRotation() * 3.141592) / 180) * 200,
							-sin((cars[i].car.getRotation() * 3.141592) / 180) * 200);
						carlines[i * 10 + 4].position = cars[i].car.getPosition();
						carlines[i * 10 + 5].position = cars[i].car.getPosition() + Vector2f(cos((cars[i].car.getRotation() * 3.141592) / 180) * 200,
							sin((cars[i].car.getRotation() * 3.141592) / 180) * 200);
						carlines[i * 10 + 6].position = cars[i].car.getPosition();
						carlines[i * 10 + 7].position = cars[i].car.getPosition() + Vector2f(sin(((cars[i].car.getRotation() + 45) * 3.141592) / 180) * 300,
							-cos(((cars[i].car.getRotation() + 45) * 3.141592) / 180) * 300);
						carlines[i * 10 + 8].position = cars[i].car.getPosition();
						carlines[i * 10 + 9].position = cars[i].car.getPosition() + Vector2f(sin(((cars[i].car.getRotation() - 45) * 3.141592) / 180) * 300,
							-cos(((cars[i].car.getRotation() - 45) * 3.141592) / 180) * 300);
					}

					if (cars[i].alive)		//find intersects carlines and racetrack lines
						for (size_t j = 0; j < 5; j++)
						{
							float shortx = 0, shorty = 0, shortxy = 10000;
							for (size_t n = 0; n < tempRace.lines.getVertexCount() / 2; n++)
							{
								if (cross(cars[i].car.getPosition().x, cars[i].car.getPosition().y,
									carlines[i * 10 + j * 2 + 1].position.x, carlines[i * 10 + j * 2 + 1].position.y,
									tempRace.lines[n * 2].position.x, tempRace.lines[n * 2].position.y,
									tempRace.lines[n * 2 + 1].position.x, tempRace.lines[n * 2 + 1].position.y, j))
								{
									if (abs(dots[j * 2] - cars[i].car.getPosition().x) + abs(dots[j * 2 + 1] - cars[i].car.getPosition().y) < shortxy)
									{
										shortxy = abs(dots[j * 2] - cars[i].car.getPosition().x) + abs(dots[j * 2 + 1] - cars[i].car.getPosition().y);
										shortx = dots[j * 2]; shorty = dots[j * 2 + 1];
									}
								}
							}
							if (shortx == 0)
								dot[j + i * 5].setPosition(carlines[i * 10 + j * 2 + 1].position.x, carlines[i * 10 + j * 2 + 1].position.y);
							else dot[j + i * 5].setPosition(shortx, shorty);
							if (sqrt(pow(dot[j + i * 5].getPosition().y - cars[i].car.getPosition().y, 2)
								+ pow(dot[j + i * 5].getPosition().x - cars[i].car.getPosition().x, 2)) < 12)
								cars[i].alive = 0;

						}
				}


				for (size_t i = 0; i < nm; i++)
					//transfer to each AI distance drome car to the walls
				{
					if (cars[i].alive)
					{
						data[0] = sqrt(pow(dot[i * 5].getPosition().x - cars[i].car.getPosition().x, 2)
							+ pow(dot[i * 5].getPosition().y - cars[i].car.getPosition().y, 2));
						data[1] = sqrt(pow(dot[i * 5 + 1].getPosition().x - cars[i].car.getPosition().x, 2)
							+ pow(dot[i * 5 + 1].getPosition().y - cars[i].car.getPosition().y, 2));
						data[2] = sqrt(pow(dot[i * 5 + 2].getPosition().x - cars[i].car.getPosition().x, 2)
							+ pow(dot[i * 5 + 2].getPosition().y - cars[i].car.getPosition().y, 2));
						data[3] = sqrt(pow(dot[i * 5 + 3].getPosition().x - cars[i].car.getPosition().x, 2)
							+ pow(dot[i * 5 + 3].getPosition().y - cars[i].car.getPosition().y, 2));
						data[4] = sqrt(pow(dot[i * 5 + 4].getPosition().x - cars[i].car.getPosition().x, 2)
							+ pow(dot[i * 5 + 4].getPosition().y - cars[i].car.getPosition().y, 2));

						cars[i].controlSpeed(AI[i].InputData(data));
					}
				}
				description.setString("Generation: " + std::to_string(generationNum) + "\nCurrent max score: " + std::to_string(maxscore) + "\nAlive: " + std::to_string(aliveNum));
				
				viewMover += { (cars[maxscoreNum].car.getPosition().x - viewMover.x)/10, (cars[maxscoreNum].car.getPosition().y - viewMover.y) / 10};
				
				mainView.setCenter(viewMover);
				window.setView(mainView);
				if(!Keyboard::isKeyPressed(Keyboard::Enter))
					pressed = 0;
				if (aliveNum == 0 || (timer > 240 && !rd) || (Keyboard::isKeyPressed(Keyboard::Enter) && !pressed))
						//next generation
				{
					AI[maxscoreNum].SaveWeights();
					for (size_t i = 0; i < nm; i++)
					{
						AI[i].InitBestWeights("weights.txt");
						cars[i].score = 0; cars[i].alive = 1;
						cars[i].car.setPosition(tempRace.startPos);
						cars[i].car.setRotation(0);
					}
					timer = 0; maxscore = 0; maxscoreNum = 0; generationNum++; pressed = 1;
				}
				if (Keyboard::isKeyPressed(Keyboard::Escape))
					window.close();
				
				if (mrk) {
					window.draw(carlines);
					for (size_t i = 0; i < nm * 5; i++)
						if (cars[i / 5].alive)
							window.draw(dot[i]);
				}
				window.draw(tempRace.lines);
				description.setPosition(240, 215);
				window.draw(description);
				window.display();
			}	
		}
		return 0;
	}
