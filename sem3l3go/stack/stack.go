package stack

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// StackNode представляет узел стека
type StackNode struct {
	Data string
	Next *StackNode
}

// Stack представляет стек
type Stack struct {
	Top *StackNode
}

// NewStack создает новый стек
func NewStack() *Stack {
	return &Stack{
		Top: nil,
	}
}

// Push добавляет элемент на вершину
func (s *Stack) Push(value string) {
	s.Top = &StackNode{Data: value, Next: s.Top}
}

// Pop удаляет элемент с вершины
func (s *Stack) Pop() bool {
	if s.Top == nil {
		return false
	}
	s.Top = s.Top.Next
	return true
}

// Print выводит все элементы
func (s *Stack) Print() {
	temp := s.Top
	for temp != nil {
		fmt.Printf("%s ", temp.Data)
		temp = temp.Next
	}
	fmt.Println()
}

// Destroy очищает стек
func (s *Stack) Destroy() {
	for s.Top != nil {
		s.Pop()
	}
}

// LoadFromFile загружает из файла
func (s *Stack) LoadFromFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var values []string
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		values = append(values, scanner.Text())
	}

	// Загружаем в обратном порядке для правильного порядка стека
	for i := len(values) - 1; i >= 0; i-- {
		s.Push(values[i])
	}
	return scanner.Err()
}

// SaveToFile сохраняет в файл
func (s *Stack) SaveToFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	temp := s.Top
	for temp != nil {
		if _, err := fmt.Fprintln(file, temp.Data); err != nil {
			return err
		}
		temp = temp.Next
	}
	return nil
}

// RunStack выполняет команды над стеком
func RunStack(args []string) {
	stack := NewStack()
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
		stack.LoadFromFile(filename)
	}

	parts := strings.SplitN(query, " ", 2)
	command := parts[0]
	arg := ""
	if len(parts) > 1 {
		arg = parts[1]
	}

	switch command {
	case "SPUSH":
		if arg != "" {
			stack.Push(arg)
			if filename != "" {
				stack.SaveToFile(filename)
			}
		}
	case "SPOP":
		stack.Pop()
		if filename != "" {
			stack.SaveToFile(filename)
		}
	case "SPRINT":
		stack.Print()
	}

	stack.Destroy()
}
