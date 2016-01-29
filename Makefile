all:
	g++ 3ddss.cpp -o drawdss -std=c++11
	echo "execute make run to execute"

run: all
	./drawdss

clean:
	rm drawdss
