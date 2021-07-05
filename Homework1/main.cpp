/**
 *  @Author
 *	Name Surname: Abdulkadir PAZAR
 *	Student Number: 150180028
 *	Date: 15.10.2019
 */


#include <iostream>
#include <cstdlib>
#include <stdio.h>

using namespace std;

struct node{
	int size;
	int quant;
	node* next;
};

struct stock {
	node *head;
	FILE *stock_file;
	char *filename;
	void create();
	void add_stock(int shoe_num);
	void sell(int shoe_num);
	void current_stock();
	void clear();
	void read_from_file();
	int search(int shoe_num);
	node *create_node(int shoe_num);
	void remove_node(int shoe_num);
	node *navigate_to_node(int shoe_num);
};

/**
 * This function starts the linked list by initializing the head
 * and calling the read_from_file function.
 */
void stock::create()
{
	head = NULL;
	read_from_file();
}

/**
 * This function adds the shoe number you want to
 * the stock. If it is already added it increments
 * the number by one. If the number is negative, it
 * calls the sell function. 
 * @param value of the shoe number we want to add.
 */
void stock::add_stock(int shoe_num)
{
	if(shoe_num < 0){
		sell(shoe_num);
		return;
	}
	int index = search(shoe_num);
	if(index == -1){
		if (head == NULL || shoe_num < head->size){
			node *nn = create_node(shoe_num);
			nn->next = head;
			head = nn;
			return;
		}
		node *nn = create_node(shoe_num);
		node *temp = head;
		while (temp->next && shoe_num>temp->next->size){
			temp = temp->next;
		}
		nn->next = temp->next;
		temp->next = nn;
		return;
	}
	
	node *nn = navigate_to_node(shoe_num);
	nn->quant++;
}

/**
 * This function sells the shoe number you want to sell.
 * If it does not exist it prints out "NO_STOCK".
 * If only one shoe is left, it deletes the node of that number.
 * @param value of the shoe number we want to add.
 */
void stock::sell(int shoe_num)
{
	node *nn = navigate_to_node((-1) * shoe_num);
	if (nn == NULL){
		cout << "NO_STOCK" << endl;
		return;
	}
	if (nn->quant > 1){
		nn->quant--;
		return;
	} if(nn->quant == 1){
		remove_node((-1) * shoe_num);
		return;
	}
}

/**
 * Prints out the current stock list.
 */
void stock::current_stock()
{
	node* nn = head;
	while (nn) {
		cout << nn->size << ":" << nn->quant << endl;
		nn = nn->next;
	}
}

/**
 * Deletes the entire linked list.
 */
void stock::clear()
{
	node *temp;
	while (head) {
		temp = head;
		head = head->next;
		delete temp;
	}
}

/**
 * Reads the input.txt and produces the linked list.
 *
 * @param value of the shoe number we want to add.
 */
void stock::read_from_file()
{
	int  shoe_num;
	if (!(stock_file = fopen(filename, "r+")))
		if (!(stock_file = fopen(filename, "w+"))) {
			cerr << "File could not be opened" << endl;
			exit(1);
		}
	while (!feof(stock_file)) {
		fscanf(stock_file, "%d", &shoe_num);
		if(shoe_num != 0){
			add_stock(shoe_num);
		}
		else {
			current_stock();
		}
    }
	fclose(stock_file);
}

/**
 * Searches for the position of the shoe number.
 * Returns -1 if not found.
 * @param value of the shoe number we want to add.
 * @return Index of the shoe number.
 */
int stock::search(int shoe_num)
{
	int position = 1;
	node *temp = head;
	while (temp && temp->size != shoe_num){
		position++;
		temp = temp->next;
	}
	if (temp == NULL)
		position = -1;
	return position;
}

/**
 * Creates the node for the shoe number.
 * @param value of the shoe number we want to add.
 */
node *stock::create_node(int shoe_num)
{
	node *nn = new node;
	nn->size = shoe_num;
	nn->quant = 1;
	nn->next = NULL;
	return nn;
}

/**
 * Deletes the node for the shoe number.
 * @param value of the shoe number we want to add.
 */
void stock::remove_node(int shoe_num)
{
	int index = search(shoe_num);
	if (index == -1)
		return;
	node *temp = head,*tail = temp;
	
	if (index == 1){
		head = head->next;
		delete temp;
		return;
	}

	int counter = 1;
	while (temp != NULL && counter < index){
		tail = temp;
		temp = temp->next;
		counter++;
	}
	tail->next = temp->next;
	delete temp;
}

/**
 * Navigates to the node of the shoe number we
 * want to go to.
 * @param value of the shoe number we want to add.
 * @return pointer to the node for the shoe number.
 */
node *stock::navigate_to_node(int shoe_num)
{
	node* nn = head;   
    while (nn != NULL)  
    {  
        if (nn->size == shoe_num)  
            return nn;    
        nn = nn->next;  
    }
	nn = NULL;
	return nn;
}


int main(int argc, char** argv)
{
	stock my_stock;
	my_stock.filename = argv[1];
	my_stock.create();
	my_stock.clear();
	return EXIT_SUCCESS;
}