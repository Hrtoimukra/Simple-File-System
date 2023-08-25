#include <stdio.h>
#include <string.h>
#include <time.h>

#define BLOCK_SIZE 512
#define MAX_BLOCKS 4096
#define MAX_FILES 100
#define FILE_NAME_LENGTH 32


// structure to define a basic block which is a unit of storage on the  disk
typedef struct basic_block{
    char data[BLOCK_SIZE];
}Block;

//structure to define index block which contains the pointer to the blocks 
typedef struct Index_Block{
    int block_ptrs[40];
}IndexBlock;

//structure of the file descriptor under index allocation scheme
typedef struct Index_allocation_scheme{
    char name[32];
    int size;
    IndexBlock IB;
}file_t;

Block disk[MAX_BLOCKS];
file_t files[MAX_FILES];
int num_files = 0;

//Function to init the file system 
void init_file_system(){
     for(int i=0;i<MAX_BLOCKS;++i)
         disk[i].data[0]='0'; 
}

//Function to allocate a free block.A free block is a block whose first byte character is set to '0'
int allocate_block(){
    for(int i = 0; i < MAX_BLOCKS; i++){
        if (disk[i].data[0] == '0'){
            disk[i].data[0] = '1'; // mark block as allocated
            return i;
        }
    }
    return -1; // no free blocks
}


//Function to create a file given the name of the file and it's size  
int create_file(char* name, char *data){

    if(num_files >= MAX_FILES){
        printf("Maximum Limit for the files exceeded\n");
        return -1; // file table is full
    }

    int file_size=strlen(data);

    int data_index=0;
    int block_index=1;
    int block_write_index=1;

    int current_block=allocate_block(); 

    IndexBlock ib; 
    ib.block_ptrs[0]=current_block;     

    while(data_index<file_size){
  
         disk[current_block].data[block_write_index]=data[data_index];
         block_write_index++;
         data_index++;

         if((data_index+1)%(BLOCK_SIZE-2)==0 && (data_index+1)!=file_size){
            disk[current_block].data[block_write_index]='\0';
            current_block=allocate_block();
            block_write_index=1;
            ib.block_ptrs[block_index]=current_block;
            block_index++; 
         }            
    }

    file_t new_file;
    new_file.size=file_size;
    strncpy(new_file.name,name, FILE_NAME_LENGTH);
    new_file.IB=ib;
    files[num_files] = new_file;
    num_files++;
    return num_files-1;
}

int delete_file(char *name){//Function to delete the file given the name of the file
     for(int i=0;i<num_files;++i){
 
         if(strcmp(files[i].name,name)==0){

            int size=files[i].size;
            int num_blocks = size/BLOCK_SIZE;    
            if(size%BLOCK_SIZE)
               num_blocks+=1;

            for(int j=0;j<num_blocks;++j)
                disk[files[i].IB.block_ptrs[j]].data[0]='0';
            
            for(int k = i; k < num_files - 1; k++)
                files[k] = files[k + 1];

            num_files--;
            return i;                   
         }             
     }     

    return(-1); 
}  

int read_file(char* name){//function to read the content of the file given the name of the file
 
    for(int i=0;i<num_files;++i){

       if(strcmp(files[i].name,name)==0){
          
          int size=files[i].size;
          int num_blocks = size/BLOCK_SIZE;    
          if(size%BLOCK_SIZE)
             num_blocks+=1; 

          //printf("Filename =%s size=%d\n",files[i].name,files[i].size);

          for(int j=0;j<num_blocks;++j)
              printf("Block Number= %d  Data =%s\n\n\n",files[i].IB.block_ptrs[j],disk[files[i].IB.block_ptrs[j]].data);   

          printf("\n\n");
          return(i);

       }    
    }
    
    return(-1); 
}

int update_file_block(char *name,int pos,char *data){//Function to upadate the file.Given the position of block to write to and the data

    for(int i=0;i<num_files;++i){

        if(strcmp(files[i].name,name)==0){

           int length=strlen(data);
           int j=0;

           while(j<length){
               disk[files[i].IB.block_ptrs[pos]].data[j+1]=data[j];
               j++; 
           }
           disk[files[i].IB.block_ptrs[pos]].data[j+1]='\0';
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





















