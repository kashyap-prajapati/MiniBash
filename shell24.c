#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

int backpid[2048];
int backcount=0;

#define MAX 5

void validate(int length){
	if(length>MAX){
		printf("Too many commands or arguments \n");
		exit(EXIT_FAILURE);
	}
}


// Execute concat command and display result to standard output
// takes two arguments
// 1. files => number of files
// 2. count => count of files
void concat(char *files[], int count){
	for(int i=0;i<count;i++){
		char *file = strtok(files[i]," "); 
		// open file in readonly mode
		int fd = open(file,O_RDONLY);
		if(fd==-1){
			printf("No such file %s \n",files[i]);
			continue;
		}
		char buffer[1];
		int n;
		// read character byte by byte
		while((n=read(fd,buffer,sizeof(buffer)))>0){
			// write character byte by bute
			if(write(1,buffer,1)==-1){
				continue;
			}
		}
		// close file
		close(fd);
	}
}

// Execute command, takes one character array of character pointers
int process(char *task[]){
	// create a child process
	int child=fork();
	if(child==-1){
		printf("Not able to create child a process\n");
		exit(EXIT_FAILURE);
	}else if(child==0){
		// execute command
		if(execvp(task[0],task)==-1){
			printf("Error while executing command %s using execvp\n",task[0]);
			exit(-1);
		}
		exit(0);
	}else{
		int status;
		// wait for child process to complete its execution
		waitpid(child,&status,0);
		if(WIFEXITED(status) && WEXITSTATUS(status)==0){
			return 0;
		}
		return -1;
	}

}

// execute pipe command one by one
// takes 3 arguments
// 1. command array
// 2. inputfd : input file descriptor
// 3. outputfd : output file descriptor
int pipeprocess(char *task[], int inputfd, int outputfd){
	//create a child process
	int child = fork();
	if(child==-1){
		printf("Not able to create a child process \n");
		exit(EXIT_FAILURE);
	}else if(child == 0){
		// child process execution
		// check input file descriptor is not standard input
		if( inputfd != 0 ){
			// point out the standard input to the pipe read end 
			if(dup2(inputfd, 0)==-1){
				exit(EXIT_FAILURE);
			}
			// close 
			close(inputfd);
		}

		//check output file descriptor is not standard output
		if(outputfd != 1){
			// point out the standard output to the pipe write end
			if(dup2(outputfd, 1)==-1){
				exit(EXIT_FAILURE);
			}
			// close
			close(outputfd);
		}
		// execute command array
		if(execvp(task[0],task)==-1){
			exit(EXIT_FAILURE);
		}
	}else{
		// hold the status of child 
		int status;
		// waitfor the child to complete its execution
		waitpid(child, &status, 0);
		if(WIFEXITED(status)  && WEXITSTATUS(status)==0){
			return 0;
		}
		return -1;
	}
}

// Execute command without waiting for a child to complete its execution
void backprocess(char *task[]){
	// create a child process
	int child = fork();
	if(child==-1){
		printf("Not able to create a child process \n");
		exit(EXIT_FAILURE);
	}else if(child==0){
		// execute command in task array
		if(execvp(task[0],task)==-1){
			printf("Error while executing command %s using execvp\n",task[0]);
			exit(EXIT_FAILURE);
		}
	}else{
		backpid[backcount++]=child;
	}
}

// execute command sequentially one after another
void sequence(char *commands[],int argc){
	int i=0;
	// itergate through commnad arrays
	while(argc>i){
		// prepare argument array for each command and check validation
		char *args[6];
		char *arg= strtok(commands[i]," ");
		int j=0;
		while(arg!=NULL){
			args[j++]=arg;
			arg = strtok(NULL," ");
		}
		validate(j);
		args[j]=NULL;
		process(args);
		i++;
	}

}


//Execute each command separated by pipe 
void pipes(char *commands[],int argc){
	int i=0;
	// inputfd = read from the standard input
	int inputfd=0;
	// to hold he pipe read end and write end
	int pipearray[2];
	// iterate through each command
	while(i<argc){
		// prepare argument array for each command
		char *args[6];
		char *arg = strtok(commands[i]," ");
		int j=0;
		while(arg != NULL){
			args[j++] = arg;
			arg = strtok(NULL," ");
		}
		validate(j);
		args[j]=NULL;
		// last command or not 
		// if Yes. provide output fd as standard output
		if(i==argc-1){
			pipeprocess(args,inputfd,1);
		}else{
			// if no. pipe system call for read and write for the subsequent command
			if(pipe(pipearray)==-1){
				exit(EXIT_FAILURE);
			}
			// execute command with output fd as pipe write end
			pipeprocess(args,inputfd,pipearray[1]);
			// close pipe write end
			close(pipearray[1]);
			// change input fd to pipe read end
			inputfd = pipearray[0];
		}
		i++;
	}
}


// Execute input redirection operator and output redirection operator
void redirection(char *commands[],int argc){
	// create a child processs
	int child = fork();
	if(child==-1){
		printf(" Not able to create child process \n");
		exit(EXIT_FAILURE);
	}else if(child == 0 ){
		// child process execution
		int i=0;
		int ip=0;
		int op=0;
		char *ipc = NULL;
		char *opc = NULL;
		//iterate through each command array
		while(i<argc){
			// check command is input redirection or not
			if(strcmp(commands[i],"<")==0){
				ip=1;
				// get the file name from user input
				ipc=commands[i+1];
				commands[i]=NULL;
			// check command is output redirection or not
			}else if(strcmp(commands[i],">")==0){
				op=1;
				// get the file name from user input
				opc=commands[i+1];
				commands[i]=NULL;
			// check command is output redirection with append mode not
			}else if(strcmp(commands[i],">>")==0){
				op=2;
				// get the file name from user input
				opc=commands[i+1];
				commands[i]=NULL;
			}
			i++;
		}

		// command is input redirection
		if(ip==1){
			// open file in read only mode
			int fd = open(ipc,O_RDONLY);
			if(fd==-1){
				exit(EXIT_FAILURE);
			}
			// read standard input from the file.
			if(dup2(fd,0)==-1){
				exit(EXIT_FAILURE);
			}
			//close file
			close(fd);
		}
		// command is output redirection
		if(op==1 || op==2){
			int fd;
			if(op==2){
				// append content to the end of the file
				fd = open(opc, O_CREAT | O_WRONLY | O_APPEND,0644);
			}else{
				// truncate the file and write content to the file
				fd = open(opc, O_CREAT | O_WRONLY | O_TRUNC,0644);
			}

			if(fd==-1){
				printf("Not able to create file \n");
				exit(EXIT_FAILURE);
			}
			// make standard output to the file.
			if(dup2(fd, 1)==-1){
				exit(EXIT_FAILURE);
			}
			// close file descriptor
			close(fd);
		}
		//execute commmad usinf execvp
		int exe = execvp(commands[0],commands);
		if(exe==-1){
			exit(EXIT_FAILURE);
		}
	}else {
		int status;
		// wait for the child to complete its execution
		waitpid(child,&status,0);

		if(WIFEXITED(status) &&  WEXITSTATUS(status)!=0){
			exit(EXIT_FAILURE);
		}
	}

}

void conditional(char *input){
// execute command for conditional logic operator
	int count=0;
	char *commands[6];
	char *command ;
	// hold the last operator
	char *lop=NULL;
	// hold the last status
	int ls;
	// seperate command via &&||
	while((command = strsep(&input,"||&&"))!=NULL){
		if(*command == '\0'){
			continue;
		}
		//prepare argument array
		char *args[6];
		char *arg;
		int j=0;
		while((arg = strsep(&command," ")) != NULL){
			if(*arg=='\0'){
				continue;
			}
			args[j++]=arg;
		}
		validate(j);
		args[j]=NULL;
		// check last operator is not null or not
		if(lop!=NULL){
			// check last operator is || or not
			if(strcmp(lop,"||")==0 && ls==0){
				break;
			// check last operator is && or not
			}else if(strcmp(lop,"&&")==0 && ls!=0){
				break;
			}
		}
		// execute command
		int status = process(args);
		// update  the last operator
		if(input!=NULL){
			if(input[0]=='|'){
				lop = "||";
			}else if(input[0]=='&'){
				lop = "&&";
			}
		}
		ls = status;
		count++;
		validate(count);
	}
}

// shell utility to take input from the user execute command
void shell(char *input){
	if(strcmp(input,"newt")==0){
		int x = execlp("gnome-terminal","gnome-terminal","--","bash","-c","shell24",NULL);
		if(x==-1){
			printf("Error \n");
			exit(EXIT_FAILURE);
		}else{
			printf("Running a terminal !!\n");
		}
	}else if(strchr(input,'#')!= NULL){
		int count = 0;
		char *commands[6];
		char *command = strtok(input," ");
		while(command != NULL){
			commands[count++]=command;
			command = strtok(NULL," ");
		}
		validate(count);
		concat(commands,count);
	}else if(strchr(input,';')!=NULL){
		int count=0;
		char *commands[6];
		char *command = strtok(input,";");
		while(command != NULL){
			commands[count++]=command;
			command = strtok(NULL,";");
		}
		validate(count);
		sequence(commands, count);
	}else if(strchr(input,'|')!=NULL && strstr(input,"||")==NULL){
		int count=0;
		char *commands[6];
		char *command = strtok(input,"|");
		while(command != NULL){
			commands[count++]=command;
			command = strtok(NULL,"|");
		}
		validate(count-1);
		pipes(commands,count);
	}else if(strchr(input,'>')!=NULL || strchr(input,'<')!=NULL || strstr(input,">>")!=NULL){
		int count=0;
		char *commands[6];
		char *command = strtok(input," ");
		while(command != NULL){
			commands[count++]=command;
			command = strtok(NULL," ");
		}
		validate(count);
		redirection(commands, count);
	}
	else if(strcmp(input,"fg")==0){
		if(backcount>0){
			int status;
			waitpid(backpid[backcount-1],&status,0);
			backcount--;
		}else{
			printf("No background processes.\n ");
		}
	}
	else if(strstr(input,"&&")!=NULL || strstr(input,"||")!=NULL){
		conditional(input);
	}else if(strchr(input,'&')!=NULL){
		int count=0;
		char *commands[6];
		char *command = strtok(input," ");
		while(command != NULL){
			commands[count++]=command;
			command = strtok(NULL," ");
		}
		validate(count);
		if(strcmp(commands[count-1],"&")==0){
			commands[count-1]=NULL;
			count--;
		}
		backprocess(commands);
	}
	else{
		int count=0;
		char *commands[6];
		char *command = strtok(input," ");
		while(command != NULL){
			commands[count++]=command;
			command = strtok(NULL," ");
		}
		commands[count]=NULL;
		process(commands);
	}

}

int main(int argc,char *argv[]){
	char input[2048];
		
	while(1==1){
		int child = fork();
		if(child == -1){
			printf("Not able to create a child process \n");
			exit(EXIT_FAILURE);
		}else if(child == 0){
			while(1){
				printf("\nshell24$ ");
				fflush(stdout);
				//read input from standard input
				if(fgets(input,sizeof(input),stdin)==NULL){
					exit(EXIT_FAILURE);
				}
				input[strlen(input)-1]='\0';
				if(strlen(input)==0)
					continue;
				// execute shell method with input parameter
				shell(input);
			}
		}else{
			int status = 0;
			//wait for the child to complete its execution
			waitpid(child, &status, 0);
		}
		kill(child,SIGKILL);
	}
	return 0;
}
