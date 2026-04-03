package queue

import (
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
)

type QueueTestSuite struct {
	suite.Suite
	queue *Queue
}

func (s *QueueTestSuite) SetupTest() {
	s.queue = NewQueue()
}

func (s *QueueTestSuite) TearDownTest() {
	s.queue.Destroy()
}

func (s *QueueTestSuite) TestEnqueue() {
	s.queue.Enqueue("first")
	assert.Equal(s.T(), "first", s.queue.Front.Data)
	assert.Equal(s.T(), "first", s.queue.Rear.Data)

	s.queue.Enqueue("second")
	assert.Equal(s.T(), "first", s.queue.Front.Data)
	assert.Equal(s.T(), "second", s.queue.Rear.Data)
	assert.Equal(s.T(), "second", s.queue.Front.Next.Data)
}

func (s *QueueTestSuite) TestDequeue() {
	s.queue.Enqueue("a")
	s.queue.Enqueue("b")

	ok := s.queue.Dequeue()
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.queue.Front.Data)

	ok = s.queue.Dequeue()
	assert.True(s.T(), ok)
	assert.Nil(s.T(), s.queue.Front)
	assert.Nil(s.T(), s.queue.Rear)

	ok = s.queue.Dequeue()
	assert.False(s.T(), ok)
}

func (s *QueueTestSuite) TestPrint() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Пустая очередь
	s.queue.Print()

	// Очередь с элементами
	s.queue.Enqueue("a")
	s.queue.Enqueue("b")
	s.queue.Enqueue("c")
	s.queue.Print()

	w.Close()
	os.Stdout = old
	// Читаем вывод чтобы "использовать" r
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *QueueTestSuite) TestDestroy() {
	s.queue.Enqueue("a")
	s.queue.Enqueue("b")
	s.queue.Destroy()
	assert.Nil(s.T(), s.queue.Front)
	assert.Nil(s.T(), s.queue.Rear)
}

func (s *QueueTestSuite) TestFileIO() {
	filename := "test_queue.txt"
	defer os.Remove(filename)

	s.queue.Enqueue("hello")
	s.queue.Enqueue("world")
	err := s.queue.SaveToFile(filename)
	assert.NoError(s.T(), err)

	newQueue := NewQueue()
	err = newQueue.LoadFromFile(filename)
	assert.NoError(s.T(), err)

	assert.Equal(s.T(), "hello", newQueue.Front.Data)
	assert.Equal(s.T(), "world", newQueue.Front.Next.Data)
	assert.Equal(s.T(), "world", newQueue.Rear.Data)
}

func (s *QueueTestSuite) TestFileIOEmptyFilename() {
	// Сохранение с пустым именем
	err := s.queue.SaveToFile("")
	assert.NoError(s.T(), err)

	// Загрузка с пустым именем
	err = s.queue.LoadFromFile("")
	assert.NoError(s.T(), err)
}

func (s *QueueTestSuite) TestFileIONotFound() {
	// Загрузка несуществующего файла
	err := s.queue.LoadFromFile("nonexistent.txt")
	assert.Error(s.T(), err)
}

func (s *QueueTestSuite) TestFileIOError() {
	// Попытка сохранения в недоступное место
	err := s.queue.SaveToFile("/invalid/path/file.txt")
	assert.Error(s.T(), err)
}

func (s *QueueTestSuite) TestRunQueue() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Создаем временный файл
	filename := "test_run.txt"
	defer os.Remove(filename)

	// Тестируем QPUSH
	RunQueue([]string{"prog", "--file", filename, "--query", "QPUSH first"})
	RunQueue([]string{"prog", "--file", filename, "--query", "QPUSH second"})
	RunQueue([]string{"prog", "--file", filename, "--query", "QPUSH third"})

	// Тестируем QPRINT
	RunQueue([]string{"prog", "--file", filename, "--query", "QPRINT"})

	// Тестируем QPOP
	RunQueue([]string{"prog", "--file", filename, "--query", "QPOP"})
	RunQueue([]string{"prog", "--file", filename, "--query", "QPOP"})

	// Проверяем после удалений
	RunQueue([]string{"prog", "--file", filename, "--query", "QPRINT"})

	// Удаляем последний элемент
	RunQueue([]string{"prog", "--file", filename, "--query", "QPOP"})

	// Попытка удалить из пустой очереди
	RunQueue([]string{"prog", "--file", filename, "--query", "QPOP"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *QueueTestSuite) TestRunQueueNoFile() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем без файла
	RunQueue([]string{"prog", "--query", "QPUSH first"})
	RunQueue([]string{"prog", "--query", "QPUSH second"})
	RunQueue([]string{"prog", "--query", "QPRINT"})
	RunQueue([]string{"prog", "--query", "QPOP"})
	RunQueue([]string{"prog", "--query", "QPRINT"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *QueueTestSuite) TestRunQueueInvalidCommand() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Неверная команда
	RunQueue([]string{"prog", "--query", "INVALID"})

	// Команда без аргумента
	RunQueue([]string{"prog", "--query", "QPUSH"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *QueueTestSuite) TestRunQueueNoArgs() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Без аргументов
	RunQueue([]string{})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func TestQueueSuite(t *testing.T) {
	suite.Run(t, new(QueueTestSuite))
}

// Бенчмарки
func BenchmarkQueueEnqueue(b *testing.B) {
	queue := NewQueue()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		queue.Enqueue("test")
	}
	queue.Destroy()
}

func BenchmarkQueueDequeue(b *testing.B) {
	queue := NewQueue()
	for i := 0; i < b.N; i++ {
		queue.Enqueue("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		queue.Dequeue()
	}
	queue.Destroy()
}

func BenchmarkQueuePrint(b *testing.B) {
	queue := NewQueue()
	for i := 0; i < 100; i++ {
		queue.Enqueue("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		queue.Print()
	}
	queue.Destroy()
}
