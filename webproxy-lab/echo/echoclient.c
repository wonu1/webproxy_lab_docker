#include "../csapp.h"

int main(int argc, char **argv)
{
    // 서버와 연결된 클라이언트 소켓의 파일 디스크립터
    int clientfd;

    // 접속할 서버의 호스트 이름과 포트 번호를 가리킬 포인터
    char *host, *port;

    // 키보드에서 입력한 문자열과
    // 서버로부터 받은 응답 한 줄을 잠시 저장할 버퍼
    char buf[MAXLINE];

    // Robust I/O를 위한 버퍼 상태 구조체 변수
    // 서버 소켓(clientfd)에서 읽을 때 내부 버퍼 상태를 관리한다
    rio_t rio;

    // 프로그램 이름, 호스트, 포트
    // 총 3개의 명령줄 인자가 아니면 종료
    // 예: ./echoclient localhost 8000
    //     argv[0] = "./echoclient"
    //     argv[1] = "localhost"
    //     argv[2] = "8000"
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    // 명령줄 인자에서 접속할 서버의 호스트와 포트를 꺼낸다
    host = argv[1];
    port = argv[2];

    // host와 port를 이용해 서버에 연결한다
    // 성공하면 서버와 통신할 소켓 fd를 반환한다
    clientfd = Open_clientfd(host, port);

    // clientfd 소켓을 읽기 위한 Rio 버퍼 상태를 초기화한다
    // 앞으로 서버로부터 데이터를 읽을 때 rio가 상태를 관리한다
    Rio_readinitb(&rio, clientfd);

    // 표준 입력(stdin), 즉 키보드에서 한 줄씩 입력받는다
    // 사용자가 한 줄 입력할 때마다 서버에 보내고
    // 서버가 돌려준 응답 한 줄을 다시 화면에 출력한다
    while (Fgets(buf, MAXLINE, stdin) != NULL) {

        // 사용자가 입력한 한 줄을 서버에 전송한다
        // strlen(buf)만큼만 보내면 입력한 문자열 길이만큼 정확히 전송된다
        Rio_writen(clientfd, buf, strlen(buf));

        // 서버가 보낸 응답 한 줄을 읽어서 다시 buf에 저장한다
        // echo 서버라면 방금 보낸 내용을 그대로 다시 받게 된다
        Rio_readlineb(&rio, buf, MAXLINE);

        // 서버로부터 받은 응답을 표준 출력(stdout), 즉 화면에 출력한다
        Fputs(buf, stdout);
    }

    // 입력이 끝나면 서버와의 연결 소켓을 닫는다
    Close(clientfd);
    return 0;
}