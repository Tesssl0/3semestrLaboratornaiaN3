package binarytree

import (
	"encoding/binary"
	"fmt"
	"os"
	"strings"
)

// Node представляет узел бинарного дерева
type Node struct {
	Key   string
	Left  *Node
	Right *Node
}

// BinaryTree представляет бинарное дерево поиска
type BinaryTree struct {
	Root *Node
}

// NewBinaryTree создает новое дерево
func NewBinaryTree() *BinaryTree {
	return &BinaryTree{
		Root: nil,
	}
}

// Insert вставляет ключ в дерево
func (bt *BinaryTree) Insert(key string) {
	bt.Root = bt.insertNode(bt.Root, key)
}

func (bt *BinaryTree) insertNode(node *Node, key string) *Node {
	if key == "" {
		return node
	}
	if node == nil {
		return &Node{Key: key}
	}
	if key < node.Key {
		node.Left = bt.insertNode(node.Left, key)
	} else if key > node.Key {
		node.Right = bt.insertNode(node.Right, key)
	}
	return node
}

// Search ищет ключ в дереве
func (bt *BinaryTree) Search(key string) bool {
	return bt.searchNode(bt.Root, key)
}

func (bt *BinaryTree) searchNode(node *Node, key string) bool {
	if node == nil {
		return false
	}
	if key == node.Key {
		return true
	}
	if key < node.Key {
		return bt.searchNode(node.Left, key)
	}
	return bt.searchNode(node.Right, key)
}

// IsFull проверяет, является ли дерево полным
// IsFull проверяет, является ли дерево полным
func (bt *BinaryTree) IsFull() bool {
	if bt.Root == nil {
		return true
	}
	return bt.isFullNode(bt.Root)
}

func (bt *BinaryTree) isFullNode(node *Node) bool {
	if node == nil {
		return true
	}

	// Если узел - лист (нет детей)
	if node.Left == nil && node.Right == nil {
		return true
	}

	// Если узел имеет обоих детей
	if node.Left != nil && node.Right != nil {
		return bt.isFullNode(node.Left) && bt.isFullNode(node.Right)
	}

	// Если узел имеет только одного ребенка
	return false
}

// PrintInorder выводит in-order обход
func (bt *BinaryTree) PrintInorder() {
	fmt.Print("Inorder traversal: ")
	bt.inorder(bt.Root)
	fmt.Println()
}

func (bt *BinaryTree) inorder(node *Node) {
	if node == nil {
		return
	}
	bt.inorder(node.Left)
	fmt.Printf("%s ", node.Key)
	bt.inorder(node.Right)
}

// PrintPreorder выводит pre-order обход
func (bt *BinaryTree) PrintPreorder() {
	fmt.Print("Preorder traversal: ")
	bt.preorder(bt.Root)
	fmt.Println()
}

func (bt *BinaryTree) preorder(node *Node) {
	if node == nil {
		return
	}
	fmt.Printf("%s ", node.Key)
	bt.preorder(node.Left)
	bt.preorder(node.Right)
}

// PrintPostorder выводит post-order обход
func (bt *BinaryTree) PrintPostorder() {
	fmt.Print("Postorder traversal: ")
	bt.postorder(bt.Root)
	fmt.Println()
}

func (bt *BinaryTree) postorder(node *Node) {
	if node == nil {
		return
	}
	bt.postorder(node.Left)
	bt.postorder(node.Right)
	fmt.Printf("%s ", node.Key)
}

// PrintBFS выводит BFS обход
func (bt *BinaryTree) PrintBFS() {
	if bt.Root == nil {
		fmt.Println()
		return
	}

	queue := []*Node{bt.Root}
	first := true

	for len(queue) > 0 {
		current := queue[0]
		queue = queue[1:]

		if !first {
			fmt.Print(" ")
		}
		fmt.Print(current.Key)
		first = false

		if current.Left != nil {
			queue = append(queue, current.Left)
		}
		if current.Right != nil {
			queue = append(queue, current.Right)
		}
	}
	fmt.Println()
}

// saveNode рекурсивно сохраняет узел в бинарном формате
func (bt *BinaryTree) saveNode(node *Node, file *os.File) error {
	exists := node != nil
	if err := binary.Write(file, binary.LittleEndian, exists); err != nil {
		return err
	}
	if !exists {
		return nil
	}

	if err := binary.Write(file, binary.LittleEndian, int32(len(node.Key))); err != nil {
		return err
	}
	if _, err := file.Write([]byte(node.Key)); err != nil {
		return err
	}

	if err := bt.saveNode(node.Left, file); err != nil {
		return err
	}
	return bt.saveNode(node.Right, file)
}

// loadNode рекурсивно загружает узел из бинарного файла
func (bt *BinaryTree) loadNode(file *os.File) (*Node, error) {
	var exists bool
	if err := binary.Read(file, binary.LittleEndian, &exists); err != nil {
		return nil, err
	}
	if !exists {
		return nil, nil
	}

	var keyLen int32
	if err := binary.Read(file, binary.LittleEndian, &keyLen); err != nil {
		return nil, err
	}

	keyBytes := make([]byte, keyLen)
	if _, err := file.Read(keyBytes); err != nil {
		return nil, err
	}

	node := &Node{Key: string(keyBytes)}

	left, err := bt.loadNode(file)
	if err != nil {
		return nil, err
	}
	node.Left = left

	right, err := bt.loadNode(file)
	if err != nil {
		return nil, err
	}
	node.Right = right

	return node, nil
}

// SaveToFile сохраняет дерево в бинарный файл
func (bt *BinaryTree) SaveToFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	return bt.saveNode(bt.Root, file)
}

// LoadFromFile загружает дерево из бинарного файла
func (bt *BinaryTree) LoadFromFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	root, err := bt.loadNode(file)
	if err != nil {
		return err
	}
	bt.Root = root
	return nil
}

// RunBinaryTree выполняет команды над деревом
func RunBinaryTree(args []string) {
	tree := NewBinaryTree()
	var filename string
	var query string

	for i := 0; i < len(args); i++ {
		if args[i] == "--file" && i+1 < len(args) {
			filename = args[i+1]
			i++
		} else if args[i] == "--query" && i+1 < len(args) {
			query = args[i+1]
			i++
		}
	}

	if filename != "" {
		tree.LoadFromFile(filename)
	}

	parts := strings.SplitN(query, " ", 2)
	command := parts[0]
	arg := ""
	if len(parts) > 1 {
		arg = parts[1]
	}

	switch command {
	case "TINSERT":
		if arg != "" {
			tree.Insert(arg)
			if filename != "" {
				tree.SaveToFile(filename)
			}
		}
	case "TGET", "TSEARCH":
		if tree.Search(arg) {
			if command == "TGET" {
				fmt.Println(arg)
			} else {
				fmt.Println("true")
			}
		} else {
			if command == "TGET" {
				fmt.Println("NOT_FOUND")
			} else {
				fmt.Println("false")
			}
		}
	case "TFULL":
		if tree.IsFull() {
			fmt.Println("true")
		} else {
			fmt.Println("false")
		}
	case "TINORDER":
		tree.PrintInorder()
	case "TPREORDER":
		tree.PrintPreorder()
	case "TPOSTORDER":
		tree.PrintPostorder()
	case "TBFS":
		fmt.Print("BFS обход: ")
		tree.PrintBFS()
	}
}
