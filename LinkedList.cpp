#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;


Node* newCanvas()
{
	// TODO: Write the code for the function
	ListItemType canvas;
	initCanvas(canvas);
	Node* head = new Node;
	copyCanvas(head->item, canvas);
	head->next = NULL;
	return head;
}


Node* newCanvas(Node* oldNode)
{
	// TODO: Write the code for the function
	Node* head = new Node;
	copyCanvas(head->item, oldNode->item);
	head->next = NULL;
	return head;
}


void play(List& clips)
{
	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		// TODO: Write the code for the function
		playRecursive(clips.head, clips.count);
	}
}


void playRecursive(Node* head, int count)
{
	// TODO: Write the code for the function
	count--;
	//Base Case
	if (count == 0)
	{
		displayCanvas(head->item);
		return;
	}
	//Recursive Case
	else
	{
		playRecursive(head->next, count);
		Sleep(100);
		displayCanvas(head->item);
	}
}


void addUndoState(List& undoList, List& redoList, Node* &current)
{
	// TODO: Write the code for the function
	Node* head = newCanvas(current);
	addNode(undoList, head);
	deleteList(redoList);
}


void restore(List& undoList, List& redoList, Node* &current)
{
	// TODO: Write the code for the function
	addNode(redoList, current);
	current = removeNode(undoList);
}


void addNode(List& list, Node* nodeToAdd)
{
	// TODO: Write the code for the function
	Node* temp = nodeToAdd;
	temp->next = list.head;
	list.head = temp;
	list.count++;
}


Node* removeNode(List& list)
{
	// TODO: Write the code for the function
	Node* temp = list.head;
	list.head = list.head->next;
	list.count--;
	return temp;
}


void deleteList(List& list)
{
	// TODO: Write the code for the function
	Node* head = list.head;
	while (head != NULL)
	{
		Node* temp = head->next;
		delete head;
		head = temp;
	}
}


bool loadClips(List& clips, char filename[])
{
	// TODO: Write the code for the function
	deleteList(clips);

	char name[FILENAMESIZE];
	int count = 1;
	snprintf(filename, FILENAMESIZE, "-%d.txt", count);
	Node* nodeTwo = newCanvas();
	if (loadCanvas(nodeTwo->item, filename) == false)
	{
		return false;
	}

	do
	{
		addNode(clips, nodeTwo);
		count++;
		snprintf(filename, FILENAMESIZE,"-%d.txt", count);
		loadCanvas(nodeTwo->item, filename);
		nodeTwo = newCanvas();
	} while (loadCanvas(nodeTwo->item, name));
	clips.count = count;
	return true;
}


bool saveClips(List& clips, char filename[])
{
	// TODO: Write the code for the function
	return true;
}