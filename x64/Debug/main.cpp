 #include "snake.h"
using namespace sf;
using namespace std;
int main(){
	//Loading sound files and buffers once in the main
	pacman_eatghost_buffer.loadFromFile("pacman\\pacman_eatghost.wav");
	pacman_eatghost.setBuffer(pacman_eatghost_buffer);
	pacman_eatfruit_buffer.loadFromFile("pacman\\pacman_eatfruit.wav");
	pacman_eatfruit.setBuffer(pacman_eatfruit_buffer);
	pacman_chomp_buffer.loadFromFile("pacman\\pacman_chomp.wav");
	pacman_chomp.setBuffer(pacman_chomp_buffer);
	pacman_death_buffer.loadFromFile("pacman\\pacman_death.wav");
	pacman_death.setBuffer(pacman_death_buffer);
	pacman_beginning_buffer.loadFromFile("pacman\\pacman_beginning.wav");
	pacman_beginning.setBuffer(pacman_beginning_buffer);
	eatBuffer.loadFromFile("snake\\eat.wav");
	dieBuffer.loadFromFile("snake\\die.wav");
	levelBuffer.loadFromFile("snake\\level2.wav");
	piece_move_buffer.loadFromFile("graphics\\piece_move.wav");
	piece_move.setVolume(100);
	piece_move.setBuffer(piece_move_buffer);
	RenderWindow window(VideoMode(800, 600), "Arcade El 7ag Bor3y");
	Sprite main_menu;
	Texture main_menu_image;
	main_menu_image.loadFromFile("main_menu.png");
	main_menu.setTexture(main_menu_image);
	while (window.isOpen()){
		Event event;
		while (window.pollEvent(event)){
			if (event.type == Event::Closed) window.close();
			if (event.type == event.MouseButtonPressed){
				if (event.mouseButton.y > 310 && event.mouseButton.x < 391 && event.mouseButton.button == Mouse::Left){
					freopen("snake_highscore.txt", "r", stdin);
					int saved_high_score;
					cin >> saved_high_score;
		     		high_score_ = saved_high_score;
					snake_main(window);
					window.create(VideoMode(800, 600), "Arcade El 7ag Bor3y");
					freopen("snake_highscore.txt", "w", stdout); cout << high_score_ << endl;
				}
				else if (event.mouseButton.y < 301 && event.mouseButton.button == Mouse::Left){
					freopen("pacman_highscore.txt", "r", stdin);
					int saved_high_score;
					cin >> saved_high_score;
					high_score = saved_high_score;
					pacman_main(window);
					window.create(VideoMode(800, 600), "Arcade El 7ag Bor3y");
					freopen("pacman_highscore.txt", "w", stdout); cout << high_score << endl;
				}
				else if (event.mouseButton.y > 310 && event.mouseButton.x > 400 && event.mouseButton.button == Mouse::Left){
					chess_main(window);
					window.create(VideoMode(800, 600), "Arcade El 7ag Bor3y");
				}
				score = score_ = quit = quit_ = 0;
				level_ = level__ = 1;
				whiteScore = blackScore = 0;
			}
		}
		window.clear();
		window.draw(main_menu);
		window.display();
	}
}