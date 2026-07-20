#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    int id;
    int humidity;
    double temperature;
    int CO2;
    int light;
} SensorData;

#define SHM_SIZE 1024
#define FIFO_PATH "/tmp/request_fifo"

// P操作：挂起等待
void P(int semid, int num) {
  struct sembuf op = {num, -1, 0};
  semop(semid, &op, 1);
}

int main(void) {
  // 1. 生成独一无二的 key (使用自身的 PID)
  key_t my_key = getpid();
  printf("A[%d]: 我是新建的 CGI 请求，我的专属 KEY 是 %d\n", my_key, my_key);

  // 2. 使用专属 key 创建属于我自己的共享内存
  int shmid = shmget(my_key, SHM_SIZE, IPC_CREAT | 0666);
  char *addr = shmat(shmid, NULL, 0);

  // 3. 使用专属 key 创建属于我自己的信号量 (只需要1个即可，专用于 B 唤醒 A)
  int semid = semget(my_key, 1, IPC_CREAT | 0666);// 1 代表信号集只有一个信号
  semctl(semid, 0, SETVAL, 0); // 初始化为 0，准备阻塞

  // 4. 把我的 KEY 丢进公共的 FIFO 管道排队发送给 B
  mkfifo(FIFO_PATH, 0666); // 确保管道存在 (如果存在也不会报错)
  int fifo_fd = open(FIFO_PATH, O_WRONLY); 
  if (fifo_fd > 0) {
      write(fifo_fd, &my_key, sizeof(key_t)); // 把自己的 key 塞进管道
      close(fifo_fd);
  }

  printf("A[%d]: 已将 KEY 通过管道发送给 B,阻塞等待 B 传回数据...\n", my_key);

  // 5. 阻塞等待 B 向我的专属共享内存传东西，并用我的专属信号量唤醒我
  P(semid, 0);

  // 6. B 传完了，A 醒来！
//   printf("A[%d]: B 处理完毕！我拿到的数据是: %s\n", my_key, addr);
  // 正确：强转为 SensorData 结构体指针
    SensorData *data = (SensorData *)addr;
    printf("A[%d]: B 处理完毕！我拿到的数据是: ID=%d, 湿度=%d, 温度=%.2f, CO2=%d, 光照=%d\n",
    my_key, data->id, data->humidity, data->temperature, data->CO2, data->light);

        
  // 7. A 负责清理现场：断开挂载，并删除专属共享内存和信号量
  // 建议由 A 自己来删除，因为 A 明确知道自己什么时候“读完了”。
  shmdt(addr);
  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 0, IPC_RMID);

  return 0;
}
