#include "pacman.h"
using namespace sf;
using namespace std;
//setting the dimensions of the window
const int side_ = 16, W_ = 48 * side_, H_ = 35 * side_;
bool wall_[H_][W_];
Texture obstacle_image;
//implementation for game objects
class GameObject_{
public:
	bool edible;
	int x;
	int y;
	int score;
	Sprite shape;
	Texture image;
};
class Snake :public GameObject_{
public:
	Snake(){}
	//function to check for collisions
	bool ISCollide(GameObject_ &object)
	{
		return sqrt(abs(x - object.x)*abs(x - object.x) + abs(y - object.y)*abs(y - object.y)) < side_;
	}
};
class Fruit_ :public GameObject_
{
public:
	int score;
	Fruit_()
	{}
	Fruit_(int x, int y, int score)
	{
		this->x = x;
		this->y = y;
		this->score = score;
	}
};
class Game_
{
public:
	char direction;
	vector <GameObject_> Board;
	deque <Snake> Player;
	Fruit_ fruit;
	//moving function
	void move()
	{
		Snake snake_ = Player.front();
		wall_[Player.front().y / side_][Player.front().x / side_] = 1; // adjusting the snake itself as a boundary
		snake_.x += W_;
		snake_.y += H_;
		//changing the position
		if (direction == 'N')
			snake_.y -= side_;
		else if (direction == 'S')
			snake_.y += side_;
		else if (direction == 'E')
			snake_.x += side_;
		else if (direction == 'W')
			snake_.x -= side_;
		//connect the left side_ of the window with the right side_
		snake_.x %= W_;
		snake_.y %= H_;
		//................................
		wall_[Player.back().y / side_][Player.back().x / side_] = 0;
		//removing the tail and positioning it in fornt of the head
		Player.pop_back();
		Player.push_front(snake_);
	}
	//implementation of the drawing function
	void Draw(RenderWindow &window)
	{
		for (deque<Snake> ::iterator it = Player.begin(); it != Player.end(); it++)
		{
			it->shape.setPosition(Vector2f(it->x, it->y));
			window.draw(it->shape);
		}
		for (int i = 0; i < Board.size(); i++)
		{
			Board[i].shape.setPosition(Vector2f(Board[i].x, Board[i].y));
			window.draw(Board[i].shape);
		}
		fruit.shape.setPosition(fruit.x, fruit.y);
		window.draw(fruit.shape);
	}
};
class Wall_ :public GameObject_
{};