package main

import (
	"fmt"
	"os"

	"github.com/yourusername/datastructures/array"
	"github.com/yourusername/datastructures/binarytree"
	"github.com/yourusername/datastructures/dlinkedlist"
	"github.com/yourusername/datastructures/hashtable"
	"github.com/yourusername/datastructures/linkedlist"
	"github.com/yourusername/datastructures/queue"
	"github.com/yourusername/datastructures/stack"
)

func main() {
	if len(os.Args) < 2 {
		printUsage()
		return
	}

	command := os.Args[1]
	args := os.Args[2:]

	switch command {
	case "array":
		array.RunDynamicArray(args)
	case "linkedlist":
		linkedlist.RunLinkedList(args)
	case "dlinkedlist":
		dlinkedlist.RunDLinkedList(args)
	case "queue":
		queue.RunQueue(args)
	case "stack":
		stack.RunStack(args)
	case "hashtable":
		hashtable.RunHashTable(args)
	case "binarytree":
		binarytree.RunBinaryTree(args)
	default:
		fmt.Printf("Unknown command: %s\n", command)
		printUsage()
	}
}

func printUsage() {
	fmt.Println("Usage: go run main.go <command> [arguments]")
	fmt.Println("Commands:")
	fmt.Println("  array       - Dynamic Array operations")
	fmt.Println("  linkedlist  - Singly Linked List operations")
	fmt.Println("  dlinkedlist - Doubly Linked List operations")
	fmt.Println("  queue       - Queue operations")
	fmt.Println("  stack       - Stack operations")
	fmt.Println("  hashtable   - Hash Table operations")
	fmt.Println("  binarytree  - Binary Tree operations")
	fmt.Println("\nEach command accepts --file and --query arguments")
}
