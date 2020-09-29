/* 
 * PYTHAGORAS
 *
 * a small program to show the pythagorean theorem
 * written in C with the SDL2 library
 *
 * written by 
 *   __      _     _
 *  / _| ___(_) __| | ___ _ __
 * | |_ / _ \ |/ _` |/ _ \ '__|
 * |  _|  __/ | (_| |  __/ |
 * |_|  \___|_|\__,_|\___|_|
 *
 * for the German Blitz Basic forum (www.blitzforum.de)
 *
 * Have fun!
 *
 */



#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "stdio.h"
#include "math.h"

enum Gamestate{
    MENU,
    EX1,
    EX2,
    QUIT,
};

void init_ex1();
void init_ex2();

/* globals */
SDL_Window * window;    
SDL_Renderer * renderer;
SDL_Texture * screen;
const Uint8 * keystate;
Uint8 * keyhit;
Uint8 * keyhit_prev;
int num_keys;
enum Gamestate gamestate;
TTF_Font * font;
SDL_Texture ** texts;
Uint8 print_color[3];
char strbuf[1000];

/* globals both ex */
double a, b, c, p, q, h;
double scale = 5;
int px_mx = 320;
int px_my = 240;
int px_u ;
int px_d ;
int px_l ;
int px_r;

/* globals ex 1 */
int ex1_show_lines;


/* globals ex 2 */
int ex2_state;
/* 0 =  triangle, 
 * 1 =  h shown, 
 * 2 =  q and p shown
 * 3 =  rectangle
 * 4 =  formulas
 * */



/* init the program */
int init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Could not init SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "1");

    window = SDL_CreateWindow(
            "pythagoras - feider",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640, 480,
            SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(
            window, -1, 0);
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    num_keys = 0;
    keystate = SDL_GetKeyboardState(&num_keys);
    keyhit = (Uint8 *) malloc(sizeof(Uint8)*num_keys);
    keyhit_prev = (Uint8 *) malloc(sizeof(Uint8)*num_keys);
    memset(keyhit, 0, sizeof(Uint8)*num_keys);
    memset(keyhit_prev, 0, sizeof(Uint8)*num_keys);

    gamestate = MENU;

    TTF_Init();
    font = TTF_OpenFont("LiberationMono-Regular.ttf", 16);

    return 0;
}

void set_print_color(Uint8 r, Uint8 g, Uint8 b)
{
    print_color[0] = r;
    print_color[1] = g;
    print_color[2] = b;
}


void print(char *text, int x, int y)
{
    SDL_Color c = {print_color[0], print_color[1], print_color[2], 255};
    SDL_Surface * sf = TTF_RenderText_Solid(font, text, c);
    SDL_Texture * t = SDL_CreateTextureFromSurface(renderer, sf);
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = sf->w;
    r.h = sf->h;
    SDL_FreeSurface(sf);
    SDL_SetRenderTarget(renderer, screen);
    SDL_RenderCopy(renderer, t, NULL, &r);
    SDL_DestroyTexture(t);
}


void key_logic()
{
    for(int k = 0; k<num_keys; k++)
    {
        /* key is hit */
        if(keystate[k] == 1 && keyhit_prev[k] == 0 && keyhit[k] == 0)
        {
            keyhit[k] = 1;
            keyhit_prev[k] = 1;
            continue;
        }
        /* key is held */
        if(keystate[k] == 1 && keyhit_prev[k] == 1 && keyhit[k] == 1)
        {
            keyhit[k] = 0;
            continue;
        }

        /* key is released */
        if(keystate[k] == 0 && keyhit_prev[k] == 1)
        {
            keyhit[k] = 0;
            keyhit_prev[k] = 0;
            continue;
        }
    }
}

void menu_logic()
{
    if(keyhit[SDL_SCANCODE_1])
    {
        printf("enter exp 1\n");
        init_ex1();
        gamestate = EX1;
    }
    if(keyhit[SDL_SCANCODE_2])
    {
        init_ex2();
        printf("enter exp 2\n");
        gamestate = EX2;
    }
    if(keyhit[SDL_SCANCODE_ESCAPE])
    {        
        printf("quit\n");
        gamestate = QUIT;
    }
}

void render_menu()
{
    set_print_color(255, 255, 255);
    print("1 - Example 1", 10, 10);
    print("2 - Example 2", 10, 30);
    print("ESC - Quit", 10, 50);
}

void ex1_logic()
{
    if(keyhit[SDL_SCANCODE_ESCAPE])
    {        
        printf("enter menu\n");
        gamestate = MENU;
    }
    if(keyhit[SDL_SCANCODE_LEFT])
    {
        b -= 1;
        if(b < 1) b = 1;
    }
    if(keyhit[SDL_SCANCODE_RIGHT])
    {
        b += 1;
    }
    if(keyhit[SDL_SCANCODE_UP])
    {
        a += 1;
    }
    if(keyhit[SDL_SCANCODE_DOWN])
    {
        a -= 1;
        if(a < 1) a = 1;
    }

    if(keyhit[SDL_SCANCODE_SPACE])
        ex1_show_lines = 1- ex1_show_lines;

    c = sqrt( (a*a) + (b*b) );
    p = (a*a)/c;
    q = (b*b)/c;
    h = sqrt(p*q);
}

void init_ex1()
{
    a = 16;
    b = 12;
    ex1_show_lines = 0;
}


int compute_xb(double x, double y)
{
    double dx = q/b;
    double dy = -p/a;
    return px_l + round ( ( (dx*x) + (dy*y) ) *scale );
}
int compute_yb(double x, double y)
{
    double dx = -h/b;
    double dy = -h/a;
    return px_d + round ( ( (dx*x) + (dy*y) ) *scale );
}

int compute_xa(double x, double y)
{
    double dx = p/a;
    double dy = q/b;
    return px_mx + round ( ( (dx*x) + (dy*y) ) *scale );
}
int compute_ya(double x, double y)
{
    double dx = h/a;
    double dy = -h/b;
    return px_u + round ( ( (dx*x) + (dy*y) ) *scale );
}

int compute_yc(int num)
{
    int mod = floor(c);
    return px_d + floor(((num%mod)+0.5)*scale);
}

int compute_xc(int num)
{
    double y = floor( (double)num / floor(c) );
    return px_l + round (y*scale);
}

void render_ex1()
{
    set_print_color(255, 255, 255);
    print("ESC - Menu  SPACE - toggle line", 10, 10);
    print("up: a+1, down: a-1 left: b+1, right: b-1", 10, 30);
    sprintf(strbuf, "a: %f, b: %f, c:%f", a, b, c);
    print(strbuf, 10, 50);


    px_u = px_my-((h/2.0)*scale);
    px_d = px_my+((h/2.0)*scale);
    px_l = px_mx-(q*scale);
    px_r = px_mx+(p*scale);

    SDL_SetRenderTarget(renderer, screen);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0xFF);

    SDL_RenderDrawLine(renderer, px_l, px_d, px_mx, px_u);
    SDL_RenderDrawLine(renderer, px_mx, px_u, px_r, px_d);
    SDL_RenderDrawLine(renderer, px_r, px_d, px_l, px_d);

    int n = 0;
    double n_max = (a*a) + (b*b);
    double frac = 0;

    for(double x = 1; x<=b; x++)
        for(double y = 1; y<=b; y++)
        {
            frac = (y+(x*b))/(b*b);
            if(frac > 1) frac = 1;
            if(frac < 0) frac = 0;
            SDL_SetRenderDrawColor(renderer, 255*frac, 255*(1-frac), 255, 0xFF);
            int xb = compute_xb(x-0.5, y);
            int yb = compute_yb(x-0.5, y);
            SDL_RenderDrawPoint(renderer, xb, yb);
            int xc = compute_xc(n);
            int yc = compute_yc(n);
            SDL_RenderDrawPoint(renderer, xc, yc);
            SDL_SetRenderDrawColor(renderer, 255*frac, 255*(1-frac), 255, 0xF);

            if(ex1_show_lines)
                SDL_RenderDrawLine(renderer, xb, yb, xc, yc);
            n += 1;
        }

    for(double x = 1; x<=a; x++)
        for(double y = 1; y<=a; y++)
        {
            frac = (y+(x*a))/(a*a);
            if(frac > 1) frac = 1;
            if(frac < 0) frac = 0;
            SDL_SetRenderDrawColor(renderer, 255, 255 * frac, 255*(1.0-frac), 0xFF);
            int xa = compute_xa(x-0.5, y);
            int ya = compute_ya(x-0.5, y);
            SDL_RenderDrawPoint(renderer, xa, ya);
            int xc = compute_xc(n);
            int yc = compute_yc(n);
            SDL_RenderDrawPoint(renderer, xc, yc);
            SDL_SetRenderDrawColor(renderer, 255, 255 * frac, 255*(1.0-frac), 0xF);
            if(ex1_show_lines)
                SDL_RenderDrawLine(renderer, xa, ya, xc, yc);
            n+=1;
        }
    
}

void init_ex2()
{
    a = 16;
    b = 12;
    ex2_state = 0;
    
}

void ex2_logic()
{
    if(keyhit[SDL_SCANCODE_ESCAPE])
    {        
        printf("enter menu\n");
        gamestate = MENU;
    }
    if(keyhit[SDL_SCANCODE_LEFT])
    {
        b -= 1;
        if(b < 1) b = 1;
    }
    if(keyhit[SDL_SCANCODE_RIGHT])
    {
        b += 1;
    }
    if(keyhit[SDL_SCANCODE_UP])
    {
        a += 1;
    }
    if(keyhit[SDL_SCANCODE_DOWN])
    {
        a -= 1;
        if(a < 1) a = 1;
    }
    if(keyhit[SDL_SCANCODE_SPACE])
    {
        ex2_state ++;
        if(ex2_state>4) ex2_state = 0;
    }

    c = sqrt( (a*a) + (b*b) );
    p = (a*a)/c;
    q = (b*b)/c;
    h = sqrt(p*q);
}
void render_ex2()
{
    set_print_color(255, 255, 255);
    print("ESC - Menu  SPACE - next", 10, 10);
    print("up: a+1, down: a-1 left: b+1, right: b-1", 10, 30);
    sprintf(strbuf, "a: %f, b: %f, c:%f, q:%f, p:%f", a, b, c, q, p);
    print(strbuf, 10, 50);


    px_u = px_my-((h/2.0)*scale);
    px_d = px_my+((h/2.0)*scale);
    px_l = px_mx-(q*scale);
    px_r = px_mx+(p*scale);

    SDL_SetRenderTarget(renderer, screen);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0xFF);

    SDL_RenderDrawLine(renderer, px_l, px_d, px_mx, px_u);
    SDL_RenderDrawLine(renderer, px_mx, px_u, px_r, px_d);
    SDL_RenderDrawLine(renderer, px_r, px_d, px_l, px_d);
    print("a", px_mx+20, px_u-20);
    print("b", px_mx-20, px_u-20);
    print("c", px_mx+20, px_d-20);

    if(ex2_state > 0)
        SDL_RenderDrawLine(renderer, px_mx, px_u, px_mx, px_d);
    if(ex2_state > 1)
    {
        set_print_color(100, 100, 255);
        print("q", px_mx-20, px_d);
        print("q = b*b/c", 10, 70);
        set_print_color(255, 100, 100);
        print("p", px_mx+20, px_d);
        print("p = a*a/c", 140, 70);
    }
    if(ex2_state > 2)
    {
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 0xFF);
        SDL_RenderDrawLine(renderer, px_l, px_d, px_l, px_d+(c*scale));
        SDL_RenderDrawLine(renderer, px_l, px_d+(c*scale), px_mx, px_d+(c*scale));
        SDL_RenderDrawLine(renderer, -1+px_mx, px_d, -1+px_mx, px_d+(c*scale));
        set_print_color(100, 100, 255);
        print("c", px_l-20, px_d+40);
        print("b*b", px_mx-40, px_d+40);
        print("b*b = q*c", 10, 90);

        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 0xFF);
        SDL_RenderDrawLine(renderer, px_mx, px_d, px_mx, px_d+(c*scale));
        SDL_RenderDrawLine(renderer, px_mx, px_d+(c*scale), px_r, px_d+(c*scale));
        SDL_RenderDrawLine(renderer, px_r, px_d, px_r, px_d+(c*scale));
        set_print_color(255, 100, 100);
        print("c", px_r+20, px_d+40);
        print("a*a", px_mx+20, px_d+40);
        print("a*a = p*c", 140, 90);
    }
    if(ex2_state > 3)
    {
        SDL_SetRenderDrawColor(renderer, 255,255, 100, 0xFF);
        SDL_RenderDrawLine(renderer, px_l-1, px_d, px_l-1, px_d+(c*scale)+1);
        SDL_RenderDrawLine(renderer, px_l-1, px_d+(c*scale)+1, px_r+1, px_d+(c*scale)+1);
        SDL_RenderDrawLine(renderer, px_r+1, px_d-1, px_r+1, px_d+(c*scale)+1);
        SDL_RenderDrawLine(renderer, px_l-1, px_d-1, px_r+1, px_d-1);
        set_print_color(255, 255, 100);
        print("a*a + b*b = c*c", 10, 110);
    }
}

/* main */
int main()
{
    init();
    SDL_Event event;
    /* main loop */
    while(gamestate != QUIT)
    {
        /* event handling */
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT)
            break;
        key_logic();

        SDL_SetRenderTarget(renderer, screen);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(renderer);
        switch(gamestate)
        {
            case MENU:
                menu_logic();
                render_menu();
                break;
            case EX1:
                ex1_logic();
                render_ex1();
                break;
            case EX2:
                ex2_logic();
                render_ex2();
                break;
            case QUIT:
                break;
        }
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, screen, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    return 0;

}
