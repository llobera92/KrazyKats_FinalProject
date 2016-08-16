// KrazyKats_FinalProject.cpp : Defines the entry point for the console application.
//

/**
COP1334 Final Project
KrazyKats_FinalProject.cpp
Purpose: Krazy Kats. Console game where the user plays the role of a kat's hunter.
		 User must catch every kat to win. User looses when kats steel all his tuna. 

NOTE: All functions are commented in its prototype areas.

@author Randy Llobera Lara
@version 1.1 07/27/16
*/
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <queue>

using namespace std;

#pragma region Constants and Gobal variables

//Board dimensions.
const int ROW = 15; 
const int COL = 30;
//ASCII characters that represents the diffenten player in the game.
const char KATSYMB = 228;
const char HUNSYMB = 234;
const char EMPTYSYMB = 176;
//Number of actions each player can do in each turn. 
const int NUMACTIONS = 2;
//Cooldown for speels to cast it again
int PURRSCOOLDOWN = 0;
//Non-attack actions each kat may do each turn.
enum eKatAction {K_ENDTURN = 0, K_UP, K_LEFT, K_DOWN, K_RIGHT, K_UPLEFT, K_UPRIGHT,
	K_DOWNLEFT, K_DOWNRIGHT};
//Non-attack action for every hunter to do each turn.
enum eHunterAction {ENDTURN = 112, UP = 119, LEFT=97, DOWN=115, RIGHT=100, UPLEFT=113, UPRIGHT=101,
	DOWNLEFT=122, DOWNRIGHT=99, GLASSES=114, EARMUFFHAPPY=116};

#pragma endregion

#pragma region Classes

/**
Class Point used to represent the location of a player in the board.

Members:
Intergers x, y represents row location and column location of the character.
*/
class Point {
private:
	int x, y;
public:
	Point(int inputX = 0, int inputY = 0)
	{
		x = inputX;
		y = inputY;
	}
	int getX() { return x; }
	int getY() { return y; }
	void setX(int newX) { x = newX; }
	void setY(int newY) { y = newY; }

};

#pragma endregion

#pragma region Structures

/**
Hunter struc used to represent the hunter.

Members:
Hunter location in the board.
Life(tuna left) of the hunter.
Modifiers that chage the behaivior of the hunter.
*/
struct Hunter
{
	Point myPos;
	int tunaCans;
	bool isStunned;
	bool reflectiveGlassesSpell;
	bool earmuffOfHappinessSpell;
	Hunter()
	{
		myPos.setX(rand() % (ROW - 2) + 1);
		myPos.setY(rand() % (COL - 2) + 1);
		tunaCans = 5;
		isStunned = false;
		reflectiveGlassesSpell = false;
		earmuffOfHappinessSpell = false;
	}
};

/**
Kat struct used to represent a kat with its characteristics and skills.

Members:
This kat location in the board.
Modifiers that change the behaivor of the character
*/
struct Kat
{
	Point myPos;
	bool isStupid;
	Kat()
	{

	}
	Kat(bool stupidity)
	{
		myPos.setX(rand() % (ROW - 2) + 1);
		myPos.setY(rand() % (COL - 2) + 1);
		isStupid = stupidity;
	}
};

/**
Board struct to represent the game's field.

Members:
Map representation.
Game loop handler.
Kats playing.
The hunter.
*/
struct Board
{
	char map[ROW][COL];
	bool gameOver;
	vector<Kat> aliveKats;
	Hunter hunter;
	Board()
	{
		gameOver = false;
	}
};

#pragma endregion

/*********************
	PROTOTYPE AREA
*********************/

#pragma region Game functions prototype

//Returns a Non-attack action based on users input.
eHunterAction KeyboardInput();
//Displays menu for user to know what key to press.
void UserMenu();
//Creates the game field.
void FillBoard(Board&);
//Displays board in console.
void DisplayBoard(Board&);
//Initializes the game with every character and its modifiers.
void InitializeGame(Board&);
//Fill up game board with characters. Hunter and Kats.
void DrawPlayers(Board&, Hunter, vector<Kat>);
//Logic of the game. Triggers every turns. Decides when game is over. 
void GameEngine(Board&);
//Check if a certain location can be occupied for a character.
bool IsAvailable(Board, Point);
//General fuction to dertermine if an attack succeded or not.
bool AttackSucceded(int);


#pragma endregion

#pragma region Hunter functions prototype

//Creates the hunter and positions him in the board.
Hunter HunterKreator(Board&);
//Handles each hunter turn. Decides if attack or non-attack action will be performed.
void HunterTurn(Board&);
//Creates a queue that contains all kats in the hunter scope.
queue<Kat> KatNearby(Board, Point, int);
//Decides non-attack action hunter will perform when no attack is possible.
void HunterNonAttackAction(Board&);
//Hunter attack.
bool HunterAttack(Board&, Kat);
//Remove Kaptured Kat from the game.
void RemoveKat(Board&, Kat);

#pragma endregion

#pragma region Kats functions prototype

//Creates a Kat and positions it in the board.
Kat KatKreator(bool, Board&);
//Handles Kats turns. Define action narrows down action to perform based on kat stupidity.
void KatsTurn(Board&);
//Checks if hunter nearby. Variable passed by reference holds distance the hunter is from Kat.
bool HunterNearby(Board, Kat, int&);
//Handles each Stupid Kat turn. Decides if attack or non-attack action will be performed.
void StupidKatActtion(Board&, Kat, int);
//Handles each Smart Kat turn. Decides if attack or non-attack action will be performed.
void SmartKatAction(Board&, Kat, int);
//Perform Eyes of Submission attack.
void KatEyesOfSub(Board&);
//Casts Purrs of Doom spell.
void PurrsOfDoom(Board&);
//Decides the direction Smart Kats take to reach the Hunter
eKatAction KatMoveEngine(Board, Point);
//Decides non-attack action Kats will perform when no attack is possible.
void KatNonAttackAction(Board&, Kat, int);
//hey
#pragma endregion

/*********************
	FUNCTIONS AREA
*********************/

#pragma region Game Logic Functions

void UserMenu()
{
	cout << "*****************************************************" << endl;
	cout << "Please select one the following option for your move." << endl;
	cout << endl;
	cout << "\tPress 'w' to move up " << endl;
	cout << "\tPress 'a' to move left " << endl;
	cout << "\tPress 's' to move down " << endl;
	cout << "\tPress 'd' to move right " << endl;
	cout << "\tPress 'q' to move upleft " << endl;
	cout << "\tPress 'e' to move upright " << endl;
	cout << "\tPress 'z' to move downleft " << endl;
	cout << "\tPress 'c' to move downright " << endl;
	cout << "\tPress 'r' to cast Reflective Glasses spell " << endl;
	cout << "\tPress 't' to cast Earmuffs of Happiness spell " << endl;
	cout << "\tPress 'f' to end turn " << endl;
	cout << endl;
	cout << "*****************************************************" << endl;
}

eHunterAction KeyboardInput()
{
	char keyInput;
	UserMenu();
	cout << "Your move: ";
	cin >> keyInput;
	
	switch (keyInput)
	{
	case 'f':
		return ENDTURN;
		break;
	case 'w':
		return UP;
		break;
	case 'a':
		return LEFT;
		break;
	case 's':
		return DOWN;
		break;
	case 'd':
		return RIGHT;
		break;
	case 'q':
		return UPLEFT;
		break;
	case 'e':
		return UPRIGHT;
		break;
	case 'z':
		return DOWNLEFT;
		break;
	case 'c':
		return DOWNRIGHT;
		break;
	case 'r':
		return GLASSES;
		break;
	case 't':
		return EARMUFFHAPPY;
		break;
	default:
		break;
	}
}

void FillBoard(Board& board)	
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (i == 0 || i == ROW - 1)
			{
				board.map[i][j] = 205;
			}
			else if (j == 0 || j == COL - 1)
			{
				board.map[i][j] = 186;
			}
			else
			{
				board.map[i][j] = EMPTYSYMB;
			}
		}
	}

	board.map[0][0] = 201;
	board.map[ROW - 1][0] = 200;
	board.map[0][COL - 1] = 187;
	board.map[ROW - 1][COL - 1] = 188;
}

void DisplayBoard(Board& board)
{
	system("cls");
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			cout << board.map[i][j];
		}
		cout << endl;
	}
	cout << "Hunter tuna cans remaining: " << board.hunter.tunaCans << endl;
	cout << "Number of Kat still in the town: " << board.aliveKats.size() << endl;
	Sleep(500);
}

void InitializeGame(Board& board)	
{
	//Working in creation of characters
	int smartKats;
	int stupidKats;
	Kat newKat;
	cout << "How many Smarts Kats you want to kill: ";
	cin >> smartKats;										//Number of Smart Kats in the game
	cout << "How many Stupid Kats you want to kill: ";
	cin >> stupidKats;										//Number of Stupid Kats in the game
	//Initializing characters
	Hunter newHunter = HunterKreator(board);				
	board.hunter = newHunter;								
	for (int i = 0; i < smartKats; i++)
	{
		newKat = KatKreator(false, board);
		board.aliveKats.push_back(newKat);
	}
	for (int i = 0; i < stupidKats; i++)
	{
		newKat = KatKreator(true, board);
		board.aliveKats.push_back(newKat);
	}
	//Drawing players in the map
	DrawPlayers(board, newHunter, board.aliveKats);
}

void DrawPlayers(Board& board, Hunter newHunter, vector<Kat> aliveKats)		
{

	Point hunterPoint(newHunter.myPos.getX(), newHunter.myPos.getY());
	//Drawing each character in the map
	if (IsAvailable(board, hunterPoint))
	{
		board.map[newHunter.myPos.getX()][newHunter.myPos.getY()] = 234;
	}
	for (int i = 0; i < aliveKats.size(); i++)
	{
		Kat currentKat = aliveKats[i];
		Point katPoint(currentKat.myPos.getX(), currentKat.myPos.getY());

		if (IsAvailable(board, katPoint))
		{
			board.map[currentKat.myPos.getX()][currentKat.myPos.getY()] = 228;
		}
	}
}

bool IsAvailable(Board board, Point point)	
{
	char currentSymb = board.map[point.getX()][point.getY()];
	return (currentSymb == EMPTYSYMB);	//If diffetent of empty symbol, then this position is not occupied.
}

void GameEngine(Board& board)
{
	Hunter currentHunter = board.hunter;
	if (currentHunter.tunaCans > 0)
	{
		HunterTurn(board);
		if (board.hunter.tunaCans <= 0) //I validate here because the hunter can kill himself with Kat Kapture.
		{
			board.gameOver = true;
			cout << endl;
			cout << "Those Kats are strong. Good Try." << endl;
			cin.get();
		}
	}
	if (board.aliveKats.size() > 0)		//Since the hunter might have killed all Kats, is good to check here if there is Kats alive.
	{
		KatsTurn(board);
		if (currentHunter.tunaCans <= 0)
		{
			board.gameOver = true;
			cout << endl;
			cout << "Those Kats are strong. Good Try." << endl;
			Sleep(4000);
		}
	}
	else
	{
		board.gameOver = true;
		cout << endl;
		cout << "WOW! You are a Kat Assasing. Good job." << endl;
		Sleep(4000);
	}
}

bool AttackSucceded(int accPercent)
{
	int accuracy = rand() % 100 + 1;
	return (accuracy <= accPercent);	//If random number is less than the accuracy range, then attack succeded.
}

bool Move(Board& board, char player,Point startPos, Point endPos)
{
	if (IsAvailable(board, endPos))
	{
		board.map[endPos.getX()][endPos.getY()] = player;			//Place character in the position if is available.
		board.map[startPos.getX()][startPos.getY()] = EMPTYSYMB;
		return true;
	}
	return false;
}

#pragma endregion

#pragma region Hunter related Functions

Hunter HunterKreator(Board& board)
{
	/*This funtion will create a hunter. This hunter
	will have random coordenates since he just get into
	the game. Have to check is available*/
	Hunter newHunter;
	Point newHunterPos(newHunter.myPos.getX(), newHunter.myPos.getY());
	while (!IsAvailable(board,newHunterPos))
	{
		newHunterPos.setX(rand() % (ROW - 2) + 1);
		newHunterPos.setY(rand() % (COL - 2) + 1);
	}
	newHunter.myPos.setX(newHunterPos.getX());
	newHunter.myPos.setY(newHunterPos.getY());
	board.map[newHunter.myPos.getX()][newHunter.myPos.getY()] = HUNSYMB;
	
	return newHunter;
}

void HunterTurn(Board& board)
{
	/*Hunter has priority over Kats in the turn.
	There is a pathern in choosing the action to perform.
	If there is kats to attack attack them, if not, the user can move using
	the keyboard or choose a defensive spell*/
	
	Kat katToAttack;
	int actionsCount = 0;	//Actions counter. when two actions perform, break.
	queue<Kat> katsNearby;	//Criatures available to attack.
	if (!board.hunter.isStunned)
	{
		while (actionsCount < NUMACTIONS)
		{	
			katsNearby = KatNearby(board, board.hunter.myPos, 1);
			if (!katsNearby.empty())
			{
				katToAttack = katsNearby.front();
				if (HunterAttack(board, katToAttack))
				{
					katsNearby.pop();
					if (board.aliveKats.empty()) return;
				}
				actionsCount++;
			}
			else
			{
				HunterNonAttackAction(board);	//If no Kats to kill. Perform a non-attack action.
				actionsCount++;
			}
			DisplayBoard(board);	//Display what happened.
		}
		PURRSCOOLDOWN++;	//Complete cooldown to let Kats stun the hunter again.
	}
	else
	{
		cout << "Hunter stunned for one turn." << endl;
		Sleep(1500);
		board.hunter.isStunned = false;
	}
	cout << "Hunter turn ended." << endl;
	Sleep(1500);
}

queue<Kat> KatNearby(Board board, Point refPoint, int squaresAway)
{
	/*Check for every Kat nearby (means eight positions around the hunter)
	in order to attack them.
	It recieves a squareAway interger to add flexibility for attacks
	with more than one square of range.*/
	queue<Kat> katsNearby;
	Kat tempKat;
	int x = refPoint.getX();
	int y = refPoint.getY();

	for (int i = 0; i < board.aliveKats.size(); i++)
	{
		tempKat = board.aliveKats[i];
		if (x > 0 && y > 0)
		{
			if ((x - squaresAway) == tempKat.myPos.getX() && (y - squaresAway) == tempKat.myPos.getY())
			{
				katsNearby.push(tempKat);
			}

		}
		if (x > 0 && y < COL)
		{
			if ((x - squaresAway) == tempKat.myPos.getX() && (y + squaresAway) == tempKat.myPos.getY())
			{
				katsNearby.push(tempKat);
			}
		}
		if (x < ROW && y > 0)
		{
			if ((x + squaresAway) == tempKat.myPos.getX() && (y - squaresAway) == tempKat.myPos.getY())
			{
				katsNearby.push(tempKat);
			}
		}
		if (x < ROW && y < COL)
		{
			if ((x - squaresAway) == tempKat.myPos.getX() && (y - squaresAway) == tempKat.myPos.getY())
			{
				katsNearby.push(tempKat);
			}
		}
		if (x > 0)
		{
			if ((x - squaresAway) == tempKat.myPos.getX() && (y) == tempKat.myPos.getY())
			{
				katsNearby.push(tempKat);
			}
		}
		if (x < ROW)
		{
			if ((x + squaresAway) == tempKat.myPos.getX() && (y) == tempKat.myPos.getY())
			{
				katsNearby.push(tempKat);
			}
		}
		if (y > 0)
		{
			if ((x) == tempKat.myPos.getX() && (y - squaresAway) == tempKat.myPos.getY())
			{
				katsNearby.push(tempKat);
			}
		}
		if (y < COL)
		{
			if ((x) == tempKat.myPos.getX() && (y + squaresAway) == tempKat.myPos.getY())
			{
				katsNearby.push(tempKat);
			}
		}
	}
	return katsNearby;
}

void HunterNonAttackAction(Board& board)
{
	//Perform a non-attack action based on user input.
	eHunterAction nextAccion;
	Point newPoint;
	bool canMove = false;
	int hunterX = board.hunter.myPos.getX();
	int hunterY = board.hunter.myPos.getY();
	newPoint.setX(hunterX);
	newPoint.setY(hunterY);
	do
	{
		nextAccion = KeyboardInput();
		switch (nextAccion)
		{
		case ENDTURN:
			canMove = true;
			break;
		case UP:
			newPoint.setX(hunterX - 1);
			newPoint.setY(hunterY);
			break;
		case LEFT:
			newPoint.setX(hunterX);
			newPoint.setY(hunterY - 1);
			break;
		case DOWN:
			newPoint.setX(hunterX + 1);
			newPoint.setY(hunterY);
			break;
		case RIGHT:
			newPoint.setX(hunterX);
			newPoint.setY(hunterY + 1);
			break;
		case UPLEFT:
			newPoint.setX(hunterX - 1);
			newPoint.setY(hunterY - 1);
			break;
		case UPRIGHT:
			newPoint.setX(hunterX - 1);
			newPoint.setY(hunterY + 1);
			break;
		case DOWNLEFT:
			newPoint.setX(hunterX + 1);
			newPoint.setY(hunterY - 1);
			break;
		case DOWNRIGHT:
			newPoint.setX(hunterX + 1);
			newPoint.setY(hunterY + 1);
			break;
		case GLASSES:
			board.hunter.reflectiveGlassesSpell = true;
			canMove = true;
			cout << "Hunter cast Reflective Glasses spell" << endl;
			Sleep(1500);
			break;
		case EARMUFFHAPPY:
			board.hunter.earmuffOfHappinessSpell = true;
			canMove = true;
			cout << "Hunter cast Earmuff of Happiness spell" << endl;
			Sleep(1500);
			break;
		default:
			break;
		}	//If position is empty, user can move there. If not he have to enter an empty one or pass his turn.
		if (Move(board, HUNSYMB, board.hunter.myPos, newPoint))	canMove = true;
		else cout << "I am sorry you can't move to that spot." << endl;

	}while (!canMove);

	board.hunter.myPos.setX(newPoint.getX());	//Change hunter's position.
	board.hunter.myPos.setY(newPoint.getY());
}

bool HunterAttack(Board& board, Kat enemy)
{
	const int KATKAPT = 75;
	const int UNOPENTUNA = 10;
	const int AUTOKAPT = 1;

	if (AttackSucceded(KATKAPT))
	{
		cout << "You won the battle" << endl;
		Sleep(1500);
		board.map[enemy.myPos.getX()][enemy.myPos.getY()] = EMPTYSYMB; //Clear Kat position in the map.
		RemoveKat(board, enemy);
		if (AttackSucceded(UNOPENTUNA))
		{
			board.hunter.tunaCans++;
			cout << "You found an unopen tuna can" << endl;
			Sleep(1500);
		}
		return true;
	}
	else if (AttackSucceded(AUTOKAPT))  //Here should end the game because the hunter kaptured himself
	{
		cout << "Oh no! Hunter you captures yourself" << endl;
		Sleep(1500);
		board.gameOver = true;
		board.hunter.tunaCans = 0;
	}
	return false;
}

void RemoveKat(Board& board, Kat kat)
{
	//Remove Kat from vector of Kats alive in the game.
	vector<Kat> aliveKats = board.aliveKats;
	for (int i = 0; i < aliveKats.size(); i++)
	{
		if (aliveKats[i].myPos.getX() == kat.myPos.getX() && aliveKats[i].myPos.getY() == kat.myPos.getY())
			board.aliveKats.erase(board.aliveKats.begin() +i);
	}
}

#pragma endregion

#pragma region Kat related Functions

Kat KatKreator(bool stupidity, Board& board)
{
	/*This funtion will create a kat. This kat
	will have random coordenates since he just get into
	the game. Have to check is available*/
	Kat newKat(stupidity);
	Point newKatPos(newKat.myPos.getX(), newKat.myPos.getY());
	while (!IsAvailable(board, newKatPos))
	{
		newKatPos.setX(rand() % (ROW - 2) + 1);
		newKatPos.setY(rand() % (COL - 2) + 1);
	}
	newKat.myPos.setX(newKatPos.getX());
	newKat.myPos.setY(newKatPos.getY());
	board.map[newKat.myPos.getX()][newKat.myPos.getY()] = KATSYMB;
	return newKat;
}

void KatsTurn(Board& board)
{
	/*Handle every kat turn.
	Perform different actions based on kat stupidity.*/
	int innerRand;
	bool isStunned;
	int distance;
	for (int i = 0; i < board.aliveKats.size(); i++)
	{
		if (board.aliveKats[i].isStupid)
		{
			for (int j = 0; j < NUMACTIONS; j++)
			{
				StupidKatActtion(board, board.aliveKats[i], i); //Go and do your stupid move.
				DisplayBoard(board);
			}
		}
		else
		{
			for (int j = 0; j < NUMACTIONS; j++)
			{
				SmartKatAction(board, board.aliveKats[i], i); //Determine what action should this Smart Kat do.
				DisplayBoard(board);
			}
		}
		if(board.hunter.tunaCans <= 0) return;
	}
}

bool HunterNearby(Board board, Kat currentKat,int& squaresAway)
{
	//Have to check every neightbor to see if hunter is near.
	int katX = currentKat.myPos.getX();
	int katY = currentKat.myPos.getY();
	int hunterX = board.hunter.myPos.getX();
	int hunterY = board.hunter.myPos.getY();

	for (int i = 1; i <= 2; i++)
	{
		if ((katX - i) > 0 && (katY - i) > 0)
		{
			if ((katX - i) == hunterX && (katY - i) == hunterY)
			{
				squaresAway = i;
				return true;
			}

		}
		if ((katX - i) > 0 && (katY + i) < COL)
		{
			if ((katX - i) == hunterX && (katY + i) == hunterY)
			{
				squaresAway = i;
				return true;
			}

		}
		if ((katX + i) < ROW && (katY - i) > 0)
		{
			if ((katX + i) == hunterX && (katY - i) == hunterY)
			{
				squaresAway = i;
				return true;
			}

		}
		if ((katX + i) < ROW && (katY + i) < COL)
		{
			if ((katX + i) == hunterX && (katY + i) == hunterY)
			{
				squaresAway = i;
				return true;
			}

		}
		if ((katX - i) > 0)
		{
			if ((katX - i) == hunterX && katY == hunterY)
			{
				squaresAway = i;
				return true;
			}

		}
		if ((katX + i) < ROW)
		{
			if ((katX + i) == hunterX && katY == hunterY)
			{
				squaresAway = i;
				return true;
			}

		}
		if ((katY - i) > 0)
		{
			if (katX == hunterX && (katY - i) == hunterY)
			{
				squaresAway = i;
				return true;
			}

		}
		if (katX && (katY + i) < COL)
		{
			if (katX == hunterX && (katY + i) == hunterY)
			{
				squaresAway = i;
				return true;
			}

		}
	}
	return false;
}

void StupidKatActtion(Board& board, Kat stupidKat, int katIndex)
{
	int innerRand = rand() % 2;
	int squaresAway;
	if (innerRand == 1 && HunterNearby(board, stupidKat, squaresAway))
	{
		innerRand = rand() % 2;
		if (!(board.hunter.isStunned) && innerRand == 1 && squaresAway <= 2 && PURRSCOOLDOWN == 2)
			PurrsOfDoom(board);
		else if (board.hunter.isStunned && innerRand == 2 && squaresAway == 1)
			KatEyesOfSub(board);
	}
	else
	{
		KatNonAttackAction(board, stupidKat, katIndex);
	}
}

void SmartKatAction(Board& board, Kat smartKat, int katIndex)
{
	bool isStunned;
	int squaresAway;
	if (HunterNearby(board, smartKat, squaresAway))
	{
		if (!(board.hunter.isStunned) && PURRSCOOLDOWN == 2)
			PurrsOfDoom(board);
		else if (squaresAway == 1)
			KatEyesOfSub(board);
		else
			KatNonAttackAction(board, smartKat, katIndex);
	}
	else
	{
		KatNonAttackAction(board, smartKat, katIndex);
	}
}

void KatEyesOfSub(Board& board)
{
	int eyesSub;
	if(board.hunter.reflectiveGlassesSpell)
		eyesSub = 30;
	eyesSub = 60;

	if (AttackSucceded(eyesSub))
	{
		cout << "Kat hit you with Eyes of Submission" << endl;
		Sleep(1500);
		board.hunter.tunaCans--;
	}
}

void PurrsOfDoom(Board& board)
{
	int puursDoom;
	if (board.hunter.earmuffOfHappinessSpell)
		puursDoom = 40;
	puursDoom = 80;

	if (AttackSucceded(puursDoom))
	{
		board.hunter.isStunned = true;
		cout << "Kat stunned you with Puurs of Doom" << endl;
		Sleep(1500);
		PURRSCOOLDOWN = 0;
	}
}

eKatAction KatMoveEngine(Board board, Point katPos, int katIndex)	
{
	int katX = katPos.getX();
	int katY = katPos.getY();
	int hunterX = board.hunter.myPos.getX();
	int hunterY = board.hunter.myPos.getY();
	Point newPoint;

	if (hunterX < katX && hunterY < katY)
	{
		newPoint.setX(katX - 1);
		newPoint.setY(katY - 1);
		if(IsAvailable(board, newPoint))return K_UPLEFT;
	}
	else if (hunterX > katX && hunterY < katY)
	{
		newPoint.setX(katX + 1);
		newPoint.setY(katY - 1);
		if (IsAvailable(board, newPoint))return K_DOWNLEFT;
	}
	else if (hunterX > katX && hunterY > katY)
	{
		newPoint.setX(katX + 1);
		newPoint.setY(katY + 1);
		if (IsAvailable(board, newPoint))return K_DOWNRIGHT;
	}
	else if (hunterX < katX && hunterY > katY)
	{
		newPoint.setX(katX - 1);
		newPoint.setY(katY + 1);
		if (IsAvailable(board, newPoint))return K_UPRIGHT;
	}
	else if (hunterX < katX && hunterY == katY)
	{
		newPoint.setX(katX - 1);
		newPoint.setY(katY);
		if (IsAvailable(board, newPoint))return K_UP;
	}
	else if (hunterX == katX && hunterY < katY)
	{
		newPoint.setX(katX);
		newPoint.setY(katY - 1);
		if (IsAvailable(board, newPoint))return K_LEFT;
	}
	else if (hunterX > katX && hunterY == katY)
	{
		newPoint.setX(katX + 1);
		newPoint.setY(katY);
		if (IsAvailable(board, newPoint))return K_DOWN;
	}
	else if (hunterX == katX && hunterY > katY)
	{
		newPoint.setX(katX);
		newPoint.setY(katY + 1);
		if (IsAvailable(board, newPoint))return K_RIGHT;
	}
	return K_ENDTURN;
}

void KatNonAttackAction(Board& board, Kat kat, int katIndex)
{
	eKatAction nextAction;
	bool canMove = false;
	int katX = kat.myPos.getX();
	int katY = kat.myPos.getY();
	Point newPoint(katX, katY);
	do
	{
		if(!kat.isStupid) nextAction = KatMoveEngine(board, kat.myPos, katIndex);
		else nextAction = static_cast<eKatAction>(rand() % 8 + 1);
		switch (nextAction)
		{

		case K_ENDTURN:
			canMove = true;
			break;
		case K_UP:
			newPoint.setX(katX - 1);
			newPoint.setY(katY);
			break;
		case K_LEFT:
			newPoint.setX(katX);
			newPoint.setY(katY - 1);
			break;
		case K_DOWN:
			newPoint.setX(katX + 1);
			newPoint.setY(katY);
			break;
		case K_RIGHT:
			newPoint.setX(katX);
			newPoint.setY(katY + 1);
			break;
		case K_UPLEFT:
			newPoint.setX(katX - 1);
			newPoint.setY(katY - 1);
			break;
		case K_UPRIGHT:
			newPoint.setX(katX - 1);
			newPoint.setY(katY + 1);
			break;
		case K_DOWNLEFT:
			newPoint.setX(katX + 1);
			newPoint.setY(katY - 1);
			break;
		case K_DOWNRIGHT:
			newPoint.setX(katX + 1);
			newPoint.setY(katY + 1);
			break;
		default:
			break;
		}
		if(!canMove)
			canMove = Move(board, KATSYMB, kat.myPos, newPoint);	
	} while (!canMove);
	board.aliveKats[katIndex].myPos.setX(newPoint.getX());
	board.aliveKats[katIndex].myPos.setY(newPoint.getY());
}

#pragma endregion

int main()
{
	srand(time(0));
	Board gameBoard;
	FillBoard(gameBoard);
	InitializeGame(gameBoard);
	DisplayBoard(gameBoard);
	while (!gameBoard.gameOver)
	{
		GameEngine(gameBoard);
		DisplayBoard(gameBoard);
	}
    return 0;
}