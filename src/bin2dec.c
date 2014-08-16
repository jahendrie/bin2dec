/*******************************************************************************
 * bin2dec.c    |   version 1.4     |   FreeBSD License     |   2014-04-20
 * James Hendrie                    |   hendrie.james@gmail.com
 *
 *  Description:
 *      Converts binary numbers to decimal integers
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_LENGTH 256
#define BIN_STR_LENGTH 7


/*  --------------------    Global Options ----------------------- */
int bin2bin;        //  Binary output
int bin2dec;        //  Do binary to decimal conversion (default)
int bin2hex;        //  binary to hex
int bin2phex;       //  binary to precise hex
int bin2oct;        //  binary to octal
int sections;       //  print in 4-block sections if appropriate
int hexCaps;
int phexCaps;
int lineSpacing;    //  prettier output if desired
int verbose;        //  Verbosity
int textMode;       //  Text conversion mode
int totalConversions;   //  Total number of conversions

static const char *optString = "bdvxXaAoslht";


/*  Print the usage text */
void print_usage(FILE *fp)
{
    fprintf( fp, "Usage:\tbin2dec [OPTIONS] BINARY_NUMBER[s]\n");
}


/*  Print the help text */
void print_help(void)
{
    /*  Print usage */
    print_usage(stdout);

    /*  Yeah, I know this is ugly.  */
    printf("\nThis program converts a binary number to a decimal number.  For");
    printf(" example, issuing\n\n");
    printf("\t'bin2dec 1011'\n\nwould result in a decimal output of 11.\n");

    printf("\nOptions:\n");
    printf("  -h or --help\tPrint this help text\n");
    printf("  --version\tPrint version and author info\n");
    printf("  -\t\tPipe from stdin\n");
    printf("  -v\t\tEnable verbose output (print what type each result is)\n");
    printf("  -d\t\tDecimal output (default)\n");
    printf("  -b\t\tBinary output\n");
    printf("  -o\t\tOctal output\n");
    printf("  -x\t\tHexadecimal output\n");
    printf("  -X\t\tHex output with capital letters\n");
    printf("  -a\t\tPrecise hex output (printf %%a, see 'man 3 printf')\n");
    printf("  -A\t\tSame, but with capital letters\n");
    printf("  -s\t\tOutput in 4-character sections, space-separated\n");
    printf("  -l\t\tPrint a new line between sections of text\n");
    printf("  -t\t\tText conversion mode\n");
}


/*  version and author info */
void print_version(void)
{
    printf("bin2dec version 1.4\n");
    printf("James Hendrie - hendrie.james@gmail.com\n");
}


/*  Memory error; *description is sent from calling function */
void mem_error( const char *description )
{
    fprintf(stderr, "ERROR:  Out of memory\n");
    fprintf(stderr, "%s\n", description);
}


/*  Check to make sure that the string has nothing but binary chars (1 or 0) */
int check_for_binary( char *s )
{
    int i = 0;
    for( i = 0; i < strlen(s); ++i )
    {
        /*  Check for nulls or linefeeds */
        if( s[i] == '\0' || s[i] == '\n' )
            break;

        /*  Run through the string, make sure all chars are either 0 or 1 */
        if( s[i] != '0' && s[i] != '1' )
        {
            return(1);
        }
    }

    return(0);
}


/*  Multiply 'number' to the power of 'power' */
unsigned int power_of( int number, int power )
{
    /*  If we're multiplying to the power of zero */
    if( power == 0 )
        return(1);

    int current = number;

    /*  For loop; make my number, GROW!!!! */
    int temp = 0;
    for( temp = 1; temp < power; ++temp )
    {
        current = current * number;
    }

    /*  Return finished number */
    return( current );
}


/*
 * Take a string (char *s), which has previously been verified to contain only
 * binary characters (1 or 0), and convert it to a decimal number
 */
double binary_to_decimal( char *s )
{
    /*  Our initial variables */
    unsigned int placement = 0;
    double total = 0;

    /*  We're starting on the 'right side' of the binary string */
    int i = 0;
    for( i = strlen(s); i >= 0; --i )
    {
        /*  If we hit a newline or a null char, just ignore it */
        if( s[i] == '\n' || s[i] == '\0' )
            continue;

        /*
         * If we hit a zero, move to the 'left' but don't add to the decimal
         * total
         */
        if( s[i] == '0' )
        {
            ++placement;
        }

        /*  Add to decimal total according to placement in binary string */
        else if( s[i] == '1' )
        {
            total += power_of( 2, placement );
            ++placement;
        }
    }

    /*  Return our total */
    return( total );
}


/*  Prints our mostly-finished number string */
void print_number_string( char *s )
{
    /*  If we want to print in sections */
    if( sections != 0 )
    {
        int i = 0;          //  Counter
        int spaces = 0;     //  Number of spaces 'to the right' we've moved

        /*  Find and print the number of zeroes we need to pad */
        for( i = 0; i < ( 4 - strlen(s) % 4 ); ++i )
        {
            printf("0");
            ++spaces;
        }

        /*  Print the string char by char, add spaces when necessary */
        for( i = 0; i < strlen(s); ++i )
        {
            if( spaces == 4 )
            {
                printf(" ");
                spaces = 0;
            }

            printf("%c", s[i]);
            ++spaces;
        }

        printf("\n");   //  Finish with a newline
    }
    else    //  Otherwise, just print the damn string
    {
        printf("%s\n", s);
    }
}


/*
 * Master output decider function thingy
 */
int string_printer( double *decimalResult )
{
    /*  Create a string pointer, allocate memory to it */
    char *s = NULL;
    s = malloc( MAX_STRING_LENGTH );
    if( s == NULL )
    {
        mem_error( "string_printer:  Cannot allocate memory for string" );
        return(1);
    }

    /*  Print the result */
    if( bin2dec == 1 )  //  Decimal
    {
        if( verbose == 1 )
            printf( "DEC\t");

        sprintf( s, "%s%0.lf", s, *decimalResult );
        print_number_string( s );
        memset( s, '\0', MAX_STRING_LENGTH );
    }

    if( bin2oct == 1 )  //  Octal
    {
        if( verbose == 1 )
            printf("OCT\t");

        sprintf( s, "%s%o", s, (unsigned int)*decimalResult );
        print_number_string( s );
        memset( s, '\0', MAX_STRING_LENGTH );
    }

    if( bin2hex == 1 )  //  Hex
    {
        if( verbose == 1 )
            printf("HEX\t");

        sprintf( s, "%s%x", s, (unsigned int)*decimalResult );

        print_number_string( s );
        memset( s, '\0', MAX_STRING_LENGTH );
    }

    if( hexCaps == 1 )  //  Hex (with caps)
    {
        if( verbose == 1 )
            printf("HEX\t");

        sprintf( s, "%s%X", s, (unsigned int)*decimalResult );
        print_number_string( s );
        memset( s, '\0', MAX_STRING_LENGTH );

    }

    if( bin2phex == 1 ) //  SUPER HEX
    {
        if( verbose == 1 )
            printf("0xHEX\t");

        sections = 0;   //  Need to do this, otherwise stuff gets ugly

        sprintf( s, "%s%a", s, *decimalResult );
        print_number_string( s );
        memset( s, '\0', MAX_STRING_LENGTH );
    }

    if( phexCaps == 1 ) //  SUPER HEX (with caps)
    {
        if( verbose == 1 )
            printf("0xHEX\t");

        sections = 0;   //  Need to do this, otherwise stuff gets ugly

        sprintf( s, "%s%A", s, *decimalResult );
        print_number_string( s );
        memset( s, '\0', MAX_STRING_LENGTH );

    }

    /*  free our string buffer */
    free( s );
    s = NULL;

    return(0);
}



/*
 * Here, we put the finishing touches on our line
 */
char* text_mode_printer_finish( char *line )
{

    if( lineSpacing == 0 && totalConversions > 1 )
        sprintf( line, "%s\t", line );
    else if( totalConversions > 1 )
        sprintf( line, "%s\n", line );

    return( line );
}



/*
 * Herein we compile a string of text to be printed, based on various factors
 * such as whether or not verbosity is switched on, which conversions were
 * called for, etc.
 */
void text_mode_printer( int ch, char *string )
{
    char *line = malloc( 32 );
    if( line == NULL )
    {
        mem_error("In:  text_mode_printer");
        exit(1);
    }

    if( verbose == 1 )
    {
        if( bin2bin == 1 )
        {
            sprintf( line, "%c  BIN    %s", ch, string );
            text_mode_printer_finish( line );
            printf("%s", line);

            memset( line, '\0', 32 );
        }

        if( bin2oct == 1 )
        {
            sprintf( line, "%c  OCT    %o", ch, ch );
            text_mode_printer_finish( line );
            printf("%s", line);

            memset( line, '\0', 32 );
        }

        if( bin2dec == 1 )
        {
            sprintf( line, "%c  DEC    %d", ch, ch );
            text_mode_printer_finish( line );
            printf("%s", line);

            memset( line, '\0', 32 );
        }

        if( bin2hex == 1 )
        {
            sprintf( line, "%c  HEX    %x", ch, ch );
            text_mode_printer_finish( line );
            printf("%s", line);

            memset( line, '\0', 32 );
        }

        if( hexCaps == 1 )
        {
            sprintf( line, "%c  HEX    %X", ch, ch );
            text_mode_printer_finish( line );
            printf("%s", line);

            memset( line, '\0', 32 );
        }
    }

    else
    {
        sprintf( line, "%c", ch );
        text_mode_printer_finish( line );
        printf("%s", line );
    }


    free( line );
    line = NULL;

}


/*
 * Convert a binary number (string) to a series of characters
 */
int number_to_string( char *string )
{
    int sLen = strlen( string );    //  Length of original string
    int ch = 0;                     //  Integer to receive character value
    int stringMoved = 0;            //  Number of times string was incremeneted
    int counter;                    //  Generic counter


    /*  Create our bin string */
    char *binStr = malloc( (BIN_STR_LENGTH) );
    if( binStr == NULL )
    {
        mem_error("In:  number_to_string");
        return(1);
    }

    /*  Null out binStr */
    memset( binStr, '\0', (BIN_STR_LENGTH) );


    /*
     * Go through the string, grabbing 8-bit (or fewer) chunks of it and
     * converting said 'chunks' to ASCII character values, printing each
     * along the way.
     */
    while( 1 )
    {
        /*  Check if we're only working with one character */
        if( sLen <= (BIN_STR_LENGTH) + 1)
        {
            ch = (int)binary_to_decimal( string );

            if( ch != 0 )
            {
                text_mode_printer( ch, string );

                if( lineSpacing == 1 && totalConversions < 2 )
                    printf("\n");
            }

            break;
        }
        else
        {
            for( counter = 0; counter <= (BIN_STR_LENGTH) ; ++counter )
            {
                binStr[counter] = string[counter];
            }

            string += counter;
            stringMoved += counter;
            sLen -= counter;


            ch = (int)binary_to_decimal( binStr );

            text_mode_printer( ch, binStr );
            memset( binStr, '\0', (BIN_STR_LENGTH) );

            if( lineSpacing == 1 )
                printf("\n");
            else if( verbose == 1 )
                printf("\t");
        }


    }

    if( lineSpacing == 0 )
        printf("\n");

    /*  Reset the original string */
    string -= stringMoved;

    /*  Free / null binStr */
    free( binStr );
    binStr = NULL;

    return(0);
}


int send_string_stdin(void)
{
    char *theNumber = NULL;     //  Used for strtok
    char line[256];
    double decimalResult = 0;
    int pCount = 0;

    while( fgets(line, 256, stdin) != NULL )
    {
        /*
         * We'll use strtok to tokenize the string, number by number, using the
         * space character as a delimiter
         */
        theNumber = strtok(line, " ");
        while( theNumber != NULL )
        {
            /*  If the user wants slightly prettier output */
            if( lineSpacing == 1 && pCount > 0 && textMode == 0)
            {
                printf("\n");
            }

            if( textMode == 1 )
                number_to_string( theNumber );
            else
            {
                decimalResult = binary_to_decimal( theNumber );
                string_printer( &decimalResult );
            }

            theNumber = strtok(NULL, " ");  //  Re-run strtok, grab next number

            ++pCount;
        }
    }

    return(0);
}


/*
 *  ==========================================================================
 *                              MAIN FUNCTION
 *  ==========================================================================
 */
int main( int argc, char *argv[] )
{

    if( argc > 1 )
    {
        /*  If they want help */
        if( strcmp( "-h", argv[1]) == 0 || strcmp( "--help", argv[1]) == 0 )
        {
            print_help();
            return(0);
        }

        /*  If they want version / author info */
        if( strcmp( "--version", argv[1]) == 0 )
        {
            print_version();
            return(0);
        }
    }

    /*  Initialize variables */
    bin2bin = 0;
    bin2dec = 0;
    bin2hex = 0;
    bin2phex = 0;
    bin2oct = 0;
    sections = 0;
    hexCaps = 0;
    phexCaps = 0;
    verbose = 0;

    /*  Get options */
    int opt = getopt( argc, argv, optString );
    while ( opt != -1 )
    {
        switch( opt )
        {
            case 'h':   //  Help
                print_help();
                return(0);
                break;
            case 'b':   //  Binary
                bin2bin = 1;
                break;
            case 'd':   //  Decimal
                bin2dec = 1;
                break;
            case 'v':   //  Verbosity
                verbose = 1;
                break;
            case 'x':   //  Hexadecimal
                bin2hex = 1;
                break;
            case 'X':   //  Hexadecimal with caps
                hexCaps = 1;
                break;
            case 'a':   //  Extra precise hexadecimal
                bin2phex = 1;
                break;
            case 'A':   //  Same, but also with caps
                phexCaps = 1;
                break;
            case 'o':   //  Octal
                bin2oct = 1;
                break;
            case 's':   //  Use sections
                sections = 4;
                break;
            case 'l':   //  Print newlines between sections of output
                lineSpacing = 1;
                break;
            case 't':
                textMode = 1;
                break;
            default:
                //  Do nuttin'
                break;
        }

        opt = getopt( argc, argv, optString );
    }


    /*  Shift / adjust argc and argv */
    argv += ( optind - 1 );
    argc -= ( optind - 1 );

    /*  With no args, we default to decimal */
    if( bin2dec == 0 && bin2hex == 0 && bin2phex == 0 && bin2oct == 0 &&
            hexCaps == 0 && phexCaps == 0 && bin2bin == 0 )
    {
        bin2dec = 1;
    }

    /*  Set the total number of conversions */
    totalConversions = ( bin2dec + bin2hex + bin2phex + bin2oct +
            hexCaps + phexCaps + bin2bin );


    /*  If they're reading from stdin */
    if( argc == 1 || (strcmp( "-", argv[1] ) == 0) )
    {
        /*  Grab string */
        if( send_string_stdin() == 1 )
            return(1);
        else
            return(0);
    }

    int pCount = 0;     //  Random counter!
    while( argc > 1 )
    {
        if( textMode == 1 )
        {
            number_to_string( argv[1] );
            --argc;
            ++argv;
            ++pCount;
            continue;
        }

        /*  Check to make sure it's a binary number */
        if( check_for_binary( argv[1] ) != 0 )
        {
            /*  If it isn't, tell them but don't kill the program */
            fprintf(stderr, "WARNING:  Not a binary number:\t%s\n", argv[1] );
            --argc;
            ++argv;
            ++pCount;
            continue;
        }

        /*  Get our decimal number from the binary to decimal converter */
        double decimalResult = binary_to_decimal( argv[1] );

        /*  If the user wants prettier output, give it to them */
        if( lineSpacing == 1 && pCount > 0 )
            printf("\n");
        
        /*  Handle binary output */
        if( bin2bin == 1 )
        {
            if( verbose == 1 )
                printf( "BIN\t" );

            print_number_string( argv[1] );
        }

        /*  Print that sumbitch out */
        if( string_printer( &decimalResult ) == 1 )
            return(1);

        /*  Increment or decrement our various lists and counters */
        --argc;
        ++argv;
        ++pCount;
    }

    return(0);
}
