#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int readFile_findMazeSize (char *fileName);
void readFile_createMaze (char *fileName, int** maze);
int move(int** maze, int matrix_size, int i, int j, char* path, int path_counter, int* key, int key_counter, int* exit_flag, FILE *outputFile);



/* Main Function */
int main (int argc, char **argv)
{
    char *input = argv[1];
    int c, i, t, start_point, matrix_size = 0;
	char path[2000];
	int key[26];
	int path_counter = 0;
	int key_counter = 0;	
	int exit = 0;
	int *exit_flag = &exit;
	FILE *outputFile;
	
	outputFile = fopen("program.txt", "w");
	if(outputFile == NULL)
	{
    	printf("Error!");
    	exit ;
	}
	fprintf(outputFile,"Start ");
	
		    
    /* Function is going to find the matrix size */
    matrix_size = readFile_findMazeSize (argv[1]);
    
    /* reserve matrix space */
	int ** maze = (int**) malloc (sizeof (int*) * matrix_size);
	for (i=0; i<matrix_size; i++)
	{
		maze[i] = (int*) malloc (sizeof (int) * matrix_size);
	}

	/* initialize matrix */
	readFile_createMaze(argv[1], maze);
		
	for (i=0; i<matrix_size; i++)
	{
		if (maze[0][i] == 83)		/* search start point 'S' (ASCII 83 = S)*/
		{
			start_point = i;
		}
	}
		
	/* starts recursion with starting point 'S' */	
	move (maze, matrix_size, 0, start_point, path, path_counter, key, key_counter, exit_flag, outputFile);
	
	
	fclose(outputFile);
	
	/* free to memory */	
	for (t = 0; t < matrix_size; t++)
	{
		free(maze[t]);
	}  		 
	free (maze);
	
	
    return 0;
}



int move(int** maze, int matrix_size, int i, int j, char* path, int path_counter, int* key, int key_counter, int *exit_flag, FILE *outputFile)
{	
	/* exit_flag will be'1', if exit is found otherwise search the exit way */
	if (! *exit_flag)
	{		
		int k = 0;
		int t;
		
		if ( maze[i][j] == 69 ) /*ASCII 'E'*/
		{			
			for(t=0; t<path_counter; t++)	/* writing to .txt file the path which goes to the Exit!! */
			{
				fprintf(outputFile,"%c ", path[t]);
			}
			fprintf(outputFile,"Exit");
			*exit_flag = 1;
			path_counter--;
			return *exit_flag;
		}
		
		if ( islower(maze[i][j]) )	/* it checks to lower character or not */
		{		
			key[key_counter] = maze[i][j];	/* it saves the key on key[] array for open the gates */
			key_counter ++;
					
			for(t=0; t<path_counter; t++)	/* writing to txt file the path which goes to the key then reset to path_counter value */
			{
				fprintf(outputFile,"%c ", path[t]);
			}
			fprintf(outputFile,"%c ", maze[i][j]);
			
			maze[i][j] = 48/*ASCII '0'*/;
			path_counter = 0; 
		}
	
		/* if statement for going to down side */
		if ( (i+1 != matrix_size) && (maze[i+1][j] != 49/*ASCII '1'*/) && (path[path_counter-1] != 78/*ASCII 'N'*/))
		/* if stataments checks lower border of matrix, closed way of maze and previous move for vicious circle */
		{
			if ( (islower(maze[i+1][j])) || (maze[i+1][j] == 69/*ASCII 'E'*/) || (maze[i+1][j] == 48/*ASCII '0'*/)) 
			{
				path[path_counter] = 'S';	/* Adding path the side which is gone */
				path_counter ++;
				move (maze, matrix_size, i+1, j, path, path_counter, key, key_counter, exit_flag, outputFile);		/*** RECURSION! ***/
				path_counter--;
			
				if(*exit_flag == 1)
				{
					return *exit_flag;
				}
			}
			
			/* if next step will be big character(door), check key[] array for key. If key is taken, move through the door */
			else if( (isupper(maze[i+1][j])) )
			{
				for (k=0; k<=key_counter; k++){
					if ( key[k] == tolower(maze[i+1][j]) )
					{
						path[path_counter] = 'S';	/* Adding path the side which is gone */
						//path_counter ++;
						path[path_counter +1] = maze[i+1][j];
						path_counter += 2;
						int tempPoint = maze[i][j];		/* after pass the key, change the door with '0' for prevent a vicious circle between the doors */
						maze[i][j] = 49 ;
						move (maze, matrix_size, i+1, j, path, path_counter, key, key_counter, exit_flag, outputFile);		/*** RECURSION! ***/
						maze[i][j] = tempPoint;		/* if could not find an exit way, fix the door value in matrix and delete the direction from the path[] array */
						path_counter -= 2;								
						if(*exit_flag ==1)
						{
							return *exit_flag;
						}
					} 
				}
			}
		}

		/* if statement for going to right side */	
		if ( (j+1 != matrix_size) && (maze[i][j+1] != 49) && (path[path_counter-1] != 87/*ASCII 'W'*/) )
		/* if stataments checks right border of matrix, closed way of maze and previous move for vicious circle */
		{
			if ( (islower(maze[i][j+1])) || (maze[i][j+1] == 69/*ASCII 'E'*/) || (maze[i][j+1] == 48/*ASCII '0'*/))
			{
				path[path_counter] = 'E';	/* Adding path the side which is gone */		
				path_counter ++;
				move (maze, matrix_size, i, j+1, path, path_counter, key, key_counter, exit_flag, outputFile);		/*** RECURSION! ***/
				path_counter--;
				if(*exit_flag ==1)
				{
					return *exit_flag;
				}
			}

			/* if next step will be big character(door), check key[] array for key. If key is taken, move through the door */
			else if( (isupper(maze[i][j+1])) )
			{
				for (k=0; k<=key_counter; k++){
					if ( key[k] == tolower(maze[i][j+1]) )
					{
						path[path_counter] = 'E';	/* Adding path the side which is gone */
						//path_counter ++;
						path[path_counter + 1] = maze[i][j+1];
						path_counter +=2;
						int tempPoint = maze[i][j];		/* after pass the key, change the door with '0' for prevent a vicious circle between the doors */
						maze[i][j] = 49 ;
						move (maze, matrix_size, i, j+1, path, path_counter, key, key_counter, exit_flag, outputFile);		/*** RECURSION! ***/
						maze[i][j] = tempPoint;		/* if could not find an exit way, fix the door value in matrix and delete the direction from the path[] array */
						path_counter -=2;								
						if(*exit_flag ==1)
						{
							return *exit_flag;
						}
					} 
				}
			}		
		}
	
		/* if statement for going to up side */
		if ( (i-1 != -1) && (maze[i-1][j] != 49) && (path[path_counter-1] != 83/*ASCII 'S'*/) )
		/* if stataments checks upper border of matrix, closed way of maze and previous move for vicious circle */	
		{
			if ( (islower(maze[i-1][j])) || (maze[i-1][j] == 69)/*ASCII 'E'*/ || (maze[i-1][j] == 48)/*ASCII '0'*/ )
			{
				path[path_counter] = 'N';	/* Adding path the side which is gone */
				path_counter ++;
				move (maze, matrix_size, i-1, j, path, path_counter, key, key_counter, exit_flag, outputFile);		/*** RECURSION! ***/
				path_counter--;
				if(*exit_flag ==1)
				{
					return *exit_flag;
				}
			}
			
			/* if next step will be big character(door), check key[] array for key. If key is taken, move through the door */
			else if( (isupper(maze[i-1][j])) )
			{
				for (k=0; k<=key_counter; k++){
					if ( key[k] == tolower(maze[i-1][j]) )
					{
						path[path_counter] = 'N';	/* Adding path the side which is gone */
						//path_counter ++;
						path[path_counter + 1] = maze[i-1][j];
						path_counter +=2;
						int tempPoint = maze[i][j];		/* after pass the key, change the door with '0' for prevent a vicious circle between the doors */
						maze[i][j] = 49 ;						
						move (maze, matrix_size, i-1, j, path, path_counter, key, key_counter, exit_flag, outputFile);		/*** RECURSION! ***/
						maze[i][j] = tempPoint;		/* if could not find an exit way, fix the door value in matrix and delete the direction from the path[] array */
						path_counter -=2;								
						if(*exit_flag ==1)
						{
							return *exit_flag;
						}
					} 
				}
			}
		}
		
		/* if statement for going to left side */
		if ( (j-1 != -1) && (maze[i][j-1] != 49) && (path[path_counter-1] != 69/*ASCII 'E'*/) )
		/* if stataments checks left border of matrix, closed way of maze and previous move for vicious circle */
		{
			if ( (islower(maze[i][j-1])) || (maze[i][j-1] == 69/*ASCII 'E'*/) || (maze[i][j-1] == 48/*ASCII '0'*/))
			{
				path[path_counter] = 'W';	/* Adding path the side which is gone */
				path_counter ++;
				move (maze, matrix_size, i, j-1, path, path_counter, key, key_counter, exit_flag, outputFile);		/*** RECURSION! ***/
				path_counter--;
				if(*exit_flag ==1)
				{
					return *exit_flag;
				}
			}
			
			/* if next step will be big character(door), check key[] array for key. If key is taken, move through the door */
			else if( (isupper(maze[i][j-1])) )
			{
				for (k=0; k<=key_counter; k++){
					if ( key[k] == tolower(maze[i][j-1]) )
					{
						path[path_counter] = 'W';	/* Adding path the side which is gone */
						//path_counter ++;
						path[path_counter + 1] = maze[i][j-1];
						path_counter +=2;
						int tempPoint = maze[i][j];		/* after pass the key, change the door with '0' for prevent a vicious circle between the doors */
						maze[i][j] = 49 ;						
						move (maze, matrix_size, i, j-1, path, path_counter, key, key_counter, exit_flag, outputFile);		/*** RECURSION! ***/
						maze[i][j] = tempPoint;		/* if could not find an exit way, fix the door value in matrix and delete the direction from the path[] array */
						path_counter -=2;								
						if(*exit_flag ==1)
						{
							return *exit_flag;
						}
					} 
				}
			}					
		}		
	}
}




void readFile_createMaze (char *fileName, int** maze)
{
	int c;
	FILE *input_file;

    input_file = fopen(fileName, "r");

    if (input_file == 0)
    {
        /* fopen returns 0, the NULL pointer, on failure */
        perror("Canot open input file\n");
        exit(-1);
    }
    else
    {
        int found_word = 0;
        int i, j;
        
        found_word = 0;
		i = 0;
		j = 0;
        while ((c =fgetc(input_file)) != EOF )
        {            
            if (isalpha(c) || isalnum(c))
            {
                found_word = 1;
                maze[i][j] = c;
                j++;
            }
            else {
                if (found_word) {
                    i++;
                    j=0;
                    found_word = 0;
                }
            }

        }
	}
	fclose(input_file);
}


int readFile_findMazeSize (char *fileName)
{
    int c, matrix_size = 0;
	FILE *input_file;

    input_file = fopen(fileName, "r");

    if (input_file == 0)
    {
        /* fopen returns 0, the NULL pointer, on failure */
        perror("Canot open input file\n");
        exit(-1);
    }
    else
    {
    	
        int found_word = 0;
        while ((c =fgetc(input_file)) != EOF )
        {            
            if (isalpha(c) || isalnum(c))
            {
                found_word = 1;
                matrix_size ++;
            }
            else {
                if (found_word) {
                    found_word = 0;
                    break;
                }
        	}
        }
	}	
	fclose(input_file);	
	
	return matrix_size;
}
