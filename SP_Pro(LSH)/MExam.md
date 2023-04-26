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
- 여러개의 프로세스가 동시에 동작하는 프로그램
- IPC(Inter Process Communication)

### Process & PID
- Process: 자신만의 주소 공간에서,
  - Text(Code)
  - Data
  - Stack
  - Heap 영역을 갖고 운용되는 프로그램
- PID=0: Scheduler known as thw swapper
- PID=1: init process (/etc/init or /sbin/init)
- PID=2: pagedaemon, 가상 메모리를 지원
- 각 프로세스는 고유의 PCB(Process Control Block)을 갖고 있음

### System calls for PIDs

```c
#include <unistd.h>

pid_t getpid(void); // R: process ID of calling process
pid_t getppid(void); // R: parent process ID of calling process
uid_t getuid(void); // R: real user ID of calling process
uid_t geteuid(void); // R: effective user ID of calling process (유효 사용자=프로세스가 수행할때 따르는 권한)
gid_t getgid(void); // R: real group ID of calling process
gid_t getegid(void); // R: effective group ID of calling process
```

### Process creation in Linux

```c
#include <unistd.h>

pid_t fork(void);
```

- 자식 프로세스(child process) 생성
  - 부모의 클론
- PID==0 -> chlide
- PID!=0 -> parent

### Parent and Child
- Child는 Parent의 Copy(Clone)
- Child는 Parent의 자원을 상속 받음
  - Text
  - real/effective UID, GID
  - current working, root dir
  - open files before fork (including tty, (stdin, stdout, stderr))
  - share r/w offsets of the files whicj the parent opened before fork
- 같은 코드를 수행하지만, 서로 다른 프로그램임
  - Data, Heap, Stack 등의 내용이 달라짐 (`fork`시 복사되긴 함)
  - 다른 PID와 PCB
  - 커널에 의한 독립된 스케줄
  - private resources 존재
- TEXT (공유) / DATA, HEAP, STACK (Private area)
- `CHILD_MAX`를 초과하면 fork 실패

### Terminating a process

```c
#include <stdlib.h>

void exit(int status);
```

- `exit()`로 인한 상태가 부모에게 전달
- 부모 프로세스는 `wait()`를 통해 해당 값을 전달 받음
- 일반적으로 5가지 종료 방법이 있음
  - executing a return from the main function
  - calling the `exit()` func.
  - calling the `pthread_exit` func. from the last thread in the process
- 커널은 종료된 프로세스에 대해 다음 작업을 수행
  - 열린 FD 닫기
  - 할당된 메모리 반환

#### Zombie process
- 자식 프로세스보다, 부모 프로세스가 먼저 종료되는 경우

### Waiting a child process

```c
#include <sys/wait.h>

pid_t wait(int *status); // 불특정 다수
pid_t waitpid(pid_t pid, int *status, int options); // 특정, pid>0: 동일한 pid를 갖은 자식 대기, pid==-1: 아무 자식 대기
```

- 부모 프로세스에서, 자식 프로세스 종료를 대기
- 자식의 PCB를 제거 (`wait`)

### Macros for exit status
- `WEXISTAUTS(status)`: fetch the exit code which the child sends
- `WIFEXITED(status)`: true if the child terminated normally
- `WIFSIGNALED(status)`: true if status was child terminated abnormally by a signal
- `WIFSTOPPED(stats)`: true if status was returned for a child that is currently stopped
- `WTERMSIG(status)`: fetch the signal # that caused the child to be terminated
- `WCOREDUMP(status)`: true if a core file or the terminated process was generated

#### [EX] wait() and exit() example

```c
#include <wait.h>

void main(void){
  int res, pid;

  res = fork();
  if (res==0){ // Child
    // Do Something
    exit(77);
  }
  else{ // Parent
    pid = wait(&status);
    printf("The child %s id exited with %d\n", pid, WEXITSTATUS(status));
  }
}
```

### exec() system call
- 호출한 프로세스가 새로운 프로세스로 대치 됨
- PID 변경 X

### exec family

```c
#include <unistd.h>

int excel(const char *path, const char *arg, ... /*(char *) NULL*/);
int execlp(const char *file, const char *arg, ... /*(char *) NULL*/);
int execle(const char *path, const char *arg, ... /*(char *) NULL, char *const envp[]*/);

int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
```

- Exec family of call
  - path/file: 실행할 program binary
    - file의 경우, 현재 dir 및 PATH 환경 변수에 정의된 경로에서 file을 찾음
  - arg/argv[]: 실행할 program의 인자
    - 마지막에는 `NULL`을 넣어주어야 함
  - envp[]: program 실행 시, 새로 지정할 환경 변수
    - 마지막에는 `NULL`을 넣어주어야 함
  - Return=없음, 성공, -1 error

- l과 v의 차이는 전달되는 인자의 타입
- p가 붙은 경우에는 file 경로를 줌
- e가 붙은 경우 환경 변수를 인자로 넣어줌

### Shell & Process tree
- `./a.out &`: a.out as a background process
- `./a.out > output`: stdout is changed to "output" before exec() (I/O redirection)

#### [EX] Simple shell example

```c
while(not logout){
  print prompt;
  get a line string;
  parse the line;
  if((pid=fork())==0){
    // child
    // IO redirection if designated
    execv("parse name", argv);
  }
  if(foreground){ // &
    while(pid!=wait());
  }
  // if background, does not wait()
}
```

### Multitask scheduling
- Context switching (or process switching): 일정 시간마다 스위칭 (time-slicing)
- Race condition 야기 가능
- Critical Section -> mutual exclusion으로 해결

### Inter-Process Communication (IPC)
- 프로세스간 통신
- msg. queue
- semaphores
- signals
- network sockets
- pipe: circular queue
  - named pipe: 모든 프로세스 간
  - unnamed pipe: 부모 자식 간

### Pipe
- IPC tool between processes
- avoid a race condition by block & wakeup
- A FIFO file create with two r/w pointers
- 빈 파이프에 대한 읽기 시도로 인한 차단 발생
- 읽기 파이프가 닫힌 경우, 쓰기 파이프가 활성화

#### [EX] Pipe example

```c
int main(void){
  int fd[2];
  int input, output;
  int n;
  char buf[10];

  pipe(fd); // fd[0] for reading, fd[1] for writing

  if(fork()==0){
    // child
    input = open("input.dat", 0);
    close(fd[0]); // close read pipe
    while ((n=read(input, buf, 10))!=0){
      write(fd[1], buf, n);
    }
    close(input);
    close(fd[1]);
    exit(0);
  }
  else{
    // parent
    close(fd[1]);
    output = creat("output.dat", 0666);
    while ((n=read(fd[0], buf, 10))!=0){
      write(output, buf, n);
    }
    close(output);
    close(fd[0]);
    wait();
  }
}
```

# 5. Process Control
### Getting Process IDs

```c
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void); // R: PID of caller process
pid_t getpgrp(void); // R: Process GID of caller
```

- Session
  - A Group of processes
  - session is a kind of work space
  - group leader called session leader
- Process GID (group id): will be a session leader's pid (usually, shell's pid)

### Getting Parent Process ID

```c
#include <sys/types.h>
#include <unistd.h>

pid_t getppid(void); // R: PID of caller's parent
pid_t getpgid(pid_t tpid); // tpid: process id for inquiry
```

#### [EX] pid-ex.c

```c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(){
  int pid;
  printf("Original process: pid = %d, ppid = %d, pgrp = %d, pgid = %d\n", getpid(), getppid(), getpgrp(), getpgid(getpid()));

  pid = fork();
  if(pid!=0){
    // parent
    printf("Parent process: pid = %d, ppid = %d, pgrp = %d, pgid = %d\n", getpid(), getppid(), getpgrp(), getpgid(getpid()));
  }
  else{
    // child
    printf("Child process: pid = %d, ppid = %d, pgrp = %d, pgid = %d\n", getpid(), getppid(), getpgrp(), getpgid(getpid()));
  }
  return 0;
}
```

### Process Group Leader
- A session leader process' pid will be the pgid
- 시그널은 모든 그룹의 프로세스에게 감
- 세션 리더가 종료되면, `SIGHUP`이 모든 그룹 프로세스에 전달되고 종료 됨
- `nohup`을 이용해, 로그아웃시에도 동작 가능하게 만들 수 있음 or demon 형태

```bash
$ nohup command ... &
```

### Setting Process Group ID

```c
#include <sys/types.h>
#include <unistd.h>

int setpgid(pid_t pid, pid_t pgid);
```

### Process Running Start
- `exec` system call
- C start-up routine

### Environment

```c
#include <stdlib.h>

char *getenv(const char *name);
int putenv(const char *name);
int setenv(const char *name, const char *value, int rewrite); // rewrite: 0이 아니면, 기존 값이 새로운 값으로 대체 됨. 0이면 유지.
```

#### [EX] env-ex.c

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  char **ptr;
  char *nptr;
  extern char **environ;

  for(ptr=environ ; *ptr!=0 ; ptr++){
    // print all env.
    printf("%s \n", *ptr);
  }

  nptr = getenv("HOME");
  printf("HOME = %s\n", nptr);

  nptr = getenv("SHELL");
  printf("SHELL = %s\n", nptr);

  nptr = getenv("PATH");
  printf("PATH = %s\n", nptr);

  exit(0);
}
```

### Process Termination
- Normal termination (정상 종료)
  - main이 종료되면 반환, C 시작 루틴은 `exit()`를 call
  - 프로그램 내에서 직접 호출
  - `exit()`: 뒷정리 후, 프로세스 종료
  - `_exit()`: 뒷정리 없이, 프로세스 종료
- Abnormal termination (비정상 종료)
  - `abort()`
  - `SIGABRT` 시그널을 프로세스에게 보냄
  - 시그널에 의한 종료

### Exit handler: atexit()

```c
#include <stdlib.h>

int atexit(void (*func)(void));
```

- 종료 시, 실행할 함수 등록

#### [EX] fptr-ex.c

```c
#include <stdio.h>

void greet(){ printf("Hello, world!\n"); }
void caller(void (*myFunc)(void)){ myFunc(); }

int main(){
  void (*sayHello)(void);

  printf("Calling greet() directly.\n");
  greet();
  printf("\n");

  printf("Calling greet() via a pointer.\n");
  sayHello = greet;
  sayHello();
  printf("\n");

  printf("Calling greet() via another function.\n");
  caller(greet);
}
```

#### [EX] atexit-ex.c

```c
#include <stdio.h>
#include <stdlib.h>
#define TMPFILE "/tmp/mylog"

static void myexit(void){
  if(unlink(TMPFILE)){ perror("myexit: unlink"); exit(3); }
}

int main(void){
  FILE *fp;
  if (atexit(myexit)!=0){ perror("atexit error"); exit(1); }
  if ((fp = fopen(TMPFILE, "a+"))==NULL){ perror("fopen error"); exit(2); }
  fprintf(fp, "This is temporary log entry of pid %d\n", getpid());
  close(fp);
  exit(0);
}
```

### Daemons
- 터미널과 연결되어 있지 않은 백그라운드 프로세스
- 일반적으로 부팅시 실행 or 특별한 사용자에 의해 실행
- 프로세스 마지막에 `d`가 붙음

#### Demon two condition
1. `init`의 자식으로서 실행될 것
2. 터미널과 연결되어 있지 않을 것

#### To become daemon,
1. `fork()` call
2. Call `exit()` in parent
3. Call `setsid()` (새로운 프로세스 그룹과 세션 할당)
4. Using `chdir()` to go to the lot dir
5. Close all FD
6. Connect FD 0, 1, 2 (stdin, stdout, stderr) to /dev/null

### setsid()

```c
#include <sys/types.h>
#include <unistd.h>

pid_t setsid(void);
```

- 새로운 세션 생성
- 제어 불가능

#### [EX] daemon-ex.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void){
  pid_t pid;
  int i = 10000;

  if ((pid=fork())<0){ perror("fork error"); }
  else if (pid==0){
    // child
    printf("Child: pid=%d\n", getpid());
    // -> daemon
    close(0); close(1); close(2);
    setsid();
    while(1){
      printf("c(%d)\n", i);
      i++;
      sleep(1);
    }
  }
  else{
    // parent
    printf("Parent: pid=%d\n", getpid());
    printf("Parent: Child pid=%d\n", pid);
    sleep(1);
    printf("Parent: exit\n");
    exit(0);
  }
}
```

```bash
$ps -ef // 현재 시스템에서 돌아가는 모든 프로세스 확인
```

#### [EX] zombie-ex.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void){
  pid_t pid;
  int i = 10000;

  if ((pid=fork())<0){ perror("fork error"); }
  else if (pid==0){
    // child
    printf("Child: pid=%d\n", getpid());
    while(1){
      printf("c(%d)\n", i);
      i++;
      sleep(1);
    }
  }
  else{
    // parent
    printf("Parent: pid=%d\n", getpid());
    printf("Parent: Child pid=%d\n", pid);
    sleep(1);
    printf("Parent: exit\n");
    exit(0);
  }
}
```

### system()

```c
#include <stdlib.h>

int system(const char *string);
```

- 전달한 커맨드를 쉘에서 실행

#### [EX] system-ex.c

```c
#include <stdio.h>

int main(int argc, char *argv[]){
  int i;
  char cmdstr[1024];

  strcpy(cmdstr, "/bin/ls");
  for(i=1 ; i<argc ; i++){
    strcat(cmdstr, agrv[i]);
    strcat(cmdstr, " ");
  }
  fprintf(stdout, "cmdstr = \"%s\"\n", cmdstr);

  system(cmdstr)l

  exit(0);
}
```

### Process time

```c
#include <sys/times.h>

clock_t times(struct tms *buf);

struct tms(
  clock_t tms_utime; // user CPU time
  clock_t tms_stime; // system CPU time
  clock_t tms_cutime; // child user CPU time
  clock_t tms_cstime; // child system CPU time
)
```

- user: 사용자
- system: 커널 (사용자 프로세스의 절반일수도)

#### [EX] time-ex.c

```c
#include <sys/times.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend){
  static long clktck = 0;
  if (clktck==0){
    if ((clktck=sysconf(_SC_CLK_TCK))<0){ err_sys("sysconf error"); }
  }
  printf("real: %7.2f\n", real/(double)clktck);
  printf("user: %7.2f\n", (tmsend->tms_utime - tmsstart->tms_utime)/(double)clktck);
  printf("sys: %7.2f\n", (tmsend->tms_stime - tmsstart->tms_stime)/(double) clktck);
  printf(" child user: %7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime)/(double)clktck);
printf(" child sys: %7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime)/(double) clktck);
}
static void do_cmd(char *cmd){
  struct tms tmsstart, tmsend;
  clock_t start, end;
  int statusl
  printf("\ncommand: %s\n", cmd);

  if ((start=times(&tmsstart))==-1){ err_sys("times error"); }
  if ((status=system(cmd))<0){ err_sys("system() error"); }
  if ((end=times(&tmsend))==-1){ err_sys("times error"); }
  pr_times(end-start, &tmsstart, tmsend);
}
void err_sys(char *p){ perror(p); eixt(-1); }

int main(int argc, char *argv[]){
  int i;
  for (i=1 ; i<argc ; i++){ do_cmd(argv[i]); }
  exit(0);
}
```

# 6. Threads
### Concurrent Processes
- A process includes many things:
  - An address space
  - OS resources and accounting info
  - HW execution state
- 위와 같은 이유로, 새로운 프로세스를 생성하는건 많은 비용이 듬
- IPC를 사용하는것도 비용이 들지만 OS에서 주로 사용 (시스템 콜, 데이터 복사 오버헤드)

### Rethinking the processes?
- What's similar in these cooperating processes?
  - share the same CODE and DATA (addr space)
  - same privilege
  - same resource (files, sockets, etc.)
- Diff
  - Each has its own HW execution state (PC, reg. SP, STACK)

### What is a thread?
- A thread of control
  - 프로세스 컨트롤 흐름
  - 프로세스는 단일-스레드
  - 멀티 스레드 프로세스는 다양한 컨트롤 흐름이 존재
  - 스레드는 CPU 스케쥴링 단위
- Thread ID, PC(program counter), reg.로 구성
- STACK은 지역 변수와 반환 주소를 갖음
- 다중 스레드가 공유하는 것
  - CODE, DATA, OS resource
- flow control만 독립적으로 유지
- 멀티 프로세스는(`fork`) 오버헤드가 증가함

### Multi-threaded application
- single-threaded > multi-process -> multi-threaded

### Thread libraries
- POSIX Pthreads

### POSIX Pthreads

```c
#include <pthread.h>
```

```bash
$ gcc test.c -lpthread
```

#### Pthreads API and data types
- `pthread_`: 스레드 자체 및 기타 서브루틴
- `pthread_attr_`: 스레드 속성 객체
- `pthread_mutex_`: mutex 루틴
- `pthread_mutexattr_`: mutex 속성 객체
- `pthread_cond_`: Condition 변수 루틴
- `pthread_condattr_`: Condition 변수 객체
- `pthread_key_`: Thread_specific_data keys

### pthread_ create

```c
#include <pthread.h>

int pthread_create(pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
```

- 새로운 스레드 생성
- `thread`: 스레드 ID 저장
- `attr`: 새로운 스레드의 속성 (default=NULL)
- `start_routine`: 새로운 스레드에서 수행할 함수
- `arg`: 스레드 함수에 전달한 인자

### Terminating a pthread
- a thread returns from its start routine
- a thread makes a call to `pthread_exit` func
- the thread is cancelled by another thread via the `pthread_cancel` func.
- the entire process is terminated due to `exit` call

```c
#include <pthread.h>

void pthread_exit(void *retval); // retval: 호출하는 다른 스레드에 전달되는 값 pthread_join to wait 
```

### Detaching/Joining a thread

```c
#include <pthread.h>

int pthread_detach(pthread_t id); // 독립
int pthread_join(pthread_t tid, void **retval); // 조인, PTHREAD_CANCELED is placed on retval, target thread is cnacelled
```

- 한번 분리되면, 다시 join 불가
- 독립(분리)시, 자원 즉시 할당
- `pthread_join` 유사 `waitpid`

### Getting Thread ID

```c
#include <pthread.h>

int pthread_self(void);
int pthread_equal(pthread_t t1, pthread_t t2); // 동일하면 0이 아닌 값 반환
```

#### [EX] th_hello.c (pthread example)

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int global;

void *printmsg(void *msg){
  int i=0;
  while (i<5){
    printf("%s: %d\n", (char*)msg, i++);
    sleep(1);
  }
  pthread_exit((void*)pthread_self());
}

void main(){
  pthread_t t1, t2, t3;
  void *rval1;
  void *rval2;
  int mydata;

  pthread_create(&t1, NULL, printmsg, "Hello");
  pthread_create(&t2, NULLm printmsg, "World");

  pthread_join(t1, (void*)&rval1);
  pthread_join(t2, (void*)&rval2);

  printf("t1: %lu, t2: %lu\n", t1, t2);
  printf("rval1: %lu, rval2: %lu\n", (unsigned long)rval1, (unsigned long)rval2);
}
```

### Thread cancellation
- 완료전에 스레드가 종료
- call `pthread_cancel(pthread_t tid)`
- Cancellation types
  - asynchronous cancellation: 즉시 종료
  - deferred cancellation: 종료 연기
    - cancellation points에서 종료 (종료 가능 여부 체크)
    - call `pthread_testcancel()` to make a cancellation point
    - call `pthread_cleanup_push()` to register a cleanup handler which is called at cancellation point

```c
#include <pthread.h>

int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
```

#### Cancellation modes (PTHREAD_CANCEL_??)

| mode | state | type |
| :--- | :--- | :--- |
| _DISABLE | NOT | cancellable |
| _ENABLE | cancellable | not used |
| _DEFERRED | cancellable | deferred |
| _ASYNCHRONOUS | cancellable | immediate |

#### [EX] th_cancel.c

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>

void *threadFunc(void *arg){
  int count = 0;
  printf("new thread started ...\n");
  int retval;

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  prhtead_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  while(1){
    printf("count = %d\n", count++);
    sleep(1);
    pthread_testcancel();
    if(count==10) break;
  }
  pthread_exit((void*)count);
}

int main(int argc, char *argv[]){
  pthread_t tid;
  int retval;
  void *res;

  pthread_create(&tid, NULL, threadFunc, NULL);
  sleep(5);
  pthread_cancel(tid);
  retval = pthread_join(tid, &res);
  if (retval!=0){ perror("pthread_join: "); exit(EXIT_FAILURE); }
  if (res==PTHREAD_CANCELED){ printf("thread canceled\n"); }
  else{ printf("thread is normal exit retval = %d\n", (int)rest); }

  exit(EXIT_SUCCESS);
}
```

#### [EX] Thread arguments example

```c
#include <pthread.h> 
#include <stdio.h> 
#include <malloc.h> 
#include <string.h>
#define NUM_THREADS3

struct thread_data{ 
  int thread_id; 
  int sum; 
  char *message; 
}; 
struct thread_data thread_data_array[NUM_THREADS];
pthread_t threads[NUM_THREADS];

void *printHello(void *arg){ 
  struct thread_data *my_data;
  int taskid, sum;
  char *hello_msg;
  my_data = (struct thread_data *) arg; 
  taskid = my_data->thread_id; 
  sum = my_data->sum; 
  hello_msg = my_data->message; 
  printf("taskid = %d\n",taskid);
  printf("sum = %d\n", sum);
  printf("message = %s\n",hello_msg);
}

int main(int argc, char *argv[]){
  int rc, i, sum;
  void *res;
  char messages[3][1024];
  strcpy(messages[0], "hello");
  strcpy(messages[1], "system programming");
  strcpy(messages[2], "world");

  for(i=0 ; i<3 ; i++){
    sum += i;
    thread_data_array[i].thread_id = i;
    thread_data_array[i].sum = sum;
    thread_data_array[i].message = messages[i];
    rc = pthread_create(&threads[i], NULL, printHello, (void*)&thread_data_array[i]);
  }
  for(i=0; i<3 ; i++){ pthread_join(threads[i], &res); }
}
```

### Dangerous Argument Passing
- implcit sharing

### Thread Synchronization

### Mutex (Mutual exclusion)
- Mutex variables
  -  공유 데이터 보호 목적

### pthread mutex creation

```c
#include <pthread.h>

int pthread_mutex_init(*mutex, *attr);
int pthread_mutex_destroy(*mutex);
int pthread_mutexattr_init(*attr);
int pthread_mutexattr_destroy(*attr);

pthread_mutex_t *mutex
pthread_mutexattr_t *attr
```

- Mutex 변수는 선언 뒤에 반드시 초기화 되어야 함.

```c
pthread_mutex_t mymutex = THREAD_MUTEX_INITIALIZER;
...
pthread_mutex_init(mymutex, attr); // when call
```

### ptrhead_mutexattr
- protocol
- prioceiling
- process-shared

### Mutex Lock/Unlock

```c
int pthread_mutex_lock(*mutex); 
int pthread_mutex_unlock(*mutex); 
int pthread_mutex_trylock(*mutex); 
int pthread_mutexattr_unlock(*mutex);

pthread_mutex_t *mutex
pthread_mutexattr_t *attr
```

#### [EX] mutex_counter.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t mVar = PTHREAD_MUTEX_INITIALIZER;

void *Incrementer(){
  for(;;){
    pthread_mutex_lock(&mVar);
    counter++;
    printf("Inc: %d\n", counter);
    pthread_mutex_unlock(&mVar);
  }
}
void *Decrementer(){
  for(;;){
    pthread_mutex_lock(&mVar);
    counter--;
    printf("Dec: %d\n", counter);
    pthread_mutex_unlock(&mVar);
  }
}

int main(){
  pthread_t ptid, ctid;

  pthread_mutex_init(&mVar, NULL);

  pthread_create(&ptid, NULL, Incrementer, NULL);
  pthread_create(&ctid, NULL, Decrementer, NULL);
  pthread_join(ptid, NULL);
  pthread_join(ctid, NULL);
  return 0;
}
```

#### [EX] dot_product.c

```c
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>

typedef struct{
  double *a;
  double *b;
  double sum;
  int veclen;
}DOTDATA;

#define NUMTHRDS 4
#define VECLEN 100

DOTDATA dotstr;

pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;

void *dotprod(void *arg){
  int i, start, end, offsset, len;
  double mysum, *x, *y;

  offset = (int)arg;
  len = dotstr.veclen;
  start = offset*len;
  end = start+len;
  x = dotstr.a;
  y = dotstr.b;

  // dot product
  mysum=0;
  for(i=start ; i<end ; i++){ mysum += (x[i] * y[i]); }
  pthread_mutex_lock(&mutexsum);
  dotstr.sum += mysum;
  pthread_mutex_unlock(&mytexsum);
  pthread_exit((void*)0);
}

int main(int argc, char *argv[]){
  int i;
  double *a, *b;
  int status;

  a = (double*)malloc(NUMTHRDS*VECLEN*sizeof(double));
  b = (double*)malloc(NUMTHRDS*VECLEN*sizeof(double));
  // dummy
  for (i=0 ; i<VECLEN*NUMTHRDS ; i++){
    b[i] = a[i] = 1;
  }

  dotstr.veclen = VECLEN;
  dotstr.a = a;
  dotstr.b = b;
  dotstr.sum = 0;
  
  pthread_mutex_init(&mtuexsum, NULL);
  // Create threads to perform the dot-product
  for(i=0 ; i<NUMTHRDS ; i++){ pthread_create(&callThd[i], NULL, dotprod, (void*)i); }
  // Wait on the other threads
  for(i=0 ; i<NUMTHRDS ; i++){ pthread_join(callThd[i], (void **)&status); }

  printf("Sum=%f\n", dotstr.sum);

  free(a);
  free(b);

  pthread_mutex_destroy(&mutexsum);
  pthread_exit(NULL);
}
```

# 7. Threads-RecordLock
### Condition variables
- mutex를 사용하는 스레드간 동기화 목적
- wake up the waiting thread(s) by signaling

### Creating a condition variable

```c
#include <pthread.h>

int pthread_cond_init(*condition, *attr);
int pthread_cond_destroy(*condition, *attr);
int pthread_condattr_init(*attr);
int pthread_condattr_destroy(*attr);

pthread_cond_t *condition;
pthread_contattr_t *attr;
```

```c
// static
pthread_cond_t myconvar = PTHREAD_COND_INITIALIZER;
// dynamic
pthread_cond_init(*condition, *attr);
```

### Waiting/Signaling on CV

```c
pthread_cond_wait(condition, mutex);
pthread_cond_signal(condition); // wakes up the first of threads
pthread_cond_broadcast(condition); // wakes up all
```

#### [EX] cond-var.c

```c
#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

int count = 0;
int thread_ids[3] = {0, 1, 2};
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t count_threshold_cv = PTHREAD_COND_INITIALIZER;

void *int_count(void *idp){
  int i, j;
  double result = 0.0;
  int *my_id = idp;

  for(i=0 ; i<TCOUNT ; i++){
    pthread_mutex_lock(&count_mutex);
    count++;
    if (count==COUNT_LIMIT){
      pthread_cond_signal(&count_threshold_cv);
      printf("inc_count(): thread %d, count = %d Threshold reached.\n", *my_id, count);
    }
    printf("inc_count(): thread %d, count = %d, unlocking mutex\n", *my_id, count);
    pthread_mutex_unlock(&count_mutex);

    for (j=0 ; j<1000 ; j++){ result += (double)random(); }
  }
  pthread_exit(NULL);
}

void *watch_count(void *pid){
  int *my_id = pid;
  printf("Starting watch_count(): thread %d\n", *my_id);

  pthread_mutex_lock(&count_mutex);
  while(count<COUNT_LIMIT){
    pthread_cond_wait(&count_threshold_cv, &count_mutex);
    printf("watch_count(): thread %d Codition signal received.\n", *my_id);
  }
  pthread_mutex_unlock(&count_mutex);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  int i, rc;
  pthread_t threads[3];

  pthread_create(&thread[0], NULL, inc_count, (void*)&thread_ids[0]);
  pthread_create(&thread[1], NULL, inc_count, (void*)&thread_ids[1]);
  pthread_create(&thread[2], NULL, watch_count, (void*)&thread_ids[2]);

  for(i=0 ; i<NUM_THREADS ; i++){ pthread_join(threads[i], NULL); }

  printf("Main(): Waited on %d threads. Done\n", NUM_THREADS);
  pthread_exit(NULL);
}
```

#### [EX] prod-cons.c

```c
#include <stdio.h>
#include <pthread.h>

void *producer(void*);
void *consumer(void*);
#define MAX_BUF 100

int buffer[MAX_BUF];
int count = 0;
int in = -1;
int out = -1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER;

int main(void){
  int i;
  pthread_t threads[2];
  pthread_create(&threads[0], NULL, producer, NULL);
  pthread_create(&threads[1], NULL, consumer, NULL);

  for(i=0 ; i<2 ; i++){
    pthread_join(threads[i], NULL);
  }

  return 0;
}

void *producer(void *v){
  int i;
  for(i=0 ; i<1000 ; i++){
    pthread_mutex_lock(&mutex);
    if(count==MAX_BUF){ pthread_cond_wait(&buffer_has_space, &mutex); }
    in = in++ % MAX_BUF;
    buffer[in] = i;
    count++;
    pthread_cond_signal(&buffer_has_data);
    pthread_mutex_unlock(&mutex);
  }
}

void *consumer(void *v){
  int i, data;
  for(i=0 ; i<1000 ; i++){
    pthread_mutex_lock(&mutex);
    if(count==0){ pthread_cond_wait(&buffer_has_data, &mutex); }
    out = out++  % MAX_BUF;
    data = buffer[out];
    count--;
    pthread_cond_signal(*&buffer_has_space);
    pthread_mutex_unlock(&mutex);
    printf("data = %d\n", data);
  }
}
```

## Record Lock
### Concurrent-Readers / Exclusive-Writers
- writing -> exclusive lock
- reading -> shared lock

### Record Lock in a File

```c
#include <fcntl.h>

int fcntl(int filedes, int cmd, struct flock *lock);
```

- `cmd`
  - `F_GETLK`: check if the lock can be acquired
    - 이미 잠겨있으면, filled lock structure
    - can be acquired, lock structure with `F_UNLCK`
  - `F_SETLK`: try to set lock
    - l_type==`F_RDLCK` or `F_WRLCK`
  - `F_SETLKW`: if already locked, the calling process must be
blocked until it can get the lock (`F_SETLK` blocking ver.)

#### [EX] rec-pro.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define NUM_RECORDS 100

struct record{
  char name[20];
  int id;
  int balance;
};

void get_new_record(struct record *curr);
void display_record(struct record *curr);

int main(int argc, char *argv[]){
  FILE *fp;
  struct record current;
  int record_no;
  int fd;
  long pos;
  char yes;

  fd = open(argv[1], O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
  if((fp=fdopen(fd, "r+"))==NULL){ perror(argv[1]); exit(2); }
  printf("enter record number: ");
  scanf("%d", &record_no);
  getchar();

  while(record_no>=0 && record_no<NUM_RECORDS){
    pos = record_no * sizeof(struct record);
    fseek(fp, pos, SEEK_SET);
    fread(&current, sizeof(struct record), 1, fp);
    display_record(&current);
    printf("update records? yes = y\n");
    scanf("%c", &yes);
    getchar();
    if(yes=='y'){
      get_new_record(&current);
      fseek(fp, pos, SEEK_SET);
      fwrite(&current, sizeof(struct record), 1, fp);
      printf("update done\n");
    }
    printf("enter record number: ");
    scanf("%d", &record_no);
    getchar();
  }
  fclose(fp);
}

// 동시에 동일 레코드에 접근하지 않으므로 동작은 가능하지만, 읽는 와중에 데이터를 쓰는 경우 문제 발생
void get_new_record(struct record *curr){
  printf("> id? ");
  scanf("%d", &curr->id);
  printf("> name? ");
  scanf("%s", curr->name);
  printf("> balance? ");
  scanf("%d", &curr->balance);
}
void display_record(struct record *curr){
  printf("\n");
  printf("id: %d \n", curr->id);
  printf("name: %s \n", curr->name);
  printf("balance: %d \n", curr->balance);
  printf("\n");
}
```

#### [EX] reclock.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define NUM_RECORDS 100

struct record{
  char name[20];
  int id;
  int balance;
};

int reclock(int fd, int recno, int len, int type);
void display_record(struct record *curr);

int main(){
  sturct record current;
  int record_no;
  int fd, pos, i, n;
  char yes;
  char operation;
  int amount;
  char buffer[100];
  int quit=FALSE;

  fd = open("./account", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
  while(1){
    printf("enter account number (0-99): ");
    scanf("%d", &record_no);
    fgets(buffer, 100, stdin);

    if(record_no<0 && record_no>=NUM_RECORDS) break;
    printf("enter operation name (c/r/d/q): ");
    scanf("%c", &operation);

    switch(operation){
      case 'c':
        reclock(fd, record_no, sizeof(struct record), F_WRLCK);
        pos = record_no * sizeof(struct record);
        lseek(fd, pos, SEEK_SET);
        printf("> id ?");
        scanf("%d", &current.id);
        printf("> name ?");
        scanf("%s", &current.name);
        current.balance = 0;
        n = write(fd, &current, sizeof(struct record));
        display_record(&current);
        reclock(fd, record_no, sizeof(struct record), F_UNLCK);
        break;
      case 'r':
        reclock(fd, record_no, sizeof(struct record), F_RDLCK);
        pos = record_no * sizeof(struct record);
        lseek(fd, pos, SEEK_SET);
        n = read(fd, &current, sizeof(struct record));
        display_record (&current);
        reclock(fd, record_no, sizeof(struct record), F_UNLCK);
        break;
      case 'd':
      reclock(fd, record_no, sizeof(struct record), F_WRLCK);
        pos = record_no * sizeof(struct record);
        lseek(fd, pos, SEEK_SET);
        n = read(fd, &current, sizeof(struct record));
        display_record (&current);
        printf("enter amount\n");
        scanf("%d", &amount);
        current.balance += amount;
        lseek(fd, pos, SEEK_SET);
        write(fd, &current, sizeof(struct record));
        reclock(fd, record_no, sizeof(struct record), F_UNLCK);
        break;
      case 'q':
        quit = TRUE;
        break;
      default:
        printf("liiegal input\n");
        continue;
    }
  }
  close(fd);
  fflush(NULL);
}

int reclock(int fd, int recno, int len, int type){
  struct flock fl;
  switch(type){
    case F_RDLCK:
    case F_WRLCK:
    case F_UNLCK:
      fl.l_type = type;
      fl.l_whence = SEEK_SET;
      fl.l_start = recno*len;
      fl.l_len = len;
      fcntl(fd, F_SETLKW, &fl);
      return 1;
    default:
      return -1;
  }
}

void display_record(struct record *curr){
  printf("\n");
  printf("id: %d \n", curr->id);
  printf("name: %s \n", curr->name);
  printf("balance: %d \n", curr->balance);
  printf("\n");
}
```

### File Lock

```c
#include <fcntl.h>

int flock(int fd, int operation); // lock entire file
```

- LOCK_SH: place a shared lock
- LOCK_EX: place an exclusive lock
- LOCK_UN: remove an existing lock held by this process
- can be OR’d with LOCK_NB (non-blocking)

### Advisory vs. Mandatory Locking
- Advisory Locking
  - 커널에 의해 시행되지 않음
  - 읽기 또는 쓰기가 프로토콜 위반 가능
  - 프로세스가 자발적으로 프로토콜을 준수해야 함
- Mandatory Lockcing
  - 커널에 실행
  - 읽기 또는 쓰기가 프로토콜 위반 불가
  - 오버헤드가 높음 (모든 읽기를 검사해야해서)
  - 필수 잠금 비트 `chmod ~ lockfile` or `chmod g+s, g-x lockfile`