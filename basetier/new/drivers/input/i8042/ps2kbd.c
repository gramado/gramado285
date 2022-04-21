
// ps2kbd.c
// ps/2 keyboard support.
// ring0, kernel base.
// Created by Fred Nora.

#include <kernel.h>


void ps2kbd_initialize_device (void)
{
    debug_print ("ps2kbd_initialize_device:\n");
    PS2Keyboard.initialized = FALSE;


// globals
    keyboard_init_modifier_keys();
    keyboard_init_lock_keys();

// enable keyboard port
    wait_then_write(I8042_STATUS, 0xae);
    keyboard_expect_ack();


    PS2Keyboard.initialized = TRUE;
    debug_print ("ps2kbd_initialize_device: done\n");
}  



/*
 * DeviceInterface_PS2Keyboard: 
 *     Vamos pegar o raw code.
 *     Keyboard handler for abnt2 keyboard.
 *     fica dentro do driver de teclado.
 *     A interrupção de teclado vai chamar essa rotina.
 *     @todo: Usar keyboardABNT2Handler().
 * void keyboardABNT2Handler() 
 * Esse será o handler do driver de teclado
 * ele pega o scacode e passa para a entrada do line discipline dentro do kernel.
 * #todo: ISSO DEVERÁ IR PARA UM ARQUIVO MENOR ... OU AINDA PARA UM DRIVER.
 * Pega o scacode cru e envia para a disciplina de linhas que deve ficar no kernelbase.
 * Essa é a parte do driver de dispositivo de caractere.
 * #importante:
 * O driver deverá de alguma maneira notificar o kernel sobrea a ocorrência
 * do evento de input. Para que o kernel acorde as trheads que estão esperando 
 * por eventos desse tipo.
 */

// #importante:
// Provavelmente uma interrupção irá fazer esse trabalho de 
// enviar o scancode para o kernel para que ele coloque na fila.
// Nesse momento o kernel de sentir-se alertado sobre o evento de 
// input e acordar a threa que está esperando por esse tipo de evento. 
// #obs: 
// Esse buffer está em gws/user.h 
// Low level keyboard writter.
// Isso poderia usar uma rotina de tty
// O teclado esta lidando no momento com um buffer pequeno, 128 bytes.
// PUT SCANCODE

void DeviceInterface_PS2Keyboard(void)
{
    // Usado nos testes
    struct process_d *p;

    static int __has_e0_prefix = 0;
    static int __has_e1_prefix = 0;

// =============================================
// #test

#define I8042_STATUS 0x64
#define I8042_WHICH_BUFFER 0x20
#define I8042_BUFFER_FULL 0x01
#define I8042_MOUSE_BUFFER 0x20
#define I8042_KEYBOARD_BUFFER 0x00

// Get status.
    unsigned char status = in8(I8042_STATUS);

// buffer full?
    if (!(status & I8042_BUFFER_FULL))
        return;

// which device?
    int is_mouse_device = 
        ((status & I8042_WHICH_BUFFER) == I8042_MOUSE_BUFFER) 
        ? TRUE 
        : FALSE;

    // Yes it is a mouse.
    if ( is_mouse_device == TRUE )
        return;

// =============================================

// Não precisamos perguntar para o controlador se
// podemos ler, porque foi uma interrupção que nos trouxe aqui.
// #obs:
// O byte pode ser uma resposta à um comando ou 
// um scancode.

    unsigned char __raw = 0;
    unsigned char val   = 0;

sc_again:

//===========================================
// #test
// Testing with ack
// credits: minix
// #define KEYBD   0x60  /* I/O port for keyboard data */
// #define PORT_B  0x61  /* I/O port for 8255 port B (kbd, beeper...) */
// #define KBIT    0x80  /* bit used to ack characters to keyboard */

// Get the rawbyte for the key struck.
    __raw = in8(0x60);

//===========================================
// Get
// > Strobe the keyboard to ack the char
// > Send back
// Strobe the bit high
// and then strobe it low.

    val = in8(0x61); 
    out8(0x61, val | 0x80);  
    out8(0x61, val);
//===========================================

//++
// ===========================================
// #todo
// Temos que checar se o primeiro byte é um ack ou um resend.
// isso acontece logo apos a inicialização.
// #todo
// me parece que o primeiro byte pode ser um ack ou resend.
// #define ACKNOWLEDGE         0xFA
// #define RESEND              0xFE

// ??
// Assim como no mouse, talvez o ack so seja enviado
// quando estivermos em modo streaming.

    switch (__raw){

    case 0:
       goto done;
       break;

    // ACKNOWLEDGE
    case 0xFA:
        //#test
        printf ("DeviceInterface_PS2Keyboard: [test.first_byte] ack\n");
        refresh_screen();
        goto done;
        break;
    
    // RESEND
    case 0xFE:
        //#test
        printf ("DeviceInterface_PS2Keyboard: [test.first_byte] resend\n");
        refresh_screen();
        goto done;
        break;

    case 0xFF:
       goto done;
       break;

    ScancodeOrPrefix:
    default:
        goto CheckByte;
        break;
    };

// ===========================================
//--

// #bugbug
// [Enter] in the numerical keyboard isn't working.
// teclas do teclado extendido.
// Nesse caso pegaremos dois sc da fila.
// #obs:
// O scancode é enviado para a rotina,
// mas ela precisa conferir ke0 antes de construir a mensagem,
// para assim usar o array certo.
// See: ps2kbd.c
// #bugbug
// Esse tratamento do scancode não faz sentido quando temos um
// window server instalado. Nesse caso deveríamos deixar o
// window server pegar os scancodes.
// Mas por enquanto, essa rotina manda mensagens para o ws
// caso tenha um instalado.

CheckByte:

// Check prefix for extended keyboard sequence
    if ( __raw == 0xE0 ){ __has_e0_prefix = 1; goto done; }
    if ( __raw == 0xE1 ){ __has_e1_prefix = 1; goto done; }


// Process the normal byte
NormalByte:

// + Build the message and send it to the thread's queue.
// This routine will select the target thread.
// + Or send the message to the input TTY.
// This way the foreground process is able to get this data.
// See: ps2kbd.c
// See: user/console.c

// IN: 
// device type, data.
// 1 = keyboard

// Call the event handler.
// Console interrupt
// Valid foreground thread.
// Handler for keyboard input.
// See: kgwm.c

    if ( foreground_thread >= 0 && 
         foreground_thread < THREAD_COUNT_MAX )
    {
       xxxKeyEvent(
           (int) foreground_thread,
           (unsigned char) __raw );
    }

// Clean the mess.
    __has_e0_prefix = 0;
    __has_e1_prefix = 0;

done:
    return;
}


// #todo: Change this name.
void keyboard_init_modifier_keys (void)
{
// Modifier keys.
    shift_status  = 0;
    ctrl_status   = 0;
    winkey_status = 0;
    alt_status    = 0;

// Alternate Graphic.
    //altgr_status = 0; //@todo
    
	// Function.
	//fn_status = 0;  //@todo
	
	//...
}


// #todo: Change this name.
void keyboard_init_lock_keys (void)
{
    // Capital Lock.	
	capslock_status = 0;
	
	// Scrolling Lock.
	scrolllock_status = 0;
	
	// Number Lock.
	numlock_status = 0;	
}


// keyboardDisable:
// Disable keyboard.
// Wait for bit 1 of status reg to be zero.
// Send code for setting disable command.
    
void keyboardDisable (void)
{
    while ( ( in8 (0x64) & 2) != 0 )
    { 
         // Nothing.
    };

    out8 (0x60,0xF5);
    //sleep(100);
}

// keyboardEnable:
//     Enable keyboard.

void keyboardEnable (void)
{
    // #bugbug
    // Dizem que isso pode travar o sistema.

	// Wait for bit 1 of status reg to be zero.
    while ( ( in8 (0x64) & 2) != 0 )
    {
		//Nothing.
    };

	//Send code for setting Enable command.
    out8 (0x60,0xF4);
    //sleep(100);
}

/*
 * keyboard_set_leds:
 *     Set keyboard flags.
 *     ED = Set led.
 */

void keyboard_set_leds (char flag)
{
    //#todo: Filtro.

    // Wait for bit 1 of status reg to be zero.
    while ( ( in8 (0x64) & 2) != 0 )
    {
        // Nothing.
    };
    // Send code for setting the flag.
    out8 (0x60,0xED); 
    sleep (100);


    // Wait for bit 1 of status reg to be zero.
    while ( ( in8 (0x64) & 2) != 0 )
    {
        // Nothing.
    };
    // Send flag. 
    out8 (0x60,flag);
    sleep (100);

    // #todo: Mudar o status.
    // switch(flag){}
}


/*
 * keyboardGetKeyState: 
 * 
 */

// Pega o status das teclas de modificação.

unsigned long keyboardGetKeyState ( unsigned char key )
{
	unsigned long State=0;
	
	switch (key){
	case VK_LSHIFT:    State = shift_status;       break;
    case VK_LCONTROL:  State = ctrl_status;        break;
    case VK_LWIN:      State = winkey_status;      break;
    case VK_LMENU:     State = alt_status;         break;
    case VK_RWIN:      State = winkey_status;      break;
    case VK_RCONTROL:  State = ctrl_status;        break;
    case VK_RSHIFT:    State = shift_status;       break;
    case VK_CAPITAL:   State = capslock_status;    break;
    case VK_NUMLOCK:   State = numlock_status;     break;
    case VK_SCROLL:    State = scrolllock_status;  break;
    // ...
	};

    return (unsigned long) State;
}


// Get alt Status.
int get_alt_status (void)
{
    return (int) alt_status;
}

// Get control status.
int get_ctrl_status (void)
{
    return (int) ctrl_status;
}

// Get shift status.
int get_shift_status (void)
{
    return (int) shift_status;
}


/*
 * xxx_keyboard_read: 
 * 
 */

// Esta função será usada para ler dados do teclado na 
// porta 0x60, fora do IRQ1.

uint8_t xxx_keyboard_read (void)
{
    kbdc_wait (0);

    uint8_t val = in8(0x60);
    
    wait_ns(400);
    
    return (uint8_t) val;
}


/*
 * xxx_keyboard_write: 
 * 
 */

// Esta função será usada para escrever dados do teclado 
// na porta 0x60, fora do IRQ1.

void xxx_keyboard_write (uint8_t data)
{
    kbdc_wait(1);

    out8 ( 0x60, data );

    wait_ns(400);
}


/*
 * zzz_keyboard_read:
 *     Pega um byte na porta 0x60.
 */

unsigned char zzz_keyboard_read (void)
{
    prepare_for_input();
    
    return (unsigned char) in8(0x60);
}


void keyboard_expect_ack (void)
{
    // #bugbug
    // ? loop infinito  
    // while ( xxx_mouse_read() != 0xFA );

    unsigned char ack_value=0;
    int timeout=100;

    while(1) 
    {
        timeout--;
        if (timeout<0){  break;  }

        //ack_value = (unsigned char) zzz_keyboard_read();
        
        prepare_for_input();
        ack_value = (unsigned char) in8(0x60);
        
        // OK
        if (ack_value == 0xFA){
            return;  
        }
    }; 

// Acabou o tempo, vamos checar o valor.
// Provavelmente esta errado.
    if ( ack_value != 0xFA )
    {
        //#debug
        //printf ("expect_ack: not ack\n");
        return;
        //return -1;
    }

    return;
    //return 0;
}


