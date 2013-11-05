#include "mem.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

int page_size=0;
int m_init_flag=0;
int m_error;

typedef struct __node_t {
int size;
struct __node_t *next;
} node_t;

//GLOBAL VARIABLES FOR HEADER AND TOTAL SIZE
node_t* g_head;
int g_addr_space;


///////////MEM_INIT////////////////////
int Mem_Init(int sizeOfRegion)
{
m_error=0;
page_size=getpagesize();
if((sizeOfRegion<=0)||m_init_flag){
	printf("ERROR\n");
	m_error=E_BAD_ARGS;
	return -1;
	}
if(sizeOfRegion%page_size!=0){
	sizeOfRegion-=(sizeOfRegion%page_size);
	sizeOfRegion+=page_size;
	}
g_addr_space=sizeOfRegion;

int fd=open("/dev/zero",O_RDWR);
node_t* head = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
g_head=head;
if (head == MAP_FAILED){
	perror("mmap"); 
	return 1; 
	}
//printf("Address space start point is(including the header): %p\n",head);
head->size=sizeOfRegion-sizeof(node_t);
head->next=NULL;

//printf("Size of the address space(excluding the header):%d\n",head->size);
close(fd);
m_init_flag=1;
return 0;
}

void *Mem_Alloc(int size, int style)
{
m_error=0;
//int i=0;
//int j=0;

//printf("Mem_Alloc executing NOW!!!\n");
node_t* head=g_head;
node_t* allotted=NULL;
int service_complete=0;
if(size%8!=0){
	size+=(8-(size%8));
}
switch(style){
case FIRSTFIT:
	while((!service_complete)&&(head!=NULL)){
		if(size==head->size){
			head->size=0;	
			service_complete=1;
		}
		else if(size<head->size){
			node_t* temp=head->next;

			if(head->size-size>=sizeof(node_t)){
			head->next=(void *)head+size+sizeof(node_t);
			head->next->size=head->size-(size+sizeof(node_t));
			head->next->next=temp;
		 	head->size=0;
			allotted=head;
			head=head->next;
			}
			else{
//			head
			}
			service_complete=1;
		}
		else{
			head=head->next;
		}
	}
	if(service_complete){
		return ((void*)allotted+sizeof(node_t));
	}
	else{
	//	printf("Insufficient space !!!\n\n");
		m_error=E_NO_SPACE;
		return NULL;
	}
	break;

case BESTFIT:
	while(head!=NULL){
		if(size<=head->size){
			if(allotted==NULL){
				allotted=head;
			}
			else{
				if(head->size<allotted->size)
					allotted=head;
			}
		}
		head=head->next;
	}
	if(allotted==NULL){
	//	printf("Insufficient space !!!\n");
		m_error=E_NO_SPACE;
		return NULL;
	}
	else{
		if(allotted->size==size){
			allotted->size=0;
		}
		else if(allotted->size>size){
			if(allotted->size-size>sizeof(node_t)){
				node_t* temp=allotted->next;
				if(temp!=NULL){
					allotted->next=(void*)allotted+size+sizeof(node_t);
					if(temp->size==0){
						allotted->next->size=allotted->size-(size+sizeof(node_t));
						allotted->next->next=temp;
						allotted->size=0;
					}
					else{
						allotted->next->size=allotted->size-size+temp->size;
						allotted->next->next=temp->next;
						allotted->size=0;
					}
				}
				else{
					allotted->next=(void*)allotted+size+sizeof(node_t);
					allotted->next->size=(void*)g_head+g_addr_space-(void*)allotted-(size+sizeof(node_t));
					allotted->next->next=NULL;
					allotted->size=0;
				}
			}
			else{
				node_t* temp=allotted->next;
				if(temp!=NULL){
					if(temp->size==0){
						allotted->size=0;
					}
					else if(temp->size!=0){
						allotted->next=(void*)allotted+size+sizeof(node_t);
						allotted->next->size=allotted->size-size+temp->size;
						allotted->next->next=temp->next;
						allotted->size=0;
					}
				}
				else{
					allotted->size=0;
				}
			}
		}


	}







	/*
	else{
		if(allotted->size==size){
			allotted->size=0;
		}
		else{
			node_t* temp=allotted->next;
  			if(allotted->size-size>=sizeof(node_t)){
			if(allotted->next==NULL){
			
			allotted->next=(void*)allotted+size+sizeof(node_t);
			allotted->next->size=allotted->size-(size+sizeof(node_t));
			allotted->next->next=temp;
			allotted->size=0;
			}
			
			else{
			node_t* temp2=allotted->next->next;
			allotted->next=(void*)allotted+size+sizeof(node_t);
			allotted->next->size=(allotted->size-size+temp->size);
			allotted->next->next=temp2;
			allotted->size=0;
				
			}
		}
//		m_error=0;
		return ((void*) allotted + sizeof(node_t));
	}
	*/
  	return (void*) allotted + sizeof(node_t);	
	break;

case WORSTFIT:
	while(head!=NULL){
		if(size<=head->size){
			if(allotted==NULL){
				allotted=head;
			}
			else{
				if(head->size>allotted->size)
					allotted=head;
			}
		}
		head=head->next;
	}
	if(allotted==NULL){
	//	printf("Insufficient space !!!\n");
		m_error=E_NO_SPACE;
		return NULL;
	}
	else{
		if(allotted->size==size){
			allotted->size=0;
		}
		else if(allotted->size>size){
			if(allotted->size-size>sizeof(node_t)){
				node_t* temp=allotted->next;
				if(temp!=NULL){
					allotted->next=(void*)allotted+size+sizeof(node_t);
					if(temp->size==0){
						allotted->next->size=allotted->size-(size+sizeof(node_t));
						allotted->next->next=temp;
						allotted->size=0;
					}
					else{
						allotted->next->size=allotted->size-size+temp->size;
						allotted->next->next=temp->next;
						allotted->size=0;
					}
				}
				else{
					allotted->next=(void*)allotted+size+sizeof(node_t);
					allotted->next->size=(void*)g_head+g_addr_space-(void*)allotted-(size+sizeof(node_t));
					allotted->next->next=NULL;
					allotted->size=0;
				}
			}
			else{
				node_t* temp=allotted->next;
				if(temp!=NULL){
					if(temp->size==0){
						allotted->size=0;
					}
					else if(temp->size!=0){
						allotted->next=(void*)allotted+size+sizeof(node_t);
						allotted->next->size=allotted->size-size+temp->size;
						allotted->next->next=temp->next;
						allotted->size=0;
					}
				}
				else{
					allotted->size=0;
				}
			}
		}

	}	
	return ((void*) allotted + sizeof(node_t));
	
	break;

default:
	break;
}
return NULL;
}

int Mem_Free(void *ptr)
{
m_error=0;

if(ptr==NULL)
	return -1;
node_t* head=g_head;
while(head!=NULL){
	if(((void*)head->next+sizeof(node_t)==ptr)&&(head->size!=0)){               
		if(head->next->next==NULL){
                        head->size=(int)((void*)g_head+g_addr_space-(void*)head)-sizeof(node_t);
			head->next=NULL;
                        return 0;
                }
                else{
                        if(head->next->next->size!=0){
                                head->size=(int)((void*)head->next->next-(void*)head)+head->next->next->size;//-sizeof(node_t);
                                head->next=head->next->next->next;
                        }
                        else{
                                head->size=(int)((void*)head->next->next-(void*)head)-sizeof(node_t);
				head->next=head->next->next;
			}
                        return 0;
		}		
	}
	else if(((void*)head+sizeof(node_t))==ptr){
		if(head->next==NULL){
			head->size=(int)((void*)g_head+g_addr_space-(void*)head)-sizeof(node_t);
			return 0;
		}
		else{
			if(head->next->size!=0){
				head->size=(int)((void*)head->next-(void*)head)+head->next->size;
				head->next=(head->next)->next;
			}
			else
				head->size=(int)((void*)head->next-(void*)head)-sizeof(node_t);
			return 0;
		}
	}
	head=head->next;
   }
m_error= E_BAD_POINTER;
return -1;
}

void Mem_Dump()
{
node_t* head=g_head;
printf("Regions below\n");
while(head!=NULL){
	printf("Starting address of the chunk: %p Size available: %d\n",(void*)head+sizeof(node_t),head->size);
	head=head->next;
}
}
