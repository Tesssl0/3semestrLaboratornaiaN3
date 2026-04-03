package hashtable

import (
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
)

type HashTableTestSuite struct {
	suite.Suite
	ht *HashTable
}

func (s *HashTableTestSuite) SetupTest() {
	s.ht = NewHashTable()
}

func (s *HashTableTestSuite) TearDownTest() {
	s.ht = nil
}

func (s *HashTableTestSuite) TestInsertAndGet() {
	s.ht.Insert("key1", "value1")
	s.ht.Insert("key2", "value2")

	assert.Equal(s.T(), "value1", s.ht.Get("key1"))
	assert.Equal(s.T(), "value2", s.ht.Get("key2"))
	assert.Equal(s.T(), "NOT_FOUND", s.ht.Get("nonexistent"))
}

func (s *HashTableTestSuite) TestUpdate() {
	s.ht.Insert("key", "old")
	s.ht.Insert("key", "new")

	assert.Equal(s.T(), "new", s.ht.Get("key"))
}

func (s *HashTableTestSuite) TestRemove() {
	s.ht.Insert("a", "1")
	s.ht.Insert("b", "2")

	ok := s.ht.Remove("a")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "NOT_FOUND", s.ht.Get("a"))
	assert.Equal(s.T(), "2", s.ht.Get("b"))

	ok = s.ht.Remove("x")
	assert.False(s.T(), ok)
}

func (s *HashTableTestSuite) TestRemoveFromHead() {
	// Вставляем несколько ключей для гарантии коллизии
	keys := []string{"a", "b", "c", "d", "e"}
	for _, key := range keys {
		s.ht.Insert(key, "val")
	}

	// Удаляем первый элемент
	ok := s.ht.Remove("a")
	assert.True(s.T(), ok)
}

func (s *HashTableTestSuite) TestCollision() {
	// Создаем ключи, используем числа
	for i := 0; i < 20; i++ {
		key := string(rune(i))
		s.ht.Insert(key, "val")
	}

	// Проверяем, что все ключи доступны
	for i := 0; i < 20; i++ {
		key := string(rune(i))
		assert.NotEqual(s.T(), "NOT_FOUND", s.ht.Get(key))
	}
}

func (s *HashTableTestSuite) TestPrint() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Пустая таблица
	s.ht.Print()

	// Таблица с данными
	s.ht.Insert("key1", "value1")
	s.ht.Insert("key2", "value2")
	s.ht.Print()

	w.Close()
	os.Stdout = old
	// Читаем вывод чтобы "использовать" r
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

/*
	func (s *HashTableTestSuite) TestFileIO() {
		filename := "test_hash.txt"
		defer os.Remove(filename)

		s.ht.Insert("hello", "world")
		s.ht.Insert("foo", "bar")
		err := s.ht.SaveToFile(filename)
		assert.NoError(s.T(), err)

		newHT := NewHashTable()
		err = newHT.LoadFromFile(filename)
		assert.NoError(s.T(), err)

		assert.Equal(s.T(), "world", newHT.Get("hello"))
		assert.Equal(s.T(), "bar", newHT.Get("foo"))
	}
*/
func (s *HashTableTestSuite) TestFileIOEmptyFilename() {
	// Сохранение с пустым именем
	err := s.ht.SaveToFile("")
	assert.NoError(s.T(), err)

	// Загрузка с пустым именем
	err = s.ht.LoadFromFile("")
	assert.NoError(s.T(), err)
}

func (s *HashTableTestSuite) TestFileIONotFound() {
	// Загрузка несуществующего файла
	err := s.ht.LoadFromFile("nonexistent.txt")
	assert.Error(s.T(), err)
}

/*
	func (s *HashTableTestSuite) TestFileIOInvalidFormat() {
		// Создаем файл с неправильным форматом
		filename := "invalid.txt"
		defer os.Remove(filename)

		file, _ := os.Create(filename)
		file.WriteString("invalid line without space\n")
		file.WriteString("valid key value\n")
		file.Close()

		newHT := NewHashTable()
		err := newHT.LoadFromFile(filename)
		assert.NoError(s.T(), err) // Должен проигнорировать неправильную строку
		assert.Equal(s.T(), "value", newHT.Get("valid"))
	}
*/
func (s *HashTableTestSuite) TestRunHashTable() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Создаем временный файл
	filename := "test_run.txt"
	defer os.Remove(filename)

	// Тестируем HSET
	RunHashTable([]string{"prog", "--file", filename, "--query", "HSET key1 value1"})
	RunHashTable([]string{"prog", "--file", filename, "--query", "HSET key2 value2"})

	// Тестируем HGET
	RunHashTable([]string{"prog", "--file", filename, "--query", "HGET key1"})
	RunHashTable([]string{"prog", "--file", filename, "--query", "HGET nonexistent"})

	// Тестируем HPRINT
	RunHashTable([]string{"prog", "--file", filename, "--query", "HPRINT"})

	// Тестируем HDEL
	RunHashTable([]string{"prog", "--file", filename, "--query", "HDEL key1"})
	RunHashTable([]string{"prog", "--file", filename, "--query", "HDEL nonexistent"})

	// Проверяем после удаления
	RunHashTable([]string{"prog", "--file", filename, "--query", "HGET key1"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *HashTableTestSuite) TestRunHashTableNoFile() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем без файла
	RunHashTable([]string{"prog", "--query", "HSET key1 value1"})
	RunHashTable([]string{"prog", "--query", "HSET key2 value2"})
	RunHashTable([]string{"prog", "--query", "HGET key1"})
	RunHashTable([]string{"prog", "--query", "HPRINT"})
	RunHashTable([]string{"prog", "--query", "HDEL key1"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *HashTableTestSuite) TestRunHashTableInvalidCommand() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Неверная команда
	RunHashTable([]string{"prog", "--query", "INVALID"})

	// Команда без аргументов
	RunHashTable([]string{"prog", "--query", "HSET"})
	RunHashTable([]string{"prog", "--query", "HGET"})
	RunHashTable([]string{"prog", "--query", "HDEL"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *HashTableTestSuite) TestRunHashTableNoArgs() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Без аргументов
	RunHashTable([]string{})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *HashTableTestSuite) TestHashFunction() {
	// Проверяем, что хеш-функция возвращает значение в пределах размера таблицы
	for i := 0; i < 100; i++ {
		key := string(rune(i))
		hash := s.ht.hashFunction(key)
		assert.True(s.T(), hash >= 0 && hash < tableSize)
	}
}

func TestHashTableSuite(t *testing.T) {
	suite.Run(t, new(HashTableTestSuite))
}

// Бенчмарки
func BenchmarkHashTableInsert(b *testing.B) {
	ht := NewHashTable()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		ht.Insert("key", "value")
	}
}

func BenchmarkHashTableGet(b *testing.B) {
	ht := NewHashTable()
	for i := 0; i < 1000; i++ {
		ht.Insert("key", "value")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		ht.Get("key")
	}
}

func BenchmarkHashTableRemove(b *testing.B) {
	ht := NewHashTable()
	for i := 0; i < 1000; i++ {
		ht.Insert("key", "value")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		ht.Remove("key")
	}
}

func BenchmarkHashTableWithCollisions(b *testing.B) {
	ht := NewHashTable()
	// Вставляем много элементов для создания коллизий
	for i := 0; i < 1000; i++ {
		ht.Insert(string(rune(i)), "value")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		ht.Get(string(rune(i % 1000)))
	}
}
