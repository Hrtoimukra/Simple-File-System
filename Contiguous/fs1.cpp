#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define FILE_NAME_LENGTH 32

//Meta information for contiguous file allocation scheme

typedef struct contiguous_allocation{
    int block_num;//location to the start block 
    int file_size;//file size
    char file_name[FILE_NAME_LENGTH];//file name
} file_t;

char disk[NUM_BLOCKS][BLOCK_SIZE];//Total disk space  
file_t file_table[NUM_BLOCKS];//File table used to keep track of files created and deleted 
int num_files = 0;//current number of files


void init_file_system(){//File system initialization.Every block is marked '0' to show that it is unused
     for(int i=0;i<NUM_BLOCKS;++i)
         disk[i][0]='0';   
}

int allocate_blocks(int num_blocks){//Utility to allocate a contiguous set of blocks

    int i,j;
    int start_block=-1;
    int numd_free_blocks=0;
    bool success=false;

    i=0;

    while(i<=NUM_BLOCKS-num_blocks){//i will search for the starting  point 
          if(disk[i][0]=='0'){

            int count=0;//count the number of blocks available contiguously
            j=i;
            start_block=i;

            while(disk[j][0]=='0' && count<num_blocks){
                  count++;
                  j++;	 
            }

            if(count==num_blocks){//If there are required number of blocks available then success in allocation and break out of loop
               success=true;
               break;    
            }	
          } 	 
          i++;
    }

    if(success){//Mark those allocated blocks as unused
       i=start_block;
       while(i<start_block+num_blocks){
             disk[i][0]='1';
             i++; 
       }
       return(start_block);
    }
    else
       return(-1);      
}
/*
int create_file(char *file_name, int file_size){//Function to create a new file given the file name and it's size

    if (num_files >= NUM_BLOCKS){//Number of files can't exceed the number of blocks
        printf("Error: Maximum number of files reached\n");
        return -1;
    }

    int num_blocks=file_size/BLOCK_SIZE;//calculate the number of blocks required

    if(file_size%BLOCK_SIZE)
       num_blocks+=1; 

    int start_block = allocate_blocks(num_blocks);

    if (start_block == -1){
        printf("Error: Not enough free space on disk\n");
        return -1;
    }

    file_t new_file;//New file declaration
    new_file.block_num = start_block;//pointer to the starting block
    new_file.file_size = file_size;//Intialize the size
    strncpy(new_file.file_name, file_name, FILE_NAME_LENGTH);//copy the file name
    file_table[num_files++] = new_file;
    return num_files - 1;
}

*/


int create_file(char *file_name,char *data){//Function to create the file given the file name and data
    int file_size=strlen(data);
    int data_index=0;
    int block_index=0; 

        
    int ACTUAL_BLOCK_SIZE=BLOCK_SIZE-2;

    int num_blocks=file_size/ACTUAL_BLOCK_SIZE;//calculate the number of blocks required

    if(file_size%ACTUAL_BLOCK_SIZE)
       num_blocks+=1;   
  
    int start_block=allocate_blocks(num_blocks); 
    int block_write_index=1;

    while(data_index<file_size){
         disk[start_block+block_index][block_write_index]=data[data_index];
         data_index++;
         block_write_index++;

         if((data_index+1)%ACTUAL_BLOCK_SIZE==0){
            disk[start_block+block_index][block_write_index]='\0';
            block_index++;
            block_write_index=1;
         }          
    }

    file_t new_file;//New file declaration
    new_file.block_num = start_block;//pointer to the starting block
    new_file.file_size = file_size;//Intialize the size
    strncpy(new_file.file_name, file_name, FILE_NAME_LENGTH);//copy the file name
    file_table[num_files++] = new_file;
    return num_files - 1; 
}

int delete_file(char *file_name){//Utility to delete the file name 

    int i, j, k, found = 0;

    for (i = 0; i < num_files; i++) {

        if (strcmp(file_table[i].file_name, file_name) == 0){
            found = 1;
            int start_block=file_table[i].block_num;
            int num_blocks =file_table[i].file_size/(BLOCK_SIZE-2);

            if(file_table[i].file_size % (BLOCK_SIZE-2))
               num_blocks+=1;	  

            for(j=start_block;j<start_block+num_blocks;++j)//Mark all the blocks of the files as unused
                disk[j][0]='0';      

            for (k = i; k < num_files - 1; k++)//Remove the file from the file table
                file_table[k] = file_table[k + 1];
         
            num_files--;
            break;
        }
    }
    if (!found) {
        printf("Error: File not found\n");
        return -1;
    }
    return 0;
}

int read_file(char *file_name){//Utility to read all the blocks of the file

	int i, j, k, found = 0;

    for (i = 0; i < num_files; i++) {

        if (strcmp(file_table[i].file_name, file_name) == 0){
            found = 1;
            int start_block=file_table[i].block_num;
            int num_blocks =file_table[i].file_size/(BLOCK_SIZE-2);

            if(file_table[i].file_size % (BLOCK_SIZE-2))
               num_blocks+=1;

            printf("File Name= %s  ",file_table[i].file_name);
            printf("File size= %d bytes\n",file_table[i].file_size);   	   

            for(j=start_block;j<start_block+num_blocks;++j){
            	printf("BLOCK -->%d  CONTENTS\n",j);
                //for(k=1;k<BLOCK_SIZE;k++)
                    printf("%s",disk[j]);
                printf("\n\n\n");
             } 

            printf("\n");                
            break;
        }
    }

    if (!found) {
        printf("Error: File not found\n");
        return -1;
    }
    return 0; 
}

void update_file_block(char *file_name,int pos,char *data){//Utility to update the file.Arguments file name ,pos the position of the block it wants to write and data 

    int  found = 0; 

    for (int i = 0; i < num_files; i++){

        if (strcmp(file_table[i].file_name, file_name) == 0){

            found = 1;  	   

            int length=strlen(data);
            int start_block=file_table[i].block_num;

            int k=0;
            while(k<length){
                 disk[start_block+pos][k+1]=data[k];
                 k++; 	
            }

            disk[start_block+pos][k+1]='\0';                
            break;
        }
    }

    if(!found)
       printf("File does not exist\n"); 
}

int main(int argc,char **argv){


    time_t start;
    time_t end;

    long long opt;

    time(&start);

    init_file_system();
    create_file("First.txt","This article is about the rocket and spacecraft manufacturer. For the British art gallery, see Spacex (art gallery). redirects here. For the general topics, see Space exploration and Space technology.");

//    for(int i=0;i<100;++i)
       // update_file("First.txt",i,"I don't know");

    read_file("First.txt");

    
    create_file("Second.txt","giraffesunicyclerainbowwaterfallmountainhikingadventure The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.");
    
    read_file("Second.txt");

    update_file_block("Second.txt",16,"Modification by block");

    read_file("Second.txt");

    create_file("Third.txt","Gratitude is a powerful emotion that can have a profound impact on our lives. It involves being thankful and appreciative for the positive aspects of our lives, including the people, experiences, and opportunities that have helped us grow and thrive.When we practice gratitude, we cultivate a sense of appreciation and contentment, which can lead to increased happiness and well-being. Studies have shown that individuals who regularly practice gratitude are more optimistic, have better relationships, and experience fewer symptoms of depression and anxiety.There are many ways to practice gratitude, including keeping a gratitude journal, expressing thanks to others, and reflecting on the positive aspects of our lives. One effective method is to make a habit of listing three things we are grateful for each day. This simple exercise can help us focus on the positive aspects of our lives and cultivate a sense of abundance and joy.Gratitude is not just a personal practice; it can also have a positive impact on our relationships and communities. When we express gratitude to others, we build stronger connections and create a sense of mutual appreciation and respect. Gratitude can also inspire us to give back to others and contribute to our communities, which can create a positive cycle of generosity and kindness.In conclusion, gratitude is a powerful emotion that can enrich our lives and contribute to our overall well-being. By cultivating a sense of appreciation and thankfulness, we can create more joy, happiness, and positive connections in our lives and communities.");

    read_file("Third.txt");

    delete_file("Second.txt");

    read_file("Second.txt");
/*
       



    create_file("Second.txt",102400);
    for(int i=0;i<200;++i)
       update_file("Second.txt",i,"In state of oblivion");

    read_file("Second.txt");   


    create_file("Third.txt",204800);
    for(int i=0;i<400;++i)
        update_file("Third.txt",i,"Why am I like this !");
     
    read_file("Third.txt");
    
    delete_file("Second.txt");

    read_file("First.txt");
    read_file("Third.txt");  

    create_file("Fourth.txt",153600);

    for(int i=0;i<300;++i)
       update_file("Fourth.txt",i,"It will get Better from here !");

    read_file("Fourth.txt");


    create_file("Fifth.txt",256000);

    for(int i=0;i<500;++i)
        update_file("Fifth.txt",i,"What is life if full of care "); 

    read_file("Fifth.txt");

    time(&end);

    printf("Time Taken for the total operation = %ld",end-start);     
   
   */          
    return(0);        
}































