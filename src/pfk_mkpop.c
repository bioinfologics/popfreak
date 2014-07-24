#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
	uint64_t kmer;
        uint16_t freq;
} kfreq_t;

// block header (every 1000 kmers, last block will include less kmers)
// first kmer on block (64 bit)
// last kmer on block (64 bit)
// block filesize ( 64 bit, excluding header)
//record structure for each kmer:
// kmer (64bit)
// presence_bitfield (64bit)
// frequency (16 bits per "on" bit on the bitfield, variable)

//returns 0 if the file finished, 1 if a record was read

//TODO: add another function that keeps looking for stuff that is not past the cutoff but is still there.
int read_next_record(const kfreq_t * dest, FILE * file/*, uint32_t minfreq*/){
	//do {
		if (fread((void *)dest,sizeof(kfreq_t),1,file)==0) return 0;
	//} while(dest->freq<minfreq);
	return 1;
}

void condense_files(unsigned int n, FILE ** infiles, unsigned int * minfreqs, FILE * outfile){
	printf("condensing %d files\n",n);
	kfreq_t records[64];
	unsigned char file_ends[64];
	for( int i=0;i<n;i++){
		if (0==read_next_record(records+i,infiles[i])) file_ends[i]=1;
		else file_ends[i]=0;
	}
	unsigned char closed_files=0;
	uint64_t count=0,ok_count=0;
	//TODO: write first empty header
	while(closed_files<n){
		//finds min and sets bitfield, all in one go
		unsigned char passed_cutoff=0;
		uint64_t cbitfield=0,bitfield=0;
		uint64_t min=UINT64_MAX;
		for(int i=0;i<n;i++){
			if (file_ends[i]) continue;
			if (records[i].kmer<min) {
				min=records[i].kmer;
				bitfield=0;
				cbitfield=0;
				passed_cutoff=0;
			}
			if (records[i].kmer==min) {
				bitfield+=1<<i;
				if (minfreqs[i]<=records[i].freq) {
					passed_cutoff=1;
					cbitfield+=1<<i;
				}
			}
		}
		//printf("Min kmer found: %lld, bitfield=%lld",min,bitfield);
		//write kmer and bitfield
		if (passed_cutoff){
			ok_count++;
			fwrite(&min,sizeof(uint64_t),1,outfile);
			fwrite(&cbitfield,sizeof(uint64_t),1,outfile);
		}
		//for each bit turned on, write freq, 

		for(int i=0;i<n;i++){
			if(bitfield&1<<i){
				//write freq
				if (cbitfield&1<<i){
					fwrite(&records[i].freq,sizeof(uint16_t),1,outfile);
				}
				//read next record with cutoff
				if (0==read_next_record(records+i,infiles[i])){
					//if no record mark file as ended and closed_file++
					file_ends[i]==1;
					closed_files++;
				}
			}
		}
		//TODO: if 10000 kmers have passed, go back and fill the header and then put a dummy header on
		if (0==++count%10000000) printf("%d records processed - %d condensed\n",count,ok_count);
	}
	//fill the header for the last block
}

int main(int argc,char ** argv ) {
    FILE * infiles[64];
    unsigned int minfreqs[64];
    FILE * outfile;
    //for each pair of arguments open a file and set its frequency limit (till the last argument, which is the output
    for (int i=1;i<argc-2;i+=2){
	infiles[(i-1)/2]=fopen(argv[i],"r");
	minfreqs[(i-1)/2]=atoi(argv[i+1]);
	printf ("File %s opened with limit %d\n",argv[i],minfreqs[(i-1)/2]);
    }
    //open the output file
    outfile=fopen(argv[argc-1],"w");
    printf ("Output %s opened\n",argv[argc-1]);
    //call the merge function
    condense_files((argc-2)/2,infiles,minfreqs,outfile);
    return 0;
}
