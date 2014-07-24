#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define STRUCT_ALLOC_SIZE 1000000


int uint64_cmp(const void* a, const void* b) {
	uint64_t x = *(uint64_t*)a;
	uint64_t y = *(uint64_t*)b;
	return x - y;	
}

int main(int argc,char ** argv ) {
    FILE * infile;
    uint64_t cont_size=STRUCT_ALLOC_SIZE,elements=0;
    uint64_t * bfs;
    char * endptr;
    uint64_t cutoff;
    cutoff=strtoull(argv[2],&endptr,10); 
    //for each pair of arguments open a file and set its frequency limit (till the last argument, which is the output
    infile=fopen(argv[1],"r");
    bfs=malloc(STRUCT_ALLOC_SIZE*sizeof(uint64_t));


    //while not eof
    uint16_t read_freqs[64];
    uint64_t kbf[3];//third position is to cast it as kbf struct
    uint64_t count=0;
    while (!feof(infile)) {
	
    	//read next bitfield
    	fread(kbf,sizeof(uint64_t),2,infile);//read kmer and bitfield
    	//skip as many freqs as bits on
	//Caculate Hamming Weight TODO: optimize (Reingold and Nievergelt, 1977)
	//premature optimization is the root of all evil
	int w=0;
	for (int i=0; i<64; i++) if (kbf[1]&((uint64_t)1)<<i) w++;
	fread(read_freqs,sizeof(uint16_t),w,infile);
	//Insert at the end (grow if needed first
    	if (cont_size==elements){
		cont_size+=STRUCT_ALLOC_SIZE;
		bfs=realloc(bfs,sizeof(uint64_t)*cont_size);
	}
	bfs[elements++]=kbf[1];
    }
    //sort the array
    qsort(bfs,elements,sizeof(uint64_t),uint64_cmp);
    //print the collection
    uint64_t lastv=0,vcount=0;
    for (uint64_t i=0;i<elements;i++){
	if (bfs[i]!=lastv){
		if (vcount>cutoff) printf("%lld,%lld\n",lastv,vcount);
		vcount=0;
		lastv=bfs[i];
	}
	vcount++;
    }
    if (vcount>cutoff) printf("%lld,%lld\n",lastv,vcount);

    return 0;
}
