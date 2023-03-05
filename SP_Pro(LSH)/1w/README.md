### fileio-ex.c
```
// test1.txt
Hello, World (1)

// test2.txt
Hello, World (2)

$gcc -o fileio-ex fileio-ex.c
$./fileio-ex test1.txt test2.txt

// Output
// test2.txt
Hello, World (2)
Hello, World (1)
```
### lineio-ex.c
```
// test1.txt
Hello, World (1)

// test2.txt
Hello, World (2)

$gcc -o lineio-ex lineio-ex.c
$./lineio-ex test1.txt test2.txt

// Output
// test2.txt
Hello, World (2)
Hello, World (1)
```

### frandom-ex.c
```
// test.dat
This is a test data.

$gcc -o frandom-ex frandom-ex.c
$./frandom-ex test.dat

// Output
Position: 8
a test data.
Position: 0
This is a test data.
```

### ferror-ex.c
```
// test.dat
1234 abcd

$gcc -o ferror-ex ferror-ex.c
$./ferror-ex test.dat

// Output
ferror() return 1
ferror() return 0
```

### feof-ex.c
```
// test.dat
1234 abcd

$gcc -o feof-ex feof-ex.c
$./feof-ex test.dat

// Output
1234 abcd
feof returned 1
```