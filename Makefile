CXX := g++
OBJ := mb
SRC := utils.cpp mb.cpp

all: clean mb

clean:
	@rm -f ${OBJ}

mb:
	${CXX} -o ${OBJ} ${SRC}
