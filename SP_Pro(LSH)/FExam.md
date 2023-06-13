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

# 13. Network

# 14. Time Managemnet