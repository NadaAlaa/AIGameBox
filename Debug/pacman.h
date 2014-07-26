#include "chess.h"
using namespace std;
using namespace sf;
int start_x, start_y, score = 0, cheese, high_score = 0, level_ = 1, lives = 3;
bool quit_ = false;
Fruit dot, fruit;
Energizer energizer;
Pacman pacman;
Sound pacman_eatghost, pacman_eatfruit, pacman_chomp, pacman_death, pacman_beginning;
SoundBuffer pacman_eatghost_buffer, pacman_eatfruit_buffer, pacman_chomp_buffer, pacman_death_buffer, pacman_beginning_buffer;
Sprite live[3];
Texture scared, dead, obs, energy;
//function to load images into textures and called every time the game restarts or moves to a new level
bool Begin(Game& game)
{
	cheese = 0;
	//loading ghost images
	game.ghost[0].image.loadFromFile("pacman\\monster 1.png");
	game.ghost[0].shape.setTexture(game.ghost[0].image);
	game.ghost[1].image.loadFromFile("pacman\\monster 2.png");
	game.ghost[1].shape.setTexture(game.ghost[1].image);
	game.ghost[2].image.loadFromFile("pacman\\monster 3.png");
	game.ghost[2].shape.setTexture(game.ghost[2].image);
	game.ghost[3].image.loadFromFile("pacman\\monster 4.png");
	game.ghost[3].shape.setTexture(game.ghost[3].image);
	//......................
	game.ghost[0].direction = game.ghost[2].direction = game.ghost[3].direction = 'N'; //setting a startup direction for each towards North
	game.ghost[0].bestMoveFactor = game.ghost[1].bestMoveFactor = game.ghost[2].bestMoveFactor = game.ghost[3].bestMoveFactor = 6;  //setting the best move factor for ghosts
	dot.edible = fruit.edible = energizer.edible = 1; //edible feature is given to dots, fruits and energy at the beginning of the level
	int difficulty; //the range which the ghost is able to see the player within representes the difficulty of the level
	char is_new_level;
	cin >> is_new_level;
	if (is_new_level == '0') return false; //game completed
	cin >> difficulty;
	game.ghost[0].range = game.ghost[1].range = game.ghost[2].range = game.ghost[3].range = difficulty;
	//reading the input from an input file
	for (int i = 0; i < 20; i++)
	for (int j = 0; j < 19; j++)
	{
		char c;
		cin >> c;
		if (c == '.') //the '.' represents the wall
		{
			Wall Obstacle;
			Obstacle.edible = 0;
			Obstacle.x = j * side;
			Obstacle.y = i * side;
			Obstacle.shape.setTexture(obs);
			for (int k = 0; k < side; k++)
			for (int l = 0; l < side; l++)
				wall[k + i * side][l + j * side] = 1;
			game.Board.push_back(Obstacle);
		}
		else if (c == '1') //the '1' represents pacman's position
		{
			start_x = pacman.x = j*side;
			start_y = pacman.y = i*side;
			pacman.direction = 'W';
		}
		else if (c == 'M') //character 'M' represents the first ghost
		{
			game.ghost[0].start_x = game.ghost[0].x = j*side; //saving the startup position
			game.ghost[0].start_y = game.ghost[0].y = i*side;
			game.ghost[0].house = game.ghost[0];
			game.ghost[0].score = 200;
		}
		else if (c == 'N') //character 'N' represents the second ghost
		{
			game.ghost[1].start_x = game.ghost[1].x = j*side;
			game.ghost[1].start_y = game.ghost[1].y = i*side;
			game.ghost[1].house = game.ghost[1];
			game.ghost[1].score = 200;
		}
		else if (c == 'P') //character 'P' represents the third ghost
		{
			game.ghost[2].start_x = game.ghost[2].x = j*side;
			game.ghost[2].start_y = game.ghost[2].y = i*side;
			game.ghost[2].house = game.ghost[2];
			game.ghost[2].score = 200;
		}
		else if (c == 'Q') //character 'Q' represents the fourth ghost
		{
			game.ghost[3].start_x = game.ghost[3].x = j*side;
			game.ghost[3].start_y = game.ghost[3].y = i*side;
			game.ghost[3].house = game.ghost[3];
			game.ghost[3].score = 200;
		}
		else if (c == '/') //character '/' represents the edible dots
		{
			dot.x = side*j;
			dot.y = side*i;
			dot.score = 10;
			dot.type = "cheese";
			game.Board.push_back(dot);
			cheese++; //counting the number of the dots/cheese to check for the next level
		}
		else if (c == 'f') //character 'f' represents the fruit
		{
			fruit.x = side*j;
			fruit.y = side*i;
			fruit.score = 100;
			fruit.type = "fruit";
			game.Board.push_back(fruit);
		}
		else if (c == 'E') //character 'E' represents the energizer
		{
			energizer.x = side*j;
			energizer.y = side*i;
			energizer.type = "energy";
			game.Board.push_back(energizer);
		}
	}
	pacman.clock.restart();
	return true;
}
//called every new level
bool level(RenderWindow &window)
{
	Clock clock, moving_clock, moving_clock_monster; //clock for the startup sound, moving_clock to make pacman's speed constant and faster than the ghosts' speed
	Text text;
	Font font;
	Game game;
	bool move_ = false, is_first_level = true;
	font.loadFromFile("pacman\\SNAP____.ttf"); //font of the text
	memset(wall, 0, sizeof(wall));
	if (!Begin(game)) return false; //if begin return false, the game is completed
	game.player = pacman;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			int y = game.player.y + H, x = game.player.x + W;
			//moving the player depends on the arrow keys
			if (event.key.code == Keyboard::Up&& !wall[(y - 1) % H][(x + error) % W] && !wall[(y - 1) % H][(x + side - 1 - error) % W])
			{
				if (game.player.direction != 'N')
					game.player.shape.setTexture(game.player.image1);
				game.player.direction = 'N';
				move_ = true;
				break;
			}
			else if (event.key.code == Keyboard::Down&&!wall[(y + side) % H][(x + error) % W] && !wall[(y + side) % H][(x + side - 1 - error) % W])
			{
				if (game.player.direction != 'S')
					game.player.shape.setTexture(game.player.image2);
				game.player.direction = 'S';
				move_ = true;
				break;
			}
			else if (event.key.code == Keyboard::Right&&!wall[(y + error) % H][(x + side) % W] && !wall[(y + side - 1 - error) % H][(x + side) % W])
			{
				if (game.player.direction != 'E')
					game.player.shape.setTexture(game.player.image);
				game.player.direction = 'E';
				move_ = true;
				break;
			}
			else if (event.key.code == Keyboard::Left&&!wall[(y + error) % H][(x - 1) % W] && !wall[(y + side - 1 - error) % H][(x - 1) % W])
			{
				if (game.player.direction != 'W')
					game.player.shape.setTexture(game.player.image3);
				game.player.direction = 'W';
				move_ = true;
				break;
			}
			//setting "P" key for the "Pause" option
			else if (event.key.code == Keyboard::P)
			{
				move_ = false;
				break;
			}
			//setting "Esc" key for quiting the game
			else if (event.key.code == Keyboard::Escape)
			{
				quit_ = true;
				return 0;
			}
		}
		if (move_ && moving_clock.getElapsedTime().asMilliseconds() >= 3) //pacman moves ahead once every 3 milliseconds, constant speed
		{
			game.player.move();
			moving_clock.restart();
		}
		if (move_ && moving_clock_monster.getElapsedTime().asMilliseconds() >= 5) //ghosts move ahead once every 5 milliseconds, so they would be slower
		{
			moving_clock_monster.restart();
			for (int i = 0; i < 4; i++)
			{
				//checks for the collisions arises between pacman and the ghosts
				if (game.player.ISCollide(game.ghost[i]))
				{
					//in case the ghost is scared but not dead, collision happens
					if (game.ghost[i].is_scared&&!game.ghost[i].is_dead)
					{
						pacman_eatghost.play();
						if (game.ghost[i].is_dead == false) score += game.ghost[i].score; //score increases
						game.ghost[i].is_dead = 1;
						game.ghost[i].edible = false;
						game.ghost[i].shape.setTexture(dead);
					}
					//in case pacman is not energized, he dies and loses a life
					else if (!game.ghost[i].is_dead)
					{
						pacman_death.play();
						//in case he runs out of lives, the game restarts from the first level
						if (!lives)
						{
							level_ = 1;
							lives = 3;
							score = 0;
							freopen("pacman\\in_pacman.txt", "r", stdin);
							return 1;
						}
						lives--;
						//each returns back to its original position
						game.player.x = start_x, game.player.y = start_y;
						game.player.shape.setTexture(game.player.image3);
						for (int j = 0; j < 4; j++)
						{
							game.ghost[j].x = game.ghost[j].start_x;
							game.ghost[j].y = game.ghost[j].start_y;
							game.ghost[j].shape.setTexture(game.ghost[j].image);
							game.ghost[j].is_scared = game.ghost[j].is_dead = false;
						}
						move_ = false;
					}
				}
				game.ghost[i].move(game.player);
			}
		}
		for (int i = 0; i < game.Board.size(); i++)
		{
			//check for collisions arises between pacman and the edible game objects
			if (game.Board[i].edible && game.player.ISCollide(game.Board[i]))
			{
				game.Board[i].shape.setScale(0, 0); //setting scale of the object to zero
				game.Board[i].edible = 0;
				score += game.Board[i].score;
				if (game.Board[i].type == "cheese")
				{
					pacman_chomp.play();
					cheese--;
				}
				else if (game.Board[i].type == "fruit") pacman_eatfruit.play();
				//check if the game object is energy, changes the ghosts' images
				else if (game.Board[i].type == "energy")
				for (int i = 0; i < 4; i++)
				{
					game.ghost[i].is_scared = true;
					game.ghost[i].is_dead = false;
					game.ghost[i].shape.setTexture(scared);
					game.ghost[i].scaryClock.restart();
				}
			}
		}
		//After 14 seconds of being energized, the ghosts return back to their original state and chase after the player
		for (int i = 0; i < 4; i++)
		if (game.ghost[i].scaryClock.getElapsedTime().asSeconds() >= 14 && game.ghost[i].is_scared && !game.ghost[i].is_dead)
		{
			game.ghost[i].is_scared = false;
			game.ghost[i].is_dead = false;
			game.ghost[i].scaryClock.restart();
			game.ghost[i].shape.setTexture(game.ghost[i].image);
		}
		//if number of dots/cheese reacher zero, the level is completed
		if (cheese == 0)
		{
			level_++;
			return 1;
		}
		//setting the score, the high score and the level style, and position
		stringstream ss_score, ss_highscore, ss_level;
		window.clear();
		game.Draw(window);
		text.setColor(Color::Yellow);
		text.setCharacterSize(25);
		text.setFont(font);
		high_score = max(score, high_score);
		ss_score << score; ss_highscore << high_score; ss_level << level_;
		text.setPosition(Vector2f(W, 0));
		text.setString("Score: \n" + ss_score.str());
		window.draw(text);
		text.setPosition(Vector2f(W, side));
		text.setString("\n\nHigh Score: \n" + ss_highscore.str());
		window.draw(text);
		text.setPosition(Vector2f(W, side * 2));
		text.setString("\n\n\n\nLevel: \n" + ss_level.str());
		window.draw(text);
		//.................
		for (int i = 0; i < lives; i++)
			window.draw(live[i]);
		window.display();
		//play the beginning sound for 4 seconds at the beginning of each level
		if (is_first_level){
			pacman_beginning.play();
			while (clock.getElapsedTime().asSeconds() <= 4);
			is_first_level = false;
		}
		//.......
	}
	return 0;
}
void pacman_main(RenderWindow &window)
{
	//setting the window's position and size
	window.create(VideoMode(W + 5 * side, H), "Pacman");
	window.setPosition(Vector2i(400, 0));
	//loading images
	scared.loadFromFile("pacman\\scared.png");
	dead.loadFromFile("pacman\\Pacman-ghost-eyes.png");
	dot.image.loadFromFile("pacman\\dot.png");
	dot.shape.setTexture(dot.image);
	fruit.image.loadFromFile("pacman\\f.png");
	fruit.shape.setTexture(fruit.image);
	obs.loadFromFile("pacman\\rect.png");
	energy.loadFromFile("pacman\\energy.png");
	pacman.image.loadFromFile("pacman\\Pacman.png");
	pacman.image1.loadFromFile("pacman\\Pacman-up.png");
	pacman.image2.loadFromFile("pacman\\Pacman-down.png");
	pacman.image3.loadFromFile("pacman\\Pacman-left.png");
	pacman.image4.loadFromFile("pacman\\Pacman_.png");
	live[0].setTexture(pacman.image);
	live[2] = live[1] = live[0];
	live[0].setPosition(W, H - 50);
	live[1].setPosition(W + 50, H - 50);
	live[2].setPosition(W + 100, H - 50);
	pacman.shape.setTexture(pacman.image3);
	energizer.shape.setTexture(energy);
	//.........................
	Sprite game_completed;
	Texture game_completed_image;
	game_completed_image.loadFromFile("pacman\\pacman_game_completed.png");
	game_completed.setTexture(game_completed_image);
	while (true)
	{
		srand(time(0));
		freopen("pacman\\in_pacman.txt", "r", stdin);
		while (level(window));
		if (quit_) return;
		Event event;
		bool is_option_choosen = false;
		while (window.isOpen()){
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)	window.close();
				if (event.key.code == Keyboard::Space) //"Space" key to continue
				{
				 	is_option_choosen = true;
					break;
				}
				else if (event.key.code == Keyboard::Escape || event.type == Event::Closed) //"Esc" key to exit
					return;
			}
			window.clear();
			window.draw(game_completed);
			window.display();
			if (is_option_choosen) break;
		}
		score = 0;
		level_ = 1;
	}
}