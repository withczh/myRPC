#include "common.h"
#include "serialize.h"

void rpc_send_recv (serialized_buffer_t *client_send_ser_buffer,serialized_buffer_t *client_recv_ser_buffer)
{

    struct sockaddr_in dest;
    int sockfd = 0, rc = 0, recv_size = 0;
    int addr_len;

    dest.sin_family = AF_INET;
    dest.sin_port = htons(SERVER_PORT);
    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP);
    dest.sin_addr = *((struct in_addr *)host->h_addr);
    addr_len = sizeof(struct sockaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd == -1){
        perror("socket creation failed\n");
        return;
    }

    rc = sendto(sockfd, client_send_ser_buffer->buf,get_serialize_buffer_data_size(client_send_ser_buffer),0,(struct sockaddr *)&dest, sizeof(struct sockaddr));

    printf("%s() : %d bytes sent\n", __FUNCTION__, rc);

    recv_size = recvfrom(sockfd, client_recv_ser_buffer->buf,get_serialize_buffer_length(client_recv_ser_buffer), 0,(struct sockaddr *)&dest, &addr_len);

    printf("%s() : %d bytes recieved\n", __FUNCTION__, recv_size);
}

/* client_stub_marshal()*/
serialized_buffer_t * multiply_client_stub_marshal(int a, int b)
{

    serialized_buffer_t *client_send_ser_buffer = NULL;
    init_serialized_buffer_of_defined_size(&client_send_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);

    /*Serialize the first Argument*/
    serialize_data(client_send_ser_buffer, (char *)&a, sizeof(int));
    /*Serialize the second Argument*/
    serialize_data(client_send_ser_buffer, (char *)&b, sizeof(int));

    return client_send_ser_buffer;
}

int multiply_client_stub_unmarshal(serialized_buffer_t *client_recv_ser_buffer)
{

    /*Reconstruct the result obtained from Server*/
    int res = 0;
    de_serialize_data((char *)&res, client_recv_ser_buffer, sizeof(int));
    return res;
}

int multiply_rpc(int a , int b)
{

    /*步骤 2 : 序列化参数*/
    /*函数模板 :  serialized_buffer_t* (client_stub_marshal) <Arg1, Arg2, . . . ,> */
    serialized_buffer_t *client_send_ser_buffer = multiply_client_stub_marshal(a, b);
    serialized_buffer_t *client_recv_ser_buffer = NULL;

    /*准备接收缓冲区用以存放从server端接收的序列化数据*/
    init_serialized_buffer_of_defined_size(&client_recv_ser_buffer,MAX_RECV_SEND_BUFF_SIZE);

    /*步骤 3 : 向server端发送序列化的数据,并等待回应*/



    /*发送和接收的序列化缓冲区的处理函数*/
     rpc_send_recv(client_send_ser_buffer, client_recv_ser_buffer);

     /*发送完缓冲区后释放*/
     free_serialize_buffer(client_send_ser_buffer);
     client_send_ser_buffer = NULL;

    /*步骤 4 , 5, 6 , 7, 8 在server端执行*/

    /*步骤 9 : 反序列化从server接收的数据
     * 模板 : <rpc return type> (client_stub_unmarshal) <serialized_buffer_t *>
     * */
     
    int res = multiply_client_stub_unmarshal(client_recv_ser_buffer);

    /*Client端成功得到结果，释放client_recv_ser_buffer*/

    free_serialize_buffer(client_recv_ser_buffer);
    return res;
}

int
main(int argc, char **argv){

    int a, b;
    printf("Enter First number : ");
    scanf("%d", &a);
    printf("Enter Second number : ");
    scanf("%d", &b);

    /*步骤 1 : 调用rpc*/
    int res = multiply_rpc(a, b);

    /*步骤 7 : 打印RPC的回调结果*/
    printf("result = %d\n", res);
    return 0; 
}