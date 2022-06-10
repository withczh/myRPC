#include "common.h"
#include "serialize.h"

void rpc_send_recv (serialized_buffer_t *client_send_ser_buffer,serialized_buffer_t *client_recv_ser_buffer)
{

    struct sockaddr_in dest;
    int sockfd = 0, rc = 0, recv_size = 0;
    int addr_len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("socket creation failed\n");
        return;
    }
    bzero(&dest,sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET,SERVER_IP,&dest.sin_addr);    
    addr_len = sizeof(struct sockaddr);

    if(connect(sockfd,(struct sockaddr*)&dest,addr_len)==-1)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    printf("connect success,ready to call..\n");
    rc = send(sockfd, client_send_ser_buffer->buf,get_serialize_buffer_size(client_send_ser_buffer),0);

    printf("%s() : %d bytes sent\n", __FUNCTION__, rc);

    recv_size = recv(sockfd, client_recv_ser_buffer->buf,get_serialize_buffer_length(client_recv_ser_buffer),0);

    printf("%s() : %d bytes recieved\n", __FUNCTION__, recv_size);
}

/* client_stub_marshal()*/
serialized_buffer_t * client_stub_marshal(int a, int b,int c)
{

    serialized_buffer_t *client_send_ser_buffer = NULL;
    rpc_hdr_t rpc_hdr;
    
    init_serialized_buffer_of_defined_size(&client_send_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);

    /*准备标识头的缓冲空间*/
    serialize_buffer_skip(client_send_ser_buffer,sizeof(rpc_hdr));
    switch (c)
    {
    case 1:
        rpc_hdr.rpc_id=ADD_ID;
        break;
    case 2:
        rpc_hdr.rpc_id=SUB_ID;
        break;
    case 3:
        rpc_hdr.rpc_id=MULTIPLY_ID;
        break;
    case 4:
        rpc_hdr.rpc_id=DIV_ID;
        break;
    default:
        break;
    }
    rpc_hdr.pay_load_size=0;
    
    /*Serialize the first Argument*/
    serialize_data(client_send_ser_buffer, (char *)&a, sizeof(int));
    /*Serialize the second Argument*/
    serialize_data(client_send_ser_buffer, (char *)&b, sizeof(int));

    /*序列化标识头*/
    rpc_hdr.pay_load_size=get_serialize_buffer_size(client_send_ser_buffer)-sizeof(rpc_hdr);
    copy_in_serialized_buffer_by_offset(client_send_ser_buffer,sizeof(rpc_hdr.rpc_id),(char*)&rpc_hdr.rpc_id,0);
    copy_in_serialized_buffer_by_offset(client_send_ser_buffer,sizeof(rpc_hdr.pay_load_size),(char*)&rpc_hdr.pay_load_size,sizeof(rpc_hdr.pay_load_size));    

    return client_send_ser_buffer;
}

int client_stub_unmarshal(serialized_buffer_t *client_recv_ser_buffer)
{

    /*Reconstruct the result obtained from Server*/
    int res = 0;
    de_serialize_data((char *)&res, client_recv_ser_buffer, sizeof(int));
    return res;
}

int remote_procedure_call(int a , int b,int c)
{

    /*步骤 2 : 序列化参数*/
    /*函数模板 :  serialized_buffer_t* (client_stub_marshal) <Arg1, Arg2, . . . ,> */
    serialized_buffer_t *client_send_ser_buffer = client_stub_marshal(a, b,c);
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
     
    int res = client_stub_unmarshal(client_recv_ser_buffer);

    /*Client端成功得到结果，释放client_recv_ser_buffer*/

    free_serialize_buffer(client_recv_ser_buffer);
    return res;
}

int
main(int argc, char **argv){

    int a, b,c;
    printf("Enter First number : ");
    scanf("%d", &a);
    printf("Enter Second number : ");
    scanf("%d", &b);
    printf("what you want to do ?\n");
    printf("1.add\t2.sub\t3.multiply\t4.div\n");
    scanf("%d",&c);
    if(c==4)
    {
        while(b==0)
        {
            printf("分母不能为0！！！,重输:\n");
            scanf("%d",&b);
        }
    }

    /*步骤 1 : 调用rpc*/
    int res = remote_procedure_call(a, b,c);

    /*步骤 7 : 打印RPC的回调结果*/
    printf("result = %d\n", res);
    return 0; 
}