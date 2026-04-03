package linkedlist

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// ListNode представляет узел односвязного списка
type ListNode struct {
	Data string
	Next *ListNode
}

// LinkedList представляет односвязный список
type LinkedList struct {
	Head *ListNode
	Tail *ListNode
}

// NewLinkedList создает новый пустой список
func NewLinkedList() *LinkedList {
	return &LinkedList{
		Head: nil,
		Tail: nil,
	}
}

// AddToHead добавляет элемент в начало
func (ll *LinkedList) AddToHead(value string) {
	newNode := &ListNode{Data: value, Next: ll.Head}
	ll.Head = newNode
	if ll.Tail == nil {
		ll.Tail = ll.Head
	}
}

// AddToTail добавляет элемент в конец
func (ll *LinkedList) AddToTail(value string) {
	newNode := &ListNode{Data: value, Next: nil}
	if ll.Tail != nil {
		ll.Tail.Next = newNode
	}
	ll.Tail = newNode
	if ll.Head == nil {
		ll.Head = ll.Tail
	}
}

// RemoveFromHead удаляет элемент с головы
func (ll *LinkedList) RemoveFromHead() bool {
	if ll.Head == nil {
		return false
	}
	ll.Head = ll.Head.Next
	if ll.Head == nil {
		ll.Tail = nil
	}
	return true
}

// RemoveFromTail удаляет элемент с хвоста
func (ll *LinkedList) RemoveFromTail() bool {
	if ll.Tail == nil {
		return false
	}
	if ll.Head == ll.Tail {
		ll.Head = nil
		ll.Tail = nil
		return true
	}
	temp := ll.Head
	for temp.Next != ll.Tail {
		temp = temp.Next
	}
	temp.Next = nil
	ll.Tail = temp
	return true
}

// RemoveByValue удаляет элемент по значению
func (ll *LinkedList) RemoveByValue(value string) bool {
	if ll.Head == nil {
		return false
	}
	if ll.Head.Data == value {
		return ll.RemoveFromHead()
	}
	temp := ll.Head
	for temp.Next != nil {
		if temp.Next.Data == value {
			if temp.Next == ll.Tail {
				ll.Tail = temp
			}
			temp.Next = temp.Next.Next
			return true
		}
		temp = temp.Next
	}
	return false
}

// Search ищет элемент по значению
func (ll *LinkedList) Search(value string) bool {
	temp := ll.Head
	for temp != nil {
		if temp.Data == value {
			return true
		}
		temp = temp.Next
	}
	return false
}

// Print выводит все элементы
func (ll *LinkedList) Print() {
	temp := ll.Head
	for temp != nil {
		fmt.Printf("%s ", temp.Data)
		temp = temp.Next
	}
	fmt.Println()
}

// Destroy очищает список
func (ll *LinkedList) Destroy() {
	for ll.Head != nil {
		ll.RemoveFromHead()
	}
}

// LoadFromFile загружает из файла
func (ll *LinkedList) LoadFromFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		ll.AddToTail(scanner.Text())
	}
	return scanner.Err()
}

// SaveToFile сохраняет в файл
func (ll *LinkedList) SaveToFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	temp := ll.Head
	for temp != nil {
		if _, err := fmt.Fprintln(file, temp.Data); err != nil {
			return err
		}
		temp = temp.Next
	}
	return nil
}

// AddBefore добавляет элемент перед target
func (ll *LinkedList) AddBefore(target, value string) bool {
	if ll.Head == nil {
		return false
	}
	if ll.Head.Data == target {
		ll.AddToHead(value)
		return true
	}
	prev := ll.Head
	for prev.Next != nil && prev.Next.Data != target {
		prev = prev.Next
	}
	if prev.Next == nil {
		return false
	}
	newNode := &ListNode{Data: value, Next: prev.Next}
	prev.Next = newNode
	return true
}

// AddAfter добавляет элемент после target
func (ll *LinkedList) AddAfter(target, value string) bool {
	node := ll.Head
	for node != nil && node.Data != target {
		node = node.Next
	}
	if node == nil {
		return false
	}
	newNode := &ListNode{Data: value, Next: node.Next}
	node.Next = newNode
	if node == ll.Tail {
		ll.Tail = newNode
	}
	return true
}

// RemoveBefore удаляет элемент перед target
func (ll *LinkedList) RemoveBefore(target string) bool {
	if ll.Head == nil || ll.Head.Data == target {
		return false
	}
	if ll.Head.Next != nil && ll.Head.Next.Data == target {
		return ll.RemoveFromHead()
	}
	prev := ll.Head
	for prev.Next != nil && prev.Next.Next != nil && prev.Next.Next.Data != target {
		prev = prev.Next
	}
	if prev.Next == nil || prev.Next.Next == nil {
		return false
	}
	nodeToRemove := prev.Next
	prev.Next = nodeToRemove.Next
	if nodeToRemove == ll.Tail {
		ll.Tail = prev
	}
	return true
}

// RemoveAfter удаляет элемент после target
func (ll *LinkedList) RemoveAfter(target string) bool {
	node := ll.Head
	for node != nil && node.Data != target {
		node = node.Next
	}
	if node == nil || node.Next == nil {
		return false
	}
	nodeToRemove := node.Next
	node.Next = nodeToRemove.Next
	if nodeToRemove == ll.Tail {
		ll.Tail = node
	}
	return true
}

// RunLinkedList выполняет команды над списком
func RunLinkedList(args []string) {
	list := NewLinkedList()
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
		list.LoadFromFile(filename)
	}

	parts := strings.SplitN(query, " ", 3)
	command := parts[0]
	token1, token2 := "", ""
	if len(parts) > 1 {
		token1 = parts[1]
	}
	if len(parts) > 2 {
		token2 = parts[2]
	}

	switch command {
	case "LPUSH":
		if token1 != "" {
			list.AddToHead(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "LAPPEND":
		if token1 != "" {
			list.AddToTail(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "LREMOVEHEAD":
		list.RemoveFromHead()
		if filename != "" {
			list.SaveToFile(filename)
		}
	case "LREMOVETAIL":
		list.RemoveFromTail()
		if filename != "" {
			list.SaveToFile(filename)
		}
	case "LREMOVE":
		if token1 != "" {
			list.RemoveByValue(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "LSEARCH":
		if token1 != "" {
			if list.Search(token1) {
				fmt.Println("true")
			} else {
				fmt.Println("false")
			}
		}
	case "LPRINT":
		list.Print()
	case "LADDTO":
		if token1 != "" && token2 != "" {
			list.AddBefore(token1, token2)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "LADDAFTER":
		if token1 != "" && token2 != "" {
			list.AddAfter(token1, token2)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "LREMOVETO":
		if token1 != "" {
			list.RemoveBefore(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "LREMOVEAFTER":
		if token1 != "" {
			list.RemoveAfter(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	}

	list.Destroy()
}
