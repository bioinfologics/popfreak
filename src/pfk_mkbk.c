#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
	uint64_t kmer;
        uint16_t freq;
} kfreq_t;

int kfreq_cmp(const void * kf1, const void * kf2){
	if (((kfreq_t *)kf1)->kmer > ((kfreq_t *)kf2)->kmer) return 1;
	if (((kfreq_t *)kf2)->kmer > ((kfreq_t *)kf1)->kmer) return -1;
	return 0;
}

void kfreq_from_line(kfreq_t * dest, char * line, unsigned char k){
	dest->kmer=0;
	dest->freq=0;
	for (int i=0;i<k;i++){
		dest->kmer=dest->kmer*4;
		if (line[i]=='A') dest->kmer+=0;
		if (line[i]=='C') dest->kmer+=1;
		if (line[i]=='G') dest->kmer+=2;
		if (line[i]=='T') dest->kmer+=3;
	}
	for (int i=k+1;line[i];i++){
		dest->freq=dest->freq*10+line[i]-'0';
	}
}

int create_block_files(char * infile,char * outprefix, unsigned char k,uint64_t block_size){
    char line[1024];
    FILE *fp;
    int blocks=0;
    uint64_t kmer;
    uint64_t i;
    char outfilename[100];
    kfreq_t * kfreqs;
    kfreqs= malloc(sizeof(kfreq_t)*block_size);

    fp = fopen(infile,"r");
    if( fp == NULL ) {
        return 1;
    }
    
     
    while (!feof(fp)){
	
    	for(i=0; i<block_size &&  fgets(line,1024,fp);i++ ) {
	    kfreq_from_line(kfreqs+i,line,k);
    	}
	//sort kfreqs
	qsort (kfreqs,i,sizeof(kfreq_t),kfreq_cmp);
	//open file with appropiate name
	sprintf(outfilename,"%s.block%03d",outprefix,blocks);
	//write file and close
	FILE * outfile=fopen(outfilename,"w");
	fwrite(kfreqs,sizeof(kfreq_t),i,outfile);
	fclose(outfile);
        blocks++;
    }
    free(kfreqs);
    return blocks;
}

int main(int argc,char ** argv ) {
    
    char * infile= argv[1];
    unsigned char k=atoi(argv[2]);
    char * endptr;
    uint64_t block_size;
    block_size=strtoull(argv[3],&endptr,10);
    char * outprefix= argv[4];
    create_block_files(infile,outprefix,k,block_size);
    //merge_block_files();
    return 0;
}

/*
main(){
	FILE 
	allocate array of blocksize elements
	while not EOF
		while t>blocksize and not EOF
			//read a line
			//parse first k characters into a kmer
			//parse k+1: into a number
			insert k, count into array
			t++
			
		sort array (from pos 0 to t-1)
		write file (from pos 0 to t-1), files++
	free array
	merge files(files)
}*/
