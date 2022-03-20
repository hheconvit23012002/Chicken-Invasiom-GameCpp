#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<iostream>
#include<vector>
#include<cstdlib>
#include<ctime>
#include <list>
#include<sstream>
using namespace std;
using namespace sf;
float DEGTORAD = 0.017453f;
float deltaTime = 0;
int game = 1000;
int poin = 0;
int check = 0;
class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
	vector<IntRect> frames;
	Animation() {}

	Animation(Texture& t, int x, int y, int w, int h, int count, float Speed)
	{
		Frame = 0;
		speed = Speed;

		for (int i = 0; i < count; i++)
			frames.push_back(IntRect(x + i * w, y, w, h));

		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);
	}
	void update()
	{
		Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
	}

	bool isEnd()
	{
		return Frame + speed >= frames.size();
	}

};
class Entity
{
public:
	float x, y, dx, R, dy, angle;
	bool life;
	string name;
	int hp = 500;
	Animation anim;

	Entity()
	{
		life = 1;
	}

	void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 1)
	{
		anim = a;
		angle = Angle;
		x = X; y = Y;
		R = radius;
	}
	virtual void update() {};

	void draw(RenderWindow& app)
	{
		anim.sprite.setPosition(x, y);
		app.draw(anim.sprite);
	}
	virtual ~Entity() {};
	float getX() {
		return this->x;
	}
	float getY() {
		return this->y;
	}
};
vector<Entity*> entities;

class Chicken : public Entity
{
public:
	Chicken()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "Chicken";
	}
	virtual void update()
	{
		if (dy < 0) dy = -dy;
		while (dy == 0) {
			dy = rand() % 8 - 4;
		}

		y += dy;
		if (y >= 1000) {
			life = 0;
		}

	}
};
class Egg : public Entity
{
public:

	Egg()
	{
		dy = 6;
		name = "Egg";
	}
	virtual void update()
	{
		y += dy;
		if (y >= 1000) {
			life = 0;

		}
	}
};
class bullet :public Entity {
public:
	bullet()
	{
		name = "bullet";
	}
	void update() {
		dx = 0;
		dy = 6;
		x -= dx;
		y -= dy;

		if (y > 1000 || y < 0) {
			life = 0;
		}
	}
};
class Boss : public Entity
{
public:
	int check_boss = 0;
	Boss()
	{
		name = "Boss";
	}
	void update() {
		int x0 = 3, y0 = 0;
		if (x > 1470) {
			check_boss = 1;
		}
		if (x < 30) {
			check_boss = 2;
		}
		if (check_boss == 1) {
			x0 = -3;
		}
		if (check_boss == 2) {
			x0 = 3;
		}
		if (y > 960) {
			if (y0 > 0) {
				y0 = 0;
			}
		}
		if (y < 40) {
			if (y0 < 0) {
				y0 = 0;
			}
		}
		x += x0;
		y += y0;
	}

};

class sReload {
public:
	float t = 0;
	int okk = 1;
	float t0 = 0;
	float t1 = 0;
public:
	void RunEntity(Animation& sChicken, Animation sEgg,Animation& boss ) {
		t += deltaTime;
		t0 += deltaTime;
		t1 += deltaTime;
		if (t0>=30 && check==0 ) {
			Boss* a = new Boss();
			a->settings(boss, 0, 55, 0, 50);
			check = 1;
			if (t1 >= 3.3) {
				Egg* e_ = new Egg();
				e_->settings(sEgg, a->x, a->y, 0, 50);
				entities.push_back(e_);
				t1 = 0;
			}
			entities.push_back(a);
			t0 = 0;
		}
		if (t >= 2) {
			Chicken* a = new Chicken();
			
			a->settings(sChicken, rand() % 1450, -100, 0, 50);
			if (t1 >= 2.3) {
				Egg* e_ = new Egg();
				e_->settings(sEgg, a->x, a->y, 0, 50);
				entities.push_back(e_);
				t1 = 0;
			}
			entities.push_back(a);
			t = 0;
		}
		
		//cout << entities.size();
		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity* e = *i;
			e->update();
			e->anim.update();
			/*Egg* a = new Egg();
			a->settings(sEgg, e->x, e->y);
			entities.push_back(a);*/
			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}
	}
};
class player : public Entity
{
public:
	bool thrust;

	player()
	{
		name = "player";
	}

	void update(float x0, float y0)
	{
		if (x > 1470) {
			if (x0 > 0) {
				x0 = 0;
			}
		}
		if (x < 30) {
			if (x0 < 0) {
				x0 = 0;
			}
		}
		if (y > 960) {
			if (y0 > 0) {
				y0 = 0;
			}
		}
		if (y < 40) {
			if (y0 < 0) {
				y0 = 0;
			}
		}
		x += x0;
		y += y0;
	}

};
bool isCollide(Entity* a, Entity* b)
{
	return (b->x - a->x) * (b->x - a->x) +
		(b->y - a->y) * (b->y - a->y) <
		(a->R + b->R) * (a->R + b->R);
}
int main() {
	int HighScore = 0;
	SoundBuffer danban;
	danban.loadFromFile("audio/dan_ban.wav");
	Sound danbanaudio;
	danbanaudio.setBuffer(danban);
	SoundBuffer no;
	no.loadFromFile("audio/dan_ban.wav");
	Sound noaudio;
	noaudio.setBuffer(no);
	RenderWindow window_intro(sf::VideoMode(1250, 1000), "intro");
	Texture intro;
	intro.loadFromFile("images/intro.png");
	Sprite  introgame(intro);
	float t_intro = 0;
	Clock clock_intro;
	while (window_intro.isOpen())
	{
		Vector2i pos = Mouse::getPosition(window_intro);
		int x = pos.x;
		int y = pos.y;
		Event event;
		while (window_intro.pollEvent(event))
		{
			t_intro += clock_intro.restart().asSeconds();
			if (event.type == Event::Closed)
			{
				window_intro.close();
			}
			//if (event.type == Event::MouseButtonPressed)
			//	if (event.key.code == Mouse::Left) {
			//		if (x > 0 && x < 1250 && y < 1000 && y>0) {
			//			window_intro.close();
			//		}
			//	}
			if (t_intro >= 2) {
				window_intro.close();
			}
		}
		window_intro.clear();
		window_intro.draw(introgame);
		window_intro.display();
	}
	while (1) {
		int ok = -1;
		RenderWindow windows(sf::VideoMode(1474, 922), "Menu");
		Texture giaodiencap;
		giaodiencap.loadFromFile("images/state.png");
		Sprite  giaodiencapgame(giaodiencap);
		while (windows.isOpen())
		{
			Vector2i pos = Mouse::getPosition(windows);
			int x = pos.x;
			int y = pos.y;
			Text myText3;
			Font font3;
			font3.loadFromFile("D:/Game/TestGamecpp/TestGamecpp/font/MagdalenaAreli-Wydqv.otf");
			stringstream ss3;
			ss3 << HighScore;
			myText3.setString(ss3.str().c_str());
			myText3.setPosition(580, 385);
			myText3.setFillColor(Color::White);
			myText3.setCharacterSize(50);
			myText3.setFont(font3);
			Event event;
			while (windows.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					ok = 0;
					windows.close();
				}
				if (event.type == Event::MouseButtonPressed)
					if (event.key.code == Mouse::Left) {
						if (x>150 && x<700 && y<770 && y>630) {
							ok = 1;
							windows.close();
						}
						if (y > 550 && y< 840 && x>1140 && x<1350) {
							ok = 2;
							windows.close();
						}
					}
			}
			windows.clear(Color::White);
			windows.draw(giaodiencapgame);
			windows.draw(myText3);
			windows.display();
		}
		int okk = 1;
		if (ok == 0) {
			break;
		}
		if (ok == 1) {
			int kill = 3;
			RenderWindow window(VideoMode(1500, 1000), "Game");
			window.setFramerateLimit(90);
			Texture t1, t2, t3, t4, t5, t6, t7, t8, t9;
			t9.loadFromFile("images/OrdinaryChickenMilitary.png");
			t1.loadFromFile("images/planes.png");
			t2.loadFromFile("images/background.png");
			t3.loadFromFile("images/explosions/type_C.png");
			t4.loadFromFile("images/DroneChicken.png");
			t5.loadFromFile("images/Egg.png");
			t6.loadFromFile("images/fire_red.png");
			t7.loadFromFile("images/explosions/type_B.png");
			Sprite  sBackground(t2);
			//setTextureRect lay 1 anh trong nhieu anh
			// vi tri tuong doi voi 0,0
			Animation sPlayer(t1, 0, 0, 80, 80, 1, 0.8);
			Animation sPlayer_go(t1, 0, 0, 80, 80, 1, 0.0);
			Animation sBullet(t6, 0, 0, 32, 64, 16, 0.8);
			Animation sChicken(t4, 0, 0, 100, 95, 1, 0.2);
			Animation sEgg(t5, 0, 0, 50, 51, 1, 0.2);
			Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
			Animation boss(t9, 0, 0, 130, 123, 1, 0.8);
			sReload* a = new sReload();
			//	a->DataEntity(sChicken);
			Clock clock;
			float totalTime = 0, fps = 0;
			float paddleSpeed = 300.f;
			player* p = new player();
			p->settings(sPlayer, 800, 900, 0, 40);
			entities.push_back(p);
			while (window.isOpen()) {
				deltaTime = clock.restart().asSeconds();
				totalTime += deltaTime;
				fps++;

				if (totalTime >= 1) {
					totalTime -= 1;
					//cout << fps << endl;
					fps = 0;
				}
				Event event;
				while (window.pollEvent(event)) {
					if (event.type == Event::Closed) {
						window.close();
					}
					if (event.type == Event::KeyPressed)
						if (event.key.code == Keyboard::Space)
						{
							danbanaudio.play();
							bullet* b = new bullet();
							b->settings(sBullet, p->x, p->y, 10);
							entities.push_back(b);

						}
				}
				if (Keyboard::isKeyPressed(Keyboard::D)) p->update(6, 0);
				if (Keyboard::isKeyPressed(Keyboard::A)) p->update(-6, 0);
				if (Keyboard::isKeyPressed(Keyboard::S)) p->update(0, 6);
				if (Keyboard::isKeyPressed(Keyboard::W)) p->update(0, -6);
				Text myText;
				Font font;
				font.loadFromFile("D:/Game/TestGamecpp/TestGamecpp/font/MagdalenaAreli-Wydqv.otf");
				stringstream ss;
				ss << poin;
				myText.setString(ss.str().c_str());
				myText.setPosition(140, 7);
				myText.setFillColor(Color::White);
				myText.setCharacterSize(50);
				myText.setFont(font);
				Text myText1;
				Font font1;
				font1.loadFromFile("D:/Game/TestGamecpp/TestGamecpp/font/MagdalenaAreli-Wydqv.otf");
				stringstream ss1;
				ss1 << kill;
				myText1.setString(ss1.str().c_str());
				myText1.setPosition(135, 65);
				myText1.setFillColor(Color::White);
				myText1.setCharacterSize(50);
				myText1.setFont(font1);
				Text myText3;
				for (auto a : entities) {
					for (auto b : entities)
					{
						if (a->name == "Chicken" && b->name == "bullet")
							if (isCollide(a, b))
							{
								poin++;
								if (poin > HighScore) {
									HighScore = poin;
								}
								noaudio.play();
								a->life = false;
								b->life = false;
							}
						if (a->name == "player" && b->name == "Chicken")
							if (isCollide(a, b))
							{
								noaudio.play();
								b->life = false;
								p->settings(sPlayer, 750, 900, 0, 20);
								kill--;
								if (kill == 0) {
									window.close();
								}
							}
						if (a->name == "player" && b->name == "Egg")
							if (isCollide(a, b))
							{
								kill--;
								noaudio.play();
								b->life = false;
								p->settings(sPlayer, 750, 900, 0, 20);
								if (kill == 0) {
									window.close();
								}
							}
						if (a->name == "Boss" && b->name == "bullet")
							if (isCollide(a, b))
							{
								a->hp--;
								cout << a->hp << endl;
								b->life == false;
								poin += 10;
								noaudio.play();
								if (a->hp == 0) {
									a->life = false;
									check = 0;
								}
							}
					}
				}
				if (p->thrust)  p->anim = sPlayer_go;
				else   p->anim = sPlayer;
				a->RunEntity(sChicken, sEgg, boss);
				window.clear();
				window.draw(sBackground);
				window.draw(myText);
				window.draw(myText1);
				for (auto i : entities) i->draw(window);
				window.display();
			}
			RenderWindow window_(sf::VideoMode(1608, 1000), "ket thuc");
			Texture giaodiencap_;
			giaodiencap_.loadFromFile("images/End.png");
			Sprite  giaodiencapgame_(giaodiencap_);
			while (window_.isOpen())
			{
				Vector2i pos = Mouse::getPosition(window_);
				int x = pos.x;
				int y = pos.y;

				Event event;
				while (window_.pollEvent(event))
				{
					if (event.type == Event::Closed)
					{
						okk = 0;
						window_.close();
					}
					if (event.type == Event::MouseButtonPressed)
						if (event.key.code == Mouse::Left) {
							if (x > 1400 && x < 1600 && y < 1000 && y>950) {
								window_.close();
							}
						}
				}
				Text myText2;
				Font font2;
				font2.loadFromFile("D:/Game/TestGamecpp/TestGamecpp/font/MagdalenaAreli-Wydqv.otf");
				stringstream ss2;
				ss2 << poin;
				myText2.setString(ss2.str().c_str());
				myText2.setPosition(970, 625);
				myText2.setFillColor(Color::White);
				myText2.setCharacterSize(60);
				myText2.setFont(font2);
				window_.clear();
				window_.draw(giaodiencapgame_);
				window_.draw(myText2);
				window_.display();
			}
		}
		if (ok == 2) {
			RenderWindow window_help(sf::VideoMode(1470, 914), "Help");
			Texture help_;
			help_.loadFromFile("images/help.png");
			Sprite  giaodienhelp(help_);
			while (window_help.isOpen())
			{
				Vector2i pos = Mouse::getPosition(window_help);
				int x = pos.x;
				int y = pos.y;
				Event event;
				while (window_help.pollEvent(event))
				{
					if (event.type == Event::Closed)
					{
						window_help.close();
					}
					if (event.type == Event::MouseButtonPressed)
						if (event.key.code == Mouse::Left) {
							if (x > 1380 && x < 1470 && y < 70 && y>0) {
								window_help.close();
							}
						}
				}
				window_help.clear();
				window_help.draw(giaodienhelp);
				window_help.display();
			}
		}
		entities.clear();
		poin = 0;
		if (okk == 0) {
			break;
		}
	}
}