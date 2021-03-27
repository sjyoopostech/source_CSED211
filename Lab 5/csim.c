/*
 * name : Yoo SeungJae
 * loginID : sjyoo
 */

#include "cachelab.h"
#include <getopt.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
	int hit;
	int miss;
	int eviction;
} count;

typedef struct {
	int lru;
	int valid;
	unsigned long long tag;
	char *block;
} line;

typedef struct {
	line *lines;
} set;

typedef struct {
	set *sets;
} cache;

cache makecache(long long S, int E, long long B) {
	cache c;
	set s;
	line l;
	c.sets = (set*)malloc(sizeof(set)*S);
	for (int i = 0; i < S; i++) {
		s.lines = (line*)malloc(sizeof(line)*E);
		c.sets[i] = s;
		for (int j = 0; j < E; j++) {
			l.lru = 0;
			l.valid = 0;
			l.tag = 0;
			s.lines[j] = l;
		}
	}
	return c;
}

void freecache(cache c, long long S, int E, long long B) {
	for (int i = 0; i < S; i++) {
		set s = c.sets[i];
		if (s.lines != NULL) free(s.lines);
	}
	if (c.sets != NULL) free(c.sets);
}

void printoption() {
	printf("-h : Optional help flag that prints usage info\n");
	printf("-v : Optional verbose flag that displays trace info\n");
	printf("-s <s> : Number of set index bits\n");
	printf("-E <E> : Associativity\n");
	printf("-b <b> : Number of block bits\n");
	printf("-t <tracefile> : Name of the valgrind trace to replay\n");
}

int least(set s, int E) {
	int minimum = s.lines[0].lru;
	int index = 0;
	for (int i = 1; i < E; i++) {
		if (minimum > s.lines[i].lru) {
			minimum = s.lines[i].lru;
			index = i;
		}
	}
	return index;
}

int most(set s, int E) {
	int maximum = s.lines[0].lru;
	for (int i = 1; i < E; i++) {
		if (maximum < s.lines[i].lru) {
			maximum = s.lines[i].lru;
		}
	}
	return maximum;
}

count simulator(cache c, count cachecount, unsigned long long address, int S, int E, int B, int v) {

	int ishit = 0;
	int isfull = 1;
	int emptyindex=0;
	int tsize = 64-S-B;
	unsigned long long input = address >> (S+B);
	unsigned long long i = (address << tsize) >> (tsize+B);

	set s = c.sets[i];
	for (int j = 0; j < E; j++) {
		if (s.lines[j].valid) {
			if (s.lines[j].tag == input) {
				s.lines[j].lru = most(s, E)+1;
				cachecount.hit++;
				if (v) printf(" hit");
				ishit = 1;
			}
		}
		else if (isfull) {
			isfull = 0;
			emptyindex = j;
		}
	}
	if (ishit) {
		c.sets[i] = s;
		return cachecount;
	}
	cachecount.miss++;	
	if (v) printf(" miss");

	if (isfull) {
		cachecount.eviction++;
		if (v) printf(" eviction");
		int evicindex = least(s, E);		
		s.lines[evicindex].lru = most(s, E) + 1;
		s.lines[evicindex].tag = input;
	}
	else {
		s.lines[emptyindex].lru = most(s, E) + 1;
		s.lines[emptyindex].valid = 1;
		s.lines[emptyindex].tag = input;
	}
	c.sets[i] = s;
	return cachecount;
}

int main(int argc, char **argv) {

	int option;
	int s=0, E=0, b=0, v=0;
	char *t;

	while ((option=getopt(argc, argv, "vhs:E:b:t:")) != -1) {
		switch (option) {
			case 'v' :
				v = 1;
				break;
			case 'h' :
				printoption();
				exit(0);
			case 's' :
				s = atoi(optarg);
				break;
			case 'E' :
				E = atoi(optarg);
				break;
			case 'b' :
				b = atoi(optarg);
				break;
			case 't' :
				t = optarg;
				break;
			default :
				printoption();
				exit(1);
		}
	}

	if ((s==0)|(E==0)|(b==0)|(t==NULL)) {
		printoption();
		exit(1);
	}

	long long S = pow(2.0, s);
	long long B = pow(2.0, b);
	count cachecount;
	cachecount.hit = 0;
	cachecount.miss = 0;
	cachecount.eviction = 0;
	cache c = makecache(S, E, B);
	FILE *input = fopen(t, "r");

	char command;
	unsigned long long address;
	int size;

	if (input != NULL) {
		while (fscanf(input, " %c %llx,%d", &command, &address, &size) == 3) {
			if (v) printf("%c %llx,%d", command, address, size);
			switch(command) {
				case 'I' :
					break;
				case 'L' :
					cachecount = simulator(c, cachecount, address, s, E, b, v);
					break;
				case 'S' :
					cachecount = simulator(c, cachecount, address, s, E, b, v);
					break;
				case 'M' :
					cachecount = simulator(c, cachecount, address, s, E, b, v);
					cachecount = simulator(c, cachecount, address, s, E, b, v);
					break;
			}
			if (v) printf("\n");
		}
	}

	printSummary(cachecount.hit, cachecount.miss, cachecount.eviction);
	freecache(c, S, E, B);
	fclose(input);
	return 0;
}
