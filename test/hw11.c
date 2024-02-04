#include<stdio.h>
#include<stdlib.h>

typedef struct prefixs{
	unsigned ip; 
	unsigned char len; 
	struct prefixs *next;
}prefix;

int main(int argc, char *argv[]){
	// init
	char *routing_table = argv[1];
	
	prefix dump;
	input(routing_table, &dump);
	//print_prefix(&dump);
	length_distribution(&dump);

	printf("routing_table: %s\n", routing_table);
	
	return 0;
}

// target 1: read all the prefixes from the input "file"
// target 2: print out the total number of prefixes in the input file
void input(char *filename, prefix *dump){
	// init
	const unsigned MAX_LENGTH = 256;
	char buffer[MAX_LENGTH];
	prefix *prefix_dump, *prefix_current, *prefix_next;
	unsigned int ip_temp, temp;
	int i=0,j=0;

	FILE *fp = fopen(filename, "r");

	if (fp == NULL){
		printf("Error: could not open file %s\n", filename);
		return 1;
	}

	prefix_current = (prefix*)malloc(sizeof(prefix));
	prefix_dump = prefix_current;
	while(fgets(buffer, MAX_LENGTH, fp)){
//		printf("%s", buffer);
		
		prefix_next = (prefix*)malloc(sizeof(prefix));
		
		j = 0;
		temp = 0;
		ip_temp = 0;
		while(buffer[j]!='\/'){
			if(buffer[j]=='.'){
				ip_temp = (ip_temp<<8) | temp;
				temp = 0;
			}
			else if('0'<=buffer[j] && buffer[j]<='9'){
				temp = temp*10 + buffer[j]-'0';
			}
			j++;
		}
		ip_temp = (ip_temp<<8) | temp;
		temp = 0;		
		j++;
		
		while(buffer[j]!='\n'){
			if('0'<=buffer[j] && buffer[j]<='9'){
				temp = temp*10 + buffer[j]-'0';
			}
			j++;
		}		
		
		prefix_current->ip = ip_temp;
		prefix_current->len = temp;
		prefix_current->next = prefix_next;
		prefix_current = prefix_next;
		prefix_current->next = NULL;
	}

	fclose(fp);

	*dump = *prefix_dump;
}

// target 1: compute the number of prefixes with prefix length i, for i = 0 to 32
void length_distribution(prefix *dump){
	int distri[32];
	int i=0;
	
	prefix *current;
	current = dump;
		
	for(i=0;i<32;i++){
		distri[i] = 0;
	}
		
	while(current->next!=NULL){
		
		distri[(current->len)-1] = distri[(current->len)-1] + 1;
		current = current->next;
	}
	
// test
	for(i=0;i<32;i++){
		printf("the number of prefixes with prefix length %d = %d\n", i, distri[i]);
	}	
}

// target 1: divide the prefixes of length >= d into 2^d groups 
//		such that the "prefixes in the same group have the same first d bits". Assume d > 8.
//		prefix of length < d are put in a special group
// target 2: printout the number of prefixes in group i for i = 0 to 2^d – 1
// target 3: use singly linked list to chain the prefixes which are in same group together
void segment(int d){

}

// target 1: 在執行 segment 後，從 "inserted_prefixes" 逐行插入 segment 後的資料結構
void prefix_insert(){

}

// target 1: 執行完 prefix_insert 後，從 "prefix_delete" 逐行刪除 segment 後的資料結構
void prefix_delete(){

}

// target 1: 執行完 prefix_delete 後，從 "trace_file" giving an IP address to report 
// 		if the search is successful or failed
void search(){

}

void print_prefix(prefix *dump){
	prefix *current;
	current = dump;
	while(current->next!=NULL){
		printf("data=%d.",current->ip>>24 & 0X000000FF);
		printf("%d.",current->ip>>16 & 0X000000FF);
		printf("%d.",current->ip>>8 & 0X000000FF);
		printf("%d, ",current->ip & 0X000000FF);
		printf("len=%d\n",current->len);
		current = current->next;
	}	
/*	for test
	while(current->next!=NULL){	
		if(current->len==0){
			printf("data=%d.",current->ip>>24 & 0X000000FF);
			printf("%d.",current->ip>>16 & 0X000000FF);
			printf("%d.",current->ip>>8 & 0X000000FF);
			printf("%d, ",current->ip & 0X000000FF);
			printf("len=%d\n",current->len);
		}
		current = current->next;
	}
*/
}
