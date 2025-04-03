#include <stdint.h>
#include "uart.h"
#include "memory.h"
#include "fat12.h"
#include "vfs.h"
#include "mmc.h"
#include "net.h"
#include "proc.h"
#include "syscall.h"

#define GREEN 		"\033[0;92m"
#define RESET  		"\e[0m"
#define ENTER		13
#define BACKSPACE	8

#define SHELL_BUFFER_SIZE 256
#define TOKEN_BUFFER_SIZE 64

int drawInitialized() {
	uart0_printf("                       __________          .___  .___       ________    _________\n");
	uart0_printf("                      \\______   \\__ __  __| _/__| _/__.__. \\_____  \\  /   _____/\n");
	uart0_printf("                       |    |  _/  |  \\/ __ |/ __ <   |  |  /   |   \\ \\_____  \\ \n");
	uart0_printf("                       |    |   \\  |  / /_/ / /_/ |\\___  | /    |    \\/        \\ \n");
	uart0_printf("                       |______  /____/\\____ \\____ |/ ____| \\_______  /_______  /\n");
	uart0_printf("                              \\/           \\/    \\/\\/              \\/        \\/\n");
	uart0_printf("\n");

	uart0_printf("                 ,                                            `\\-.   `\n");
	uart0_printf("               |',   ,                                          \\ `.  `\n");
	uart0_printf("              .\\   '-.|/,                                        \\  \\ |\n");
	uart0_printf("             .|       '  '-.                              __.._  |   \\.          \n");
	uart0_printf("             |              '-.  ,               ..---\"\"     \" . |    Y\n");
	uart0_printf("            '|                 ',|/                \"-.          `|    |\n");
	uart0_printf("         ,  |                    ' /                   `.               `\"\"--.\n");
	uart0_printf("          /',                       /                    \\                    \".\n");
	uart0_printf("          /              _          |  .                  \\                     \\__. . -- -  .\n");
	uart0_printf("           |           .` ',    .-. '`\\             .-\"\"\"\"\"      ,    ,            \"\"\"\"\"---...._\n");
	uart0_printf("           /          \\      / \\   / \\           .-\"___        ,'/  ,'/ ,'\\          __...---\"\"\"\n");
	uart0_printf("         ,  |        |        V    |\\                   \"\".    /._\\_( ,(/_. 7,-.    \"\"---...__\n");
	uart0_printf("          /\"''   .--./ )           (                   _...>-  P\"\"6=`_/\"6\"\"   6)    ___...--\"\"\"\n");
	uart0_printf("           '-,   |)6 ''ee._ ', .`.b\"                    \"\"--._ \\`--') `---'   9'  _..--\"\"\"\n");
	uart0_printf("              '-./ '   )  \"\"-- =\"\\                           \" \\\"\"/_  \"\"\"   /`-.--\"\"\n");
	uart0_printf("                 '|\".   \"---` / \\                              `. ---    .'   \\_\n");
	uart0_printf("                 .|  '.    -..\"\\                                 `.\" _.-'     | \"-.,-------._\n");
	uart0_printf("                 |   ','-..__-`                              ..._../\"\"   ./       .-'    .-\"\"\"-.\n");
	uart0_printf("              _.\\/    /'-. ' ).__                      ,--\"\"\" ,'...\\` _./.----\"\".'/    /'       `-\n");
	uart0_printf("         _.-\"\"-._ .--------..----/'--._            _.-(      |\\    `\"/ _____..-' /    /      _.-\"\"`.\n");
	uart0_printf("     .--'-._     '-..__     /   .\\   )_'-.        /   |     /. ^---\"\"\"\"       ' /    /     ,'  \".   \\\n");
	uart0_printf("   .` .   .-',         \"\"\"\"\"\"\"\"\"         \\        (   /     (  .           _ ' /'    /    ,/      \\  )\n");
	uart0_printf("  / .`   `    /                        \\ ' |      (`. |     `\\   - - - - ~   /'      (   /         . |\n");
	uart0_printf("  | |         |                        |   |       \\.\\|       \\            /'        \\  |`.          /\n");
	uart0_printf("  |     \\     |                        /'. /       /.'\\\\      `\\         /'           \"-\\         . /\\\n");
	uart0_printf("  \\ /  |    .`\\  - .                ', |  ' \\     /,   (        `\\     /'                `.___..-     \\\n");
	uart0_printf("  /\\ '- .--\"/`                       | |   \\ \\   | |    \\         `\\_/'                  //      \\.    |\n");
	uart0_printf("   |   \\   /                         \\ \\    | |  | |     |                             /' |       |    |\n");
	uart0_printf("   |   |   |-.                        \\ |   \\ |  | |     |                             |  |       |    |\n");
	uart0_printf("\n");
	return 0;
}

int strcmp(char *str1, char *str2) {
	int i = 0;
	while (str1[i] == str2[i]) {
		if (str1[i] == '\0' || str2[i] == '\0') {
			break;
		}
		i++;
	}

	if (str1[i] == '\0' && str2[i] == '\0') {
		return 0;
	} else {
		return str1[i] - str2[i];
	}
}

void cat(char* filename) {
	char buf[65];
	const char prefix[] = "/home/";
	int prefixLen = 6;
	int filenameLen = 0;

	while (filename[filenameLen] != '\0') {
		filenameLen++;
	}

	char fullpath[prefixLen + filenameLen + 1];

	for (int i = 0; i < prefixLen; i++) {
		fullpath[i] = prefix[i];
	}

	for (int j = 0; j < filenameLen; j++) {
		fullpath[prefixLen + j] = filename[j];
	}

	fullpath[prefixLen + filenameLen] = '\0';

	int fd = vfs_open(fullpath, O_READ);
	int bytes = vfs_read(fd, buf, 64);
	while (bytes > 0) {
		buf[64] = '\0';
		uart0_printf("%s", buf);
		bytes = vfs_read(fd, buf, 64);
	}	
	vfs_close(fd);
}

void exec(char* filename) {
	char buf[65];
	const char prefix[] = "/home/";
	int prefixLen = 6;
	int filenameLen = 0;

	while (filename[filenameLen] != '\0') {
		filenameLen++;
	}

	char fullpath[prefixLen + filenameLen + 1];

	for (int i = 0; i < prefixLen; i++) {
		fullpath[i] = prefix[i];
	}

	for (int j = 0; j < filenameLen; j++) {
		fullpath[prefixLen + j] = filename[j];
	}

	fullpath[prefixLen + filenameLen] = '\0';

    uart0_printf("%s",fullpath);

    __f_exec(fullpath);
	
}

int parseShellCommands(char** tokens) {
	if (strcmp(tokens[0], "exit") == 0) {
		uart0_printf("BuddyOS exiting...");
		return 0;	
	} else if (strcmp(tokens[0], "help") == 0) {
		uart0_printf("Available commands\n");
		uart0_printf("help - shows this message\n");
		uart0_printf("exit - exits the shell\n");
		uart0_printf("ls - displays file system\n");
		uart0_printf("clear - clears terminal\n");
		uart0_printf("cat - display contents of file\n");
		uart0_printf("echo <text> - prints text to terminal\n");
	} else if (strcmp(tokens[0], "clear") == 0) {
		uart0_printf("\033[2J\033[H");	
	} else if (strcmp(tokens[0], "echo") == 0) {
		if (tokens[1] != 0) {
            		for (int i = 1; tokens[i] && tokens[i][0] != '\0'; i++) {
                		uart0_printf("%s ", tokens[i]);
           		 }
        	}
        	uart0_printf("\n");
	} else if (strcmp(tokens[0], "ls") == 0) {
		uint32_t buffer[128];
		uint32_t allFlag = (tokens[1] && strcmp(tokens[1], "-a") == 0);
		list_dir(buffer, allFlag);
	} else if (strcmp(tokens[0], "cat") == 0) {
		cat(tokens[1]);
		uart0_printf("\n");
	} else if (strcmp(tokens[0], "ping") == 0) {
		
    uint32_t ip = strtoip(tokens[1]);
  
    uint8_t gateway_mac[MAC_ADDR_LEN] = {0xD8,0xBB,0xC1,0xF7,0xD0,0xD3};

    icmp_echo_request(ip, gateway_mac);

    delay();

		uart0_printf("\n");
  }else if (strcmp(tokens[0], "exec") == 0) {
		exec(tokens[1]);
		uart0_printf("\n");
  }

	return 1;	
}


int shell(){
    uart0_printf("Entering Kernel\n");
    drawInitialized();   
    static char buffer[SHELL_BUFFER_SIZE];
    char userChar;
    int charIndex;
    int tokenCount;
    int i;
    
    static char *tokens[TOKEN_BUFFER_SIZE] = {((void *)0)};
    int shellStatus = 1;

    do {
	    uart0_printf(GREEN "root>/home$ " RESET);
	    userChar = '\0';
	    charIndex = 0;
	    tokenCount = 0;
	    i = 0;

      while (userChar != ENTER && charIndex < SHELL_BUFFER_SIZE - 1) {
		    userChar = uart0_getch(); 
		    if (userChar == BACKSPACE && charIndex > 0) {
			    uart0_printf("\b \b");
			    charIndex--;
		    } else if (userChar != ENTER) {
			    uart0_printf("%c", userChar);
			    buffer[charIndex++] = userChar;
		    }
	    }
	
      buffer[charIndex] = '\0';

	    while (buffer[i] == ' ') {
		    i++;
	    }

	    while (buffer[i] != '\0' && tokenCount < TOKEN_BUFFER_SIZE - 1) {
		    tokens[tokenCount++] = &buffer[i];
		
		    while (buffer[i] != ' ' && buffer[i] != '\0') {
			    i++;
		    }

		    if (buffer[i] == ' ') {
			    buffer[i] = '\0';
			    i++;
		    }
	    }
	
	    uart0_printf("\n");

	    if (tokenCount > 0) {
		    shellStatus = parseShellCommands(tokens);
	    }

	    kmemset32(buffer, 0, sizeof(buffer));
	    kmemset32(tokens, 0, sizeof(tokens));

    } while (shellStatus);

    uart0_printf("Exited shell successfully");
    return 0;
}

