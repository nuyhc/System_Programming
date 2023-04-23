# 1. Intro SP
### Operating System
- 컴퓨터와 사용자 사이의 중개자 역할
- OS 목표
  - 사용의 편의성
  - 하드웨어의 효율적 사용

### Fundamental OS Concepts
- Multi-user environment
- Process and Scheduling
- User space and Kernel space
- Basic and Advanced I/O

### Advantages & Weakness
- Advantages
  - 오픈 소스
  - Open developer site & User group
  - 무료
- Weakness
  - Too fast version upgrade, many venders
  - Less official programs
  - Device drivers

### Computer System Overview
- SW: [Application -> System Calls -> OS(kernel)]
- Architecture
- HW: [CPU, Mem, I/O Devices]

### Layered Linux Structure
![img](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSSC-QtILSrV2_kqnBA290u-yF8mSYP_fjMEA&usqp=CAU)
1. HW: CPU, Mem, Disk, Peripherals(주변 장치)
2. Kernel
   1. Process management
   2. File management
   3. Memory management
   4. Device management
3. System Call: 리눅스 커널에 대한 기능적 인터페이스
4. Commands, Utilities, App programs: 라이브러리 루틴 또는 시스템 호출을 사용하여 커널 서비스 요청

### System Calls vs. Library Calls
- System Calls
  - OS가 제공하는 인터페이스
  - 커널 코드의 진입점
- Library Calls
  - 사용자의 코드를 변환
  - API(Application Programming Interfece)라고도 함

### Shell
- OS의 사용자 인터페이스
- 로그인시 실행됨

### Linux manual sections
- `man [section #] [keyword]`

# 2. File IO (Standard I/O Library)
### System Calls vs. Library Calls
- System Calls: OS 내부에 존재, man page 2
- Library Calls: OS 외부에 존재, man page 3

### Library
- API 제공 목적
- 자주 사용되는 함수 모음
- 사용자가 Call 할때 linked 됨

#### Types
- Shared library(*.so, *.dll)
  - 메모리에 하나의 카피본만 존재
  - 실행시 주소가 전달 (메모리가 효율적, dynamic linking or binding)
  - 서버 시스템에 효과적
- Static library(*.a)
  - 동일한 코드가 각각 들어가 있음
  - 속도가 빠르지만 메모리 오버헤딩 존재 (라이브러리 이동이 없어 프로그램 효율이 좋지만, 중복된 코드로 메모리 효율이 떨어짐)
  - 임베디드 시스템에 효과적

### Standard I/O Library
- `<stdio.h>` 

### FILE object in C
- I/O stream object create by standard I/O library
  - `FILE*`로 접근
  - 파일 포인터는, 열려는 파일의 일부 정보를 갖고 있음
- `stdin, stdout, stderr`

### File descriptor
- 시스템 콜로 I/O를하는 경우, 스트림 객체를 사용하지 않고 File descriptor를 사용 (라이브러리 이용시 FP 사용)
- 0, 1, 2는 각각 stdin, stdout, stderr
- 라이브러리는 최종적으로 System Call을 사용
- Why use Library?
  - 편리성
  - formatting
  - buffering

### File Stream(Library) & File Descriptor(System Call)
- 파일 스트림은 FD에 1:1로 맵핑됨

```c
#include <stdio.h>

int fileno(FILE *stream); // R: FD(#) for the ope FILE stream
FILE* fdopen(int fildes, const char *mode); // using FD of an open, create / R: FILE stream
```

### Library buffering
- buffering: 데이터를 임시 저장하는 공간
- 시스템 콜 횟수를 줄여줌 (한번에 모아서 사용하기 때문)

#### Types
1. Full buffering
   1. 대부분의 라이브러리가 사용
   2. 디스크 블럭의 라이브러리 수준
   3. `fflush()`를 통해, 버퍼가 채워지지 않아도 시스템 콜 호출 가능
2. Line buffering (`stdin/stdout`)
   1. 쉘의 콘솔에서 주로 사용
   2. 메모리 낭비는 적지만, 데이터를 잃어버릴 가능성 존재
   3. "ENTER"가 입력될 때까지 버퍼링
3. Unbuffering (`stderr`)
   1. 버퍼링을하지 않음
   2. 시스템 콜을 바로 호출

### Set Buffering Type

```c
#include <stdio.h>

void setbuf(FILE *stream, char *buf); // NULL for Unberffering, Non-NULL for normal (Full)
int setvbuf(FILE *stream, char *buf, int type, size_t size);
```

- type
  - `_IOFBF`: Full
  - `_IOLBF`: Line
  - `_IONBF`: Unbuffering

### Kernel Buffering
- buffer: 일방향(쓰기) / Cache: 중간, 데이터 존재 여부 검사(읽기/쓰기)
- SW caching by the kernel
- page cache(buffer cache): disk I/O 감소
- (읽기) 페이지 캐시에 데이터가 없으면 디스크에 접근
- (쓰기) 캐시에 먼저 작성하고, `sync`시 디스크에 작성

### fflush

```c
#include <stdio.h>

int fflush(FILE *stream);
```

- 버퍼 용량이 차지 않아도 커널 레벨로 내려줌
- 파일이 닫히는 경우, 자동으로 실행
- lib. buffer -> page cache

### File Open

```c
#include <stdio.h>

FILE *fopen(const char *filename, const char *type);
```

- type
  - `r`: Read only
  - `w`: 파일의 크기를 0으로 변경하거나 생성
  - `a`: Append mode, write only, 파일이 없는 경우 생성
  - `r+`: Read/Write
  - `w+`: 파일의 크기를 0으로 변경하거나 생성 (For Read and Write)
  - `a+`: Read/append mode. 파일이 없는 경우 생성

### File Reopen

```c
#include <stdio.h>

FILE *freopen(const char *filename, const char *type, FILE *stream);
```

- close a file linked to the input stream, and open a file with a given filename by reusing the old stream
- 기존 FD도 재사용 됨

```c
// 콘솔에 출력되지 않고, 연결한 txt 파일에 내용이 써짐
freopen("myfile.txt", "w", stdout);
printf("This sentence is redirected to a file");
fclose(stdout);
```

### File Close

```c
#include <stdio.h>

int fclose(FILE *stream);
```

- 일반적으로, 프로그램이 종료(exit)되면 자동으로 파일들이 닫힘
- 파일을 닫지 않고 프로세스를 종료하는 경우,
  - `fclose`를 이용한 오류 검사 불가
  - lib. buffer의 파일 데이터가 사라짐

### File I/O functions (stream 형태로 읽기/쓰기)

```c
#include <stdio.h>

size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream);
size_t fwrite(void *ptr, size_t size, size_t nitems, FILE *stream);
```

### Character Input & Output

```c
#include <stdio.h>

// 파일로부터 글자를 읽어 옴
int getc(FILE *stream);
int fgetc(FILE *stream);
// stdin으로부터 글자를 읽어 옴
int getchar(void);
// "\0" 입력시까지 문자열을 가져옴
char *fgets(char *s, int size, FILE *stream);
// stdin으로부터, "
char *gets(char *s);
// 파일로부터 c를 읽을 수 있는 경우
int ungetc(int c, FILE *stream);

// 파일에 글자를 씀
int putc(int c, FILE *stream);
int fputc(int c, FILE *stream);
// stdout에 글자를 씀
int putchar(int c);
// "\0" 입력시까지 문자열을 씀
int *fputs(const char *s, FILE *stream);
// stdout에, "
char *puts(const char *s);
```

#### [EX] fileio-ex.c

```c
// 첫 번째 파일 내용 -> 두 번째 파일 내용
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int c;
    FILE *fpin, *fpout;

    if (argc!=3){ perror(argv[0]); exit(1); }
    // 첫 번째 파일 open for read
    if ((fpin=fopen(argv[1], "r"))==NULL){
        perror(argv[1]);
        exit(2);
    }
    // 두 번째 파일 open for write (append)
    if ((fpout=fopen(argv[2], "a"))==NULL){
        perror(argv[2]);
        exit(3);
    }

    // Unbuffered
    setbuf(fpin, NULL);
    setbuf(fpout, NULL);

    // 입력 (복사)
    while ((c=getc(fpin))!=EOF){
        putc(c, fpout);
    }

    fclose(fpin);
    fclose(fpout);
    exit(0);
}
```

#### [EX] lineio-ex.c

```c
// Line buffer가 추가된 예제
#include <stdio.h>
#define BUFFER_SIZE 100

int main(int argc, char *argv[]){
    char ubuf[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    FILE *fpin, *fpout;

    if (argc!=3){ perror(argv[0]); return 1; }
    // 첫 번째 파일 open for read
    if ((fpin=fopen(argv[1], "r"))==NULL){
        perror(argv[2]);
        return 2;
    }
    // 두 번째 파일 open for write (append)
    if ((fpout=fopen(argv[2], "a"))==NULL){
        perror(argv[3]);
        return 3;
    }

    // Line Buffer
    if(setvbuf(fpin, ubuf, _IOLBF, BUFFER_SIZE)!=0){ perror("setvbuf(fpint)"); return 5; }
    if(setvbuf(fpout, ubuf, _IOLBF, BUFFER_SIZE)!=0){ perror("setvbuf(fpout)"); return 6; }

    // 입력 (복사)
    while (fgets(line, BUFFER_SIZE, fpin)!=NULL){
        fputs(line, fpout);
    }

    fclose(fpin);
    fclose(fpout);
    return 0;
}
```

#### [EX] Array I/O

```c
#include <stdio.h>
#define ARRAY_SIZE 10

int main(int argc, char *argv[]){
    int i;
    int sample_array[ARRAY_SIZE];
    FILE *stream;

    if ((stream=fopen(argv[1], "w"))==NULL){
        perror(argv[1]);
        return 1;
        }
    if (fwrite(sample_array, sizeof(int), ARRAY_SIZE, stream)!=ARRAY_SIZE){
        perror("fwrite error");
        return 2;
        }

    fclose(stream);
    return 0;
}
```

#### [EX] Struct I/O

```c
struct{
    short count;
    char sample;
    long total;
    float numeric[LENGTH];
}object;
...
FILE *stream;
...
if(fwrite(&object, sizeof(object), 1, stream)!=1){
    perror("fwrite error");
}
```

#### [EX] File copy with Full buffering

```c
#include <stdio.h>
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    char ubuf[BUFFER_SIZE];
    char fbuf[BUFFER_SIZE];
    int n;
    FILE *fpin, *fpout;

    if (argc!=3){ perror(argv[0]); return 1; }
    // 첫 번째 파일 (원본)
    if ((fpin=fopen(argv[1], "r"))==NULL){
        perror(argv[1]);
        return 2;
    }
    // 두 번째 파일 (복사본)
    if ((fpout=fopen(argv[2], "w"))==NULL){
        perror(argv[2]);
        return 3;
    }
    // Full Buffering
    if (setvbuf(fpin, ubuf, _IOFBF, BUFFER_SIZE)!=0){ perror("setvbuf(fpin)"); return 4; }
    if (setvbuf(fpout, ubuf, _IOFBF, BUFFER_SIZE)!=0){ perror("setvbuf(fpout)"); return 5; }
    // 복사
    while ((n=fread(fbuf, sizeof(char), BUFFER_SIZE, fpin))>0){
        fwrite(fbuf, sizeof(char), n, fpout);
    }

    fclose(fpin);
    fclose(fpout);
    return 0;
}
```

### File Offset
- 열린 모든 파일들은 다음 접근 위치를 위한 오프셋을 갖고 있음
  - Open R/W -> offset은 시작점
  - Appending -> offset은 파일의 끝
- 읽기/쓰기가 진행되는 동안 offset은 자동으로 변화

### File Access Methods
- Sequential access: r/w offset을 따라 접근
- Random access
  - `fseek()` 함수를 이용해 원하는 offset으로 이동
  - `fseek()`: 라이브러리 콜 / `lseek()`: 시스템 콜
  - 저장 과정에서 주로 사용
- Keyed access: DB에서 사용되는 방식

### R/W offset related functions

```c
#include <stdio.h>

int fseek(FILE *stream, long offset, int sopt);
void rewind(FILE *stream); // offset을 처음으로 되돌림, == fseek(*stream, 0, SEEK_SET)
long ftell(FILE *stream); // 현 offset 반환
```

#### SEEK options
- `SEEK_SET`: new r/w offset = offset
- `SEEK_CUR`: new r/w offset = current_offset + offset
- `SEEK_END`: new r/w offset = EOF + offset(-값)

#### [EX] frandom-ex.c

```c
#include <stdio.h>

int main(int argc, char *argv[]){
    FILE *fp;
    char buf[256];
    int rspn;
    long pos;

    if ((fp=fopen(argv[1], "r"))==NULL){ perror(argv[1]); return 1; }
    // 오프셋 설정
    rspn = fseek(fp, 8L, SEEK_SET);
    pos = ftell(fp);

    fgets(buf, 256, fp);
    printf("Position: %ld\n", pos);
    printf("%s\n", buf);

    // 오프셋 초기화
    rewind(fp);
    pos = ftell(fp);

    fgets(buf, 256, fp);
    fclose(fp);

    printf("Position: %ld\n", pos);
    printf("%s\n", buf);
    return 0;
}
```

### I/O Types
- Unformatted I/O (Binary I/O)
- Formatted I/O (Text I/O)

### Formatted Output & Input

```c
int printf(const char *format, /* args */ ...); // to the console
int fprintf(FILE *stream, const char *format, /* args */ ...); // to a file
int springf(char *s, const char *format, /* args */ ...); // to a string

int scanf(const char *format, ...); // from the console
int fscanf(FILE *stream, const char *format, ...); // from a file
int sscanf(char *s, const char *format, ...); // from a string
```

#### [EX] stdio-ex.c

```c
#include <stdio.h>

int main(int argc, char argv[]){
    FILE *fp;
    char buf[256];
    int num, Nnum;
    char str[30], Nstr[30];
    // 사용자 입력
    scanf("%d %s", &num, str);
    // 입력 내용 파일 저장
    if ((fp=fopen("test.txt", "w"))==NULL){ perror("test.txt"); return 1; }
    fprintf(fp, "%d %s\n", num, str);
    // 파일에서 내용 읽어와서 출력
    if ((fp=freopen("test.txt", "r", fp))==NULL){ perror("test.txt"); return 1; }
    fscanf(fp, "%d %s\n", &Nnum, Nstr);
    printf("%d %s\n", Nnum, Nstr);

    fclose(fp);
    return 0;
}
```

### File error check

```c
#include <stdio.h>

int ferror(FILE *stream);
int feof(FILE *stream);
void clearerr(FILE *stream);
```

#### [EX] ferror-ex.c

```c
#include <stdio.h>

int main(void){
    int ret;
    FILE *fp;

    fp = fopen("test.txt", "r");
    putc("?", fp);

    if (ret=ferror(fp)){ printf("ferror() return %d\n", ret); }
    clearerr(fp);
    printf("ferror() return %d\n", ferror(fp));
    fclose(fp);
    return 0;
}
```

#### [EX] EOF check, feof-ex.c

```c
#include <stdio.h>

int main(){
    int stat=0;
    FILE *fp;
    char buf[256];

    fp = fopen("test.txt", "r");
    while(!stat){
        if(fgets(buf, 256, fp)){ printf("%s\n", buf); }
        else{ stat = feof(fp); }
    }
    printf("feof returned %d\n", stat);
    fclose(fp);
    return 0;
}
```

### Error handling
- Important ANSI C Features
  - function prototypes
  - generic pointers (`void *`)
  - abstract data types (e.g. `pid_t`, `size_t`)
- Error Handling
  - `errno` 변수
  - `#include <errno.h>`

```c
#include <string.h>
char *strerror(int errnum); // R: pointer to msg string
#include <stdio.h>
void perror(const cahr *msg); // print the last error with the msg
```

# 3. File IO (System Call)
### Linux File System
- 각 파일은 `inode`를 갖고 있음
- inode is a data structure having all information on a file (meta data)
- 디스크에 상주 (inodes of all files reside in a disk)

### File types
- Regular file (Ordinary file)
  - Text, binary files
- Directory file
  - 파일명, inode #을 갖는 집합
- FIFO file (pipe)
  - Named pipe
  - Unnamed pipe (대부분 해당)
- Special files (IO devices)
  - Character: 문자 지향 장치 (키보드)
  - Block: 블록 지향 장치 (HDD), 큰용량
- Symbolic link files (우회해서 접근)
  - a file which points to another file
  - 하드링크는 파일이 아님

### File Descriptor
- A FD (or file handle) is a small, non-negative interger which identifies a file to the kernel
- `stdin, stdout, stderr` => `0, 1, 2`
- 최대 1024개의 파일을 열 수 있음 (`ulimit -a`로 확인)

### File open

```c
#include <fcntl.h>

int open(const char *path, int oflag);
int open(const char *path, int oflag, mode_t mode);
```

#### oflag options (in <fcntl.h>)
- `O_RDONLY`: read only (0)
- `O_WRONLY`: write only (1)
- `O_RDWR`: read and write (3)

#### 부가 옵션
- `O_CREAT`: 파일 생성
- `O_EXCL`: `O_CREAT`와 함쎄 사용, 이미 파일이 존재하면 에러 반환
- `O_TRUNC`: 파일이 있는 경우, 내용을 지움
- `O_APPEND`: 파일의 끝부분부터 입력
- `O_SYNC`: file I/O와 동시에 디스크 동기화 실행

#### File access modes
- `S_ISUID`: set-user-id at execution
- `S_ISGID`: set-group-id at exection
- `S_ISVTX`: set sticky bit
- `S_IRWXU`: owner RWX
- `S_IRUSR`: owner R
- `S_IWUSR`: owner W
- `S_IXUSR`: owner X
- ... (U/USR -> owner, G/GRP -> group, O/OTH -> others)

### File close

```c
#include <unistd.h>

int close(int fd);
```

#### [EX] File open, open-ex.c

```c
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    FILE *fpo; // FP
    int fdo;  // FD

    if (argc!=2){ perror(argv[0]); return 1; }
    // 파일 생성 or 열기 / User 읽기 실행
    if ((fdo=open(argv[1], O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR))==-1){
        perror(argv[1]);
        return 1;
    }

    if ((fpo=fdopen(fdo, "r+"))==NULL){
        perror("fdopen");
        return 2;
    }

    fprintf(fpo, "Hello, world!\n");
    fclose(fpo);

    return 0;
}
```

### File creation

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int creat(const char *path, mode_t mode);
```

### File seeking

```c
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

### File reading

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t nbyte);
```

### File writing

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t nbyte);
```

#### [EX] File create/lseek, create-ex.c

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void){
    int fd;
    char buf1[] = "Test1 data";
    char buf2[] = "Test2 data";

    if ((fd=creat("test.txt", S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0){
        printf("creat error");
        return 1;
    }

    write(fd, buf1, 10);
    if(lseek(fd, 6L, SEEK_SET)==-1){
        printf("lseek error");
        return 2;
    }

    write(fd, buf2, 10);

    return 0;
}
```

#### [EX] fcopy2-ex.c

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    int fdi, fdo;
    char buf[BUFFER_SIZE];
    ssize_t n;

    if (argc!=3){ perror(argv[0]); return 1; }
    if ((fdi=open(argv[1], O_RDONLY))==-1){ perror(argv[1]); return 2; }
    if ((fdo=open(argv[2], O_WRONLY|O_CREAT|O_TRUNC|O_EXCL, S_IRUSR|S_IWUSR))==-1){ perror(argv[2]); return 3; }

    while ((n=read(fdi, buf, BUFFER_SIZE))>0){
        write(fdo, buf, n);
    }

    close(fdi);
    close(fdo);
    return 0;
}

```

### Duplicattion of FD

```c
#include <unistd.h>

int dup(int fd);
int dup2(int fd1, int fd2); // src, des
```

#### [EX] io-redir.c

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
    int backup_des, stdout_des, ofdes;

    stdout_des = fileno(stdout);
    backup_des = dup(stdout_des);
    printf("Hello, world! (1)\n");

    ofdes = open("test.txt", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);

    dup2(ofdes, stdout_des);
    printf("Hello, world! (2)\n");

    dup2(backup_des, stdout_des);
    printf("Hello, world! (3)\n");

    close(ofdes);
}
```

### Link
- 참조 개념
- Symbolic Link (Soft Link)
  - 타겟이되는 파일의 경로를 저장하고 있음
  - 타겟 파일이 삭제되면 유효하지 않지만, 존재하고 있음
- Hard Link
  - 존재하는 `inode`를 가리키는 파일
  - `inode`가 공유됨
  - 하나의 파일이 업데이트되면, 모두 업데이트 됨
  - 하나의 파일이 사라져도 다른 하드 링크로 접근 가능

```bash
$ ln -s original_file symbolic_link_name
$ ln original_file hard_link_name
```

### Hard link

```c
#include <unistd.h>

int link(const char *existing, const char *new_link);
```

### File permission attributes
- 파일 종류_특수권한_소유자_그룹_기타

### Process's Creator

```c
#include <sys/types.h>
#include <unistd.h>

uid_t getuid(void); // User
uid_t getgid(void); // Group
```

- uid=euid (일반적)
- dynamic protection system에서는 동일하지 않음

### File's ID
- 프로세스 생성시, 생성자의 ID가 프로세스에 할당 됨
- 파일 소유자의 ID가 euid로 설정되는 경우
  - `S_ISUID` or `S_ISGID` 비트로 실행

#### How to set the bits

```bash
$ chmod u+s a.out
$ chmod g+s a.out
```

### Sticky bit: S_ISVTX
- 사용자는 디렉토리에 자신의 파일 또는 하위 디렉토리를 만들 수 있음
- 그러나, 각 파일은 소유자나 관리자에 의해서만 삭제 가능

### File access of a process
- 다음과 같은 경우에만 파일에 접근 가능
    1. EUID==0 -> supervisor
    2. EUID==file owner, access permission bit of owner SET
    3. EGID==file owner, access permission bit of group SET
    4. other's access permission bit is SET

### File's access permission

```c
#include <unistd.h>

int access(const char *path, int amode); // R: 0S-1E
```

- 파일에 접근 가능한지 조사
- `amode`
  - `R_OK`: READ permission check
  - `W_OK`: WRITE permission check
  - `X_OK`: Execute or Exploration permission check
  - `F_OK`: File existence check

#### [EX] access-ex.c

```c
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if(argc<2){ perror("argument error"); return 1; }

    if (access(argv[1], F_OK)==0){
        printf("%s: File Exists\n", argv[1]);
        if (access(argv[1], R_OK)==0) printf("%s: Read\n", argv[1]);
        if (access(argv[1], W_OK)==0) printf("%s: Write\n", argv[1]);
        if (access(argv[1], X_OK)==0) printf("%s: Execute\n", argv[1]);
    }else printf("%s: NOT exist\n", argv[1]);

    return 0;
}
```

### Default permission change (기본 권한 수정)

```c
#include <sys/types.h>
#include <sys/stat.h>

mode_t umask(mode_t cmask);
```

- default file: 0666 (rw-rw-rw)
- default dir: 0777 (rwx-rwx-rwx)
- 해당 함수로, 끄고자하는 권한을 입력
- 0022 -> 0644

### File permission change (기존 권한 수정)

```c
#include <sys/types.h>
#include <sys/stat.h>

int chmod(const char *path, mode_t mode);
int fchmod(int fd, mode_t mode);
```

#### [EX] chmod-ex.c

```c
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    struct stat statbuf;
    if (argc!=2){ perror("argument error"); return 1; }
    // 파일 정보 가져오기
    if (lstat(argv[1], &statbuf)<0){ perror("lstat"); return 2; }
    // 일반 파일이면,
    if (S_ISREG(statbuf.st_mode)){
        if(chmod(argv[1], (statbuf.st_mode & ~S_IXGRP))<0){ perror("chmod"); return 3; } // 그룹 실행 권한 off
    }else{ printf("%s is not regluar file\n", argv[1]); }

}
```

### Ownership change

```c
#include <unistd.h>
#include <sys/types.h>

int chown(const char *path, uid_t owner, gid_t group);
int lchown(const char *path, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
```

#### [EX] chown-ex.c

```c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){
    int owner_id, group_id, filedes;
    
    if(argc!=5){ perror("argument error"); return 1; }

    owner_id = atoi(argv[3]);
    group_id = atoi(argv[4]);
    if (strcmp(argv[1], "chown")==0){
        if(chown(argv[2], owner_id, group_id)){ perror("chown"); return 2; }
        printf("chown %s to %s, %s\n", argv[2], argv[3], argv[4]);
    }
    else if (strcmp(argv[1], "fchown")==0){
        filedes = open(argv[2], O_RDWR);
        if (fchown(filedes, owner_id, group_id)){ perror("chown"); return 3; }
        printf("fchown %s to %s, %s\n", argv[2], argv[3], argv[4]);
    }
    else if (strcmp(argv[1], "lchown")==0){
        if (lchown(argv[2], owner_id, group_id)){ perror("lchown"); return 4; }
        printf("lchown %s to %s, %s\n", argv[2], argv[3], argv[4]);
    }
}
```

#### [EX] hlink-ex.c

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    if (argc!=3){ perror("argument error"); return 1; }
    if (link(argv[1], argv[2])<0){ perror("link fail"); return 2; }
}
```

### Symbolic link

```c
#include <unistd.h>

int symlink(const char *existing, const char *link_name);
```

#### [EX] symlink-ex.c

```c
#include <unistd.h>

int main(int argc, char *argv[]){
    if (argc!=3){ perror("argument error"); return 1; }
    if (symlink(argv[1], argv[2])<0){ perror("symlink fail"); return 2; }
}
```

### System calls and Symbolick links
- 소프트 링크를 따르는 시스템 콜
  - lchown
  - lstat
  - remove
  - readlink
  - rename
  - unlink
- 따르지 않는 시스템 콜
  - accessm chdir
  - chmod
  - chown
  - ...

### Following a link

```c
#include <unistd.h>

int readlink(const char *path, void *buf, size_t bufsize);
```

- 링크하고 있는 원본 파일 이름을 출력

#### [EX] readlink-ex.c

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#define BUFFER_SIZE 100

int main(int argc, char *argv[]){
    char buf[BUFFER_SIZE];
    int read_size = 0;

    if (argc!=2){ perror("argument error"); return 1; }
    if ((read_size=readlink(argv[1], buf, BUFFER_SIZE))<0){ perror("readlink"); return 2; }

    buf[read_size] = '\0';
    printf("%s\n", buf);
}
```

### File information retrieval

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stat(const char *path, struct stat *buf);
int lstat(const chat *path, struct stat *buf); // 링크 파일인 경우 자기 자신의 정보를 출력
int fstat(int fd, struct stat *buf);
```

#### struct stat fields

```c
struct stat {
dev_t st_dev; // device
ino_t st_ino; // i-node #
mode_t st_mode; // access mode
nlink_t st_nlink; // number of hard links
uid_t st_uid; // owner id
gid_t st_gid; // group owner
dev_t st_rdev; // device type (if inode device)
off_t st_size; // total size of file
long st_blksize; // block size for I/O
long st_blocks; // number of blocks allocated
time_t st_atime; // time of last access
time_t st_mtime; // time of last modification
time_t st_ctime; // time of last change (including ownership change)
};
```

#### Macros for stuct stat

- `S_ISREG(st_mode)` return true if the file is regular file
- `S_ISDIR(st_mode)` return true if the file is directory
- `S_ISCHR(st_mode)` return true if the file is character device file
- `S_ISBLK(st_mode)` return true if the file is block device file
- `S_ISFIFO(st_mode)` return true if the file is FIFO file
- `S_ISLNK(st_mode)` return true if the file is link file
- `S_ISCOCK(st_mode)` return true if the file is socket file

#### [EX] fstat-ex.c

```c
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){
    struct stat statbuf;

    if (argc!=3){ perror("argument error"); return 1; }

    if (!strcmp(argv[1], "stat")){
        if (stat(argv[2], &statbuf)<0){ perror("stat"); return 2; }
    }
    else if (!strcmp(argv[1], "fstat")){
        int filedes = open(argv[2], O_RDWR);
        if (fstat(filedes, &statbuf)<0){ perror("fstat"); return 3; }
    }
    else if (!strcmp(argv[2], "lstat")){
        if (lstat(argv[2], &statbuf)<0){ perror("lstat"); return 4; }
    }

    if(S_IREG(statbuf.st_mode)) printf("%s is Regular File\n", argv[2]);
    if(S_ISDIR(statbuf.st_mode)) printf("%s is Directory\n", argv[2]);
    if(S_ISLNK(statbuf.st_mode)) printf("%s is Link File\n", argv[2]);
}
```

# 4. Concurrent Process

# 5. Process Control

# 6. Threads

# 7. Threads-RecordLock