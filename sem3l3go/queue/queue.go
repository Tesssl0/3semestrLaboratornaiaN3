package queue

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// QueueNode представляет узел очереди
type QueueNode struct {
	Data string
	Next *QueueNode
}

// Queue представляет очередь
type Queue struct {
	Front *QueueNode
	Rear  *QueueNode
}

// NewQueue создает новую очередь
func NewQueue() *Queue {
	return &Queue{
		Front: nil,
		Rear:  nil,
	}
}

// Enqueue добавляет элемент в конец очереди
func (q *Queue) Enqueue(value string) {
	newNode := &QueueNode{Data: value, Next: nil}
	if q.Rear != nil {
		q.Rear.Next = newNode
	}
	q.Rear = newNode
	if q.Front == nil {
		q.Front = q.Rear
	}
}

// Dequeue удаляет элемент из начала очереди
func (q *Queue) Dequeue() bool {
	if q.Front == nil {
		return false
	}
	q.Front = q.Front.Next
	if q.Front == nil {
		q.Rear = nil
	}
	return true
}

// Print выводит все элементы
func (q *Queue) Print() {
	temp := q.Front
	for temp != nil {
		fmt.Printf("%s ", temp.Data)
		temp = temp.Next
	}
	fmt.Println()
}

// Destroy очищает очередь
func (q *Queue) Destroy() {
	for q.Front != nil {
		q.Dequeue()
	}
}

// LoadFromFile загружает из файла
func (q *Queue) LoadFromFile(filename string) error {
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
		q.Enqueue(scanner.Text())
	}
	return scanner.Err()
}

// SaveToFile сохраняет в файл
func (q *Queue) SaveToFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	temp := q.Front
	for temp != nil {
		if _, err := fmt.Fprintln(file, temp.Data); err != nil {
			return err
		}
		temp = temp.Next
	}
	return nil
}

// RunQueue выполняет команды над очередью
func RunQueue(args []string) {
	queue := NewQueue()
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
		queue.LoadFromFile(filename)
	}

	parts := strings.SplitN(query, " ", 2)
	command := parts[0]
	arg := ""
	if len(parts) > 1 {
		arg = parts[1]
	}

	switch command {
	case "QPUSH":
		if arg != "" {
			queue.Enqueue(arg)
			if filename != "" {
				queue.SaveToFile(filename)
			}
		}
	case "QPOP":
		queue.Dequeue()
		if filename != "" {
			queue.SaveToFile(filename)
		}
	case "QPRINT":
		queue.Print()
	}

	queue.Destroy()
}
