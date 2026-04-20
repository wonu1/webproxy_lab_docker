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

        // accept에 넘기기 전에
        // clientaddr라는 주소 구조체의 전체 크기를 clientlen에 넣어 둔다
        // accept는 이 값을 보고 "얼마나 큰 주소 저장 공간이 준비됐는지" 안다
        clientlen = sizeof(clientaddr);

        // listenfd에서 클라이언트의 연결 요청 하나를 받아들인다
        // 연결해 온 클라이언트의 주소 정보는 clientaddr에 채워지고
        // 실제 사용된 주소 정보 길이는 clientlen에 다시 써진다
        // 그리고 이 클라이언트와 실제로 통신할 새 소켓 fd를 connfd에 반환한다
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        // 방금 연결된 클라이언트와 echo 서비스를 수행한다
        // 클라이언트가 보낸 데이터를 읽고
        // 그대로 다시 클라이언트에게 돌려보내는 일을 한다
        echo(connfd);

        // echo 처리가 끝났으면
        // 이 클라이언트와 연결된 소켓을 닫는다
        // listenfd는 계속 살아 있으므로 다음 클라이언트를 또 받을 수 있다
        Close(connfd);
    }

    return 0;
}

void echo(int connfd)
{
    // 한 번 읽거나 쓴 바이트 수를 저장할 변수
    size_t n;

    // 클라이언트가 보낸 한 줄의 데이터를 임시로 저장할 버퍼
    // MAXLINE 크기만큼의 문자 배열
    char buf[MAXLINE];

    // Robust I/O를 위한 버퍼 상태 구조체 변수
    // connfd에서 읽을 때 내부 버퍼와 읽기 위치 등을 관리한다
    rio_t rio;

    // connfd 소켓을 읽기 위한 Rio 버퍼 상태를 초기화한다
    // 앞으로 connfd에서 데이터를 읽을 때 rio가 그 상태를 관리한다
    Rio_readinitb(&rio, connfd);

    // connfd로부터 한 줄씩 계속 읽는다
    // 읽은 데이터는 buf에 들어가고
    // 실제 읽은 바이트 수는 n에 저장된다
    // 반환값이 0이면 EOF, 즉 연결이 끝났다는 뜻이므로 반복을 종료한다
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {

        // 서버 입장에서 "이번에 몇 바이트를 받았는지" 화면에 출력한다
        // 디버깅용/확인용 메시지라고 보면 된다
        printf("server received %d bytes\n", (int)n);

        // 방금 읽은 buf 내용을 그대로 다시 클라이언트에게 보낸다
        // n 바이트만큼 정확히 전송한다
        // 이것이 echo 서버의 핵심 동작이다
        Rio_writen(connfd, buf, n);
    }
}