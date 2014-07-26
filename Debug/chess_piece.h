#include "chess_tools.h"
using namespace std;
using namespace sf;

class Piece {
protected:
	short movingDirection;
public:
	PieceType pieceType;
	PieceColor pieceColor;
	short rowPosition, columnPosition;
	short value;
	short moves;
	vector<pair<short, short> > availableMoves;
	Texture mainTex;
	Sprite icon;
	bool promoted = false;

	Piece() : pieceType((PieceType)blank), pieceColor((PieceColor)Blank) {}

	Piece(short RP, short CP) : pieceType((PieceType)blank), pieceColor((PieceColor)Blank), rowPosition(RP), columnPosition(CP), moves(0) {}

	Piece(PieceType PT, PieceColor PC, short RP, short CP) : pieceType(PT), pieceColor(PC), rowPosition(RP), columnPosition(CP), moves(0) {
		RP == 0 || RP == 1 ? movingDirection = 1 : movingDirection = -1;
	}

	virtual vector<pair<short, short> > AvailableMoves(PieceColor boardC[8][8], PieceType boardT[8][8]) { vector<pair<short, short> > x; x.push_back(make_pair(0, 0)); return x; }
	virtual bool inCheck(PieceColor boardC[8][8], PieceType boardT[8][8]) { return true; }
	virtual void Promote(Piece* piece, PieceType newPieceType) {};
	virtual short getHeuristicValue(short i, short j) { return 0; }

};

class King : public Piece {
private:
	vector<vector<short> > heuristicValue;

public:

	void initializeHeuristicValues() {
		heuristicValue.insert(heuristicValue.end(), {
			{ -30, -40, -40, -50, -50, -40, -40, -30 },
			{ -30, -40, -40, -50, -50, -40, -40, -30 },
			{ -30, -40, -40, -50, -50, -40, -40, -30 },
			{ -30, -40, -40, -50, -50, -40, -40, -30 },
			{ -20, -30, -30, -40, -40, -30, -30, -20 },
			{ -10, -20, -20, -20, -20, -20, -20, -10 },
			{ 20, 20, 0, 0, 0, 0, 20, 20 },
			{ 20, 30, 10, 0, 0, 10, 30, 20 }
		});
	}

	King(PieceColor PC, short RP, short CP) : Piece((PieceType)king, PC, RP, CP) {
		initializeHeuristicValues();
		value = 20000;
		if (PC == Black) {
			mainTex.loadFromFile("graphics\\KingB.png");
		}
		else {
			mainTex.loadFromFile("graphics\\KingW.png");
		}
		icon.setTexture(mainTex);
		icon.setOrigin(24, 24);
	}

	virtual bool inCheck(PieceColor boardC[8][8], PieceType boardT[8][8]) {
		short upwards(rowPosition), downwards(rowPosition), right(columnPosition), left(columnPosition);
		bool b_up = true, b_down = true, b_right = true, b_left = true, upright = true, upleft = true, downright = true, downleft = true;

		while ((b_up || b_down || b_right || b_left || upright || upleft || downright || downleft) && (upwards < 8 || downwards >= 0 || right < 8 || left >= 0)) {
			upwards++;
			downwards--;
			right++;
			left--;

			if (upright && upwards < 8 && right < 8) {
				if (boardC[upwards][right] == pieceColor) upright = 0;
				else if (boardC[upwards][right] != Blank) {
					if (boardT[upwards][right] == queen || boardT[upwards][right] == bishop || (boardT[upwards][right] == king && abs(upwards - rowPosition) == 1 && abs(right - columnPosition) == 1) || (movingDirection == -1 && boardT[upwards][right] == pawn && abs(upwards - rowPosition) == 1 && abs(right - columnPosition) == 1))
						return 1;
					upright = 0;
				}
			}

			if (upleft && upwards < 8 && left >= 0) {
				if (boardC[upwards][left] == pieceColor) upleft = 0;
				else if (boardC[upwards][left] != Blank) {
					if (boardT[upwards][left] == queen || boardT[upwards][left] == bishop || (boardT[upwards][left] == king && abs(upwards - rowPosition) == 1 && abs(left - columnPosition) == 1) || (movingDirection == -1 && boardT[upwards][left] == pawn && abs(upwards - rowPosition) == 1 && abs(left - columnPosition) == 1))
						return 1;
					upleft = 0;
				}
			}

			if (downright && downwards >= 0 && right < 8) {
				if (boardC[downwards][right] == pieceColor) downright = 0;
				else if (boardC[downwards][right] != Blank) {
					if (boardT[downwards][right] == queen || boardT[downwards][right] == bishop || (boardT[downwards][right] == king && abs(downwards - rowPosition) == 1 && abs(right - columnPosition) == 1) || (movingDirection == 1 && boardT[downwards][right] == pawn && abs(downwards - rowPosition) == 1 && abs(right - columnPosition) == 1))
						return 1;
					downright = 0;
				}
			}

			if (downleft && downwards >= 0 && left >= 0) {
				if (boardC[downwards][left] == pieceColor) downleft = 0;
				else if (boardC[downwards][left] != Blank) {
					if (boardT[downwards][left] == queen || boardT[downwards][left] == bishop || (boardT[downwards][left] == king && abs(downwards - rowPosition) == 1 && abs(left - columnPosition) == 1) || (movingDirection == 1 && boardT[downwards][left] == pawn && abs(downwards - rowPosition) == 1 && abs(left - columnPosition) == 1))
						return 1;
					downleft = 0;
				}
			}

			if (b_up && upwards < 8) {
				if (boardC[upwards][columnPosition] == pieceColor) b_up = 0;
				else if (boardC[upwards][columnPosition] != Blank) {
					if (boardT[upwards][columnPosition] == queen || boardT[upwards][columnPosition] == rook || (boardT[upwards][columnPosition] == king && abs(upwards - rowPosition) == 1))
						return 1;
					b_up = 0;
				}
			}

			if (b_down && downwards >= 0) {
				if (boardC[downwards][columnPosition] == pieceColor) b_down = 0;
				else if (boardC[downwards][columnPosition] != Blank) {
					if (boardT[downwards][columnPosition] == queen || boardT[downwards][columnPosition] == rook || (boardT[downwards][columnPosition] == king && abs(downwards - rowPosition) == 1))
						return 1;
					b_down = 0;
				}
			}

			if (b_right && right < 8) {
				if (boardC[rowPosition][right] == pieceColor) b_right = 0;
				else if (boardC[rowPosition][right] != Blank) {
					if (boardT[rowPosition][right] == queen || boardT[rowPosition][right] == rook || (boardT[rowPosition][right] == king && abs(right - columnPosition) == 1))
						return 1;
					b_right = 0;
				}
			}

			if (b_left && left >= 0) {
				if (boardC[rowPosition][left] == pieceColor) b_left = 0;
				else if (boardC[rowPosition][left] != Blank) {
					if (boardT[rowPosition][left] == queen || boardT[rowPosition][left] == rook || (boardT[rowPosition][left] == king && abs(left - columnPosition) == 1))
						return 1;
					b_left = 0;
				}
			}
		}
		short horizontal[] = { 1, 1, -1, -1, 2, 2, -2, -2 }, vertical[] = { 2, -2, 2, -2, 1, -1, 1, -1 };
		for (int i = 0; i < 8; i++) {
			if (rowPosition + vertical[i] >= 0 && rowPosition + vertical[i] < 8 && columnPosition + horizontal[i] >= 0 && columnPosition + horizontal[i] < 8 && boardC[rowPosition + vertical[i]][columnPosition + horizontal[i]] != pieceColor && boardT[rowPosition + vertical[i]][columnPosition + horizontal[i]] == knight)
				return 1;
		}
		return 0;
	}


	virtual vector<pair<short, short> > AvailableMoves(PieceColor boardC[8][8], PieceType boardT[8][8]) {
		vector<pair<short, short> > availables;

		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if ((i != 0 || j != 0) && rowPosition + i >= 0 && rowPosition + i < 8 && columnPosition + j >= 0 && columnPosition + j < 8 && boardC[rowPosition + i][columnPosition + j] != pieceColor) {
					PieceColor temporaryC = boardC[rowPosition + i][columnPosition + j];
					boardC[rowPosition + i][columnPosition + j] = boardC[rowPosition][columnPosition];
					boardC[rowPosition][columnPosition] = Blank;
					PieceType temporaryT = boardT[rowPosition + i][columnPosition + j];
					boardT[rowPosition + i][columnPosition + j] = boardT[rowPosition][columnPosition];
					boardT[rowPosition][columnPosition] = blank;
					rowPosition += i; columnPosition += j;

					if (!inCheck(boardC, boardT)) availables.push_back(make_pair(rowPosition, columnPosition));

					rowPosition -= i; columnPosition -= j;
					boardC[rowPosition][columnPosition] = boardC[rowPosition + i][columnPosition + j];
					boardC[rowPosition + i][columnPosition + j] = temporaryC;
					boardT[rowPosition][columnPosition] = boardT[rowPosition + i][columnPosition + j];
					boardT[rowPosition + i][columnPosition + j] = temporaryT;
				}
			}
		}

		if (moves == 0 && !inCheck(boardC, boardT)) {
			short right(columnPosition + 1), left(columnPosition - 1);
			bool rightCastling = true, leftCastling = true;
			for (; right < 7; right++) {
				if (boardT[rowPosition][right] != blank) {
					rightCastling = false;
					break;
				}
			}
			if (boardT[rowPosition][7] != rook || boardC[rowPosition][7] != pieceColor) rightCastling = false;

			for (; left > 0; left--) {
				if (boardT[rowPosition][left] != blank) {
					leftCastling = false;
					break;
				}
			}
			if (boardT[rowPosition][0] != rook || boardC[rowPosition][0] != pieceColor) leftCastling = false;

			if (rightCastling) {
				swap(boardC[rowPosition][columnPosition], boardC[rowPosition][columnPosition + 2]);
				swap(boardT[rowPosition][columnPosition], boardT[rowPosition][columnPosition + 2]);
				swap(boardC[rowPosition][columnPosition + 1], boardC[rowPosition][7]);
				swap(boardT[rowPosition][columnPosition + 1], boardT[rowPosition][7]);
				columnPosition += 2;

				if (!inCheck(boardC, boardT)) availables.push_back(make_pair(rowPosition, columnPosition));

				columnPosition -= 2;
				swap(boardC[rowPosition][columnPosition], boardC[rowPosition][columnPosition + 2]);
				swap(boardT[rowPosition][columnPosition], boardT[rowPosition][columnPosition + 2]);
				swap(boardC[rowPosition][columnPosition + 1], boardC[rowPosition][7]);
				swap(boardT[rowPosition][columnPosition + 1], boardT[rowPosition][7]);
			}

			if (leftCastling) {
				swap(boardC[rowPosition][columnPosition], boardC[rowPosition][columnPosition - 2]);
				swap(boardT[rowPosition][columnPosition], boardT[rowPosition][columnPosition - 2]);
				swap(boardC[rowPosition][columnPosition - 1], boardC[rowPosition][0]);
				swap(boardT[rowPosition][columnPosition - 1], boardT[rowPosition][0]);
				columnPosition -= 2;

				if (!inCheck(boardC, boardT)) availables.push_back(make_pair(rowPosition, columnPosition));

				columnPosition += 2;
				swap(boardC[rowPosition][columnPosition], boardC[rowPosition][columnPosition - 2]);
				swap(boardT[rowPosition][columnPosition], boardT[rowPosition][columnPosition - 2]);
				swap(boardC[rowPosition][columnPosition - 1], boardC[rowPosition][0]);
				swap(boardT[rowPosition][columnPosition - 1], boardT[rowPosition][0]);
			}
		}
		return availables;
	}

	virtual short getHeuristicValue(short i, short j) {
		return heuristicValue[i][j];
	}
};

King checkMateKing(Blank, -1, -1);

class Bishop : public Piece {
private:
	vector<vector<short> > heuristicValue;

public:

	void initializeHeuristicValues() {
		heuristicValue.insert(heuristicValue.end(), {
			{ -20, -10, -10, -10, -10, -10, -10, -20 },
			{ -10, 0, 0, 0, 0, 0, 0, -10 },
			{ -10, 0, 5, 10, 10, 5, 0, -10 },
			{ -10, 5, 5, 10, 10, 5, 5, -10 },
			{ -10, 0, 10, 10, 10, 10, 0, -10 },
			{ -10, 10, 10, 10, 10, 10, 10, -10 },
			{ -10, 5, 0, 0, 0, 0, 5, -10 },
			{ -20, -10, -40, -10, -10, -40, -10, -20 }
		});
	}

	Bishop(PieceColor PC, short RP, short CP) : Piece((PieceType)bishop, PC, RP, CP) {
		initializeHeuristicValues();
		value = 330;
		if (PC == Black) {
			mainTex.loadFromFile("graphics\\BishopB.png");
		}
		else {
			mainTex.loadFromFile("graphics\\BishopW.png");
		}
		icon.setTexture(mainTex);
		icon.setOrigin(24, 24);
	}

	virtual vector<pair<short, short> > AvailableMoves(PieceColor boardC[8][8], PieceType boardT[8][8]) {
		vector<pair<short, short> > availables;
		short upwards(rowPosition), downwards(rowPosition), right(columnPosition), left(columnPosition);

		bool upright = true, upleft = true, downright = true, downleft = true;
		while ((upwards < 8 || downwards >= 0) && (right < 8 || left >= 0)) {
			upwards++;
			downwards--;
			right++;
			left--;

			if (upright && upwards < 8 && right < 8) {
				if (boardC[upwards][right] == pieceColor) upright = 0;
				else if (boardC[upwards][right] == Blank) availables.push_back(make_pair(upwards, right));
				else {
					availables.push_back(make_pair(upwards, right));
					upright = 0;
				}
			}

			if (upleft && upwards < 8 && left >= 0) {
				if (boardC[upwards][left] == pieceColor) upleft = 0;
				else if (boardC[upwards][left] == Blank) availables.push_back(make_pair(upwards, left));
				else {
					availables.push_back(make_pair(upwards, left));
					upleft = 0;
				}
			}

			if (downright && downwards >= 0 && right < 8) {
				if (boardC[downwards][right] == pieceColor) downright = 0;
				else if (boardC[downwards][right] == Blank) availables.push_back(make_pair(downwards, right));
				else {
					availables.push_back(make_pair(downwards, right));
					downright = 0;
				}
			}

			if (downleft && downwards >= 0 && left >= 0) {
				if (boardC[downwards][left] == pieceColor) downleft = 0;
				else if (boardC[downwards][left] == Blank) availables.push_back(make_pair(downwards, left));
				else {
					availables.push_back(make_pair(downwards, left));
					downleft = 0;
				}
			}
		}

		checkMateKing.pieceColor = pieceColor;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (boardT[i][j] == king && boardC[i][j] == pieceColor) {
					checkMateKing.rowPosition = i;
					checkMateKing.columnPosition = j;
					break;
				}
			}
		}

		for (int i = 0; i < availables.size(); i++) {
			bool inCheck = 0;
			PieceColor temporaryC = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = boardC[rowPosition][columnPosition];
			boardC[rowPosition][columnPosition] = Blank;
			PieceType temporaryT = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = boardT[rowPosition][columnPosition];
			boardT[rowPosition][columnPosition] = blank;

			if (checkMateKing.inCheck(boardC, boardT)) inCheck = 1;

			boardC[rowPosition][columnPosition] = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = temporaryC;
			boardT[rowPosition][columnPosition] = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = temporaryT;

			if (inCheck) availables.erase(availables.begin() + i), i--;
		}

		return availables;
	}

	virtual short getHeuristicValue(short i, short j) {
		return heuristicValue[i][j];
	}

};

class Knight : public Piece {
private:
	vector<vector<short> > heuristicValue;
public:


	void initializeHeuristicValues() {
		heuristicValue.insert(heuristicValue.end(), {
			{ -50, -40, -30, -30, -30, -30, -40, -50 },
			{ -40, -20, 0, 0, 0, 0, -20, -40 },
			{ -30, 0, 10, 15, 15, 10, 0, -30 },
			{ -30, 5, 15, 20, 20, 15, 5, -30 },
			{ -30, 0, 15, 20, 20, 15, 0, -30 },
			{ -30, 5, 10, 15, 15, 10, 5, -30 },
			{ -40, -20, 0, 5, 5, 0, -20, -40 },
			{ -50, -40, -20, -30, -30, -20, -40, -50 }
		});
	}

	Knight(PieceColor PC, short RP, short CP) : Piece((PieceType)knight, PC, RP, CP) {
		initializeHeuristicValues();
		value = 320;
		if (PC == Black) {
			mainTex.loadFromFile("graphics\\KnightB.png");
		}
		else {
			mainTex.loadFromFile("graphics\\KnightW.png");
		}
		icon.setTexture(mainTex);
		icon.setOrigin(24, 24);
	}

	virtual vector<pair<short, short> > AvailableMoves(PieceColor boardC[8][8], PieceType boardT[8][8]) {
		vector<pair<short, short> > availables;

		short horizontal[] = { 1, 1, -1, -1, 2, 2, -2, -2 }, vertical[] = { 2, -2, 2, -2, 1, -1, 1, -1 };

		for (int i = 0; i < 8; i++) {
			if (rowPosition + vertical[i] >= 0 && rowPosition + vertical[i] < 8 && columnPosition + horizontal[i] >= 0 && columnPosition + horizontal[i] < 8 && boardC[rowPosition + vertical[i]][columnPosition + horizontal[i]] != pieceColor)
				availables.push_back(make_pair(rowPosition + vertical[i], columnPosition + horizontal[i]));
		}

		checkMateKing.pieceColor = pieceColor;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (boardT[i][j] == king && boardC[i][j] == pieceColor) {
					checkMateKing.rowPosition = i;
					checkMateKing.columnPosition = j;
					break;
				}
			}
		}

		for (int i = 0; i < availables.size(); i++) {
			bool inCheck = 0;
			PieceColor temporaryC = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = boardC[rowPosition][columnPosition];
			boardC[rowPosition][columnPosition] = Blank;
			PieceType temporaryT = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = boardT[rowPosition][columnPosition];
			boardT[rowPosition][columnPosition] = blank;

			if (checkMateKing.inCheck(boardC, boardT)) inCheck = 1;

			boardC[rowPosition][columnPosition] = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = temporaryC;
			boardT[rowPosition][columnPosition] = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = temporaryT;

			if (inCheck) availables.erase(availables.begin() + i), i--;
		}

		return availables;
	}

	virtual short getHeuristicValue(short i, short j) {
		return heuristicValue[i][j];
	}

};

class Rook : public Piece {
private:
	vector<vector<short> > heuristicValue;
public:


	void initializeHeuristicValues() {
		heuristicValue.insert(heuristicValue.end(), {
			{ 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 5, 10, 10, 10, 10, 10, 10, 5 },
			{ -5, 0, 0, 0, 0, 0, 0, -5 },
			{ -5, 0, 0, 0, 0, 0, 0, -5 },
			{ -5, 0, 0, 0, 0, 0, 0, -5 },
			{ -5, 0, 0, 0, 0, 0, 0, -5 },
			{ -5, 0, 0, 0, 0, 0, 0, -5 },
			{ 0, 0, 0, 5, 5, 0, 0, 0 }
		});
	}

	Rook(PieceColor PC, short RP, short CP) : Piece((PieceType)rook, PC, RP, CP) {
		initializeHeuristicValues();
		value = 500;
		if (PC == Black) {
			mainTex.loadFromFile("graphics\\RookB.png");
		}
		else {
			mainTex.loadFromFile("graphics\\RookW.png");
		}
		icon.setTexture(mainTex);
		icon.setOrigin(24, 24);
	}

	virtual vector<pair<short, short> > AvailableMoves(PieceColor boardC[8][8], PieceType boardT[8][8]) {
		vector<pair<short, short> > availables;

		short upwards(rowPosition), downwards(rowPosition), right(columnPosition), left(columnPosition);
		bool b_up = true, b_down = true, b_right = true, b_left = true;
		while ((b_up || b_down || b_right || b_left) && (upwards < 8 || downwards >= 0 || right < 8 || left >= 0)) {
			upwards++;
			downwards--;
			right++;
			left--;

			if (b_up && upwards < 8) {
				if (boardC[upwards][columnPosition] == pieceColor) b_up = 0;
				else if (boardC[upwards][columnPosition] == Blank) availables.push_back(make_pair(upwards, columnPosition));
				else {
					availables.push_back(make_pair(upwards, columnPosition));
					b_up = 0;
				}
			}

			if (b_down && downwards >= 0) {
				if (boardC[downwards][columnPosition] == pieceColor) b_down = 0;
				else if (boardC[downwards][columnPosition] == Blank) availables.push_back(make_pair(downwards, columnPosition));
				else {
					availables.push_back(make_pair(downwards, columnPosition));
					b_down = 0;
				}
			}

			if (b_right && right < 8) {
				if (boardC[rowPosition][right] == pieceColor) b_right = 0;
				else if (boardC[rowPosition][right] == Blank) availables.push_back(make_pair(rowPosition, right));
				else {
					availables.push_back(make_pair(rowPosition, right));
					b_right = 0;
				}
			}

			if (b_left && left >= 0) {
				if (boardC[rowPosition][left] == pieceColor) b_left = 0;
				else if (boardC[rowPosition][left] == Blank) availables.push_back(make_pair(rowPosition, left));
				else {
					availables.push_back(make_pair(rowPosition, left));
					b_left = 0;
				}
			}
		}

		checkMateKing.pieceColor = pieceColor;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (boardT[i][j] == king && boardC[i][j] == pieceColor) {
					checkMateKing.rowPosition = i;
					checkMateKing.columnPosition = j;
					break;
				}
			}
		}

		for (int i = 0; i < availables.size(); i++) {
			bool inCheck = 0;
			PieceColor temporaryC = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = boardC[rowPosition][columnPosition];
			boardC[rowPosition][columnPosition] = Blank;
			PieceType temporaryT = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = boardT[rowPosition][columnPosition];
			boardT[rowPosition][columnPosition] = blank;

			if (checkMateKing.inCheck(boardC, boardT)) inCheck = 1;

			boardC[rowPosition][columnPosition] = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = temporaryC;
			boardT[rowPosition][columnPosition] = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = temporaryT;

			if (inCheck) availables.erase(availables.begin() + i), i--;
		}

		return availables;
	}

	virtual short getHeuristicValue(short i, short j) {
		return heuristicValue[i][j];
	}

};

class Queen : public Piece {
private:
	vector<vector<short> > heuristicValue;
public:


	virtual void initializeHeuristicValues() {
		heuristicValue.insert(heuristicValue.end(), {
			{ -20, -10, -10, -5, -5, -10, -10, -20 },
			{ -10, 0, 0, 0, 0, 0, 0, -10 },
			{ -10, 0, 5, 5, 5, 5, 0, -10 },
			{ -5, 0, 5, 5, 5, 5, 0, -5 },
			{ 0, 0, 5, 5, 5, 5, 0, -5 },
			{ -10, 5, 5, 5, 5, 5, 0, -10 },
			{ -10, 0, 5, 0, 0, 0, 0, -10 },
			{ -20, -10, -10, -5, -5, -10, -10, -20 }
		});
	}

	Queen(PieceColor PC, short RP, short CP) : Piece((PieceType)queen, PC, RP, CP) {
		value = 900;
		initializeHeuristicValues();

		if (PC == Black) {
			mainTex.loadFromFile("graphics\\QueenB.png");
		}
		else {
			mainTex.loadFromFile("graphics\\QueenW.png");
		}
		icon.setTexture(mainTex);
		icon.setOrigin(24, 24);
	}

	virtual vector<pair<short, short> > AvailableMoves(PieceColor boardC[8][8], PieceType boardT[8][8]) {
		vector<pair<short, short> > availables;
		short upwards(rowPosition), downwards(rowPosition), right(columnPosition), left(columnPosition);
		bool b_up = true, b_down = true, b_right = true, b_left = true, upright = true, upleft = true, downright = true, downleft = true;

		while ((b_up || b_down || b_right || b_left || upright || upleft || downright || downleft) && (upwards < 8 || downwards >= 0 || right < 8 || left >= 0)) {
			upwards++;
			downwards--;
			right++;
			left--;

			if (upright && upwards < 8 && right < 8) {
				if (boardC[upwards][right] == pieceColor) upright = 0;
				else if (boardC[upwards][right] == Blank) availables.push_back(make_pair(upwards, right));
				else {
					availables.push_back(make_pair(upwards, right));
					upright = 0;
				}
			}

			if (upleft && upwards < 8 && left >= 0) {
				if (boardC[upwards][left] == pieceColor) upleft = 0;
				else if (boardC[upwards][left] == Blank) availables.push_back(make_pair(upwards, left));
				else {
					availables.push_back(make_pair(upwards, left));
					upleft = 0;
				}
			}

			if (downright && downwards >= 0 && right < 8) {
				if (boardC[downwards][right] == pieceColor) downright = 0;
				else if (boardC[downwards][right] == Blank) availables.push_back(make_pair(downwards, right));
				else {
					availables.push_back(make_pair(downwards, right));
					downright = 0;
				}
			}

			if (downleft && downwards >= 0 && left >= 0) {
				if (boardC[downwards][left] == pieceColor) downleft = 0;
				else if (boardC[downwards][left] == Blank) availables.push_back(make_pair(downwards, left));
				else {
					availables.push_back(make_pair(downwards, left));
					downleft = 0;
				}
			}

			if (b_up && upwards < 8) {
				if (boardC[upwards][columnPosition] == pieceColor) b_up = 0;
				else if (boardC[upwards][columnPosition] == Blank) availables.push_back(make_pair(upwards, columnPosition));
				else {
					availables.push_back(make_pair(upwards, columnPosition));
					b_up = 0;
				}
			}

			if (b_down && downwards >= 0) {
				if (boardC[downwards][columnPosition] == pieceColor) b_down = 0;
				else if (boardC[downwards][columnPosition] == Blank) availables.push_back(make_pair(downwards, columnPosition));
				else {
					availables.push_back(make_pair(downwards, columnPosition));
					b_down = 0;
				}
			}

			if (b_right && right < 8) {
				if (boardC[rowPosition][right] == pieceColor) b_right = 0;
				else if (boardC[rowPosition][right] == Blank) availables.push_back(make_pair(rowPosition, right));
				else {
					availables.push_back(make_pair(rowPosition, right));
					b_right = 0;
				}
			}

			if (b_left && left >= 0) {
				if (boardC[rowPosition][left] == pieceColor) b_left = 0;
				else if (boardC[rowPosition][left] == Blank) availables.push_back(make_pair(rowPosition, left));
				else {
					availables.push_back(make_pair(rowPosition, left));
					b_left = 0;
				}
			}
		}

		checkMateKing.pieceColor = pieceColor;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (boardT[i][j] == king && boardC[i][j] == pieceColor) {
					checkMateKing.rowPosition = i;
					checkMateKing.columnPosition = j;
					break;
				}
			}
		}

		for (int i = 0; i < availables.size(); i++) {
			bool inCheck = 0;
			PieceColor temporaryC = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = boardC[rowPosition][columnPosition];
			boardC[rowPosition][columnPosition] = Blank;
			PieceType temporaryT = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = boardT[rowPosition][columnPosition];
			boardT[rowPosition][columnPosition] = blank;

			if (checkMateKing.inCheck(boardC, boardT)) inCheck = 1;

			boardC[rowPosition][columnPosition] = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = temporaryC;
			boardT[rowPosition][columnPosition] = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = temporaryT;

			if (inCheck) availables.erase(availables.begin() + i), i--;
		}

		return availables;
	}

	virtual short getHeuristicValue(short i, short j) {
		return heuristicValue[i][j];
	}

};

class Pawn : public Piece {
private:
	vector<vector<short> > heuristicValue;
public:


	void initializeHeuristicValues() {
		heuristicValue.insert(heuristicValue.end(), {
			{ 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 50, 50, 50, 50, 50, 50, 50, 50 },
			{ 10, 10, 20, 30, 30, 20, 10, 10 },
			{ 5, 5, 10, 27, 27, 10, 5, 5 },
			{ 0, 0, 0, 25, 25, 0, 0, 0 },
			{ 5, -5, -10, 0, 0, -10, -5, 5 },
			{ 5, 10, 10, -25, -25, 10, 10, 5 },
			{ 0, 0, 0, 0, 0, 0, 0, 0 }
		});
	}

	Pawn(PieceColor PC, short RP, short CP) : Piece((PieceType)pawn, PC, RP, CP) {
		value = 100;
		initializeHeuristicValues();
		if (PC == Black) {
			mainTex.loadFromFile("graphics\\PawnB.png");
		}
		else {
			mainTex.loadFromFile("graphics\\PawnW.png");
		}
		icon.setTexture(mainTex);
		icon.setOrigin(24, 24);
	}

	virtual vector<pair<short, short> > AvailableMoves(PieceColor boardC[8][8], PieceType boardT[8][8]) {
		vector<pair<short, short> > availables;

		if (rowPosition + movingDirection >= 0 && rowPosition + movingDirection < 8 && boardC[rowPosition + movingDirection][columnPosition] == Blank)
			availables.push_back(make_pair(rowPosition + movingDirection, columnPosition));

		if (moves == 0 && boardC[rowPosition + movingDirection][columnPosition] == Blank && boardC[rowPosition + (2 * movingDirection)][columnPosition] == Blank)
			availables.push_back(make_pair(rowPosition + (2 * movingDirection), columnPosition));

		if (rowPosition + movingDirection >= 0 && rowPosition + movingDirection < 8 && columnPosition + 1 >= 0 && columnPosition + 1 < 8 && boardC[rowPosition + movingDirection][columnPosition + 1] != Blank && boardC[rowPosition + movingDirection][columnPosition + 1] != pieceColor)
			availables.push_back(make_pair(rowPosition + movingDirection, columnPosition + 1));

		if (rowPosition + movingDirection >= 0 && rowPosition + movingDirection < 8 && columnPosition - 1 >= 0 && columnPosition - 1 < 8 && boardC[rowPosition + movingDirection][columnPosition - 1] != Blank && boardC[rowPosition + movingDirection][columnPosition - 1] != pieceColor)
			availables.push_back(make_pair(rowPosition + movingDirection, columnPosition - 1));

		checkMateKing.pieceColor = pieceColor;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (boardT[i][j] == king && boardC[i][j] == pieceColor) {
					checkMateKing.rowPosition = i;
					checkMateKing.columnPosition = j;
					break;
				}
			}
		}

		for (int i = 0; i < availables.size(); i++) {
			bool inCheck = 0;
			PieceColor temporaryC = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = boardC[rowPosition][columnPosition];
			boardC[rowPosition][columnPosition] = Blank;
			PieceType temporaryT = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = boardT[rowPosition][columnPosition];
			boardT[rowPosition][columnPosition] = blank;

			if (checkMateKing.inCheck(boardC, boardT)) inCheck = 1;

			boardC[rowPosition][columnPosition] = boardC[availables[i].first][availables[i].second];
			boardC[availables[i].first][availables[i].second] = temporaryC;
			boardT[rowPosition][columnPosition] = boardT[availables[i].first][availables[i].second];
			boardT[availables[i].first][availables[i].second] = temporaryT;

			if (inCheck) availables.erase(availables.begin() + i), i--;
		}

		return availables;
	}


	virtual short getHeuristicValue(short i, short j) {
		return heuristicValue[i][j];
	}

	virtual void Promote(Piece* piece, PieceType newPieceType) {
		if (newPieceType == queen) piece = new Queen(piece->pieceColor, piece->rowPosition, piece->columnPosition);
		else if (newPieceType == knight) piece = new Knight(piece->pieceColor, piece->rowPosition, piece->columnPosition);
		else if (newPieceType == rook) piece = new Rook(piece->pieceColor, piece->rowPosition, piece->columnPosition);
		else if (newPieceType == bishop) piece = new Bishop(piece->pieceColor, piece->rowPosition, piece->columnPosition);
	}
};