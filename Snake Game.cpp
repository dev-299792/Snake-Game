#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <thread>

struct point
{
	int x;
	int y;
};

const char block = (char)219;		//for border 

void gotoxy( int x, int y )
{
	x=x*2;
	COORD p = { (short)x, (short)y };
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), p );
}

enum direction
{
	UP,DOWN,LEFT,RIGHT
};
//for random number generation
const unsigned int seed = time(0);
std::mt19937_64 rng(seed);

class mod_queue
{
	std::vector<point> queue_elements = std::vector<point>(1000);
	int front,end,size;
public:
	int get_size()
	{
		return size;
	}

	point get_front()
	{
		point p{-1,-1};
		if(size!=0)
			return queue_elements[front];
		else
			return (p);
	}

	mod_queue()
	{
		front = end = -1;
		size = 0;
	}

	void add(point p)
	{
		if (size<1000)
		{
			if (front==end && end==-1)
			{
				front = end = 0;
				
			}
			else 
			{
				(end==(1000-1)) ? (end = 0) : (end++);
			}
			queue_elements[end].x = p.x;
			queue_elements[end].y = p.y;
			size++;
		}
		else
			std::cout<<"in add queue";
	}

	void delete_element()
	{
		if(size == 1)
		{
			front = end = -1;
			--size;
		}
		else if(size > 1)
		{
			if (front==1000-1)
			{
				front = 0;
			}
			else
				++front;
			--size;
		}
		else
			std::cout<<"in delete_element queue underflow";
	}

	bool point_on_line_segment(point to_check,point p1,point p2)
	{
		if (p1.x == to_check.x && p2.x == to_check.x)
		{
			if(p1.y >= to_check.y && to_check.y >= p2.y||p2.y >= to_check.y && to_check.y >= p1.y)
			{
				return true;
			}
		}
		else if(p1.y == to_check.y && p2.y == to_check.y )
		{
			if(p1.x >= to_check.x && to_check.x>= p2.x||p2.x >= to_check.x && to_check.x >= p1.x)
			{
				return true;
			}
		}
		return false;
	}	

	bool point_on_curve(point to_check,point head,point tail)
	{
		if(size == 0)
		{
			if(point_on_line_segment(to_check,head,tail))
				return true;
		}
		else
		{
			if(point_on_line_segment(to_check,tail,queue_elements[front]))
				return true;
			for (int j=front; j!=end; j=(j+1)%1000)
			{
				if(point_on_line_segment(to_check,queue_elements[j],queue_elements[j+1]))
					return true;
			}
			if(point_on_line_segment(to_check,head,queue_elements[end]))
				return true;
		}
		return false;
	}

	bool head_on_body(point head,point tail)
	{
		if(head.x == tail.x && head.y == tail.y)
		{
			return false;
		}
		if(size == 0)
		{
			return false;
		}
		if(point_on_line_segment(head,tail,queue_elements[front]))
		{
			return true;
		}
		for (int j=front; j!=end; j=(j+1)%1000)
		{
			if(point_on_line_segment(head,queue_elements[j],queue_elements[j+1]))
				return true;
		}
		return false;
	}

	bool front_tail_coincides(point tail)
	{
		if(size!=0)
		{
			if(tail.x == queue_elements[front].x && tail.y == queue_elements[front].y)
				return true;
		}
		return false;
	}
};

class snake_game
{
private:
	point food,snake_head,snake_tail;
	mod_queue snake_body;
	int X=5,Y=5,count = 0;
	bool live = true;

	int my_random(int low,int high)
	{
		std::uniform_int_distribution<int> uni(low, high);		
		return uni(rng);
	}

public:
	direction snake_direction = RIGHT,tail_direction=RIGHT;

	snake_game()
	{
		snake_head.x = X+20;
		snake_head.y = Y+10;
		snake_tail.x = X+20-4;
		snake_tail.y = Y+10;
		food.x = my_random(X+1,X+40-1);
		food.y = my_random(Y+1,Y+20-1);
	}

	void start()
	{
		display(X,Y);
	}

	void new_food()
	{
		while(snake_body.point_on_curve(food,snake_head,snake_tail))
		{
			food.x = my_random(X+1,X+40-1);
			food.y = my_random(Y+1,Y+20-1);
		}
		gotoxy(food.x,food.y);
		std::cout<<"@";
	}

	void display(int x,int y)
	{

		//display boundry
		for(int i=0;i<40;i++)
		{
			gotoxy(i+x,y);
			std::cout<<block<<block;
		}
		for (int i = 0; i < 20; ++i)
		{
			gotoxy(x,i+y+1);
			std::cout<<block<<block;	
		}	
		for (int i = 0; i < 21; ++i)
		{
			gotoxy(40+x,i+y);
			std::cout<<block<<block;	
		}	
		for (int i = 0; i < 40; ++i)
		{
			gotoxy(x+i,20+y);
			std::cout<<block<<block;	
		}
		//display snake
		gotoxy(snake_head.x,snake_head.y);
		std::cout<<"H";
		gotoxy(snake_tail.x,snake_tail.y);
		std::cout<<"B B B B ";
		//display food
		new_food();
	}

	bool is_live()
	{
		return live;
	}

	direction my_snake_direction()
	{
		return snake_direction;
	}

	bool isperpendicular(direction a,direction b)
	{
		if(a == UP||a==DOWN)
		{
			if (b==RIGHT||b==LEFT)
			{
				return true;
			}
		}
		if(a==RIGHT||a==LEFT)
		{
			if (b == UP||b==DOWN)
			{
				return true;
			}
		}
		return false;
	}

	bool isopposite(direction a,direction b)
	{
		if(a==UP&&b==DOWN||a==RIGHT&&b==LEFT||b==UP&&a==DOWN||b==RIGHT&&a==LEFT)
		{
			return true;
		}
		return false;
	}

	int move(direction d)
	{
		bool food_eaten = false;		
		if( isperpendicular(snake_direction,d))
		{
			snake_body.add(snake_head);
		}
		if(isopposite(snake_direction,d))
		{
			return 0;
		}
		gotoxy(snake_head.x,snake_head.y);
		std::cout<<'B';
		switch(d)
		{
			case UP:
			snake_head.y--;
			break;
			case DOWN:
			snake_head.y++;
			break;
			case RIGHT:
			snake_head.x++;
			break;
			case LEFT:
			snake_head.x--;
			break;
		}
		gotoxy(snake_head.x,snake_head.y);
		std::cout<<'H';
		if(head_on_boundry()||snake_body.head_on_body(snake_head,snake_tail))
		{
			live = false;
			return 1;
		}
		if(food.x == snake_head.x && food.y == snake_head.y)
		{
			count++;
			new_food();
			gotoxy(6,2);
			std::cout<<"SCORE : "<<count*10;
			std::cout<<'\a';
			food_eaten = true;
		}
		if(!food_eaten)
		{
			gotoxy(snake_tail.x,snake_tail.y);
			std::cout<<' ';

			switch(tail_direction)
			{ 
				case UP:
				snake_tail.y--;
				break;
				case DOWN:
				snake_tail.y++;
				break;
				case RIGHT:
				snake_tail.x++;
				break;
				case LEFT:
				snake_tail.x--;
				break;
			}

			point front_q = snake_body.get_front();
			if(snake_body.front_tail_coincides(snake_tail))
			{
				snake_body.delete_element();
			}
			front_q = snake_body.get_front();
			if(snake_body.get_size() == 0)
			{
				tail_direction = snake_direction;
			}
			else
			{
				if(front_q.x == snake_tail.x)
				{
					if(front_q.y>snake_tail.y)
					{
						tail_direction = DOWN;
					}
					else
						tail_direction = UP;
				}
				else if(front_q.y == snake_tail.y)
				{
					if (front_q.x>snake_tail.x)
					{
						tail_direction = RIGHT;
					}
					else
						tail_direction = LEFT;
				}
				else
				{	
					std::cout<<"move 1";
					system("pause");
				}
			}
		}
		snake_direction = d;
		return 1;
	}

	bool head_on_boundry()
	{
		if (snake_head.x==5||snake_head.x==45)
		{
			return true;
		}
		else if (snake_head.y==5||snake_head.y==25)
		{
			return true;
		}
		return false;
	}
};

int main(void)
{
	system("color e1");
	system("title SNAKE GAME");
	int millisec=200,choice;
	do{
		system("cls");
		std::cout<<" DIFFICULTY LEVEL\n1.EASY\n2.INTERMEDIATE\n3.HARD\n\n BY DEFAULT INTERMEDIATE\n\nENTER DIFFICULTY LEVEL :";
		std::cin>>choice;
		if(choice==1)
			millisec = 300;
		if(choice==3)
			millisec = 100;
		system("cls");

		snake_game s;
		s.start();
		char ch;
		int i=0;
		gotoxy(6,2);
		std::cout<<"SCORE : 0";
		while( s.is_live() )
		{
			int j;
			std::this_thread::sleep_for (std::chrono::milliseconds(millisec));
			if (kbhit())
			{
				ch = getch();
				if(ch==-32)
					ch = getch();
				if(ch==72||ch==75||ch==77||ch==80)
					switch(ch)
				{
					case 72:					//up
					j=s.move(direction::UP); i++;
					break;
					case 75:					//left
					j=s.move(direction::LEFT);	i++;
					break;
					case 77:					//right
					j=s.move(direction::RIGHT);	i++;
					break;
					case 80:					//down
					j=s.move(direction::DOWN);	i++;
					break;
				}	
			}
			else
				j=0;
			if(j==0)
				s.move(s.my_snake_direction());
		}
		std::cin.ignore();
		system("cls");
		gotoxy(0,1);
		std::cout<<"Game Over";
		getch();
		std::cout<<"\n\nLets play again !!\n1.YES\n2.NO\nEnter Your Choice(1 or 2): ";
		std::cin>>choice;
	}while(choice==1);
	return 0;
}