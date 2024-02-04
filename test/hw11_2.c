#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define test_flag 1

typedef struct prefixs{
	unsigned int ip; 
	unsigned int len; 
	struct prefixs *next;
}prefix;

int main(int argc, char *argv[]){
	// init
	char *routing_table = argv[1], *d_number = argv[5];
	int pd, d=0, i=0;

	// pre string argv[5] to int
	for(i=0;i<strlen(d_number);i++){
		if('0'<=d_number[i] && d_number[i]<='9'){
			d = d*10 + d_number[i]-'0';
		}
	}
	
	prefix dump;
	input(routing_table, &dump);
	//print_prefix(&dump);
	//length_distribution(&dump);

	pd = pow(2, d);
	prefix *seg[pd];
	segment(d, &dump, &seg[0]);

	print_seg(test_flag, &seg[0], pd);
	
	printf("routing_table: %s\n", routing_table);
	printf("d: %s\n", argv[5]);

	return 0;
}

// target 1: read all the prefixes from the input "file"
// target 2: print out the total number of prefixes in the input file
int input(char *filename, prefix *dump){
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
	return 0;
}

// target 1: compute the number of prefixes with prefix length i, for i = 0 to 32
int length_distribution(prefix *dump){
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

	return 0;
}

// target 1: divide the prefixes of length >= d into 2^d groups 
//		such that the "prefixes in the same group have the same first d bits". Assume d > 8.
//		prefix of length < d are put in a special group
// target 2: printout the number of prefixes in group i for i = 0 to 2^d – 1
// target 3: use singly linked list to chain the prefixes which are in same group together
int segment(int d, prefix *dump1, prefix *dump2[]){
	// init
	int i=0, pd=0;
	prefix *dump1_current, *dump2_current, *dump2_next;
	dump1_current = dump1;
	
	pd = pow(2, d);
	
	for(i=0;i<pd;i++){
		dump2_current = (prefix*)malloc(sizeof(prefix));
		dump2_current->next = NULL;
		dump2[i] = dump2_current;
	}

	dump1_current = dump1;
	while(dump1_current->next!=NULL){
		i = dump1_current->ip>>(32-d);

		dump2_next = (prefix*)malloc(sizeof(prefix));
		dump2_current = dump2[i];

		while(dump2_current->next!=NULL){
			dump2_current = dump2_current->next;
		}
		dump2_current->next = dump2_next;
		dump2_current->ip = dump1_current->ip;
		dump2_current->len = dump1_current->len;
		dump2_current = dump2_next;
		dump2_current->next = NULL;

		dump1_current = dump1_current->next;
	}	

	return 0;
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

int print_prefix(prefix *dump){
	prefix *current;
	current = dump;
	while(current->next!=NULL){
		printf("data = %d.",current->ip>>24 & 0X000000FF);
		printf("%d.",current->ip>>16 & 0X000000FF);
		printf("%d.",current->ip>>8 & 0X000000FF);
		printf("%d, ",current->ip & 0X000000FF);
		printf("len = %d\n",current->len);
		current = current->next;
	}	

	return 0;
}

int print_seg(int test_flag, prefix *dump[],int pd){
	int i=0, total=0;
	int sum[pd];
	prefix *current;

	for(i=0;i<pd;i++){
		sum[i] = 0;
	}

	if(test_flag==1){
		for(i=0;i<pd;i++){
			current = dump[i];
			printf("seg = %d\n",i);
			while(current->next!=NULL){
				sum[i] = sum[i] + 1;
				printf("data = %d.",current->ip>>24 & 0X000000FF);
				printf("%d.",current->ip>>16 & 0X000000FF);
				printf("%d.",current->ip>>8 & 0X000000FF);
				printf("%d, ",current->ip & 0X000000FF);
				printf("len = %d\n",current->len);
				current = current->next;
			}	
		}
	}

	for(i=0;i<pd;i++){
		total = total + sum[i];
	}
	printf("The total number of prefixes in the input file is : %d.", total);
	for(i=0;i<pd;i++){
		printf("The number of prefixes in group %d = %d.", i, sum[i]);
	}

	return 0;
}
