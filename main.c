#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL/SDL.h>


#ifdef __MINGW32__
#undef main
#endif

void MD_extract(int argc, char** argv);


int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    MD_extract(argc,argv);

    SDL_Quit();

    return 0;

}

void MD_extract(int argc, char** argv)
{
    SDL_Surface *image,*screen;
    FILE *file;
    unsigned char *pixels;
    SDL_Rect position;
    int addr = 0,d,isize = 0;
    
    if(argc < 2)
    {
        printf("no argument\n");
        return;
    }
    
    int H = 0;
    int V = 0;
    
    if(argc > 2)
		H = strtol(argv[2],NULL,0);
		
	if(argc > 3)
		V = strtol(argv[3],NULL,0);
    
    if(argc > 4)
		addr = strtol(argv[4],NULL,0);

	if(argc > 5)
		isize = strtol(argv[5],NULL,0);
	
	
	file = fopen(argv[1],"rb");

    if(file == NULL)
    {
        printf("Open failed rom\n");
        return;
    }

    image = SDL_CreateRGBSurface(0,8,8,24,0,0,0,0);
    pixels = image->pixels;
    position.x = 0;
    position.y = 0;

    fseek(file, 0, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0, SEEK_SET);


	unsigned char *buffer = malloc(size);
    if(buffer == NULL)
	{
		fclose(file);
		return;
	}
	fread(buffer,1,size,file);
	fclose(file);

    printf("ROM : %x bytes\n",size);
    
    int w = 4096;
    int h = 4096;
    
    size -= addr;
    
    if(isize != 0) size = isize;
    
    if(size <= 0x400000)
    {
		w = 2048;
		h = 4096;
	}

	if(size <= 0x200000)
    {
		w = 2048;
		h = 2048;
	}
	
	if(size <= 0x100000)
    {
		w = 1024;
		h = 2048;
	}
	
	if(size <= 0x80000)
    {
		w = 1024;
		h = 1024;
	}
	
	if(size <= 0x40000)
    {
		w = 512;
		h = 1024;
	}

    if(size <= 0x20000)
    {
		w = 512;
		h = 512;
	}
	
	if(size <= 0x10000)
    {
		w = 256;
		h = 512;
	}
	
	if(size <= 0x8000)
    {
		w = 256;
		h = 256;
	}
	
	printf("%d %d\n",w,h);
	
	screen = SDL_CreateRGBSurface(0,w,h,24,0,0,0,0);

	char str[100];

	int i,l=0,k,n = 0,color,x,y;
	unsigned char tile[32];
	
	int dn = 1;
	
	H = H&3;
	V = V&3;
	if(H > 0) dn = H+1;
	if(V > 0) dn = dn*(V+1);
	
	int sw = 8;
	int sh = 8;
	
	sw *= (H+1);
	sh *= (V+1);
	
	int iv,ih;
	
	printf("%d : %d %d\n",dn,sw,sh);
	
	for(i = 0;i < size;)
	{
		iv = 0;
		ih = 0;
		SDL_Rect pos = position;
		
		for(d = 0; d < dn;d++)
		{
			memcpy(tile,buffer+addr,32);
			addr += 32;
			l = 0;
			for(y = 0;y <8;y++)
			{
				for(x = 0;x < 4;x++)
				{
					k = x + (y*4);
					color = (tile[k]&0xF0);
					color = 240 - color;
					pixels[l+0] = color;
					pixels[l+1] = color;
					pixels[l+2] = color;
					l+=3;
					
					color = (tile[k]&0x0F)<<4;
					color = 240 - color;
					pixels[l+0] = color;
					pixels[l+1] = color;
					pixels[l+2] = color;
					l+=3;

				}
			}
				
			i+= 32;
			
			
			SDL_BlitSurface(image,NULL,screen,&pos);
			
			if(iv == V)
			{
				pos.y = position.y;
				iv = 0;
				
				if(ih == H)
				{
					pos.x = position.x;
					ih = 0;
				}else
				{
					pos.x += 8;
					ih++;
				}
				
			}else
			{
				pos.y += 8;
				iv++;
			}
			
			
			
			
			
		}
		position.x += sw;
		if(position.x >= w)
		{
			position.x = 0;
			position.y += sh;
		}

		if(position.y >= h)
		{
			position.x = 0;
			position.y = 0;
			break;
			n++;
		}
		
	}
	
	sprintf(str,"save.bmp");
	SDL_SaveBMP(screen,str);
		
	free(buffer);
    SDL_FreeSurface(image);
	SDL_FreeSurface(screen);
}
