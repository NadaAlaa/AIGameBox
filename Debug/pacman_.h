#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <utility>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <queue>
#include <deque>
#include <stack>
#include <iterator>
using namespace std;
using namespace sf;
const int side = 35, W = 19 * side, H = 20 * side, error = 16; //error factor for adjasting the roundings
bool wall[H][W]; //boolean array for checking the obstacles
//available direction on the x-axis and y-axis
int arrx[4] = { 0, 0, side, -side };
int arry[4] = { -side, side, 0, 0 };
//class for game objects, all the game objects are inherited from this class
class GameObject
{
public:
	Texture image;
	bool edible; //to check for fruits and edible ghosts
	int x, y, score; //score for each edible object
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
	Texture image1, image2, image3, image4; //four images for the four directions
	Clock clock; //clock for Pacman's movement, so it would be constant
	Pacman()
	{}
	char direction;
	Pacman(int x, int y, char direction)
	{
		this->x = x;
		this->y = y;
		this->direction = direction;
	}
	//move function for Pacman's movement
	void move()
	{
		x += W;
		y += H;
		bool stopped = false; //checks if Pacman hit a wall
		//checking the available directions, in case the upper arrow key is pressed and there exist a wall, so it would continue in its original
		//direction
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
			//changing the images depending on the direction
			stopped = 1;
			if (direction == 'N')
				shape.setTexture(image1);
			if (direction == 'S')
				shape.setTexture(image2);
			if (direction == 'W')
				shape.setTexture(image3);
			if (direction == 'E')
				shape.setTexture(image);
			//....................
		}
		x %= W;
		y %= H;
		//checks for the boundaries of the square it is moving into, so diagonal moves can not exist
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
		//Switching between the two images (opened and closed mouth) every 100 milliseconds
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
		//......................
	}
	//Function to check for collisions
	bool ISCollide(GameObject &object){
		return sqrt(abs(x - object.x)*abs(x - object.x) + abs(y - object.y)*abs(y - object.y)) < side / 2;
	}
};
class Monster :public GameObject{
public:
	bool is_scared, is_dead;
	Clock scaryClock;
	GameObject house; //Original position for each ghost
	char direction;
	int  right, left, up, bestMoveFactor, down, range, start_x, start_y;
	//Measure distance between Pacman and the Ghost to check if Pacman is in the range of the Ghost
	int distance(GameObject &player, int x, int y) {
		return abs(x - player.x) + abs(y - player.y);
	}
	//Check for valid directions where there exist no walls
	bool is_valid(int y, int x)
	{
		if (!wall[(y + H) % H][(x + W) % W])
			return true;
		return false;
	}
	//BFS function returns the best movement for the Ghost to reach Pacman
	int bfs(GameObject &player, char direction)
	{
		queue < pair <int, pair<int, int > > > qee; //queue of pair of integer (direction to move) and a pair of two integers (xCo and yCo)
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
				return qee.front().first; //ends the BFS function when it reaches the destination and returns the best direction to move to
			//for loop to check for the four (up down left and right) directions
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
	//end of the BFS function
	//.............................
	//function move for the ghost after finding the best path to the player
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
			//checks if the ghost is dead and reached his house so it gets back to life
			if (distance(house, x, y) < side && is_dead)
			{
				shape.setTexture(image);
				is_dead = is_scared = 0;
			}
			//if the ghost is dead/eaten, then the BFS function is called with different parameters, as the ghost's destination becomes his house
			if (is_dead)
			{
				up = down = right = left = 0;
				int bestMove = bfs(house, direction);
				if (bestMove == 0) up = bestMoveFactor;
				else if (bestMove == 1) down = bestMoveFactor;
				else if (bestMove == 2) right = bestMoveFactor;
				else if (bestMove == 3) left = bestMoveFactor;
			}
			//check if the player is in the ghost's range
			else if (distance(player, x, y) <= range)
			{
				int bestMove = bfs(player, direction); //find the best move using BFS
				//if the pacman is energized, the ghost is scared and the best move factor is multiplied by all the possible directionto get far from the player
				if (is_scared)
				{
					if (bestMove == 0){
						down *= bestMoveFactor;
						right *= bestMoveFactor;
						left *= bestMoveFactor;
					}
					if (bestMove == 1){
						up *= bestMoveFactor;
						right *= bestMoveFactor;
						left *= bestMoveFactor;
					}
					if (bestMove == 2){
						down *= bestMoveFactor;
						up *= bestMoveFactor;
						left *= bestMoveFactor;
					}
					if (bestMove == 3){
						down *= bestMoveFactor;
						right *= bestMoveFactor;
						up *= bestMoveFactor;
					}
				}
				//else, multiply the best move factor by the best direction value returned from the BFS function
				else
				{
					if (bestMove == 0) up *= bestMoveFactor;
					else if (bestMove == 1) down *= bestMoveFactor;
					else if (bestMove == 2) right *= bestMoveFactor;
					else if (bestMove == 3) left *= bestMoveFactor;
				}
			}
			//giving the ghost some random moves so it doesn't chase pacman all the time
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
	vector <GameObject> Board; //Vector for game objects
	//Drawing function for the game objects
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