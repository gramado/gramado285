/*
 * File: stdlib.c
 * stdlib
 * 2015
 */


#include <kernel.h>


static int randseed = 1234;


//Alimenta a função rand.
//void srand(unsigned int seed)
//{};


/* rand: 
 * Gera um número inteiro semi-randômico. 
 */

int rand (void)
{
    return (int) ( randseed = randseed * 1234 + 5 );
}


// ok.
int abs(int j)
{
    return (j < 0 ? -j : j);
}


// Supporting the services 808 e 809.
// See: sci.c
// #todo: change the name to 'ubuf'.
int __ptsname (int fd, char *buf, size_t buflen)
{
     char *ptsname_buffer = (char *) buf;

     char test_str[50] = "new-__ptsname: test string";

// #todo: fd

    if ( (void*) buf == NULL )
    {
        return (int) (-EINVAL);
    }

    // 64 bytes limit
    strcpy ( ptsname_buffer, (const char *) test_str );
    
    // Lá na lib em ring3 a rotina retorna para o app o 
    // ponteiro para o buffer
    return 0;  //ok
}


/*
 // See: https://en.wikipedia.org/wiki/Slab_allocation
void *slab_alloc (size_t size);
void *slab_alloc (size_t size)
{}
*/


/* 
 *****************************************
 * kmalloc:
 *     Standard kmalloc function. 
 */

void *kmalloc (size_t size){

    void *h;
    unsigned long s = ( unsigned long) size;

	//s = (s ? s : 1);	/* if s == 0, s = 1 */

    if ( s < 0 ){
        debug_print ("kmalloc: s\n");
        return NULL;
    }

    if ( s == 0 ){
        debug_print ("kmalloc: s ajust\n");
        s=1;
    }

// Alocar memória no heap do kernel.
// ps/x86/memory.c

    h = (void *) heapAllocateMemory(s);

    if ( (void *) h == NULL ){
        debug_print ("kmalloc: h\n");
        return NULL;
    }

    // Ok.
    return (void *) h; 
}


/*
 ************************************
 * kfree:
 *
 * >> #importante:
 * >> Tradicionalmente essa função só libera o que foi alocado 
 * pela última chamada de malloc. Mas estamos tentando algo diferente.
 *
 * The free() function frees the memory space pointed to by ptr, 
 * which must have been returned by a previous call 
 * to malloc(), calloc() or realloc(). 
 * Otherwise, or if free(ptr) has already been called before, 
 * undefined behavior occurs. 
 * >> If ptr is NULL, no operation is performed.
 *
 * Importante:
 *     uma estratégia seria apenas sinalizarmos na estrutura que 
 * desejamos que o GC libere os recurso. Em seguida devemos sinalizar 
 * no mmblock que libere o bloco para outras alocações. 
 *
 */

void kfree (void *ptr){

    if ( (void *) ptr == NULL ){
        debug_print ("kfree: ptr\n");
        return;
    }
    
    // ps/x86/memory.c
    FreeHeap (ptr);
}    


/*
 * kcalloc: alloca e preenche com zero.
 * @todo:
 */
/*
void *kcalloc (size_t count, size_t size)
{
    size_t s = count * size;
    
	void *value = malloc (s);
    
	if (value != 0)
    {    
	    memset (value, 0, s);
	};
  return value;
};
*/



