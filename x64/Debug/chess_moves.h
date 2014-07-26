#include "chess_piece.h"
class Moves{
public:
	MoveType movetype;
	pair<short, short> to;
	pair<short, short> from;
	Piece* piecemoved;
	Piece* piecepromoted; // copy of the promoted pawn with all its data
	PieceType promotedToPieceType; // the new PieceType of the pawn
	Piece* piecekilled; // captured piece
	Moves() {}
	Moves(MoveType MT, pair<short, short>F, pair<short, short>T, Piece* PM) : movetype(MT), from(F), to(T), piecemoved(PM) {} //RegularMove
	Moves(MoveType MT, pair<short, short>POS, Piece* PP, PieceType NPT) : movetype(MT), to(POS), piecepromoted(PP), promotedToPieceType(NPT) { } //Promotion
	Moves(MoveType MT, pair<short, short>F, pair<short, short>T, Piece* PM, Piece* PK) : movetype(MT), from(F), to(T), piecemoved(PM), piecekilled(PK) {} //Capture
};