#include <stdlib.h>
#include <stdio.h>

#include "argo.h"
#include "global.h"
#include "debug.h"

/**
 * @brief  Read JSON input from a specified input stream, parse it,
 * and return a data structure representing the corresponding value.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON value,
 * according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.  See the assignment handout for
 * information on the JSON syntax standard and how parsing can be
 * accomplished.  As discussed in the assignment handout, the returned
 * pointer must be to one of the elements of the argo_value_storage
 * array that is defined in the const.h header file.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  A valid pointer if the operation is completely successful,
 * NULL if there is any error.
 */
char getNoWhitespace(FILE *f){
    char c = fgetc(f);
    while(argo_is_whitespace(c)){
        c = fgetc(f);
    }
    if(c == EOF)
        return -1;
    return c;
}

int argo_read_object(ARGO_OBJECT *o, FILE *f){
    char c = getNoWhitespace(f);
    ARGO_VALUE *prev = NULL;
    ARGO_VALUE *head = NULL;
    ARGO_VALUE *v;
    while(c != ARGO_RBRACE){
        if(c!= ARGO_QUOTE)
            return -1;
        ARGO_STRING name;
        int success = argo_read_string(&name, f);
        if(success)
            return -1;
        c=getNoWhitespace(f);
        v=argo_read_value(f);
        if(!v)
            return -1;
        v->name = name;
        if(!head){
            head = v;
        }
        else{
                v->prev = prev;
                prev->next = v;
        }
         prev = v;

        c=fgetc(f); // get rid of comma
        if(c==1){
            c=fgetc(f); // get rid of comma
            ungetc(c,f); // get rid of comma
        }
        if(c!= ARGO_RBRACE)
            c=getNoWhitespace(f);
        if(c==ARGO_COMMA){
            c=fgetc(f); // get rid of comma
        }
        if(c==EOF)
            return -1;
    }
    if(head){
        
        (argo_value_storage+argo_next_value)->type = ARGO_NO_TYPE;
        (argo_value_storage+argo_next_value)->next = head;
        (argo_value_storage+argo_next_value)->prev = v;

        v->next = (argo_value_storage+argo_next_value);
        head->prev = (argo_value_storage+argo_next_value);
        o->member_list =(argo_value_storage+argo_next_value);
        argo_next_value++;
    }
    else
        o->member_list = NULL;
    return 0;
}
int argo_read_array(ARGO_ARRAY *a, FILE *f){
    char c = getNoWhitespace(f);
    ARGO_VALUE *prev = NULL;
    ARGO_VALUE *head = NULL;
    ARGO_VALUE *v;
    int success;
    while(c != ARGO_RBRACK){
        ungetc(c,f);
        v=argo_read_value(f);
        if(!v)
            return -1;
        if(!head){
            head = v;
        }
        else{
                v->prev = prev;
                prev->next = v;
        }
         prev = v;
        
        c=fgetc(f); // get rid of comma
        
        if(c==1){
            c=fgetc(f); // get rid of comma
            ungetc(c,f); // get rid of comma
        }
        if(c!= ARGO_RBRACK)
            c=getNoWhitespace(f);
        if(c==ARGO_COMMA){
            c=fgetc(f); // get rid of comma
        }
        if(c==EOF)
            return -1;
    }
    if(head){
        // v->next = head;
        // head->prev = v;
        // a->element_list = head;
        (argo_value_storage+argo_next_value)->type = ARGO_NO_TYPE;
        (argo_value_storage+argo_next_value)->next = head;
        (argo_value_storage+argo_next_value)->prev = v;

        v->next = (argo_value_storage+argo_next_value);
        head->prev = (argo_value_storage+argo_next_value);
        a->element_list =(argo_value_storage+argo_next_value);
        argo_next_value++;
    }
    else
        a->element_list = NULL;
    return 0;
}

ARGO_VALUE *argo_read_value(FILE *f) {
    ARGO_VALUE v;
    v.name.content = NULL;
    int success=-1;
    char c = getNoWhitespace(f);
    switch(c){
        case ARGO_T: 
            for(int i =0; i < 4;i++){
                if(c!=*(ARGO_TRUE_TOKEN+i))
                    return NULL;
                c = fgetc(f);
            
            }
            ungetc(c,f);
            v.type = ARGO_BASIC_TYPE;
            v.content.basic=ARGO_TRUE;
            success = 0;
            break;
        case ARGO_F: 
            for(int i =0; i < 5;i++){
                if(c!=*(ARGO_FALSE_TOKEN+i))
                    return NULL;
                c = fgetc(f);
            }
            ungetc(c,f);
            v.type = ARGO_BASIC_TYPE;
            v.content.basic=ARGO_FALSE;
            success = 0;
            break;
        case ARGO_N: 
            for(int i =0; i < 4;i++){
                if(c!=*(ARGO_NULL_TOKEN+i))
                    return NULL;
                c = fgetc(f);
            }
            ungetc(c,f);
            v.type = ARGO_BASIC_TYPE;
            v.content.basic=ARGO_NULL;
            success = 0;
            break;
       
        case ARGO_QUOTE:
             v.type = ARGO_STRING_TYPE;
            success = argo_read_string(&v.content.string, f);
            break;
        case ARGO_LBRACK: //ARRAY CASE
            v.type = ARGO_ARRAY_TYPE;
            success = argo_read_array(&v.content.array,f);
            break;
        case ARGO_LBRACE: //OBJECT CASE
            v.type = ARGO_OBJECT_TYPE;
            success = argo_read_object(&v.content.object,f);
            break;
        default:
            if(argo_is_digit(c) || c == ARGO_MINUS){
                v.type = ARGO_NUMBER_TYPE;
                ungetc(c,f);
                success = argo_read_number(&v.content.number, f);
                break;
            }
            else
                return NULL;
    }
    if(!success){
        ARGO_VALUE *a = (argo_value_storage+argo_next_value);
        *a = v;
        argo_next_value++;
        return a;
    }
    else
        return NULL;
}

/**
 * @brief  Read JSON input from a specified input stream, attempt to
 * parse it as a JSON string literal, and return a data structure
 * representing the corresponding string.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON string
 * literal, according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */

int argo_read_string(ARGO_STRING *s, FILE *f) {
    char c = fgetc(f);
    ungetc(c,f);
    c=fgetc(f);
    s->length=0;
    s->capacity=0;
    if(c == EOF)
        return -1;
    while(c != ARGO_QUOTE){
        if(c!= ARGO_BSLASH)
            argo_append_char(s,c);
        else{
            char c2 = fgetc(f);
            if(c2 == EOF)
                return -1;
            int val = 0;
            char h;
            switch (c2)
            {
            case ARGO_B:
                argo_append_char(s,ARGO_BS);
                break;
            case ARGO_F:
                argo_append_char(s,ARGO_FF);
                break;
            case ARGO_N:
                argo_append_char(s,ARGO_LF);
                break;
            case ARGO_R:
                argo_append_char(s,ARGO_CR);
                break;
            case ARGO_T:
                argo_append_char(s,ARGO_HT);
                break;
            case ARGO_BSLASH:
                argo_append_char(s,ARGO_BSLASH);
                break;
            case ARGO_FSLASH:
                argo_append_char(s,ARGO_FSLASH);
                break;
            case ARGO_QUOTE:
                argo_append_char(s,ARGO_QUOTE);
                break;
            case ARGO_U:
                for(int i = 0; i< 4; i++){
                    val*=16;
                    h = fgetc(f);
                    if(h == EOF)
                        return -1;
                    if(argo_is_digit(h)){
                        val+= h-48;
                    }
                    else {
                        switch (h)
                        {
                        case 'A': case 'a':
                            val+=10;
                            break;
                        case 'B': case 'b':
                            val+=11;
                            break;
                        case 'C': case 'c':
                            val+=12;
                            break;
                        case 'D': case 'd':
                            val+=13;
                            break;
                        case 'E': case 'e':
                            val+=14;
                            break;
                        case 'F': case 'f':
                            val+=15;
                            break;
                        default:
                            return -1;
                            break;
                        }
                    }
                } 
                argo_append_char(s,val);
                break;
            default:
                return -1;
                break;
            }
        }
        c = fgetc(f); 
        if(c == EOF)
            return -1;
    }
    return 0;
}

/**
 * @brief  Read JSON input from a specified input stream, attempt to
 * parse it as a JSON number, and return a data structure representing
 * the corresponding number.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON numeric
 * literal, according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.  The returned value must contain
 * (1) a string consisting of the actual sequence of characters read from
 * the input stream; (2) a floating point representation of the corresponding
 * value; and (3) an integer representation of the corresponding value,
 * in case the input literal did not contain any fraction or exponent parts.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */


int argo_read_number(ARGO_NUMBER *n, FILE *f) {
    char c = fgetc(f);
    n->string_value.length=0;
    n->string_value.capacity=0;
    if(c == EOF)
        return -1;
    while((argo_is_digit(c) || argo_is_exponent(c) || c == ARGO_MINUS || c == ARGO_PLUS || c == ARGO_PERIOD)){
        argo_append_char(&n->string_value, c);
        if(c == ARGO_PERIOD)
            n->valid_float=1;
        c=fgetc(f);
    }
    double val = 0;
    if(n->string_value.length==0){
        return -1;
    }
    int numberStartPos =0;
    int numberStartPos2 =0;
    if(*(n->string_value.content) == ARGO_MINUS){
        numberStartPos =1;
    }
    else if(*(n->string_value.content) == ARGO_PLUS)
        numberStartPos2 = 1;
    int exponentExists = 0;
    int exponentStartPos;
    int decimalExists = 0;
    int decimalStartPos;
    for(int i = numberStartPos+numberStartPos2; i<n->string_value.length; i++){
        if( *(n->string_value.content+i) >=48 && *(n->string_value.content+i) <= 57){
            if(!decimalExists){
                val+=*(n->string_value.content+i) -48;
                val*=10;
            }
            else{
                double dec = (*(n->string_value.content+i) -48);
                
                for(int j =0; j<(i-decimalStartPos);j++){ 
                    dec/=10;
                }
                val+=dec;
            }
        }
        else if(argo_is_exponent(*(n->string_value.content + i))){
            exponentExists=1;
            exponentStartPos=i+1;
            break;
        }
        else if(*(n->string_value.content+i)==ARGO_PERIOD){
            decimalExists=1;
            decimalStartPos=i;
            val/=10;
        }
        else{
            return -1;
        }
    }
    if(!decimalExists){
        val/=10;
    }
    int exponent=0;
    if(exponentExists){
        int exponentSign = 1;
        if(*(n->string_value.content+exponentStartPos)==ARGO_MINUS){
            exponentSign=-1;
            exponentStartPos++;
        }
        else if(*(n->string_value.content+exponentStartPos)==ARGO_PLUS)
            exponentStartPos++;
        for(int i = exponentStartPos; i<n->string_value.length; i++){
            if(argo_is_digit(*(n->string_value.content+i))){
                exponent+=*(n->string_value.content+i) -48;
                exponent*=10;
            }
            else{
                return -1;
            }
        }
        exponent/=10;
        exponent*=exponentSign;
    }
    if(exponent>0){
        for(int i=0; i<exponent;i++){
            val*=10;
        }
    }
    else{
        for(int i=exponent; i< 0;i++){
            val/=10;
        }
    }
    if(numberStartPos)
        val*=-1;
    ungetc(1,f);
    n->valid_string=1;
    int a = val;
    if(val!=a){
        n->valid_float = 1;
        n->valid_int = 0;
        n->int_value = 0;
        n->float_value = val;

    }
    else{
        n->valid_int = 1;
        n->valid_float = 1;
        n->int_value = val;
        n->float_value=n->int_value;
    }
    return 0;
}

/**
 * @brief  Write canonical JSON representing a specified value to
 * a specified output stream.
 * @details  Write canonical JSON representing a specified value
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.
 *
 * @param v  Data structure representing a value.
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */
static int argo_str_equal(ARGO_STRING *s1, ARGO_STRING *s2){
    if(s1->length != s2->length){
        return 0;
    }
    for(int i =0; i<s1->length; i++){
        if(*(s1->content+i) != *(s2->content+i)){
            return 0;
        }
    }
    return 1;
}
static int putSpaces(FILE* f){
    if(!indent_level){
        // indent_level = global_options & 0x000000FF;
        indent_level=1;
        if(!indent_level)
            indent_level = -1;
    }
    if(indent_level>0){
        fputc(ARGO_LF,f);
        for(int i =0; i< indent_level*(global_options & 0x000000FF); i++){
            fputc(ARGO_SPACE,f);
        }
    }
    return 0;
}

int argo_write_object(ARGO_OBJECT *o, FILE *f){
    fputc(ARGO_LBRACE,f);
    if(global_options & 0x000000FF)
        putSpaces(f);
    argo_write_value(o->member_list->next, f);
    if(o->member_list->next->next != o->member_list){
        fputc( ARGO_COMMA,f);
        if(global_options & 0x000000FF)
            putSpaces(f);
    }
    ARGO_VALUE *currentMember = o->member_list;
    currentMember = currentMember->next;
    currentMember = currentMember->next;
    // while(!argo_str_equal(&currentMember->name,&o->member_list->name)){
    if(currentMember != o->member_list)
        while(currentMember->next != o->member_list){
            argo_write_value(currentMember, f);
            currentMember = currentMember->next;
            fputc( ARGO_COMMA,f);
            if(global_options & 0x000000FF)
                putSpaces(f);
        }
    if(o->member_list->next->next != o->member_list)
        argo_write_value(currentMember, f);
    // indent_level -= global_options & 0x000000FF;
    if(global_options & 0x000000FF){
        indent_level--;
        if(indent_level){
            putSpaces(f);
            fputc( ARGO_RBRACE,f);
        }
        else{
            fputc(ARGO_LF,f);
            fputc( ARGO_RBRACE,f);
            fputc(ARGO_LF,f);
            
        }
    }
    else{
        fputc( ARGO_RBRACE,f);
    }
    return 0;
}

int argo_write_array(ARGO_ARRAY *a, FILE *f){
    fputc(ARGO_LBRACK,f);
    if(global_options & 0x000000FF)
        putSpaces(f);
    argo_write_value(a->element_list->next, f);
    if(a->element_list->next->next != a->element_list){
        fputc( ARGO_COMMA,f);
        if(global_options & 0x000000FF)
            putSpaces(f);
    }
    ARGO_VALUE *currentMember = a->element_list;
    currentMember = currentMember->next;
    currentMember = currentMember->next;
    // while(!argo_str_equal(&currentMember->name,&o->member_list->name)){
    if(currentMember != a->element_list)
        while(currentMember->next != a->element_list){
            argo_write_value(currentMember, f);
            currentMember = currentMember->next;
            fputc( ARGO_COMMA,f);
            if(global_options & 0x000000FF)
                putSpaces(f);
        }
    if(a->element_list->next->next != a->element_list)
        argo_write_value(currentMember, f);
    // indent_level -= global_options & 0x000000FF;
    if(global_options & 0x000000FF){
        indent_level--;
        if(indent_level){
            putSpaces(f);
            fputc( ARGO_RBRACK,f);
        }
        else{
            fputc(ARGO_LF,f);
            fputc( ARGO_RBRACK,f);
            fputc(ARGO_LF,f);
        }
    }
    else{
        fputc( ARGO_RBRACK,f);
    }
    return 0;
}


int argo_write_value(ARGO_VALUE *v, FILE *f) { //TODO
    int n=0;
    if(v->name.content){
        fputc(ARGO_QUOTE,f);
        n=argo_write_string(&v->name,f);
        fputc(ARGO_QUOTE, f);
        fputc(ARGO_COLON,f);
        if(indent_level>0)
            fputc(ARGO_SPACE,f);
    }
    switch (v->type)
    {
    case 1: //BASIC CASE
        if(v->content.basic ==ARGO_NULL){
            fputs( ARGO_NULL_TOKEN,f);
        }
        else if(v->content.basic==ARGO_TRUE){
            fputs( ARGO_TRUE_TOKEN,f);
        }
        else if(v->content.basic == ARGO_FALSE){
            fputs(ARGO_FALSE_TOKEN,f);
        }
        else{
            return -1;
        }
        return n;
        break;

    case 2: //NUMBER CASE
        return argo_write_number(&v->content.number, f);
        break;

    case 3: //STRING CASE
        fputc(ARGO_QUOTE,f);
        int x = argo_write_string(&v->content.string, f);
        fputc(ARGO_QUOTE,f);
        return x+n;
        break;
    
    case 4: //OBJECT CASE
        indent_level++;
        return argo_write_object(&v->content.object,f)+n;
        break;
    
    case 5: //ARRAY CASE
        indent_level++;
        return argo_write_array(&v->content.array, f)+n;
    default:
        break;
    }
    return -1;
}

/**
 * @brief  Write canonical JSON representing a specified string
 * to a specified output stream.
 * @details  Write canonical JSON representing a specified string
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.  The argument string may contain any sequence of
 * Unicode code points and the output is a JSON string literal,
 * represented using only 8-bit bytes.  Therefore, any Unicode code
 * with a value greater than or equal to U+00FF cannot appear directly
 * in the output and must be represented by an escape sequence.
 * There are other requirements on the use of escape sequences;
 * see the assignment handout for details.
 *
 * @param v  Data structure representing a string (a sequence of
 * Unicode code points).
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */
int printHex(int val, FILE* f){
     if(val >= 0 && val <= 9)
            fputc(val+48, f);
    else if(val>=10 && val<=15){
        fputc(val+87,f);
    }
    else{
        return -1;
    }
    return 0;
}
int fputHex(char c, FILE* f){
        int val = (c & 0xF000) >> 3;
        int x = printHex(val,f);
        val = (c & 0x0F00) >> 2;
        x+=printHex(val,f);
        val = (c & 0x00F0) >> 1;
        x+=printHex(val,f);
        val = (c & 0x000F);
        x+=printHex(val,f);
        return x;
}
int argo_write_string(ARGO_STRING *s, FILE *f) {
    // TO BE IMPLEMENTED.
    int x=0;
    for(int i = 0; i < s->length; i++){
        switch (*(s->content+i))
        {
        case 0:
            break;
        case ARGO_QUOTE:
            fputs( "\\\"",f);
            break;
        case ARGO_FSLASH:
            fputs("/",f);
            break;
        case ARGO_BSLASH:
            fputs("\\\\",f);
            break;
        case ARGO_BS:
            fputs( "\\b",f);
            break;
        case ARGO_FF:
            fputs( "\\f",f);
            break;
        case ARGO_LF:
            fputs( "\\n",f);
            break; 
        case ARGO_CR:
            fputs( "\\r",f);
            break;
        case ARGO_HT:
            fputs( "\\t",f);
            break;
        default:
            if(*(s->content+i) > 0x001F && *(s->content+i)<0x00FF){
                fputc(*(s->content+i), f);
            }
            else{
                fputs("\\u",f);
                x=fputHex(*(s->content+i),f);
            }
        }
    }
    return x;
}

/**
 * @brief  Write canonical JSON representing a specified number
 * to a specified output stream.
 * @details  Write canonical JSON representing a specified number
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.  The argument number may contain representations
 * of the number as any or all of: string conforming to the
 * specification for a JSON number (but not necessarily canonical),
 * integer value, or floating point value.  This function should
 * be able to work properly regardless of which subset of these
 * representations is present.
 *
 * @param v  Data structure representing a number.
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */
static double strToFloat(ARGO_STRING *s){
    double val = 0;
    if(s->length==0){
        return -1;
    }
    int numberStartPos =0;
    if(*(s->content) == ARGO_MINUS){
        numberStartPos =1;
    }
    int exponentExists = 0;
    int exponentStartPos;
    int decimalExists = 0;
    int decimalStartPos;
    for(int i = numberStartPos; i<s->length; i++){
        if( *(s->content+i) >=48 && *(s->content+i) <= 57){
            if(!decimalExists){
                val+=*(s->content+i) -48;
                val*=10;
            }
            else{
                double dec = (*(s->content+i) -48);
                
                for(int j =0; j<(i-decimalStartPos);j++){ 
                    dec/=10;
                }
                val+=dec;
            }
        }
        else if(argo_is_exponent(*(s->content + i))){
            exponentExists=1;
            exponentStartPos=i+1;
            break;
        }
        else if(*(s->content+i)==ARGO_PERIOD){
            decimalExists=1;
            decimalStartPos=i;
            val/=10;
        }
        else{
            return -1;
        }
    }
    if(!decimalExists){
        val/=10;
    }
    int exponent=0;
    if(exponentExists){
        int exponentSign = 1;
        if(*(s->content+exponentStartPos)==ARGO_MINUS){
            exponentSign=-1;
            exponentStartPos++;
        }
        else if(*(s->content+exponentStartPos)==ARGO_PLUS)
            exponentStartPos++;
        for(int i = exponentStartPos; i<s->length; i++){
            if(argo_is_digit(*(s->content+i))){
                exponent+=*(s->content+i) -48;
                exponent*=10;
            }
            else{
                return -1;
            }
        }
        exponent/=10;
        exponent*=exponentSign;
    }
    if(exponent>0){
        for(int i=0; i<exponent;i++){
            val*=10;
        }
    }
    else{
        for(int i=exponent; i< 0;i++){
            val/=10;
        }
    }
    if(numberStartPos)
        return val*-1;
    else
        return val;
}
static int printFloat(double d,FILE *f){
    if(d){
        if(d < 0){
            fputc(ARGO_MINUS,f);
            d/=-1;
        }
        int exponent = 0; 
        while(d >= 1){
            d /=10;
            exponent+=1;
        }
        while(d < 0.1){
            d*=10;
            exponent -=1;
        }
        // d-=0.000000000001;
        fprintf(f,"%.16f",d);          //TODO
        if(exponent!=0){
            fputc(ARGO_E,f);
            fprintf(f,"%d",exponent);
        }
        return 0;
    }
    else{
        fputc('0', f);
        return 0;
    }
}

int argo_write_number(ARGO_NUMBER *n, FILE *f) {
    // TO BE IMPLEMENTED.
    if(n->valid_int && n->valid_float && n->int_value != n->float_value){
        return -1;
    }
    if(n->valid_int && n->valid_string ){
        if(n->int_value != strToFloat(&n->string_value)){
            return -1;
            }
    }
    if(n->valid_string && n->valid_float ){
        if(strToFloat(&n->string_value) > (n->float_value+0.000000000000001) || strToFloat(&n->string_value ) < (n->float_value-0.000000000000001)){
            return -1;
            }
    }
    if(n->valid_float && !n->valid_int && n->float_value==0){
        fputs("0.0",f);
        return 0;
    }
    else if(n->valid_int){
        fprintf(f, "%ld",n->int_value);
        return 0;
    }
    else if(n->valid_float){
        return printFloat(n->float_value, f);;
    }
    else if(n->valid_string){
        return printFloat(strToFloat(&n->string_value),f);
        
    }
    return -1;
}
