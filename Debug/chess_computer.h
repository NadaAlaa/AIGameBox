#include "chess_board.h"

using namespace std;

class Computer {
private:
	PieceColor maximize;
	//Evaluation function used to evaluate the current state of the board.
	int evaluate(Board curBoard) {
		int sum = 0, material = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (curBoard.board[i][j]->pieceColor == maximize) {
					//Getting the strategic value of the location of the current piece.
					sum += curBoard.board[i][j]->getHeuristicValue(i, j);
					//Getting the strategic value of the piece itself.
					material += curBoard.board[i][j]->value;
				}
				else {
					sum -= curBoard.board[i][j]->getHeuristicValue(i, j);
					material -= curBoard.board[i][j]->value;
				}
			}
		}
		return material + sum;
	}
	//Quiescence Search to fix the horizon effect.
	int Quiescence(Board curBoard, PieceColor turn, int alpha, int beta) {
		//standing pat is used so that we don't evaluate branches that are useless, to lessen the search space.
		int standingPat = evaluate(curBoard);
		if (standingPat >= beta) {
			return beta;
		}
		if (alpha < standingPat) {
			alpha = standingPat;
		}
		int score;
		PieceColor enemy = (turn == Black ? White : Black);
		curBoard.getBoardColor();
		curBoard.getBoardType();
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				PieceColor pC = curBoard.board[i][j]->pieceColor, antiPC = (curBoard.board[i][j]->pieceColor == Black ? White : Black);
				if (pC == turn) {
					vector<pair<short, short> > PM = curBoard.board[i][j]->AvailableMoves(curBoard.boardColor, curBoard.boardType);
					for (int k = 0; k < PM.size(); k++) {
						if (curBoard.board[PM[k].first][PM[k].second]->pieceColor != pC && curBoard.board[PM[k].first][PM[k].second]->pieceType != blank) {
							// move the piece
							Piece* toPiece;
							toPiece = curBoard.board[PM[k].first][PM[k].second];
							curBoard.board[PM[k].first][PM[k].second] = new Piece(PM[k].first, PM[k].second);
							swap(curBoard.board[PM[k].first][PM[k].second], curBoard.board[i][j]);
							curBoard.board[PM[k].first][PM[k].second]->rowPosition = PM[k].first;
							curBoard.board[PM[k].first][PM[k].second]->columnPosition = PM[k].second;
							curBoard.board[PM[k].first][PM[k].second]->moves++;

							//-----------------------------------------------------
							score = -Quiescence(curBoard, enemy, -alpha, -beta);
							//----------------------------------------------------

							// reverse move the piece
							swap(curBoard.board[PM[k].first][PM[k].second], curBoard.board[i][j]);
							curBoard.board[i][j]->rowPosition = i;
							curBoard.board[i][j]->columnPosition = j;
							curBoard.board[i][j]->moves--;
							curBoard.board[PM[k].first][PM[k].second] = toPiece;

							if (score >= beta) {
								return beta;
							}
							if (score > alpha) {
								alpha = score;
							}
						}
					}
				}
			}
		}
		return alpha;
	}

	int minimax(Board curBoard, int depth, int alpha, int beta, PieceColor turn) {
		if (depth <= 0) {
			//return Quiescence(curBoard,turn,alpha,beta);
			return evaluate(curBoard);
		}
		curBoard.getBoardColor();
		curBoard.getBoardType();
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				PieceColor pC = curBoard.board[i][j]->pieceColor, antiPC = (curBoard.board[i][j]->pieceColor == Black ? White : Black);
				if (pC == turn) {
					vector<pair<short, short> > PM = curBoard.board[i][j]->AvailableMoves(curBoard.boardColor, curBoard.boardType);
					for (int k = 0; k < PM.size(); k++) {
						if (curBoard.board[PM[k].first][PM[k].second]->pieceColor != pC) {
							// move the piece
							Piece* toPiece;
							toPiece = curBoard.board[PM[k].first][PM[k].second];
							curBoard.board[PM[k].first][PM[k].second] = new Piece(PM[k].first, PM[k].second);
							swap(curBoard.board[PM[k].first][PM[k].second], curBoard.board[i][j]);
							curBoard.board[PM[k].first][PM[k].second]->rowPosition = PM[k].first;
							curBoard.board[PM[k].first][PM[k].second]->columnPosition = PM[k].second;
							curBoard.board[PM[k].first][PM[k].second]->moves++;

							//-----------------------------------------------------
							//We search for the maximum score for the white player.
							if (turn == White) alpha = max(alpha, minimax(curBoard, depth - 1, alpha, beta, antiPC));
							//-----------------------------------------------------
							//We search for the minimum score for the black player.
							else beta = min(beta, minimax(curBoard, depth - 1, alpha, beta, antiPC));
							//-----------------------------------------------------
							// reverse move the piece
							swap(curBoard.board[PM[k].first][PM[k].second], curBoard.board[i][j]);
							curBoard.board[i][j]->rowPosition = i;
							curBoard.board[i][j]->columnPosition = j;
							curBoard.board[i][j]->moves--;
							curBoard.board[PM[k].first][PM[k].second] = toPiece;
							//Alpha beta failing cut off
							if (alpha >= beta) {
								return turn == White ? beta : alpha;
							}
						}
					}
				}
			}
		}
		return turn == White ? alpha : beta;
	}

public:
	//The main search method, looking for the best move
	Moves makemove(Board curBoard, PieceColor turn) {
		maximize = turn;
		int alpha = -1e9, beta = 1e9;
		int tempbestscore = -1e9, bestscore = -1e9;
		Moves bestmove;
		curBoard.getBoardColor();
		curBoard.getBoardType();
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				PieceColor pC = curBoard.board[i][j]->pieceColor, antiPC = (curBoard.board[i][j]->pieceColor == Black ? White : Black);
				if (pC == turn) {
					vector<pair<short, short> > PM = curBoard.board[i][j]->AvailableMoves(curBoard.boardColor, curBoard.boardType);
					for (int k = 0; k < PM.size(); k++) {
						if (curBoard.board[PM[k].first][PM[k].second]->pieceColor != pC) {
							// move the piece
							Piece* toPiece;
							toPiece = curBoard.board[PM[k].first][PM[k].second];
							curBoard.board[PM[k].first][PM[k].second] = new Piece(PM[k].first, PM[k].second);
							swap(curBoard.board[PM[k].first][PM[k].second], curBoard.board[i][j]);
							curBoard.board[PM[k].first][PM[k].second]->rowPosition = PM[k].first;
							curBoard.board[PM[k].first][PM[k].second]->columnPosition = PM[k].second;
							curBoard.board[PM[k].first][PM[k].second]->moves++;

							tempbestscore = max(tempbestscore, minimax(curBoard, 3, alpha, beta, (turn == White ? Black : White)));

							// reverse the movement
							swap(curBoard.board[PM[k].first][PM[k].second], curBoard.board[i][j]);
							curBoard.board[i][j]->rowPosition = i;
							curBoard.board[i][j]->columnPosition = j;
							curBoard.board[i][j]->moves--;
							curBoard.board[PM[k].first][PM[k].second] = toPiece;
							if (bestscore < tempbestscore) {
								bestscore = tempbestscore;
								bestmove.from.first = i;
								bestmove.from.second = j;
								bestmove.to.first = PM[k].first;
								bestmove.to.second = PM[k].second;
							}
						}
					}
				}
			}
		}
		// Setting the move type for the game history
		if (curBoard.board[bestmove.to.first][bestmove.to.second]->pieceColor != turn && curBoard.board[bestmove.to.first][bestmove.to.second]->pieceColor != Blank) {
			bestmove.piecekilled = curBoard.board[bestmove.to.first][bestmove.to.second];
			bestmove.movetype = Capture;
		}
		return bestmove;
	}
};