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
// ARGO_VALUE *argo_read_value(FILE *f) {
//     // TO BE IMPLEMENTED.
//     abort();
// }

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
// int argo_read_string(ARGO_STRING *s, FILE *f) {
//     // TO BE IMPLEMENTED.
//     abort();
// }

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

// int argo_read_number(ARGO_NUMBER *n, FILE *f) {
//     // TO BE IMPLEMENTED.
//     abort();
// }

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
    putSpaces(f);
    argo_write_value(o->member_list, f);
    // fputc( ARGO_COMMA,f);
    ARGO_VALUE *currentMember = o->member_list->next;
    // while(!argo_str_equal(&currentMember->name,&o->member_list->name)){
    while(currentMember->next != o->member_list){
        argo_write_value(currentMember, f);
        currentMember = currentMember->next;
        fputc( ARGO_COMMA,f);
        putSpaces(f);
    }
    argo_write_value(currentMember, f);
    // indent_level -= global_options & 0x000000FF;
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
    return 0;
}

int argo_write_array(ARGO_ARRAY *a, FILE *f){
    fputc(ARGO_LBRACK,f);
    putSpaces(f);
    argo_write_value(a->element_list, f);
    // fputc( ARGO_COMMA,f);
    ARGO_VALUE *currentMember = a->element_list->next;
    // while(!argo_str_equal(&currentMember->name,&o->member_list->name)){
    while(currentMember->next != a->element_list){
        argo_write_value(currentMember, f);
        currentMember = currentMember->next;
        fputc( ARGO_COMMA,f);
    putSpaces(f);
    }
    argo_write_value(currentMember, f);
    // indent_level -= global_options & 0x000000FF;
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
    return 0;
}


int argo_write_value(ARGO_VALUE *v, FILE *f) { //TODO
    int n;
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
        // printf("PRINTING NUMBER");
        return argo_write_number(&v->content.number, f);
        break;

    case 3: //STRING CASE
        // printf("PRINTING STRING");
        fputc(ARGO_QUOTE,f);
        int x = argo_write_string(&v->content.string, f);
        fputc(ARGO_QUOTE,f);
        return x+n;
        break;
    
    case 4: //OBJECT CASE
        // printf("PRINTING OBJECT");
        // indent_level += global_options & 0x000000FF;
        indent_level++;
        return argo_write_object(&v->content.object,f)+n;
        break;
    
    case 5: //ARRAY CASE
        // printf("PRINTING ARRAY");
        // indent_level += global_options & 0x000000FF;
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
    int x;
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
                // fprintf(f, "\\u%4.4x", *(s->content+i));  //TODO
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
static double strToFloat(ARGO_STRING *s){
    double val = 0;
    if(s->length==0){
        return 0;
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
            return 0;
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
        for(int i = exponentStartPos; i<s->length; i++){
            if(argo_is_digit(*(s->content+i))){
                exponent+=*(s->content+i) -48;
                exponent*=10;
            }
            else{
                return 0;
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
int argo_write_number(ARGO_NUMBER *n, FILE *f) {
    // TO BE IMPLEMENTED.
    if(n->valid_int && n->valid_float && n->int_value != n->float_value){
        return -1;
    }
    if(n->valid_int && n->valid_string ){
        if(n->int_value != strToFloat(&n->string_value)){
            printf("String(%.16f)didnt match int(%ld)",strToFloat(&n->string_value), n->int_value);
            return -1;
            }
    }
    if(n->valid_string && n->valid_float ){
        if(strToFloat(&n->string_value) > (n->float_value+0.000000000000001) || strToFloat(&n->string_value ) < (n->float_value-0.000000000000001)){
             printf("String(%.16f)didnt match float(%.16f)",strToFloat(&n->string_value), n->float_value);
            return -1;
            }
    }
    if(n->valid_int){
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
