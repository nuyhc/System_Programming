# HPC Lab SP
## 1강-2. Unix/Linux 개요

### Unix/Linux system
- 현재, 다양한 시스템에서 가장 널리 사용되는 OS

### Unix/Linux architecture
- Kernel
  - OS의 핵심 기능을 모아 놓은 것
- System call
  - OS의 기능 중 응용프로그램이 사용하도록 허락된 기능들의 집합
- Shell
  - 사용자와 OS 사이의 인터페이스
  - 사용자의 입력을 해석하여 커넣의 함수 호출
- Utilities

### Linux
- PC를 위한 효율적인 유닉스 시스템
- 오픈소스
- 다양한 플랫폼에 이식 가능
- 높은 안정성
- 강력한 네트워크 기능
- GNU 소프트웨어와 함께 배포

## 1강-3. Linux 기초 명령어
### 기초 명령어
#### Login/Logout
| 명령 | 기능 | 주요 옵션 | EX |
| :--- | :---: | :--- | :--- |
| telnet | 유닉스시스템에 접속 | | telnet 주소 |
| logout <br> exit | 유닉스시스템에서 접속 해제 | | logout <br> exit |

#### Process management
| 명령 | 기능 | 주요 옵션 | EX |
| :--- | :---: | :--- | :--- |
| ps | 현재 실행 중인 프로세스의 정보 출력 | -u : 특정 사용자의 Proc. <br> -ef: 모든 Proc.의 상세 정보 | ps <br> ps -u 사용자ID <br> ps -ef |
| kill | Process 강제 종료 | -9: 강제 종료 | kill 5000 <br> kill -9 5001 |

#### File/Directory
| 명령 | 기능 | 주요 옵션 | EX |
| :--- | :---: | :--- | :--- |
| pwd | 현재 디렉토리 경로 출력 | | |
| ls | 디렉토리 내용 출력 | -a: 숨긴파일출력 <br> -l: 파일 상세정보 출력 | |
| cd | 현재 디렉토리 변경 | | |
| cp | 파일/디렉토리 복사 | -r: 디렉토리 복사 | cp a.txt b.txt <br> cp -r dir1 dir2 |
| mv | 파일/디렉토리 이름변경과 이동 | | mv a.txt b.txt <br> mv a.txt dir1 |
| rm | 파일/디렉토리 삭제 | -r: 디렉토리 삭제 | |
| mkdir | 디렉토리 생성 | | |
| rmdir | 빈 디렉토리 삭제 | | |
| cat | 파일 내용 출력 | | |
| more | 파일 내용을 쪽단위로 출력 | | |
| chmod | 파일 접근권한 변경 | | chmod 755 a.exe <br> chmod go+x a.exe |
| grep | 패턴 검색 | | grep abcd a.txt |

#### Others
| 명령 | 기능 | 주요 옵션 | EX |
| :--- | :---: | :--- | :--- |
| su | 사용자 계정 변경 | -: 변경할 사용자의 환경 초기화 파일 실행 | |
| tar | 파일/디렉토리 묶기 | -cvf: tar파일 생성 <br> -tvf: tar파일내용보기 <br> -xvf: tar파일 풀기 | tar cvf a.tar * <br> tar tvf a.tar <br> tar xvf a.tar |
| whereis <br> which | 파일 위치 검색 | | whereis ls <br> which telnet |

### Man page
- Unix/Linux 시스템 사용 설명서
- ```man [options] [section] page```
  - page: 검색하려는 명령어/함수 이름
  - section: page의 소속 구분
    - 1: Shell cmd, 2: System call, 3: Std. library

## Hello Linux
### VI editor
| 기능 | 명령 | 기능 | 명령 |
| :--- | :---: | :--- | :---: |
| 입력모드전환 | i, a, o, O | 명령모드전환 | ESC |
| 커서이동 | j, k, h, l 또는 방향키 | 행이동 | #G (50G, 143G, ...) <br> :행번호 |
| 한글자수정 | r | 여러글자수정 | #s (5s, 7s, ...) |
| 단어수정 | cw | 명령취소 | u, U |
| 검색하여수정 | :%s/aaa/bbb/g | 복사 | #yy (5yy, 10yy) |
| 붙이기 | p | 커서이후삭제 | D(shidt-d) |
| 잘라내기 | x, #x(3x, 5x, ...) | 행삭제(잘라내기) | dd, #dd(3dd, 4dd, ...) |
| 저장하고 종료 | :wq! <br> ZZ | 저장하지 않고 종료 | :q! |

## 1강-4. Makefile & Make (Unix/Linux 컴파일 환경)
### GNU C compiler
- Basic compiler for Unix/Linux
- ```gcc [options] filename```
  - options
    - -c: object file(.o)만 생성
    - -o: execution file name 지정

### Makefile & Make
#### Makefile
- Compile 방법을 기술 하는 파일
  - 관련 파일 정보, compile 명령, 실행 파일명 등
- 여러 파일로 구성된 프로젝트 설정과 비슷한 개념

#### Make
- 주어진 Makefile에 따라 compile을 수행하고, 실행파일을 생성
- 최초 컴파일 이후에는, 변경이 있는 파일만 컴파일 함

#### Without make file
```bash
$gcc -c -o main.o main.c
$gcc -c -o foo.o foo.c
$gcc -c -o bar.o bar.c

$gcc -o app.out main.o foo.o bar.o
```

#### Makefile -> 소스코드가 있는 폴더에 위치
```bash
app.out: main.o foo.o bar.o
  gcc -o app.out main.o foo.o bar.o

main.o: foo.h bar.h main.c
  gcc -c -o main.o main.c

foo.o: foo.h foo.c
  gcc -c -o foo.o foo.c

bar.o: bar.h bar.c
  gcc -c -o bar.o bar.c
```
- Rule block
```bash
<Target>:<Dependencies>
  <Recipe>

# Target
  - Bulid 대상 이름
  - 일반적으로 최종 결과 파일명 사용
# Dependency
  - Build 대상이 의존하는 Target이나 파일 목록
# Recipe
  - Build 대상을 생성하는 명령어
```
- Implicit rules (Built-in rules)
  - 자주 사용되는 규칙을 자동으로 처리
```bash
app.out: main.o foo.o bar.o
  gcc -o app.out main.o foo.o bar.o

main.o: foo.h bar.h main.c
foo.o: foo.h foo.c
bar.o: bar.h bar.c
```

#### Variables (or Macro)
`$(VarName)`
```bash
CC = gcc
CFLAGS = -g -Wall
OBJS = main.o foo.o bar.o
TARGET = app.out

$(TARGET):$(OBJS)
  $(CC) -o $@ $(OBJS)

main.o: foo.h bar.h main.c
foo.o: foo.h foo.c
bar.o: bar.h bar.c
```

#### Clean rule
- Build로 생성된 파일들을 삭제하느 규칙
- Clean build를 위해 사용 가능
```bash
clean:
  rm -f *.o
  rm -f $(TARGET)

$make clean
```

### Makefile -basic pattern
```bash
CC = gcc
CFLAGS = -g -Wall
OBJS = main.o foo.o bar.o
TARGET = app.out

all: $(TARGET)

$(TARGET) : $(OBJS)
  $(CC) -o $@ $(OBJS)

main.o: foo.h bar.h main.c
foo.o: foo.h foo.c
bar.o: bar.h bar.c

clean:
  rm -f *.o
  rm -f $(TARGET)
```

```bash
CC = <compiler>
CFLAGS = <options for compiler>
LDFLAGS = <options for linker>
LDLIBS = <a list of library to link>
OBJS = <a list of object file>
TARGET = <build target name>

all: $(TARGET)

clean:
  rm -f *.o
  rm -f $(TARGET)

$(TARGET) : $(OBJS)
  $(CC) -o $@ $(OBJS)
```