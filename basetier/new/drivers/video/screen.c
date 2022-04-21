
// screen.c


#include <kernel.h>


extern unsigned long SavedX;
extern unsigned long SavedY;
extern unsigned long SavedBPP;


unsigned long screen_width; 
unsigned long screen_height;
unsigned long screen_bpp;

unsigned long screen_pitch;


// Qual é o endereço virtual do início de uma dada linha da tela?
unsigned long screen_scanline_va( int scanline )
{
    unsigned long scanline_va=0;
    unsigned long start = FRONTBUFFER_VA;

// pitch
    unsigned long pitch = (unsigned long) screenGetHeight();
    if ( pitch == 0 ){
        panic ("screen_scanline_va: pitch limits");
    }

// height
    unsigned long height = (unsigned long) screenGetHeight();
    if ( height == 0 ){
        panic ("screen_scanline_va: height limits");
    }

    if (scanline >= height){
        panic ("screen_scanline_va: scanline limits");
    }

    scanline_va = (unsigned long) ( start + (pitch * scanline) );

// done:
// É o endereço virtual do início de uma dada linha da tela.
    return (unsigned long) scanline_va;
}


// Get screen width
unsigned long screenGetWidth (void)
{
    return (unsigned long) screen_width;
}


// Get screen height
unsigned long screenGetHeight (void)
{
    return (unsigned long) screen_height;
}


// Get screen bpp
unsigned long screenGetBPP (void)
{
    return (unsigned long) screen_bpp;
}


// Get screen pitch
unsigned long screenGetPitch (void)
{
    return (unsigned long) screen_pitch;
}


// screenSetSize:
//     Configura as dimensões da tela.
//     Tamanho do monitor.

void screenSetSize( unsigned long width, unsigned long height )
{
    screen_width  = (unsigned long) width;
    screen_height = (unsigned long) height;
}


/*
 *********************************************************
 * fb_refresh_screen:
 *     Coloca o conteúdo do BackBuffer no LFB da memória de vídeo.
 *     Ok, isso está no lugar certo. Pois somente
 *  O driver de vídeo pode acessar o LFB
 *  /x/video/screen.c 
 */

// backup.
// This is working very well.
// Copying long by long.
// ?? 
// Total size is wrong.
// Esse é mais rápido.
// #bugbug
// #danger
// Dependendo do tamanho da tela, 
// essa rotina pode acabar copiando conteúdo 
// para alguma parte não mapeada no lfb.
// Precisamos de limites. Obedecendo a quantidade mapeada.
// #bugbug
// Como temos apenas 2MB de lfb mapeados, 
// então vamos copiar menos dados pra evitar ultrapassar o limite
// e causar PF.

// #todo
// We can also use 16 bytes ... SSE?

void fb_refresh_screen (unsigned long flags)
{
    int i=0;

// Char. The slow way.
    unsigned char *backbuffer  = (unsigned char *) BACKBUFFER_VA;
    unsigned char *frontbuffer = (unsigned char *) FRONTBUFFER_VA;

// Long. The fast way.
    unsigned long *backbuffer_long  = (unsigned long *) BACKBUFFER_VA;
    unsigned long *frontbuffer_long = (unsigned long *) FRONTBUFFER_VA;


// We can't refresh.
// The buffer wasn't validated.
    if ( refresh_screen_enabled != TRUE ){
        debug_print_string("fb_refresh_screen: [FAIL-FIXME] refresh_screen_flag\n");
        return;
    }

    if ( SavedX == 0 || SavedY == 0 || SavedBPP == 0 )
    {
        debug_print_string("fb_refresh_screen: [FAIL] validation\n");
        return;
    }

// #todo
// Isso significa que só poderemos 
// usar o modo 320x200 por enquanto.
// clipping:
// Podemos recortar, e mesmo que a resoluçao seja alta,
// somente escreveremos nos primeiros 2mb ...
// Isso seria divertido, a velocidade seria alta,
// mas teriamos uma tela recortada. kkk
// Essa tecnica pode ser uma opçao configuravel

    //int Total = (int)(SavedX*SavedBPP*SavedY);
    int Total = (screen_size_in_kb * 1024);

/*
// nao pode ser mais que 2mb
    //if ( Total >= (2*1024*1024) )
    if (Total >= 2097152 ){
        debug_print_string("fb_refresh_screen: [FAIL-FIXME] Total\n");
        return;
    }
*/
// Se algo deu errado 
// e realmente precisamos de alguma tela
// para debug na inicializaçao.
// entao vamos limitar o tamanho do refresh screen
// isso ajuda no refresh screen, mas nao em outras rotinas graficas.
    if( g_use_fake_screen_size == TRUE)
    {
        //fake_screen_size_in_kb = (( 320*4*200 )/1024);
        Total = (fake_screen_size_in_kb * 1024);
    }

// nao pode ser mais que 2mb
    if (Total >= 2097152 ){
        debug_print_string("fb_refresh_screen: [FAIL-FIXME] Total\n");
        return;
    }

//
// Refresh
//

// Vertical synchonization?
//  if(flags & 1)
    vsync();

// Fast way ?
// Divisible by 8. So use the fast way.

    int FastTotal=0;
    if ( (Total % 8) == 0 )
    {
        FastTotal = (Total >> 3);
        for ( i=0; i<FastTotal; i++ ){
            frontbuffer_long[i] = backbuffer_long[i];
        };
        return;
    }

// Slow way.
// One byte per time.

    for ( i=0; i<Total; i++ ){
        frontbuffer[i] = backbuffer[i];
    };

    return;
}   


void refresh_screen (void)
{
    fb_refresh_screen(0);
}


// The whole screen is dirty.
// It can be flushed into the framebuffer.

void invalidate_screen(void)
{
    screen_is_dirty = TRUE;
}


/*
 **********************************************
 * screenInit:
 *     Inicializando o gerenciamento de tela.
 */ 

// #bugbug
// Screen is a reagion in the display, or in many displays.
// Display is a monitor, or a set o hid in a given host.

int screenInit (void){

    debug_print ("screenInit:\n");


// Validate the whole screen
// We don't need to flush the screen yet.

    screen_is_dirty = FALSE;


// Configura globais com base nos valores passados pelo Boot Loader.

    screenSetSize ( (unsigned long) SavedX, (unsigned long) SavedY );
    
    screen_bpp = SavedBPP;
    screen_pitch = ( SavedBPP * SavedX );

// Setup Screen structure.
// See: screen.h
    
    Screen = (void *) kmalloc ( sizeof(struct screen_d) );
    
    if ( (void *) Screen == NULL ){
        panic ("screenInit:\n");
    }else{
        Screen->used  = TRUE;
        Screen->magic = 1234;
        Screen->id = 0;
        Screen->left = SCREEN_DEFAULT_LEFT;
        Screen->top  = SCREEN_DEFAULT_TOP; 
        Screen->width  = (unsigned long) screenGetWidth();
        Screen->height = (unsigned long) screenGetHeight();
        Screen->next   = NULL;
        // ...
 
        // salvando o ponteiro da estrutura. 
        ScreenInfo    = (void *) Screen;
        CurrentScreen = (void *) Screen;
        //...
    };

    // ?

    return 0;
}


