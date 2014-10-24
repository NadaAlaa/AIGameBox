#include "chess_moves.h"
using namespace std;

class Board{
public:
	Piece* board[8][8];
	pair<int, int>boardCor[8][8];
	PieceColor boardColor[8][8];
	PieceType boardType[8][8];

	Board() {
		for (int i = 2; i < 6; i++)
		for (int j = 0; j < 8; j++)
			board[i][j] = new Piece(i, j);
		float deltaX = 70, deltaY = 70;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				boardCor[i][j].first = 71 + (j*deltaX);
				boardCor[i][j].second = 71 + (i*deltaY);
			}
		}
	}

	void initialize(PieceColor PlayerColor) {
		PieceColor antiPlayerColor = (PlayerColor == Black ? White : Black);

		for (int i = 0; i < 8; i++) {
			board[1][i] = new Pawn((PieceColor)antiPlayerColor, 1, i);
			board[6][i] = new Pawn((PieceColor)PlayerColor, 6, i);
		}

		board[0][0] = new Rook((PieceColor)antiPlayerColor, 0, 0);
		board[0][1] = new Knight((PieceColor)antiPlayerColor, 0, 1);
		board[0][2] = new Bishop((PieceColor)antiPlayerColor, 0, 2);
		board[0][3] = new Queen((PieceColor)antiPlayerColor, 0, 
			
			
			
			
			
			3);
		board[0][4] = new King((PieceColor)antiPlayerColor, 0, 4);
		board[0][5] = new Bishop((PieceColor)antiPlayerColor, 0, 5);
		board[0][6] = new Knight((PieceColor)antiPlayerColor, 0, 6);
		board[0][7] = new Rook((PieceColor)antiPlayerColor, 0, 7);

		board[7][0] = new Rook((PieceColor)PlayerColor, 7, 0);
		board[7][1] = new Knight((PieceColor)PlayerColor, 7, 1);
		board[7][2] = new Bishop((PieceColor)PlayerColor, 7, 2);
		board[7][3] = new Queen((PieceColor)PlayerColor, 7, 3);
		board[7][4] = new King((PieceColor)PlayerColor, 7, 4);
		board[7][5] = new Bishop((PieceColor)PlayerColor, 7, 5);
		board[7][6] = new Knight((PieceColor)PlayerColor, 7, 6);
		board[7][7] = new Rook((PieceColor)PlayerColor, 7, 7);
	}

	void getBoardColor() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				boardColor[i][j] = board[i][j]->pieceColor;
			}
		}
	}

	void getBoardType() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				boardType[i][j] = board[i][j]->pieceType;
			}
		}
	}

	bool isValid(short rowPosition, short columnPosition) {
		return rowPosition >= 0 && rowPosition < 8 && columnPosition >= 0 && columnPosition < 8;
	}

	void recordMove(Moves move) {
		swap(board[move.from.first][move.from.second], board[move.to.first][move.to.second]);
		//delete board[move.from.first][move.from.second];
		board[move.from.first][move.from.second] = new Piece(move.from.first, move.from.second);
		board[move.to.first][move.to.second]->rowPosition = move.to.first;
		board[move.to.first][move.to.second]->columnPosition = move.to.second;
		board[move.to.first][move.to.second]->moves++;
	}

	void reverseMove(Moves move) {
		swap(board[move.from.first][move.from.second], board[move.to.first][move.to.second]);
		board[move.from.first][move.from.second]->rowPosition = move.from.first;
		board[move.from.first][move.from.second]->columnPosition = move.from.second;
		board[move.from.first][move.from.second]->moves = (board[move.from.first][move.from.second]->moves - 1, 0);
		if (move.movetype == Capture) board[move.to.first][move.to.second] = move.piecekilled;
		else board[move.to.first][move.to.second] = new Piece(move.to.first, move.to.second);
	}
};