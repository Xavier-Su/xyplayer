win:main.c lrc.c linklist.c map.c database.c sqlite3.c
	gcc -o xyplayer main.c lrc.c linklist.c map.c database.c sqlite3.c -lpthread -ldl  `pkg-config --cflags --libs  gtk+-2.0`
	./xyplayer
clean:
	rm -f xyplayer
