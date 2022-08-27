#include "utils.h"

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


