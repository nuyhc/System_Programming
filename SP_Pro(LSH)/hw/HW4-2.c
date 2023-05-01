#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
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
    struct record current;
    int record_no;
    int sendRecord_no;
    int fd, pos, i, n;
    char yes;
    char operation;
    int amount;
    char buffer[100];
    int quit = FALSE;

    fd = open("./account", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    while(!quit){
        printf("Enter account number (0-99): ");
        scanf("%d", &record_no);
        fgets(buffer, 100, stdin);

        if(record_no<0 && record_no>=NUM_RECORDS){
            printf("Invalid account number!\n");
            break;
        }
        printf("Enter Operation Name (c/r/d/w/t/q): ");
        scanf("%c", &operation);

        switch(operation){
            case 'c':
                reclock(fd, record_no, sizeof(struct record), F_WRLCK);
                pos = record_no*sizeof(struct record);
                lseek(fd, pos, SEEK_SET);
                printf("> id? ");
                scanf("%d", &current.id);
                printf("> name ? ");
                scanf("%s", current.name);
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
                display_record(&current);
                reclock(fd, record_no, sizeof(struct record), F_UNLCK);
                break;
            case 'd':
                reclock(fd, record_no, sizeof(struct record), F_WRLCK);
                pos = record_no * sizeof(struct record);
                lseek(fd, pos, SEEK_SET);
                n = read(fd, &current, sizeof(struct record));
                display_record(&current);
                printf("Enter amount: ");
                scanf("%d", &amount);
                current.balance += amount;
                display_record(&current);
                lseek(fd, pos, SEEK_SET);
                write(fd, &current, sizeof(struct record));
                reclock(fd, record_no, sizeof(struct record), F_UNLCK);
                break;
            case 'w':
                reclock(fd, record_no, sizeof(struct record), F_WRLCK);
                pos = record_no*sizeof(struct record);
                lseek(fd, pos, SEEK_SET);
                n = read(fd, &current, sizeof(struct record));
                display_record(&current);
                printf("Enter amount: ");
                scanf("%d", &amount);
                if(amount>current.balance){
                    printf("Not enough money!\n");
                    break;
                }
                current.balance -= amount;
                display_record(&current);
                lseek(fd, pos, SEEK_SET);
                write(fd, &current, sizeof(struct record));
                reclock(fd, record_no, sizeof(struct record), F_UNLCK);
                break;
            case 't':
                reclock(fd, record_no, sizeof(struct record), F_WRLCK);
                pos = record_no*sizeof(struct record);
                display_record(&current);
                printf("Enter amount: ");
                scanf("%d", &amount);
                if(amount>current.balance){
                    printf("Not enough money!\n");
                    break;
                }
                current.balance -= amount;
                display_record(&current);
                lseek(fd, pos, SEEK_SET);
                write(fd, &current, sizeof(struct record));
                reclock(fd, sendRecord_no, sizeof(struct record), F_UNLCK);

                printf("Enter account number for send: ");
                scanf("%d", &sendRecord_no);
                reclock(fd, sendRecord_no, sizeof(struct record), F_WRLCK);
                pos = sendRecord_no*sizeof(struct record);
                lseek(fd, pos, SEEK_SET);
                n = read(fd, &current, sizeof(struct record));
                display_record(&current);
                current.balance += amount;
                display_record(&current);
                lseek(fd, pos, SEEK_SET);
                write(fd, &current, sizeof(struct record));
                reclock(fd, sendRecord_no, sizeof(struct record), F_UNLCK);
                break;
            case 'q':
                quit = TRUE;
                break;
            default:
                printf("illegal input\n");
                continue;
        }
    }
    close(fd);
    fflush(NULL);
}

int reclock(int fd, int recno, int len, int type){
    struct flock f1;
    switch(type){
        case F_RDLCK:
        case F_WRLCK:
        case F_UNLCK:
            f1.l_type = type;
            f1.l_whence = SEEK_SET;
            f1.l_start = recno * len;
            f1.l_len = len;
            fcntl(fd, F_SETLKW, &f1);
            return 1;
        default:
            return -1;
    }
}

void display_record(struct record *curr){
    printf("\n");
    printf("id: %d\n", curr->id);
    printf("name: %s\n", curr->name);
    printf("balance: %d\n", curr->balance);
    printf("\n");
}