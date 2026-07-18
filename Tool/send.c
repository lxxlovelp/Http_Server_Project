#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include"send.h"
#include"json.h"


int Send_Data_Quene(){
		// 1. 生成key值
	key_t key = ftok("/tmp", 556);
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	printf("key = %x\n", key);

	// 2. 通过key值获取、创建 IPC对象，创建时，增加0666的权限位
	int msg_fd = msgget(key, IPC_CREAT | 0666);
	if (msg_fd == -1) {
		perror("msg get");
		return -1;
	}
	printf("get msgQueue id: %x\n", msg_fd);

	// 4. 定义消息类型的消息
    // 填充 msg.text 的各个字段...
    // msgsz 精确指定为单个 SensorData 的大小
    int ret = msgsnd(msg_fd, &msg2, sizeof(SensorData), 0);
    if (ret==-1){
        perror("msg snd");
		return 0;
    }
	return 0;

}
