#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "process_create.h"

pid_t process_pid;
int process_create(){
 // 用来存 fork() 的返回值（区分父子进程）

    process_pid = fork();
     if (process_pid) {
        perror("fork");   // 打印错误原因
        return -1;
    }
    return process_pid;
}

//  if (pid == -1) {
//         perror("fork");   // 打印错误原因
//         return -1;
//     }

//     if (pid == 0) {
//         printf("child process running...\n");
//         getchar();
//         printf("child process normal exit...\n");
//         exit(5);
//     }
//     else {
//         int state;   // 用来保存子进程“死亡信息”
//         pid_t rc;    // wait() 的返回值（子进程 PID）
//         printf("parent process wait...!\n");
//         rc = wait(&state);
//         if (rc == -1) {
//             perror("wait");
//             exit(0);
//         }
//         if (rc == pid) {
//             printf("catch child process exit state: %x!\n", state);
//             if (WIFEXITED(state)) {
//                 printf("child process normal exit: %d\n",
//                         (state));
//             } else {
//                 printf("child normal exit!\n");
//             }
//         }
//     }
//     printf("parent process ready exit!\n");




    
