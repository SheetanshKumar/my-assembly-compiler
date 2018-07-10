#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

char *opcode[] = { "MOVRM", "MOVMR", "ADD", "SUB", "MUL", "JMP", "IF", "EQ", "LT", "GT", "LTEQ", "GTEQ", "PRINT", "READ", "ENDIF" };

int * registers = (int *)malloc(sizeof(int)*8);
int * variables = (int *)malloc(sizeof(int) * 1000);
int *memory = (int *)malloc(sizeof(int) * 1000);

struct symbolTable{
	char name;
	int address;
	int size;
};

struct intermediateTable{
	int index;
	int opcode;
	int * parameters;
};

struct lableTable{
	char * name;
	int address;
};

int lt_count = 0;
int read_reg_count = 0, print_reg_count = 0;


struct Stack{
	int top;
	int * arr;
	unsigned capacity;
};
struct Stack * createStack(unsigned capacity){
	Stack * s = (Stack *)malloc(sizeof(Stack));
	if (!s)
		return NULL;
	s->top = -1;
	s->arr = (int *)malloc(sizeof(int)*capacity);
	if (!s->arr)
		return NULL;
	return s;
}

int isEmpty(Stack * s)
{
	return s->top == -1;
}
char peek(Stack * s)
{
	return s->arr[s->top];
}
void push(Stack * s, int num)
{
	s->arr[++s->top] = num;
}
int pop(struct Stack * s)
{
	if (!isEmpty(s))
		return s->arr[s->top--];
	return -100;
}
Stack * s = createStack(10);
void getTokens(char * str, int ind)
{
	int i = 0; 
	char * token = (char *)malloc(sizeof(str)*strlen(str));
	while (str[i] != '\0')
	{
		int j = 0;
		while (str[i] != ' ')
		{
			token[j++] = str[i++];
		}
		token[j] = '\0';
		
	}
	free(token);
}

int getInt(char * str)
{
	int i = 0;
	int num = 0;
	while (str[i] != '\0' && str[i] != '\n')
	{
		num = num * 10 + (str[i] - '0');
		i++;
	}
	return num;
}
char * strip(char * str, char delim)
{
	char * st = (char *)malloc(sizeof(char)*strlen(str));
	int i = 0, j = 0;
	while (str[i] != '\0')
	{
		if (str[i] != delim)
			st[j++] = str[i];
		i++;
	}
	st[j] = '\0';
	return st;
}
void create_symbol_table(char * str, int ind, struct symbolTable ** stable)
{
	stable[ind] = (struct symbolTable *)malloc(sizeof(struct symbolTable));
	int i = 0;
	char * token = (char *)malloc(sizeof(str)*strlen(str));
	int j = 0;
	while (str[i] != ' ')
	{
		token[j++] = str[i++];
	}
	token[j] = '\0';
	i++;
	if (strcmp(token, "DATA") == 0)
	{
		int j = 0;
		int flag = 0, mul = 1;
		while (str[i] != '\0')
		{
			if (str[i] == '[')
			{
				flag = 1;
				break;
			}
			token[j++] = str[i++];
		}
		token[j] = '\0';
		//printf("\n%s\n", token);
		//stable[ind]->name = (char *)malloc(sizeof(char)*strlen(token));
		
		stable[ind]->name = token[0];
		if (flag == 1)
		{
			i++;
			int j = 0;
			while (str[i] != ']')
			{
				token[j++] = str[i++];
			}
			token[j] = '\0';
			mul = getInt(token);
		}

		
		stable[ind]->size = 1*mul;
		stable[ind]->address = 0;
		if (ind == 0)
			stable[ind]->address = 0;
		else
			stable[ind]->address = stable[ind - 1]->size + stable[ind - 1]->address;
		if (ind != 0 && stable[ind - 1]->size == 0)
			stable[ind]->address++;
		printf("name = %c, address = %d, size = %d\n", stable[ind]->name, stable[ind]->address, stable[ind]->size);
	}
	else if (strcmp(token, "CONST") == 0)
	{
		int j = 0;
		while (str[i] != ' ' )
		{
			token[j++] = str[i++];
		}
		token[j] = '\0';
		token = strip(token, ' ');
		stable[ind]->name = token[0];
		while (str[i++] != '=');
		j = 0;
		while (str[i] != '\0' && str[i] != '\n')
			token[j++] = str[i++];
		token[j] = '\0';
		token = strip(token, ' ');
		int num = getInt(token);
		stable[ind]->size = 0;
		if (ind == 0)
			stable[ind]->address = 0;
		else
			stable[ind]->address =  stable[ind - 1]->size + stable[ind - 1]->address;
		if (ind != 0 && stable[ind - 1]->size == 0)
			stable[ind]->address++;
		memory[stable[ind]->address + 1] = num;
		printf("name = %c, address = %d, size = %d\n", stable[ind]->name, stable[ind]->address, stable[ind]->size);
	}
	free(token);
}


void read(char * token, int ind, struct intermediateTable ** itable, struct symbolTable ** stable)
{
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 14;
	//itable[ind]->parameters = (int *)malloc(sizeof(int) * 2);
	itable[ind]->parameters[0] = token[0] - 'A';
}
void print(char * token,int slen, int ind, struct intermediateTable ** itable, struct symbolTable ** stable)
{
	/*int reg_ind = token[0] - 'A' + 48;
	int num;
	num = registers[reg_ind];
	printf("%d\n", num);*/
	
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 13;
	token = strip(token, ' ');
	token = strip(token, '\n');
	if (token[1] == 'X')
		itable[ind]->parameters[0] = token[0] - 'A';
	else
	{
		for (int i = 0; i < slen; i++)
		{		
			if (token[0] == stable[i]->name)
			{
				itable[ind]->parameters[0] = stable[i]->address;
				break;
			}
		}
	}
}
void mov_rm(char * token, int ind, struct intermediateTable ** itable, struct symbolTable ** stable, int length_stable)
{
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 1;
	token = strip(token, ' ');
	int arrsize = 0;
	if (token[1] == '[')
	{
		char * tempst = (char *)malloc(sizeof(char) * 5);
		int k = 0, j = 2;
		while (token[j] != ']')
			tempst[k++] = token[j++];
		tempst[k] = '\0';
		arrsize = getInt(tempst);
		free(tempst);
	}
	int k = 0;
	while (token[k++] != ',');
	itable[ind]->parameters[1] = token[k] - 'A';//a,ax		a[0],ax
												//01234567	0123456
	for (int i = 0; i < length_stable; i++)
	{
		if (stable[i]->name == token[0])
		{
			itable[ind]->parameters[0] = stable[i]->address + arrsize; // arrsize is needed.
			break;
		}
	}

}
void mov_mr(char * token, int ind, struct intermediateTable ** itable, struct symbolTable ** stable, int length_stable)
{
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 2;
	itable[ind]->parameters[0] = token[0] - 'A';//ax,a		ax,a[0]
	token = strip(token, ' ');					 //0123		0123456
	int arrsize = 0;
	if (token[4] == '[')
	{
		char * tempst = (char *)malloc(sizeof(char) * 5);
		int k = 0, j = 5;
		while (token[j] != ']')
			tempst[k++] = token[j++];
		tempst[k] = '\0';
		arrsize = getInt(tempst);
		free(tempst);
	}
	for (int i = 0; i < length_stable; i++)
	{
		if (stable[i]->name == token[3])
		{
			itable[ind]->parameters[1] = stable[i]->address + arrsize;
			break;
		}
	}

}
void add(char * token, int ind, struct intermediateTable ** itable)
{
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 3;
	itable[ind]->parameters[0] = token[0] - 'A';
	itable[ind]->parameters[1] = token[3] - 'A' ;
	itable[ind]->parameters[2] = token[6] - 'A' ;
}
void sub(char * token, int ind, struct intermediateTable ** itable)
{
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 4;
	itable[ind]->parameters[0] = token[0] - 'A';
	itable[ind]->parameters[1] = token[3] - 'A' ;
	itable[ind]->parameters[2] = token[6] - 'A' ;
}
void mul(char * token, int ind, struct intermediateTable ** itable)
{
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 5;
	itable[ind]->parameters[0] = token[0] - 'A' ;
	itable[ind]->parameters[1] = token[3] - 'A' ;
	itable[ind]->parameters[2] = token[6] - 'A' ;
}
void if_function(char * token, int ind, struct intermediateTable ** itable){
	push(s, ind + 1);
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 7;
	itable[ind]->parameters[0] = token[0] - 'A';

	int i = 0, j = 0;
	char str[100];
	printf("%s", token);
	while (token[i++] != ' ');
	while (token[i] != ' ')
		str[j++] = token[i++];
	str[j] = '\0';
	int k = 0;
	while (strcmp(str, opcode[k++]) != 0);
	itable[ind]->parameters[2] = k;
	j = 0;
	i++;
	while (token[i] != '\0')
		str[j++] = token[i++];
	str[j] = '\0';
	printf("%s", str);
	itable[ind]->parameters[1] = str[0] - 'A';
}
void else_function(struct intermediateTable ** itable, int ind)
{
	int if_ind = pop(s) - 1;
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 6;
	push(s, ind + 1);
	itable[if_ind]->parameters[3] = ind + 2;
}
void end_function(struct intermediateTable ** itable, int ind)
{
	int else_ind = pop(s) - 1;
	//pop(s);
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 15;
	itable[else_ind]->parameters[0] = ind + 1;	
}
void jump(char * token, int ind, struct intermediateTable ** itable,struct lableTable** lt)
{
	itable[ind]->index = ind + 1;
	itable[ind]->opcode = 6;
	int i = 0;
	//printf("\n%d", lt_count);
	for (i = 0; i < lt_count; i++)
	{
		//printf("%c, %c",token[0], lt[i]->name[0]);
		if (token[0] == lt[i]->name[0])
			break;
	}
	//printf("%s", token);
	//printf("%d", i);
	itable[ind]->parameters[0] = lt[i]->address;
}
int in(char * str, int chr)
{
	int i = 0;
	while (str[i] != '\0')
	{
		if (str[i++] == chr)
			return 1;
	}
	return 0;
}
void create_intermediate_table(char * str, int ind, struct intermediateTable ** itable, struct symbolTable ** stable, int length_stable, struct lableTable **lt)
{
	itable[ind] = (struct intermediateTable *)malloc(sizeof(intermediateTable));
	itable[ind]->parameters = (int *)malloc(sizeof(int) * 7);
	lt[lt_count] = (struct lableTable *)malloc(sizeof(struct lableTable));
	lt[lt_count]->name = (char *)malloc(sizeof(char) * 100);
	for (int i = 0; i < 6; i++)
		itable[ind]->parameters[i] = -1;
	int i = 0, j = 0;
	char * token = (char *)malloc(sizeof(char)*strlen(str));
	while (str[i] != '\0' && str[i] != ' ')
	{
		token[j++] = str[i++];
	}
	token[j] = '\0';	
	i++;
	if (!strcmp(token, "READ"))
	{
		j = 0;
		while (str[i] != '\0')
			token[j++] = str[i++];
		token[j] = '\0';
		read(token, ind, itable, stable);
	}
	else if (!strcmp(token, "PRINT"))
	{
		j = 0;
		while (str[i] != '\0')
			token[j++] = str[i++];
		token[j] = '\0';
		print(token, length_stable,ind, itable, stable);
	}
	else if (!strcmp(token, "MOV"))
	{
		j = 0;
		while (str[i] != '\0')
			token[j++] = str[i++];
		token[j] = '\0';
		//printf("\n---------------------------%s\n", token);
		if (token[1] == 'X')
		{
			mov_mr(token, ind, itable, stable, length_stable);
		}
		else
		{
			mov_rm(token, ind, itable, stable, length_stable);
		}
	}
	else if (!strcmp(token, "ADD"))
	{
		j = 0;
		while (str[i] != '\0')
			token[j++] = str[i++];
		token[j] = '\0';
		token = strip(token,' ');
		add(token, ind, itable);
	}
	else if (!strcmp(token, "SUB"))
	{
		j = 0;
		while (str[i] != '\0')
			token[j++] = str[i++];
		token[j] = '\0';
		token = strip(token, ' ');
		sub(token, ind, itable);
	}
	else if (!strcmp(token, "MUL"))
	{
		j = 0;
		while (str[i] != '\0')
			token[j++] = str[i++];
		token[j] = '\0';
		token = strip(token, ' ');
		mul(token, ind, itable);
	}
	else if (!strcmp(token, "IF"))
	{
		j = 0;
		while (str[i] != '\0')
			token[j++] = str[i++];
		token[j] = '\0';
		
		if_function( token, ind, itable);
	}
	else if (!strcmp(token, "ELSE\n"))
	{
		else_function(itable, ind);
	}
	else if (!strcmp(strip(token, '\n'), "ENDIF"))
	{
		end_function(itable, ind);
	}
	else if (in(token, ':'))
	{
		int j = 0;
		//printf("%s, %d", token, lt_count);
		i = 0;
		while (token[i] != ':')
			lt[lt_count]->name[j++] = token[i++];
		lt[lt_count]->name[j] = '\0';
		lt[lt_count]->address = ind + 1;
		lt_count++;
	}
	else if (!strcmp(token, "JUMP"))
	{
		j = 0;
		while (str[i] != '\0')
			token[j++] = str[i++];
		token[j] = '\0';
		
		jump(token, ind, itable,lt);
	}
}

void perform_operation(intermediateTable ** itable, symbolTable ** stable, int slen, int ilen)
{
	for (int i = 0; i < ilen; i++)
	{
		int constflag = 0;
		switch (itable[i]->opcode)
		{
		case 1:	// mov (register to memory)
			variables[itable[i]->parameters[0]] = registers[itable[i]->parameters[1]];
			break;
		case 2: //mov (memory/constant to register)
			for (int j = 0; j < slen; j++)
				if (stable[j]->address == itable[i]->parameters[1])
					if (stable[j]->size == 0)
					{
						constflag = 1;
						break;
					}
			if (constflag == 1)
				registers[itable[i]->parameters[0]] = memory[itable[i]->parameters[1]+1];
			else
				registers[itable[i]->parameters[0]] = variables[itable[i]->parameters[1]];
			break;
		case 3: //add
			registers[itable[i]->parameters[0]] = registers[itable[i]->parameters[1]] + registers[itable[i]->parameters[2]];
			break;
		case 4: //sub
			registers[itable[i]->parameters[0]] = registers[itable[i]->parameters[1]] - registers[itable[i]->parameters[2]];
			break;
		case 5: //mul
			registers[itable[i]->parameters[0]] = registers[itable[i]->parameters[1]] * registers[itable[i]->parameters[2]];
			break;
		case 6:// jump
			i = itable[i]->parameters[0] - 2;
			break;
		case 7: //if 
			switch (itable[i]->parameters[2])
			{// try here to false the condition to jump into the location
			case 8:// EQ
				if (registers[itable[i]->parameters[0]] != registers[itable[i]->parameters[1]])
					i = itable[i]->parameters[3] - 2;
				break;
			case 9:// LT
				if (registers[itable[i]->parameters[0]] >= registers[itable[i]->parameters[1]])
					i = itable[i]->parameters[3] - 2;// - 2 because i++ in the loop.
				break;
			case 10:// GT
				if (registers[itable[i]->parameters[0]] <= registers[itable[i]->parameters[1]])
					i = itable[i]->parameters[3] - 2;
				break;
			case 11:// LTEQ
				if (registers[itable[i]->parameters[0]] > registers[itable[i]->parameters[1]])
					i = itable[i]->parameters[3] - 2;
				break;
			case 12:// GTEQ
				if (registers[itable[i]->parameters[0]] < registers[itable[i]->parameters[1]])
					i = itable[i]->parameters[3] - 2;
				break;
			}
			break;
		case 13: //print
			for (int j = 0; j < slen; j++)
				if (stable[j]->address == itable[i]->parameters[0])
					if (stable[j]->size == 0)
					{
						constflag = 2;
						break;
					}
			if (constflag == 2)
				printf("%d\n", memory[itable[i]->parameters[0] + 1]);
			else
				printf("%d\t", registers[itable[i]->parameters[0]]);
			break;
		case 14: //read
			printf("\nEnter Value:\n");
			scanf("%d", &registers[itable[i]->parameters[0]]);
			fflush(stdin);
			break;
		case 15: //endif
			break;
		}
	}
}


int main()
{
	struct symbolTable ** stable = (struct symbolTable**)malloc(sizeof(symbolTable*)*26);
	struct intermediateTable ** itable = (struct intermediateTable **)malloc(sizeof(struct intermediateTable *)*1000);
	struct lableTable ** lt = (struct lableTable **)malloc(sizeof(struct lableTable *) * 100);
	
	char * filename = (char *)malloc(sizeof(char) * 20);
	printf("Enter filename:\n");
	scanf("%s", filename);
	fflush(stdin);
	FILE * fp = fopen(filename, "r");
	char * str = (char *)malloc(sizeof(char) * 100);
	const size_t line_size = 100;
	int i = 0;
	
	printf("Symbol Table:\n");
	while (!feof(fp))
	{
		fgets(str, line_size, fp);
		if (strcmp(str, "START:\n") == 0)
			break;
		//itable[i]->parameters = (int *)malloc(sizeof(int) * 7);
		create_symbol_table(str, i, stable);
		//getTokens(str,i);
		fflush(stdin);
		i++;
	}
	int length_symbol_table = i;
	i = 0;
	while (!feof(fp))
	{
		fgets(str, line_size, fp);
		//printf("%s\n", str);
		create_intermediate_table(str, i, itable, stable, length_symbol_table, lt);
		if (!in(str, ':'))
			i++;
		else
			lt_count++;
	}
	printf("\n\nIntermediate Code:");
	for (int j = 0; j < i; j++)
	{
		printf("\nind = %d, opcode = %d,	", itable[j]->index, itable[j]->opcode);
		for (int k = 0; k < 5; k++)
			if (itable[j]->parameters[k] != -1)
				printf("%d	", itable[j]->parameters[k]);
		printf("\n");
	}
	int intermediate_table_length = i;

	printf("\n\nlable table\n\n");

	for (int j = 0; j < lt_count-1; j++)
	{
		printf("%s\t%d", lt[j]->name,lt[j]->address);
	}

	printf("\n\nOutput:\n");
	perform_operation(itable, stable, length_symbol_table, intermediate_table_length);

	fclose(fp);
	char * filename_solution = (char *)malloc(sizeof(char) * 20);
	filename_solution = "solution.o";
	fp = fopen(filename_solution, "w");
	for (int j = 0; j < i; j++)
	{
		fprintf(fp,"%d,%d,", itable[j]->index, itable[j]->opcode);
		for (int k = 0; k < 5; k++)
			if (itable[j]->parameters[k] != -1)
				fprintf(fp,"%d,", itable[j]->parameters[k]);
		fprintf(fp,"\n");
	}
	fclose(fp);
	printf("\nOpcode files saved successfully.\n");
		/*for (int j = 0; j < i-1; j++)
			printf("name = %s, address = %d, size = %d\n", stable[j]->name, stable[j]->address, stable[j]->size);*/
			//printf("\ni = %d", i);
	cin.get();
	return 0;	
}