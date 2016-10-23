/**
* TEXBSave.cpp
* Saves TextureBank to TEXB
**/

#include "TEXB.h"

#include <string>
#include <sstream>
#include <vector>

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <stdint.h>

int32_t TextureBank::SaveToMemory(uint8_t*& Memory,size_t* MemorySize,uint32_t /*CompressLevel*/)
{
	if(MemorySize==NULL) return EINVAL;

	uint16_t temp_short=0;
	uint32_t temp_int=0;
	uint8_t* temp_buffer;
	uint8_t prebuf[4];

	std::stringstream sstream(std::ios::binary|std::ios::out);
	sstream.write("TEXB",4);
	sstream.write("\0\0\0\0",4);

	uint32_t nameSize=Name.length()+7;
	temp_short=(nameSize+1)/2*2;
	prebuf[0]=temp_short>>8;
	prebuf[1]=temp_short&255;
	sstream.write(reinterpret_cast<char*>(prebuf),2);
	sstream.put('T');
	sstream.write((Name+".texb").c_str(),nameSize-1);
	if(nameSize%2==1) sstream.put(0);

	/*temp_short=htons(RawImageWidth);
	sstream.write(reinterpret_cast<char*>(&temp_short),2);
	temp_short=htons(RawImageHeight);
	*/
	prebuf[0]=RawImageWidth>>8;
	prebuf[1]=RawImageWidth&255;
	prebuf[2]=RawImageHeight>>8;
	prebuf[3]=RawImageHeight&255;
	sstream.write(reinterpret_cast<char*>(prebuf),4);

	prebuf[0]=0;
	prebuf[1]=196;
	sstream.write(reinterpret_cast<char*>(prebuf),2);

	uint16_t tImgs=ImageList_Id.size();
	temp_short=tImgs*4;
	prebuf[0]=temp_short>>8;
	prebuf[1]=temp_short&255;
	temp_short=tImgs*6;
	prebuf[2]=temp_short>>8;
	prebuf[3]=temp_short&255;
	sstream.write(reinterpret_cast<char*>(prebuf),4);
	prebuf[0]=tImgs>>8;
	prebuf[1]=tImgs&255;
	sstream.write(reinterpret_cast<char*>(prebuf),2);

	for(uint32_t i=0;i<ImageList_Id.size();i++)
	{
		TextureImage* cur=ImageList_Id[i];
		sstream.write("TIMG\0\0",6);
		uint32_t curStreamSize=sstream.tellp();
		nameSize=cur->Name.length()+11;
		temp_short=(nameSize+1)/2*2;
		prebuf[0]=temp_short>>8;
		prebuf[1]=temp_short&255;
		sstream.write(reinterpret_cast<char*>(prebuf),2);
		sstream.put('I');
		sstream.write((cur->Name+".png.imag").c_str(),nameSize-1);
		if(nameSize%2==1) sstream.put(0);

		sstream.write("\xFF\xFF\x00\x01\x00\x00\x00\x00\x00\x01\x00\x01\x04\x06",14);
		prebuf[0]=cur->Width>>8;
		prebuf[1]=cur->Width&255;
		prebuf[2]=cur->Height>>8;
		prebuf[3]=cur->Height&255;
		sstream.write(reinterpret_cast<char*>(prebuf),4);
		sstream.write("\0\0\0\0",4);

		uint32_t* vrtxCur=VertexIndexUVs[i];

		for(uint32_t j=0;j<16;j++)
		{
			prebuf[0]=vrtxCur[j]>>24;
			prebuf[1]=(vrtxCur[j]>>16)&255;
			prebuf[2]=(vrtxCur[j]>>8)&255;
			prebuf[3]=vrtxCur[j]&255;
			sstream.write(reinterpret_cast<char*>(prebuf),4);
		}
		sstream.write(reinterpret_cast<char*>(vrtxCur)+64,6);

		temp_int=sstream.tellp();
		sstream.seekp(curStreamSize-2,std::ios::beg);
		temp_short=(temp_int-=curStreamSize);
		prebuf[0]=temp_short>>8;
		prebuf[1]=temp_short&255;
		sstream.write(reinterpret_cast<char*>(prebuf),2);
		sstream.seekp(0,std::ios::end);
	}

	temp_int=RawImageWidth*RawImageHeight*4;
	sstream.write(reinterpret_cast<char*>(RawImage),temp_int);
	
	temp_int=static_cast<int>(sstream.tellp())-8;
	prebuf[0]=temp_int>>24;
	prebuf[1]=(temp_int>>16)&255;
	prebuf[2]=(temp_int>>8)&255;
	prebuf[3]=temp_int&255;
	sstream.seekp(4,std::ios::beg);
	sstream.write(reinterpret_cast<char*>(prebuf),4);

	sstream.seekp(0,std::ios::end);
	temp_int=sstream.tellp();
	temp_buffer=LIBTEXB_ALLOC(uint8_t,temp_int);
	memcpy(temp_buffer,sstream.str().c_str(),temp_int);
	Memory=temp_buffer;
	*MemorySize=temp_int;

	return 0;
}

inline int32_t TextureBank::SaveToMemory(uint8_t** Memory,size_t* MemorySize,uint32_t /*CompressLevel*/)
{
	return SaveToMemory(*Memory,MemorySize);
}

int32_t TextureBank::SaveToFile(std::string Filename,uint32_t CompressLevel)
{
	FILE* fptr;
	uint8_t* temp_buffer;
	size_t bufsize;
	uint32_t ret;

	fptr=fopen(Filename.c_str(),"wb");
	if(fptr==NULL) return errno;

	ret=SaveToMemory(&temp_buffer,&bufsize,CompressLevel);
	if(ret==0)
	{
		fwrite(temp_buffer,1,bufsize,fptr);
		fclose(fptr);
	}

	LIBTEXB_FREE(temp_buffer);
	return ret;
}
