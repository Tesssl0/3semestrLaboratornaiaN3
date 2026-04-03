package array

import (
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
)

type ArrayTestSuite struct {
	suite.Suite
	arr *DynamicArray
}

func (s *ArrayTestSuite) SetupTest() {
	s.arr = NewDynamicArray(10)
}

func (s *ArrayTestSuite) TearDownTest() {
	s.arr = nil
}

func (s *ArrayTestSuite) TestNewDynamicArray() {
	assert.Equal(s.T(), 0, s.arr.Length())
	assert.Equal(s.T(), 10, s.arr.capacity)
}

func (s *ArrayTestSuite) TestAdd() {
	s.arr.Add("one")
	s.arr.Add("two")
	assert.Equal(s.T(), 2, s.arr.Length())
	assert.Equal(s.T(), "one", s.arr.Get(0))
	assert.Equal(s.T(), "two", s.arr.Get(1))
}

func (s *ArrayTestSuite) TestInsert() {
	s.arr.Add("a")
	s.arr.Add("c")

	ok := s.arr.Insert(1, "b")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.arr.Get(1))
	assert.Equal(s.T(), 3, s.arr.Length())

	ok = s.arr.Insert(5, "x")
	assert.False(s.T(), ok)
}

func (s *ArrayTestSuite) TestInsertResize() {
	smallArr := NewDynamicArray(2)
	smallArr.Add("a")
	smallArr.Add("b")

	ok := smallArr.Insert(2, "c")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), 3, smallArr.Length())
	assert.Equal(s.T(), "c", smallArr.Get(2))
}

func (s *ArrayTestSuite) TestRemove() {
	s.arr.Add("a")
	s.arr.Add("b")
	s.arr.Add("c")

	ok := s.arr.Remove(1)
	assert.True(s.T(), ok)
	assert.Equal(s.T(), 2, s.arr.Length())
	assert.Equal(s.T(), "a", s.arr.Get(0))
	assert.Equal(s.T(), "c", s.arr.Get(1))

	ok = s.arr.Remove(5)
	assert.False(s.T(), ok)
}

func (s *ArrayTestSuite) TestSet() {
	s.arr.Add("a")
	s.arr.Add("b")

	ok := s.arr.Set(1, "x")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "x", s.arr.Get(1))

	ok = s.arr.Set(5, "y")
	assert.False(s.T(), ok)
}

func (s *ArrayTestSuite) TestGet() {
	s.arr.Add("hello")
	assert.Equal(s.T(), "hello", s.arr.Get(0))
	assert.Equal(s.T(), "", s.arr.Get(5))
}

func (s *ArrayTestSuite) TestClear() {
	s.arr.Add("data")
	s.arr.Clear()
	assert.Equal(s.T(), 0, s.arr.Length())
	assert.Equal(s.T(), 10, s.arr.capacity)
}

func (s *ArrayTestSuite) TestPrint() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Пустой массив
	s.arr.Print()

	// Массив с данными
	s.arr.Add("a")
	s.arr.Add("b")
	s.arr.Add("c")
	s.arr.Print()

	w.Close()
	os.Stdout = old
	// Читаем вывод чтобы "использовать" r
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *ArrayTestSuite) TestFileIO() {
	filename := "test_array.txt"
	defer os.Remove(filename)

	s.arr.Add("hello")
	s.arr.Add("world")
	err := s.arr.SaveToFile(filename)
	assert.NoError(s.T(), err)

	newArr := NewDynamicArray(10)
	err = newArr.LoadFromFile(filename)
	assert.NoError(s.T(), err)

	assert.Equal(s.T(), 2, newArr.Length())
	assert.Equal(s.T(), "hello", newArr.Get(0))
	assert.Equal(s.T(), "world", newArr.Get(1))
}

func (s *ArrayTestSuite) TestFileIOEmptyFilename() {
	// Сохранение с пустым именем
	err := s.arr.SaveToFile("")
	assert.NoError(s.T(), err)

	// Загрузка с пустым именем
	err = s.arr.LoadFromFile("")
	assert.NoError(s.T(), err)
}

func (s *ArrayTestSuite) TestFileIONotFound() {
	// Загрузка несуществующего файла
	err := s.arr.LoadFromFile("nonexistent.txt")
	assert.Error(s.T(), err)
}

func (s *ArrayTestSuite) TestFileIOError() {
	// Попытка сохранения в недоступное место
	err := s.arr.SaveToFile("/invalid/path/file.txt")
	assert.Error(s.T(), err)
}

func (s *ArrayTestSuite) TestFileIOEmptyFile() {
	// Создаем пустой файл
	filename := "empty.txt"
	defer os.Remove(filename)

	file, _ := os.Create(filename)
	file.Close()

	newArr := NewDynamicArray(10)
	err := newArr.LoadFromFile(filename)
	assert.NoError(s.T(), err)
	assert.Equal(s.T(), 0, newArr.Length())
}

func (s *ArrayTestSuite) TestRunDynamicArray() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Создаем временный файл
	filename := "test_run.txt"
	defer os.Remove(filename)

	// Тестируем MPUSH
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MPUSH first"})
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MPUSH second"})
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MPUSH third"})

	// Тестируем MLEN
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MLEN"})

	// Тестируем MGET
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MGET 0"})
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MGET 5"})

	// Тестируем MSET
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MSET 1 xyz"})

	// Тестируем MINSERT
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MINSERT 2 inserted"})

	// Тестируем MPRINT
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MPRINT"})

	// Тестируем MDEL
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MDEL 1"})
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MDEL 10"})

	// Проверяем после удалений
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MPRINT"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *ArrayTestSuite) TestRunDynamicArrayNoFile() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем без файла
	RunDynamicArray([]string{"prog", "--query", "MPUSH first"})
	RunDynamicArray([]string{"prog", "--query", "MPUSH second"})
	RunDynamicArray([]string{"prog", "--query", "MLEN"})
	RunDynamicArray([]string{"prog", "--query", "MGET 0"})
	RunDynamicArray([]string{"prog", "--query", "MPRINT"})
	RunDynamicArray([]string{"prog", "--query", "MDEL 0"})
	RunDynamicArray([]string{"prog", "--query", "MPRINT"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *ArrayTestSuite) TestRunDynamicArrayInvalidCommand() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Неверная команда
	RunDynamicArray([]string{"prog", "--query", "INVALID"})

	// Команды без аргументов
	RunDynamicArray([]string{"prog", "--query", "MINSERT"})
	RunDynamicArray([]string{"prog", "--query", "MSET"})
	RunDynamicArray([]string{"prog", "--query", "MGET"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *ArrayTestSuite) TestRunDynamicArrayNoArgs() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Без аргументов
	RunDynamicArray([]string{})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *ArrayTestSuite) TestRunDynamicArrayWithFile() {
	// Тестируем загрузку из файла
	filename := "test_load.txt"
	defer os.Remove(filename)

	// Создаем файл с данными
	file, _ := os.Create(filename)
	file.WriteString("one\ntwo\nthree\n")
	file.Close()

	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Должен загрузить данные из файла
	RunDynamicArray([]string{"prog", "--file", filename, "--query", "MPRINT"})

	w.Close()
	os.Stdout = old
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func TestArraySuite(t *testing.T) {
	suite.Run(t, new(ArrayTestSuite))
}

// Бенчмарки
func BenchmarkArrayAdd(b *testing.B) {
	arr := NewDynamicArray(10)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		arr.Add("test")
	}
}

func BenchmarkArrayGet(b *testing.B) {
	arr := NewDynamicArray(1000)
	for i := 0; i < 1000; i++ {
		arr.Add("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		arr.Get(i % 1000)
	}
}

func BenchmarkArrayInsert(b *testing.B) {
	arr := NewDynamicArray(1000)
	for i := 0; i < 1000; i++ {
		arr.Add("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		arr.Insert(500, "test")
	}
}

func BenchmarkArrayRemove(b *testing.B) {
	arr := NewDynamicArray(1000)
	for i := 0; i < 1000; i++ {
		arr.Add("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		arr.Remove(500)
	}
}

func BenchmarkArrayPrint(b *testing.B) {
	arr := NewDynamicArray(100)
	for i := 0; i < 100; i++ {
		arr.Add("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		arr.Print()
	}
}
