package hashtable

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

const tableSize = 10

// HashNode представляет узел хеш-таблицы
type HashNode struct {
	Key   string
	Value string
	Next  *HashNode
}

// HashTable представляет хеш-таблицу
type HashTable struct {
	table [tableSize]*HashNode
}

func (ht *HashTable) Clear() {
	for i := range ht.table {
		ht.table[i] = nil
	}
}

// NewHashTable создает новую хеш-таблицу
func NewHashTable() *HashTable {
	return &HashTable{
		table: [tableSize]*HashNode{},
	}
}

// hashFunction вычисляет хеш ключа
func (ht *HashTable) hashFunction(key string) int {
	hash := 0
	for _, ch := range key {
		hash += int(ch)
	}
	return hash % tableSize
}

// Insert вставляет или обновляет пару ключ-значение
func (ht *HashTable) Insert(key, value string) {
	index := ht.hashFunction(key)

	// Проверяем, существует ли уже ключ
	current := ht.table[index]
	for current != nil {
		if current.Key == key {
			current.Value = value
			return
		}
		current = current.Next
	}

	// Добавляем новый узел в начало
	ht.table[index] = &HashNode{
		Key:   key,
		Value: value,
		Next:  ht.table[index],
	}
}

// Get возвращает значение по ключу
func (ht *HashTable) Get(key string) string {
	index := ht.hashFunction(key)

	current := ht.table[index]
	for current != nil {
		if current.Key == key {
			return current.Value
		}
		current = current.Next
	}
	return "NOT_FOUND"
}

// Remove удаляет пару по ключу
func (ht *HashTable) Remove(key string) bool {
	index := ht.hashFunction(key)

	current := ht.table[index]
	var prev *HashNode

	for current != nil {
		if current.Key == key {
			if prev == nil {
				ht.table[index] = current.Next
			} else {
				prev.Next = current.Next
			}
			return true
		}
		prev = current
		current = current.Next
	}
	return false
}

// Print выводит содержимое таблицы
func (ht *HashTable) Print() {
	for i := 0; i < tableSize; i++ {
		fmt.Printf("Bucket %d: ", i)
		current := ht.table[i]
		for current != nil {
			fmt.Printf("[%s: %s] ", current.Key, current.Value)
			current = current.Next
		}
		fmt.Println()
	}
}

// SaveToFile сохраняет таблицу в файл
func (ht *HashTable) SaveToFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	for i := 0; i < tableSize; i++ {
		current := ht.table[i]
		for current != nil {
			if _, err := fmt.Fprintf(file, "%s:%s\n", current.Key, current.Value); err != nil {
				return err
			}
			current = current.Next
		}
	}
	return nil
}

// LoadFromFile загружает таблицу из файла
func (ht *HashTable) LoadFromFile(filename string) error {
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
		parts := strings.Fields(scanner.Text())
		if len(parts) == 2 {
			ht.Insert(parts[0], parts[1])
		}
	}
	return scanner.Err()
}

// RunHashTable выполняет команды над хеш-таблицей
func RunHashTable(args []string) {
	ht := NewHashTable()
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
		ht.LoadFromFile(filename)
	}

	parts := strings.SplitN(query, " ", 3)
	command := parts[0]
	var key, value string
	if len(parts) > 1 {
		key = parts[1]
	}
	if len(parts) > 2 {
		value = parts[2]
	}

	switch command {
	case "HSET":
		if key != "" && value != "" {
			ht.Insert(key, value)
			if filename != "" {
				ht.SaveToFile(filename)
			}
		}
	case "HGET":
		if key != "" {
			fmt.Println(ht.Get(key))
		}
	case "HDEL":
		if key != "" {
			ht.Remove(key)
			if filename != "" {
				ht.SaveToFile(filename)
			}
		}
	case "HPRINT":
		ht.Print()
	}
}
