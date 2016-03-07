#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "error_hdr.h"

#include "rdconfig.h"

int get_config_string(const char *pInFileName,char *pInSectionName,char *pInKeyName,char *pOutKeyValue)
{
    FILE *fpConfig;
    char szBuffer[150];
    char *pStr1,*pStr2;
    int iRetCode = 0;

    /*test*/
    /*
    printf("pInFileName: %s !\n",pInFileName);
    printf("pInSectionName: %s !\n",pInSectionName);
    printf("pInKeyName: %s !\n",pInKeyName);
    */

    memset(szBuffer,0,sizeof(szBuffer));
    if( NULL==( fpConfig=fopen(pInFileName,"r") ) )
        return FILENAME_NOTEXIST;

    while( !feof(fpConfig) )
    {
        if( NULL==fgets(szBuffer,150,fpConfig) )
            break;
        pStr1 = szBuffer ;
        while( (' '==*pStr1) || ('\t'==*pStr1) )
            pStr1++;
        if( '['==*pStr1 )
        {
            pStr1++;
            while( (' '==*pStr1) || ('\t'==*pStr1) )
                pStr1++;
            pStr2 = pStr1;
            while( (']'!=*pStr1) && ('\0'!=*pStr1) )
                pStr1++;
            if( '\0'==*pStr1)
                continue;
            while( ' '==*(pStr1-1) )
                pStr1--;
            *pStr1 = '\0';

            iRetCode = compare_string(pStr2,pInSectionName);
            if( !iRetCode )/*检查节名*/
            {
                iRetCode = get_key_value(fpConfig,pInKeyName,pOutKeyValue);
                fclose(fpConfig);
                return iRetCode;
            }
        }
    }

    fclose(fpConfig);
    return SECTIONNAME_NOTEXIST;

}

/*区分大小写*/
int compare_string(const char *pInStr1,const char *pInStr2)
{
    if( strlen(pInStr1)!=strlen(pInStr2) )
    {
        return STRING_LENNOTEQUAL;
    }

    /*while( toupper(*pInStr1)==toupper(*pInStr2) )*//*#include <ctype.h>*/
    while( *pInStr1==*pInStr2 )
    {
        if( '\0'==*pInStr1 )
            break;
        pInStr1++;
        pInStr2++;
    }

    if( '\0'==*pInStr1 )
        return STRING_EQUAL;

    return STRING_NOTEQUAL;

}

int get_key_value(FILE *fpConfig,char *pInKeyName,char *pOutKeyValue)
{
    char szBuffer[150];
    char *pStr1,*pStr2,*pStr3;
    unsigned int uiLen;
    int iRetCode = 0;

    memset(szBuffer,0,sizeof(szBuffer));
    while( !feof(fpConfig) )
    {
        if( NULL==fgets(szBuffer,150,fpConfig) )
            break;
        pStr1 = szBuffer;
        while( (' '==*pStr1) || ('\t'==*pStr1) )
            pStr1++;
        if( '#'==*pStr1 )
            continue;
        if( ('/'==*pStr1)&&('/'==*(pStr1+1)) )
            continue;
        if( ('\0'==*pStr1)||(0x0d==*pStr1)||(0x0a==*pStr1) )
            continue;
        if( '['==*pStr1 )
        {
            pStr2 = pStr1;
            while( (']'!=*pStr1)&&('\0'!=*pStr1) )
                pStr1++;
            if( ']'==*pStr1 )
                break;
            pStr1 = pStr2;
        }
        pStr2 = pStr1;
        while( ('='!=*pStr1)&&('\0'!=*pStr1) )
            pStr1++;
        if( '\0'==*pStr1 )
            continue;
        pStr3 = pStr1+1;
        if( pStr2==pStr1 )
            continue;
        *pStr1 = '\0';
        pStr1--;
        while( (' '==*pStr1)||('\t'==*pStr1) )
        {
            *pStr1 = '\0';
            pStr1--;
        }

        iRetCode = compare_string(pStr2,pInKeyName);
        if( !iRetCode )/*检查键名*/
        {
            pStr1 = pStr3;
            while( (' '==*pStr1)||('\t'==*pStr1) )
                pStr1++;
            pStr3 = pStr1;
            while( ('\0'!=*pStr1)&&(0x0d!=*pStr1)&&(0x0a!=*pStr1) )
            {
                if( ('/'==*pStr1)&&('/'==*(pStr1+1)) )
                    break;
                pStr1++;
            }
            *pStr1 = '\0';
            uiLen = strlen(pStr3);
            memcpy(pOutKeyValue,pStr3,uiLen);
            *(pOutKeyValue+uiLen) = '\0';
            return SUCCESS;
        }
    }

    return KEYNAME_NOTEXIST;
}

int get_config_int(const char *pInFileName,char *pInSectionName,char *pInKeyName,int *pOutKeyValue)
{
    int iRetCode = 0;
    char szKeyValue[16],*pStr;

    memset(szKeyValue,0,sizeof(szKeyValue));
    iRetCode = get_config_string(pInFileName,pInSectionName,pInKeyName,szKeyValue);
    if( iRetCode )
        return iRetCode;
    pStr    = szKeyValue;
    while( (' '==*pStr)||('\t'==*pStr))
        pStr++;
    if( ('0'==*pStr)&&( ('x'==*(pStr+1))||('X'==*(pStr+1)) ) )
        sscanf(pStr+2,"%x",pOutKeyValue);
    else
        sscanf(pStr,"%d",pOutKeyValue);

    return SUCCESS;

}
