#include "serialize.h"
#include "common.h"

int add(int a,int b)
{
    return a+b;
}
int sub(int a,int b)
{
    return a-b;
}
int multiply(int a,int b)
{
    return a*b;
}
int division(int a,int b)
{
    return a/b;
}

int add_server_stub_unmarshal(serialized_buffer_t *server_recv_ser_buffer)
{

    int a , b;
    de_serialize_data((char *)&a, server_recv_ser_buffer, sizeof(int));   
    de_serialize_data((char *)&b, server_recv_ser_buffer, sizeof(int));

    /*步骤 6  : 调用远程的实际方法*/
    return add(a, b);
}

void add_server_stub_marshal(int res, serialized_buffer_t *server_send_ser_buffer)
{

    serialize_data(server_send_ser_buffer, (char *)&res, sizeof(int));
}

int sub_server_stub_unmarshal(serialized_buffer_t *server_recv_ser_buffer)
{

    int a , b;
    de_serialize_data((char *)&a, server_recv_ser_buffer, sizeof(int));   
    de_serialize_data((char *)&b, server_recv_ser_buffer, sizeof(int));

    /*步骤 6  : 调用远程的实际方法*/
    return sub(a, b);
}

void sub_server_stub_marshal(int res, serialized_buffer_t *server_send_ser_buffer)
{

    serialize_data(server_send_ser_buffer, (char *)&res, sizeof(int));
}

int multiply_server_stub_unmarshal(serialized_buffer_t *server_recv_ser_buffer)
{

    int a , b;
    de_serialize_data((char *)&a, server_recv_ser_buffer, sizeof(int));   
    de_serialize_data((char *)&b, server_recv_ser_buffer, sizeof(int));

    /*步骤 6  : 调用远程的实际方法*/
    return multiply(a, b);
}

void multiply_server_stub_marshal(int res, serialized_buffer_t *server_send_ser_buffer)
{

    serialize_data(server_send_ser_buffer, (char *)&res, sizeof(int));
}

int div_server_stub_unmarshal(serialized_buffer_t *server_recv_ser_buffer)
{

    int a , b;
    de_serialize_data((char *)&a, server_recv_ser_buffer, sizeof(int));   
    de_serialize_data((char *)&b, server_recv_ser_buffer, sizeof(int));

    /*步骤 6  : 调用远程的实际方法*/
    return division(a, b);
}

void div_server_stub_marshal(int res, serialized_buffer_t *server_send_ser_buffer)
{

    serialize_data(server_send_ser_buffer, (char *)&res, sizeof(int));
}

void rpc_server_process_msg(serialized_buffer_t *server_recv_ser_buffer,serialized_buffer_t *server_send_ser_buffer)
{

   /*  步骤 5 */
   /*  Signature : <rpc return type> server_stub_unmarshal (serialized_buffer_t *ser_data)
    *  Unmarshalling of Arguments*/
    /* Unmarshalling of Arguments is done here. Reconstruct the Arguments*/

    rpc_hdr_t rpc_hdr;
    de_serialize_data((char*)&rpc_hdr.rpc_id,server_recv_ser_buffer,sizeof(rpc_hdr.rpc_id));
    de_serialize_data((char*)&rpc_hdr.pay_load_size,server_recv_ser_buffer,sizeof(rpc_hdr.pay_load_size));

    /*步骤 7 : Now we have got the RPC result, time to serialize/Marshall the result*/
    int res=0;
    switch (rpc_hdr.rpc_id)
    {
    case ADD_ID:
        res = add_server_stub_unmarshal(server_recv_ser_buffer);
        add_server_stub_marshal(res,server_send_ser_buffer);
        break;
    case SUB_ID:
        res = sub_server_stub_unmarshal(server_recv_ser_buffer);
        sub_server_stub_marshal(res,server_send_ser_buffer);
        break;
    case MULTIPLY_ID:
        res = multiply_server_stub_unmarshal(server_recv_ser_buffer); 
        multiply_server_stub_marshal(res, server_send_ser_buffer);
        break;
    case DIV_ID:
        res = div_server_stub_unmarshal(server_recv_ser_buffer);
        div_server_stub_marshal(res,server_send_ser_buffer);
        break;    
    default:
        break;
    }
}

int main(int argc, char **argv)
{

	int listenfd = 0,confd=0, len, addr_len, opt = 1,reply_msg_size = 0;
         
	struct sockaddr_in server_addr,client_addr;
    socklen_t client_len;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0 )) == -1)
    {
        perror("socket creation failed\n");
        exit(1);
    }
    bzero(&server_addr,sizeof(server_addr));
     server_addr.sin_family = AF_INET;
     server_addr.sin_port = htons(SERVER_PORT);
     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     client_len = sizeof(client_addr);

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt))<0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

    if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("socket bind failed\n");
        exit(1);
    }

    if(listen(listenfd,5)==-1)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    /*Prepare Server Memory buffers to send and Recieve serialized Data*/

    serialized_buffer_t *server_recv_ser_buffer = NULL,*server_send_ser_buffer = NULL;

    init_serialized_buffer_of_defined_size(&server_recv_ser_buffer,MAX_RECV_SEND_BUFF_SIZE);

    init_serialized_buffer_of_defined_size(&server_send_ser_buffer,MAX_RECV_SEND_BUFF_SIZE);

	printf("Server ready to service rpc calls..\n");

    while(1)
    {
        reset_serialize_buffer(server_recv_ser_buffer);

        /*步骤 4 : Recieve the Data from client in local buffer*/
        confd=accept(listenfd,(struct sockaddr*)&client_addr,&client_len);
        if(confd<0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        printf("connected client's info:[%s:%d]\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        len = recv(confd, server_recv_ser_buffer->buf,get_serialize_buffer_length(server_recv_ser_buffer),0);
        printf("rpc server received with %d bytes msg\n", len);

        /*prepare the buffer to store the reply msg to be sent to client*/
        reset_serialize_buffer(server_send_ser_buffer);

	    rpc_server_process_msg(server_recv_ser_buffer, /*Serialized Data which came from client*/
                           server_send_ser_buffer); /*Empty serialized buffer*/

        /*步骤 8 : Send the serialized result data back to client*/
	    reply_msg_size = send(confd, server_send_ser_buffer->buf,get_serialize_buffer_size(server_send_ser_buffer),0);

	    printf("rpc server replied with %d bytes msg\n", reply_msg_size);
    }
	return 0;
}
