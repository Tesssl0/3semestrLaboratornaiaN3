package dlinkedlist

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// DlistNode представляет узел двусвязного списка
type DlistNode struct {
	Data string
	Next *DlistNode
	Prev *DlistNode
}

// DlinkedList представляет двусвязный список
type DlinkedList struct {
	Head *DlistNode
	Tail *DlistNode
}

// NewDlinkedList создает новый пустой список
func NewDlinkedList() *DlinkedList {
	return &DlinkedList{
		Head: nil,
		Tail: nil,
	}
}

// AddToHead добавляет элемент в начало
func (dll *DlinkedList) AddToHead(value string) {
	newNode := &DlistNode{Data: value, Next: dll.Head, Prev: nil}
	if dll.Head != nil {
		dll.Head.Prev = newNode
	}
	dll.Head = newNode
	if dll.Tail == nil {
		dll.Tail = dll.Head
	}
}

// AddToTail добавляет элемент в конец
func (dll *DlinkedList) AddToTail(value string) {
	newNode := &DlistNode{Data: value, Next: nil, Prev: dll.Tail}
	if dll.Tail != nil {
		dll.Tail.Next = newNode
	}
	dll.Tail = newNode
	if dll.Head == nil {
		dll.Head = dll.Tail
	}
}

// RemoveFromHead удаляет элемент с головы
func (dll *DlinkedList) RemoveFromHead() bool {
	if dll.Head == nil {
		return false
	}
	dll.Head = dll.Head.Next
	if dll.Head != nil {
		dll.Head.Prev = nil
	} else {
		dll.Tail = nil
	}
	return true
}

// RemoveFromTail удаляет элемент с хвоста
func (dll *DlinkedList) RemoveFromTail() bool {
	if dll.Tail == nil {
		return false
	}
	dll.Tail = dll.Tail.Prev
	if dll.Tail != nil {
		dll.Tail.Next = nil
	} else {
		dll.Head = nil
	}
	return true
}

// RemoveByValue удаляет элемент по значению
func (dll *DlinkedList) RemoveByValue(value string) bool {
	for current := dll.Head; current != nil; current = current.Next {
		if current.Data == value {
			if current.Prev != nil {
				current.Prev.Next = current.Next
			} else {
				dll.Head = current.Next
			}
			if current.Next != nil {
				current.Next.Prev = current.Prev
			} else {
				dll.Tail = current.Prev
			}
			return true
		}
	}
	return false
}

// Search ищет элемент по значению
func (dll *DlinkedList) Search(value string) bool {
	for current := dll.Head; current != nil; current = current.Next {
		if current.Data == value {
			return true
		}
	}
	return false
}

// Print выводит все элементы
func (dll *DlinkedList) Print() {
	for current := dll.Head; current != nil; current = current.Next {
		fmt.Printf("%s ", current.Data)
	}
	fmt.Println()
}

// Destroy очищает список
func (dll *DlinkedList) Destroy() {
	for dll.Head != nil {
		dll.RemoveFromHead()
	}
}

// LoadFromFile загружает из файла
func (dll *DlinkedList) LoadFromFile(filename string) error {
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
		dll.AddToTail(scanner.Text())
	}
	return scanner.Err()
}

// SaveToFile сохраняет в файл
func (dll *DlinkedList) SaveToFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	for current := dll.Head; current != nil; current = current.Next {
		if _, err := fmt.Fprintln(file, current.Data); err != nil {
			return err
		}
	}
	return nil
}

// AddBefore добавляет элемент перед target
func (dll *DlinkedList) AddBefore(target, value string) bool {
	if dll.Head == nil {
		return false
	}

	// Если target в голове
	if dll.Head.Data == target {
		dll.AddToHead(value)
		return true
	}

	// Ищем target
	current := dll.Head
	for current != nil && current.Data != target {
		current = current.Next
	}
	if current == nil {
		return false
	}

	// Вставляем перед current
	newNode := &DlistNode{Data: value, Next: current, Prev: current.Prev}
	if current.Prev != nil {
		current.Prev.Next = newNode
	}
	current.Prev = newNode
	return true
}

// AddAfter добавляет элемент после target
func (dll *DlinkedList) AddAfter(target, value string) bool {
	current := dll.Head
	for current != nil && current.Data != target {
		current = current.Next
	}
	if current == nil {
		return false
	}

	newNode := &DlistNode{Data: value, Next: current.Next, Prev: current}
	if current.Next != nil {
		current.Next.Prev = newNode
	} else {
		dll.Tail = newNode
	}
	current.Next = newNode
	return true
}

// RemoveBefore удаляет элемент перед target
func (dll *DlinkedList) RemoveBefore(target string) bool {
	if dll.Head == nil || dll.Head.Data == target {
		return false
	}

	// Если второй элемент - target
	if dll.Head.Next != nil && dll.Head.Next.Data == target {
		return dll.RemoveFromHead()
	}

	// Ищем target
	current := dll.Head
	for current != nil && current.Data != target {
		current = current.Next
	}
	if current == nil || current.Prev == nil {
		return false
	}

	// Удаляем элемент перед target
	toRemove := current.Prev
	if toRemove.Prev != nil {
		toRemove.Prev.Next = current
		current.Prev = toRemove.Prev
	} else {
		// Если удаляем первый элемент
		dll.Head = current
		current.Prev = nil
	}
	return true
}

// RemoveAfter удаляет элемент после target
// RemoveAfter удаляет элемент после target
func (dll *DlinkedList) RemoveAfter(target string) bool {
	if dll.Head == nil {
		return false
	}

	current := dll.Head
	for current != nil && current.Data != target {
		current = current.Next
	}
	if current == nil || current.Next == nil {
		return false
	}

	toRemove := current.Next
	current.Next = toRemove.Next
	if toRemove.Next != nil {
		toRemove.Next.Prev = current
	} else {
		dll.Tail = current
	}
	return true
}

// RunDLinkedList выполняет команды над списком
func RunDLinkedList(args []string) {
	list := NewDlinkedList()
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
	case "DPUSH":
		if token1 != "" {
			list.AddToHead(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "DAPPEND":
		if token1 != "" {
			list.AddToTail(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "DREMOVEHEAD":
		list.RemoveFromHead()
		if filename != "" {
			list.SaveToFile(filename)
		}
	case "DREMOVETAIL":
		list.RemoveFromTail()
		if filename != "" {
			list.SaveToFile(filename)
		}
	case "DREMOVE":
		if token1 != "" {
			list.RemoveByValue(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "DSEARCH":
		if token1 != "" {
			if list.Search(token1) {
				fmt.Println("true")
			} else {
				fmt.Println("false")
			}
		}
	case "DPRINT":
		list.Print()
	case "DADDTO":
		if token1 != "" && token2 != "" {
			list.AddBefore(token1, token2)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "DADDAFTER":
		if token1 != "" && token2 != "" {
			list.AddAfter(token1, token2)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "DREMOVETO":
		if token1 != "" {
			list.RemoveBefore(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	case "DREMOVEAFTER":
		if token1 != "" {
			list.RemoveAfter(token1)
			if filename != "" {
				list.SaveToFile(filename)
			}
		}
	}

	list.Destroy()
}
