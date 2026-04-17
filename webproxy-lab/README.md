####################################################################
# CS:APP Proxy Lab
#
# 학생용 소스 파일
####################################################################

이 디렉터리에는 CS:APP Proxy Lab에 필요한 파일들이 들어 있습니다.

proxy.c
csapp.h
csapp.c
    이것들은 시작용 파일입니다. csapp.c와 csapp.h는
    교재에서 설명되어 있습니다.

    이 파일들은 원하는 대로 수정해도 됩니다. 또한
    원하는 추가 파일을 만들어 제출해도 됩니다.

    프록시나 tiny 서버에 사용할 고유한 포트를 생성하려면
    `port-for-user.pl` 또는 `free-port.sh`를 사용하세요.

Makefile
    이 makefile은 proxy 프로그램을 빌드합니다. 해결 코드를 빌드하려면
    "make"를 입력하고, 새로 빌드하려면 "make clean" 후
    "make"를 입력하세요.

    제출할 tar 파일을 만들려면 "make handin"을 입력하세요.
    원하는 대로 수정할 수 있습니다. 담당 강사는 여러분의
    Makefile을 사용해 소스에서 프록시를 빌드합니다.

port-for-user.pl
    특정 사용자용 무작위 포트를 생성합니다.
    사용법: ./port-for-user.pl <userID>

free-port.sh
    프록시나 tiny에 사용할 수 있는 미사용 TCP 포트를 찾아주는
    편리한 스크립트입니다.
    사용법: ./free-port.sh

driver.sh
    Basic, Concurrency, Cache용 자동 채점기입니다.
    사용법: ./driver.sh

nop-server.py
     자동 채점기용 헬퍼입니다.

tiny
    CS:APP 교재의 Tiny 웹 서버
