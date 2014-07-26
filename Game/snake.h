#include "snake_.h"
using namespace sf;
using namespace std;
int high_score_ = 0, score_ = 0, level__ = 1;
bool quit = false, arcade_mode = false;
//RenderWindow window(VideoMode(W_, H_ + 50), "Snake");
Snake snake;
//sounds
SoundBuffer eatBuffer;
SoundBuffer dieBuffer;
SoundBuffer levelBuffer;
Sound eatSound;
Sound dieSound;
Sound levelSound;
//function begin, called everytime the game restarts, or new levels
bool Begin(Game_ &game)
{
	memset(wall_, 0, sizeof wall_);
	char is_new_level;
	cin >> is_new_level;
	if (is_new_level == '0') return false;
	//loading images
	game.fruit.image.loadFromFile("snake\\apple.png");
	game.fruit.shape.setTexture(game.fruit.image);
	obstacle_image.loadFromFile("snake\\wall.png");
	snake.image.loadFromFile("snake\\bodypart.png");
	game.fruit.x = rand() % W_; game.fruit.y = rand() % H_; 	//resetting the position of the fruit randomly
	game.direction = 'E'; //default direction of the snake is East
	eatSound.setBuffer(eatBuffer);
	dieSound.setBuffer(dieBuffer);
	levelSound.setBuffer(levelBuffer);
	game.fruit.edible = 1; //giving the fruit an edible feature
	//Reading the input from an input file
	for (int height = 0; height < H_ / side_; height++)
	for (int width = 0; width < W_ / side_; width++)
	{
		char symbol;
		cin >> symbol;
		if (symbol == '/')
		{
			Wall_ obstacle;
			obstacle.edible = false;
			obstacle.x = width * side_;
			obstacle.y = height * side_;
			obstacle.shape.setTexture(obstacle_image);
			wall_[height][width] = 1;
			game.Board.push_back(obstacle);
		}
		else if (symbol == '0')
		{
			Snake BodyPart;
			BodyPart.edible = false;
			BodyPart.x = width * side_;
			BodyPart.y = height * side_;
			BodyPart.shape.setTexture(snake.image);
			game.Player.push_front(BodyPart);
		}
	}
	//................
	int tmp_x = rand() % (W_ / side_), tmp_y = rand() % (H_ / side_);
	//handling the position of the fruit so that it won't be positioned on a wall or the snake's tale
	while (wall_[tmp_y][tmp_x] == 1)
		tmp_x = rand() % (W_ / side_), tmp_y = rand() % (H_ / side_);
	game.fruit.x = tmp_x*side_;
	game.fruit.y = tmp_y*side_;
	return true;
}
bool level_snake(RenderWindow &window)
{
	Clock clock; //clock to make the speed of the snake constant
	Text text; Font font;
	font.loadFromFile("snake\\JOKERMAN.ttf");
	text.setColor(Color::Blue); text.setCharacterSize(35); text.setFont(font); //Setting score_ position and style
	Game_ game;
	memset(wall_, 0, sizeof wall_);
	if (!Begin(game)) return false; //if not a new level, return from this function
	bool move_ = false; //boolean variable for the "pause" feature
	while (window.isOpen())
	{
		stringstream ss_score_, ss_high_score_, ss_level;
		Event event;
		high_score_ = max(score_, high_score_); //setting the high score_
		ss_score_ << score_;
		if (arcade_mode)
			ss_level << level__;
		else
			ss_level << "--";
		ss_high_score_ << high_score_;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
			//changing the direction of the snake depending on the arrow keys
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Up && game.direction != 'S'){
					game.direction = 'N'; game.move(); clock.restart(); move_ = 1; break;
				}
				else if (event.key.code == Keyboard::Down && game.direction != 'N'){
					game.direction = 'S'; game.move(); clock.restart(); move_ = 1; break;
				}
				else if (event.key.code == Keyboard::Right && game.direction != 'W'){
					game.direction = 'E'; game.move(); clock.restart(); move_ = 1; break;
				}
				else if (event.key.code == Keyboard::Left && game.direction != 'E'){
					game.direction = 'W'; game.move(); clock.restart(); move_ = 1; break;
				}
				else if (event.key.code == Keyboard::P){
					move_ = 0; break;
				}
				else if (event.key.code == Keyboard::Escape){
					quit = true;
					return 0;
				}
			}
			//..............................
		}
		window.clear(sf::Color::White); //white background
		if (move_&&clock.getElapsedTime().asMilliseconds() >= 100) //if the pause key "P" not pressed, the snake moves once per 100 milliSeconds
		{
			game.move();
			clock.restart();
		}
		//checking for collisions with the fruit
		if (game.Player[0].ISCollide(game.fruit))
		{
			int tmp_x = rand() % (W_ / side_), tmp_y = rand() % (H_ / side_);
			while (wall_[tmp_y][tmp_x] == 1)
				tmp_x = rand() % (W_ / side_), tmp_y = rand() % (H_ / side_);
			game.fruit.x = tmp_x*side_;
			game.fruit.y = tmp_y*side_;
			score_ += 10; //increasing score_ by the value of the fruit
			//if the score_ is mod 200 equals zero, go to the next level
			if (arcade_mode){
				if (score_ % 200 == 0)
				{
					levelSound.play();
					clock.restart();
					while (clock.getElapsedTime().asMilliseconds() < 1000);
					while (window.pollEvent(event));
					return 1;
				}
				else eatSound.play();
			}
			//................
			else
				eatSound.play();
			game.Player.push_back(game.Player.back());
		}
		//checking for collision with a wall_
		if (wall_[game.Player[0].y / side_][game.Player[0].x / side_])
		{
			dieSound.play();
			clock.restart();
			while (clock.getElapsedTime().asMilliseconds()<2000);
			while (window.pollEvent(event));
			game.Player.clear();
			game.Board.clear();
			freopen("snake\\in_snake.txt", "r", stdin);
			score_ = 0;
			level__ = 1;
			move_ = 0;
			Begin(game);
		}
		//......................
		game.Draw(window);
		//displaying the score_, the level and the high score_
		text.setString("Score: " + ss_score_.str());
		text.setPosition(Vector2f(5, H_ + 5));
		window.draw(text);
		text.setString("Level: " + ss_level.str());
		text.setPosition(Vector2f(270, H_ + 5));
		window.draw(text);
		text.setString("High Score: " + ss_high_score_.str());
		text.setPosition(Vector2f(480, H_ + 5));
		window.draw(text);
		window.display();
	}
	return 0;
}
void snake_main(RenderWindow &window)
{
	window.create(VideoMode(W_, H_ + 50), "Snake");
	Sprite game_completed, mode_decision;
	Texture game_completed_image, mode_decision_image;
	bool is_option_choosen = false;
	arcade_mode = false;
	mode_decision_image.loadFromFile("snake\\mode_decision.png");
	mode_decision.setTexture(mode_decision_image);
	game_completed_image.loadFromFile("snake\\snake_game_completed.png");
	game_completed.setTexture(game_completed_image);
	while (window.isOpen()){
		Event event;
		while (window.pollEvent(event)){
			if (event.mouseButton.x >= 232 && event.mouseButton.x <= 567 && event.mouseButton.y >= 219 && event.mouseButton.y <= 277 && event.mouseButton.button == Mouse::Left){
				is_option_choosen = true;
				break;
			}
			else if (event.mouseButton.x >= 232 && event.mouseButton.x <= 567 && event.mouseButton.y >= 318 && event.mouseButton.y <= 381 && event.mouseButton.button == Mouse::Left){
				is_option_choosen = true;
				arcade_mode = true;
				break;
			}
			else if (event.key.code == Keyboard::Escape || event.type == Event::Closed) return;
		}
		window.clear();
		window.draw(mode_decision);
		window.display();
		if (is_option_choosen) break;
	}
	while (true){
		srand(time(0));
		freopen("snake\\in_snake.txt", "r", stdin);
		while (level_snake(window))level__++;
		if (quit) return;
		Event event;
		is_option_choosen = false;
		while (window.isOpen()){
			while (window.pollEvent(event)){
				if (event.type == sf::Event::Closed) window.close();
				if (event.key.code == Keyboard::Space){
					is_option_choosen = true;
					break;
				}
				else if (event.key.code == Keyboard::Escape || event.type == Event::Closed){
					return;
				}
			}
			window.clear();
			window.draw(game_completed);
			window.display();
			if (is_option_choosen) break;
		}
		score_ = 0;
		level__ = 1;
	}
}