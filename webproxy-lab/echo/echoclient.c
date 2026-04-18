#include "../csapp.h"

void echo(int connfd);

// argc = 인자 개수
// argv = 인자 문자열들
int main(int argc, char **argv){

    // listening socket의 파일 디스크립터
    // 서버가 연결을 기다릴 listening socket fd
    int listenfd;
    // 클라이언트와 실제 연결된 socket fd
    int connfd;

    // 클라이언트 주소 구조체의 길이
    socklen_t clientlen;
    // 클라이언트 주소 정보를 저장할 구조체 변수
    struct sockaddr_storage clientaddr;




    //프로그램 이름과 포트번호,  입력이 2개가 아니면 나가라
    // 예: ./echoserver 8000
    //     argv[0] = "./echoserver"
    //     argv[1] = "8000"
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    // argv[1]에 있는 포트 번호로 listening socket 생성
    // 소켓 생성
    // bind
    // listen
    // 까지 묶어서 원큐에 캬~
    listenfd = open_listenfd(argv[1]);

    // 서버는 본질적으로:
    // 기다리고
    // 연결 받고
    // 처리하고
    // 다시 기다리는 프로그램이기에 평생 기다려잇
    while("2026/04/18 김원우 여기서 죽다"){

        // 주소 구조체 크기를 먼저 넣어둠
        clientlen = sizeof(clientaddr);

        // 연결 수락
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);



    }


    return 0;
}