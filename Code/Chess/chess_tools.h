#include "pacman_.h"
enum PieceType {
	queen,
	king,
	knight,
	bishop,
	rook,
	pawn,
	enPassant,
	blank
};

enum PieceColor {
	Black,
	White,
	Blank
};

enum MoveType {
	RegularMove,
	Capture,
	Castling,
	EnPassant,
	Promotion
};
enum GameState {
	WhiteWin,
	BlackWin,
	Draw,
	Stalemate,
	Running
};