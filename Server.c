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

struct client_data
{
  char name[10000];
  int socket;
  
};
char censored_words[10][10000];
int censored_word_count = 0;
const char server_pass[100] = "VIIT1234$";
Node* head = NULL;
Node* tail = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// here we have pointers to the data structures which are created (buttons, window, label, in the GUI) in the mmemory
//the builder
GtkBuilder *builder; //pointer to the builder which loads the XML file

//Startscreen widgets
GtkWidget *welcomewindow;
GtkWidget *startserverbtn;
GtkWidget *serversettings;
GtkWidget *WelcomeLabel;


//serverscreen widgets
GtkWidget *serverscreen;
GtkWidget *serverstatus;
GtkWidget *messageopen;
GtkWidget *monitorview;
GtkTextBuffer *monitorbuffer;

//Server Setttings window
GtkWidget *settingswindow;
GtkWidget *passwordinput;
GtkWidget *censoredwordinput;
GtkWidget *savebutton;

//Chat monitor window
GtkWidget *messagemonitor;
GtkWidget *chatview;
GtkTextBuffer *chatbuffer;

//method removes a node from the Linked List Data Structure to store user data
int main(int argc, char **argv)
{  
    gtk_init(&argc, &argv); 
    
    //establish contact with XML code through builders and pointers
    
    builder = gtk_builder_new_from_file("Server.glade"); //the function returns an Gtkbuilder object, which parses this XML file and creates all the objects described in it ( the builder object also contains references to the different object it creates
   
   //GTK_WIDGET is just for typecasting 
   /* WELCOME SCREEN POINTER assignment */
   welcomewindow = GTK_WIDGET(gtk_builder_get_object(builder, "StartWindow"));
   startserverbtn = GTK_WIDGET(gtk_builder_get_object(builder, "startserverbtn"));
   serversettings = GTK_WIDGET(gtk_builder_get_object(builder, "serversettings"));
   WelcomeLabel = GTK_WIDGET(gtk_builder_get_object(builder, "welcomelabel"));
   g_signal_connect(welcomewindow,"destroy",G_CALLBACK(gtk_main_quit),NULL); //G_CALLBACK is just a caste (pointer)  gtk_main_quit is the function to be called when the window is closed (when we click cross button)
   
   
   /* SERVER SCREEN POINTER assignment */
   serverscreen = GTK_WIDGET(gtk_builder_get_object(builder, "serverscreen"));
   messageopen = GTK_WIDGET(gtk_builder_get_object(builder,"messageopen"));
   serverstatus = GTK_WIDGET(gtk_builder_get_object(builder, "serverstatus"));
   monitorview = GTK_WIDGET(gtk_builder_get_object(builder, "monitorview"));
   monitorbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(monitorview));
   g_signal_connect(serverscreen,"destroy",G_CALLBACK(gtk_main_quit),NULL);
     
   /*Settings window pointer assignment */
   settingswindow = GTK_WIDGET(gtk_builder_get_object(builder,"settingswindow"));
   savebutton = GTK_WIDGET(gtk_builder_get_object(builder, "savebutton"));
   passwordinput = GTK_WIDGET(gtk_builder_get_object(builder, "password"));
   censoredwordinput = GTK_WIDGET(gtk_builder_get_object(builder,"censoredwords"));
   //g_signal_connect(settingswindow,"delete-event",G_CALLBACK(gtk_main_quit),NULL); 
   
   /*chat window*/
   messagemonitor = GTK_WIDGET(gtk_builder_get_object(builder,"messagemonitor")); 
   chatview = GTK_WIDGET(gtk_builder_get_object(builder,"chatview"));
   chatbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chatview));   //to set up the texter. It returns us the buffer being used by the Text view
   
    
   gtk_builder_connect_signals(builder, NULL); 
   gtk_widget_show(welcomewindow); //tell the system to show the window
   gtk_main(); // our forever loop (watches for events, and calls the functions)
   
   
   return 0;
}


//two Functions below to disable clicking on the textview
gboolean on_monitorview_button_press_event(GtkWidget *widget, GdkEvent *event)
{
   return True;
}
on_chatview_button_press_event(GtkWidget *widget, GdkEvent *event)
{
 return True;
}
//
gboolean on_settingswindow_delete_event(GtkWidget *widget,GdkEvent  *event, gpointer   user_data)
{
   return gtk_widget_hide_on_delete(settingswindow);
}
gboolean on_messagemonitor_delete_event(GtkWidget *widget,GdkEvent  *event, gpointer   user_data)
{
   return gtk_widget_hide_on_delete(messagemonitor);
}


/* ************Message Censoring Functions************ */

// below compares the word with the list of censored words

int iscensored(char *word)
{
  for(int i = 0; i<censored_word_count; i++)
  {
     if(strcmp(word,censored_words[i]) == 0) return 1;
     
  }
  return 0;
}

char* censor(char *message)    
{
  char *new_message = (char*)calloc(1000,sizeof(char));
  strcpy(new_message,"");
  char temp[10000];
  
  
  for(int i = 0,j=0; i<=strlen(message); i++)
  {
     if(message[i] == ' '|| message[i]== '\0'|| message[i] == '?' || message[i] == ',')
     { 
       
        temp[j] = '\0';
        j = 0;
       
        if(iscensored(temp))
        {
           strcat(new_message, "*CENSORED* ");
           
        }
        else
        {
          strcat(new_message, temp);
         
          strcat(new_message, " ");
      
        }
        strcpy(temp,"");
     }
     else
       {
        temp[j++] = message[i];
       }
  }
  
  return new_message;
}
/* ******************************************* */

// Functions to Handle Client and Start server are below
void send_msg_all_clients(char * client_message)
{
  pthread_mutex_lock(&mutex);
  for(Node* temp = head; temp!=NULL; temp = temp->next)
  {
      if(send(temp->data.socket,client_message,strlen(client_message),0) < 0)
        {
           perror("Sending Error");
        }
        
  }
  free(client_message);
  pthread_mutex_unlock(&mutex);
}

void* client_handler(void * client_soc)
{ 
   int sock = *(int*)client_soc;
   
   char client_message[2560];
   int len;  
   
   while((len = recv(sock,client_message,256,0))>0)
   {   
      client_message[len] = '\0';
      char* new_message= censor(client_message);
      gtk_text_buffer_insert_at_cursor(chatbuffer, client_message, strlen(client_message));
      send_msg_all_clients(new_message);
      
   }
      char * leave_message = (char*)calloc(100,sizeof(char));
      pthread_mutex_lock(&mutex); 
      deletenode(head,sock,leave_message);
      pthread_mutex_unlock(&mutex);  
      send_msg_all_clients(leave_message);
      
      
      
   printf("thread ended. len: %d \n",len);
   
}

//Below Function is to the check if entered username on clientside is unique or not
int isunique(char *name)
{   
     
   pthread_mutex_lock(&mutex);  
   for(Node* temp = head; temp!=NULL; temp = temp->next)
   {  
      
      if(strcmp(temp->data.name,name) == 0)
      {
        pthread_mutex_unlock(&mutex);  
        return 0;
      } 
   }
    pthread_mutex_unlock(&mutex);  
    return 1;
}

//Below Function to start the server and seek connections from the client
void* start_server(void* arg)
{
   int server_socket = socket(AF_INET, SOCK_STREAM, 0);
   int flag = 1;
   //creating server adress details
   struct sockaddr_in server_address; 
     server_address.sin_family = AF_INET;
     server_address.sin_port = htons(9002); //host to network 
     server_address.sin_addr.s_addr =  INADDR_ANY;
     
   if(bind(server_socket, (struct sockaddr*) &server_address,sizeof(server_address))<0)
   {
      perror("Binding Error: ");
      flag = 0;

   }
   if(listen(server_socket,10)<0)
   {
      perror("Listening Error: ");
      flag = 0;
   }
   if(flag == 1)
   {
     printf("Server has Started! \n ");
     gtk_label_set_text(GTK_LABEL(serverstatus),"ONLINE");
   }
   
   while(1)
   {   
       int client_socket = accept(server_socket, NULL, NULL);
       printf("connection established \n");
       char client_name[1000];
       char user_password[1000];
       int pass_len;
       int name_len;
       //password verification
        pass_len = recv(client_socket, user_password,1000,0);
        user_password[pass_len] = '\0';
        if(strcmp(user_password,"DONE") !=0)
         {
            if(strcmp(user_password,server_pass) == 0)
              {
                 send(client_socket, "SUCCESS",strlen("SUCCESS"),0);
                 
              }
            else
              { 
                 send(client_socket, "FAILURE",strlen("FAILURE"),0);
                 continue;
              }

            name_len = recv(client_socket, client_name, 1000,0);
            client_name[name_len] = '\0';
           
            
            if(isunique(client_name))
            {  
               send(client_socket, "SUCCESS",strlen("SUCCESS"),0); 
               continue;
            }
            else
            {
              send(client_socket, "FAILURE",strlen("FAILURE"),0);
              continue;
              
            }
              
          }
          else
          { 
            name_len = recv(client_socket, client_name, 1000,0);
            client_name[name_len] = '\0';  
            
          }
              
      
       //*****adding client details in the linked list ***** \\
       pthread_mutex_lock(&mutex);
      pthread_mutex_unlock(&mutex);
       
       
       //GUI modifications when a new user joins
       char * display_message = (char*)calloc(100,sizeof(char));
       strcpy(display_message, client_name);
       strcat(display_message,"   h a s  j o i n e d   t h e  c h a t  ! \n \n");
       gtk_text_buffer_insert_at_cursor(monitorbuffer,display_message, strlen(display_message));
       send_msg_all_clients(display_message);
       
            
       if(client_socket<0)
       {
          perror("Accept Error: ");
       }
      
       
       //creation of thread to handle new user (each thread for each user)
       pthread_t tid1;
       pthread_attr_t attr;
       pthread_attr_init(&attr);
       pthread_create(&tid1, &attr, client_handler, &client_socket); 
       
       
   }
   
}

void on_messageopen_clicked(GtkButton *b)
{
   gtk_widget_show(messagemonitor);
}

// ************** All functions of the Start Screen + server settings screen of the Program are written below **************
  
void on_startserverbtn_clicked(GtkButton *b)
{
 gtk_widget_hide(welcomewindow);
 gtk_widget_show(serverscreen);
 
 // creating a thread which will create a new server and will actively seek for connections in the background
 pthread_t tid1;
 pthread_attr_t attr;
 pthread_attr_init(&attr);
 pthread_create(&tid1, &attr, start_server, NULL); 
 
}

void on_serversettings_clicked(GtkButton *b)
{
   gtk_widget_show(settingswindow);
}
//Below function to check if the password entered on Server Settings window is empty or not

int passisempty(char *pass)  //returns true if password is empty
{
   for(int i = 0; i<strlen(pass); i++)
   {
      if(pass[i] !=' ') return 0;
   }
   return 1;
}
//sets the entered server password on server settings window as the password

void set_password()
{  
   if(passisempty((char*)gtk_entry_get_text(passwordinput)) == 0) //checks if  password is non empty
     {
        strcpy(server_pass,(const char*)gtk_entry_get_text(passwordinput));
        
     }
     else
     {
        strcpy(server_pass, "VIIT1234$");
     }    

}
void set_censored_words()
{   
    censored_word_count = 0;
    char censored_string[1000];
    strcpy(censored_string, (const char*)gtk_entry_get_text(censoredwordinput));
    
    for(int i = 0,j = 0,word_count = 0; i<=strlen(censored_string); i++)
    { 
      if(censored_string[i] == ',' || censored_string[i] == ' ' || censored_string[i] == '\0' )
        {
           censored_words[word_count++][j] = '\0';
           j = 0;
           censored_word_count++;
        }
      else
      {
         censored_words[word_count][j++] = censored_string[i];
      }  
      
    }
    
    
}


void on_savebutton_clicked(GtkButton *b)
{
   set_censored_words();
   set_password();
   gtk_widget_hide(settingswindow);
    
}

  
  
