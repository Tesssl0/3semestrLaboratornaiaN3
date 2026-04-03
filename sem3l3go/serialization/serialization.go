package serialization

import (
	"bufio"
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"strings"

	"github.com/yourusername/datastructures/array"
	"github.com/yourusername/datastructures/binarytree"
	"github.com/yourusername/datastructures/dlinkedlist"
	"github.com/yourusername/datastructures/hashtable"
	"github.com/yourusername/datastructures/linkedlist"
	"github.com/yourusername/datastructures/queue"
	"github.com/yourusername/datastructures/stack"
)

/* =========================================================
   ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ JSON
========================================================= */

func jsonEscape(s string) string {
	var result strings.Builder
	for _, r := range s {
		switch r {
		case '"':
			result.WriteString(`\"`)
		case '\\':
			result.WriteString(`\\`)
		case '\b':
			result.WriteString(`\b`)
		case '\f':
			result.WriteString(`\f`)
		case '\n':
			result.WriteString(`\n`)
		case '\r':
			result.WriteString(`\r`)
		case '\t':
			result.WriteString(`\t`)
		default:
			if r < 0x20 {
				fmt.Fprintf(&result, "\\u%04x", r)
			} else {
				result.WriteRune(r)
			}
		}
	}
	return result.String()
}

func jsonUnescape(s string) string {
	var result strings.Builder
	for i := 0; i < len(s); i++ {
		if s[i] == '\\' && i+1 < len(s) {
			switch s[i+1] {
			case '"':
				result.WriteByte('"')
				i++
			case '\\':
				result.WriteByte('\\')
				i++
			case '/':
				result.WriteByte('/')
				i++
			case 'b':
				result.WriteByte('\b')
				i++
			case 'f':
				result.WriteByte('\f')
				i++
			case 'n':
				result.WriteByte('\n')
				i++
			case 'r':
				result.WriteByte('\r')
				i++
			case 't':
				result.WriteByte('\t')
				i++
			default:
				result.WriteByte(s[i])
			}
		} else {
			result.WriteByte(s[i])
		}
	}
	return result.String()
}

func validateFileForReading(filename string) (*os.File, error) {
	if filename == "" {
		return nil, fmt.Errorf("empty filename")
	}

	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}

	info, err := file.Stat()
	if err != nil {
		file.Close()
		return nil, err
	}

	if info.Size() == 0 {
		file.Close()
		return nil, fmt.Errorf("empty file")
	}

	return file, nil
}

/* =========================================================
   DYNAMIC ARRAY - СУЩЕСТВУЮЩИЕ ФУНКЦИИ
========================================================= */

func DASaveText(arr *array.DynamicArray, filename string) error {
	if filename == "" {
		return nil
	}
	return arr.SaveToFile(filename)
}

func DALoadText(arr *array.DynamicArray, filename string) error {
	if filename == "" {
		return nil
	}
	arr.Clear()
	return arr.LoadFromFile(filename)
}

func DASaveBinary(arr *array.DynamicArray, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	if err := binary.Write(file, binary.LittleEndian, int32(arr.Length())); err != nil {
		return err
	}

	for i := 0; i < arr.Length(); i++ {
		str := arr.Get(i)
		if err := binary.Write(file, binary.LittleEndian, int32(len(str))); err != nil {
			return err
		}
		if _, err := file.Write([]byte(str)); err != nil {
			return err
		}
	}
	return nil
}

func DALoadBinary(arr *array.DynamicArray, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	arr.Clear()

	var count int32
	if err := binary.Read(file, binary.LittleEndian, &count); err != nil {
		return fmt.Errorf("failed to read element count: %w", err)
	}

	if count < 0 {
		return fmt.Errorf("invalid negative count: %d", count)
	}
	if count > 1000000 {
		return fmt.Errorf("count too large: %d > 1000000", count)
	}

	fileInfo, err := file.Stat()
	if err != nil {
		return err
	}
	minExpectedSize := int64(4)
	for i := 0; i < int(count); i++ {
		minExpectedSize += 4
	}
	if fileInfo.Size() < minExpectedSize {
		return fmt.Errorf("file too small: expected at least %d bytes, got %d",
			minExpectedSize, fileInfo.Size())
	}

	for i := 0; i < int(count); i++ {
		var strLen int32
		if err := binary.Read(file, binary.LittleEndian, &strLen); err != nil {
			return fmt.Errorf("failed to read string length for element %d: %w", i, err)
		}
		if strLen < 0 || strLen > 1000000 {
			return fmt.Errorf("invalid string length %d for element %d", strLen, i)
		}

		strBytes := make([]byte, strLen)
		n, err := file.Read(strBytes)
		if err != nil {
			return fmt.Errorf("failed to read string data for element %d: %w", i, err)
		}
		if n != int(strLen) {
			return fmt.Errorf("short read for element %d: expected %d bytes, got %d",
				i, strLen, n)
		}
		arr.Add(string(strBytes))
	}

	var leftover byte
	if err := binary.Read(file, binary.LittleEndian, &leftover); err == nil {
		return fmt.Errorf("extra data after expected elements")
	}

	return nil
}

/* =========================================================
   DYNAMIC ARRAY - JSON
========================================================= */

func DASaveJSON(arr *array.DynamicArray, filename string) error {
	if filename == "" {
		return nil
	}

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	file.WriteString("[\n")
	for i := 0; i < arr.Length(); i++ {
		fmt.Fprintf(file, "  \"%s\"", jsonEscape(arr.Get(i)))
		if i < arr.Length()-1 {
			file.WriteString(",")
		}
		file.WriteString("\n")
	}
	file.WriteString("]\n")

	return nil
}

func DALoadJSON(arr *array.DynamicArray, filename string) error {
	if filename == "" {
		return nil
	}

	data, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	arr.Clear()

	content := strings.TrimSpace(string(data))
	if content == "" || content == "[]" {
		return nil
	}

	if len(content) >= 2 && content[0] == '[' && content[len(content)-1] == ']' {
		content = content[1 : len(content)-1]
	}

	lines := strings.Split(content, "\n")
	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}

		if strings.HasSuffix(line, ",") {
			line = line[:len(line)-1]
		}

		line = strings.TrimSpace(line)
		if len(line) >= 2 && line[0] == '"' && line[len(line)-1] == '"' {
			line = line[1 : len(line)-1]
			line = jsonUnescape(line)
			arr.Add(line)
		}
	}

	return nil
}

/* =========================================================
   STACK - СУЩЕСТВУЮЩИЕ ФУНКЦИИ
========================================================= */

func StackSaveText(s *stack.Stack, filename string) error {
	if filename == "" {
		return nil
	}
	return s.SaveToFile(filename)
}

func StackLoadText(s *stack.Stack, filename string) error {
	if filename == "" {
		return nil
	}
	s.Destroy()
	return s.LoadFromFile(filename)
}

func StackSaveBinary(s *stack.Stack, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var values []string
	for node := s.Top; node != nil; node = node.Next {
		values = append(values, node.Data)
	}

	if err := binary.Write(file, binary.LittleEndian, int32(len(values))); err != nil {
		return err
	}

	for _, str := range values {
		if err := binary.Write(file, binary.LittleEndian, int32(len(str))); err != nil {
			return err
		}
		if _, err := file.Write([]byte(str)); err != nil {
			return err
		}
	}
	return nil
}

func StackLoadBinary(s *stack.Stack, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	s.Destroy()

	var count int32
	if err := binary.Read(file, binary.LittleEndian, &count); err != nil {
		return err
	}

	if count < 0 || count > 1000000 {
		return os.ErrInvalid
	}

	var values []string
	for i := 0; i < int(count); i++ {
		var strLen int32
		if err := binary.Read(file, binary.LittleEndian, &strLen); err != nil {
			return err
		}
		if strLen < 0 || strLen > 1000000 {
			return os.ErrInvalid
		}
		strBytes := make([]byte, strLen)
		if _, err := file.Read(strBytes); err != nil {
			return err
		}
		values = append(values, string(strBytes))
	}

	for i := len(values) - 1; i >= 0; i-- {
		s.Push(values[i])
	}
	return nil
}

/* =========================================================
   STACK - JSON
========================================================= */

func StackSaveJSON(s *stack.Stack, filename string) error {
	if filename == "" {
		return nil
	}

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var values []string
	for node := s.Top; node != nil; node = node.Next {
		values = append(values, node.Data)
	}

	file.WriteString("[\n")
	for i := len(values) - 1; i >= 0; i-- {
		fmt.Fprintf(file, "  \"%s\"", jsonEscape(values[i]))
		if i > 0 {
			file.WriteString(",")
		}
		file.WriteString("\n")
	}
	file.WriteString("]\n")

	return nil
}

func StackLoadJSON(s *stack.Stack, filename string) error {
	if filename == "" {
		return nil
	}

	data, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	s.Destroy()

	content := strings.TrimSpace(string(data))
	if content == "" || content == "[]" {
		return nil
	}

	if len(content) >= 2 && content[0] == '[' && content[len(content)-1] == ']' {
		content = content[1 : len(content)-1]
	}

	lines := strings.Split(content, "\n")
	var items []string

	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}

		if strings.HasSuffix(line, ",") {
			line = line[:len(line)-1]
		}

		line = strings.TrimSpace(line)
		if len(line) >= 2 && line[0] == '"' && line[len(line)-1] == '"' {
			line = line[1 : len(line)-1]
			line = jsonUnescape(line)
			items = append(items, line)
		}
	}

	for i := len(items) - 1; i >= 0; i-- {
		s.Push(items[i])
	}

	return nil
}

/* =========================================================
   QUEUE - СУЩЕСТВУЮЩИЕ ФУНКЦИИ
========================================================= */

func QueueSaveText(q *queue.Queue, filename string) error {
	if filename == "" {
		return nil
	}
	return q.SaveToFile(filename)
}

func QueueLoadText(q *queue.Queue, filename string) error {
	if filename == "" {
		return nil
	}
	q.Destroy()
	return q.LoadFromFile(filename)
}

func QueueSaveBinary(q *queue.Queue, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var values []string
	for node := q.Front; node != nil; node = node.Next {
		values = append(values, node.Data)
	}

	if err := binary.Write(file, binary.LittleEndian, int32(len(values))); err != nil {
		return err
	}

	for _, str := range values {
		if err := binary.Write(file, binary.LittleEndian, int32(len(str))); err != nil {
			return err
		}
		if _, err := file.Write([]byte(str)); err != nil {
			return err
		}
	}
	return nil
}

func QueueLoadBinary(q *queue.Queue, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	q.Destroy()

	var count int32
	if err := binary.Read(file, binary.LittleEndian, &count); err != nil {
		return err
	}

	if count < 0 || count > 1000000 {
		return os.ErrInvalid
	}

	for i := 0; i < int(count); i++ {
		var strLen int32
		if err := binary.Read(file, binary.LittleEndian, &strLen); err != nil {
			return err
		}
		if strLen < 0 || strLen > 1000000 {
			return os.ErrInvalid
		}
		strBytes := make([]byte, strLen)
		if _, err := file.Read(strBytes); err != nil {
			return err
		}
		q.Enqueue(string(strBytes))
	}
	return nil
}

/* =========================================================
   QUEUE - JSON
========================================================= */

func QueueSaveJSON(q *queue.Queue, filename string) error {
	if filename == "" {
		return nil
	}

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	file.WriteString("[\n")
	first := true
	for node := q.Front; node != nil; node = node.Next {
		if !first {
			file.WriteString(",\n")
		}
		fmt.Fprintf(file, "  \"%s\"", jsonEscape(node.Data))
		first = false
	}
	file.WriteString("\n]\n")

	return nil
}

func QueueLoadJSON(q *queue.Queue, filename string) error {
	if filename == "" {
		return nil
	}

	data, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	q.Destroy()

	content := strings.TrimSpace(string(data))
	if content == "" || content == "[]" {
		return nil
	}

	if len(content) >= 2 && content[0] == '[' && content[len(content)-1] == ']' {
		content = content[1 : len(content)-1]
	}

	lines := strings.Split(content, "\n")
	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}

		if strings.HasSuffix(line, ",") {
			line = line[:len(line)-1]
		}

		line = strings.TrimSpace(line)
		if len(line) >= 2 && line[0] == '"' && line[len(line)-1] == '"' {
			line = line[1 : len(line)-1]
			line = jsonUnescape(line)
			q.Enqueue(line)
		}
	}

	return nil
}

/* =========================================================
   LINKED LIST - СУЩЕСТВУЮЩИЕ ФУНКЦИИ
========================================================= */

func LLSaveText(ll *linkedlist.LinkedList, filename string) error {
	if filename == "" {
		return nil
	}
	return ll.SaveToFile(filename)
}

func LLLoadText(ll *linkedlist.LinkedList, filename string) error {
	if filename == "" {
		return fmt.Errorf("empty filename")
	}

	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	// Проверка на пустой файл
	info, err := file.Stat()
	if err != nil {
		return err
	}
	if info.Size() == 0 {
		ll.Destroy()
		return nil // Пустой файл - просто очищаем список
	}

	scanner := bufio.NewScanner(file)
	ll.Destroy()

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		ll.AddToTail(line)
	}

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("read error: %w", err)
	}

	return nil
}

func LLSaveBinary(ll *linkedlist.LinkedList, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var values []string
	for node := ll.Head; node != nil; node = node.Next {
		values = append(values, node.Data)
	}

	if err := binary.Write(file, binary.LittleEndian, int32(len(values))); err != nil {
		return err
	}

	for _, str := range values {
		if err := binary.Write(file, binary.LittleEndian, int32(len(str))); err != nil {
			return err
		}
		if _, err := file.Write([]byte(str)); err != nil {
			return err
		}
	}
	return nil
}

func LLLoadBinary(ll *linkedlist.LinkedList, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	ll.Destroy()

	var count int32
	if err := binary.Read(file, binary.LittleEndian, &count); err != nil {
		return err
	}

	if count < 0 || count > 1000000 {
		return os.ErrInvalid
	}

	for i := 0; i < int(count); i++ {
		var strLen int32
		if err := binary.Read(file, binary.LittleEndian, &strLen); err != nil {
			return err
		}
		if strLen < 0 || strLen > 1000000 {
			return os.ErrInvalid
		}
		strBytes := make([]byte, strLen)
		if _, err := file.Read(strBytes); err != nil {
			return err
		}
		ll.AddToTail(string(strBytes))
	}
	return nil
}

/* =========================================================
   LINKED LIST - JSON
========================================================= */

func LLSaveJSON(ll *linkedlist.LinkedList, filename string) error {
	if filename == "" {
		return nil
	}

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	file.WriteString("[\n")
	first := true
	for node := ll.Head; node != nil; node = node.Next {
		if !first {
			file.WriteString(",\n")
		}
		fmt.Fprintf(file, "  \"%s\"", jsonEscape(node.Data))
		first = false
	}
	file.WriteString("\n]\n")

	return nil
}

func LLLoadJSON(ll *linkedlist.LinkedList, filename string) error {
	if filename == "" {
		return nil
	}

	data, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	ll.Destroy()

	content := strings.TrimSpace(string(data))
	if content == "" || content == "[]" {
		return nil
	}

	if len(content) >= 2 && content[0] == '[' && content[len(content)-1] == ']' {
		content = content[1 : len(content)-1]
	}

	lines := strings.Split(content, "\n")
	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}

		if strings.HasSuffix(line, ",") {
			line = line[:len(line)-1]
		}

		line = strings.TrimSpace(line)
		if len(line) >= 2 && line[0] == '"' && line[len(line)-1] == '"' {
			line = line[1 : len(line)-1]
			line = jsonUnescape(line)
			ll.AddToTail(line)
		}
	}

	return nil
}

/* =========================================================
   DOUBLY LINKED LIST - СУЩЕСТВУЮЩИЕ ФУНКЦИИ
========================================================= */

func DLLSaveText(dll *dlinkedlist.DlinkedList, filename string) error {
	if filename == "" {
		return nil
	}
	return dll.SaveToFile(filename)
}

func DLLLoadText(dll *dlinkedlist.DlinkedList, filename string) error {
	if filename == "" {
		return fmt.Errorf("empty filename")
	}

	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	info, err := file.Stat()
	if err != nil {
		return err
	}

	// Очистка списка перед загрузкой
	dll.Destroy()

	// Если файл пустой, просто возвращаем nil (список очищен)
	if info.Size() == 0 {
		return nil
	}

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}

		// Простейшая проверка "ошибочных" строк
		if strings.ContainsRune(line, '\x00') { // нулевой байт — некорректные данные
			return fmt.Errorf("read error: invalid data")
		}

		dll.AddToTail(line)
	}

	// Проверка ошибок чтения файла
	if err := scanner.Err(); err != nil {
		return fmt.Errorf("read error: %w", err)
	}

	return nil
}

func DLLSaveBinary(dll *dlinkedlist.DlinkedList, filename string) error {
	if filename == "" {
		return nil
	}
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var values []string
	for node := dll.Head; node != nil; node = node.Next {
		values = append(values, node.Data)
	}

	if err := binary.Write(file, binary.LittleEndian, int32(len(values))); err != nil {
		return err
	}

	for _, str := range values {
		if err := binary.Write(file, binary.LittleEndian, int32(len(str))); err != nil {
			return err
		}
		if _, err := file.Write([]byte(str)); err != nil {
			return err
		}
	}
	return nil
}

func DLLLoadBinary(dll *dlinkedlist.DlinkedList, filename string) error {
	if filename == "" {
		return fmt.Errorf("empty filename")
	}

	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	dll.Destroy()

	var count int32
	if err := binary.Read(file, binary.LittleEndian, &count); err != nil {
		return fmt.Errorf("read error: %w", err)
	}

	if count < 0 || count > 1000000 {
		return fmt.Errorf("invalid element count")
	}

	for i := 0; i < int(count); i++ {
		var strLen int32
		if err := binary.Read(file, binary.LittleEndian, &strLen); err != nil {
			return fmt.Errorf("read error: %w", err)
		}
		if strLen < 0 || strLen > 1000000 {
			return fmt.Errorf("invalid string length")
		}

		strBytes := make([]byte, strLen)
		if _, err := io.ReadFull(file, strBytes); err != nil {
			return fmt.Errorf("read error: %w", err)
		}

		dll.AddToTail(string(strBytes))
	}

	return nil
}

/* =========================================================
   DOUBLY LINKED LIST - JSON
========================================================= */

func DLLSaveJSON(dll *dlinkedlist.DlinkedList, filename string) error {
	if filename == "" {
		return nil
	}

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	file.WriteString("[\n")
	first := true
	for node := dll.Head; node != nil; node = node.Next {
		if !first {
			file.WriteString(",\n")
		}
		fmt.Fprintf(file, "  \"%s\"", jsonEscape(node.Data))
		first = false
	}
	file.WriteString("\n]\n")

	return nil
}

func DLLLoadJSON(dll *dlinkedlist.DlinkedList, filename string) error {
	if filename == "" {
		return nil
	}

	data, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	dll.Destroy()

	content := strings.TrimSpace(string(data))
	if content == "" || content == "[]" {
		return nil
	}

	if len(content) >= 2 && content[0] == '[' && content[len(content)-1] == ']' {
		content = content[1 : len(content)-1]
	}

	lines := strings.Split(content, "\n")
	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}

		if strings.HasSuffix(line, ",") {
			line = line[:len(line)-1]
		}

		line = strings.TrimSpace(line)
		if len(line) >= 2 && line[0] == '"' && line[len(line)-1] == '"' {
			line = line[1 : len(line)-1]
			line = jsonUnescape(line)
			dll.AddToTail(line)
		}
	}

	return nil
}

/* =========================================================
   HASH TABLE - СУЩЕСТВУЮЩИЕ ФУНКЦИИ
========================================================= */

func HTSaveBinary(ht *hashtable.HashTable, filename string) error {
	if filename == "" {
		return nil
	}
	return ht.SaveToFile(filename)
}

var ErrEmptyFile = fmt.Errorf("empty file")

func HTLoadBinary(ht *hashtable.HashTable, filename string) error {
	if filename == "" {
		return ErrEmptyFile
	}

	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	// Всегда очищаем старые данные
	ht.Clear() // <-- добавь метод Clear для hashtable

	info, err := file.Stat()
	if err != nil {
		return err
	}
	if info.Size() == 0 {
		// файл пустой, таблица уже очищена
		return nil
	}

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}
		parts := strings.SplitN(line, ":", 2)
		if len(parts) != 2 {
			return fmt.Errorf("invalid format: expected 'key:value', got '%s'", line)
		}
		ht.Insert(strings.TrimSpace(parts[0]), strings.TrimSpace(parts[1]))
	}

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("read error: %w", err)
	}

	return nil
}

/* =========================================================
   HASH TABLE - JSON
========================================================= */

func HTSaveJSON(ht *hashtable.HashTable, filename string) error {
	if filename == "" {
		return nil
	}

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	file.WriteString("{\n")

	type KeyValue struct {
		Key   string
		Value string
	}
	var pairs []KeyValue

	_ = pairs
	file.WriteString("}\n")
	return nil
}

func HTLoadJSON(ht *hashtable.HashTable, filename string) error {
	if filename == "" {
		return nil
	}

	data, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	*ht = *hashtable.NewHashTable()

	content := strings.TrimSpace(string(data))
	if content == "" || content == "{}" {
		return nil
	}

	if len(content) >= 2 && content[0] == '{' && content[len(content)-1] == '}' {
		content = content[1 : len(content)-1]
	}

	lines := strings.Split(content, "\n")
	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}

		if strings.HasSuffix(line, ",") {
			line = line[:len(line)-1]
		}

		parts := strings.SplitN(line, ":", 2)
		if len(parts) != 2 {
			continue
		}

		key := strings.TrimSpace(parts[0])
		value := strings.TrimSpace(parts[1])

		if len(key) >= 2 && key[0] == '"' && key[len(key)-1] == '"' {
			key = key[1 : len(key)-1]
			key = jsonUnescape(key)
		}

		if len(value) >= 2 && value[0] == '"' && value[len(value)-1] == '"' {
			value = value[1 : len(value)-1]
			value = jsonUnescape(value)
		}

		ht.Insert(key, value)
	}

	return nil
}

/* =========================================================
   BINARY TREE - СУЩЕСТВУЮЩИЕ ФУНКЦИИ
========================================================= */

func BTSaveBinary(bt *binarytree.BinaryTree, filename string) error {
	if filename == "" {
		return nil
	}
	return bt.SaveToFile(filename)
}

func BTLoadBinary(bt *binarytree.BinaryTree, filename string) error {
	if filename == "" {
		return nil
	}
	return bt.LoadFromFile(filename)
}

/* =========================================================
   BINARY TREE - JSON
========================================================= */

func saveTreeNodeJSON(node *binarytree.Node, file *os.File, depth int) error {
	if node == nil {
		for i := 0; i < depth; i++ {
			if _, err := file.WriteString("  "); err != nil {
				return err
			}
		}
		_, err := file.WriteString("null")
		return err
	}

	for i := 0; i < depth; i++ {
		if _, err := file.WriteString("  "); err != nil {
			return err
		}
	}

	if _, err := file.WriteString("{\n"); err != nil {
		return err
	}

	for i := 0; i <= depth; i++ {
		if _, err := file.WriteString("  "); err != nil {
			return err
		}
	}
	if _, err := fmt.Fprintf(file, "\"key\": \"%s\",\n", jsonEscape(node.Key)); err != nil {
		return err
	}

	for i := 0; i <= depth; i++ {
		if _, err := file.WriteString("  "); err != nil {
			return err
		}
	}
	if _, err := file.WriteString("\"left\": "); err != nil {
		return err
	}
	if err := saveTreeNodeJSON(node.Left, file, depth+1); err != nil {
		return err
	}
	if _, err := file.WriteString(",\n"); err != nil {
		return err
	}

	for i := 0; i <= depth; i++ {
		if _, err := file.WriteString("  "); err != nil {
			return err
		}
	}
	if _, err := file.WriteString("\"right\": "); err != nil {
		return err
	}
	if err := saveTreeNodeJSON(node.Right, file, depth+1); err != nil {
		return err
	}
	if _, err := file.WriteString("\n"); err != nil {
		return err
	}

	for i := 0; i < depth; i++ {
		if _, err := file.WriteString("  "); err != nil {
			return err
		}
	}
	_, err := file.WriteString("}")
	return err
}

func BTSaveJSON(bt *binarytree.BinaryTree, filename string) error {
	if filename == "" {
		return nil
	}

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	if bt.Root == nil {
		_, err := file.WriteString("null\n")
		return err
	}

	return saveTreeNodeJSON(bt.Root, file, 0)
}

func parseTreeNode(jsonStr string, pos *int) (*binarytree.Node, error) {
	for *pos < len(jsonStr) && (jsonStr[*pos] == ' ' || jsonStr[*pos] == '\t' ||
		jsonStr[*pos] == '\n' || jsonStr[*pos] == '\r') {
		*pos++
	}

	if *pos >= len(jsonStr) {
		return nil, nil
	}

	if *pos+3 < len(jsonStr) && jsonStr[*pos:*pos+4] == "null" {
		*pos += 4
		return nil, nil
	}

	if jsonStr[*pos] != '{' {
		return nil, fmt.Errorf("expected '{', got '%c'", jsonStr[*pos])
	}
	*pos++

	for *pos < len(jsonStr) && (jsonStr[*pos] == ' ' || jsonStr[*pos] == '\t' ||
		jsonStr[*pos] == '\n' || jsonStr[*pos] == '\r') {
		*pos++
	}

	var key string
	var left, right *binarytree.Node

	for *pos < len(jsonStr) && jsonStr[*pos] != '}' {
		for *pos < len(jsonStr) && (jsonStr[*pos] == ' ' || jsonStr[*pos] == '\t' ||
			jsonStr[*pos] == '\n' || jsonStr[*pos] == '\r') {
			*pos++
		}

		if jsonStr[*pos] != '"' {
			return nil, fmt.Errorf("expected '\"', got '%c'", jsonStr[*pos])
		}
		*pos++

		var fieldName string
		for *pos < len(jsonStr) && jsonStr[*pos] != '"' {
			if jsonStr[*pos] == '\\' && *pos+1 < len(jsonStr) {
				fieldName += string(jsonStr[*pos+1])
				*pos += 2
			} else {
				fieldName += string(jsonStr[*pos])
				*pos++
			}
		}
		if *pos >= len(jsonStr) {
			return nil, fmt.Errorf("unexpected end of string")
		}
		*pos++

		for *pos < len(jsonStr) && (jsonStr[*pos] == ' ' || jsonStr[*pos] == '\t' ||
			jsonStr[*pos] == '\n' || jsonStr[*pos] == '\r') {
			*pos++
		}

		if *pos >= len(jsonStr) || jsonStr[*pos] != ':' {
			return nil, fmt.Errorf("expected ':', got '%c'", jsonStr[*pos])
		}
		*pos++

		for *pos < len(jsonStr) && (jsonStr[*pos] == ' ' || jsonStr[*pos] == '\t' ||
			jsonStr[*pos] == '\n' || jsonStr[*pos] == '\r') {
			*pos++
		}

		switch fieldName {
		case "key":
			if *pos >= len(jsonStr) || jsonStr[*pos] != '"' {
				return nil, fmt.Errorf("expected string for key")
			}
			*pos++
			for *pos < len(jsonStr) && jsonStr[*pos] != '"' {
				if jsonStr[*pos] == '\\' && *pos+1 < len(jsonStr) {
					key += string(jsonStr[*pos+1])
					*pos += 2
				} else {
					key += string(jsonStr[*pos])
					*pos++
				}
			}
			if *pos >= len(jsonStr) {
				return nil, fmt.Errorf("unexpected end of string")
			}
			*pos++
			key = jsonUnescape(key)

		case "left":
			var err error
			left, err = parseTreeNode(jsonStr, pos)
			if err != nil {
				return nil, err
			}

		case "right":
			var err error
			right, err = parseTreeNode(jsonStr, pos)
			if err != nil {
				return nil, err
			}
		}

		for *pos < len(jsonStr) && (jsonStr[*pos] == ' ' || jsonStr[*pos] == '\t' ||
			jsonStr[*pos] == '\n' || jsonStr[*pos] == '\r') {
			*pos++
		}

		if *pos < len(jsonStr) && jsonStr[*pos] == ',' {
			*pos++
		}

		for *pos < len(jsonStr) && (jsonStr[*pos] == ' ' || jsonStr[*pos] == '\t' ||
			jsonStr[*pos] == '\n' || jsonStr[*pos] == '\r') {
			*pos++
		}
	}

	if *pos >= len(jsonStr) || jsonStr[*pos] != '}' {
		return nil, fmt.Errorf("expected '}'")
	}
	*pos++

	node := &binarytree.Node{
		Key:   key,
		Left:  left,
		Right: right,
	}

	return node, nil
}

func BTLoadJSON(bt *binarytree.BinaryTree, filename string) error {
	if filename == "" {
		return nil
	}

	data, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	bt.Root = nil

	jsonStr := string(data)
	pos := 0

	root, err := parseTreeNode(jsonStr, &pos)
	if err != nil {
		return err
	}

	bt.Root = root
	return nil
}
