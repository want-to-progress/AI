#include "STcpClient.h"

#include <iostream>

using namespace std;

struct chess {
	//where my chess is
	int  pos_X, pos_Y;
	//   ---->Y
	//  |
	//  X
};

/*Store the position of the chess with label.*/
void init(vector< vector<int> > board, int label, vector<chess>& c);
/*Choose some chess from c by computing their priority.*/
void choose_chess(vector< vector<int> > board, bool is_black,
	vector<chess> c, vector<chess>& chosen);
/*Compute heuristic.*/
int computeH(vector< vector<int> > board, bool is_black);
/*Check the movement. If legal, move the chess. (Single movement)*/
bool moveCheck_and_move(vector< vector<int> >& board, int label,
	chess& c, int X_move, int Y_move, vector< vector<int> >& step,
	vector< vector<bool> >& visitTable);
/*Move the chess.*/
void moving(vector< vector<int> >& board, bool is_black, chess c,
	vector< vector<int> >& step);
/*Max in minimax.*/
void Max(vector< vector<int> > board, bool is_black,
	vector< vector<int> >& step);
/*Min in minimax*/
int Min(std::vector< std::vector<int> > board, bool is_black, int a);

void init(vector< vector<int> > board, int label, vector<chess>& c)
{
	for (int i = 0; i<8; i++) {
		for (int j = 0; j<8; j++) {
			chess NC;
			if (board[i][j] == label) {
				NC.pos_X = i;
				NC.pos_Y = j;
				c.push_back(NC);
			}
		}
	}
	return;
}

void choose_chess(vector< vector<int> > board, bool is_black,
	vector<chess> c, vector<chess>& chosen)
{
	int enemy = (is_black) ? 2 : 1;
	int max = -10;
	int p[9] = { 0 };
	for (int i = 0; i < c.size(); i++) {
		//if the chess next to the enemy, give the largest priority
		if ((c[i].pos_X < 7 && c[i].pos_X>0) && (c[i].pos_Y < 7 && c[i].pos_Y>0) &&
			(board[c[i].pos_X + 1][c[i].pos_Y] == enemy ||
				board[c[i].pos_X - 1][c[i].pos_Y] == enemy ||
				is_black && board[c[i].pos_X][c[i].pos_Y + 1] == enemy ||
				!is_black && board[c[i].pos_X][c[i].pos_Y - 1] == enemy))
		{
			chosen.push_back(c[i]);
			continue;
		}

		//If the chess is in destination, give the lowest priority
		if ((is_black && c[i].pos_Y > 5) || (!is_black && c[i].pos_Y < 2)) 
		{
			p[i] = -7;
			continue;
		}

		//if the chess is at y == 2 or y == 6, give the second largest priority
		if ((is_black && c[i].pos_Y == 5 && board[c[i].pos_X][c[i].pos_Y + 1] == 0) ||
			(!is_black && c[i].pos_Y == 2 && board[c[i].pos_X][c[i].pos_Y - 1] == 0))
		{
			p[i] = 50;
			continue;
		}

		//if a chess have some chess next to (on its right, left, above, or botton)it,
		//than increase the piority	    
		if (c[i].pos_X != 7 && board[c[i].pos_X + 1][c[i].pos_Y] != 0) p[i]++;
		if (c[i].pos_X != 0 && board[c[i].pos_X - 1][c[i].pos_Y] != 0) p[i]++;
		if (is_black && c[i].pos_Y != 7 && board[c[i].pos_X][c[i].pos_Y + 1] != 0) p[i]++;
		if (!is_black && c[i].pos_Y != 0 && board[c[i].pos_X][c[i].pos_Y - 1] != 0) p[i]++;

		//if a chess have emeny(black == 1 --> emeny == 2) chess on the diagonal
		//lower the piority	
		if (c[i].pos_X != 7 && c[i].pos_Y != 7 && board[c[i].pos_X + 1][c[i].pos_Y + 1] == enemy) p[i]--;
		if (c[i].pos_X != 7 && c[i].pos_Y != 0 && board[c[i].pos_X + 1][c[i].pos_Y - 1] == enemy) p[i]--;
		if (c[i].pos_X != 0 && c[i].pos_Y != 7 && board[c[i].pos_X - 1][c[i].pos_Y + 1] == enemy) p[i]--;
		if (c[i].pos_X != 0 && c[i].pos_Y != 0 && board[c[i].pos_X - 1][c[i].pos_Y - 1] == enemy) p[i]--;

		if (p[i] > max)	max = p[i];
	}
	
	if (chosen.size() > 2)	return;
	
	for (int i = 0; i < 9; ++i)
	{
		if(p[i]==50)	chosen.push_back(c[i]);
	}
	if (chosen.size() > 2)	return;

	/*Choose chess which doesn't have the largest or the second largest priority.*/
	while (chosen.size() < 3 && chosen.size() < c.size())
	{
		vector<int> pool;
		for (int i = 0; i < 9; ++i)
		{
			if (p[i] == max && i < c.size())
			{
				chosen.push_back(c[i]);
				p[i] = -10;
			}
		}
		max = -10;
		for (int i = 0; i < 9; ++i)
			if (p[i] > max && i < c.size())	max = p[i];
	}
	return;
}

int computeH(vector< vector<int> > board, bool is_black)
{
	int numB = 0, numW = 0, numBD = 0, numWD = 0; 

	/*Record the number of black chess, black chess in destination,
	  white chess, and white chess in destination.*/
	for (int i = 0; i<8; i++) {
		for (int j = 0; j<2; j++) {
			if (board[i][j] == 1) numB += j + 1;
			if (board[i][j] == 2) numWD++;
		}
		for (int j = 2; j<6; j++) {
			if (board[i][j] == 1) numB += j + 1;
			if (board[i][j] == 2) numW += 8 - j;
		}
		for (int j = 6; j<8; j++) {
			if (board[i][j] == 1) numBD++;
			if (board[i][j] == 2) numW += 8 - j;
		}
	}
	
	//compute H
	int H;
	if (is_black) H = 2 * numB + 15 * numBD - numW - 15 * numWD;
	else H = 2 * numW + 15 * numWD - numB - 15 * numBD;
	
	return H;
}

bool moveCheck_and_move(vector< vector<int> >& board, int label,
	chess& c, int X_move,  int Y_move, vector< vector<int> >& step,
	vector< vector<bool> >& visitTable)
{
	int nX = c.pos_X + 2 * X_move;
	int nY = c.pos_Y + 2 * Y_move;

	if (nX > 7 || nX < 0 || nY > 7 || nY < 0) return false;

	//If legal, move it.
	if (board[c.pos_X + X_move][c.pos_Y + Y_move] == label &&
		board[nX][nY] == 0 && !visitTable[nX][nY]) {
		vector<int> each_step;
		each_step.push_back(nX);
		each_step.push_back(nY);
		step.push_back(each_step);
		visitTable[nX][nY] = 1;
		board[nX][nY] = board[c.pos_X][c.pos_Y];
		board[c.pos_X][c.pos_Y] = 0;
		if (label != board[nX][nY])
			board[c.pos_X + X_move][c.pos_Y + Y_move] = 0;
		c.pos_X = nX;
		c.pos_Y = nY;
		return true;
	};
	return false;
}

void moving(vector< vector<int> >& board, bool is_black, chess c,
	vector< vector<int> >& step)
{
	int enemy = (is_black) ? 2 : 1;
	int partner = (is_black) ? 1 : 2;
	vector<int> each_step;
	each_step.push_back(c.pos_X);
	each_step.push_back(c.pos_Y);
	step.push_back(each_step);
	vector< vector<bool> > visitTable;
	for (int i = 0; i < 8; ++i)
	{
		vector<bool> r(8, 0);
		visitTable.push_back(r);
	}
	visitTable[c.pos_X][c.pos_Y] = 1;

	//search
	int iter = 0;
	while (iter < 50) {
		iter++;

		if (c.pos_X > 7 || c.pos_X < 0 ||
			c.pos_Y > 7 || c.pos_Y < 0) return;

		//find way - 1 enemy beside
		if (is_black)
		{
			if (moveCheck_and_move(board, enemy, c, 0, 1, step, visitTable))
				continue;
		}
		else {
			if (moveCheck_and_move(board, enemy, c, 0, -1, step, visitTable))
				continue;
		}
		if (moveCheck_and_move(board, enemy, c, 1, 0, step, visitTable))
			continue;
		if (moveCheck_and_move(board, enemy, c, -1, 0, step, visitTable))
			continue;

		//find way - 2 friend beside
		if (is_black)
		{
			if (moveCheck_and_move(board, partner, c, 0, 1, step, visitTable))
				continue;
		}
		else {
			if (moveCheck_and_move(board, partner, c, 0, -1, step, visitTable))
				continue;
		}
		if (moveCheck_and_move(board, partner, c, 1, 0, step, visitTable))
			continue;
		if (moveCheck_and_move(board, partner, c, -1, 0, step, visitTable))
			continue;

		//find way - 3 forward
		//if has jumped in case 1 or 2, won't into case 3 
		if (is_black &&step.size() == 1 && c.pos_Y != 7 && board[c.pos_X][c.pos_Y + 1] == 0) {
			each_step.clear();
			each_step.push_back(c.pos_X);
			each_step.push_back(c.pos_Y + 1);
			step.push_back(each_step);
			board[c.pos_X][c.pos_Y + 1] = board[c.pos_X][c.pos_Y];
			board[c.pos_X][c.pos_Y] = 0;
			c.pos_Y += 1;
		}
		else if (!is_black && step.size() == 1 && c.pos_Y != 0 && board[c.pos_X][c.pos_Y - 1] == 0) {
			each_step.clear();
			each_step.push_back(c.pos_X);
			each_step.push_back(c.pos_Y - 1);
			step.push_back(each_step);
			board[c.pos_X][c.pos_Y - 1] = board[c.pos_X][c.pos_Y];
			board[c.pos_X][c.pos_Y] = 0;
			c.pos_Y -= 1;
		}
		else if (step.size() == 1 && c.pos_X != 7 && board[c.pos_X + 1][c.pos_Y] == 0) {
			each_step.clear();
			each_step.push_back(c.pos_X + 1);
			each_step.push_back(c.pos_Y);
			step.push_back(each_step);
			board[c.pos_X + 1][c.pos_Y] = board[c.pos_X][c.pos_Y];
			board[c.pos_X][c.pos_Y] = 0;
			c.pos_X += 1;
		}
		else if (step.size() == 1 && c.pos_X != 0 && board[c.pos_X - 1][c.pos_Y] == 0) {
			each_step.clear();
			each_step.push_back(c.pos_X - 1);
			each_step.push_back(c.pos_Y);
			step.push_back(each_step);
			board[c.pos_X - 1][c.pos_Y] = board[c.pos_X][c.pos_Y];
			board[c.pos_X][c.pos_Y] = 0;
			c.pos_X -= 1;
		}

		//none of above
		return;
	}
	return;
}

void Max(vector< vector<int> > board, bool is_black,
	vector< vector<int> >& step)
{
	int enemy = (is_black) ? 2 : 1;
	int partner = (is_black) ? 1 : 2;

	vector<chess> mine, chosen;
	init(board, partner, mine);
	choose_chess(board, is_black, mine, chosen);
	int max = -9999;

	//Try all the chosen chess and choose best 
	for (int i = 0; i < chosen.size(); ++i)
	{
		vector< vector<int> > tempBoard = board;
		vector< vector<int> > tempStep;
		moving(tempBoard, is_black, chosen[i], tempStep);
		if (tempStep.size() == 1) continue;
		int temp = Min(tempBoard, is_black, max);
		if (temp > max && temp != 99999)
		{
			max = temp;
			step = tempStep;
		}
	}
	
	return;
}

int Min(std::vector< std::vector<int> > board, bool is_black, int a) {
	int partner = (is_black) ? 2 : 1;

	vector<chess> mine, chosen;
	init(board, partner, mine);
	choose_chess(board, is_black, mine, chosen);
	int min = 99999;

	//Try all the chosen chess and choose best 
	if (chosen.empty()) min = computeH(board, is_black);
	for (int i = 0; i < chosen.size(); ++i)
	{
		vector< vector<int> > tempBoard = board;
		vector< vector<int> > step;
		moving(tempBoard, !is_black, chosen[i], step);
		int temp = computeH(tempBoard, is_black);
		if (temp < min)
			min = temp;
		if (min < a)
			return -9999;
	}
	return min;
}

std::vector< std::vector<int> > GetStep(std::vector< std::vector<int> >& board, bool is_black) {
	std::vector< std::vector<int> > step;
	
	Max(board, is_black, step);
	
	return step;
}

int main() {
	int id_package;
	std::vector< std::vector<int> > board, step;
	bool is_black;
	while (true) {
		if (GetBoard(id_package, board, is_black))
			break;

		step = GetStep(board, is_black);
		SendStep(id_package, step);
	}
}

