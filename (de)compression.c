#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"lz4.h"

// acceleration to max for LZ4_compress_fast()
#define LZ4_ACCELERATION_MAX 65537



size_t write_u32(FILE* fp,uint32_t i){return fwrite(&i,sizeof(i),1,fp);}
size_t write_bin(FILE* fp,const void* array,size_t arrayBytes){return fwrite(array,1,arrayBytes,fp);}
size_t read_u32(FILE* fp,uint32_t* i){return fread(i,sizeof(*i),1,fp);}
size_t read_bin(FILE* fp,void* array,size_t arrayBytes){return fread(array,1,arrayBytes,fp);}


FILE* read;
FILE* write;
//examples of full paths
char file[]="C:\\Users\\nik\\Documents\\email.txt";
char file_com[]="C:\\Users\\nik\\Documents\\email.txt.lz4";


//compress function
void compress(){
	char inpBuf[2][1024*64];
	unsigned int  inpBufIndex=0;
	char* inpPtr;
	int inpBytes;
	uint32_t cmpBytes;
    unsigned long long int test;

	LZ4_stream_t stream_body;
	LZ4_stream_t* stream=&stream_body;

    /*lines 35-38 test if file is to small for compression.If you dont need them you can remove them */
    read=fopen(file,"rb");
	if(read==NULL){return;}
	fseek(read,0,SEEK_END);
    test=ftell(read);
    if(test<5120){fclose(read); return;}
    rewind(read);
	write=fopen(file_com,"wb");
    if(write==NULL){return;}

	LZ4_initStream(stream,sizeof(*stream));
	for(;;){
        inpPtr=inpBuf[inpBufIndex];
		inpBytes=(int)read_bin(read,inpPtr,1024*64);
		if(0==inpBytes){break;}{
		    char cmpBuf[LZ4_COMPRESSBOUND(1024*64)];
			cmpBytes=(uint32_t)LZ4_compress_fast_continue(stream,inpPtr,cmpBuf,inpBytes,sizeof(cmpBuf),1);
			if(cmpBytes<=0){break;}
			write_u32(write,cmpBytes);
			write_bin(write,cmpBuf,(size_t)cmpBytes);}
		inpBufIndex =(inpBufIndex+1)%2;}
	write_u32(write,0);
	fclose(read);
	fclose(write);
	remove(file);}

//decompress function
void decompress(){

    char de_buf[2][1024*64];
    char* Buf1=de_buf[0];
    char* Buf2=de_buf[1];
    uint32_t cmpBytes;
    size_t read_count;
    size_t read_count1;
    int de_bytes;

    LZ4_streamDecode_t stream_body;
    LZ4_streamDecode_t* stream=&stream_body;

    read=fopen(file_com,"rb");
    if(read==NULL){return;}
    write=fopen(file,"wb");
    if(write==NULL){return;}

    LZ4_setStreamDecode(stream,NULL,0);
    for(;;){
        char cmpBuf[LZ4_COMPRESSBOUND(1024*64)];
        cmpBytes=0;{
            read_count=read_u32(read,&cmpBytes);
            if(read_count!=1||cmpBytes==0){break;}
            read_count1=read_bin(read,cmpBuf,cmpBytes);
            if(read_count1!=cmpBytes){break;}}{
            de_bytes=LZ4_decompress_safe_continue(stream,cmpBuf,Buf1,cmpBytes,1024*64);
            if(de_bytes<=0){break;}
            write_bin(write,Buf1,(size_t)de_bytes);
            char* tmp=Buf2;
            Buf2=Buf1;
            Buf1=tmp;}}
    fclose(read);
    fclose(write);
    remove(file_com);}



int main(){

    /*call compress or decompress
    compress();
    decompress(); */


    return 0;}
