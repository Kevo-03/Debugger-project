

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 100 
#define LINE_LENGTH 80 

struct buf {
char line[80]; 
int link; 
};
struct buf Buffer[100];
int head = 0; 
int row, col;
int current_line = 0;
char Tr_var[20];
int nTrvar = 0;

#define BLANK_LINE_LINK 99 
#define LAST_LINE_LINK -1 


void read_to_buffer(const char* filename)
{
    FILE *fptr;
    fptr = fopen(filename,"r");

    for(int i = 0;i < 100;i++)
    {
        if(fgets(Buffer[i].line,80,fptr)) 
        Buffer[i].link = i + 1; 
        else
        Buffer[i].link = BLANK_LINE_LINK; 
        
    }
    
    for(int i = 0;i < 100;i++)
    {
        if(Buffer[i].link == BLANK_LINE_LINK)
        {
            Buffer[i-1].link = LAST_LINE_LINK;
            break;
        }
    }

    fclose(fptr);
}

void add_to_buffer(int index,const char* directive)
{
   
   
     
    int j = head;
    while(Buffer[j].link != BLANK_LINE_LINK) 
    {
        j++;
    }
  
    
    int prev = head;
    for(int check = 1; check < index - 1; check++)
    {
        prev = Buffer[prev].link;
    }

    
    int spaces = 0;
    while(Buffer[prev].line[spaces] == ' ' || Buffer[prev].line[spaces] == '\t')
    {
        spaces++;
    }
    char dir_w_spaces[80];
    snprintf(dir_w_spaces,80,"%*s%s",spaces," ",directive); 
    
    struct buf newBuf;
    strcpy(newBuf.line, dir_w_spaces);
    newBuf.link = Buffer[prev].link;

    
    Buffer[prev].link = j;
    Buffer[j] = newBuf; 
    
   
}

void write_to_file(const char* filename)
{
    FILE *fptr;
    fptr = fopen(filename,"w");

   
    for(int i = head;Buffer[i].link != BLANK_LINE_LINK;i = Buffer[i].link)
    {
        fprintf(fptr,"%s",Buffer[i].line);
        if(Buffer[i].link == LAST_LINE_LINK) 
        break;
    }

    fclose(fptr);
}

int main(void) {
    read_to_buffer("myprog.c"); 
    initscr(); 
    cbreak();  
    keypad(stdscr, TRUE);
    noecho();
    int r = 1;
    for (int i = head; Buffer[i].link != BLANK_LINE_LINK; i = Buffer[i].link) 
    {
               
                for (int a = 0; Buffer[i].line[a] != '\0'; a++) 
                {
                    mvprintw(r, a, "%c", Buffer[i].line[a]);
                }
               
                r++;
                if (Buffer[i].link == LAST_LINE_LINK) 
                {
                    break;

                }
    }
    refresh(); 
    int ch;
    char directive[LINE_LENGTH];
    while ((ch = getch()) != 'x') 
    {
        switch (ch) 
        {
            case 'd':
                clear(); 
                int j = 1;
               

                for (int i = head; Buffer[i].link != BLANK_LINE_LINK; i = Buffer[i].link) 
                {
                    
                    for (int a = 0; Buffer[i].line[a] != '\0'; a++) 
                    {
                        mvprintw(j, a, "%c", Buffer[i].line[a]);
                    }
                    
                    j++;
                    if (Buffer[i].link == LAST_LINE_LINK) 
                    {
                        break;

                    }
                }
                refresh(); 
                break;
            case 'w':
                if (current_line > 0) current_line--;
                break;
            case 's':
                if (current_line < MAX_LINES - 1) current_line++;
                break;
            case 'i':
                echo();
                getyx(stdscr, row, col); 
                mvprintw(LINES - 1, 0, "Enter directive: ");
                getnstr(directive, LINE_LENGTH - 1); 
                strcat(directive,"\n");
                add_to_buffer(current_line, directive); 

                clear(); 
             
                refresh(); 
                noecho();
                break;
        }
        move(current_line % LINES, 0); 
        refresh();
        
    }




    endwin();
    write_to_file("temp.txt");
    read_to_buffer("temp.txt");
    
   

    int index = 0;
    for(int i = head; Buffer[i].link != BLANK_LINE_LINK;i = Buffer[i].link)
    {
       index++;
       if(strstr(Buffer[i].line,"@var") != NULL)
       {
            char temp[80];
            char *pos;
            char *variable;
            int index;

            pos = strstr(Buffer[i].line,"@var");
            variable = pos + strlen("@var");
            variable++;
            Tr_var[nTrvar] = variable[0];
            ++nTrvar;
            index = pos - Buffer[i].line;
            strncpy(temp, Buffer[i].line, index);
            temp[index] = '\0';
            
            strcat(temp,"add_TT('");
            strncat(temp,variable,1);
            strcat(temp,"');");
            strcat(temp,variable + 1);
            strcpy(Buffer[i].line,temp);
       }
       else if(strstr(Buffer[i].line,"@trace") != NULL)
       {
            char temp[80];
            char *pos;
            char *variable;
            int index;

            pos = strstr(Buffer[i].line,"@trace");
            variable = pos + strlen("@trace");
            variable++;
            index = pos - Buffer[i].line;
            strncpy(temp, Buffer[i].line, index);
            temp[index] = '\0';
            
            strcat(temp,"display_TT('");
            strncat(temp,variable,1);
            strcat(temp,"');");
            strcat(temp,variable + 1);
            strcpy(Buffer[i].line,temp);
       }
     
       else if ((strchr(Buffer[i].line, '=') != NULL) && (strstr(Buffer[i].line, "==") == NULL)) 
       {
            
            char *pos = strchr(Buffer[i].line, '=');
            pos--;
            
            for (int varIndex = 0; varIndex < nTrvar; varIndex++)
            {
                if (Tr_var[varIndex] == pos[0]) 
                {
                    
                    char updateCall[80];
                    sprintf(updateCall, "update_TT('%c', %c);", Tr_var[varIndex], Tr_var[varIndex]);
                    strcat(updateCall,"\n");

                    
                    add_to_buffer(index + 1, updateCall);
                    i = Buffer[i].link;
                    index++;

                    break;
                }
                else
                continue;
            }
        }
       else if(Buffer[i].link == LAST_LINE_LINK)
       {
            break;
       }
       else 
       continue;
    }
    for (int varIndex = 0; varIndex < nTrvar; varIndex++)
    {
    printf("%c",Tr_var[varIndex]);
    }
    for(int i = head; Buffer[i].link != BLANK_LINE_LINK;i = Buffer[i].link)
    {
        printf("%s",Buffer[i].line);
        if(Buffer[i].link == LAST_LINE_LINK)
        break;
    }
    write_to_file("expanded.c");
    return 0;
}
