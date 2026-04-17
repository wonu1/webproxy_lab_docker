# Proxy Lab용 Makefile 
#
# 이 파일은 원하는 대로 수정해도 됩니다(단, handin
# 규칙은 제외). 담당 강사는 여러분의 Makefile에서 "make"를 실행하여
# 소스에서 프록시를 빌드합니다.

CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: proxy

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

proxy.o: proxy.c csapp.h
	$(CC) $(CFLAGS) -c proxy.c

proxy: proxy.o csapp.o
	$(CC) $(CFLAGS) proxy.o csapp.o -o proxy $(LDFLAGS)

# 이후 제출할 수 있도록 ../proxylab-handin.tar에 tarball을 생성합니다
# 절대 수정하지 마세요!
handin:
	(make clean; cd ..; tar cvf $(USER)-proxylab-handin.tar proxylab-handout --exclude tiny --exclude nop-server.py --exclude proxy --exclude driver.sh --exclude port-for-user.pl --exclude free-port.sh --exclude ".*")

clean:
	rm -f *~ *.o proxy core *.tar *.zip *.gzip *.bzip *.gz

