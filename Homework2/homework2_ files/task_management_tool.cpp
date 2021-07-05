/*  
	@Author
	Student Name: Abdulkadir Pazar
	Student Number: 150180028
	Date: 09/11/2019
*/
/*
	PLEASE, DO NOT CHANGE void display(bool verbose, bool testing), int getUsableDay() and int getUsableTime() FUNCTIONS.
	YOU HAVE TO WRITE THE REQUIRED  FUNCTIONS THAT IS MENTIONED ABOVE. YOU CAN ADD NEW FUNCTIONS IF YOU NEED.
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <iomanip>

#include "task_management_tool.h"

using namespace std; 


void WorkPlan::display(bool verbose, bool testing)
{
	string inone = "***";
	if (head != NULL)
	{
		Task *pivot = new Task;
		Task *compeer = new Task;

		pivot = head;
		do
		{
			if (testing)
				inone += " ";
			else
				cout << pivot->day << ". DAY" << endl;
			compeer = pivot;
			while (compeer != NULL)
			{
				string PREV = compeer->previous != NULL ? compeer->previous->name : "NULL";
				string NEXT = compeer->next != NULL ? compeer->next->name : "NULL";
				string CONT = compeer->counterpart != NULL ? compeer->counterpart->name : "NULL";
				if (testing)
					inone += compeer->name;
				else
					if (verbose)
						cout << "\t" << setw(2) << compeer->time << ":00\t" << PREV << "\t<- " << compeer->name << "(" << compeer->priority << ")->\t" << NEXT << "\t |_" << CONT << endl;
					else
						cout << "\t" << setw(2) << compeer->time << ":00\t" << compeer->name << "(" << compeer->priority << ")" << endl;
				compeer = compeer->counterpart;
			}
			pivot = pivot->next;
		} while (pivot != head);
		if (testing)
		{
			cout << inone << endl;
			cout << "(checking cycled list:";
			if (checkCycledList())
				cout << " PASS)" << endl;
			else
				cout << " FAIL)" << endl;
		}
	}
	else
		cout << "There is no task yet!" << endl;
}

int WorkPlan::getUsableDay()
{
	return usable_day;
}

int WorkPlan::getUsableTime()
{
	return usable_time;
}

void WorkPlan::create()
{
	head = NULL;
}

void WorkPlan::close()
{
	Task *temp = head;
	Task *traversal;
	Task *ftod;
	
	do {
		ftod = temp;
		temp = temp->next;
		while (ftod){
			traversal = ftod;
			ftod = ftod->counterpart;
			remove(traversal);
		}
		temp = head;
	} while(temp != head);
}

void WorkPlan::copyTaskFromFile(Task* task, Task *new_task)
{
	new_task->name = new char[strlen(task->name) + 1];
	strcpy(new_task->name, task->name);
	new_task->day = task->day;
	new_task->time = task->time;
	new_task->priority = task->priority;
	new_task->next = NULL;
	new_task->previous = NULL;
	new_task->counterpart = NULL;
}

void WorkPlan::add(Task *task)
{
	Task *new_task = new Task;
	copyTaskFromFile(task, new_task);

	Task *temp = head;
	Task *tail = temp;

	//creating the first node
	if(head == NULL){
		head = new_task;
		new_task->next = new_task;
		new_task->previous = new_task;
		return;
	}
	//moving temp to the place before the new task with tail following
	while(new_task->day > temp->day){
		tail = temp;
		temp = temp->next;
		//if task we want to add is bigger than all days move temp back by one
		if(head->previous->day < new_task->day){
			temp = head->previous;
			break;
		}
	}
	//add to head of list
	if(new_task->day < head->day){
		//if head points to itself (there is one day)
		if(head->next->day == head->day){
			new_task->next = head;
			new_task->previous = head;
			head->previous = new_task;
			head->next = new_task;
			head = new_task;
			head->counterpart = NULL;
		} //there is more than one day 
		else{
			new_task->next = head;
			new_task->previous = head->previous;
			head->previous->next = new_task;
			head->previous = new_task;
			head = new_task;
			head->counterpart = NULL;
		}
		return;
	}
	//if day we want to add a task to exists
	if(temp->day == new_task->day){
		//if we need to add a task to a time before an existing task 
		if(temp->time > new_task->time){
			//if there is only one day
			if(head->next->day == head->day){
				new_task->counterpart = temp;
				head = new_task;
				head->previous = head;
				head->next = head;
				temp->next = NULL;
				temp->previous = NULL;
				temp->counterpart = NULL;

			}
			//if we need to add a task to the day head points to
			else if(temp->day == head->day){
				new_task->counterpart = temp;
				new_task->previous = temp->previous;;
				new_task->next = temp->next;
				temp->previous->next = new_task;
				temp->next->previous = new_task;
				temp->next = NULL;
				temp->previous = NULL;
				temp->counterpart = NULL;
				head = new_task;
			}
			else{
				new_task->counterpart = temp;
				new_task->previous = temp->previous;;
				new_task->next = temp->next;
				temp->previous->next = new_task;
				temp->next->previous = new_task;
				temp->next = NULL;
				temp->previous = NULL;
			}
		}
		//if we want to add a task to a time with existing task
		else if(temp->time == new_task->time){
			//if new task has higher priority (temp has to be delayed)
			if(new_task->priority > temp->priority){
				new_task->next = temp->next;
				new_task->previous = temp->previous;
				temp->previous->next = new_task;
				temp->next->previous = new_task;
				new_task->counterpart = temp->counterpart;
				if(temp == head){
					head = new_task;
				}
				checkAvailableNextTimesFor(temp);
				temp->day = getUsableDay();
				temp->time = getUsableTime();
				//we call add task on temp to move it to its new place recursively
				return add(temp);
			}
			//new task has to be delayed
			else{
				new_task->next = temp->next;
				new_task->previous = temp->previous;
				new_task->counterpart = temp->counterpart;
				checkAvailableNextTimesFor(new_task);
				new_task->day = getUsableDay();
				new_task->time = getUsableTime();
				//we call add task on new_task to move it to its new place recursively
				return add(new_task);
			}
		}
		//if the task we want to add is later
		else if(temp->time < new_task->time){
			//temp moves within a day until it finishes the day or finds a suitable place for the new task
			//temp points to the last task of the day if there are no empty slots
			while(temp->counterpart && new_task->time > temp->time){
				tail = temp;
				temp = temp->counterpart;
			}
			//if new task conflicts with a task in the day resolve the conflict by checking priority
			if(temp->time == new_task->time){
				if(day_delayed){
					checkAvailableNextTimesFor(new_task);
					new_task->day = getUsableDay();
					new_task->time = getUsableTime();
					return add(new_task);
				}
				if(new_task->priority > temp->priority){
					tail->counterpart = new_task;
					new_task->counterpart = temp->counterpart;
					new_task->next = NULL;
					new_task->previous = NULL;
					checkAvailableNextTimesFor(temp);
					temp->day = getUsableDay();
					temp->time = getUsableTime();
					//we call add task on temp to move it to its new place recursively
					return add(temp);
				}
				else{ 
					checkAvailableNextTimesFor(new_task);
					new_task->day = getUsableDay();
					new_task->time = getUsableTime();
					//we call add task on new_task to move it to its new place recursively
					return add(new_task);
				}
			}
			//if we find a place inbetween two tasks place the new task there
			else if(new_task->time < temp->time){
				new_task->counterpart = temp;
				tail->counterpart = new_task;
			}
			//if new task is later than the last task of day it is pointed to by temp->counterpart
			else if(new_task->time > temp->time){
				temp->counterpart = new_task;
			}
		}	
	}
		//add a day that's not in the list and should be in the middle of the list
		if(new_task->day < temp->day){
			new_task->next = temp;
			new_task->previous = tail;
			tail->next = new_task;
			temp->previous = new_task;
		}
		//if task we want to add has a day that's not in the list and is bigger than
		//all days in the list
		if(new_task->day > temp->day){
			new_task->previous = temp;
			new_task->next = temp->next;
			temp->next->previous = new_task;
			temp->next = new_task;			
		}
} 

Task *WorkPlan::getTask(int day, int time)
{
	Task *temp = head;
	//navigate until day is equal to parameter
	while (temp && temp->day != day){
		temp = temp->next;
	}
	if(temp->time == time){
		return temp;
	} else{
		//navigate until time is equal to parameter
		while(temp->counterpart && temp->time != time){
			temp = temp->counterpart;
		}
		return temp;
	}
}


void WorkPlan::checkAvailableNextTimesFor(Task *delayed)	
{	
	Task *temp = delayed;
	Task *tail;
	
	do{ 
		//if delayed task is the last task of day
		if(!temp->counterpart){
			tail = head;
			//make tail point to the first task of the day after delayed task
			while(tail->day != temp->day){
				tail = tail->next;
			}
			tail = tail->next;

			int first_time = 8;
			//iterate until we find an empty hour
			while(first_time == tail->time && tail->counterpart){
				first_time += 1;
				tail = tail->counterpart;
			}
			usable_day = tail->day;
			usable_time = first_time;
			if(tail->day == head->day){
				if(delayed->time == 16){
						usable_day = delayed->day + 1;
						usable_time = 8;
					}
				else{
					usable_day = delayed->day;
					usable_time = delayed->time + 1;
				}
				return;
			}
			return;
		}
		while(!temp->counterpart){
			tail = head;
			while(tail->day != temp->day){
				tail = tail->next;
			}
			tail = tail->next;
			temp = tail;
				//if it's the last day 
				if(tail->day == head->day){
					//usable time is 8 in a newly created day if 16 is full
					if(delayed->time == 16){
						usable_day = delayed->day + 1;
						usable_time = 8;
					}
					//usable time is the next time within the day
					else{
					usable_day = delayed->day;
					usable_time = delayed->time + 1;
				}
				return;
			}
		}
		while(temp->counterpart && (temp->time + 1 == temp->counterpart->time)){
			tail = temp;
			temp = temp->counterpart;
		}
	} while(!temp->counterpart);
	
	//usable time is one hour later if there is no special condition
	usable_day = temp->day;
	usable_time = temp->time + 1;
}


void WorkPlan::delayAllTasksOfDay(int day)
{
	Task *temp, *new_task, *traverse;
	temp = head;
	day_delayed = true;
	int priority;

    //navigate until temp and traverse points to the day
	while(temp && temp->day != day){
		temp = temp->next;
	}
	traverse = temp;

	while(temp->counterpart){
		traverse = temp;
		//create a new task and copy the name of temp and set the day to next day
		//if no next day create a new day
		new_task = new Task;
		new_task->name=new char [strlen(temp->name) + 1];
		strcpy(new_task->name, temp->name);
		if(temp->next->day == head->day){
			new_task->day = temp->day + 1;
		}
		else{
			new_task->day = temp->next->day;
		}
		new_task->time = 8;
		new_task->priority = 0;
		//if temp is not the last element in the day keep iterating
		if(temp->counterpart){
			priority = temp->priority;
			temp = temp->counterpart;
			remove(traverse);
			new_task->priority = priority;
			add(new_task);
		}
		else{
			priority = temp->priority;
			remove(traverse);		
			priority = new_task->priority;
			add(new_task);
			return;
		}
		//enters the if when temp points to the last task in a day
		//(if temp->counterpart became null because of the if block above) 
		if(!temp->counterpart){
			traverse = temp;
			new_task = new Task;
			new_task->name=new char [strlen(temp->name) + 1];
			strcpy(new_task->name, temp->name);
			if(temp->next->day == head->day){
				new_task->day = temp->day + 1;
			}
			else{
				new_task->day = temp->next->day;
			}
			new_task->time = 8;
			new_task->priority = temp->priority;
			remove(traverse);
			add(new_task);
			return;
		}
	}
}


void WorkPlan::remove(Task *target)
{
	Task *traverse, *tail_for_traverse, *temp;
	temp = head;
	while(temp->day != target->day){
		temp = temp->next;
	}
	//if target is the first task in a day
	if(temp->time == target->time){
		//if target is the only task in a day
		if(!target->counterpart){
			target->previous->next = target->next;
			target->next->previous = target->previous;
			if(target == head){
				head = head->next;
			}
			delete target;
		} else{
			target->counterpart->previous = target->previous;
			target->counterpart->next = target->next;
			target->previous->next = target->counterpart;
			target->next->previous = target->counterpart;
			if(target == head){
				head = target->counterpart;
			}
			delete target;
		}
	}
	//if target is not the first task in a day
	else{
		traverse = head;
		tail_for_traverse = traverse;
		//traverse navigating to the target with two while loops with tail behind it 
		while(traverse && traverse->day != target->day){
			tail_for_traverse = traverse;
			traverse = traverse->next;
		}
		while(traverse->counterpart && traverse->time != target->time){
			tail_for_traverse = traverse;
			traverse = traverse->counterpart;
		}
		tail_for_traverse->counterpart = traverse->counterpart;
		delete target;
	}
}

bool WorkPlan::checkCycledList()
{				
	Task *pivot=new Task();
	pivot=head;
	int patient=100;
	bool r=false;
	while (pivot!=NULL&&patient>0)
	{		
		patient--;
		pivot=pivot->previous;
		if(pivot==head)
		{
			r=true;
			break;
		}
	}
	cout<<"("<<100-patient<<")";
	patient=100;
	bool l=false;
	while (pivot!=NULL&&patient>0)
	{		
		patient--;
		pivot=pivot->next;
		if(pivot==head)
		{
			l=true;
			break;
		}
	}
	return r&l;
}
