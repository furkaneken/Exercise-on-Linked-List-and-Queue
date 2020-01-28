#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include"strutils.h"
using namespace std;

struct LectureInfo
{
	string name;
	int capacity;
	LectureInfo *next;

	LectureInfo()
	{
		name = "";
		capacity = 0;
		next = nullptr;
	}
};

class LinkedList
{
private:
	LectureInfo *head;
	LectureInfo *tail;
public:
	LinkedList() {head = tail = nullptr;}

	~LinkedList()
	{
		tail = nullptr;
		while (head != nullptr)
		{
			LectureInfo *tmp = head->next;
			delete head;
			head = head->next;
		}
	}

	void Add(LectureInfo *add)
	{
		if (head == nullptr)
		{
			head = tail = add;
		}
		else
		{
			tail->next = add;
			tail = tail->next;
		}
	}

	LectureInfo* GetHead() {return head;}
};

struct LectureNode
{
	string lectureName;
	LectureNode *next;

	LectureNode(string name = "", LectureNode* ptr = nullptr)
	{
		lectureName = name;
		next = ptr;
	}
};

struct StudentNode
{
	string name;
	int id;
	LectureNode *lectures;
	StudentNode *next;


	StudentNode()
	{
		name = "";
		id = 0;
		lectures = nullptr;
		next = nullptr;
	}

	StudentNode(string n, int i, LectureNode *ptr = nullptr)
	{
		name = n;
		id = i;
		lectures = ptr;
	}

};

struct XMLTag
{
	string tagname;
	XMLTag *next;

	XMLTag(string t = "") : tagname(t) {next = nullptr;}
};

class XMLStack
{
	private:
		XMLTag *top;
	public:

		XMLStack()
		{
			top = nullptr;
		}

		~XMLStack()
		{
			while (top)
			{
				XMLTag *temp = top->next;
				delete top;
				top = temp;
			}
		}

		void AddTag(string s)
		{
			if (top == nullptr)
			{
				top = new XMLTag (s);
			}
			else
			{
				XMLTag *temp = new XMLTag (s);
				temp->next = top;
				top = temp;
			}
		}

		void RemoveTag()
		{
			XMLTag *temp = top->next;
			delete top;
			top = temp;
		}

		bool isEmpty()
		{
			if(top)
				return false;
			else
				return true;
		}

		XMLTag* GetTop()
		{
			return top;
		}
};

class StudentQueue
{
	private:
		StudentNode *front;
		StudentNode *rear;
	public:
		StudentQueue()
		{
			front = nullptr;
			rear = nullptr;
		}

		~StudentQueue()
		{
			while (front)
				Delete();
		}

		//void Enqueue(const string & s,const int & i) //used while parsing the xml noooooo
		//{
		//	if (front == nullptr)
		//	{
		//		front = rear = new StudentNode(s, i);
		//	}
		//	else
		//	{
		//		rear->next = new StudentNode(s, i);
		//		rear = rear->next;
		//	}
		//}

		void Enqueue(StudentNode *& add) //used in fucking everywhere, hopefully...
		{
			if (front == nullptr)
			{
				front = rear = add;
			}
			else
			{
				rear->next = add;
				rear = rear->next;
			}
		}

		StudentNode* Dequeue() //dequeue function will create a copy of the object and returns the pointer, then deletes the original from the queue
		{
			if (front)
			{
				StudentNode *removed = new StudentNode(front->name, front->id);
				LectureNode* flec = front->lectures;
				removed->lectures = new LectureNode(flec->lectureName);
				LectureNode* rlec = removed->lectures;
				while (flec->next) //while there is a lecture to copy
				{
					rlec->next = new LectureNode(flec->next->lectureName);
					flec = flec->next;
					rlec = rlec->next;
				}
				StudentNode *temp = front->next;
				delete front;
				front = temp;
				return removed;
			}
			else
				return nullptr;
		}

		void Delete() //a function other than dequeue (this one) will be used in the destruction of the object or just the deletion of the member, this doesn't create a returning object
		{
			bool final = OneElement();
			StudentNode *temp = front->next;
			delete front;
			front = temp;
			if (final)
				front = rear = nullptr;

		}

		bool isEmpty()
		{
			if(front)
				return false;
			else
				return true;
		}

		StudentNode* GetFront()
		{
			return front;
		}

		void DeleteCourse() //this function will remove the first course from the front students list
		{
			LectureNode *temp = front->lectures->next;
			delete front->lectures;
			front->lectures = temp;
		}

		bool OneElement()
		{
			return front == rear;
		}
};

bool OpenTag(fstream & input, char & c, XMLStack & tags) //returns true if tag is opened, returns false if the tag found is closing tag
{
	string tagn = "";
	while (c == '\t' || c == '\n' || c == '<')
	{
		c = input.get();
		if (input.eof())
			return false;
	}
	while (c != '>')
	{
		if (c == '/') return false;
		tagn += c;
		c = input.get();
	}
	c = input.get();
	tags.AddTag(tagn);
	return true;
}

void CloseTag(fstream & input, char & c, XMLStack & tags) //compares the tag with the top of the stack, if it matches removes tag from the stack, otherwise stops execution
{
	if (c == '<')
		c = input.get();
	c = input.get();
	string tagn = "";
	while (c != '>')
	{
		tagn += c;
		c = input.get();
	}
	if (tags.GetTop()->tagname != tagn)
	{
		cout << "Invalid XML format!.. Exiting." << endl;
		exit(1);
	}
	tags.RemoveTag();
	c = input.get();
}

void S_Write(fstream & input, char & c, XMLStack & tags, StudentQueue & queue)
{
	bool first = true;
	StudentNode *stu = new StudentNode;
	stu->lectures = new LectureNode;
	LectureNode *lec = stu->lectures;
	while (OpenTag(input, c, tags)) //as long as there are tags ( i.e. tags can be opened)
	{
		if (tags.GetTop()->tagname == "name")
		{
			string _name = "";
			while (c != '<')
			{
				_name += c;
				c = input.get();
			}
			stu->name = _name;
			CloseTag(input, c, tags);
		}
		else if (tags.GetTop()->tagname == "id")
		{
			string _id = "";
			while (c != '<')
			{
				_id += c;
				c = input.get();
			}
			stu->id = atoi(_id);
			CloseTag(input, c, tags);
		}
		else if (tags.GetTop()->tagname == "lecture")
		{
			string _lecname = "";
			while (c != '<')
			{
				_lecname += c;
				c = input.get();
			}
			if (first) //modify the first node
			{
				stu->lectures->lectureName = _lecname; //earlier default construction
				first = false;
			}
			else
			{
				lec->next = new LectureNode(_lecname);
				lec = lec->next;
			}
			CloseTag(input, c, tags);
		}
	}
	queue.Enqueue(stu);
	CloseTag(input, c, tags); //closes the student tag
}

void L_Write(fstream & input, char & c, XMLStack & tags, LinkedList & lectures)
{
	LectureInfo *lec = new LectureInfo;
	while (OpenTag(input, c, tags)) //as long as there are tags ( i.e. tags can be opened)
	{
		if (tags.GetTop()->tagname == "name")
		{
			string _name = "";
			while (c != '<')
			{
				_name += c;
				c = input.get();
			}
			lec->name = _name;
			CloseTag(input, c, tags);
		}
		else if (tags.GetTop()->tagname == "capacity")
		{
			string _capacity = "";
			while (c != '<')
			{
				_capacity += c;
				c = input.get();
			}
			lec->capacity = atoi(_capacity);
			CloseTag(input, c, tags);
		}
	}
	lectures.Add(lec);
	CloseTag(input, c, tags); //closes the lecture tag
}

int main()
{
	fstream s_input; //file stream that holds the student list
	fstream l_input; //file stream that holds the lectures list
	string stufile;
	do
	{
		cout << "Please enter the name of the Student XML file: ";
		cin >> stufile;
		s_input.open(stufile.c_str());
		if (s_input.fail())
			cout << "Invalid file" << endl;
	} while (s_input.fail());

	char c;
	c = s_input.get();
	c = s_input.get();
	XMLStack s_tags;
	StudentQueue queue;
	while(!s_input.eof())
	{
		if (s_tags.GetTop() == nullptr) //if there are no active tags
		{
			OpenTag(s_input, c, s_tags); //this call opens "student" tag only
		}
		else if (s_tags.GetTop()->tagname == "student")
		{
			S_Write(s_input, c, s_tags, queue); //parse info for one student
		}
	}
	string lecfile;
	XMLStack l_tags;
	
	do
	{
		cout << "Please enter the name of the Lectures XML file: ";
		cin >> lecfile;
		l_input.open(lecfile.c_str());
		if (l_input.fail())
			cout << "Invalid file" << endl;
	} while (l_input.fail());

	LinkedList lectures;
	c = l_input.get();
	c = l_input.get();

	while(!l_input.eof())
	{
		if (l_tags.GetTop() == nullptr) //if there are no active tags
		{
			OpenTag(l_input, c, l_tags); //this call opens "lecture" tag only
		}
		else if (l_tags.GetTop()->tagname == "lecture")
		{
			L_Write(l_input, c, l_tags, lectures); //parse info for one lecture
		}
	}

	//here begins the round robin implementation
	while (!queue.isEmpty())
	{
		LectureInfo * lec = lectures.GetHead();
		while (lec != nullptr)
		{
			if (queue.GetFront()->lectures->lectureName == lec->name)
			{
				if (lec->capacity > 0)
				{
					lec->capacity--;
					cout << queue.GetFront()->lectures->lectureName << " is assigned to " << queue.GetFront()->name << "(" << queue.GetFront()->id << ")" << endl;
					//LectureNode* lect = queue.GetFront()->lectures->next;
					queue.DeleteCourse();
					break;
				}
				else
				{
					cout << queue.GetFront()->lectures->lectureName << " can not be assigned to " << queue.GetFront()->name << "(" << queue.GetFront()->id << ")" << endl;
					queue.DeleteCourse();
					break;
				}
			}
			lec = lec->next;
		}
		if (lec == nullptr)
		{
			cout << queue.GetFront()->lectures->lectureName << " can not be assigned to " << queue.GetFront()->name << "(" << queue.GetFront()->id << ")" << endl;
			queue.DeleteCourse();
		}
		
		if (queue.GetFront()->lectures == nullptr) //if the student has no desired courses left
		{
			queue.Delete(); //remove him from the queue completely
		}
		else
		{
			if (!queue.OneElement())
			{
				StudentNode *temp = queue.Dequeue(); //otherwise, put the student at the end of the queue
				queue.Enqueue(temp);
			}
		}
	}






	

	return 0;
}