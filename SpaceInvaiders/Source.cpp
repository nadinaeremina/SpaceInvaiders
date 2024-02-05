#include <iostream>
#include <time.h>
#include <Windows.h>
#include <vector>
#include <conio.h>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>

using namespace std;

enum Color
{
	Black, Blue, Green, Cyan, Red, Magenta, Brown,
	LightGray, DarkGray, LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White
};

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

void setColor(Color text, Color background)
{
	SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

enum Direction { Up = 72, Left = 75, Right = 77, Down = 80, Enter = 13, Esc = 27, Space = 32 };

void setCursor(int x, int y)
{
	COORD myCoords = { x,y };
	SetConsoleCursorPosition(hStdOut, myCoords);
}

vector <COORD> all_shut_coords;

class Character
{
protected:

	int health = 0;
	Color color;
	enum { row = 3 };
	enum { col = 5 };
	char view[row][col];
	vector <COORD> one_shut_coords;
	bool isAlive = true;

public:

	virtual void deadBuka() = 0;

	void setColorColor(Color color)
	{
		this->color = color;
	}

	void setView()
	{
		for (size_t i = 0; i < row; i++)
		{
			for (size_t j = 0; j < col; j++)
			{
				if ((i == 0) || (i == 1 && j > 0 && j < col - 1))
					view[i][j] = '*';
				else if (i == 2 && j == col / 2)
					view[i][j] = '&';
				else
					view[i][j] = ' ';
			}
		}
	}

	int getRow() const
	{
		return row;
	}

	int getCol() const
	{
		return col;
	}

	int getHealth() const
	{
		return health;
	}

	vector <COORD> getCoords() const
	{
		return one_shut_coords;
	}

	int getX() const
	{
		return one_shut_coords[12].X;
	}

	int getY() const
	{
		return one_shut_coords[12].Y;
	}

	Color getColor() const
	{
		return color;
	}

	bool getAlive() const
	{
		return isAlive;
	}

	Character() : Character(30, Magenta)
	{
	}

	Character(int health, Color color)
	{
		this->health = health;
		this->color = color;
	}

	void showEnemy(COORD koord)
	{
		vector <COORD> local_coords;
		for (size_t i = 0; i < row; i++)
		{
			COORD obj = koord;
			setCursor(koord.X, koord.Y++);
			for (size_t j = 0; j < col; j++)
			{
				if (view[i][j] == '*')
					setColor(color, color);
				else if (view[i][j] == '&')
					setColor(Red, Red);
				else
					setColor(Black, Black);
				local_coords.push_back(obj);
				obj.X++;
				cout << view[i][j];
			}
		}
		one_shut_coords = local_coords;
		if (all_shut_coords.size() == 450)
		{
			all_shut_coords.clear();
		}
		for (size_t i = 0; i < one_shut_coords.size(); i++)
		{
			all_shut_coords.push_back(one_shut_coords[i]);
		}
		setColor(White, Black);
	}

	void delEnemy(COORD koord)
	{
		setCursor(koord.X, koord.Y);
		for (size_t i = 0; i < row; i++)
		{
			setCursor(koord.X, koord.Y + i);
			for (size_t j = 0; j < col; j++)
			{
				setColor(Black, Black);
				cout << view[i][j];
			}
		}
		setColor(White, Black);
	}
};

class Enemy : public Character
{
public:

	void deadBuka() override
	{
		for (size_t i = 0; i < row; i++)
		{
			for (size_t j = 0; j < col; j++)
			{
				view[i][j] = ' ';
			}
		}
		isAlive = false;
	}

	Enemy() : Character()
	{
		setView();
	}

	Enemy(int health, Color color) : Character(health, color)
	{
		setView();
	}

	~Enemy()
	{
		setCursor(10, 10);
		cout << "Dead!";
		deadBuka();
	}
};

class Enemies
{
	vector <Character*> bukahi;
	int row = 3;
	int col = 10;

public:

	Enemies()
	{
		row = 3;
		col = 10;
		int health = 3;
		for (size_t i = 0; i < row; i++)
		{
			for (size_t j = 0; j < col; j++)
			{
				if (i == 0)
					bukahi.push_back(new Enemy(health, Green));
				else if (i == 1)
				{
					health = 2;
					bukahi.push_back(new Enemy(health, Yellow));
				}
				else
				{
					health = 1;
					bukahi.push_back(new Enemy(health, Magenta));
				}
			}
		}
		for (size_t i = 0; i < bukahi.size(); i++)
		{
			if (i == 0 || i == 1 || i == 2 || i == 7 || i == 8 || i == 9 || i == 10 || i == 19)
				bukahi[i]->deadBuka();
		}
	}

	void showEnemis(COORD koord)
	{
		int temp = koord.X, kol = 0;
		for (auto var : bukahi)
		{
			var->showEnemy(koord);
			koord.X += 8;
			kol++;
			if (kol == 10 || kol == 20)
			{
				koord.Y += 5;
				koord.X = temp;
			}
		}
	}

	int getSize()
	{
		return bukahi.size();
	}

	int getAliveSize() const
	{
		int size = 0;
		for (size_t i = 0; i < bukahi.size(); i++)
		{
			if (bukahi[i]->getAlive())
			{
				size++;
			}
		}
		return size;
	}

	int getLowerY() const
	{
		int index = -1;
		for (size_t i = 0; i < bukahi.size(); i++)
		{
			if (bukahi[i]->getAlive())
				index = i;
		}
		if (index < 10)
			return all_shut_coords[all_shut_coords.size() - 1 - 20].Y;
		else if (index >= 10 && index < 20)
			return all_shut_coords[all_shut_coords.size() - 1 - 10].Y;
		else
			return all_shut_coords[all_shut_coords.size() - 1].Y;
	}

	Character* operator[](int index)
	{
		if (index < bukahi.size() && index >= 0)
		{
			return bukahi[index];
		}
	}

	void delEnemis(COORD koord)
	{
		int temp = koord.X, kol = 0;
		for (auto var : bukahi)
		{
			var->delEnemy(koord);
			koord.X += 8;
			kol++;
			if (kol == 10 || kol == 20)
			{
				koord.Y += 5;
				koord.X = temp;
			}
		}
	}

	~Enemies()
	{
		bukahi.clear();
	}
};

class Hero
{
protected:

	int health = 5;
	enum { row = 4 };
	enum { col = 7 };
	char view[row][col];
	vector <COORD> all_coords_hero;

public:

	Hero() : Hero(health)
	{
		health = 3;
	}

	Hero(int health)
	{
		this->health = health;
		setView();
	}

	void setHealth(int health)
	{
		this->health = health;
	}

	int getHealth() const
	{
		return health;
	}

	vector <COORD> KOORD() const
	{
		return all_coords_hero;
	}

	void setView()
	{
		for (size_t i = 0; i < row; i++)
		{
			for (size_t j = 0; j < col; j++)
			{
				if (i == 0 && j == col / 2)
					view[i][j] = '*';
				else if ((i == 1 && j > 1 && j < col - 2) || (i == 2 && j > 1 && j < col - 2))
					view[i][j] = '&';
				else if ((i == 2 && (j == 1 || j == col - 2)) || i == 3)
					view[i][j] = '%';
				else
					view[i][j] = ' ';
			}
		}
	}

	void showEnemy(COORD koord)
	{
		vector <COORD> local_coords;
		for (size_t i = 0; i < row; i++)
		{
			COORD obj = koord;
			setCursor(koord.X, koord.Y++);
			for (size_t j = 0; j < col; j++)
			{
				if (view[i][j] == '*')
					setColor(Red, Red);
				else if (view[i][j] == '&')
					setColor(Blue, Blue);
				else if (view[i][j] == '%')
					setColor(White, White);
				else
					setColor(Black, Black);
				local_coords.push_back(obj);
				obj.X++;
				cout << view[i][j];
			}
		}
		all_coords_hero = local_coords;
		setColor(White, Black);
	}

	void delEnemy(COORD koord)
	{
		setCursor(koord.X, koord.Y);
		for (size_t i = 0; i < row; i++)
		{
			setCursor(koord.X, koord.Y + i);
			for (size_t j = 0; j < col; j++)
			{
				setColor(Black, Black);
				cout << view[i][j];
			}
		}
		setColor(White, Black);
	}
};

class Fire
{
private:

	char fire;
	Color color;
	COORD koord;

public:

	Fire() : Fire(fire, color, koord)
	{
	}

	Fire(char fire, Color color, COORD koord)
	{
		this->fire = fire;
		this->color = color;
		this->koord.X = koord.X;
		this->koord.Y = koord.Y;
	}

	void showFire()
	{
		setColor(color, color);
		setCursor(koord.X, koord.Y);
		cout << fire;
	}

	void delFire()
	{
		setColor(Black, Black);
		setCursor(koord.X, koord.Y);
		cout << fire;
	}

	COORD moveFire(int number)
	{
		while (koord.Y != number)
		{
			showFire();
			Sleep(25);
			delFire();
			koord.Y--;
		}
		return this->koord;
	}

	COORD moveFire2(int number)
	{
		while (koord.Y != number)
		{
			showFire();
			Sleep(25);
			delFire();
			koord.Y++;
		}
		return this->koord;
	}
};

void printMenu(vector<string>masMenu, int punct, int X, int Y)
{
	for (size_t i = 0; i < masMenu.size(); i++)
	{
		if (i == punct)
		{
			setColor(White, Magenta);
		}
		else
		{
			setColor(White, Black);
		}
		setCursor(X, Y + i);
		cout << masMenu[i];
	}
	setColor(White, Black);

}

int choiseMenu(vector<string>masMenu, int X, int Y)
{
	int key;
	int punctNumber = 0;
	do
	{
		printMenu(masMenu, punctNumber, X, Y);
		key = _getch();
		switch (key)
		{
		case Up:
			if (punctNumber == 0)
			{
				punctNumber = masMenu.size() - 1;
			}
			else if (punctNumber > 0)
			{
				punctNumber--;
			}
			break;
		case Down:
			if (punctNumber == masMenu.size() - 1)
			{
				punctNumber = 0;
			}
			else if (punctNumber < masMenu.size() - 1)
			{
				punctNumber++;
			}
			break;
		case Enter:
			if (punctNumber == masMenu.size() - 1)
			{
				return -1;
			}
			return punctNumber;
		}
	} while (key != Esc);
	return -1;
}

int X, Y;

void printS(int** mas, int r, int c, Color color)
{
	X = 20, Y = 25;
	setCursor(X, Y);
	for (size_t i = 0; i < r; i++)
	{
		setCursor(X, Y + i);
		for (size_t j = 0; j < c; j++)
		{
			if ((i == 0 || i == 1 || i == 3 || i == 4 || i == 6 || i == 7) || (i == 2 && j == 0) || (i == 5 && j == 2))
				setColor(color, color);
			else
				setColor(Black, Black);
			cout << mas[i][j];
		}
		cout << "\n";
	}
	setColor(White, Black);
}

void printI(int** mas, int r, int c, Color color)
{
	X = 65, Y = 25;
	setCursor(X, Y);
	for (size_t i = 0; i < r; i++)
	{
		setCursor(X, Y + i);
		for (size_t j = 0; j < c; j++)
		{
			if ((i == 0 || i == 7) || (j == 1))
				setColor(color, color);
			else
				setColor(Black, Black);
			cout << mas[i][j];
		}
		cout << "\n";
	}
	setColor(White, Black);
}

void setMasEnemies(int** mas, int r, int c)
{
	for (size_t i = 0; i < r; i++)
	{
		for (size_t j = 0; j < c; j++)
		{
			mas[i][j] = 6;
		}
	}
}

void printOneEnemy(int** mas, int r, int c, int x, int y, Color color)
{
	setCursor(x, y);
	for (size_t i = 0; i < r; i++)
	{
		setCursor(x, y + i);
		for (size_t j = 0; j < c; j++)
		{
			if ((i == 0 && (j == 2 || j == 8)) || (i == 1 && (j == 3 || j == 7)) || (i == 2 && (j > 1 && j < 9)))
				setColor(color, color);
			else if (i == 3 && ((j == 1 || j == 2) || (j == 4 || j == 5 || j == 6) || (j == 8 || j == 9)))
				setColor(color, color);
			else if (i == 4 || (i == 5 && ((j == 0) || (j > 1 && j < 9) || j == 10)) || i == 7 && (j == 6 || j == 7))
				setColor(color, color);
			else if ((i == 6 && ((j == 0) || j == 2 || j == 8 || j == 10)) || (i == 7 && (j == 3 || j == 4)))
				setColor(color, color);
			else
				setColor(Black, Black);
			cout << mas[i][j];
		}
		cout << "\n";
	}
	setColor(White, Black);
}

void printThreeEnemies(int** mas, int r, int c)
{
	X = 57, Y = 5;
	setCursor(X, Y);
	printOneEnemy(mas, r, c, X, Y, Green);

	X = 37, Y = 10;
	setCursor(X, Y);
	printOneEnemy(mas, r, c, X, Y, LightBlue);

	X = 77, Y = 10;
	setCursor(X, Y);
	printOneEnemy(mas, r, c, X, Y, Red);
}

void delMemory(int** mas, int row, int col)
{
	if (mas != nullptr)
	{
		for (size_t i = 0; i < row; i++)
		{
			delete[]mas[i];
		}
		delete[]mas;
	}
}

struct User
{
	string login;
	string pass;
	int score = 0;
};

int napravlenie = 0, key, coins = 0, index_buk = -1;
mutex mut;
COORD koord_hero{ 60,50 };
User user;
vector <User> users;
ifstream readF;
ofstream writeF;
string name = "user.txt";

void addCoins()
{
	int sizeF;
	users[index_buk].score = coins;
	writeF.open(name);
	writeF << users[0].login << " " << users[0].pass << " " << users[0].score << " ";
	for (size_t i = 1; i < users.size(); i++)
	{
		writeF << "\n" << users[i].login << " " << users[i].pass << " " << users[i].score << " ";
	}
	writeF.close();
}

void moveEnemies(Enemies bukahi, COORD koord, Hero geroi)
{
	int number, health = 5, kol = 0;
	COORD start_fire, end_fire;
	while (geroi.getHealth() && bukahi.getSize())
	{
		mut.lock();
		bukahi.showEnemis(koord);
		if (bukahi.getAliveSize() > 15)
			number = 20 + rand() % 10;
		else if (bukahi.getAliveSize() > 6 && bukahi.getSize() <= 15)
		{
			do
			{
				number = 10 + rand() % 10;
			} while (bukahi[number]->getAlive() == 0);
		}
		else if (bukahi.getAliveSize() <= 6)
		{
			do
			{
				number = rand() % 20;
			} while (bukahi[number]->getAlive() == 0);
		}
		int number_2, number_3;
		bool check_1, check_2, check;
		number_2 = number + 10;
		number_3 = number + 20;
		check = bukahi[number]->getAlive();
		if (number > 10 && number < 20)
		{
			check_1 = bukahi[number_2]->getAlive();
		}
		else if (number < 10)
		{
			check_1 = bukahi[number_2]->getAlive();
			check_2 = bukahi[number_3]->getAlive();
		}

		if (check)
		{
			if (number < 10 && !check_1 && !check_2)
			{
				start_fire.X = bukahi[number]->getX();
				start_fire.Y = bukahi[number]->getY();
				Fire fire('*', Red, start_fire);
				end_fire = fire.moveFire2(koord_hero.Y);
			}
			else if (number < 20)
			{
				if (check_1 != 1)
				{
					start_fire.X = bukahi[number]->getX();
					start_fire.Y = bukahi[number]->getY();
					Fire fire('*', Red, start_fire);
					end_fire = fire.moveFire2(koord_hero.Y);
				}
			}
			else if (number > 20 && number < 30)
			{
				start_fire.X = bukahi[number]->getX();
				start_fire.Y = bukahi[number]->getY();
				Fire fire('*', Red, start_fire);
				end_fire = fire.moveFire2(koord_hero.Y);
			}
		}
		vector < COORD> temp = geroi.KOORD();
		for (size_t i = 0; i < temp.size(); i++)
		{
			if (temp[i].X == end_fire.X)
			{
				health--;
				X = 40, Y = 1;
				setCursor(X, Y);
				setColor(Black, Black);
				if (health)
				{
					do
					{
						cout << "**";
						kol++;
					} while (kol != health + 1);
					X = 40, Y = 1;
					kol = 0;
					setCursor(X, Y);
					setColor(Red, Black);
					do
					{
						cout << char(3) << " ";
						kol++;
					} while (kol != health);
					kol = 0;
					setColor(White, Black);
					geroi.setHealth(health);
				}
				break;
			}
		}
		if (health && bukahi.getSize() != 0)
		{
			if (bukahi.getAliveSize() < 10)
				this_thread::sleep_for(chrono::milliseconds(500));
			else if (bukahi.getAliveSize() > 10 && bukahi.getAliveSize() < 20)
				this_thread::sleep_for(chrono::milliseconds(750));
			else
				this_thread::sleep_for(chrono::milliseconds(1000));
			bukahi.delEnemis(koord);
			mut.unlock();
			if (napravlenie % 2 == 0)
			{
				koord.X += 4;
			}
			else
			{
				koord.X -= 4;
			}
			if (koord.X > 50 || koord.X < 3)
			{
				napravlenie++;
				koord.Y += 4;
			}
			this_thread::sleep_for(chrono::milliseconds(500));
		}
		else
		{
			system("cls");
			setCursor(55, 30);
			setColor(Yellow, Black);
			cout << "GAME OVER!" << endl;
			if (bukahi.getSize())
			{
				setCursor(55, 31);
				cout << "You are looser!";
			}
			else
			{
				setCursor(55, 31);
				cout << "You are winner!";
			}
			setCursor(55, 32);
			cout << "Your coins: " << coins;
			break;
		}
	}
}

void moveHero(Hero geroi, Enemies bukahi, COORD koord_hero)
{
	COORD koord_fire{ 5,5 };
	int lowerY, index = -1;
	while (geroi.getHealth() && bukahi.getSize())
	{
		key = _getch();
		mut.lock();
		if (key == Space)
		{
			koord_fire.X = koord_hero.X + 2;
			koord_fire.Y = koord_hero.Y - 1;
			Fire fire('*', Red, koord_fire);
			lowerY = bukahi.getLowerY();
			if (lowerY > 40)
			{
				geroi.setHealth(0);
			}
			if (geroi.getHealth())
			{
				COORD new_koord = fire.moveFire(lowerY);
				index = -1;
				for (size_t i = 0; i < all_shut_coords.size(); i++)
				{
					if (all_shut_coords[i].X == new_koord.X && all_shut_coords[i].Y == lowerY)
						index = i / 15 + 1;
				}
				if (index != -1)
				{
					if (bukahi[index - 1]->getAlive())
					{
						if (bukahi[index - 1]->getColor() == Magenta)
						{
							bukahi[index - 1]->deadBuka();
							coins = coins + 100 * (bukahi[index - 1]->getHealth());
							if (coins % 500 == 0)
								addCoins();
						}
						else if (bukahi[index - 1]->getColor() == Green)
							bukahi[index - 1]->setColorColor(Yellow);
						else
							bukahi[index - 1]->setColorColor(Magenta);
					}
					else
					{
						lowerY -= 5;
						index = -1;
						for (size_t i = 0; i < all_shut_coords.size(); i++)
						{
							if (all_shut_coords[i].X == new_koord.X && all_shut_coords[i].Y == lowerY)
								index = i / 15 + 1;
						}
						if (bukahi[index - 1]->getAlive())
						{
							if (bukahi[index - 1]->getColor() == Magenta)
							{
								bukahi[index - 1]->deadBuka();
								coins = coins + 100 * (bukahi[index - 1]->getHealth());
								if (coins % 500 == 0)
									addCoins();
							}
							else if (bukahi[index - 1]->getColor() == Green)
								bukahi[index - 1]->setColorColor(Yellow);
							else
								bukahi[index - 1]->setColorColor(Magenta);
						}
						else
						{
							lowerY -= 5;
							index = -1;
							for (size_t i = 0; i < all_shut_coords.size(); i++)
							{
								if (all_shut_coords[i].X == new_koord.X && all_shut_coords[i].Y == lowerY)
									index = i / 15 + 1;
							}
							if (bukahi[index - 1]->getAlive())
							{
								if (bukahi[index - 1]->getColor() == Magenta)
								{
									bukahi[index - 1]->deadBuka();
									coins = coins + 100 * (bukahi[index - 1]->getHealth());
									if (coins % 500 == 0)
										addCoins();
								}
								else if (bukahi[index - 1]->getColor() == Green)
									bukahi[index - 1]->setColorColor(Yellow);
								else
									bukahi[index - 1]->setColorColor(Magenta);
							}
						}
					}
				}
			}
			else if (geroi.getHealth() == 0 || bukahi.getSize() == 0)
			{
				system("cls");
				setCursor(55, 30);
				setColor(Yellow, Black);
				cout << "GAME OVER!" << endl;
				if (bukahi.getSize())
				{
					setCursor(55, 31);
					cout << "You are looser!";
				}
				else
				{
					setCursor(55, 31);
					cout << "You are winner!";
				}
				setCursor(55, 32);
				cout << "Your coins: " << coins;
				break;
			}
		}
		else if (key == Left)
		{
			if (koord_hero.X > 1)
			{
				geroi.delEnemy(koord_hero);
				koord_hero.X -= 1;
				geroi.showEnemy(koord_hero);
			}
		}
		else if (key == Right)
		{
			if (koord_hero.X < 143)
			{
				geroi.delEnemy(koord_hero);
				koord_hero.X += 1;
				geroi.showEnemy(koord_hero);
			}
		}
		X = 120, Y = 1;
		setCursor(X, Y);
		setColor(Black, Black);
		cout << "SCORE: " << coins;
		X = 120, Y = 1;
		setCursor(X, Y);
		setColor(White, Black);
		cout << "SCORE: " << coins;
		setColor(White, Black);
		mut.unlock();
	}
}

bool isBigger(User user1, User user2)
{
	return user1.score > user2.score;
}

void main()
{
	int key, index, sizeF = 0, row = 8, col = 3, col_2 = 11, kol = 0, kol_serdce = 0, hight_score = 0, kol_users;
	int** mas = new int* [row], ** mas_2 = new int* [row];
	bool isCorrect = true;
	COORD koord_enemy{ 2,10 };
	Hero geroi;
	Enemies bukahi;
	vector <string> menu{ "Registration" , "Autorization", "Leaders table", "Exit" };
	User temp_user;
	Color color_zas;

	for (size_t i = 0; i < row; i++)
	{
		mas[i] = new int[col];
	}

	for (size_t i = 0; i < row; i++)
	{
		mas_2[i] = new int[col_2];
	}

	setMasEnemies(mas_2, row, col_2);

	printThreeEnemies(mas_2, row, col_2);

	X = 37, Y = 41;
	setCursor(X, Y);
	printOneEnemy(mas_2, row, col_2, X, Y, Magenta);

	X = 77, Y = 41;
	setCursor(X, Y);
	printOneEnemy(mas_2, row, col_2, X, Y, Yellow);

	X = 57, Y = 46;
	setCursor(X, Y);
	printOneEnemy(mas_2, row, col_2, X, Y, LightGreen);

	X = 52, Y = 32;
	setCursor(X, Y);
	cout << "p a c e";
	X = 97;
	setCursor(X, Y);
	cout << "n v a d e r s ";

	do
	{
		color_zas = Color(1 + rand() % 14);
		printS(mas, row, col, color_zas);
		printI(mas, row, col, color_zas);
		Sleep(250);
		kol++;
	} while (kol < 5);

	system("cls");

	printThreeEnemies(mas_2, row, col_2);
	X = 57, Y = 17;

	readF.open(name);
	if (readF.is_open())
	{
		while (!readF.eof())
		{
			readF >> temp_user.login >> temp_user.pass >> temp_user.score;
			users.push_back(temp_user);
		}
	}
	readF.close();

	if (users.size() > 1)
	{
		sort(users.begin(), users.end(), isBigger);
		hight_score = users[0].score;
	}
	else if (users.size() == 0)
	{
		hight_score = 0;
	}

	do
	{
		X = 57, Y = 17;
		index = choiseMenu(menu, X, Y);
		system("cls");
		printThreeEnemies(mas_2, row, col_2);
		setCursor(X, Y);
		setColor(White, Magenta);
		X = 57, Y = 17;
		setCursor(X, Y);
		cout << menu[index];
		setColor(Magenta, Black);
		kol_users = 0;
		if (index == 2)
		{
			X = 50, Y = 19;
			for (auto var : users)
			{
				setCursor(X, Y);
				cout << var.login << ": " << var.score;
				kol_users++;
				if (kol_users == 5)
					break;
				Y++;
			}
			Sleep(5000);
			X = 50, Y = 19;
			setColor(Black, Black);
			for (size_t i = 0; i < kol_users; i++)
			{
				setCursor(X, Y);
				cout << users[i].login << ": " << users[i].score;
				Y++;
			}
			setColor(White, Black);
		}
	} while (index == 2);

	if (index != 3)
	{
		if (index == 0)
		{
			// регистрация
			readF.open(name);
			X -= 5, Y += 2;
			do
			{
				if (!isCorrect)
				{
					readF.open(name);
				}
				isCorrect = true;
				setCursor(X, Y);
				cout << "Your login: ";
				cin >> user.login;
				if (readF.is_open())
				{
					while (!readF.eof())
					{
						readF >> temp_user.login >> temp_user.pass >> temp_user.score;
						if (temp_user.login.compare(user.login) == 0)
						{
							Y++;
							setCursor(X, Y);
							cout << "Login is busy!";
							isCorrect = false;
							break;
						}
					}
					readF.close();
				}
				Y++;
			} while (!isCorrect);
			if (isCorrect)
			{
				writeF.open(name, ios::app);
				setCursor(X, Y);
				cout << "Your password: ";
				cin >> user.pass;
				if (!readF.is_open())
				{
					readF.open("user.txt");
				}
				readF.seekg(0, ios::end);
				sizeF = readF.tellg();
				if (sizeF > 0)
				{
					writeF << "\n";
				}
				writeF << user.login << " " << user.pass << " " << user.score;
				writeF.close();
				Y++;
				setCursor(X, Y);
				cout << "Registration is correct!";
			}
		}
		else if (index == 1)
		{
			// авторизация
			X -= 5, Y += 1;
			do
			{
				readF.open(name);
				if (readF.is_open())
				{
					Y++;
					setCursor(X, Y);
					cout << "Your login: ";
					cin >> user.login;
					while (!readF.eof())
					{
						isCorrect = false;
						readF >> temp_user.login >> temp_user.pass >> temp_user.score;
						if (temp_user.login.compare(user.login) == 0)
						{
							Y++;
							setCursor(X, Y);
							cout << "Login is found!";
							isCorrect = true;
							break;
						}
					}
					readF.close();
					Y++;
					setCursor(X, Y);
					if (isCorrect)
					{
						cout << "Your password: ";
						cin >> user.pass;
						Y++;
						setCursor(X, Y);
						if (temp_user.pass.compare(user.pass) == 0)
						{
							cout << "Autorization is correct!";
						}
						else
						{
							cout << "Password is false!";
							isCorrect = false;
							if (!isCorrect)
							{
								Y++;
								setCursor(X, Y);
								cout << "Your new login: ";
								cin >> user.login;
								if (readF.is_open())
								{
									while (!readF.eof())
									{
										readF >> temp_user.login >> temp_user.pass >> temp_user.score;
										if (temp_user.login.compare(user.login) == 0)
										{
											Y++;
											setCursor(X, Y);
											cout << "Login is busy!";
											isCorrect = true;
											break;
										}
									}
									readF.close();
								}
							} while (isCorrect);
							if (!isCorrect)
							{
								Y++;
								setCursor(X, Y);
								writeF.open(name, ios::app);
								cout << "Your password: ";
								cin >> user.pass;
								writeF << user.login << " " << user.pass << " " << user.score;
								writeF.close();
								Y++;
								setCursor(X, Y);
								cout << "Registration is correct!";
								isCorrect = true;
								readF.close();
							}
						}
					}
					else
					{
						setCursor(X, Y);
						cout << "Login is not found!";
						isCorrect = true;
						readF.open("user.txt");
						do
						{
							if (!isCorrect)
							{
								readF.open(name);
							}
							isCorrect = true;
							Y++;
							setCursor(X, Y);
							cout << "Your new login: ";
							cin >> user.login;
							if (readF.is_open())
							{
								while (!readF.eof())
								{
									readF >> temp_user.login >> temp_user.pass >> temp_user.score;
									if (temp_user.login.compare(user.login) == 0)
									{
										Y++;
										setCursor(X, Y);
										cout << "Login is busy!";
										isCorrect = false;
										break;
									}
								}
								readF.close();
							}
						} while (!isCorrect);
						if (isCorrect)
						{
							Y++;
							setCursor(X, Y);
							writeF.open(name, ios::app);
							cout << "Your password: ";
							cin >> user.pass;
							writeF << user.login << " " << user.pass << " " << user.score;
							writeF.close();
							Y++;
							setCursor(X, Y);
							cout << "Registration is correct!";
							readF.close();
						}
					}
				}
				else
				{
					readF.close();
					writeF.open(name, ios::app);
					Y++;
					setCursor(X, Y);
					cout << "Your login: ";
					cin >> user.login;
					Y++;
					setCursor(X, Y);
					cout << "Your password: ";
					cin >> user.pass;
					writeF << user.login << " " << user.pass << " " << user.score;
					writeF.close();
					Y++;
					setCursor(X, Y);
					cout << "Registration is correct!";
				}
			} while (!isCorrect);
		}
		if (readF.is_open())
		{
			while (!readF.eof())
			{
				readF >> temp_user.login >> temp_user.pass >> temp_user.score;
				users.push_back(temp_user);
			}
		}
		for (size_t i = 0; i < users.size(); i++)
		{
			if (users[i].login == user.login)
			{
				index_buk = i;
				break;
			}
		}

		Sleep(2500);
		system("cls");
		delMemory(mas, row, col);
		delMemory(mas_2, row, col_2);
		system("cls");
		geroi.showEnemy(koord_hero);
		X = 3, Y = 1;
		setCursor(X, Y);
		cout << "SPACE INVAIDERS";
		X = 40, Y = 1;
		setCursor(X, Y);
		setColor(Red, Black);
		do
		{
			cout << char(3) << " ";
			kol_serdce++;
		} while (kol_serdce != 5);
		X = 80, Y = 1;
		setCursor(X, Y);
		setColor(White, Black);
		cout << "HIGHT  SCORE: ";
		cout << hight_score;
		X = 120, Y = 1;
		setCursor(X, Y);
		cout << "SCORE: " << coins;

		thread th(moveEnemies, bukahi, koord_enemy, geroi);
		thread th2(moveHero, geroi, bukahi, koord_hero);

		th.join();
		th2.join();

		system("pause>>NULL");
	}
	else
	{
		system("cls");
		setCursor(30, 30);
		setColor(Magenta, Black);
		cout << "See you later!\n";
	}
}
