#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char user_name[1000];
int PERMANANET_SOCK = 0;
GtkBuilder *builder; 

/* Widgets for Starting screen*/
GtkWidget *startscreen;
GtkWidget *username;
GtkWidget *password;
GtkWidget *connectoserver;
GtkWidget *user_pass_check;
/* Widgets for chat window */
GtkWidget *chatwindow;
GtkWidget *chatview;
GtkWidget *sendbutton;
GtkWidget *userentry;
GtkTextBuffer *chatbuffer;
void on_sendbutton_clicked(GtkButton *b);
int main(int argc, char * argv[])
{
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file("Client.glade");
    
    /*Widget assignments for starting screen */
    startscreen = GTK_WIDGET(gtk_builder_get_object(builder,"startscreen"));
    username = GTK_WIDGET(gtk_builder_get_object(builder,"username"));
    password = GTK_WIDGET(gtk_builder_get_object(builder,"password"));
    connectoserver = GTK_WIDGET(gtk_builder_get_object(builder,"connectoserver"));
    user_pass_check = GTK_WIDGET(gtk_builder_get_object(builder,"user_pass_check"));
    g_signal_connect(startscreen,"destroy",G_CALLBACK(gtk_main_quit),NULL); 
    
     
    /*Widget assignments for chat window */
    chatwindow = GTK_WIDGET(gtk_builder_get_object(builder,"chatwindow"));
    chatview = GTK_WIDGET(gtk_builder_get_object(builder,"chatview"));
    sendbutton = GTK_WIDGET(gtk_builder_get_object(builder,"sendbutton"));
    userentry = GTK_WIDGET(gtk_builder_get_object(builder,"userentry"));
    chatbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chatview));
    g_signal_connect(chatwindow,"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(userentry, "activate", G_CALLBACK(on_sendbutton_clicked), NULL);
    
    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show(startscreen);
    gtk_main();
    
}

//Function to disable clicking on the textview
gboolean* on_chatview_button_press_event(GtkWidget *widget, GdkEvent *event)
{
   return True;
}
//Function which verifies username and password from the server
int verify()
{    
    struct sockaddr_in ServerIp;
    int sock = socket(AF_INET, SOCK_STREAM,0);
    
    ServerIp.sin_port = htons(9002);
    ServerIp.sin_family= AF_INET;
    ServerIp.sin_addr.s_addr = INADDR_ANY;
    
    if( (connect( sock ,(struct sockaddr *)&ServerIp,sizeof(ServerIp))) == -1 )
      {
         printf("\n connection to socket failed \n");
      }
      
      char pass[100];
      char username_[100];
      char server_response[100];
      int response_len;
      
      strcpy(pass,(char*)gtk_entry_get_text(password));
      strcpy(username_, (char*)gtk_entry_get_text(username));
      
      //check password
      if(send(sock,pass,strlen(pass),0)<0)
      { 
        perror(" sending failure for password: ");
        return -99;
      }
        
      response_len = recv(sock,server_response,100,0);
      
      server_response[response_len] = '\0';
      if(strcmp(server_response,"FAILURE")== 0)
      {   
          close(sock);
          return -1;
      }
      else
      { 
        sleep(2);
        if(send(sock,username_,strlen(username_),0)<0)
           { 
             perror(" sending failure for username: ");
             return -99;
           }
        
        strcpy(server_response,"");
        
        int response_len = recv(sock,server_response,100,0);
        
        server_response[response_len] = '\0';
         if(strcmp(server_response,"FAILURE") == 0)
           {
             close(sock);
             return 0;
           }
         else
           {  
             close(sock);
             return 1;
           }  
      }
     
}
// Below function looks for messages from the 
void *recvmg(void *my_sock)
{
    int sock = *((int *)my_sock);
    int len;
    char msg[1000];
    // client thread always ready to receive message
    while((len = recv(sock,msg,500,0)) > 0) 
    {
        msg[len] = '\0';
        gtk_text_buffer_insert_at_cursor(chatbuffer, msg, len);
    }
    close(sock);
}
//once password verification is done, below function will establish a new connection and create a new thread which will recieve messages and relay them to the buffer
void start_recieving() 
{
    pthread_t recvt;
    int len;
    int sock;
    char send_msg[500];
    struct sockaddr_in ServerIp;
    
    
    sock = socket( AF_INET, SOCK_STREAM,0);
    PERMANANET_SOCK = sock; //global variable
    ServerIp.sin_port = htons(9002);
    ServerIp.sin_family= AF_INET;
    ServerIp.sin_addr.s_addr = INADDR_ANY;
    if( (connect( sock ,(struct sockaddr *)&ServerIp,sizeof(ServerIp))) == -1 )
        printf("\n connection to socket failed \n");
        
    send(sock,"DONE",strlen("DONE"),0);
    sleep(1);
    
    int send_stat = send(sock,user_name,strlen(user_name),0);     //sending the name TEST 
    if(send_stat<=0) printf("sending failed \n");
    else printf("sending successful. send_stat: %d \n",send_stat);
    //creating a client thread which is always waiting for a message
    pthread_create(&recvt,NULL,(void *)recvmg,&sock);

   
    
    
    
}
// Below function to check if the user is has entered a blank messsage in the textbox
int message_not_empty( const char * message)
{
   for(int i = 0; i<strlen(message); i++)
   {
      if(message[i] != ' ') return 1;
   }
   return 0;
}

//Function to send the message entered by the user in textbox
void on_sendbutton_clicked(GtkButton *b)
 
{  
   
   char msg[500000];
   strcpy(msg,user_name);
   strcat(msg, ": ");
   strcat(msg,( const char*)gtk_entry_get_text(((GtkEntry*)userentry)));
   strcat(msg, " \n");
  
   if(message_not_empty(( const char*)gtk_entry_get_text((GtkEntry*)userentry)))
   {
      send(PERMANANET_SOCK,msg,strlen(msg),0);
   }
   gtk_entry_set_text(userentry,"");
}
// Function to call connect to server button is clicked 
void on_connectoserver_clicked(GtkButton *b)
{
   gtk_label_set_text(user_pass_check,"");
   if(strlen((char*)gtk_entry_get_text(username)) == 0 || strlen((char*)gtk_entry_get_text(password))== 0)
   {
      
      gtk_label_set_text(user_pass_check,"Username or password \n not entered.");
      
   }
   else
   { 
    int status = verify();
    if(status == 1)
    { 
       strcpy(user_name,(char*)gtk_entry_get_text(username));
       gtk_widget_hide(startscreen);
       gtk_widget_show(chatwindow);
       start_recieving();
       
    }
    else if(status == 0)
    {
       gtk_label_set_text(user_pass_check,"Username already in use.");
    }
    else if(status == -1)
    {
       
       gtk_label_set_text(user_pass_check,"Wrong Password.");
      
    }
    else
    {
       gtk_label_set_text(user_pass_check,"Server Offline.");
    }
      
   }
   
}




