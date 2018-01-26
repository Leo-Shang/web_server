#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
 // ============================================= check for command is piped ===================
bool checkPipe(char* input){
	int i = 0;
	for( ; input[i] != '\n'; i++){
		if(input[i] == '|')
			return true;
	}
	return false;
}
//=================================================== end for check ============================

int main(){
  	char input_history[10][100];									// recording histroy
  	int a, b ;
  	int input_time = 0;												// total # of commands
  	for(a=0 ;a<10;a++){												// clear out history record
  	  	for(b=0 ;b<100;b++)
  	   		input_history[a][b]='\0';
  	}

  	while(1==1){													// repeats forever
  		char input[100];											//records a single command
	  	char* cmd;
	  	int status;
  		char path[100];
      	char external_cmd[100];
      	int x=0;
      	for(;x<100;x++){
        	input[x] = '\0';
        	path[x] = '\0';
        	external_cmd[x] = '\0';
      	}
      	bool isPath = false;

  		for(a=0;a<50;a++) {path[a] = '\0';}

  		printf("%s", "mshang@my_shell:~$ ");						//prompt
    	char* try_ctrl_D = fgets(input,sizeof(input),stdin);							//get command

      	if(try_ctrl_D == '\0'){
        	printf("\n");
        	exit(0);
      	}
    	if(strcmp(input,"exit\n") == 0){							// test for exit()
    		exit(0);
	    }

	    if(input_time<10){											// record a command into history
		  	for(b=0;input[b]!='\n';b++)
		    	input_history[input_time][b] = input[b];
		}
  		else{														// overwrite the oldest history
  			int overwrite = input_time % 10;
  			for(b=0;b<100;b++) {input_history[overwrite][b] = '\0';}
  			for(b=0;input[b]!='\n';b++)
  				input_history[overwrite][b] = input[b];
  		}
  		input_time = input_time + 1;

      	bool isPipe = false;
		isPipe = checkPipe(input);

// ================================================= if the command is piped =============================================
    	if(isPipe){
    		char cmd_table[100][100];
    		char path_table[100][100];
    		char piped_input_table[100][100];
    		char third_option[100][100];
    		int third_option_array[100];
    		int isPath_array[100];

			for(a=0;a<100;a++)
				for(b=0;b<100;b++){
					cmd_table[a][b] = '\0';
					path_table[a][b] = '\0';
					piped_input_table[a][b] = '\0';
					third_option[a][b] = '\0';
					third_option_array[b] = '\0';
					isPath_array[b] = '\0';
				}

    		int row, col, counter = 0;
			int validRows = 0;

    		for(row = 0; row < 100; row++){
    			for(col = 0; col < 100 && input[counter] != '|' && counter < 100; col++){
    				piped_input_table[row][col] = input[counter];
    				counter++;
    			}
				if(input[counter] == '|') validRows += 1;
    			counter = counter + 1;
    		}
			validRows += 1;

			for(row = 0; row< 100; row++){
				if(piped_input_table[row][0] == ' ')
					for(col = 0; col < 99; col++){
						piped_input_table[row][col] = piped_input_table[row][col+1];
					}
			}

    		for(row = 0; row < 100; row++){
    			for(col = 0; col < 100 && piped_input_table[row][col] != ' ' && piped_input_table[row][col] != '\n' && piped_input_table[row][col] != '\0'; col++){
					cmd_table[row][col] = piped_input_table[row][col];
				}
				col += 1;
          		if(piped_input_table[row][col] == '\0') {continue;}
          		else{
            		isPath_array[row] = 1;
  					int path_table_col = 0;
  					for( ; col < 100 && piped_input_table[row][col] != ' ' && piped_input_table[row][col] != '\n' && piped_input_table[row][col] != '\0'; col++,path_table_col++){
  						path_table[row][path_table_col] = piped_input_table[row][col];
  					}
          		}
          		col += 1;
          		if(piped_input_table[row][col] == '\0') {continue;}
          		else{
          			third_option_array[row] = 1;
          			int third_opt_col = 0;
          			for( ;col < 100 && piped_input_table[row][col] != ' ' && piped_input_table[row][col] != '\n' && piped_input_table[row][col] != '\0'; col++,third_opt_col++){
          				third_option[row][third_opt_col] = piped_input_table[row][col];
          			}
          		}
			}

			int read_end;
			for(row = 0; row < validRows ; row++){
				int pipes[2];
				if(row != validRows-1){
					pipe(pipes);
				}
				
				int pid;
				int pidStatus;
				pid = fork();
				if(pid < 0) {printf("%s\n", "Fork Failed");}
				if(pid == 0){
					if(row == 0){
						close(STDOUT_FILENO);
						close(pipes[0]);
						dup2(pipes[1], STDOUT_FILENO);						
						if(third_option_array[row] == 1){
							execlp(cmd_table[row], "cmd", path_table[row], third_option[row], NULL);
						}
						else if(isPath_array[row] == 1){
							execlp(cmd_table[row], "cmd", path_table[row], NULL);
						}
						else
							execlp(cmd_table[row],"cmd", NULL);
					}
					else if(row == validRows-1){
						close(STDIN_FILENO);
						dup2(read_end, STDIN_FILENO);
						if(third_option_array[row] == 1){
							execlp(cmd_table[row], "cmd", path_table[row], third_option[row], NULL);
						}
						else if(isPath_array[row] == 1){
							execlp(cmd_table[row], "cmd", path_table[row], NULL);
						}
						else
							execlp(cmd_table[row],"cmd", NULL);
					}
					else{
						close(STDOUT_FILENO);
						dup2(read_end, STDIN_FILENO);
						close(pipes[0]);
						dup2(pipes[1],STDOUT_FILENO);
						if(third_option_array[row] == 1){
							execlp(cmd_table[row], "cmd", path_table[row], third_option[row], NULL);
						}
						else if(isPath_array[row] == 1){
							execlp(cmd_table[row], "cmd", path_table[row], NULL);
						}
						else
							execlp(cmd_table[row],"cmd", NULL);
					}
				}
				else{
					while(wait(&status)!=pid);
					read_end = pipes[0];
					close(pipes[1]);
				}
			}
      }
//=================================== end of piped section ===========================================================

//================================== else (command is not piped) ======================================================
      else{
      	int i = 0;
      	while(input[i] != ' ' && input[i] != '\n'){i++;}
      	i++;

        if(input[i] != '\0') {isPath = true;}
      	int j = 0;
      	for( ; input[i] != '\0'&& input[i] != '\n'; i++,j++){
      		path[j] = input[i];										// get the directory that cd to
      	}
        cmd = strtok(input,"\n");
      	cmd = strtok(input," ");
        char external_cmd[100];
  		bool didInternal = false;
  		bool didExternal = false;

  		if(strcmp(cmd,"pwd") == 0){						// command is pwd
      	char buf[500];
	      char* working_dir = getcwd(buf, sizeof(buf));
	      printf("%s\n", working_dir);
	      didInternal = true;
		}
		if(strcmp(cmd,"history") == 0){					// command is history
		    if(input_time<10){
		      int roll = 0;
		      int counter = 1;
		      for(;roll<input_time;roll++,counter++){
		        printf("%d", counter);
		        printf(": %s\n", input_history[roll]);
		    }
		    didInternal = true;
		}
		    else{
		      int roll = input_time%10;
		      int counter = 1;
		      for(;roll<10;roll++,counter++){
		        printf("%d", counter);
		        printf(": %s\n", input_history[roll]);
		      }
		      for(roll = 0; roll<input_time%10;roll++,counter++){
		        printf("%d", counter);
		        printf(": %s\n", input_history[roll]);
		      }
		      didInternal = true;
		    }
		  }
		  if(strcmp(cmd,"cd") == 0){									// command is cd
		    int ret;
		    ret = chdir(path);
		    if(ret != 0){
		      printf("%s\n", "No such file or directory");
		    }
		    didInternal = true;
		  }
		  else{
		    strcpy(external_cmd,"/bin/");
		    strcat(external_cmd,cmd);
		  }

		  pid_t pid;
		  pid = fork();
		  if(pid < 0){	
		    printf("%s\n", "Fork Failed");
		  }
		  else if(pid == 0){
		    int exeResult;
		    if(isPath == true && didInternal == false){
		      exeResult = execlp(external_cmd,"cmd",path,NULL);
		      didExternal = true;
		    }
		    if(isPath == false && didInternal == false){
		      exeResult = execlp(external_cmd,"cmd",NULL);
		      didExternal = true;
		    }
		    if(didExternal == false && didInternal == false || exeResult == -1){
		      printf("COMMAND ERROR\n" );
		    }
		    exit(0);
		  }
		  else{
		    while(wait(&status)!=pid);
		  }
		}
	}
	return 0;
}
