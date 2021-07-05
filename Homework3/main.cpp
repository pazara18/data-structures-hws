/*
	@Author	
	Student Name: Abdulkadir Pazar
	Student ID: 150180028
	Date: 18.12.2019
*/

#include <cstdlib>//EXIT_SUCCESS
#include <iostream>//cout
#include <string>//string methods
#include <algorithm>//reverse
#include <fstream>//ifstream

using namespace std;

struct MobileHost{
	int id;
	int parent_id;
	MobileHost *next;
};

struct BaseStation{
	int id;
	int parent_id;
	MobileHost *mh_head;
	BaseStation *sibling; 
	BaseStation *child;
};

struct Network{
	void create();//creates cc
	void close(BaseStation* ptr);//clears the network using postorder traversal
	void add_bs(BaseStation *added);//adds new bs
	void add_mh(MobileHost *added);//adds new mh
	BaseStation *find_bs(BaseStation *ptr, int search_id);//returns pointer to the searched bs
	BaseStation *find_mh(BaseStation *ptr, int search_id);//prints path to mh and returns pointer to bs of mh
	void send_message(string message, int mh_id);// sends message to specified mh
	void print_full_path(BaseStation *ptr, int target_id);//prints the path from root to bs
	BaseStation *root;
};

void Network::create()
{
	root = new BaseStation;
	root->id = 0;
	root->parent_id = -1;
	root->mh_head = NULL;
	root->sibling = NULL;
	root->child = NULL;
}

void Network::close(BaseStation* ptr)
{
	MobileHost *temp = NULL;
	if(ptr){
		if(ptr->child != NULL){
			close(ptr->child);
			ptr->child = NULL;
		}
		if (ptr->sibling != NULL){
			close(ptr->sibling);
			ptr->sibling = NULL;
		}
		while(ptr->mh_head){
			temp = ptr->mh_head;
			ptr->mh_head = ptr->mh_head->next;
			delete temp;
		}
		delete ptr;
	}
}

void Network::add_bs(BaseStation *added)
{
	BaseStation *temp = NULL;
	temp = find_bs(root, added->parent_id);
	if(!temp->child){
		temp->child = added;
		return;
	}
	temp = temp->child;
	while(temp->sibling)
		temp = temp->sibling;
	temp->sibling = added;
}

void Network::add_mh(MobileHost *added)
{
	BaseStation *parent = find_bs(root, added->parent_id);
	if (parent->mh_head == NULL){
		parent->mh_head = added;
		return;
	}
	else{
		MobileHost *temp = parent->mh_head;
		while(temp->next)
			temp = temp->next;
		temp->next = added;
	}
}

BaseStation *Network::find_bs(BaseStation *ptr, int search_id)
{
	BaseStation *found_bs = NULL; 
	if(ptr->id == search_id){
		return ptr;
	}
	if(ptr->child){
		if ((found_bs = find_bs(ptr->child, search_id)))
			return found_bs;
	}
	if(ptr->sibling){
		if((found_bs = find_bs(ptr->sibling, search_id)))
			return found_bs;
	}
	return NULL;
}

BaseStation *Network::find_mh(BaseStation *ptr, int search_id)
{
	MobileHost *temp = ptr->mh_head;
	BaseStation* found_parent = NULL;
	if(ptr->mh_head != NULL){
		while(temp){
			if(!temp)
				break;
			if(find_bs(root, temp->parent_id) == NULL)
				return NULL;
			if(temp->id == search_id){
				return find_bs(root, temp->parent_id);
			}
			temp = temp->next;
		}
	}
	if (ptr == root)
		cout << ptr->id << " ";
	
	if(ptr->child){
		cout << ptr->child->id << " ";
		if ((found_parent = find_mh(ptr->child, search_id)))
			return found_parent;
	}
	if(ptr->sibling){
		cout << ptr->sibling->id << " ";
		if((found_parent = find_mh(ptr->sibling, search_id)))
			return found_parent;
	}
	return NULL;
}

void Network::send_message(string message, int target_mh)
{
	cout << "Traversing:";
	BaseStation *target_parent = find_mh(root, target_mh);
	cout << endl;
	if(target_parent == NULL){
		cout << "Can not be reached the mobile host mh_" << target_mh << " at the moment" << endl;
		return;
	}
	int target_parent_id = target_parent->id;
	cout << "Message:" << message << " To:";
	print_full_path(root, target_parent_id);
	cout << "mh_" << target_mh << endl;
}

void Network::print_full_path(BaseStation *ptr, int target_id)
{
	string path, s;
	BaseStation* target = NULL;
	int bs_id = 0;

	while(target != root){//go from bs to root using parent id
		target = find_bs(root, target_id);
		bs_id = target->id;
		target_id = target->parent_id;
		s = to_string(bs_id) + " ";
		reverse(begin(s), end(s));
		path.append(s);
	}
	reverse(begin(path), end(path));//reverse the string to get real path
	cout << path;
}

int main(int argc, char **argv)
{
	Network network;
	network.create();
	ifstream in;
	in.open(argv[1]);//opens network file
	while(!in.eof()){
		string type, id, parent_id;
		in >> type >> id >> parent_id;//gets info
		if(type.compare("") == 0)//breaks if empty line is reached
			break;
		if (type.compare("BS") == 0){//enters if it is a bs
			BaseStation *newbs =  new BaseStation;
			newbs->id = stoi(id);
			newbs->parent_id = stoi(parent_id);
			newbs->mh_head = NULL;
			newbs->child = NULL;
			newbs->sibling = NULL;
			network.add_bs(newbs);
		}
		if (type.compare("MH") == 0){//enters if it is a mh
			MobileHost *newmh = new MobileHost;
			newmh->id = stoi(id);
			newmh->parent_id = stoi(parent_id);
			newmh->next = NULL;
			network.add_mh(newmh);
		}
	}
	in.close();
	in.open(argv[2]);//opens message file
	while(!in.eof()){
		string message, id;
		getline(in, message, '>');//get message
		getline(in, id, '\n');//get id
		if (message.compare("") == 0)//breaks if empty line is reached
			break;
		network.send_message(message, stoi(id));
	}
	in.close();
	network.close(network.root);
	return EXIT_SUCCESS;
}