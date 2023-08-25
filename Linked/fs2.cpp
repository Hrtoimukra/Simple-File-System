#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define FILE_NAME_LENGTH 32
#define MAX_FILES 128


//Definition of a basic block.It contains data and pointer 
typedef struct basic_block{
    char data[BLOCK_SIZE];
    int next_block;
}block;

//Structure for linked allocation scheme.It is basically a file descriptor containing filename,file_size and starting block
typedef struct linked_allocation{
    char name[FILE_NAME_LENGTH];
    int file_size;
    int start_block;
}file_t;

file_t files[MAX_FILES];//File table to keep the track of opened and closed files 
block blocks[NUM_BLOCKS];//Total disk space in terms of blocks
int free_list;           //Pointer to the list of free blocks
int num_files=0;         //Present number of files

void init_file_system(){//Initiating the file system.Initially the filesystem contains the list of blocks

    for(int i = 0; i < NUM_BLOCKS; i++)
        blocks[i].next_block = i + 1;

    blocks[NUM_BLOCKS - 1].next_block = -1;
    free_list = 0;
}

int allocate_block(){//Function to allocate a free block	
    int block_num = free_list;
    free_list = blocks[block_num].next_block;
    return block_num;
}

void free_block(int block_num){ //Function to deallocate a block
    blocks[block_num].next_block = free_list;
    free_list = block_num;
}
/*

int create_file(char *name,int file_size){//Function to create a file given the file name and it's size

    if (num_files >= MAX_FILES){
        printf("Error: Maximum number of files reached\n");
        return -1;
    }

    bool success=true;
    int num_blocks=file_size/BLOCK_SIZE;

    if(file_size%BLOCK_SIZE)
       num_blocks+=1; 

    //int num_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int start_block = allocate_block();

    if (start_block == -1){
        printf("Error: Not enough free space on disk\n");
        return -1;
    }

    file_t new_file;
    new_file.start_block = start_block;
    new_file.file_size = file_size;
    strncpy(new_file.name,name, FILE_NAME_LENGTH);

    int blocks_count=1;
    int current_block=start_block;

    while(blocks_count<num_blocks){
         int next_block=allocate_block();

         if(next_block==-1){
           printf("Not Enough Disk Space\n");
           success=false;
           break; 	  
         }

         blocks[current_block].next_block=next_block;
         current_block=next_block;
         blocks_count++; 	
    } 

    if(success){
       blocks[current_block].next_block=-1; 
       files[num_files] = new_file;
       num_files++;
       return num_files - 1; 
    }

    return -1;
}
*/

int create_file(char *file_name,char *data){//Function to create a file given the file name and it's size

    if (num_files >= MAX_FILES){
        printf("Error: Maximum number of files reached\n");
        return -1;
    }

    bool success=true;
    int file_size=strlen(data);
    int num_blocks=file_size/(BLOCK_SIZE-1);

    if(file_size%(BLOCK_SIZE-1))
       num_blocks+=1; 

    //int num_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;


    int blocks_count=0;
    int start_block=allocate_block();
    int current_block=start_block;


    if(start_block==-1){
       printf("Not Enough space on the disk\n");
       return(-1); 
    }

    int data_index=0;
    int block_write_index=0;

    int next_block;

    while(data_index<file_size){

          blocks[current_block].data[block_write_index]=data[data_index];
          block_write_index++;
          data_index++;

          if((data_index+1)%(BLOCK_SIZE-1)==0 && (data_index+1)!=file_size){
             block_write_index=0; 
             int next_block=allocate_block();

             if(next_block==-1){
               printf("Not Enough disk space\n");
               success=false;
               break; 
             }

             blocks[current_block].next_block=next_block;
             current_block=next_block; 
          }
    }


    file_t new_file;
    new_file.start_block = start_block;
    new_file.file_size = file_size;
    strncpy(new_file.name,file_name, FILE_NAME_LENGTH); 

    if(success){
       blocks[current_block].next_block=-1; 
       files[num_files] = new_file;
       num_files++;
       return num_files - 1; 
    }

    return -1;
}



int delete_file(char *name) {//Function to delete the file given it's name.
    
    for(int i = 0; i < MAX_FILES; i++){
        if(strcmp(files[i].name, name) == 0){
            int current_block = files[i].start_block;
            while (current_block != -1){
                int next_block = blocks[current_block].next_block;
                free_block(current_block);
                current_block = next_block;
            }

            for(int k = i; k < num_files - 1; k++)
                files[k] = files[k + 1];

            num_files--;
           
            return i;
        }
    }
    return -1;
}

int read_file(char *name){ //Funnction to read the content of a file given it's name
    
    for(int i=0;i<num_files;++i){
       if(strcmp(files[i].name,name)==0){
          int current_block=files[i].start_block;

          printf("File =%s  size=%d \n",name,files[i].file_size);

          while(current_block!=-1){
                printf("Block number =%d  Data =%s\n\n\n",current_block,blocks[current_block].data);
                current_block=blocks[current_block].next_block;
          } 
          printf("\n\n");
          return(i);
       } 	
    }
    
    return(-1); 
}

int update_file_block(char *name,int pos,char *data){//Function to update the file given the file name ,position of the block and data
     
    for(int i=0;i<num_files;++i){

        if(strcmp(files[i].name,name)==0){
  
           int current_block=files[i].start_block;

           if(pos>0){
             for(int i=0;i<pos;++i)
                 current_block=blocks[current_block].next_block; 
           } 

           int length=strlen(data);
           int k=0;

           while(k<length){
                 blocks[current_block].data[k]=data[k];
                 k++; 	
           }

           blocks[current_block].data[k]='\0';
           return(i);                   
        }  	 
    } 

    return(-1);    
}

int main(int argc,char **argv){
     
    init_file_system();
    create_file("First.txt","This article is about the rocket and spacecraft manufacturer. For the British art gallery, see Spacex (art gallery). redirects here. For the general topics, see Space exploration and Space technology.");


    read_file("First.txt");

    
    create_file("Second.txt","giraffesunicyclerainbowwaterfallmountainhikingadventure The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.The sky is a deep shade of blue the sun shines brightly overhead the grass rustles in the gentle breeze birds chirp and sing as they flit from tree to tree people walk and talk in the bustling streets cars honk and engines roar as they speed by children laugh and play in the nearby park dogs bark and wag their tails in excitement as they chase after their toys the world is alive with activity and energy a vibrant tapestry of sound and color that fills our senses and stirs our souls.");
    
    read_file("Second.txt");

    update_file_block("Second.txt",16,"Modification by block");

    

    read_file("Second.txt");

    create_file("Third.txt","Gratitude is a powerful emotion that can have a profound impact on our lives. It involves being thankful and appreciative for the positive aspects of our lives, including the people, experiences, and opportunities that have helped us grow and thrive.When we practice gratitude, we cultivate a sense of appreciation and contentment, which can lead to increased happiness and well-being. Studies have shown that individuals who regularly practice gratitude are more optimistic, have better relationships, and experience fewer symptoms of depression and anxiety.There are many ways to practice gratitude, including keeping a gratitude journal, expressing thanks to others, and reflecting on the positive aspects of our lives. One effective method is to make a habit of listing three things we are grateful for each day. This simple exercise can help us focus on the positive aspects of our lives and cultivate a sense of abundance and joy.Gratitude is not just a personal practice; it can also have a positive impact on our relationships and communities. When we express gratitude to others, we build stronger connections and create a sense of mutual appreciation and respect. Gratitude can also inspire us to give back to others and contribute to our communities, which can create a positive cycle of generosity and kindness.In conclusion, gratitude is a powerful emotion that can enrich our lives and contribute to our overall well-being. By cultivating a sense of appreciation and thankfulness, we can create more joy, happiness, and positive connections in our lives and communities.");

    read_file("Third.txt");

    delete_file("Second.txt");
      
        	 

    return(0);         
}























