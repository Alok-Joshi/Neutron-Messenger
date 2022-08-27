typedef struct client_data
{
  char name[10000];
  int socket;
  
}client_data;

typedef struct Node
{
  client_data data;
  struct Node* next;
}Node;

Node* deletenode(Node* H,int sock,char* display_message);
Node* insert_node(Node* head,Node* tail,char* client_name);


