/* $begin tinymain */
/*
 * tiny.c - GET 메서드를 사용하여
 *     정적 및 동적 콘텐츠를 제공하는 간단한 반복형 HTTP/1.0 웹 서버입니다.
 *
 * 수정 11/2019 droh
 *   - serve_static()와 clienterror()의 sprintf() aliasing 문제를 수정했습니다.
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);

int main(int argc, char **argv)
{
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* 명령줄 인자를 확인합니다 */
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1)
  {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen); // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);  // line:netp:tiny:doit
    Close(connfd); // line:netp:tiny:close
  }
}

/*
 * doit - 하나의 HTTP 요청/응답 트랜잭션을 처리합니다
 */
void doit(int fd)
{
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  /* 요청 줄과 헤더를 읽습니다 */
  Rio_readinitb(&rio, fd);
  if (!Rio_readlineb(&rio, buf, MAXLINE))
    return;
  printf("%s", buf);
  sscanf(buf, "%s %s %s", method, uri, version);
  if (strcasecmp(method, "GET"))
  {
    clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method");
    return;
  }
  read_requesthdrs(&rio);

  /* GET 요청에서 URI를 파싱합니다 */
  is_static = parse_uri(uri, filename, cgiargs);
  if (stat(filename, &sbuf) < 0)
  {
    clienterror(fd, filename, "404", "Not found",
                "Tiny couldn't find this file");
    return;
  }

  if (is_static)
  { /* 정적 콘텐츠를 제공합니다 */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
    {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size);
  }
  else
  { /* 동적 콘텐츠를 제공합니다 */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
    {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs);
  }
}

/*
 * read_requesthdrs - HTTP 요청 헤더를 읽습니다
 */
void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE);
  printf("%s", buf);
  while (strcmp(buf, "\r\n"))
  {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}

/*
 * parse_uri - URI를 파일 이름과 CGI 인자로 파싱합니다
 *             동적 콘텐츠면 0, 정적 콘텐츠면 1을 반환합니다
 */
int parse_uri(char *uri, char *filename, char *cgiargs)
{
  char *ptr;

  if (!strstr(uri, "cgi-bin"))
  { /* 정적 콘텐츠 */
    strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri) - 1] == '/')
      strcat(filename, "home.html");
    return 1;
  }
  else
  { /* 동적 콘텐츠 */
    ptr = index(uri, '?');
    if (ptr)
    {
      strcpy(cgiargs, ptr + 1);
      *ptr = '\0';
    }
    else
      strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }
}

/*
 * serve_static - 파일을 클라이언트로 다시 보냅니다
 */
void serve_static(int fd, char *filename, int filesize)
{
  int srcfd;
  char *srcp, filetype[MAXLINE];

  char buf[MAXBUF];
  char *p = buf;
  int n;
  int remaining = sizeof(buf);

  /* 응답 헤더를 클라이언트에 보냅니다 */
  get_filetype(filename, filetype);

  /* HTTP 응답 헤더를 올바르게 구성합니다 - 별도 버퍼를 쓰거나 이어 붙입니다 */
  n = snprintf(p, remaining, "HTTP/1.0 200 OK\r\n");
  p += n;
  remaining -= n;

  n = snprintf(p, remaining, "Server: Tiny Web Server\r\n");
  p += n;
  remaining -= n;

  n = snprintf(p, remaining, "Connection: close\r\n");
  p += n;
  remaining -= n;

  n = snprintf(p, remaining, "Content-length: %d\r\n", filesize);
  p += n;
  remaining -= n;

  n = snprintf(p, remaining, "Content-type: %s\r\n\r\n", filetype);
  p += n;
  remaining -= n;

  Rio_writen(fd, buf, strlen(buf));
  printf("Response headers:\n");
  printf("%s", buf);

  /* 응답 본문을 클라이언트에 보냅니다 */
  srcfd = Open(filename, O_RDONLY, 0);
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  Close(srcfd);
  Rio_writen(fd, srcp, filesize);
  Munmap(srcp, filesize);
}

/*
 * get_filetype - 파일 이름에서 파일 형식을 구합니다
 */
void get_filetype(char *filename, char *filetype)
{
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else
    strcpy(filetype, "text/plain");
}

/*
 * serve_dynamic - run a CGI program on behalf of the client
 */
void serve_dynamic(int fd, char *filename, char *cgiargs)
{
  char buf[MAXLINE], *emptylist[] = {NULL};
  pid_t pid;

  /* HTTP 응답의 첫 부분을 반환합니다 */
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  /* CGI 프로그램을 처리할 자식 프로세스를 생성합니다 */
  if ((pid = Fork()) < 0)
  { /* Fork 실패 */
    perror("Fork failed");
    return;
  }

  if (pid == 0)
  { /* 자식 프로세스 */
    /* 실제 서버라면 여기서 모든 CGI 변수를 설정합니다 */
    setenv("QUERY_STRING", cgiargs, 1);

    /* stdout을 클라이언트로 리다이렉트합니다 */
    if (Dup2(fd, STDOUT_FILENO) < 0)
    {
      perror("Dup2 error");
      exit(1);
    }
    Close(fd);

    /* CGI 프로그램을 실행합니다 */
    Execve(filename, emptylist, environ);

    /* 여기까지 왔다면 Execve가 실패한 것입니다 */
    perror("Execve error");
    exit(1);
  }
  else
  { /* 부모 프로세스 */
    /* 부모는 자식이 종료되기를 기다립니다 */
    int status;
    if (waitpid(pid, &status, 0) < 0)
    {
      perror("Wait error");
    }

    printf("Child process %d terminated with status %d\n", pid, status);
    /* 부모는 정상적으로 계속 진행합니다 - doit()로 돌아갑니다 */
  }
  /* 여기서 반환되면 doit()가 연결을 닫습니다 */
}

/*
 * clienterror - returns an error message to the client
 */
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXBUF];

  /* HTTP 응답 본문을 구성합니다 */
  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body, "%s<body bgcolor="
                "ffffff"
                ">\r\n",
          body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  /* HTTP 응답을 출력합니다 */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}
