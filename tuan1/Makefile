# Tên của tệp thực thi đầu ra
TARGET = main

# Danh sách các tệp nguồn
SOURCES = main.c

# Trình biên dịch C
CC = gcc

# Các cờ biên dịch
CFLAGS = -Wall

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
