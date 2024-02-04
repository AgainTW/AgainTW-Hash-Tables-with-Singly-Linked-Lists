#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define test_flag 0
#define print_prefix_flag 0
#define insert_flag 0
#define delete_flag 0
#define search_flag 0

typedef struct prefixs{
	unsigned int ip; 
	unsigned int len; 
	struct prefixs *next;
}prefix;

int input(char*, prefix*);
int length_distribution(prefix*);
int segment(int, prefix*, prefix*[]);
int file_prefix_insert(char*, prefix*[], int);
int prefix_insert(prefix*, prefix*);
int file_prefix_delete(char*, prefix*[], int );
int prefix_delete(unsigned int, unsigned int , prefix*);
int file_search(char*, prefix*[], int);
int search(unsigned int, prefix*);
int print_prefix(prefix*);
int print_seg(prefix*[], int);

int power(int x, int n)
{
	double ans=1;
	int i=0;

	for(i=0;i<n;i++){
		ans *= x;
	}

	return ans;
}

int main(int argc, char *argv[]){
	// init
	char *routing_table = argv[1], 
			*inserted_prefixes = argv[2],
			*delete_prefixes = argv[3],
			*trace_file = argv[4],
			*d_number = argv[5];
	int pd, d=0, i=0;

	// pre string argv[5] to int
	for(i=0;i<strlen(d_number);i++){
		if('0'<=d_number[i] && d_number[i]<='9'){
			d = d*10 + d_number[i]-'0';
		}
	}
	
	prefix dump;
	input(routing_table, &dump);
	if(print_prefix_flag==1) print_prefix(&dump);
	length_distribution(&dump);

	pd = power(2, d);
	prefix *seg[pd];
	segment(d, &dump, &seg[0]);
	print_seg(&seg[0], pd);
	file_prefix_insert(inserted_prefixes, &seg[0], d);
	print_seg(&seg[0], pd);
	file_prefix_delete(delete_prefixes, &seg[0], d);
	print_seg(&seg[0], pd);
	file_search(trace_file, &seg[0], d);
	
	if(test_flag==1){
		printf("routing_table: %s\n", routing_table);
		printf("inserted_prefixes: %s\n", inserted_prefixes);
		printf("delete_prefixes: %s\n", delete_prefixes);
		printf("trace_file: %s\n", trace_file);
		printf("d: %s\n", d_number);	
	}

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
	int i=0, j=0;

	FILE *fp = fopen(filename, "r");

	if (fp == NULL){
		printf("Error: could not open file %s\n", filename);
		return 1;
	}

	prefix_current = (prefix*)malloc(sizeof(prefix));
	prefix_dump = prefix_current;
	while(fgets(buffer, MAX_LENGTH, fp)){	
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
			if('0'<=buffer[j] && buffer[j]<='9'){ temp = temp*10 + buffer[j]-'0'; }
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
		
	for(i=0;i<32;i++){ distri[i] = 0; }
		
	while(current->next!=NULL){
		distri[(current->len)-1] = distri[(current->len)-1] + 1;
		current = current->next;
	}

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
	
	pd = power(2, d);
	
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
int file_prefix_insert(char *filename, prefix *dump[], int d){
	// init
	const unsigned MAX_LENGTH = 256;
	char buffer[MAX_LENGTH];
	unsigned int i=0;
	unsigned int ip_temp, temp;
	prefix *prefix_current, *dump_current;

	FILE *fp = fopen(filename, "r");

	if (fp == NULL){
		printf("Error: could not open file %s\n", filename);
		return 1;
	}

	while(fgets(buffer, MAX_LENGTH, fp)){	
		prefix_current = (prefix*)malloc(sizeof(prefix));
		
		i = 0;
		temp = 0;
		ip_temp = 0;
		while(buffer[i]!='\/'){
			if(buffer[i]=='.'){
				ip_temp = (ip_temp<<8) | temp;
				temp = 0;
			}
			else if('0'<=buffer[i] && buffer[i]<='9'){
				temp = temp*10 + buffer[i]-'0';
			}
			i++;
		}
		ip_temp = (ip_temp<<8) | temp;
		temp = 0;		
		i++;
		
		while(buffer[i]!='\n'){
			if('0'<=buffer[i] && buffer[i]<='9'){ temp = temp*10 + buffer[i]-'0'; }
			i++;
		}		
		
		prefix_current->ip = ip_temp;
		prefix_current->len = temp;
		prefix_current->next = NULL;
		if(insert_flag==1) print_prefix(prefix_current);
		i = ip_temp>>(32-d);
		dump_current = dump[i];

		prefix_insert(prefix_current, dump_current);
	}

	fclose(fp);

	return 0;	
}
int prefix_insert(prefix *insert, prefix *dump){
	prefix *current;
	current = dump;

	while(current->next!=NULL){ current = current->next; }	
	current->next = insert;

	return 0;
}

// target 1: 執行完 prefix_insert 後，從 "prefix_delete" 逐行刪除 segment 後的資料結構
int file_prefix_delete(char *filename, prefix *dump[], int d){
	// init
	const unsigned MAX_LENGTH = 256;
	char buffer[MAX_LENGTH];
	unsigned int i=0;
	unsigned int ip_temp, temp;
	prefix *dump_current;

	FILE *fp = fopen(filename, "r");

	if (fp == NULL){
		printf("Error: could not open file %s\n", filename);
		return 1;
	}

	while(fgets(buffer, MAX_LENGTH, fp)){		
		i = 0;
		temp = 0;
		ip_temp = 0;
		while(buffer[i]!='\/' && buffer[i]!='\n'){
			if(buffer[i]=='.'){
				ip_temp = (ip_temp<<8) | temp;
				temp = 0;
			}
			else if('0'<=buffer[i] && buffer[i]<='9'){
				temp = temp*10 + buffer[i]-'0';
			}
			i++;
		}
		ip_temp = (ip_temp<<8) | temp;
		temp = 0;		
		i++;
		
		while(buffer[i]!='\n'){
			if('0'<=buffer[i] && buffer[i]<='9'){ temp = temp*10 + buffer[i]-'0'; }
			i++;
		}		
		
		i = ip_temp>>(32-d);
		dump_current = dump[i];
		if(delete_flag==1) {
			printf("%s", buffer);
			printf("delete = 0x%08x, seg = %d\n", ip_temp, i);
		}
		
		if(dump_current->ip==ip_temp && dump_current->len==temp){
			dump[i] = dump_current->next;
		}
		else if(dump_current->ip==ip_temp){		// for 0x0f000000 error
			dump[i] = dump_current->next;
		}
		else{
			prefix_delete(ip_temp, temp, dump_current);
		}
		
	}

	fclose(fp);

	return 0;
}
int prefix_delete(unsigned int del_ip, unsigned int len, prefix *dump){
	prefix *current, *pre;
	current = dump;
	
	while(current->next!=NULL && (current->ip!=del_ip || current->len!=len)){ 
		pre = current;
		current = current->next;
	}
	
	if(current->next==NULL && current->ip==del_ip && current->len==len){
		pre->next = NULL;
	}
	else if(current->ip==del_ip && current->len==len){
		pre->next = current->next;
	}
	else if(current->ip==del_ip){		// for 0x0f000000 error
		pre->next = current->next;
	}
	else{
		print_prefix(dump);
		printf("error: 0x%08x\n",del_ip);
	}

	return 0;
}

// target 1: 執行完 prefix_delete 後，從 "trace_file" giving an IP address to report 
// 		if the search is successful or failed
int file_search(char *filename, prefix *dump[], int d){
	// init
	const unsigned MAX_LENGTH = 256;
	char buffer[MAX_LENGTH];
	unsigned int i=0;
	unsigned int ip_temp, temp;
	prefix *dump_current;

	FILE *fp = fopen(filename, "r");

	if (fp == NULL){
		printf("Error: could not open file %s\n", filename);
		return 1;
	}

	while(fgets(buffer, MAX_LENGTH, fp)){		
		i = 0;
		temp = 0;
		ip_temp = 0;
		while(buffer[i]!='\n'){
			if(buffer[i]=='.'){
				ip_temp = (ip_temp<<8) | temp;
				temp = 0;
			}
			else if('0'<=buffer[i] && buffer[i]<='9'){
				temp = temp*10 + buffer[i]-'0';
			}
			i++;
		}
		ip_temp = (ip_temp<<8) | temp;
		temp = 0;		
		
		i = ip_temp>>(32-d);
		dump_current = dump[i];
		if(search_flag==1) {
			printf("%s", buffer);
			printf("search = 0x%08x, seg = %d\n", ip_temp, i);
		}
		
		if(dump_current->ip==ip_temp){
			printf("successful\n");
		}
		else{
			search(ip_temp, dump_current);
		}
		
	}

	fclose(fp);

	return 0;
}
int search(unsigned int del_ip, prefix *dump){
	prefix *current;
	current = dump;
	
	while(current->next!=NULL && current->ip!=del_ip){ 
		current = current->next;
	}
	
	if(current->next==NULL && current->ip==del_ip){
		printf("successful\n");
	}
	else if(current->ip==del_ip){
		printf("successful\n");
	}
	else{
		printf("failed\n");
	}

	return 0;
}

int print_prefix(prefix *dump){
	prefix *current;
	current = dump;
	if(current->next==NULL){
		printf("data = %d.",current->ip>>24 & 0X000000FF);
		printf("%d.",current->ip>>16 & 0X000000FF);
		printf("%d.",current->ip>>8 & 0X000000FF);
		printf("%d, ",current->ip & 0X000000FF);
		printf("len = %d\n",current->len);	
	}
	else{
		while(current->next!=NULL){
			printf("data = %d.",current->ip>>24 & 0X000000FF);
			printf("%d.",current->ip>>16 & 0X000000FF);
			printf("%d.",current->ip>>8 & 0X000000FF);
			printf("%d, ",current->ip & 0X000000FF);
			printf("len = %d\n",current->len);
			current = current->next;
		}	
	}

	return 0;
}

int print_seg(prefix *dump[],int pd){
	int i=0, total=0;
	int sum[pd];
	prefix *current;

	for(i=0;i<pd;i++){ sum[i] = 0; }
	
	for(i=0;i<pd;i++){
		current = dump[i];
		if(test_flag==1) printf("seg = %d\n",i);
		while(current->next!=NULL){
			sum[i] = sum[i] + 1;
			if(test_flag==1){
				printf("data = %d.",current->ip>>24 & 0X000000FF);
				printf("%d.",current->ip>>16 & 0X000000FF);
				printf("%d.",current->ip>>8 & 0X000000FF);
				printf("%d, ",current->ip & 0X000000FF);
				printf("len = %d\n",current->len);
			}
			current = current->next;
		}	
	}

	for(i=0;i<pd;i++){ total = total + sum[i]; }
	printf("The total number of prefixes in the input file is : %d.\n", total);
	for(i=0;i<pd;i++){ printf("The number of prefixes in group %d = %d.\n", i, sum[i]); }

	return 0;
}
