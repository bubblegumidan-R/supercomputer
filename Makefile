all:
	gcc vcell_cluster.c -o vcell -lpthread -lm

clean:
	rm -f vcell
