package array

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// DynamicArray представляет динамический массив строк
type DynamicArray struct {
	data     []string
	size     int
	capacity int
}

// NewDynamicArray создает новый динамический массив
func NewDynamicArray(initialCapacity int) *DynamicArray {
	if initialCapacity <= 0 {
		initialCapacity = 10
	}
	return &DynamicArray{
		data:     make([]string, initialCapacity),
		size:     0,
		capacity: initialCapacity,
	}
}

// resize изменяет емкость массива
func (da *DynamicArray) resize(newCapacity int) {
	newData := make([]string, newCapacity)
	copy(newData, da.data[:da.size])
	da.data = newData
	da.capacity = newCapacity
}

// Add добавляет элемент в конец массива
func (da *DynamicArray) Add(value string) {
	if da.size == da.capacity {
		da.resize(da.capacity * 2)
	}
	da.data[da.size] = value
	da.size++
}

// Insert вставляет элемент в заданную позицию
func (da *DynamicArray) Insert(index int, value string) bool {
	if index < 0 || index > da.size {
		return false
	}
	if da.size == da.capacity {
		da.resize(da.capacity * 2)
	}
	for i := da.size; i > index; i-- {
		da.data[i] = da.data[i-1]
	}
	da.data[index] = value
	da.size++
	return true
}

// Remove удаляет элемент по индексу
func (da *DynamicArray) Remove(index int) bool {
	if index < 0 || index >= da.size {
		return false
	}
	for i := index; i < da.size-1; i++ {
		da.data[i] = da.data[i+1]
	}
	da.size--
	return true
}

// Get возвращает элемент по индексу
func (da *DynamicArray) Get(index int) string {
	if index < 0 || index >= da.size {
		return ""
	}
	return da.data[index]
}

// Set устанавливает значение элемента по индексу
func (da *DynamicArray) Set(index int, value string) bool {
	if index < 0 || index >= da.size {
		return false
	}
	da.data[index] = value
	return true
}

// Length возвращает текущий размер массива
func (da *DynamicArray) Length() int {
	return da.size
}

// Print выводит все элементы массива
func (da *DynamicArray) Print() {
	for i := 0; i < da.size; i++ {
		fmt.Printf("%s ", da.data[i])
	}
	fmt.Println()
}

// Clear очищает массив
func (da *DynamicArray) Clear() {
	da.data = make([]string, 10)
	da.size = 0
	da.capacity = 10
}

// LoadFromFile загружает данные из файла
func (da *DynamicArray) LoadFromFile(filename string) error {
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
		da.Add(scanner.Text())
	}
	return scanner.Err()
}

// SaveToFile сохраняет данные в файл
func (da *DynamicArray) SaveToFile(filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	for i := 0; i < da.size; i++ {
		if _, err := fmt.Fprintln(file, da.data[i]); err != nil {
			return err
		}
	}
	return nil
}

// RunDynamicArray выполняет команды над массивом
func RunDynamicArray(args []string) {
	arr := NewDynamicArray(10)
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
		arr.LoadFromFile(filename)
	}

	parts := strings.SplitN(query, " ", 2)
	command := parts[0]
	rest := ""
	if len(parts) > 1 {
		rest = parts[1]
	}

	switch command {
	case "MPUSH":
		arr.Add(rest)
		if filename != "" {
			arr.SaveToFile(filename)
		}
	case "MINSERT":
		subParts := strings.SplitN(rest, " ", 2)
		if len(subParts) == 2 {
			index, _ := strconv.Atoi(subParts[0])
			arr.Insert(index, subParts[1])
			if filename != "" {
				arr.SaveToFile(filename)
			}
		}
	case "MDEL":
		index, _ := strconv.Atoi(rest)
		arr.Remove(index)
		if filename != "" {
			arr.SaveToFile(filename)
		}
	case "MSET":
		subParts := strings.SplitN(rest, " ", 2)
		if len(subParts) == 2 {
			index, _ := strconv.Atoi(subParts[0])
			arr.Set(index, subParts[1])
			if filename != "" {
				arr.SaveToFile(filename)
			}
		}
	case "MLEN":
		fmt.Println(arr.Length())
	case "MPRINT":
		arr.Print()
	case "MGET":
		index, _ := strconv.Atoi(rest)
		fmt.Println(arr.Get(index))
	}
}
