# Tên của tệp thực thi đầu ra
TARGET = resolver

# Danh sách các tệp nguồn
SOURCES = resolver.c

# Trình biên dịch C
CC = gcc

# Các cờ biên dịch
CFLAGS = -Wall

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
