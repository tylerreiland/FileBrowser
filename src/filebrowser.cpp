#include <iostream>
#include <SDL2/SDL.h>
#include <dirent.h>  //error with this
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <bitset>
#include <SDL2/SDL_ttf.h>

typedef struct FileEntry{
    std::string filename;
    bool isDir;
}FileEntry;

typedef struct Rectangles{
    std::vector<int> x;
    std::vector<int> y;
    std::vector<int> w;
    std::vector<int> h;
    std::vector<std::string> filename;
}Rectangles;

void EventLoop(SDL_Renderer *renderer, int *mouseX, int *mouseY, bool *pressed, Rectangles *rects, std::string *path);
void Render(SDL_Renderer *renderer, std::string *directory, Rectangles *rects);
void GetDirContents(const char *path, std::vector<FileEntry> *files);
SDL_Texture* RasterizeText(std::string text, std::string font_file, int font_size, SDL_Color color, SDL_Renderer *renderer);
bool PointInRectangle(int *mouseX, int *mouseY, int rectX, int rectY, int rectW, int rectH);


int main(int argc, char **argv)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error: could not initialize SLD2\n");
        exit(1);
    }
    
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    if(SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer) != 0)
    {
        fprintf(stderr, "Error: could not create window\n");
        exit(1);
    }
    
    /*std::vector<FileEntry> files = GetDirContents(".");
    for(int i=0; i<files.size(); i++)
    {
        printf("%s\n", files[i].filename.c_str());
    } */
    
    TTF_Init();
    
    int mouseX = 0;
    int mouseY = 0;
    bool pressed = false;
    Rectangles rects;
    
    //EventLoop(renderer, &mouseX, &mouseY, &pressed);
    
    std::string path = getenv("HOME");
    
    Render(renderer, &path, &rects);
    EventLoop(renderer, &mouseX, &mouseY, &pressed, &rects, &path);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}


void EventLoop(SDL_Renderer *renderer, int *mouseX, int *mouseY, bool *pressed, Rectangles *rects, std::string *path)
{
    bool running = true;
    SDL_Event event;
    
    while(running)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                running = false;
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    *pressed = true;
                    for(int i=0; i<rects->x.size(); i++)
                    {
                        if(PointInRectangle(mouseX, mouseY, rects->x[i], rects->y[i], rects->w[i], rects->h[i]) )
                        {
                            //path = path + / + filename
                            *path = *path + "/" + rects->filename[i];
                            Render(renderer, path, rects);
                            break;
                        }
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                *mouseX = event.motion.x;
                *mouseY = event.motion.y;
                break;
        }
    }
}

void Render(SDL_Renderer *renderer, std::string *directory, Rectangles *rects)
{
	//reset rects
	rects->x.clear();
	rects->y.clear();
	rects->w.clear();
	rects->h.clear();
	rects->filename.clear();

    SDL_SetRenderDrawColor(renderer, 36,52,71, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    //current directory contents
    std::vector<FileEntry> files;
    GetDirContents(directory->c_str(), &files);
    
    SDL_Rect rectangle;
    rectangle.x = 20;
    int y = 20;
    rectangle.w = 30;
    rectangle.h = 20;
    
    SDL_Rect rectPath;
    rectPath.x = 60;
    int yPath = 20;
    
    SDL_Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;
    
    uint32_t format;
    int access, width, height;
    SDL_Texture *name;
    
    int counter = 0;

    for(int i=0; i<files.size(); i++)
    {
        if(files[i].isDir)
        {
            //red
            SDL_SetRenderDrawColor(renderer, 181, 38, 30, SDL_ALPHA_OPAQUE);
        }
        else
        {
            //blue
            SDL_SetRenderDrawColor(renderer, 11, 140, 242, SDL_ALPHA_OPAQUE);
        }
        
        if(i>0)
        {
            y += 30;
            yPath += 30;
        }
        
        counter++;
    	if(counter>=15)
    	{
    		//draw to the right if 15 files drawn already
    		rectangle.x += 250;
    		y = 20;
    		rectPath.x += 250;
    		yPath = 20;
    		counter = 0;
    	}

        //draw icon
        rectangle.y = y;
        SDL_RenderFillRect(renderer, &rectangle);
        if(files[i].isDir)
        {
            rects->x.push_back(rectangle.x);
            rects->y.push_back(rectangle.y);
            rects->w.push_back(rectangle.w);
            rects->h.push_back(rectangle.h);
            rects->filename.push_back(files[i].filename);
        }
        
        //draw file name
        name = RasterizeText(files[i].filename, "./OpenSans/OpenSans-Regular.ttf", 14, color, renderer);
        SDL_QueryTexture(name, &format, &access, &width, &height);
        rectPath.y = yPath;
        rectPath.w = width;
        rectPath.h = height;
        SDL_RenderCopy(renderer, name, NULL, &rectPath);
        SDL_DestroyTexture(name);
        
        /*if(files[i].isDir && *pressed && PointInRectangle(mouseX, mouseY, rectangle.x, rectangle.y, rectangle.w, rectangle.h))
        {
            
            //redo with files[i] as new directory
        }*/
        //SDL_RenderFillRect(renderer, &rectPath);
        
    }
    
    
    SDL_RenderPresent(renderer);
    

}

void GetDirContents(const char *path, std::vector<FileEntry> *files)
{
    DIR *dir;
    FileEntry currFile;
    int index;
    struct dirent *entry;

    if (!(dir = opendir(path)))
    {
        return;
	}
    while ((entry = readdir(dir)) != NULL)
    {
        index = 0;
            if( !(entry->d_name[0] == '.') || (strcmp(entry->d_name, "..") == 0) )
            {
                currFile.filename = entry->d_name;
                currFile.isDir = entry->d_type == DT_DIR;
                for(int i=0; i<files->size(); i++)
                {
                    if(strncasecmp(files->at(i).filename.c_str(), currFile.filename.c_str(), 256) < 0)
                    {
                        index++;
                    }
                }
                files->insert(files->begin() + index, currFile);
                
                
            }
    }
    closedir(dir);
}

SDL_Texture* RasterizeText(std::string text, std::string font_file, int font_size, SDL_Color color, SDL_Renderer *renderer)
{
    TTF_Font *font = TTF_OpenFont(font_file.c_str(), font_size);
    if(font == NULL)
    {
        fprintf(stderr, "Error: could not open font\n");
        return NULL;
    }
    
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if(surface == NULL)
    {
        fprintf(stderr, "Error: could not render text to surface\n");
        return NULL;
    }
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(texture == NULL)
    {
        fprintf(stderr, "Error: could not convert surface to texture\n");
    }
    
    SDL_FreeSurface(surface);
    return texture;
}

bool PointInRectangle(int *mouseX, int *mouseY, int rectX, int rectY, int rectW, int rectH)
{
    if(*mouseX >= rectX && *mouseX <= (rectX + rectW) )
    {
        if(*mouseY > rectY && *mouseY <= (rectY + rectH) )
        {
            return true;
        }
    }   
    return false;
}





