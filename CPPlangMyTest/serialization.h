#pragma once
#include <string>
#include <vector>
#include <fstream>

using namespace std;

#include "array.h"
#include "stack.h"
#include "queue.h"
#include "linkedList.h"
#include "dlinkedList.h"
#include "hashTable.h"
#include "binaryTree.h"

/* ---------------------------------------------------------
   DYNAMIC ARRAY
--------------------------------------------------------- */

void DA_saveText(DynamicArray& arr, const string& file);
void DA_loadText(DynamicArray& arr, const string& file);

void DA_saveBinary(DynamicArray& arr, const string& file);
void DA_loadBinary(DynamicArray& arr, const string& file);

/* ---------------------------------------------------------
   STACK
--------------------------------------------------------- */

void Stack_saveText(Stack& st, const string& file);
void Stack_loadText(Stack& st, const string& file);

void Stack_saveBinary(Stack& st, const string& file);
void Stack_loadBinary(Stack& st, const string& file);

/* ---------------------------------------------------------
   QUEUE
--------------------------------------------------------- */

void Queue_saveText(Queue& q, const string& file);
void Queue_loadText(Queue& q, const string& file);

void Queue_saveBinary(Queue& q, const string& file);
void Queue_loadBinary(Queue& q, const string& file);

/* ---------------------------------------------------------
   LINKED LIST
--------------------------------------------------------- */

void LL_saveText(LinkedList& list, const string& file);
void LL_loadText(LinkedList& list, const string& file);

void LL_saveBinary(LinkedList& list, const string& file);
void LL_loadBinary(LinkedList& list, const string& file);

/* ---------------------------------------------------------
   DOUBLY LINKED LIST
--------------------------------------------------------- */

void DLL_saveText(DlinkedList& list, const string& file);
void DLL_loadText(DlinkedList& list, const string& file);

void DLL_saveBinary(DlinkedList& list, const string& file);
void DLL_loadBinary(DlinkedList& list, const string& file);

/* ---------------------------------------------------------
   HASH TABLE
--------------------------------------------------------- */

void HT_saveBinary(const string& file);
void HT_loadBinary(const string& file);

/* ---------------------------------------------------------
   BINARY TREE
--------------------------------------------------------- */

void BT_saveBinary(BinaryTree& t, const string& file);
void BT_loadBinary(BinaryTree& t, const string& file);


/* ---------------------------------------------------------
   JSON SERIALIZATION 
--------------------------------------------------------- */

// Dynamic Array
void DA_saveJSON(DynamicArray& arr, const string& file);
void DA_loadJSON(DynamicArray& arr, const string& file);

// Stack
void Stack_saveJSON(Stack& st, const string& file);
void Stack_loadJSON(Stack& st, const string& file);

// Queue
void Queue_saveJSON(Queue& q, const string& file);
void Queue_loadJSON(Queue& q, const string& file);

// Linked List
void LL_saveJSON(LinkedList& list, const string& file);
void LL_loadJSON(LinkedList& list, const string& file);

// Doubly Linked List
void DLL_saveJSON(DlinkedList& list, const string& file);
void DLL_loadJSON(DlinkedList& list, const string& file);

// Hash Table
void HT_saveJSON(const string& file);
void HT_loadJSON(const string& file);

// Binary Tree
void BT_saveJSON(BinaryTree& t, const string& file);
void BT_loadJSON(BinaryTree& t, const string& file);