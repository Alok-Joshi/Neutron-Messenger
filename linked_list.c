#include "linked_list.h"

Node* deletenode(Node* H,int sock,char* display_message)
{ 
   if(H == NULL)
   { 
     return H;
   }
   if(H->data.socket == sock)
   {  
      
      strcpy(display_message, H->data.name);
      strcat(display_message,"      l e f t   t h e  c h a t : ( \n \n");
      gtk_text_buffer_insert_at_cursor(monitorbuffer,display_message, strlen(display_message));
      
      //actual deletion
      Node* deletenode = H;
      H = H->next;
      free(deletenode);
      return H;
      
   }
   else
   {
      H->next = deletenode(H->next,sock,display_message);
      if(H->next == NULL) tail = H;
      return H;
   }
   
}


Node* insert_node(Node* head,Node* tail,char* client_name){
	       
       if(head == NULL)
       {
		Node* newnode = (Node*)malloc(sizeof(Node));
        strcpy(newnode->data.name,client_name);
        newnode->data.socket = client_socket;
        head = newnode;
        tail = newnode;
        newnode->next = NULL;
       }
       else
       {
         Node* newnode = (Node*)malloc(sizeof(Node));
         strcpy(newnode->data.name,client_name);
         newnode->data.socket = client_socket;
         tail->next = newnode;
         tail = newnode;
         tail->next = NULL;
       }  
       
 

}
