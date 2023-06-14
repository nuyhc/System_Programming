# 9. Memory
## Process Address Space
- 프로세스가 동작하는데 필수적인 자원
- 가상 공간 -> 커널 -> 물리 메모리 맵핑
- 페이지 단위로 관리
- Text - Initialized Data - Uninitialized Data(bss) - Heap - Stack

### Detail
1. Text (Code)
    - 프로그램의 CPU 명령어
    - Read-Only 상수
2. Data
    - 전역 초기화 변수 + 전역 미초기화 변수
    - `bss` = block started by symbol
    - 컴파일러와 링커가 처음 0 값의 비트로 표현되는 정적으로 할당된 변수를 포함하는 데이터 세그먼트
    - 전역 변수
    - 정적 변수
3. Heap: 동적 할당
4. Stack
    - 함수 인자 (automatic variables in a function)
    - Call frames (arguments, return addr, etc.)
    - 지역 변수

#### [EX] vars.c

```c
#include <stdio.h>
#include <stdlib.h>

int a = 1; // 전역 변수 -> Data
static int b = 2; // 정적 변수 -> Data

int main(){
    int c = 3; // 지역 변수 -> Stack
    static int d = 4; // 정적 변수 -> Data
    char *p; // 지역 변수 -> Stack

    p = (char*)malloc(40); // 동적 할당 -> Heap
    fun(5) // n=5 매개 변수 -> Stack
}

void fun(int n){
    int m = 6; // 지역 변수 -> Stack
    ...
}
```

## Dynamic Memory Allocation
- 필요에 따라 메모리를 할당하므로, 메모리를 절약 할 수 있음
- `malloc, calloc, realloc, free`

## Memory Allocation

```c
#include <stdlib.h>
// 메모리를 할당하고 void* 타입의 메모리 시작 주소를 반환
void* malloc(size_t size);
// 모든 값을 0으로 초기화해서 n개 할당, Fail -> NULL
void* calloc(size_t n, size_t size);
// 메모리 재할당
void* realloc(void* ptr, size_t size_t newsize);
// 할당된 메모리 해제
void free(void* ptr);
```

#### [EX] stud1.c (Memory allocation ex1)

```c
#include <stdio.h>
#include <stdlib.h>

struct student{
    int id;
    char name[20];
};

int main(){
    struct student* ptr;
    int n, i;
    printf("How many student?");
    scanf("%d", &n);
    if(n<=0){
        fprintf(stderr, "errpr: wrong number\n");
        fprintf(stderr, "terminate program\n");
        exit(1);
    }

    ptr = (struct student*)malloc(n*sizeof(struct student));
    if(ptr==NULL){ perror("malloc"); exit(2); }

    printf("enter student number and name for %d. students\n", n);
    for(i=0 ; i<n ; i++){ scanf("%d %s\n", &ptr[i].id, ptr[i].name); }

    printf("\n* student information*\n");
    for(i=0 ; i>=0 ; i--){ printf("%d %s\n", ptr[i].id, ptr[i].name); }

    printf("\n");
    exit(0);
}
```

## Memory Mapped File
- 파일의 특정 공간을 메모리에 맵핑
  - 런타임에 메모리 변수를 파일로 자동 저장
  - Mem-Read == Reading from a file
  - Mem-Write == Writing to a file (`page cache`)
  - 파일 데이터를, 메모리 배열이나 포인터처럼 사용
- No copy b/w lib. buffer and kernel buffer
  - 이중 캐싱 방지, file I/O 성능 개선
- IPC를 이용해, 여러개의 프로세스가 같은 메모리 공간을 사용 가능

```c
#include <sys/types.h>
#include <sys/mman.h>

// 실제 맵핑된 시작 주소 반환, Fail -> MAP_FAILED
caddr_t mmap(
    caddr_t addr, // 맵핑 시킬 메모리 시작 주소
    size_t len, // 맵핑 사이즈
    int prot, // 권한
    int flags, // 맵핑 스키마
    int fd,
    off_t offset
);
```

### prot and flags arguments
- prot (할당되는 권한과 파일의 열기 모드가 일치해야 함)
  - `PROT_READ`: Reading is possible
  - `PROT_WRITE`: Writing is possible
  - `PROT_EXEC`: Execution is possible
  - `PROT_NONE`: Nothing is allowed
- flags (MAP_SHARED or MAP_PRIVATE 지정)
  - `MAP_SHARED*`: 메모리에 쓰기가 파일에 동기화
  - `MAP_PRIVATE*`: 쓰기는 복사본에서만 발생하며, 원본 파일은 수정 X
  - `MAP_FIXED`: 결과 주소와 주어진 인수 주소가 동일해야 함, `MAP_FIXED`가 아닌 경우 주소 인수는 참조용임
  - `MAP_NORESERVE`: 스왑 공간 보존을 하지 않음

## Release a memory mapping

```c
#include <sys/types.h>
#include <sys/mman.h>
// 0 if OK, -1 on an error
int munmap(
    caddr_t addr, // 메모리 시작 주소
    size_t len
);
```

#### [EX] mmcp.c (File Copy using mmap file)

```c
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#define FILE_MODE (S_IRUSR|S_IWUSR)

void mperr(char* call, int val){ perror(call); exit(val); }

int main(int argc, char *argv[]){
    int fdin, fdout;
    caddr_t src, dst;
    struct stat statbuf;

    if(argc!=3){ mperr("usage: a.out <fromfile> <tofile>", 1); }
    // src
    if((fdin=open(argv[1], O_RDONLY))<0){
        fprintf(stderr, "cannot open %s for writing", argv[1]);
        exit(2);
    }
    // dst
    if((fdout=open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE))<0){
        fprintf(stderr, "cannot create %s for writing", argv[2]);
        exit(3);
    }

    if(fstat(fdin, &statbuf)<0){ mperr("fstat error", 4); }
    if(lseek(fdout, statbuf.st_size-1, SEEK_SET)==-1){ mperr("lseek error", 5); }
    if(write(fdout, "", 1)!=1){ mperr("write error", 6); }

    if((src=mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0))==MAP_FAILED){ mperr("mmap error for input", 7); }
    if((dst=mmap(0, statbuf.st_size, PROT_WRITE, MAP_SHARED, fdout, 0))==MAP_FAILED){ mperr("mmap error for output", 8); }
    memcpy(dst, src, statbuf.st_size);

    if(munmap(src, statbuf.st_size)!=0){ mperr("munmap(src) error", 9); }
    if(munmap(dst, statbuf.st_size)!=0){ mperr("munmap(src) error", 10); }

    exit(0);
}
```

## Disk Synchronization of a MM Area

```c
#include <sys/mman.h>
#include <unistd.h>
// page cache에 기록된걸 disk로 즉시 보내기
int msync(const void* addr, size_t len, int flags);
```

### flags
- `MY_ASYNC`: sync 후 바로 복귀, 확인 과정이 없음
- `MY_SYNC`: 다 쓰였는지 확인하고 복귀
- `MS_INVALIDATE`: file에 있는 mmap을 모두 초기화

#### [EX] mysync-ex.c

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#define MEM_SIZE 64

int main(int argc, char** argv){
    int fd;
    char* memPtr = NULL;
    struct stat sb;
    int flag = PROT_WRITE | PROT_READ;

    if(argc!=2){ fprintf(stderr, "Usage: %s memPtr\n", argv[0]); exit(1); }
    if((fd=open(argv[1], O_RDWR|O_CREAT))<0){ perror("File Open Error"); exit(1); }
    if(fstat(fd, &sb)<0){ perror("fstat error"); exit(1); }

    memPtr = (char*)mmap(0, MEM_SIZE, flag, MAP_SHARED, fd, 0);
    if(memPtr==(void*)-1){
        perror("mmap() error");
        close(fd);
        exit(1);
    }
    printf("mem(%p), value(%s)\n", memPtr, memPtr);
    // mem <-> file (sync)
    while(1){
        scanf("%s", memPtr);
        if(!strcmp(memPtr, "exit")) break;
        if(msync(memPtr, MEM_SIZE, MS_SYNC)==-1){
            printf("msync() error(%s)\n", strerror(errno));
            break;
        }
    }
    if(munmap(memPtr, MEM_SIZE)==-1){
        printf("munmap() error(%s)\n", strerror(errno));
    }
    close(fd);
}
```

# 10. POSIX-semaphore
## Semaphore
- 자원을 공유하는 방법
- Used to control the critical section
- Basic features
  - A block/wakeup algorithm for mutual exclusion
  - process can't enter a critical section, the process will be blocked
  - owner process exits a critical section, it wakes the waiting process up (make it to be ready)
  - No waset of CPU time
  - A waiting queue is necessary if several process are waiting for the permissions for entering their critical sections
- Counting Semaphore: 프로세스를 n개로 제한
- Binary Semaphore: 프로세스를 2개로 제한(n=2)

## Mutual Exclusion by Semaphore
- using a binary(mutex와 동일/유사 방식) or counting semaphore

## Resource Allocation by a Counting Semaphore
## Synchronization with a Semaphore
- 동기화를 위한 목적으로도 활용 가능

#### [EX] Producer/Consumer with a Counting Semaphore

```c
while(1){ // producer
    produce an item;
    num_buffer.wait(); // for sync
    mutex.wait();
    buffer[rear] = pdata;
    rear = (rear+1)%n;
    mutex.signal();
    num_data.signal(); // for sync
}

while(1){ // consumer
    num_data.wait(); // for sync
    mutex.wait();
    cdata = buffer[front];
    front = (front+1)%n;
    mutex.signal();
    num_buffer.signal(); // for sync
    process an item;
}
```

## POSIX SEMAPHORES

```c
#include <semaphore.h>
// must be linked with pthread library
int sem_init(sem_t* sem, int pshared, unsigned int value);
// pshared=0 -> Local / !=0 -> 다른 프로세스간

int sem_wait(sem_t* sem);
// atomically decrease the semaphore count

int sem_trywait(sem_t* sem);
// non-blocking version of sem_wait, immediately returns 0 / EAGAIN (count is zero)

int sem_post(sem_t* sem);
// atomically increase the count of the semaphore pointed to by sem
// never blocks -> safely be used in asynchronous signal handlers

int sem_getvalue(sem_t* sem, int* sval);
// stores the current count of the semaphore (by sval)

int sem_destroy(sem_t* sem);
```

#### [EX] semaphore.c

```c
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

int cnt = 0;
static sem_t hsem;

void* Thread1(void* args);
void* Thread2(void* args);

int main(int argc, char* argv[]){
    pthread_t thread1;
    pthread_t thread2;

    if(sem_init(&hsem, 0, 1)<0){
        fprintf(stderr, "Semaphore Initilization Error\n");
        return 1;
    }
    pthread_create(&thread1, NULL, Thread1, NULL);
    pthread_create(&thread2, NULL, Thread2, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    printf("%d\n", cnt);
    sem_destroy(&hsem);
    return 0;
}

void* Thread1(void* arg){
    int i, tmp;
    for(i=0 ; i<1000 ; i++){
        sem_wait(&hsem);
        tmp = cnt;
        usleep(1000);
        cnt = tmp + 1;
        sem_post(&hsem);
    }
    printf("Thread1 End\n");
    return NULL;
}

void* Thread2(void* arg){
    int i, tmp;
    for(i=0 ; i<1000 ; i++){
        sem_wait(&hsem);
        tmp = cnt;
        usleep(1000);
        cnt = tmp + 1;
        sem_post(&hsem);
    }
    printf("Thread2 End\n");
    return NULL;
}
```

#### [EX] prod-cons.c

```c
...
#define MAX_BSIZE 10
int cnt = 0;
static sem_t hsem, num_buff, num_data;

void* Producer(void* arg){
    int i, tmp;
    for(;;){
        sem_wait(&num_buff);
        sem_wait(&hsem);
        cnt++;
        printf("prod cnt: %d\n", cnt);
        sleep(1);
        sem_post(&hsem);
        sem_post(&num_data);
    }
    printf("Producer Ends\n");
    return NULL;
}

void* Consumer(void* arg){
    int i, tmp;
    for(;;){
        sem_wait(&num_data);
        sem_wait(&hsem);
        cnt--;
        printf("cons cnt: %d\n", cnt);
        sleep(1);
        sem_post(&hsem);
        sem_post(&num_buff);
    }
    printf("Consumer Ends\n");
    return NULL;
}

int main(int argc, char* argv[]){
    pthread_t thread1;
    pthread_t thread2;

    if(sem_init(&hsem, 0, 1)<0){ fprintf(stderr, "Semaphore Init Error\n"); return 1; }
    if(sem_init(&num_buff, 0, MAX_BSIZE)<0){ fprintf(stderr, "Semaphore Init Error\n"); return 1; }
    if(sem_init(&num_data, 0, 0)<0){ fprintf(stderr, "Semaphore Init Error\n"); return 1; }

    pthread_create(&thread1, NULL, Producer, NULL);
    pthread_create(&thread2, NULL, Consumer, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    printf("%d\n", cnt);
    sem_destroy(&hsem);
    return 0;
}
```

# 11. Signal-Timer
## Signal
- A user process can handle an asynchronous urgent event like an interrupt handling by using signals
- User's view
  - user mode running -> signal occurs -> user program interrupted -> signal handler -> exit or return to interrupred user program
- Kernel's view
  - event occurs -> mark that the signal(SIGSEGMENT) happened to the relevant process's signal table in PCB -> signal handler (modify the process's stack) -> exit or return to user mode program (can be some delay)
- signal delivery
  - kernel -> usr, usr -> usr

## Signal Handiling
- 시그널이 프로세스에 전달되면, mark it to the pending signal table of the PCB
- 전달된 신호는, 프로세스가 커널에서 실행되면 사용자 모드로 전환되고, 사용자 모드로 전환되기 전에 핸들러에 의해 처리됨
- Pending Signal: 처리되지 않은 신호
- Signal Blocking: 사용자가 일시적으로 차단한 시그널

## Three Ways of Signal Handling
1. Use a defalut signal handler (`SIG_DFL`)
   1. 일반적
   2. 커널에 의해 제공
   3. do exit or core-dump & exit
2. Ignore a signal (`SIG_IGN`)
   1. `SIGKILL` and `SIGSTOP` cannot be ignored
3. Use a user defined signal handler
   1. Process can register it's own signal handler

## Signals
- `SIGABRT`: 프로그램 중단 / 6 / core-dump & exit
- `SIGALRM`: Timer alarm / 14 / exit
- SIGBUS: Bus err / 7 / core-dump & exit
- `SIGCHLD`: 하위 프로세스 종료 / 17 / Ignore
- `SIGCONT`: 종료된 프로세스의 지속 / 18 / Restart|Ignore
- SIGEMT: 에뮬레이션 트랩 / - / core-dump & exit
- SIGFPE: 계산 예외 / 8 / core-dump & exit
- SIGHUP: TTY 연결 해제 / 1 / exit
- `SIGILL`: 지침 위반(데이터 섹션 이동) / 4 / core-dump & exit
- `SIGINT`: ^C / 2 / exit
- SIGIO: 비동기 I/O / 29 / exit
- SIGIOT: 하드웨어 결함 / 6 / core-dump & exit
- `SIGKILL`: kill(9) / 9 / exit
- `SIGPIPE`: broken pipe / 13 / exit
- SIGPOLL: pollable(투표 가능한) I/O 발생 / 29 / exit
- SIGPROF: 프로파일링 타이머 만료 / 27 / exit
- SIGPWR: 전원 문제 / 30 / ignore
- SIGQUIT: ^Z, quit / 3 / core-dump & exit
- `SIGSEGV`: 비정상 메모리 접근(ptr, 커널 or 다른 프로세스 영역, read-only 영역) / 11 / core-dump & exit
- `SIGSTOP`: stop / 19 / stop
- SIGSYS: 비정상 시스템 콜 / 31 / core-dump & exit
- SIGTERM: kill(15):safe process termination / 15 / exit
- SIGTRAP: Trace/Breakpoint Trap / 5 / core-dump & exit
- SIGSTP: ^Z / 20 / stop
- SIGTTIN / 백그라운드에서 읽기 시도 / 21 / stop
- SIGTTOU / 백그라운드에서 쓰기 시도 / 22 / stop
- SIGURG / 긴급 소켓 이벤트 / 23 / ignore
- `SIGUSR1` or `SIGUSR2`: User defined signal 1/2 / 10/12 / exit
- SIGVTAALRM / 가상 타이머 알람 / 26 / exit
- SIGWINCH / size change in a tty window / 28 / ignore
- SIGXCPU / CPU 제한 시간 초과 / 24 / core-dump & exit
- SIGXFSZ / 파일 용량 초과 / 25 / core-dump & exit

```c
#include <signal.h>
// set a user-definedd signal handler
typedef void(*sighandler_t)(int); // void func ptr
sighandler_t signal(int signum, sighandler_t handler);
```

- return: old signal handler's addr / `SIG_ERR`
- 시그널 핸들러는 한번에 하나만 실행 가능
  - after the first signal reception, reset to `SIG_DFL`
  - some OS versions maintain the user defined handler

#### [EX] sighandler.c

```c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static void sigcatcher(int);
void(*was)(int);

int main(void){
    if(was=signal(SIGINT, sigcatcher)==SIG_ERR){ perror("SIGINT"); exit(1); }

    while(1) pause();
}

static void sigcatcher(int signo){
    switch(signo){
        case SIGINT:
            printf("PID %d caught signal SIGINT.\n", getpid());
            signal(SIGINT, was);
            break;
        default:
            fprintf(stderr, "something wrong\n");
            exit(1);
    }
}
```

## kill(): sending a signal

```c
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);
// pid>0: to the process with pid
// pid=0: to all process in my group
// pid=-1: pid=1을 제외한 모든 프로세스
```

#### [EX] kill() usage

```c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    int pid;
    if((pid=fork())==0){
        // child
        while(1);
    }
    else{
        // parent
        kill(pid, SIGKILL);
        printf("send a signal to the child\n");
        wait();
        printf("death of child\n");
    }
}
```

## raise(): send a signal to itself

```c
#include <signal.h>

int raise(int sig);
```

## alarm(): set an alarm clock

```c
#include <unistd.h>

unsigned alarm(unsigned sec);
```

- 리눅스는, 프로세스별로 하나의 알람만 허용 (새로운 알람이 이전 알람을 대체)
- `sleep` call also use the `SIGALRM`

#### [EX] alarm() usage

```c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void sig_catcher(int);
int alarmed = 0;

int main(){
    int pid;
    signal(SIGALRM, sig_catcher);
    alarm(3);
    // do something
    while(alarmed==0);
    printf("after alarm in main\n");
}

void sig_catcher(int signo){
    alarmed = 1;
    alarm(0);
}
```

## abort()

```c
#include <stdlib.h>

void abort(void);
```

## pause()

```c
#include <unistd.h>

int pause(void);
```

- return:
  - default handler: process exit
  - user defined handler: return after the handler is invoked

## sigprocmask

```c
#include <signal.h>
// set a signal set mask (for blocking or unblocking a signal)
int sigprocmask(
    int how, // block or unblock
    const sigset_t* set, // signal set
    sigsset_t* oset, // calling process's old signal set mask
);
```

- how
  - `SIG_BLOCK`: Add, for blocking
  - `SIG_UNBLOCK`: Remove, for nonblocking
  - `SIG_SETMASK`: Replace, for blocking
- set

```c
// for initializing of a set
int sigemptyset(sigset_t* set);
// set all signal set mask
int sigfillset(sigset_t* set);
// add a signal to a signal mask set
int sigaddset(sigset_t* set, int signo);
// del a signal from a signal mask set
int sigdelset(sigset_t* set, int signo);
// ask if a signal is in a signal mask yet
int sigismember(sigset_t* set, int signo);
```

## sigpending()

```c
#include <signal.h>

int sigpending(sigset_t* set);
```

## sigsuspend()

```c
#include <signal.h>
// 마스크 설정과 중지 동시 설정
int sigsuspend(const sigset_t* set);
```

- 해당 신호 반환시,
  - 핸들러 적용 후 반환
  - 마스크가 호출 전 마스크로 복원
  - 프로세스 종료시 반환되지 않음

#### [EX] sigsuspend.c

```c
int main(){
    int pid;
    sigset_t mysigset, oldsigset;
    // empty set
    sigemptyset(&mysigset);
    // add SIGUSR1 to the set
    sigaddset(&mysigset, SUGUSR1);
    // set a user-defined signal handler
    signal(SIGUSR1, sig_catch);
    // BLOCK SIGUSR1
    sigprocmask(SIG_BLOCK, &mysigset, oldsigset);

    if((pid=fork())==0){
        // child, to prevent the early handling before pause()
        sigsuspend(&oldsigset); // unblock and pupase
        printf("Chlid wake up\n");
        exit(0);
    }
    else{
        // parent
        sleep(1);
        kill(pid, SIGUSR1); // send the SIGUSR1 to the child
        wait();
    }
}
```

## sleep()

```c
#include <unistd.h>

unsigned int sleep(unsigned int seconds);
// nanosleep(nano-sec); usleep(micro-sec);
```

## System call and a signal
- 시스템이 호출할 때 신호가 전달되는 경우 수행되는가?
  - 시스템 호출 I/O로 인해, 프로세스가 차단되면 신호가 전달됨
  - I/O 종료 후 핸들링 (`TASK_UNINTURRUPTIBLE`)
  - I/O 중단 후 핸들링 (`TASK_INTURRUPTIBLE`)
    - 이어서 수행 or 종료 후 에러 발생
  - `siginterrupt (signal_no, TRUE/FALSE)`를 통해 설정 (TRUE=종료 후 처리)

#### [EX] alarm-getchar.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define TIMEOUT 3
#define MAXTRIES 5
#define LINESIZE 100
#define CTRL_G '\007' // bell
#define TRUE 1
#define FALSE 0

volatile int timed_out; // set when an alarm occurs
char myline[LINESIZE]; // char buffer
void sig_catch(int);

char* quickreply(char* prompt){
    void(*was)(int);
    int ntries, i;
    char* answer;

    was = signal(SIGALRM, sig_catch);
    siginterrupt(SIGALRM, TRUE);

    // set err return when a signal occurs
    for(ntries=0 ; ntries<MAXTRIES ; ntries++){
        timed_out = FALSE
        printf("\n%s>", prompt);
        fflush(stdout);

        alarm(TIMEOUT);
        for(i=0 ; i<LINESIZE ; i++){
            if((myline[i]=getchar())<0) break;
            if(myline[i]=='\n'){
                myline[i] = 0;
                break;
            }
        }
        // normal case or alarm case here
        alarm(0); // reset
        if(!timed_out){
            // normal case
            printf("%s", myline);
            break;
        }
    }
    answer = myline;
    signal(SIGALRM, was);
    return(ntries==MAXTRIES?((char*)0):answer);
}

void sig_catch(int sig_no){
    timed_out = TRUE;
    putchar(CTRL_G);
    fflush(stdout);
    // reinstall the user defined signal handler
    signal(SIGALRM, sig_catch);
}
```

## Interval Timer

```c
#include <sys/time.h>
// generates a SIGALRM signal periodically
int setitimer(
    int which, // timer type
    const struct itimerval* value, // new interval
    struct itimerval* oval // current(old) timer interval (maybe NULL)
);
int getitimer(
    int which,
    struct itimerval* oval
)
```

- which
  - `ITIMER_REAL`: Time in real: `SIGALRM`
  - `ITIMER_VIRUAL`: Time in user mode: `SIGVALRM`
  - `ITIMER_PROF`: Process running time(user mode + kernel mode): `SIGPROF`

## itimerval structure

```c
struct itimerval{
    struct timeval it_interval; // periodic interval after 1st alarm, =0 -> for one time itimer
    struct timeval it_value; // first interval / =0 -> turn off the itimer
}

struct timeval{
    long tv_sec; // seconds
    long tv_usec; // micro seconds
}
```

#### [EX] itimer.c

```c
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void alarm_handler(int signo){
    printf("Timer hit\n");
    // do the periodic job here
}

int main(){
    struct itimerval delay;
    int ret;
    signal(SIGALRM, alarm_handler);
    delay.it_value.tv_sec = 5; // first alarm
    delay.it_value.tv_usec = 0;
    delay.it_interval.tv_sec = 1; // periodic
    delay.it_interval.tv_usec = 0;
    ret = setitimer(ITIMER_REAL, &delay, NULL);
    if(ret){
        perror("setitimer");
        exit(0);
    }
    while(1){ pause(); }
}#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void alarm_handler(int signo){
    printf("Timer hit\n");
    // do the periodic job here
}

int main(){
    struct itimerval delay;
    int ret;
    signal(SIGALRM, alarm_handler);
    delay.it_value.tv_sec = 5; // first alarm
    delay.it_value.tv_usec = 0;
    delay.it_interval.tv_sec = 1; // periodic
    delay.it_interval.tv_usec = 0;
    ret = setitimer(ITIMER_REAL, &delay, NULL);
    if(ret){
        perror("setitimer");
        exit(0);
    }
    while(1){ pause(); }
}
```

## POSIX Timer (Advanced)
- POSIX Timer is a more advanced and controllable timer for realtime applications
- `SIGALRM` 외의 다른 시그널잉 사용될 수 있음 (`SIGTMIN` ~ `SIGRTMAX`)
- signal hander 대신, signal action으로 등록
- 컴파일시, `-lrt` 링킹

```c
#include <sys/time.h>

int time_create(
    clockid_t clockid,
    struct sigevent* restrict evp, // timer event
    timer_t* restrict timerid // timer id for the created timer
    );
```

- clockid
  - `CLOCK_REALTIME`: A settable system-wide real-time clock
  - `CLOCK_MONOTONIC`: A nonsettable monotonically increasing clock

```c
#include <time.h>

int timer_settime(timer_t timerid, int flags, const struct itimerspec* new_vale, struct itimerspec* old_value);
int timer_gettime(timer_t timerid, struct itimerspec* curr_value);
```

## itimerspec struct

```c
struct timespec{
    time_t tv_sec; // seconds
    long tv_nsec; // nano-seconds
};

struct itimerspec{
    struct timespec it_interval; // timer interval
    struct timespec it_value; // initial expiration
}
```

## sigaction()

```c
#include <signal.h>

int sigaction(
    int signo,
    const struct sigaction* act,
    struct sigaction* oldact
);

struct sigaction{
    void(*sa_hanlder)(int); // addr of handler
    sigset_t sa_mask;
    int sa_flags; // set for MT and RT purpose
    void(*sa_restorer)(void); // set for MT and RT purpose
};
```

#### [EX] posix-time.c

```c
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#define SIGTIMER (SIGRTMAX)
#define ONESHOTTIMER (SIGRTMAX-1)

timer_t SetTimer(int signo, int sec, int mode);
void SignalHandler(int signo, siginfo_t* info, void* context);
timer_t timerid, oneshotTimer;

int main(){
    struct sigaction sigact;
    sigempty(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = SignalHandler;

    // Set up sigaction to catch signal
    if(sigaction(SIGTIMER, &sigact, NULL)==-1){ perror("sigaction failed"); return -1; }
    if(sigaction(ONESHOTTIMER, &sigact, NULL)==-1){ perror("sigaction failed"); return -1; }

    // Establish a handler to catch CTRL+C and use it for exiting
    sigaction(SIGINT, &sigact, NULL);

    timerid = SetTimer(SIGTIMER, 1000, 1);
    oneshotTimer = SetTimer(ONESHOTTIMER, 5000, 0);
    while(1);
    return 0;
}

timer_t SetTimer(int signo, int sec, int mode){
    struct sigevent sigev;
    timer_t timerid;
    struct itimerspec itval;
    struct itimerspec oitval;
    // create the POSIX timer to gen. signo
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = signo;
    sigev.sigev_value.sival_ptr = &timerid;

    if(timer_create(CLOCK_REALTIME, &sigev, &timerid)==0){
        itval.it_value.tv_sec = sec/1000;
        itval.it_value.tv_nsec = (long)(sec%1000)*(1000000L);
        if(mode==1){ // periodic timer
            itval.it_interval.tv_sec = itval.it_value.tv_sec;
            itval.it_interval.tv_nsec = itval.it_value.tv_nsec;
        }
        else{
            // one shot timer
            itval.it_interval.tv_sec = 0;
            itval.it_interval.tv_nsec = 0;
        }
        if(timer_settime(timerid, 0, &itval, &oitval)!=0){
            perror("time_settime error!");
            return (timer_t)-1;
        }
        else{
            printf("timer_create(%d) create!: Success", timerid);
            return timerid;
        }
    }
}

SignalHandler(int signo, siginfo_t* info, void* context){
    if(signo==SIGTIMER){
        puts("Periodic timer");
    }
    else if(signo==ONESHOTTIMER){
        puts("One-Short timer");
    }
    else if(signo==SIGINT){
        timer_delete(oneshotTimer);
        timer_delete(timerid);
        printf("Ctrl + C catched!\n");
        exit(1);
    }
}

```

# 12. SystemV-IPC
## IPC methods
- Pipe: related process
- FIFO
- Message Queues
- Shared Memory

## Review on pipe
- oldest from of IPC
- Data transmitting
  - data is written into pipes using the write() system call
  - data is read from a pipe using the read() system call
  - automatic blocking when full or empty
- Limitations of pipes:
  - half duplex (data flows in one direction)
  - 같은 조상을 갖는 프로세스간에서만 사용 가능
  - process cannot pass pipes and must inherit them from their parent
  - if a process creates a pipe, all its children will inhrit it

## FIFO (named pipe)
- can be used between unrelated process

## mkfifo (매개체=file)

```c
#include <sys/type.h>
#include <sys/stat.h>

int mkfifo(
    const char* pathname,
    mode_t mode // permisson, same as for file open() func
);
```

- FIFO를 사용하는 것은 파일을 사용하는 것과 유사

#### [EX] Fifo send-recv example

```c
// fifo-recv.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define SIZE 128
#define FIFO "fifo"

int main(int argc, char* argv[]){
    int fd;
    char buffer[SIZE];

    if(mkfifo(FIFO, 0666)==-1){
        perror("mkfifo failed");
        exit(1);
    }
    if((fd=open(FIFO, O_RDWR))==-1){
        perror("open failed");
        exit(1);
    }

    while(1){
        if(read(fd, buffer, SIZE)==-1){
            perror("read failed");
            exit(1);
        }
        if(!strcmp(buffer, "quit")){ exit(0); }
        printf("receive message: %s\n", buffer);
    }
}

// fifo-send.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define SIZE 128
#define FIFO "fifo"

int main(int argc, char* argv){
    int fd, i;
    char buffer[SIZE];

    if((fd=open(FIFO, O_WRONLY))==-1){
        perror("open failed");
        exit(1);
    }
    for(i=1 ; i<argc ; i++){
        strcpy(buffer, argv[i]);

        if(write(fd, buffer, SIZE)==-1){
            perror("write failed");
            exit(1);
        }
    }
    exit(0);
}
```

## IPC methods of System V
- Message Queues
  - send and receive amount of data called "messages"
  - the sender classifies each message with a type
- Shared  Memory
  - shared memory allows two or more processes to share a given region of memory
  - readers and writers may use semaphore for synchronization

## Identifiers & Keys
- Identifier: each IPC structure has a nonnegative integer
- Key: when creating an IPC structure, a key must be specified (`key_t`)
- How to access the same IPC object?
  - Define a key in a common header (by a programmer)
  - Client and server agree to use that key
  - Server creates a new IPC structure using that key
  - Proble when the key is already in use
    - solution: del existing key, create new one again

## IPC system calls
- msg/shm get
- msg/shm ctl
- msg/shm op

## Permission Structure

```c
struct ipc_perm{
    uid_t uid; // owner's effective user id
    gid_t gid; // owner's effective group id
    uid_t cuid; // creator's effective user id
    gid_t cgid; // creator's effective group id
    mode_t mode; // access modes
    ulong seq; // slot usage sequence number
    key_y key; // key
}
```

## Message Queue
- Linked list of messages
  - stored in kernel
  - identified by message queue identifier (in kernel)
- msgget
  - create new que or open existing que
- msgsnd
  - add new message to a que
- msgrcv
  - receive a message from a que
  - message fetching order: based on a specified type

```c
// get the structure using msgctl()
// 자세히 알 필요는 없음
struct msqid_ds{
    struct ipc_perm msg_perm;
    struct msg* msg_first; // ptr to first msg on que
    struct msg* msg_last; // ptr to last msg on que
    ulong msg_cbytes; // current # bytes on que
    ulong msg_qnum; // # msgs on que
    ulong msg_qbytes; // max # bytes on que
    pid_t msg_lspid; // pid of last msgsnd()
    pid_t msg_lrpid; // pid of last msgrcv()
    time_t msg_stime; // last-msgsnd() time
    time_t msg_rtime; // last-msgrcv() time
    time_t msg_ctime; // last-change time
}

// message structure
struct mymesg{
    long mtypes; // positive message type
    char mtext[512]; // message data, of length nbytes
}
```

## Message Queue Parameters
- Each message queue is limited in terms of both
- New messages cannot be added if either limit is hit (new writes will normally block)
- `/usr/include/linux/msg.h`
- `sysctl`로 변경 가능 (`/etc/sysctl.conf`)

| name | description | defaults |
| :--- | :--- | :---: |
| MSGMNB | Max bytes in a que | 16,384 |
| MSGMNI | Max # of message que identifiers | 32,000 |
| MSGMAX | Max size of message (bytes) | 8,192 |

## msgget

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int msgget(key_t key, int flag);
```

## msgsnd

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int msgsnd(int msqud, const void* ptr, size_t nbytes, int flag);
// flag = 0 for blocking (default)
//        IPC_NOWAIT for nonblocking I/O
```

## msgrcv

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int msgrcv(int msqid, void* ptr, size_t nbytes, long msgtypes, int flag);
```

- msgtypes
  - ==0: the first message on the que is returned
  - \>0: the first message ont the que whose message type equals to the msgtype is returned
  - <0: the first message on the que whose message type is the lowest value (less than or equal to abs value of the msgtype) is returned

## msgctl

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
// performs various operation on a queue management
int msgctl(int msqid, int cmd, struct msqid_ds* buf);
```

- cmd (que control)
  - `IPC_STAT`: cpy the msg que descriptor structure -> user buff
  - `IPC_SET`: set the system msg que desciptor structure as give by the usr buff
  - `IPC_RMID`: remove the msg que, and wake up all the sender and receiver processes, when resume, error-returned(errno=EIDRM)
- buf (msqid_ds structure ptr)
  - `IPC_SET` or `IPC_STAT`

## ipcs command
- `ipc`: System V IPC resource 상태 확인
- `ipcrm`: 생성된 IPC resource 제거
- options
  - `-q`: Message Que
  - `-m`: Shared Memory
  - `-s`: Semaphore

## Shared Memory
- 프로세스는, 다른 프로세스가 접근할수 없는 공유 주소 공간이 있음 (proceess protection)
- 공유 메모리는, OS support memory(addr space)로 다른 프로세스와 공유 가능
- Allows multiple processes to share a region of memory
- fastest from of IPC
- attached (==mapping)

## shmget

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int shmget(key_t key, int size, int flag);
```

## shmat

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
// attach (map) a shared memory segment to the addr space of calling process
void* shmat(int shmid, void* addr, int flag);
// addr: 0(recommended) -> use the addr selected by kernel
//                         non-zeroe -> given addr
// flag: 0(default) / SHM_RDONLY
```

## shmdt

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
// detach(unmap) a shared memor segment from calling process
void shmdt(void* addr);
```

## shmctl

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
// performs various shared memory management operations
int shmctl(int shmid, int cmd, struct shmid_ds* buf);
// cmd: IPC_SET / IPC_STAT / IPC_RMID
```

#### [EX] prod-cons.c

```c
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

// prod
int key_pressed = 0;

static void sig_handler(int signo){
    key_pressed++;
    printf("continue to the next stage...\n");
    if(key_pressed==1 && signo==SIGINT){
        signal(SIGINT, sig_handler);
    }else if(signo==SIGINT){
        signal(SIGINT, SIG_DFL);
    }
}

int main(void){
    int shmid;
    size_t shsize = 1024;
    const int key = 16000;
    char* shm;
    sem_t* mysem;
    int i;

    siganl(SIGINT, sig_handler);
    sem_unlink("mysem"); // remove old semahpore if any
    if((mysem=sem_open("mysem", O_EXCL|O_CREAT, 0777, 1))==SEM_FAILED){
        perror("Sem Open Error");
        exit(1);
    }
    if((shmid=shmget((size_t)key, shsize, IPC_CREAT|0666))<0){
        perror("shmget");
        exit(1);
    }
    if((shm=(char*)shmat(shmid, NULL, 0))==(char*)-1){
        perror("shmat");
        exit(1);
    }
    for(i=0 ; i<10 ; i++){
        shm[i] = 0;
    }
    // write data to the shared memory
    while(!key_pressed){
        for(i=0 ; i<10 ; i++){
            shm[i]=(shm[i]+1)%10;
        }
        sem_post(mysem);
        sleep(1);
    }
    // read data from the shared memory
    while(key_pressed==1){
        for(i=0 ; i<10 ; i++){
            sem_wait(mysem);
            for(int i=0 ; i<10 ; i++){
                printf("%c", (char)shm[i]);
            }
            sem_post(mysem);
            sleep(1);
            printf("\n\n");
            fflush(stdout);
        }
    }
    sem_close(mysem);
    sem_unlink("mysem");

    shmdt(shm);
    shmctl(shmid, IPC_RMID, 0);

    return 0;
}

// cons
// key_press, sig_handler 동일
int main(void){
    int shmid;
    size_t shsize = 1024;
    const int key = 16000;
    char* shm;
    char c;
    int i;
    sem_t* mysem;

    signal(SIGINT, sig_handler);
    if((mysem=sem_open("mysem", 0, 0777, 0))=SEM_FAILED){
        perror("Sem Open Error");
        exit(1);
    }
    if((shmid=shmget((key_t)key, shsize, IPC_CREAT|0666))<0){
        perror("shmget");
        exit(1);
    }
    if((shm=(char*)shmat(shmid, NULL, 0))==(char*)-1){
        perror("shmat");
        exit(1);
    }
    while(!key_pressed){
        for(i=0 ; i<10 ; i++){
            sem_wait(mysem);
            for(int i=0 ; i<10 ; i++){
                printf("%d", (shm[i]));
            }
            putchar('\n');
            sem_post(mysem);
            sleep(1);
        }
    }
    while(key_pressed==1){
        c = 'A';
        for(i=0 ; i<10 ; i++){
            sem_wait(mysem);
            for(int i=0 ; i<10 ; i++){
                shm[i] = c;
            }
            sem_post(mysem);
            sleep(1);

            if(++c>'Z') c='A';
        }
    }
    sem_close(mysem);
    shmdt(shm);

    return 0;
}
```

# 13. Network
## Protocol
- A pre-defined communication step for error-free communications on an erroneous data link
- Usually a protocol is implemented in a multiple-layerd architecture
  - too big
  - various level of abstraction
  - various level of service
  - variable media & communications types
- OSI 7 layers
  - HW
    - Physical layer: electrical signaling system
    - Data  link layer: error free communications between adjacent nodes (MAC, Point-to-Point)
  - SW
    - Network layer: Routing, IP in the Internet Protocol
    - Transport layer: TCP/UDP
    - Session layer
    - Presentation layer
    - Application layer: ftp, email, ...

## Network Protocols for Communications
- A message from a user process can be split into multiple segemts in each protocol layer (Fragmentation)
- Messages from a user can be merged into a segment in each protocol layer
- Each layer attaches a layer's packet header to the segment
  - TCP segment: TCP header + Data segment
  - IP: IP header + TCP segment + CRC checksum
  - Each layer has its MTU(Maximum Transfer Unit)

## Protocol Examples
- OSI7을 모두 구현 X -> 유사한 것끼리 묶어 구현해둠
- Application Layer: Telnet, ftp, SNMP, etc
- Transport Layer: TCP, UDP, etc
- Network Layer: IP, ICMP(ping), IGMP(router), etc
- Data Link Layer: Network device drivers, Interface/controller cards, etc
- ARP(Addr Resoultion Protocol): IP to a physical MAC addr
- RARP(Reverse Address Resoultion Protocol): Physical MAC addr to an IP

## App. Layers Services on Transport Protocols
- TCP
  - FTP
  - TELNET
  - SMTP
  - HTTP
- UDP
  - SNMP (Simple Network Management Protocol)
  - TFTP (Trivaul FTP)

## Types of network applications service
- Client/Server: 클라이언트 프로그램 <-> 서버 프로그램
  - IP addr, Prot #
- Web-based: 웹브라우저 <-> 웹 서비스
- Peer-to-Peer(P2P): node can be a server or can be a client

## Connection-oriented vs. Connectionless Communications
- Connection-oriented
  - TCP protocol
  - Reliable data transfer is guarantedd
  - Must set up a link to each client (-> overhead)
- Connectionleess
  - UDP protocol
  - Suitable for a single message transmission
  - No link for clients: low pressure to the server

## Server Types
- Repetitive server type(single-threaded server)
  - 한개의 서버 프로세스가 다수의 클라이언트 요청을 받음
  - 느린 응답
- Concurrent server type(multi-threaded server)
  - 각각의 클라이언트가 각각의 서버와 통신
  - 높은 성능
  - Concurrency problems

## Socket Communication
- TCP (connection-oriented)
  - Binding: assign (IP address, port #) to socket
  - Every transmission of packets use the same link
  - Stream I/O, reliable, flow control, error control
- UDP (connectionless)
  - For each transmission of message, IP addr & port# of the target are necessary
  - Useful for one-time small message transmission
  - Message-based, unreliable (a message can be lost), order of message delivery can be reversed
  - Message size must be smaller than the UDP packet size
  - No flow control, restricted error control, low overhead

## Ports
- A network application process uses a port in the local host
- Port # usage (0 ~ 65535), IANA allocation
  - Well-known ports(0 ~ 1023): 이미 지정
  - Registered ports (1024 ~ 49151): 유저나 IANA에 의해 지정
  - Dynamic ports(49152 ~ 65535): free use

## Socket Address Structure

```c
#include <sys/socket.h>
// 모든 타입 네트워크에 사용 가능
struct sockaddr{
    u_char sa_len; // addr struct length
    u_char sa_family; // addr type
    char sa_data[14]; // 14 byte-addr
}

#include <netinet/in.h>
#include <sys/types.h>
// 인터넷용
struct sockaddr_in{
    u_char sin_len;
    u_char sin_family;
    u_short sin_port; // 16 bit prot #
    struct in_addr sin_addr; // 32 bit IP addr
    char sin_zero[8]; // not used
}

struct in_addr{
    u_long s_addr; // 32 bit IP addr
}
```

- sin_family
  - `AF_INET`: internet IP addr
  - `AF_UNIX`: UNIX or used for local communications
  - `AF_NS`: XEROX networks addr

## socket()

```c
#include <sys/sockey.h>
#include <sys/types.h>

int socket(int domain, int type, int protocol);
```

- domain (addr type -> protocol)
  - PF_INET: internet protocol
  - PF_INET6: IPv6 protocol
  - PF_UNIX
  - PF_NS
  - PF_IMPLINK
- type (socket type)
  - `SOCK_DGRAM`: IPPROTO_UDP
  - `SOCK_STREAM`: IPPROTO_TCP
  - SOCK_RAW: IPPROTO_ICMP (usr define)
  - SOCK_RAW: IPPROTO_RAW (usr define)

## bind()

```c
#include <sys/socket.h>
#include <sys/types.h>
// connect my host IP addr & port # to my socket
int bind(int sockfd, struct sockaddr* myaddr, int addrlen);
```

## connect()

```c
#include <sys/socket.h>
#include <sys/types.h>
// auto-binding to the socket will be done
int connect(int sockfd, struct sockaddr* servaddr, int addrlen);
```

## listen()

```c
#include <sys/socket.h>
#include <sys/types.h>

int listen(int socket, int queuesize);
```

## accept()

```c
#include <sys/sockey.h>
#include <sys/types.h>
// The old socket will be used for other client-requests for futrer connections
int accept(int sockfd, struct sockaddr* peer, int* addrlen);
```

## send() / recv()

```c
#include <sys/sockey.h>
#include <sys/types.h>
// TCP
int send(int sockfd, char* buf, int bytes, int flag);
// ACK timeout or NACK
// send/recv = stream I/O
int recv(int sockfd, char* buf, int bytes, int flag);
// flag maybe NULL

// UDP
int sendto(int sockfd, char* buf, int bytes, int flag, struct sockaddr* to, int addrlen);
int recvfrom(int sockfd, char* buf, int bytes, int flag, struct sockaddr* from, int* addrlen);
```

## close()

```c
#include <unistd.h>

int close(int sockfd);
```

- flag
  - MSG_OOB: OOB(out of bound) data, used for urgent data sending
  - MSG_PEEK: keep the data in the buffer
  - MSG_DONTROOTE: ignore the usual routing

### INADDR_ANY
- all of IP addr of a multi_home host

## TCP's stram I/O
- TCP does not guarantee to receive a whole message sent
- when the message size is fixed and known

```c
// Method 1
while(1){
    len = recv(sd, data, MAX, MSG_PEEK); // do not remove from TCP buffer
    if(len>=desired_length) break;
}
recv(sd, data, MAX, 0); // read a msg & remove from the TCP buffer

// Method 2
len = 0;
size = sizeof(struct message); // the fixed known message size
while(1){
    p = (char*)message_buf + len;
    len += recv(sd, (void*)p, size-len, 0);
    if(len>=size) break;
}
```

## Data Format Conversion

```c
// convert host long format to the network standard
u_long htonl(u_long hostlong);
// Host to network short
u_short htons(u_short hostshort);
// Network to host long
u_long ntohl(u_long netlong);
// Network to host short
u_short ntohs(u_short netshort);


void bcopy(char* src, char* dst, int bytes);
void bzero(char* dst, int bytes); // clear bytes in dst
int bcmp(char* ogn, char* tgt, int bytes);
```

## Little Endian and Big Endian
- (MSB) [1] [2] [3] [4] (LSB)
- big: (100) [1] [2] (101) [3] [4] // 아래부터 순서대로
- little: (100) [3] [4] (101) [1] [2] // 아래부터 역으로

# 14. Time Managemnet
## Calendar Time & Process Time
- UTC(Coordinated Universal Time) / GMT (Greenwich Mean Time)
- Epoch time: # of seconds from 1970.1.1 00:00 UTC
- Data type: time_t
- Process/CPU time
  - In clock tick unit
  - 1 tick = 1ms or 10ms

## Time syscall

```c
#include <time.h>

time_t time(time_t* tloc); // buff for time_t -> current epoch time
int stime(const time_t* tp); // epoch time to set
```

#### [EX] setTime.c (get/set time)

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]){
    time_t curtime, modtime;
    if(argc!=){
        perror("argument error");
        printf("format: [+|-] sec");
        exit(1);
    }
    modtime = (time_t)time(&curtime);
    printf("current time: %d sec from 1979, 1, 1, 1 00:00\n", (int)curtime);

    if(!strcmp(argv[1], "-")){ modtime=curtime-atoi(argv[2]); }
    else if(!strcmp(argv[1], "+")){ modtime=curtime+atoi(argv[2]); }
    else{
        perror("argument error");
        exit(2);
    }
    if(stime(&modtime)){ perror("stime error"); }
    printf("modified time: %d sec from 1970, 1, 1 00:00\n", (int)modtime);
}
```

## tm structure for Calendar Time

```c
struct tm{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst; // >0 (summer time), =0 (summer time x), <0 (no info)
}
```

## Conversion of Time-format

```c
#include <time.h>
struct tm* localtime(const time_t* clock);
struct tm* gmtime(const time_t* clock);
```

## Human readable -> UTC

```c
#include <time.h>

time_t mktime(struct tm* timeptr);
```

## Time to string

```c
#include <time.h>

char* ctime(const time_t* clock);
char* asctime(const struct tm* tm);
size_t strftime(
    char* s, // string buff
    size_t maxsize, // buff size
    const char* format,
    const struct tm* timeptr
    )
```

- %%: %문자 출력
- %a: 축약된 요일명
- %A: 요일명
- %b: 축약된 월명
- %B: 월명
- %c: 날짜와 시간
- %C: date 명령이 만들어내는 날짜와 시간
- %d: 일 (01~31)
- %D: m/d/y 형태의 날짜
- %e: 일 (1~31)
- %h: 축약된 월명
- %H: 시간 (00~23)
- %I: 시간 (01~12)
- %j: 한해의 날수 (001~366)
- %k: 시간 (0~23)
- %l: 시간 (1~12)
- %m: 월 번호 (01~12)
- %M: 분 (00~59)
- %n: \n과 동일 새로운 행
- %p: AM이나 PM
- %r: I:M:S [AM|PM]
- %R: H:M
- %S: 윤초를 지원하는 초
- %t: 탭문자와 동일
- %T: H:M:S
- %U: 한해의 주 번호, 일요일로 시작하는 주만
- %w: 요일 번호, 일요일=0
- %W: 한해의 주번호, 월요일로 시작하는 주만
- %x: 날짜표시
- %X: 시간표시
- %y: 그 세기의 해표시
- %Y: 년도 표시
- %Z: Time zone

# 족보
## 1. 아래와 같이 message queue를 이용해서 유저의 message queue를 전송하고 message queue를 제거하는 프로그램

```c
#define BUFFER_SIZE 1024

struct msgbuf{
    long msg_type;
    char msg_text[BUFFER_SIZE];
};

int main(void){
    struct msgbuf buf;
    key_t key;
    int msg_queueid;
    key = 1234;

    if((msg_queuid=msgget(key, 0644|IPC_CREAT))==-1){
        fprintf(stderr, "msgget error\n");
        exit(1);
    }

    printf("Enter lines of text, ^D to quit: \n");
    buf.msg_type = 1;
    while(fgets(buf.msg_text, sizeof(buf.msg_text), stdin)!=NULL){
        int length = strlen(buf.msg_text);
        if(buf.msg_text[length-1]=="\n"){ buf.msg_text[length-1] = "\0"; }
        if(msgsnd(msg_queueid, &buf, length+1, 0)==-1){ fprintf(stderr, "msgsnd error"); }
        if(msgctl(msg_queueid, IPC_STAT, NULL)==-1){ fprintf(stderr, "msgctl error"); exit(1); }
    }
    exit(0);
}
```

## 2. 특정 프로세스에게 특정 시그널을 발생시키는 시스템 콜 함수는?
- kill

## 3. MSB가 메모리의 하위 주소에 저장되는 CPU 데이터 저장 방식의 이름은?
- Big Endian

## 4. FIFO를 이용해서 message를 주고 받는 채팅을 수행하는 두 프로세스 프로그램 중 한 쪽이다. FIFO를 두개를 생성하여, receiver thread는 하나의 FIFO로부터 들어오는 데이터를 받고, main thread는 사용자의 입력을 받아서 FIFO로 보낸다.

```c
#define FIFO1 "fifo1"
#define FIFO2 "fifo2"
#define BUFF_SIZE 100

void* receiver(void* value){
    int sfd, cmd, pid;
    char buff[BUFF_SIZE];
    if((sfd=open(FIFO1, O_RDONLY))==-1){
        peintf("[SYSTEM] open error!!\n");
        return NULL;
    }
    while(1){
        memset(buff, 0, BUFF_SIZE);
        if(read(FIFO1, buff, BUFF_SIZE)==-1){ break; }
        printf("\n[RECEIVE] %s", buff);
        fflush(stdout);
        if(strcmp(buff, "quit", 4)==0){ printf("Quit chatting\n"); exit(0); }
    }
    close(sfd);
}

int main(){
    int cfd;
    int pid;
    char buff[BUFF_SIZE];
    char msg[BUFF_SIZE];
    pthread_t t_receiver;
    int quit;

    if((cfd=open(FIFO2, O_RDWR))==-1){
        printf("[SYSTEM] open error!!\n");
        exit(1);
    }
    pid = getpid();
    pthread_create(&t_receiver, NULL, receiver, NULL);
    quit = 0;
    while(quit==0){
        fgets(msg, BUFF_SIZE, stdin);
        printf("[SEND]");
        write(FIFO1, buff, strlen(msg));
        if(strcmp(msg, "quit", 4)==0){
            printf("Quit chatting\n");
            ptrhead_cancel(t_receiver);
            quit = 1;
        }
    }
    close(cfd);
}
```

## 5. pipe를 만든 후에 fork를 수행하여, parent는 1에서 10까지의 int를 하나씩 pipe로 전송하는 loop을 수행하고, chlid는 "output" file을 생성하고, pipe에서 정수를 하나씩 받아 output에 출력하는 프로그램을 완성하시오.

```c
int main(){
    int fd[2], output; // for pipe & outputfile
    int i, n;

    pipe(fd);
    if(fork()!=0){
        // parent
        close(fd[0]);
        for(i=0 ; i<10 ; i++){ write(fd[1], &i, sizeof(int)); }
        close(fd[1]);
        wait();
    }
    else{
        // child
        close(fd[1]);
        output = fopen("output", "w");
        while(read(fd[0], &n, sizeof(int))>0){
            fprintf(output, "%d\n", n);
        }
        fclose(output);
        close(fd[0]);
    }

    return 0;
}
```

## 6. 프로그램이 완성

```c
void signal_handler(int signo);
void (*_func)(int);

int main(void){
    func = siganl(SIGINT, signal_handler);
    while(1){
        printf("process running...\n");
        sleep(1);
    }
    exit(0);
}

void signal_handler(int signo){
    printf("~");
    printf("SIGINT를 SIG_DFL로 재설정 함.\n");
    signal(SIGINT, SIG_DFL);
}
```

## 7. 시그널이 발생했을 때 시그널을 핸들링하는 처리 방법
1. SIG_DFL 핸들러가 실행
2. 무시 가능한 시그널은 무시
3. 유저가 특정 시그널 핸들러를 정의하여 등록해 사용

## 8. server에서 client의 connection request를 기다리다가 client의 연결 요청이 오면 이를 처리하는 함수는?
- accept

## 9. 지역 변수가 할당되는 사용자 주소 공간 이름은?
- stack

## 10. System IPC의 방법?
1. FIFO (named pipe)
2. Message Queue
3. Shared Memory

## 11. Counting Semaphore와 Binary Semaphore의 사용 방법과 사용상의 차이점을 위주로 간략히 설명하시오.
- Counting Seamaphre는 접근 가능한 프로세스의 수를 n개로 제한하고, Binary Semaphore의 경우 특별한 경우로 2개로 제한한다.
- Binary Semaphore의 경우, Mutex와 동기화 용도로 사용 가능

## 12. 프로세스가 IO를 수행하는 도중에 발생한 시그널에 대한 처리를 IO가 끝난 후 처리할 것인지, IO를 멈추고 바로 처리할 것인지를 설정하는 시스템 콜 함수는?
- siginterrupt

## 13. 자기 자신에게 signal을 보내는 함수는?
- raise

## 14. System V IPC의 자원상태를 확인할 수 있는 쉘 명령어는?
- ipc

## 15. program abort가 발생했을 때 발생하는 signal 이름은?
- SIGABRT

## 16. 시스템 epoch time을 현지 calendar time으로 반환하는 시스템 콜 함수는?
- local time

## 17. TCP 타입의 소켓을 설정하기 위해서 사용하는 type 이름은?
- SOCK_STREAM

## 18. 일반적으로 share memory 주소 공간을 할당할 때 사용되는 프로세스 주소 공간의 영역은?
- Data

## 19. 세마포어를 사용할 수 있는 시스템 프로그래밍 예를 세가지 기술
1. 프로세스간의 동기화 시스템
2. 자원을 공유하는 시스템
3. 교착 상태를 방지해야하는 시스템

## 20. UDP를 이용해여, client에서 string msg를 server에게 보내면 server에서는 client로부터 받은 msg를 그대로 다시 전송하는 client-server 프로그램

```c
// clinet.c
#define MAX 1024
#define PROT 30001
#define HOSTADDR "192.168.111.1"

int main(int argc, char* argv[]){
    int sd, send_bytes, n, recv_bytes, serveraddrsize;
    struct sockaddr_in servaddr;
    char snddata[MAX], rcvdata[MAX];

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addrs.s_addr = inet_addr(HOSTADDR);
    servaddr.sin_port = htons(PORT);

    sd = socket(PF_INET, SOCK_DGRAM, 0);
    while(fgets(snddata, MAX, stdin)!=NULL){
        send_bytes = strlen(snddata);
        n = sendto(sd, send_bytes, 0, (struct sockaddr*)&serveraddr, sizeof(servaddr));
        recv_bytes = 0;
        while(recv_bytes<send_bytes){
            n = recvfrom(sd, rcvdata+recv_bytes, MAX, 0, (struct sockaddr*)&servaddr, &serveraddrsize);
            recv_bytes += n;
        }
        recvdata[recv_bytes] = 0;
        printf("%s", rcvdata);
    }
    close(sd);
    return 0;
}

// server.c
int main(int argc, char* argv[]){
    int sd, nsd, pid, bytes, cliaddrsize;
    int n;
    struct sockaddr_in cliaddr, servaddr;
    char data[MAX];

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    sd = socket(PF_INET, SOCK_DGRAM, 0);
    bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    while(1){
        bytes = recvfrom(sd, data, MAX, 0, (struct sockaddr*)&cliaddr, &cliaddrsize);
        if((n=sendto(sd, data, bytes, 0, (strict sockaddr*)&cliaddr, cliaddrsize))!=bytes){
            printf("can't send data.(%d)\n", n);
            exit(1);
        }
    }
}
```

## 21. 다음 프로그램은 시그널 집합을 만들어서 그 집합에 시그널을 추가한 다음, sigprocmask() 호출을 통해 시그널을 블록 시켰다가 다시 블록을 해제하는 것을 보여준다.

```c
int main(void){
    sigset_t sig_set;
    int count;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGINT);
    sigprocmask(SIG_BLOCK, &sig_set, NULL);
    for(count=3 ; 0<count ; count--){
        printf("count %d\n", count);
        sleep(1);
    }
    printf("Ctrl-C에 대한 블록 해제\n");
    sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
    printf("count 중 Ctrl-C 입력하면 이 문장은 출력되지 않음\n");
    while(1);
    exit(0);
}
```

## 22. 프로그램 실행 시 전달 받은 문자열을 shared memory에 쓰는 프로그램

```c
#define SHARED_MEMORY_SIZE 1024

int main(int argc, char* argv[]){
    key_t key;
    char* data;
    int shared_memory_id;

    if(argc>2){ fprintf(stderr, "usage: %s [data_to_write]\n", argv[0]); exit(1); }
    key = 1234;

    if(shared_memory_id=shmget(key, SHARED_MEMORY_SIZE, 0644|IPC_CREAT)==-1){ fprintf(stderr, "shmget erro\n"); exit(1); }
    if((data=shmat(shared_memory_id, (void*)0, 0))==(char*)(-1)){ fprintf(stderr, "shmat error\n"); exit(1); }
    
    if(argc==2){
        printf("writing to segment: \"%s\" \n", argv[1]);
        strncpy(data, argv[1], SHARED_MEMORY_SIZE);
    }
    else{
        printf("segment contains: \"%s\" \n", data);
    }

    if(shmdt(data)==-1){ fprintf(stderr, "shmdt error\n"); exit(1); }
    
    exit(0);
}
```