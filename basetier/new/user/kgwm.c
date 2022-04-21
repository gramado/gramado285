
// kgws.c
// Events and wm inside the kernel.

#include <kernel.h> 


// 
// Imports
//

// from swlib.asm
// Used to handle callbacks used by the window server.
extern unsigned long wmData_RDI;
extern unsigned long wmData_RSI;
extern unsigned long wmData_RDX;
extern unsigned long wmData_RCX;
extern unsigned long wmWindowMananer_SendMessage(void);


// from swlib.asm
// Used to handle callbacks used by the window server.
extern unsigned long wmData_Callback0;
extern unsigned long wmData_Callback1;
extern unsigned long wmData_Callback2;


//keyboard support
//#define KEYBOARD_KEY_PRESSED  0x80
#define KEYBOARD_KEY_MASK  0x7F
//#define KEYBOARD_FULL      1
//#define KEYBOARD_EMPTY     0
//#define KEYBOARD_OBF       0x01    //Output buffer flag.


// ============================
static unsigned long presence_level=32;
static unsigned long flush_fps=30;

// ============================

// Char map size.
#define ABNT2_CHARMAP_SIZE  136 


//
// Endereços dos charmaps usados:
// Obs: Incluídos na estrutura.
//
//unsigned long normal_charmap_address;
//unsigned long shift_charmap_address;
//unsigned long control_charmap_address;
//



/*
 * NORMAL ABNT2.
 *     ?? const char* ??
 * obs: Isso pode ser carregável para várias configurações de teclado.
 */

// minusculas

static unsigned char map_abnt2[ABNT2_CHARMAP_SIZE] = {
 0,      //Scancode 0.
 033,    //1  (Esc). (\e)
 '1',    //2
 '2',    //3
 '3',
 '4',
 '5',
 '6',
 '7',
 '8',      //9
 '9',      //10
 '0',      //11
 '-',      //12
 '=',      //13
 VK_BACK,  //14 BackSpace. (0xF7) delete
 VK_TAB,   //15 TAB.        (\t)
 'q',      //16 
 'w',      //17 (alt)
 'e',
 'r',
 't',
 'y',
 'u',
 'i',
 'o',
 'p',          //25
 ' ',          //26 /// VK_ACENTO_AGUDO acento agudo (00b4 @) dead_acute dead_grave
 '[',          //27   bracketleft   braceleft
 VK_RETURN,    //28 [Enter].
 VK_LCONTROL,  //29 ms right control. ??
 'a',          //30 (O a minúsculo é 30 em abnt 2).
 's',          //31.
 'd',          //32.
 'f',
 'g',
 'h',
 'j',
 'k',
 'l',        //38 
 135,        //39 00e7 VK_CCEDILHA ccedilla
 '~',        //40 dead_tilde	dead_circumflex
 '\'',       //41 (") (espaço) (') dead_acute dead_diaeresis 
 VK_LSHIFT,  //42 Shift.  
 ']',        //43 bracketright braceright asciitilde Control_bracketright 
 'z',        //44 
 'x',
 'c',
 'v',
 'b',
 'n',
 'm',          //50 
 ',',          //51 comma  less
 '.',          //52 period greater 
 ';',          //53  numlockdesligado=(;) numlockligado=(/)abnt2.
 VK_RSHIFT,    //54 Right Shift.
 '*',          //55 abnt2(NUM *) (NUM lock??).(printscreen(abnt2))
 VK_ALTGR,     //56 abnt2=(Alt). VK_ALTGR (@todo)
 ' ',          //57 Space. VK_SPACE
 VK_CAPITAL,   //58 Caps Lock.
 VK_F1,        //59 F1. abnt2=(F1)
 VK_F2,        //60 F2.(abnt)
 VK_F3,        //61 F3.(abnt)
 VK_F4,        //62 F4.(abnt)
 VK_F5,        //63	F5  .(abnt)
 VK_F6,        //64 F6 .(abnt)
 VK_F7,        //65 F7.(abnt)
 VK_F8,        //66 F8.(abnt)
 VK_F9,        //67 F9.(abnt)
 VK_F10,       //68 F10.(abnt)
 VK_PAUSE,      //69 abnt2=(PAUSE). (num lock) abnt2=VK_NUMLOCK,
 VK_SCROLL,     //70 abnt2(Scroll Lock) abnt2=break.
 VK_HOME,      //'7',    //71 abnt2=(7). abnt2=home
 VK_ARROW_UP,  //'8',    //72       >>> (8). abnt2=up
 VK_PAGEUP,  //'9',    //73       (9). abnt2=pgup
 '-',    //74        -
 VK_ARROW_LEFT,  //'4',    //75       >>> (4). abnt2=left 
 '5',    //76       (5).
 VK_ARROW_RIGHT,  //'6',    //77       >>> (6). abnt2=right
 '+',    //78       +
 VK_END,  //'1',    //79       (1). abnt2=end  
 VK_ARROW_DOWN,  //'2',    //80       >>> (2). abnt2=down
 VK_PAGEDOWN,  //'3',    //81       (3). abnt2=pgdn (q).     
 VK_ABNT2_INSERT,  //'0',    //82       (0). abnt2=Ins
 VK_ABNT2_DELETE,  //VK_DELETE_TECLADONUMERICO,    //83 abnt2=(,) abnt2=(Num DEL) (abnt).(´)
VK_PRINT,   //84 abnt2=(SysRq) PRINT SCREEN E sys req (abnt).
'.',        //85
'\\',       //86 abnt2=(\). abnt2=(help) backslash  bar
VK_F11,     //87 abnt2=(F11).	 
VK_F12,     //88 abnt2=(F12).
'/',        //89 slash question /?
VK_F14,     //90
VK_LWIN,               //91 abnt2=(Left Windows)  (right WinKey) ([).
VK_RWIN,               //92 abnt2=(Right Windows) (left WinKey) (\).
VK_CONTROL_MENU,       //93 abnt2=(Application). CONTROL MENU, (]).
VK_F18,    //94
VK_F19,    //95
'`',       //96 (0). (`)Acento grave.(enter)
VK_LCONTROL,  //97 (1) (a) Minúscula. control
'/',          //98 (2) (/) KP_Divide
VK_F23,       //99 (3) VoidSymbol  
VK_ALTGR,     //100 (4) AltGr
VK_BREAK,     //101 (5). 0xE9
VK_FIND,      //102 (6) (\).
VK_UP,        //103 (7).
VK_PRIOR,     //104 (8).
VK_LEFT,      //105 (9). 0xED
VK_RIGHT,     //106 *
VK_SELECT,    //107 +
VK_DOWN,      //108 RETURN.
VK_NEXT,      //109 -
VK_INSERT,    // 
VK_REMOVE,    //111 (/). 0xF3
VK_F1,   //112 (F1). MACRO
VK_F2,   //113 (F2) F13
VK_F3,   //114 (F3) F14
'/',     //115   abnt2=(/)(F4). HELP
VK_F5,   //116 (F5) DO
VK_F6,   //117 (F6) F17
VK_F7,   //118 (F7) (esc) KP_MinPlus
VK_PAUSE,   //119 (F8) Pause 
VK_F9,   //120 (F9)
VK_F10,  //121 (F10)
VK_F11,  //122 (F11)
VK_F12,  //123 (F12)
VK_F13,  //124 abnt2=F13 
VK_F14,  //125 abnt2=F14
'.',     //126 abnt=(.) abnt=(F15) VK_ACENTO_TIL   
VK_F16,	 //127          abnt=(F16) 
VK_F17,	 //128
VK_F18,	 //129
VK_F19,	 //130
VK_F20,	 //131
VK_F21,	 //132
VK_F22,	 //133
VK_F23,	 //134
VK_F24,	 //135
};










/*
 * SHIFT ABNT2.
 */

// CAPS LOCK LIGADO

// maiúsculas

static unsigned char shift_abnt2[ABNT2_CHARMAP_SIZE] = {
0, 
033,   // "
'!',   //2 
'@',   //3
'#',   //4
'$',   //5
'%',   //6
VK_ABNT2_TREMA, //168,   //7 (00a8 @) (trema)
'&',   //8
'*',   //9
'(',   //10
')',   //11
'_',   //12
'+',   //13 
VK_BACK,  // 0x0E     0177,
VK_TAB,   // 0x0F     '\t',   //15 TAB
'Q',    //16
'W',    //17
'E',    //18
'R',    //19
'T',    //20
'Y',    //21
'U',    //22
'I',    //23
'O',    //24
'P',    //25    (FAIL!)
'`',    //26
'{',    //27
VK_RETURN,   // 28
CTL,         // 29 
'A',  //30
'S',
'D',
'F',
'G',
'H',  //FAIL
'J',
'K',  //FAIL
'L',  //38
128,  //39 (00c7) Ç
'^',  //40             //OK
'"',  //41 abnt2=(")
SHF,
 '}',  //43            //OK
'Z',   //44 
'X',
'C',
'V',
'B',
'N',
'M',  //50    //FAIL
'<',  //51 abnt2
'>',  //52 abnt2
':',  //53 abnt2
SHF,  //54 
CTL,  //55
ALT,  //56
' ',  //57
CPS|L,  //58
0,      //59
0,      //60
' ',    //61
0,   //62
0,   //63
0,   //64
0,   //65
0,   //66
0,   //67
0,   //68
NUM|L,  //69
STP|L,  //70
VK_HOME,  //'7',    //71
VK_ARROW_UP,  //'8',    //72
VK_PAGEUP,  //'9',    //73
'-',    //74
VK_ARROW_LEFT,  //'4',    //75
'5',    //76
VK_ARROW_RIGHT,  //'6',    //77
'+',    //78
VK_END,  //'1',    //79
VK_ARROW_DOWN,  //'2',    //80
VK_PAGEDOWN,  //'3',    //81
VK_ABNT2_INSERT,  //'0',    //82
VK_ABNT2_DELETE,  //',',    //83 abnt2(,)
0,      //84
0,      //85
'|',    //86 abnt2(|)
0,  //87
0,  //88
0,  //89
0,  //90
0,  //91
0,  //92
0,  //93
0,  //94
0,  //95
0,  //96
0,  //97
0,  //98
0,  //99
0,  //100
0,  //101
0,  //102
0,  //103
0,  //104
0,  //105
0,  //106
0,  //107
0,  //108
0,  //109
0,  //110
0,  //111
0,  //112
0,  //113
0,  //114
'?',  //115 abnt2(?)
0,  //116
0,  //117
0,  //118
0,  //119
0,  //120
0,  //121
0,  //122
0,  //123
0,  //124
0,  //125
'.',  //126 abnt2(.).
0,
};




/*
 * CONTROL ABNT2.
 */

// com control
// control + key

static unsigned char ctl_abnt2[ABNT2_CHARMAP_SIZE] = {
0,     //0
033,   //1
'!',   //2
000,   //3
'#',   //4
'$',   //5
'%',   //6
036,   //7
'&',   //8
'*',   //9
'(',   //10
')',   //11
ASCII_US,     //12   ^_
'+',          //13 
034,          //14
'\177',       //15
ASCII_DC1,    //16  ^Q
ASCII_ETB,    //17  ^W
ASCII_ENQ,    //18  ^E  
ASCII_SO,     //19  ^R
ASCII_DC4,    //20  ^T
ASCII_EM,     //21  ^Y
ASCII_NAK,    //22  ^U
ASCII_HT,     //23  ^I
ASCII_SI,     //24  ^O
ASCII_DLE,    //25  ^P
033,          //26
ASCII_ESC,    //27     ^[
'\r',         //28
CTL,          //29
ASCII_SOH,    //30 ^A
ASCII_DC3,    //31 ^S
ASCII_EOT,    //32 ^D
ASCII_ACK,    //33 ^F
ASCII_BEL,    //34 ^G
ASCII_BS,     //35 ^H
ASCII_LF,     //36 ^J
ASCII_VT,     //37 ^K
ASCII_FF,     //38 ^L
';',          //39
ASCII_RS,     //40  ^^
'`',          //41
SHF,          //42
ASCII_GS,     //43 ^]
ASCII_SUB,    //44 ^Z
ASCII_CAN,    //45 ^X
ASCII_ETX,    //46 ^C
ASCII_SYN,    //47 ^V
ASCII_STX,    //48 ^B
ASCII_SO,     //49 ^N
ASCII_CR,     //50 ^M
'<',    //51 
'>',    //52
'?',    //53
SHF,    //54
'*',    //55
ALT,    //56
' ',    //57
CPS|L,  //58
0,      //59
0,      //60
' ',    //61
0,      //62
0,      //63
CPS|L,  //64
0,      //65 
0,   //66
0,   //67
0,   //68
0,   //69
0,   //70
0,   //71
0,   //72
0,   //73
0,   //74
0,   //75
0,   //76
0,   //77
0,   //78
0,   //79
0,   //80
0,   //81
0,   //82
0,   //83
0,   //84
0,   //85
ASCII_FS,   //86 
0,    //87
0,    //88
0,    //89
033,  //90
'7',
'4',
'1',
0, 
NUM|L,
'8',
'5',
'2',
0,
STP|L,
'9',
'6',
'3',
'.',
0,
'*',
'-',
'+',
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
};













//
// Prototypes
//

static void __launch_app_via_initprocess(int index);
static void __enter_embedded_shell(int kernel_in_debug_mode);
static void __exit_embedded_shell(void);


// ============================


// Register callbacks sent by gwssrv.bin
void 
wmRegisterWSCallbacks(
    unsigned long callback0,
    unsigned long callback1,
    unsigned long callback2 )
{

    printf("wmRegisterWSCallbacks:\n");


    gUseWMCallbacks = TRUE;

    // Save callbacks
    // See: swlib.asm
    wmData_Callback0 = (unsigned long) callback0;
    wmData_Callback1 = (unsigned long) callback1;
    wmData_Callback2 = (unsigned long) callback2;


    if( wmData_Callback0 == 0 ||
        wmData_Callback1 == 0 ||
        wmData_Callback2 == 0  )
    {
        panic("wmRegisterWSCallbacks: Invalid callbacks\n");
    }

//#debug

    //printf("wmData_Callback0 = %x \n", wmData_Callback0);
    //printf("wmData_Callback0 = %x \n", wmData_Callback1);
    //printf("wmData_Callback0 = %x \n", wmData_Callback2);

    // No while.
    // scroll will not work
    //while(1){
        //asm("call *%0" : : "r"(wsCallbacks[0]));
    //}
    

//done:
    //printf("done\n");

// #importante
// Nesse momento podemos criar
// interrupçoes para esses endereços.

    refresh_screen();
    //while(1){}
}


// Send input to the window manager
// inside the window server ( gwssrv.bin )
unsigned long wmSendInputToWindowManager(
    unsigned long wid,
    unsigned long msg,
    unsigned long long1,
    unsigned long long2)
{

// We can't!
    if( gUseWMCallbacks != TRUE )
        return 0;


    //asm("call *%0" : : "r"(wsCallbacks[0]));

// Setup parameters
// See: swlib.asm

    wmData_RDI = (unsigned long) (wid & 0xFFFF); 
    wmData_RSI = (unsigned long) (msg & 0xFFFF);
    wmData_RDX = (unsigned long) long1;
    wmData_RCX = (unsigned long) long2;


// Trampoline
// See: swlib.asm

    return (unsigned long) wmWindowMananer_SendMessage();
}


// local
// Launch an app via init process.
// Just a small range of messages are accepted.
// See: gramado/core/client
// range: 4001~4009
static void __launch_app_via_initprocess(int index)
{

    if( index < 4001)
        return;

    if( index > 4009)
        return;

    if( (void*) InitThread == NULL ){
        return;
    }

    post_message_to_tid(
        (int) InitThread->tid,
        NULL,  //window
        (int) MSG_COMMAND,  //msg code
        index,   // range: 4001~4009
        0 );
}

static void __enter_embedded_shell(int kernel_in_debug_mode)
{

//log
    //backgroundDraw(COLOR_BLACK);
    backgroundDraw(COLOR_EMBEDDED_SHELL_BG);

    if(kernel_in_debug_mode)
        printf("[[ KERNEL IN DEBUG MODE ]]\n");

    printf("kernel console number %d\n",fg_console);
    printf("Prompt ON: Type something\n");
    consolePrompt();
    refresh_screen();

// done
    ShellFlag = TRUE;
}


static void __exit_embedded_shell(void)
{

// log
    printf("\n");
    printf ("Prompt OFF: Bye\n");
    printf("\n");
    refresh_screen();

// update desktop
    if ( gUseWMCallbacks == TRUE ){
         wmSendInputToWindowManager(0,9092,0,0);
    }

// done
    ShellFlag = FALSE;
}


void exit_kernel_console(void)
{
    
    // Se estavamos no ambiente de desktop completo
    // e nao no modo debug.
    // se temos callbacks e' porque o window server inicializou isso.
    if ( gUseWMCallbacks == TRUE ){
        __exit_embedded_shell();
    }
    
    // suficiente para o modo debug.
    ShellFlag = FALSE;
}


// kernel initialized on debug mode.
// only on initialzation.
// called by init.c
void kgwm_early_kernel_console(void)
{

// fail
// se ja estamos no ambiente de desktop
// com toda a gui funcionando.
    if ( gUseWMCallbacks == TRUE )
    {
        return;
    }

    //IN: kernel in in debug mode.
    __enter_embedded_shell(TRUE);

// Enable input
    asm("sti");

    while(1){
        // exit loop
        if( ShellFlag == FALSE )
        {
            // mandamos uma mensagem para o ws, para atualizar o desktop.
            if(gUseWMCallbacks==TRUE){
                __exit_embedded_shell();
            }
            break;
        }
    };

// Disable input end return to init.c
    asm("cli");
    return;
}


/*
 * wmProcedure:
 *       Some combinations with control + F1~F12
 */
// Local function.
// Called by kgws_event_dialog in kgws.c
// Called by si_send_longmessage_to_ws and si_send_message_to_ws
// in siws.c 
// #bugbug: We don't wanna call the window server. Not now.
// #important:
// Isso garante que o usuário sempre podera alterar o foco
// entre as janelas do kgws usando o teclado, pois essa rotina
// é independente da thread que está em foreground.
// #todo
// Talvez a gente possa usar algo semelhando quando o window
// server estiver ativo. Mas possivelmente precisaremos 
// usar outra rotina e não essa. Pois lidaremos com uma estrutura
// de janela diferente, que esta localizada em ring3.
// From Windows:
// Because the mouse, like the keyboard, 
// must be shared among all the different threads, the OS 
// must not allow a single thread to monopolize the mouse cursor 
// by altering its shape or confining it to a small area of the screen.
// #todo
// This functions is the moment to check the current input model,
// and take a decision. It will help us to compose the event message.
// It is because each environment uses its own event format.

unsigned long 
wmProcedure ( 
    struct window_d *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
    int Status = -1;
    int UseSTDIN=TRUE;      // Input model
    char ch_buffer[2];
    char buffer[128];

    unsigned long tmp_value=0;

    //debug_print("wmProcedure:\n");
    
    sprintf (buffer,"My \x1b[8C string!\n"); 


// ===================================
// Control:
//     'Control + ?' belongs to the kernel.

// ===================================
// Shift:
//     'Shift + ?' belongs to the window server.


//
// Dispatcher
//

    if (msg<0){
        debug_print("wmProcedure: Invalid msg\n");
        return 0;
    }

    switch (msg){

// ==============
// msg:
// Mouse stuff.
    case MSG_MOUSEMOVE:
    case MSG_MOUSEPRESSED:
    case MSG_MOUSERELEASED:
        wmSendInputToWindowManager(0,msg,long1,long2);
        return 0;
        break;

// ==============
// msg:
// Keydown.
    case MSG_KEYDOWN:

        // Para todas as teclas quando o console não está ativo.
        // Serão exibidas pelo window server 
        // na janela com foco de entrada,
        // Se ela for do tipo editbox.
        // O ws mandará mensagens para a thread associa
        // à janela com foco de entrada.
            
        if ( ShellFlag!=TRUE ){
            wmSendInputToWindowManager(0,MSG_KEYDOWN,long1,long2);
        }

        switch (long1){

        case VK_RETURN:
            //if(ShellFlag!=TRUE){
                //wmSendInputToWindowManager(0,MSG_KEYDOWN,long1,long2);
                //return 0;
            //}
            if(ShellFlag==TRUE)
            {
                kinput('\0');               // finalize
                consoleCompareStrings();   // compare
                //invalidate_screen();
                refresh_screen();
                return 0;
            }
            break;

         //case 'd':
         //case 'D':
         //    if(ctrl_status==TRUE && alt_status==TRUE)
         //    {
         //        __enter_embedded_shell(FALSE);
         //        return 0;
         //    }
         //    break;

        //case VK_TAB: 
            //printf("TAB\n"); 
            //invalidate_screen();
            //refresh_screen(); 
            //break;

        default:

            // Console!
            // YES, we're using the embedded kernel console.
            // O teclado vai colocar o char no prompt[]
            // e exibir o char na tela somente se o prompt
            // estiver acionado.
            if (ShellFlag==TRUE)
            {
                consoleInputChar(long1);
                console_putchar ( (int) long1, fg_console );
                return 0;
            }


            // Not console.
            // NO, we're not using the kernel console.
            // Pois não queremos que algum aplicativo imprima na tela
            // enquanto o console virtual está imprimindo.
            if ( ShellFlag!=TRUE )
            {
                // Send it to the window server.
                //wmSendInputToWindowManager(0,MSG_KEYDOWN,long1,long2);
                // #test
                // Write into stdin
                if ( UseSTDIN == TRUE )
                {
                    //debug_print ("wmProcedure: Writing into stdin ...\n");
                    //stdin->sync.can_write = TRUE;
                    //ch_buffer[0] = (char) (long1 & 0xFF);
                    //sys_write(0,ch_buffer,1);
                    return 0;
                }
            }

            //debug_print ("wmProcedure: done\n");
            return 0;
            break;
        };
        break;



// ==============
// msg:
// Syskeyup.
// liberadas: teclas de funçao
    case MSG_SYSKEYUP:
        // Se nenhum modificador esta acionado,
        // entao apenas enviamos a tecla de funçao 
        // para o window server.
        // Send it to the window server.
        if( shift_status != TRUE &&
            ctrl_status != TRUE &&
            alt_status != TRUE )
        {
            wmSendInputToWindowManager(0,MSG_SYSKEYUP,long1,long2); 
            return 0;
        }
        break;

// ==============
// msg:
// Syskeydown.
// Pressionadas: teclas de funçao
    case MSG_SYSKEYDOWN:

        // Se nenhum modificador esta acionado,
        // entao apenas enviamos a tecla de funçao 
        // para o window server.
        // Send it to the window server.
        if( shift_status != TRUE &&
            ctrl_status != TRUE &&
            alt_status != TRUE )
        {
            wmSendInputToWindowManager(0,MSG_SYSKEYDOWN,long1,long2); 
            return 0;
        }
        
        // Process a set of combinations.
        switch (long1){

            // Exibir a surface do console.
            case VK_F1:
                if (ctrl_status == TRUE){
                    // control + shift + F1
                    // Full ps2 initialization and launch the app.
                    if( shift_status == TRUE){
                        PS2_initialization();
                        __launch_app_via_initprocess(4001);
                        return 0;
                    }
                    // Only launch the app.
                    __launch_app_via_initprocess(4001);
                    return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f1\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    jobcontrol_switch_console(0);
                }
                return 0;
                break;

            case VK_F2:
                if (ctrl_status == TRUE){
                     __launch_app_via_initprocess(4002);
                     //powertrio_select_client(1);
                     return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f2\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    jobcontrol_switch_console(1);
                }
                return 0;
                break;

            case VK_F3:
                if (ctrl_status == TRUE){
                    //powertrio_select_client(2);
                    __launch_app_via_initprocess(4003);
                    return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f3\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    jobcontrol_switch_console(2);
                }
                return 0;
                break;

            case VK_F4:
                if (ctrl_status == TRUE){
                    //powertrio_next();
                    //__launch_app_via_initprocess(4004);
                    return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f4\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    jobcontrol_switch_console(3);
                }
                return 0;
                break;

            // Reboot
            case VK_F5:
                if (ctrl_status == TRUE){
                    //powertrio_select_client(0);
                    //reboot();
                    __launch_app_via_initprocess(4005);
                    return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f5\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    //kgwm_next();
                }
                return 0;
                break;

            // Send a message to the Init process.
            // 9216 - Launch the redpill application
            case VK_F6:
                if (ctrl_status == TRUE){
                    __launch_app_via_initprocess(4006);
                    //powertrio_select_client(1);
                    // #todo: 
                    // shutdown. Only the ring3 applications
                    // can shutdown via qemu for now. 
                    //__kgwm_SendMessageToInitProcess(9216); 
                    return 0; 
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f6\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    //kgwm_next();
                }
                return 0;
                break;

            // Test 1.
            case VK_F7:
                if (ctrl_status == TRUE){
                    __launch_app_via_initprocess(4007);
                    //powertrio_select_client(2);
                    // Send message to init process to launch gdeshell.
                    //__kgwm_SendMessageToInitProcess(9217);
                    return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f7\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    //kgwm_next();
                }
                return 0;
                break;

            // Test 2.
            case VK_F8:
                if (ctrl_status == TRUE){

                    // #test: IN: (wid,msg,long1,long2)
                    //wmSendInputToWindowManager(0,1,0,0);
                    //wmSendInputToWindowManager(0,2,0,0);
                    //wmSendInputToWindowManager(0,3,0,0);
                    //wmSendInputToWindowManager(0,4,0,0);
                    //wmSendInputToWindowManager(0,5,0,0);
                    //wmSendInputToWindowManager(0,6,0,0);
                    //wmSendInputToWindowManager(0,7,0,0);  //close
                    wmSendInputToWindowManager(0,MSG_PAINT,0,0);
                    //wmSendInputToWindowManager(0,9,0,0);
                    //wmSendInputToWindowManager(0,10,0,0);
                    //wmSendInputToWindowManager(0,18,0,0);   //set focus
                    //wmSendInputToWindowManager(0,19,0,0);   //get focus

                    //#oldtest
                    //powertrio_next();
                    // Send message to init process to launch the launcher.
                    //__kgwm_SendMessageToInitProcess(9216); 
                    //__kgwm_SendMessageToInitProcess(9218);  // launch sysmon
                        
                    //__launch_app_via_initprocess(4008);
                    return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f8\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    //kgwm_next();
                }
                return 0;
                break;

            case VK_F9:
                if (ctrl_status == TRUE){
                    __enter_embedded_shell(FALSE);
                    return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f9\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    // #goal
                    // This is a emergency keycombination.
                    // We can't call another process and 
                    // we want to reboot the machine.
                    sys_reboot();
                    //__kgwm_SendMessageToInitProcess(9216);  //reboot
                }
                return 0;
                break;

            case VK_F10:
                if (ctrl_status == TRUE){
                    __exit_embedded_shell();
                    return 0;
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f10\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    Background_initialize();
                    show_slots(); //See: tlib.c
                    //pages_calc_mem();
                    refresh_screen();
                    //__kgwm_SendMessageToInitProcess(9217);  //gdeshell
                }
                return 0;
                break;

            case VK_F11:
                // #todo:
                // [Window Manager]: Restore
                if (ctrl_status == TRUE)
                {
                    show_slots();
                    //tmp_value = get_update_screen_frequency();
                    //tmp_value--;
                    //set_update_screen_frequency(tmp_value);
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f11\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                   hal_reboot();
                   // #bugbug: Something is wrong with this routine.
                   //__kgwm_SendMessageToInitProcess(9218);  // redpill application
                }
                return 0;
                break;

            case VK_F12:
                // #todo:
                // [Window Manager]: Close
                if (ctrl_status == TRUE)
                {
                    show_process_information();
                    //tmp_value = get_update_screen_frequency();
                    //tmp_value++;
                    //set_update_screen_frequency(tmp_value);
                }
                if (alt_status == TRUE){
                    printf ("wmProcedure: alt + f12\n");
                    refresh_screen();
                }
                if (shift_status == TRUE){
                    wmSendInputToWindowManager(0,9090,0,0);   // Switch focus
                    //__kgwm_SendMessageToInitProcess(9219);  // sysmon
                    return 0;
                }
                return 0;
                break;

            default:
                // nothing
                return 0;
            
            }

// ==============
// msg:
// default
    default:
        return 0;
        break;
    };

//unexpected_fail:
    //debug_print("wmProcedure: unexpected fail\n");
    return 0;

fail:
    debug_print("wmProcedure: fail\n");
    refresh_screen();
    return 0;
}


/*
 * xxxKeyEvent:
 * 
 *     Envia uma mensagem de teclado para a janela com o 
 * foco de entrada.
 */

// Called by DeviceInterface_PS2Keyboard in ps2kbd.c

// Pega um scancode, transforma em caractere e envia 
// na forma de mensagem para a thread de controle associada 
// com a janela que tem o foco de entrada.

// #todo
// #importante
// Precisa conferir ke0 antes de construir a mensagem,
// para assim usar o array certo. ke0 indica o teclado estendido.

// #todo
// Na verdade é 'translate' and 'dispatch'.
// Cria o evento usando o rawbyte, traduz o raw byte para ascii,
// e por fim envia para o buffer da tty de teclado e para
// a thread do processo em foreground. Tudo depende
// do input mode.

// Called by console_interrupt() in console.c

// Is this the forground thread?
// #bugbug: Não estamos usando o parâmetro tid.
// Lembrando que numa interrupção de teclado,
// não temos o contexto salvo. Então não podemos chamar o
// scheduler. Mas podemos acionar uma flag
// que de alguma direção para o escalonador.

// IN:
// target thread, raw byte 

int 
xxxKeyEvent (
    int tid, 
    unsigned char raw_byte )
{

// #todo
// Devemos considerar os marcadores de teclado extendido
// obtidos em DeviceInterface_PS2Keyboard() em ps2kbd.c

    // ??
    // __has_e0_prefix and __has_e1_prefix

// Step 0 
// Declarações de variáveis.

    struct process_d  *__p;
    struct thread_d   *t;

    unsigned long status = 0;
    int msg_status = -1;
    int save_pos = 0;

// Pegando o argumento e convertendo para ascii
    unsigned char Keyboard_RawByte  =0;
    unsigned char Keyboard_ScanCode =0;    // The scancode.

    //==============
    // [event block]
    struct window_d  *Event_Window;            //arg1 - window pointer
    int               Event_Message       =0;  //arg2 - message number
    unsigned long     Event_LongASCIICode =0;  //arg3 - ascii code
    unsigned long     Event_LongRawByte   =0;  //arg4 - raw byte
    //===================

    // Inicializando a janela, 
    // pois os outros elementos já foram incializados logo acima.
    Event_Window = NULL;

    // true for keyup and false for keydown.
    // int Break = TRUE;

//#todo
    //debug_print("xxxKeyEvent:\n");

    if (tid<0 || tid >= THREAD_COUNT_MAX)
    {
        debug_print("xxxKeyEvent: tid\n");
        return (int) (-1);
    }


// =============
// Step1
// Pegar o RawByte.
// O driver pegou o scancode e passou para a disciplina de linha 
// através de parâmetro.

    Keyboard_RawByte = raw_byte;

    //if ( Keyboard_RawByte == 0 )
        //return -1;

    if ( Keyboard_RawByte == 0xFF )
        return -1;

// #todo
// The last byte was a prefix
    //if( __has_e0_prefix == 1 )
        //goto GotE0;

// #todo
// The last byte was a prefix
    //if(__has_e1_prefix == 1 )
        //goto GotE1;


//
// Debug
//

// ++
// ===================

    // #debug.
    // Show the scancode if the flag is enabled.
    // Talvez isso nem seja necess'ario.
    
    // #hackhack
    // Variable not initialized yet.

    /*
    scStatus = FALSE;
    if (scStatus == TRUE){
        printf ("raw byte {%d,%x} \n", 
            Keyboard_RawByte, Keyboard_RawByte );
        // Refresh screen?
        refresh_screen();
    }
    */

// ===================
// --

// #todo
// Enable these shortcuts
// Se temos a flag ou não
    //if ( Keyboard_RawByte > 0x7F )
         //goto key_released;
    //if ( Keyboard_RawByte < 0x7F )
         //goto key_pressed;


// ==========
// Step 2
// Tratar as mensagens.
// Traduzir rawbyte em evento.

// #test
    //int isDown=0;
    //isDown = !(Keyboard_RawByte & 0x80);


// ================================================
//key_released:

    // Não está pressioanda
    if ( (Keyboard_RawByte & 0x80) != 0 )
    {
        // Break = TRUE;
        
        // Desativando o bit de paridade caso esteja ligado.

        Keyboard_ScanCode = Keyboard_RawByte;
        Keyboard_ScanCode &= KEYBOARD_KEY_MASK;

        // Configurando se é do sistema ou não.
        // #todo: 
        // Aqui podemos chamar uma rotina interna que faça essa checagem.
        // Os primeiros 'case' é quando libera tecla do sistema.
        // O case 'default' é pra quando libera tecla que não é do sistema.

        switch (Keyboard_ScanCode)
        {
            // left shift liberado.
            case VK_LSHIFT: 
                shift_status = FALSE;  Event_Message = MSG_SYSKEYUP;
                //printf ("VK_LSHIFT: Liberada\n"); refresh_screen();
                break;

            // left control liberado.
            case VK_LCONTROL:
                ctrl_status = FALSE;  Event_Message = MSG_SYSKEYUP;
                //printf ("VK_LCONTROL: Liberada\n"); refresh_screen();
                break;

            // left winkey liberada.
            case VK_LWIN:
                winkey_status = FALSE;  Event_Message = MSG_SYSKEYUP;
                //printf ("VK_LWIN: Liberada\n"); refresh_screen();
                break;

            // left alt liberado.
            case VK_LMENU:
                alt_status = FALSE;  Event_Message = MSG_SYSKEYUP;
                //printf ("VK_LMENU: Liberada\n"); refresh_screen();
                break;

            // right winkey liberada.
            case VK_RWIN:
                winkey_status = FALSE;  Event_Message = MSG_SYSKEYUP;
                //printf ("VK_RWIN: Liberada\n"); refresh_screen();
                break;

            // control menu.
            case VK_CONTROL_MENU:
                //controlmenu_status = 0; //@todo
                Event_Message = MSG_SYSKEYUP;
                //printf ("VK_CONTROL_MENU: Liberada\n"); refresh_screen();
                break;

            //right control liberada.
            case VK_RCONTROL:
                ctrl_status = FALSE;  Event_Message = MSG_SYSKEYUP;
                //printf ("VK_RCONTROL: Liberada\n"); refresh_screen();
                break;

            // right Shift liberado.
            case VK_RSHIFT:
                shift_status = FALSE;  Event_Message = MSG_SYSKEYUP;
                //printf ("VK_RSHIFT: Liberada\n"); refresh_screen();
                break;

            // Funções liberadas.
            case VK_F1: case VK_F2: case VK_F3: case VK_F4:
            case VK_F5: case VK_F6: case VK_F7: case VK_F8:
            case VK_F9: case VK_F10: case VK_F11: case VK_F12:
                Event_Message = MSG_SYSKEYUP;
                break;

            // ...

            // A tecla liberada NÃO é do sistema.
            default:
                Event_Message = MSG_KEYUP;
                break;
        };

        // Selecionando o char para os casos de tecla liberada.
        
        // Analiza: 
        // Se for tecla normal, pega o mapa de caracteres apropriado.
        // minúscula
        // Nenhuma tecla de modificação ligada.

        // #bugbug: some chars are not working
        // for shift_abnt2[]
        // See: include/user/kbdabnt2.h
        if (Event_Message == MSG_KEYUP)
        {
            // Minúsculas.
            if (capslock_status == FALSE)
            { Event_LongASCIICode = map_abnt2[Keyboard_ScanCode];   goto done; }
            // Maiúsculas.
            if (capslock_status == TRUE)
            { Event_LongASCIICode = shift_abnt2[Keyboard_ScanCode]; goto done; }
            // ...
        }

        // Analiza: Se for do sistema usa o mapa de caracteres apropriado. 
        // Normal.
        
        if (Event_Message == MSG_SYSKEYUP)
        {   
            // se liberada teclas de sistema como capslock ligado
            if (capslock_status == FALSE)
            { Event_LongASCIICode = map_abnt2[Keyboard_ScanCode];   goto done; }
            // se liberada teclas de sistema como capslock desligado
            if (capslock_status == TRUE)
            { Event_LongASCIICode = shift_abnt2[Keyboard_ScanCode]; goto done; }
            // ...
        }


        // Nothing.
        goto done;
    }// FI


// ================================================
// key_pressed:

    // Está pressionada.
    if ( (Keyboard_RawByte & 0x80) == 0 ) 
    {
        // Break = FALSE;
        
        Keyboard_ScanCode = Keyboard_RawByte;
        Keyboard_ScanCode &= KEYBOARD_KEY_MASK; //Desativando o bit de paridade caso esteja ligado.

        // O Último bit é zero para key press.
        // Checando se é a tecla pressionada é o sistema ou não.
        // #todo: 
        // Aqui podemos chamar uma rotina interna que faça essa checagem.

        switch (Keyboard_ScanCode)
        {
            // back space será tratado como tecla normal

            // #todo: tab,
            // tab será tratado como tecla normal por enquanto.

            // caps lock keydown
            // muda o status do capslock não importa o anterior.
            case VK_CAPITAL:
                if (capslock_status == FALSE){ 
                    capslock_status = TRUE; 
                    Event_Message = MSG_SYSKEYDOWN; 
                    break; 
                }
                if (capslock_status == TRUE){ 
                    capslock_status = FALSE; 
                    Event_Message = MSG_SYSKEYDOWN; 
                    break; 
                }
                break; 

            // left shift pressionada.
            // case KEY_SHIFT:
            case VK_LSHIFT:
                shift_status = TRUE; Event_Message = MSG_SYSKEYDOWN;
                //printf ("VK_LSHIFT: Pressionada\n"); refresh_screen();
                break;

            // left control pressionada.
            // case KEY_CTRL:
            case VK_LCONTROL:
                ctrl_status = TRUE; Event_Message = MSG_SYSKEYDOWN;
                //printf ("VK_LCONTROL: Pressionada\n"); refresh_screen();
                break;

            // left winkey pressionada.
            case VK_LWIN:
                winkey_status = TRUE; Event_Message = MSG_SYSKEYDOWN;
                //printf ("VK_LWIN: Pressionada\n"); refresh_screen();
                break;

            // left alt pressionada.
            case VK_LMENU:
                alt_status = TRUE;  Event_Message = MSG_SYSKEYDOWN;
                //printf ("VK_LMENU: Pressionada\n"); refresh_screen();
                break;

            // #todo 
            // alt gr.

            // right winkey pressionada.
            case VK_RWIN:
                winkey_status = TRUE;  Event_Message = MSG_SYSKEYDOWN;
                //printf ("VK_RWIN: Pressionada\n"); refresh_screen();
                break;

            // #todo: 
            // Control menu.

            // right control pressionada.
            case VK_RCONTROL:
                ctrl_status = TRUE; Event_Message = MSG_SYSKEYDOWN;
                //printf ("VK_RCONTROL: Pressionada\n"); refresh_screen();
                break;

            // right shift pressionada.
            case VK_RSHIFT:
                shift_status = TRUE; Event_Message = MSG_SYSKEYDOWN;
                //printf ("VK_RSHIFT: Pressionada\n"); refresh_screen();
                break;

            // Funções pressionadas.
            case VK_F1: case VK_F2: case VK_F3: case VK_F4:
            case VK_F5: case VK_F6: case VK_F7: case VK_F8:
            case VK_F9: case VK_F10: case VK_F11: case VK_F12:
                Event_Message = MSG_SYSKEYDOWN;
                break;

            // num Lock.
            // Muda o status do numlock não importa o anterior.
            case VK_NUMLOCK:
                if (numlock_status == FALSE){
                    numlock_status = TRUE; Event_Message = MSG_SYSKEYDOWN;
                    break;
                }
                if (numlock_status == TRUE){ 
                    numlock_status = FALSE; Event_Message = MSG_SYSKEYDOWN; 
                    break; 
                }
                break;

            // Scroll Lock.
            // Muda o status do numlock não importa o anterior.
            case VK_SCROLL:
                if (scrolllock_status == FALSE){  
                    scrolllock_status = TRUE; Event_Message = MSG_SYSKEYDOWN;
                    break;
                }
                if (scrolllock_status == TRUE){ 
                    scrolllock_status = FALSE; Event_Message = MSG_SYSKEYDOWN; 
                    break; 
                }
                break;

            //...

            // A tecla pressionada não é do sistema.
            default:
                // printf("keyboard debug: default: MSG_KEYDOWN\n");
                Event_Message = MSG_KEYDOWN;
                break;
        };

        // ==============================
        // Teclas de digitaçao
        // Uma tecla normal foi pressionada.
        // mensagem de digitação.
        if (Event_Message == MSG_KEYDOWN)
        {
            // Minúsculas.
            if (capslock_status == FALSE)
            { Event_LongASCIICode = map_abnt2[Keyboard_ScanCode];   goto done; }
            // Maiúsculas.
            if (capslock_status == TRUE)
            { Event_LongASCIICode = shift_abnt2[Keyboard_ScanCode]; goto done; }
            // ...
        }

        // ==============================
        // Teclas de sistema
        // Uma tecla do sistema foi pressionada.
        if (Event_Message == MSG_SYSKEYDOWN)
        {   
            // se pressionamos teclas de sistema como capslock ligado
            if (capslock_status == FALSE)
            { Event_LongASCIICode = map_abnt2[Keyboard_ScanCode];   goto done; }
            // se pressionamos teclas de sistema como capslock desligado
            if (capslock_status == TRUE)
            { Event_LongASCIICode = shift_abnt2[Keyboard_ScanCode]; goto done; }
            // ...
        }

        // Nothing.
        goto done;
        
    } // FI

//
// == Dispatch ====
//

// Done.
// Para finalizar, vamos enviar a mensagem para fila certa.
// Fixing the rawbyte to fit in the message arg.
// See: kgwm.c

done:

    Event_LongRawByte = (unsigned long) ( Keyboard_RawByte & 0x000000FF );

// Não tem virtual key '0'.
    if ( Event_LongASCIICode == 0 )
        return -1;

// #todo
// Check 'control + alt + del'.
// Teclas de digitação.
// Manda para o window server.
// Ele vai imprimir na janela com foco de entrada
// e enviar mensagem para a thread com foco.

/*
    if ( Event_Message == MSG_KEYDOWN )
    {
        if ( ShellFlag!=TRUE ){
            wmSendInputToWindowManager(0, MSG_KEYDOWN, Event_LongASCIICode, Event_LongRawByte);
            return 0;
        }
    }
*/

// #test
// Colocando no arquivo.
// Não colocamos quando estivermos no modo 'console',
// pois nesse caso o aplicativo em ring3 receberia input de teclado
// e reagiria a esse input.
// Write:
// Estamos dentro do kernel.
// Vamos escrever diretamente no buffer do arquivo,
// dado seu ponteiro de estrutura.
// O fd do processo atual não serveria nesse momento.
// See: sys.c

    char ch_buffer[2];

    // Colocamos no arquivo somente se não estivermos
    // no modo console.
    if ( ShellFlag != TRUE )
    {
        if ( Event_Message == MSG_KEYDOWN )
        {
            stdin->sync.can_write = TRUE;
            stdin->_flags = __SWR;
            ch_buffer[0] = (char) (Event_LongASCIICode & 0xFF);

            file_write_buffer ( 
                (file *) stdin, 
                (char *) ch_buffer, 
                (int) 1 );

            stdin->sync.can_read = TRUE;  // O aplicativo precisa disso.
            stdin->_flags = __SRD;        // O worker no kernel precisa disso.
        }
    }

// Process the event using the system's window procedures.
// It can use the kernel's virtual console or
// send the event to the loadable window server.
// See: kgwm.c

    wmProcedure(
        (struct window_d *) Event_Window,    // opaque pointer
        (int)               Event_Message,
        (unsigned long)     Event_LongASCIICode,
        (unsigned long)     Event_LongRawByte );

    return 0;
}


//==========

// For mouse events, see: window.h

// #todo: change parameters.
// we need more information about the mouse event.
int 
xxxMouseEvent(
    int event_id,
    long long1, 
    long long2 )
{

    unsigned long button_number = long1;
    //debug_print ("xxxMouseEvent:\n");

// Buttons:
// Pressionado ou liberado
    if( event_id == MSG_MOUSEPRESSED ||
        event_id == MSG_MOUSERELEASED )
    {
        wmProcedure(
            (struct window_d *) 0,                // opaque pointer
            (int)               event_id,         // msg code
            (unsigned long)     button_number,    // button number
            (unsigned long)     button_number );  // button number
        return 0;
    }


    //old: for ereasing
    static long old_x=0;
    static long old_y=0;

    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();


    deviceWidth  = (deviceWidth & 0xFFFF);
    deviceHeight = (deviceHeight & 0xFFFF);

    if ( long1 < 1 ){ long1 = 1; }
    if ( long2 < 1 ){ long2 = 1; }
    //if ( x > (SavedX-16) ){ x = (SavedX-16); }
    //if ( y > (SavedY-16) ){ y = (SavedY-16); }

    if ( long1 > (deviceWidth-1)  ){ long1 = (deviceWidth-1);  }
    if ( long2 > (deviceHeight-1) ){ long2 = (deviceHeight-1); }


//----

// #test
// Draw rectangle.
// #todo #bugbug
// Isso está falhando ...
// Existe algum problema na rotina de retângulo
// que gera PF. Provavelmente é alguma coisa na
// tipagem de algumas variáveis ... pois esse
// é um código portado da arquitetura de 32bit 
// para a arquitetura de 64bit.

    if ( event_id == MSG_MOUSEMOVE )
    {
        // #bugbug
        // #todo
        // Essa rotina de exibiçao de cursor deve acontecer
        // la no window server, quando enviarmos as
        // informaçoes de evento para la. Nao aqui.
        // See: gramado/core/server/wm.c
        // Erease cursor.
        // Ereasing the cursor by refreshing a little
        // part of the backbuffer into the front buffer.
        //refresh_rectangle ( old_x, old_y, 10, 10 );
        //old_x = long1;
        //old_y = long2;
        // Drawing the cursor.
        // Drawing the cursor directly into
        // the framebuffer.
        //frontbuffer_draw_rectangle( 
        //    long1, long2, 10, 10, COLOR_RED, 0 );

        wmProcedure(
            (struct window_d *) 0,         // opaque pointer
            (int)               event_id,  // msg code
            (unsigned long)     long1,         // x
            (unsigned long)     long2 );       // y
    }
//----


//
// == Dispatch ====
//

// Process the event using the system's window procedures.
// It can use the kernel's virtual console or
// send the event to the loadable window server.
// See: kgwm.c
// For mouse events, see: window.h

/*
    wmProcedure(
        (struct window_d *) 0,         // opaque pointer
        (int)               event_id,  // msg code
        (unsigned long)     long1,         // x
        (unsigned long)     long2 );       // y
*/
done:
    //debug_print ("xxxMouseEvent: Done\n");
    return 0;
}


// called by I_init()
// OUT: return TRUE if its ok.
int init_gramado (void)
{
    debug_print ("init_gramado: [fixme]\n");
    return TRUE;
}


/*
 ***********************************
 * windowLoadGramadoIcons:
 * 
 *     Carrega alguns ícones do sistema.
 *     It's a part of the window system's initialization.
 */

int windowLoadGramadoIcons (void)
{
    unsigned long fRet=0;

	//#debug
	//printf("windowLoadGramadoIcons:\n");

	//
	//  ## Icon support ##
	//

//iconSupport:

	// Carregando alguns ícones básicos usados pelo sistema.

	// ## size ##
	// Vamos carregar ícones pequenos.
	//@todo checar a validade dos ponteiros.


    // #bugbug
    // Size determinado, mas não sabemos o tamanho dos ícones.

    // 4 pages.
    // 16 KB ? Is it enough ?
    // Sim, os ícones que estamos usam possuem no máximo 2KB.
    // See: base/

    unsigned long tmp_size = (4*4096);

    // See: window.h
    shared_buffer_app_icon       = (void *) allocPages(4);
    shared_buffer_file_icon      = (void *) allocPages(4);
    shared_buffer_folder_icon    = (void *) allocPages(4);
    shared_buffer_terminal_icon  = (void *) allocPages(4);
    shared_buffer_cursor_icon    = (void *) allocPages(4);
    // ...


    if ( (void *) shared_buffer_app_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_app_icon\n");
    }

    if ( (void *) shared_buffer_file_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_file_icon\n");
    }

    if ( (void *) shared_buffer_folder_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_folder_icon\n");
    }

    if ( (void *) shared_buffer_terminal_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_terminal_icon\n");
    }

    if ( (void *) shared_buffer_cursor_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_cursor_icon\n");
    }

	//
	// Load
	//

    // app icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS,
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES,
                               "APP     BMP", 
                               (unsigned long) shared_buffer_app_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: APP.BMP\n");
    }

    // file icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS,
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES, 
                               "FILE    BMP", 
                               (unsigned long) shared_buffer_file_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: FILE.BMP\n");
    }

    // folder icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS, 
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES,
                               "FOLDER  BMP", 
                               (unsigned long) shared_buffer_folder_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: FOLDER.BMP\n");
    }

    // terminal icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS, 
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES, 
                               "TERMINALBMP", 
                               (unsigned long) shared_buffer_terminal_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: TERMINAL.BMP\n");
    }


    // cursor icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS, 
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES,
                               "CURSOR  BMP", 
                               (unsigned long) shared_buffer_cursor_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: CURSOR.BMP\n");
    }

	// More ?

    return 0;
}




// Get a shared buffer to a system icon.
// it is gonna be used by the window server.
// It is a pre allocated buffer containg an bmp icon loaded at it.
// Service 9100

void *ui_get_system_icon ( int n )
{
    if (n <= 0){ return NULL; }

    // See: window.h
    switch (n){

    case 1: return (void *) shared_buffer_app_icon;       break;
    case 2: return (void *) shared_buffer_file_icon;      break;
    case 3: return (void *) shared_buffer_folder_icon;    break;
    case 4: return (void *) shared_buffer_terminal_icon;  break;
    case 5: return (void *) shared_buffer_cursor_icon;    break;
    // ...
    };

    return NULL;
}



// =========================

unsigned long get_update_screen_frequency(void)
{
    return (unsigned long) flush_fps;
}

void set_update_screen_frequency(unsigned long fps)
{
    if(fps==0){fps=1;};
    if(fps>=1000){fps=1000;};

// See: sched.h
    flush_fps = (unsigned long) (fps&0xFFFF);

    presence_level = (unsigned long)(1000/flush_fps);
    presence_level = (unsigned long) (presence_level & 0xFFFF);
}

unsigned long get_presence_level(void)
{
    return (unsigned long) presence_level;
}

void set_presence_level(unsigned long value)
{
    if(value==0){ value=1; }
    if(value>1000){ value=1000; }
    presence_level = value;
}


// Called by task_switch
void schedulerUpdateScreen(void)
{
    int i=0;
    struct thread_d *TmpThread;

    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();

    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("schedulerUpdateScreen: w h\n");
        panic       ("schedulerUpdateScreen: w h\n");
    }

// Atualizado pelo timer.
    if( UpdateScreenFlag != TRUE )
        return;

    deviceWidth  = (deviceWidth & 0xFFFF);
    deviceHeight = (deviceHeight & 0xFFFF);


// ============================
// Redraw and flush stuff.

// Calling the window manager inside the window server
// at gwssrv.bin
// 9091 = message code for calling the compositor.

    if( gUseWMCallbacks == TRUE)
        wmSendInputToWindowManager(0,9091,0,0);

// ============================

    // Precisamos apenas validar todos retangulos
    // porque fizemos refresh da tela toda.
    
    int validate_all= FALSE;

//
// Flush the whole screen and exit.
//

// The whole screen is invalidated.
    if ( screen_is_dirty == TRUE )
    {
        refresh_screen();
        validate_all = TRUE;
        
        // Validate the screen
        screen_is_dirty = FALSE;
    }


/*
//=========================
// Blue bar:
    unsigned long fps = get_update_screen_frequency();
    char data[32];
    drawDataRectangle( 
        0, 0, deviceWidth, 24, COLOR_BLUE, 0 );
    sprintf(data,"  FPS %d       ",fps);
    data[12]=0;
    draw_string(0,8,COLOR_YELLOW,data);
    refresh_rectangle ( 0, 0, deviceWidth, 24 );
//=========================
*/


//
// Flush a list of dirty surfaces.
//

    for ( i=0; i < THREAD_COUNT_MAX; ++i )
    {
        TmpThread = (void *) threadList[i];

        if ( (void *) TmpThread != NULL )
        {
            if ( TmpThread->used  == TRUE && 
                 TmpThread->magic == 1234 && 
                 TmpThread->state == READY )
            {
                // #test 
                debug_print("  ----  Compositor  ----  \n");
                
                if ( (void *) TmpThread->surface_rect != NULL )
                {
                    if( TmpThread->surface_rect->used == TRUE && 
                        TmpThread->surface_rect->magic == 1234 )
                    {
                        // Como fizemos refresh da tela toda,
                        // então precisamos validar todos os retângulos.
                        
                        if ( validate_all == TRUE )
                            TmpThread->surface_rect->dirty = FALSE;

                        // dirty rectangle
                        // Se uma surface está suja de tinta.
                        // Precisamos copiar para o framebuffer.


                        if( TmpThread->surface_rect->dirty == TRUE )
                        {
                            refresh_rectangle ( 
                                TmpThread->surface_rect->left, 
                                TmpThread->surface_rect->top, 
                                TmpThread->surface_rect->width, 
                                TmpThread->surface_rect->height );
                            
                            // Validate the surface. (Rectangle)
                            TmpThread->surface_rect->dirty = FALSE;
                        }

                    }
                }
            }
        }
    };
    

// Chamamos o 3d demo do kernel.
// See: kgws.c

    if (DemoFlag==TRUE)
    {
        //demo0();
        DemoFlag=FALSE;
    }


// Atualizado pelo timer.
    UpdateScreenFlag = FALSE;
}












