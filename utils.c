#include "utils.h"

char * get_key_value_string(char* key, char* value){
		/* Converts a key value into string key:value */

		char * key_val = (char*)malloc(1000);
		strcpy(key_val,""); //clears the string

		strcat(key_val,"\"");
		strcat(key_val,key);
		strcat(key_val,"\"");
		strcat(key_val," : ");

		strcat(key_val, "\"");
		strcat(key_val,value);
		strcat(key_val, "\"");

		return key_val;
		

}
int get_size(char** keys, char** values, int size){
	
	int json_size = 0;
	for(int i = 0; i<size; i++){
		json_size += sizeof(keys[i]) + sizeof(values[i]);
	}
	return json_size;
}

char* json_stringify(char** keys, char **values,int size){
	
	int json_size = get_size(keys,values,size);

	char* json_string = (char*)malloc(4*json_size);
	strcat(json_string,"{");

	for(int i = 0; i<size; ++i){

		char * key_val = get_key_value_string(keys[i],values[i]);
		strcat(json_string,key_val);
		free(key_val);

		if( i != size-1){
			strcat(json_string,",");
		}
	}
	strcat(json_string,"}");
	return json_string;

}


/* PASSWORD RELATED UTILITY FUNCTIONS */
/*===============================================================================*/

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


