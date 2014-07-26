#include "chess_computer.h"
#include <Windows.h>
using namespace std;
using namespace sf;
Board game;
bool over;
PieceColor turn = White;
Sprite background;
bool twoPlayers;
Font font;
Sprite modeSelection, colorSelection, endScreen, turnHighlight;
Text wName, bName, scoreB, scoreW;
Sprite promoteBlack, promoteWhite;
Texture checkers;
Texture turnTex;
Texture modeTex, colorTex, endTex;
stack<Moves>history;
Texture whiteSign, blackSign, drawSign;
Texture promotionTexB, promotionTexW;
Texture availableTex, selectedTex, threatTex;
Sprite selectedSquare, availableSquare, inThreat;
Sprite whiteWins, blackWins, gameDraw;
int whiteScore, blackScore;
PieceColor firstPlayerC = White, secondPlayerC = Black;
Computer AI;
Sound piece_move;
SoundBuffer piece_move_buffer;
//Lets the player/s choose the first player color
void chooseColor(RenderWindow &win){
	win.draw(colorSelection);
	win.display();
	int actions = 0;
	while (actions < 1) {
		Event ev;
		while (win.pollEvent(ev)) {
			if (ev.type == Event::MouseButtonReleased && ev.mouseButton.button == Mouse::Left) {
				if (ev.mouseButton.y >= 290 && ev.mouseButton.y <= 449 && ev.mouseButton.x >= 226 && ev.mouseButton.x <= 386) {
					firstPlayerC = White;
					secondPlayerC = Black;
					actions++;
				}
				else if (ev.mouseButton.y >= 290 && ev.mouseButton.y <= 449 && ev.mouseButton.x >= 431 && ev.mouseButton.x <= 573) {
					firstPlayerC = Black;
					secondPlayerC = White;
					actions++;
				}
			}
		}
	}
}
//Lets the user choose his playing mode
void chooseMode(RenderWindow &win) {
	win.draw(modeSelection);
	win.display();
	int actions = 0;
	while (actions < 1) {
		Event ev;
		while (win.pollEvent(ev)) {
			if (ev.type == Event::MouseButtonReleased && ev.mouseButton.button == Mouse::Left) {
				if (ev.mouseButton.y >= 233 && ev.mouseButton.y <= 304 && ev.mouseButton.x >= 228 && ev.mouseButton.x <= 574) {
					twoPlayers = true;
					actions++;
				}
				else if (ev.mouseButton.y >= 333 && ev.mouseButton.y <= 403 && ev.mouseButton.x >= 228 && ev.mouseButton.x <= 574) {
					twoPlayers = false;
					actions++;
				}
			}
		}
	}
}
// The function that sets everything
void globalIn(RenderWindow &win) {
	font.loadFromFile("arial.ttf");
	scoreB.setFont(font), scoreW.setFont(font);
	whiteSign.loadFromFile("graphics\\whiteWins.png"), blackSign.loadFromFile("graphics\\blackWins.png"), drawSign.loadFromFile("graphics\\draw.png");
	whiteWins.setTexture(whiteSign), blackWins.setTexture(blackSign), gameDraw.setTexture(drawSign);
	whiteWins.setPosition(0, 0), blackWins.setPosition(0, 0), gameDraw.setPosition(0, 0);
	modeTex.loadFromFile("graphics\\chess-mode-v2.png"), colorTex.loadFromFile("graphics\\choose-color-v2.png"), endTex.loadFromFile("graphics\\chess-pause.png");
	modeSelection.setTexture(modeTex), colorSelection.setTexture(colorTex), endScreen.setTexture(endTex);
	modeSelection.setPosition(0, 0), colorSelection.setPosition(0, 0), endScreen.setPosition(0, 0);
	availableTex.loadFromFile("graphics\\AvailableDestination.png"), selectedTex.loadFromFile("graphics\\selectedSquare.png"), threatTex.loadFromFile("graphics\\inThreat.png");
	selectedSquare.setTexture(selectedTex), availableSquare.setTexture(availableTex), inThreat.setTexture(threatTex);
	selectedSquare.setOrigin(32, 32), availableSquare.setOrigin(32, 32), inThreat.setOrigin(32, 32);
	promotionTexB.loadFromFile("graphics\\PromotionB.png"), promotionTexW.loadFromFile("graphics\\PromotionW.png");
	promoteBlack.setTexture(promotionTexB), promoteWhite.setTexture(promotionTexW);
	promoteBlack.setPosition(610, 255), promoteWhite.setPosition(610, 255);
	checkers.loadFromFile("graphics\\ChessBoard2.png");
	background.setTexture(checkers);
	scoreW.setColor(Color::Black), scoreB.setColor(Color::Black);
	scoreW.setPosition(650, 54), scoreB.setPosition(650, 94);
	scoreB.setCharacterSize(18); scoreW.setCharacterSize(18);
	turnTex.loadFromFile("graphics\\turnHighlight.png");
	turnHighlight.setTexture(turnTex);
	over = false;
	chooseMode(win);
	chooseColor(win);
	game.initialize(firstPlayerC);

}
// A function that graphically represents the pieces on the board.
void displayPieces(RenderWindow &win) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (game.board[i][j]->pieceType != blank) {
				Sprite cur = game.board[i][j]->icon;
				cur.setPosition(game.boardCor[i][j].first, game.boardCor[i][j].second);
				win.draw(cur);
			}
		}
	}
}
// A function that draws everything that needs updating after adding or removing any graphical object.
//The board, the pieces, and the scores.
void drawAll(RenderWindow &win) {
	win.clear();
	win.draw(background);
	displayPieces(win);
	scoreB.setString("Black: " + to_string(blackScore));
	scoreW.setString("White: " + to_string(whiteScore));
	win.draw(scoreW), win.draw(scoreB);
	turnHighlight.setPosition(635, (turn == White ? 44 : 84));
	win.draw(turnHighlight);
}
//The function that promotes a pawn who's reached the other end
void Promote(Piece piece, int newPieceType) {
	if (newPieceType == queen) game.board[piece.rowPosition][piece.columnPosition] = new Queen(piece.pieceColor, piece.rowPosition, piece.columnPosition);
	else if (newPieceType == knight) game.board[piece.rowPosition][piece.columnPosition] = new Knight(piece.pieceColor, piece.rowPosition, piece.columnPosition);
	else if (newPieceType == rook) game.board[piece.rowPosition][piece.columnPosition] = new Rook(piece.pieceColor, piece.rowPosition, piece.columnPosition);
	else if (newPieceType == bishop) game.board[piece.rowPosition][piece.columnPosition] = new Bishop(piece.pieceColor, piece.rowPosition, piece.columnPosition);
	game.board[piece.rowPosition][piece.columnPosition]->promoted = true;
}
// A function that checks if the chosen move is valid for the chosen piece
bool checkValid(vector<pair<short, short> > available, short x, short y) {
	for (int i = 0; i < available.size(); i++) {
		if (x == available[i].first && y == available[i].second) {
			return true;
		}
	}
	return false;
}
//Checks if the move is a castling request
short isCastling(Moves choice) {
	if (choice.from.first == 7 && choice.from.second == 4) {
		if (choice.to.first == 7 && choice.to.second == 6 && !game.board[7][7]->moves) {
			return 2;
		}
		else if (choice.to.first == 7 && choice.to.second == 2 && !game.board[7][0]->moves) {
			return 1;
		}
	}
	else if (choice.from.first == 0 && choice.from.second == 4) {
		if (choice.to.first == 0 && choice.to.second == 6 && !game.board[0][7]->moves) {
			return -2;
		}
		else if (choice.to.first == 0 && choice.to.second == 2 && !game.board[0][0]->moves) {
			return -1;
		}
	}
	return 0;

}
//Carrying the castling order
pair<Moves, Moves> Castle(short type) {
	Moves king, rook;
	if (type == 2) {
		king.from.first = 7, king.from.second = 4, king.to.first = 7, king.to.second = 6;
		rook.from.first = 7, rook.from.second = 7, rook.to.first = 7, rook.to.second = 5;
	}
	else if (type == 1) {
		king.from.first = 7, king.from.second = 4, king.to.first = 7, king.to.second = 2;
		rook.from.first = 7, rook.from.second = 0, rook.to.first = 7, rook.to.second = 3;
	}
	else if (type == -2) {
		king.from.first = 0, king.from.second = 4, king.to.first = 0, king.to.second = 6;
		rook.from.first = 0, rook.from.second = 7, rook.to.first = 0, rook.to.second = 5;
	}
	else if (type == -1) {
		king.from.first = 0, king.from.second = 4, king.to.first = 0, king.to.second = 2;
		rook.from.first = 0, rook.from.second = 0, rook.to.first = 0, rook.to.second = 3;
	}
	return make_pair(king, rook);
}
//Prints the available moves and the selected square icons
void printAvailableAndSelected(vector<pair<short, short> > available, int i, int j, RenderWindow &win) {
	selectedSquare.setPosition(game.boardCor[i][j].first, game.boardCor[i][j].second);
	win.draw(selectedSquare);
	for (int i = 0; i < available.size(); i++) {
		availableSquare.setPosition(game.boardCor[available[i].first][available[i].second].first, game.boardCor[available[i].first][available[i].second].second);
		win.draw(availableSquare);
	}
}
//undos the last move, or two, depending on the game mode.
void undo() {
	if (history.empty()) return;
	if (history.top().movetype = Promotion) return;
	game.reverseMove(history.top());
	history.pop();
	if (!twoPlayers) {
		game.reverseMove(history.top());
		history.pop();
	}
	if (twoPlayers) {
		turn = (turn == White ? Black : White);
	}
}
//Player input funcions, gets the piece, prints the available moves, gets the destination, checks if it is valid, and if everything checks out, it carrys the moving order.
void PlayerMove(RenderWindow &win) {
	int actions = 0;
	Moves choice;
	choice.movetype = RegularMove;
	while (actions < 2) {
		game.getBoardType();
		game.getBoardColor();
		Event ev;
		while (win.pollEvent(ev)) {
			if (ev.type == Event::MouseButtonReleased && ev.mouseButton.button == Mouse::Left) {
				short i = (ev.mouseButton.y - 40) / 70;
				short j = (ev.mouseButton.x - 40) / 70;
				if (i>7 || i<0 || j>7 || j<0) continue;
				if (actions == 0 && game.board[i][j]->pieceColor == turn && game.board[i][j]->AvailableMoves(game.boardColor, game.boardType).size() > 0) {
					choice.from.first = i;
					choice.from.second = j;
					choice.piecemoved = game.board[i][j];
					actions++;
					drawAll(win);
					printAvailableAndSelected(game.board[i][j]->AvailableMoves(game.boardColor, game.boardType), i, j,win);
					win.display();
				}
				else if (actions == 1 && game.board[i][j]->pieceColor != turn  && checkValid(game.board[choice.from.first][choice.from.second]->AvailableMoves(game.boardColor, game.boardType), i, j)) {
					choice.to.first = i;
					choice.to.second = j;
					actions++;
				}
			}
			else if (ev.type == Event::MouseButtonReleased && ev.mouseButton.button == Mouse::Right) {
				actions = max(actions - 1, 0);
				drawAll(win);
				win.display();
			}
			else if (ev.type == Event::KeyPressed && ev.key.control && ev.key.code == Keyboard::Z) {
				actions = max(actions - 1, 0);
				undo();
				drawAll(win);
				win.display();
			}
			else if (ev.type == Event::Closed) {
				win.close();
			}
		}
		if (actions == 2) {
			break;
		}
	}
	short castleFlag = isCastling(choice);
	if (castleFlag) {
		pair<Moves, Moves> temp = Castle(castleFlag);
		game.recordMove(temp.first);
		game.recordMove(temp.second);
		history.push(temp.first);
		history.push(temp.second);
	}
	else {
		if (game.board[choice.from.first][choice.from.second]->pieceColor != game.board[choice.to.first][choice.to.second]->pieceColor && game.board[choice.to.first][choice.to.second]->pieceColor != blank) {
			choice.movetype = Capture;
			choice.piecekilled = game.board[choice.to.first][choice.to.second];
		}
		else choice.movetype = RegularMove;
		game.recordMove(choice);
		history.push(choice);
	}
}

bool isOver(PieceColor turn) {
	int res = 0;
	game.getBoardColor();
	game.getBoardType();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (game.board[i][j]->pieceColor == turn) {
				if (game.board[i][j]->AvailableMoves(game.boardColor, game.boardType).size()) return false;
			}
		}
	}
	return true;
}
GameState checkMate(RenderWindow &win) {
	game.getBoardColor();
	game.getBoardType();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (game.board[i][j]->pieceType == king) {
				if (game.board[i][j]->inCheck(game.boardColor, game.boardType)) {
					inThreat.setPosition(game.boardCor[i][j].first, game.boardCor[i][j].second);
					drawAll(win);
					win.draw(inThreat);
					win.display();
					if (isOver(game.board[i][j]->pieceColor)) {
						return (game.board[i][j]->pieceColor == White ? BlackWin : WhiteWin);
					}
				}
			}
		}
	}
	return Running;
}

void calcScore() {
	whiteScore = blackScore = 39;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (game.board[i][j]->pieceType == king || game.board[i][j]->promoted) continue;
			if (game.board[i][j]->pieceColor == Black) {
				whiteScore -= (game.board[i][j]->value / 100);
			}
			else {
				blackScore -= (game.board[i][j]->value / 100);
			}
		}
	}
}

bool checkDraw() {
	int pieces = 0;
	game.getBoardColor();
	game.getBoardType();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (game.board[i][j]->pieceType != blank) pieces++;
			if (game.board[i][j]->pieceType == king) {
				if (!game.board[i][j]->inCheck(game.boardColor, game.boardType)) {
					if (isOver(game.board[i][j]->pieceColor)) {
						return true;
					}
				}
			}
		}
	}
	if (pieces == 2) {
		return true;
	}
	return false;
}
void checkPromotion(RenderWindow &win) {
	int row = (turn == White ? 7 : 0);
	pair<short, short>pawnplace;
	bool promotion = false;
	for (int i = 0; i < 8; i++) {
		if (game.board[row][i]->pieceType == pawn) {
			promotion = true;
			pawnplace.first = row;
			pawnplace.second = i;
			break;
		}
	}
	PieceColor curColor = game.board[pawnplace.first][pawnplace.second]->pieceColor;
	PieceType choice;
	if (promotion) {
		if (game.board[pawnplace.first][pawnplace.second]->pieceColor == secondPlayerC && !twoPlayers) {
			PieceType choices[] = { knight, rook, queen, bishop };
			choice = choices[rand() % 3];
		}
		else {
			win.draw((curColor == Black ? promoteBlack : promoteWhite));
			win.display();
			Event ev;
			int actions = 0;
			while (actions < 1) {
				while (win.pollEvent(ev)) {
					cout << ev.mouseButton.x << " " << ev.mouseButton.y << endl;
					if (ev.type == Event::Closed) {
						win.close();
					}
					else if (ev.type == Event::MouseButtonReleased && ev.mouseButton.button == Mouse::Left) {
						if (ev.mouseButton.x >= 617 && ev.mouseButton.x <= 653 && ev.mouseButton.y >= 355 && ev.mouseButton.y <= 389) {
							choice = queen;
							actions++;
							break;
						}
						else if (ev.mouseButton.x >= 657 && ev.mouseButton.x <= 697 && ev.mouseButton.y >= 355 && ev.mouseButton.y <= 389) {
							choice = rook;
							actions++;
							break;
						}
						else if (ev.mouseButton.x >= 699 && ev.mouseButton.x <= 740 && ev.mouseButton.y >= 355 && ev.mouseButton.y <= 389) {
							choice = bishop;
							actions++;
							break;
						}
						else if (ev.mouseButton.x >= 741 && ev.mouseButton.x <= 780 && ev.mouseButton.y >= 355 && ev.mouseButton.y <= 389) {
							choice = knight;
							actions++;
							break;
						}
					}
				}
			}
		}
		Moves promotionMove;
		promotionMove.piecepromoted = game.board[pawnplace.first][pawnplace.second];
		promotionMove.promotedToPieceType = choice;
		promotionMove.movetype = Promotion;
		history.push(promotionMove);
		Promote(*game.board[pawnplace.first][pawnplace.second], choice);
		drawAll(win);
		win.display();
	}
}
void restart(RenderWindow &win) {
	globalIn(win);
	turn = White;
}
void endMenu(RenderWindow &win){
	win.draw(endScreen);
	win.display();
	int actions = 0;
	while (actions < 1) {
		Event ev;
		while (win.pollEvent(ev)) {
			if (ev.type == Event::KeyPressed) {
				if (ev.key.code == Keyboard::Space) {
					restart(win);
					actions++;
				}
				else if (ev.key.code == Keyboard::Escape) {
					//return to main menu
					actions++;
				}
			}
		}
	}
}
void chess_main(RenderWindow &win) {
	Clock clock;
	win.create(VideoMode(800, 640), "Chess");
	globalIn(win);
	while (win.isOpen()) {
		GameState curGameState = checkMate(win);
		checkPromotion(win);
		Event ev;
		while (win.pollEvent(ev)) {
			if (ev.type == Event::Closed || ev.key.code == Keyboard::Escape) {
				win.close();
			}
		}
		if (over) {
			clock.restart();
			while (clock.getElapsedTime().asSeconds() <= 2);
			//wait a second or two
			endMenu(win);
		}
		calcScore();
		drawAll(win);
		win.display();
		if (checkDraw() && !over) {
			win.draw(gameDraw);
			win.display();
			over = true;
		}
		if (curGameState == WhiteWin&&!over) {
			win.draw(whiteWins);
			win.display();
			over = true;
		}
		else if (curGameState == BlackWin && !over) {
			win.draw(blackWins);
			win.display();
			over = true;
		}
		if (turn == White && curGameState == Running) {
			piece_move.play();
			if (firstPlayerC == Black &&!twoPlayers) {
				history.push(AI.makemove(game, turn));
				game.recordMove(history.top());
			}
			else {
				PlayerMove(win);
			}
		}
		else if (turn == Black && curGameState == Running) {
			piece_move.play();
			if (firstPlayerC == White &&!twoPlayers) {
				history.push(AI.makemove(game, turn));
				game.recordMove(history.top());
			}
			else {
				PlayerMove(win);
			}
		}
		turn = (turn == White ? Black : White);
	}
}