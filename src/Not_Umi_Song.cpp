/**
* Not_Umi_Song.cpp
* Watashitachi wa Mirai no Hana; sorry wrong title :v
* CLI TEXB generator
**/

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

#include <cstring>
#include <cmath>

#include <TEXB.h>

#include <stdint.h>

static TextureBank* g_TEXB=NULL;

struct Flowers_TIMGInfo
{
	std::string name;
	Point pos;
	uint32_t width;
	uint32_t height;
};

std::vector<Flowers_TIMGInfo*> g_TIMGList;

inline const char* Basename(const char* a)
{
	const char* b=strrchr(a,'/'),*c = strrchr(a,'\\');
	return b==c?a:(std::max(b,c)+1);
}

// Meant to be used with strtok
template<typename T> inline T* Flowers_MustNotNULL(T* expression)
{
	if(expression==NULL)
	{
		std::cerr << "Invalid format!" << std::endl;
		exit(1);
	}
	return expression;
}

inline uint32_t Flowers_MustANumber(const char* str)
{
	unsigned long num;
	char* end;

	if((num=strtoul(str,&end,0))==0 && *end!=0)
	{
		std::cerr << "Invalid number!" << std::endl;
		exit(1);
	}
	
	return num;
}

void GenerateTIMGBlack(uint32_t x,uint32_t y,uint32_t width,uint32_t height,std::string name)
{
	static const char defaultFill[]={0,0,0,(-1)};
	static const uint32_t defaultFillUInt32=*reinterpret_cast<const uint32_t*>(defaultFill);

	Point Locations[2]={
		{x,y},
		{width,height}
	};
	uint32_t Index;
	uint32_t Area=width*height;
	uint32_t* Raw=NULL;
	TextureImage* temp=NULL;

	// Create and get the TIMG raw buffer
	g_TEXB->DefineImage(Locations,name,&Index);
	temp=g_TEXB->FetchImage(Index);
	Raw=reinterpret_cast<uint32_t*>(temp->RawImage);

	for(uint32_t i=0;i<Area;i++)
		Raw[i]=defaultFillUInt32;
}

int main(int argc,char* argv[])
{
	std::cerr << "Future of the Flowers. Blank TEXB Generator" << std::endl;
	if(argc<4)
	{
		std::cerr << "Usage: " << Basename(argv[0]) << " <texb name> <timg name:pos x:pos y:width:height> [timg name...] ... <texb output path>" << std::endl;
		return 1;
	}

	std::string TEXBName=argv[1];
	std::string TEXBOutput=argv[argc-1];

	uint32_t Width=0;
	uint32_t Height=0;

	for(int i=2;i<argc-1;i++)
	{
		char* temp=new char[strlen(argv[i])+1];
		Flowers_TIMGInfo* finfo=new Flowers_TIMGInfo;
		strcpy(temp,argv[i]);

		// Parse the information
		finfo->name=Flowers_MustNotNULL(strtok(temp,":"));
		finfo->pos.X=Flowers_MustANumber(strtok(NULL,":"));
		finfo->pos.Y=Flowers_MustANumber(strtok(NULL,":"));
		finfo->width=Flowers_MustANumber(strtok(NULL,":"));
		finfo->height=Flowers_MustANumber(strtok(NULL,":"));

		// Determine max width and height of our generated TEXB
		Width=std::max(Width,uint32_t(pow(2.0,ceil(log(double(finfo->pos.X+finfo->width))/log(2.0)))));
		Height=std::max(Height,uint32_t(pow(2.0,ceil(log(double(finfo->pos.Y+finfo->height))/log(2.0)))));

		// OK
		g_TIMGList.push_back(finfo);
		delete[] temp;
	}

	g_TEXB=new TextureBank(Width,Height);
	g_TEXB->Name=TEXBName;

	for(std::vector<Flowers_TIMGInfo*>::iterator i=g_TIMGList.begin();i!=g_TIMGList.end();i++)
	{
		Flowers_TIMGInfo* f=*i;
		GenerateTIMGBlack(f->pos.X,f->pos.Y,f->width,f->height,f->name);

		delete f;
	}

	g_TEXB->ReflectChanges();
	g_TEXB->SaveToFile(TEXBOutput);	// It will always saved as uncompressed. No, I'm serious.
	delete g_TEXB;
	return 0;
}