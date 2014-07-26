#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <utility>
#include <vector>
#include <queue>
using namespace std;
using namespace sf;

const int side = 35, W = 19 * side, H = 20 * side, error = 15;
bool wall[H][W];
Texture obs,energy;
int arrx[4] = { 0, 0, side, -side };
int arry[4] = { -side, side, 0, 0 };
class GameObject
{
public:
	Texture image;
	bool edible;
	int x, y, score;
	Sprite shape;
	string type;
	GameObject()
	{}
	GameObject(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};
class Pacman :public GameObject
{
public:
	Texture image1, image2, image3, image4;
	Clock clock;
	Pacman()
	{}
	char direction;
	Pacman(int x, int y, char direction)
	{
		this->x = x;
		this->y = y;
		this->direction = direction;
	}
	void move()
	{
		x += W;
		y += H;
		bool stopped=false;
		if (direction == 'N'&&!wall[(y - 1) % H][(x + error) % W] && !wall[(y - 1) % H][(x + side - 1 - error) % W])
			y--;
		else if (direction == 'S'&&!wall[(y + side) % H][(x + error) % W] && !wall[(y + side) % H][(x + side - 1 - error) % W])
			y++;
		else if (direction == 'E'&&!wall[(y + error) % H][(x + side) % W] && !wall[(y + side - 1 - error) % H][(x + side) % W])
			x++;
		else if (direction == 'W'&&!wall[(y + error) % H][(x - 1) % W] && !wall[(y + side - 1 - error) % H][(x - 1) % W])
			x--;
		else
		{
			stopped = 1;
			if (direction == 'N')
				shape.setTexture(image1);
			if (direction == 'S')
				shape.setTexture(image2);
			if (direction == 'W')
				shape.setTexture(image3);
			if (direction == 'E')
				shape.setTexture(image);
		}
		x %= W;
		y %= H;
		if (direction == 'N' || direction == 'S')
		{
			if (x%side > side / 2)
				x = x - x% side + side;
			else
				x -= x%side;
		}
		if (direction == 'E' || direction == 'W')
		{
			if (y%side > side / 2)
				y = y - y%side + side;
			else
				y -= y% side;
		}
		if (clock.getElapsedTime().asMilliseconds() >= 200 && !stopped)
		{
			shape.setTexture(image4);
			clock.restart();
		}
		else if (clock.getElapsedTime().asMilliseconds() >= 100)
		{
			if (direction == 'N') shape.setTexture(image1);
			else if (direction == 'S') shape.setTexture(image2);
			else if (direction == 'W') shape.setTexture(image3);
			else shape.setTexture(image);
		}
	}
	bool ISCollide(GameObject &obj)
	{
		int x1 = obj.x, y1 = obj.y;
		return sqrt(abs(x - x1)*abs(x - x1) + abs(y - y1)*abs(y - y1)) < side / 2;
	}
};
class Monster :public GameObject
{
public:
	bool is_scared,is_dead;
	Clock scaryClock;
	GameObject house;
	char direction;
	int  right, left, up, shater, down, range, start_x, start_y;
	int distance(GameObject &player, int x, int y)
	{
		return abs(x - player.x) + abs(y - player.y);
	}
	bool is_valid(int y, int x)
	{
		if (!wall[(y + H) % H][(x + W) % W])
			return true;
		return false;
	}
	int bfs(GameObject &player, char direction)
	{
		queue < pair <int, pair<int, int > > > qee;
		bool visited[H][W];
		memset(visited, 0, sizeof visited);
		if (!wall[(y - side + H) % H][x%W] && direction != 'S'){
			qee.push(make_pair(0, make_pair((y - side + H) % H, x)));
			visited[(y - side + H) % H][x] = 1;
		}
		if (!wall[(y + side) % H][x%W] && direction != 'N'){
			qee.push(make_pair(1, make_pair((y + side) % H, x)));
			visited[(y + side) % H][x] = 1;
		}
		if (!wall[y%H][(x + side) % W] && direction != 'W'){
			qee.push(make_pair(2, make_pair(y, (x + side) % W)));
			visited[y][(x - side + W) % W] = 1;
		}
		if (!wall[y%H][(x - side + W) % W] && direction != 'E'){
			qee.push(make_pair(3, make_pair(y, (x - side + W) % W)));
			visited[y][(x + side) % W] = 1;
		}
		while (qee.size())
		{
			if (abs(qee.front().second.first - player.y) * 2 <= side && 2 * abs(qee.front().second.second - player.x) <= side)
				return qee.front().first;
			for (int i = 0; i < 4; i++)
			{
				if (is_valid(arry[i] + qee.front().second.first, arrx[i] + qee.front().second.second) && visited[(arry[i] + qee.front().second.first + H) % H][(arrx[i] + qee.front().second.second + W) % W] == false)
				{
					qee.push(make_pair(qee.front().first, make_pair((qee.front().second.first + arry[i] + H) % H, (qee.front().second.second + arrx[i] + W) % W)));
					visited[(qee.front().second.first + arry[i] + H) % H][(qee.front().second.second + arrx[i] + W) % W] = true;
				}
			}
			qee.pop();
		}
	}
	void move(Pacman &player)
	{
		if (x%side == 0 && y%side == 0)
		{
			right = left = up = down = 0;
			if (!wall[(y - side + H) % H][x] && direction != 'S')
				up++; 
			if (!wall[(y + side) % H][x] && direction != 'N')
				down++;
			if (!wall[y][(x + side) % W] && direction != 'W') 
				right++;
			if (!wall[y][(x - side + W) % W] && direction != 'E')
				left++;
			if (distance(house, x, y) < side && is_dead)
			{
				shape.setTexture(image);
				is_dead = is_scared = 0;
			}
			if (is_dead)
			{
				up = down = right = left = 0;
				int temp = bfs(house, direction);
				if (temp == 0) up = shater;
				else if (temp == 1) down = shater;
				else if (temp == 2) right = shater;
				else if (temp == 3) left = shater;
			}
			else if (distance(player, x, y) <= range)
			{
				int temp = bfs(player, direction);
				if (is_scared)
				{
					if (temp == 0){ down *= shater, right *= shater, left *= shater; }
					if (temp == 1){ up *= shater, right *= shater, left *= shater; }
					if (temp == 2){ down *= shater, up *= shater, left *= shater; }
					if (temp == 3){ down *= shater, right *= shater, up *= shater; }
				}
				else
				{
					if (temp == 0) up *= shater;
					else if (temp == 1) down *= shater;
					else if (temp == 2) right *= shater;
					else if (temp == 3) left *= shater;
				}
			}
			int r = rand() % (up + down + left + right);
			if (r < down)
				direction = 'S';
			else if (r < up + down)
				direction = 'N';
			else if (r < right + up + down)
				direction = 'E';
			else
				direction = 'W';
		}
		if (direction == 'N')
			y--;
		else if (direction == 'S')
			y++;
		else if (direction == 'E')
			x++;
		else if (direction == 'W')
			x--;
		x = (x + W) % W;
		y = (y + H) % H;
	}
	Monster()
	{}
	Monster(int x, int y, char direction)
	{
		this->x = x;
		this->y = y;
		this->direction = direction;
	}
};
class Fruit :public GameObject
{
public:
	Fruit()
	{}
	Fruit(int x, int y, int score)
	{
		this->x = x;
		this->y = y;
		this->score = score;
	}
};
class Game
{
public:
	Pacman player;
	Monster ghost[4];
	vector <GameObject> Board;
	void Draw(RenderWindow &window)
	{
		player.shape.setPosition(Vector2f(player.x, player.y));
		window.draw(player.shape);
		for (int i = 0; i < Board.size(); i++)
		{
			Board[i].shape.setPosition(Vector2f(Board[i].x, Board[i].y));
			window.draw(Board[i].shape);
		}
		for (int i = 0; i < 4; i++)
		{
			ghost[i].shape.setPosition(Vector2f(ghost[i].x, ghost[i].y));
			window.draw(ghost[i].shape);
		}
	}
};
class Wall :public GameObject
{};
class Energizer :public GameObject
{};