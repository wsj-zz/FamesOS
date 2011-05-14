/************************************************************************************************
** 文件: tool.c
** 说明: 辅助函数,变量等
** 作者: Jun
** 时间: 2010-7-12
************************************************************************************************/
#define  FAMES_TOOL_C
#include "includes.h"


/*----------------------------------------------------------------------------------------------
 * 将一个INT8转化为字符串
 *
 * 支持下列选项:
 *  CHG_OPT_RAW      不转换, 直接将字节加到目标字符串中
 *  CHG_OPT_DEC      将数字转换为十进制串(否则为十六进制串)
 *  CHG_OPT_SIG      符号标志
 *  CHG_OPT_FIL      是否指定宽度
 *  CHG_OPT_ZER      如果指定宽度, 高位是否填存为0(反之填存为空格)
 *  CHG_OPT_LFT      如果指定宽度, 是否左对齐
 *  CHG_OPT_FRC      是否转为实数串(有小数点)
 *  CHG_OPT_END      是否要为字符串加上结束符
 *
 * 当指定为实数时, 低8位分为了两部分, 其高4位代表整个长度, 低4位代表小数级数(i.e. 3代表0.001)
 *                 小数级数最大为3
**--------------------------------------------------------------------------------------------*/
INT08S * INT08toSTR(INT08S * dst, INT08S src, INT16U opt)
{
    INT16S i, j;
    INT16S length, reallen;
    INT16S frclen, sign;
    INT08U usrc;
    INT08S tempbuf[8];
    #define MAX_FRC_LEN  3
    #define MAX_STR_LEN  6
    #define HEX_STR_LEN  2    /* 十六进制字符串长度 */

    FamesAssert(dst);

    if(!dst){ /* dst must't be NULL */
        return NULL;
    }

    length = opt & 0xff;
    if(opt & CHG_OPT_RAW) {
        length = sizeof(src);
    }
    if(opt & CHG_OPT_RAW) {       /* 原样复制 */
        *((INT08S *)dst)=src; /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
        reallen=length;
    } else if(opt & CHG_OPT_DEC){ /* 十进制转换 */
        sign=0;
        if(opt & CHG_OPT_SIG){
            if(src&0x80){  /*lint !e737 : Loss of sign in promotion from long to unsigned long*/
                sign=1;
                src=-src;
            }
        }
        usrc=(INT08U)src;
        if(opt & CHG_OPT_FRC){
            frclen=(length&0xf);
            length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
            if(frclen>MAX_FRC_LEN)frclen=MAX_FRC_LEN;
            for(i=(MAX_STR_LEN-1), j=0; j<frclen; i--, j++){
                tempbuf[i]=DECtoCHAR((usrc)%10);
                usrc/=10;
            }
            if(frclen>0){
                tempbuf[i]='.';
                i--;
            }
            for(; i>=1; i--){
                tempbuf[i]=DECtoCHAR((usrc)%10);
                usrc/=10;
            }
        } else {
            for(i=(MAX_STR_LEN-1); i>=1; i--){
                tempbuf[i]=DECtoCHAR((usrc)%10);
                usrc/=10;
            }        
        }
        if(sign==1){
            tempbuf[0]='-';
        } else {
            tempbuf[0]='0';
        }
        tempbuf[MAX_STR_LEN]='\0';
        reallen=MAX_STR_LEN;
        if(tempbuf[0]=='-'){       /* 一个十进制负数 */
            for(j=1; j<reallen; j++){
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* 处理数字为全0的情况 */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=1;
            if(tempbuf[j]=='.'){   /* 处理整数部分为空的情况: -.334等 */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        } else {
            for(j=0; j<reallen; j++){ /* 查找第一个非0数字的位置 */
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* 处理数字为全0的情况 */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=0;
            if(tempbuf[j]=='.'){   /* 处理整数部分为空的情况: .334等 */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        }
        if(opt & CHG_OPT_FIL){    /* 指定了宽度 */
            if(length<=reallen){  /* 如果是要截取一部分 */
                reallen=length;
            }
            if(opt & CHG_OPT_LFT){/* 左对齐, 右边只能填存空格 */
                for(i=0; i<reallen; i++){
                    dst[i]=tempbuf[i];
                }
                for(; i<length; i++){
                    dst[i]=' ';
                }
            } else {              /* 右对齐, 左边可填存空格或'0' */
                if((tempbuf[0]!='-') && (opt&CHG_OPT_ZER)){ /* 填'0' */
                    for(i=0; i<length-reallen; i++){
                        dst[i]='0';
                    }
                } else {          /* 填空格 */
                    for(i=0; i<length-reallen; i++){
                        dst[i]=' ';
                    }
                }
                for(j=0; i<length; i++, j++){
                    dst[i]=tempbuf[j];
                }                            
            }
            reallen=length;
        } else {                   /* 没有指定宽度 */
            for(i=0; i<reallen; i++){
                dst[i]=tempbuf[i];
            }
        }
    } else {                      /* 十六进制转换, 宽度固定为8位 */
        for(i=(HEX_STR_LEN-1); i>=0; i--){
            dst[i]=HEXtoCHAR((src)&0xf);
            src>>=4;  /*lint !e702: Shift right of signed quantity (int) */
        }
        reallen=HEX_STR_LEN;
    }
    if(opt & CHG_OPT_END){
        dst[reallen]='\0';
    }

    return dst;

    #undef MAX_FRC_LEN
    #undef MAX_STR_LEN
    #undef HEX_STR_LEN
}
/*----------------------------------------------------------------------------------------------
 * 将一个INT16转化为字符串
 *
 * 支持下列选项:
 *  CHG_OPT_RAW      不转换, 直接将字节加到目标字符串中
 *  CHG_OPT_DEC      将数字转换为十进制串(否则为十六进制串)
 *  CHG_OPT_SIG      符号标志
 *  CHG_OPT_FIL      是否指定宽度
 *  CHG_OPT_ZER      如果指定宽度, 高位是否填存为0(反之填存为空格)
 *  CHG_OPT_LFT      如果指定宽度, 是否左对齐
 *  CHG_OPT_FRC      是否转为实数串(有小数点)
 *  CHG_OPT_END      是否要为字符串加上结束符
 *
 * 当指定为实数时, 低8位分为了两部分, 其高4位代表整个长度, 低4位代表小数级数(i.e. 3代表0.001)
 *                 小数级数最大为5
**--------------------------------------------------------------------------------------------*/
INT08S * INT16toSTR(INT08S * dst, INT16S src, INT16U opt)
{
    INT16S i, j;
    INT16S length, reallen;
    INT16S frclen, sign;
    INT32U usrc;
    INT08S tempbuf[12];
    #define MAX_FRC_LEN  5
    #define MAX_STR_LEN  9
    #define HEX_STR_LEN  4

    FamesAssert(dst);

    if(!dst){ /* dst must't be NULL */
        return NULL;
    }

    length = opt & 0xff;
    if(opt & CHG_OPT_RAW) {
        length = sizeof(src);
    }
    if(opt & CHG_OPT_RAW) {       /* 原样复制 */
        *((INT16S *)dst)=src; /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
        reallen=length;
    } else if(opt & CHG_OPT_DEC){ /* 十进制转换 */
        sign=0;
        if(opt & CHG_OPT_SIG){
            if(src&0x8000){  /*lint !e737 : Loss of sign in promotion from long to unsigned long*/
                sign=1;
                src=-src;
            }
        }
        usrc=(INT16U)src;
        if(opt & CHG_OPT_FRC){
            frclen=(length&0xf);
            length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
            if(frclen>MAX_FRC_LEN)frclen=MAX_FRC_LEN;
            for(i=(MAX_STR_LEN-1), j=0; j<frclen; i--, j++){
                tempbuf[i]=DECtoCHAR((usrc)%10);
                usrc/=10;
            }
            if(frclen>0){
                tempbuf[i]='.';
                i--;
            }
            for(; i>=1; i--){
                tempbuf[i]=DECtoCHAR((usrc)%10);
                usrc/=10;
            }
        } else {
            for(i=(MAX_STR_LEN-1); i>=1; i--){
                tempbuf[i]=DECtoCHAR((usrc)%10);
                usrc/=10;
            }        
        }
        if(sign==1){
            tempbuf[0]='-';
        } else {
            tempbuf[0]='0';
        }
        tempbuf[MAX_STR_LEN]='\0';
        reallen=MAX_STR_LEN;
        if(tempbuf[0]=='-'){       /* 一个十进制负数 */
            for(j=1; j<reallen; j++){
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* 处理数字为全0的情况 */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=1;
            if(tempbuf[j]=='.'){   /* 处理整数部分为空的情况: -.334等 */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        } else {
            for(j=0; j<reallen; j++){ /* 查找第一个非0数字的位置 */
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* 处理数字为全0的情况 */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=0;
            if(tempbuf[j]=='.'){   /* 处理整数部分为空的情况: .334等 */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        }
        if(opt & CHG_OPT_FIL){    /* 指定了宽度 */
            if(length<=reallen){  /* 如果是要截取一部分 */
                reallen=length;
            }
            if(opt & CHG_OPT_LFT){/* 左对齐, 右边只能填存空格 */
                for(i=0; i<reallen; i++){
                    dst[i]=tempbuf[i];
                }
                for(; i<length; i++){
                    dst[i]=' ';
                }
            } else {              /* 右对齐, 左边可填存空格或'0' */
                if((tempbuf[0]!='-') && (opt&CHG_OPT_ZER)){ /* 填'0' */
                    for(i=0; i<length-reallen; i++){
                        dst[i]='0';
                    }
                } else {          /* 填空格 */
                    for(i=0; i<length-reallen; i++){
                        dst[i]=' ';
                    }
                }
                for(j=0; i<length; i++, j++){
                    dst[i]=tempbuf[j];
                }                            
            }
            reallen=length;
        } else {                  /* 没有指定宽度 */
            for(i=0; i<reallen; i++){
                dst[i]=tempbuf[i];
            }
        }
    } else {                      /* 十六进制转换, 宽度固定为8位 */
        for(i=(HEX_STR_LEN-1); i>=0; i--){
            dst[i]=HEXtoCHAR((src)&0xf);
            src>>=4;  /*lint !e702: Shift right of signed quantity (int) */
        }
        reallen=HEX_STR_LEN;
    }
    if(opt & CHG_OPT_END){
        dst[reallen]='\0';
    }

    return dst;

    #undef MAX_FRC_LEN
    #undef MAX_STR_LEN
    #undef HEX_STR_LEN
}

/*----------------------------------------------------------------------------------------------
 * 将一个INT32转化为字符串
 *
 * 支持下列选项:
 *  CHG_OPT_RAW      不转换, 直接将字节加到目标字符串中
 *  CHG_OPT_DEC      将数字转换为十进制串(否则为十六进制串)
 *  CHG_OPT_SIG      符号标志
 *  CHG_OPT_FIL      是否指定宽度
 *  CHG_OPT_ZER      如果指定宽度, 高位是否填存为0(反之填存为空格)
 *  CHG_OPT_LFT      如果指定宽度, 是否左对齐
 *  CHG_OPT_FRC      是否转为实数串(有小数点)
 *  CHG_OPT_END      是否要为字符串加上结束符
 *
 * 当指定为实数时, 低8位分为了两部分, 其高4位代表整个长度, 低4位代表小数级数(i.e. 3代表0.001)
 *                 小数级数最大为9
**--------------------------------------------------------------------------------------------*/
INT08S * INT32toSTR(INT08S * dst, INT32S src, INT16U opt)
{
    INT16S i, j;
    INT16S length, reallen;
    INT16S frclen, sign;
    INT32U usrc;
    INT08S tempbuf[16];
    #define MAX_FRC_LEN  9
    #define MAX_STR_LEN  12
    #define HEX_STR_LEN  8

    FamesAssert(dst);

    if(!dst){ /* dst must't be NULL */
        return NULL;
    }

    length = opt & 0xff;
    if(opt & CHG_OPT_RAW) {
        length = sizeof(src);
    }
    if(opt & CHG_OPT_RAW) {       /* 原样复制 */
        *((INT32S *)dst)=src; /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
        reallen=length;
    } else if(opt & CHG_OPT_DEC){ /* 十进制转换 */
        sign=0;
        if(opt & CHG_OPT_SIG){
            if(src&0x80000000L){  /*lint !e737 : Loss of sign in promotion from long to unsigned long*/
                sign=1;
                src=-src;
            }
        }
        usrc=(INT32U)src;
        if(opt & CHG_OPT_FRC){
            frclen=(length&0xf);
            length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
            if(frclen>MAX_FRC_LEN)frclen=MAX_FRC_LEN;
            for(i=(MAX_STR_LEN-1), j=0; j<frclen; i--, j++){
                tempbuf[i]=DECtoCHAR((usrc)%10L);
                usrc/=10L;
            }
            if(frclen>0){
                tempbuf[i]='.';
                i--;
            }
            for(; i>=1; i--){
                tempbuf[i]=DECtoCHAR((usrc)%10L);
                usrc/=10L;
            }
        } else {
            for(i=(MAX_STR_LEN-1); i>=1; i--){
                tempbuf[i]=DECtoCHAR((usrc)%10L);
                usrc/=10L;
            }        
        }
        if(sign==1){
            tempbuf[0]='-';
        } else {
            tempbuf[0]='0';
        }
        tempbuf[MAX_STR_LEN]='\0';
        reallen=MAX_STR_LEN;
        if(tempbuf[0]=='-'){       /* 一个十进制负数 */
            for(j=1; j<reallen; j++){
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* 处理数字为全0的情况 */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=1;
            if(tempbuf[j]=='.'){   /* 处理整数部分为空的情况: -.334等 */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        } else {
            for(j=0; j<reallen; j++){ /* 查找第一个非0数字的位置 */
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* 处理数字为全0的情况 */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=0;
            if(tempbuf[j]=='.'){   /* 处理整数部分为空的情况: .334等 */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        }
        if(opt & CHG_OPT_FIL){    /* 指定了宽度 */
            if(length<=reallen){  /* 如果是要截取一部分 */
                reallen=length;
            }
            if(opt & CHG_OPT_LFT){/* 左对齐, 右边只能填存空格 */
                for(i=0; i<reallen; i++){
                    dst[i]=tempbuf[i];
                }
                for(; i<length; i++){
                    dst[i]=' ';
                }
            } else {              /* 右对齐, 左边可填存空格或'0' */
                if((tempbuf[0]!='-') && (opt&CHG_OPT_ZER)){ /* 填'0' */
                    for(i=0; i<length-reallen; i++){
                        dst[i]='0';
                    }
                } else {          /* 填空格 */
                    for(i=0; i<length-reallen; i++){
                        dst[i]=' ';
                    }
                }
                for(j=0; i<length; i++, j++){
                    dst[i]=tempbuf[j];
                }                            
            }
            reallen=length;
        } else {                   /* 没有指定宽度 */
            for(i=0; i<reallen; i++){
                dst[i]=tempbuf[i];
            }
        }
    } else {                      /* 十六进制转换, 宽度固定为8位 */
        for(i=(HEX_STR_LEN-1); i>=0; i--){
            dst[i]=HEXtoCHAR((src)&0xfL);
            src>>=4L;  /*lint !e704: Shift right of signed quantity (long) */
        }
        reallen=HEX_STR_LEN;
    }
    if(opt & CHG_OPT_END){
        dst[reallen]='\0';
    }

    return dst;

    #undef MAX_FRC_LEN
    #undef MAX_STR_LEN
    #undef HEX_STR_LEN
}

/*----------------------------------------------------------------------------------------------
 * 字符串转化为字符串
 *
 * 支持下列选项:
 *  CHG_OPT_UPR      将字符串转为大写(仅用于字符串转换)
 *  CHG_OPT_LOW      将字符串转为小写(仅用于字符串转换)
 *  CHG_OPT_RVT      字符串反转
 *  CHG_OPT_END      为字符串加上结束符
 *
 * 注意: CHG_OPT_UPR|CHG_OPT_LOW 会将大写转为小写, 将小写转为大写
**--------------------------------------------------------------------------------------------*/
INT08S * STRtoSTR(INT08S * dst, INT08S * src, INT16U opt)
{
    INT16S i, length;

    FamesAssert(dst);
    FamesAssert(src);

    if(!dst || !src){
        return NULL;
    }

    length = opt & 0xff;
    if(length==0) {
        length=(INT16S)STRLEN(src);
    }
    for(i=0; i<length; i++) {  /* 拷贝到dst */
        if((opt&CHG_OPT_UPR)&&(src[i]>='a')&&(src[i]<='z')) {
            dst[i]=(src[i]-0x20);
        } else if((opt&CHG_OPT_LOW)&&(src[i]>='A')&&(src[i]<='Z')) {
            dst[i]=(src[i]+0x20);
        } else {
            dst[i]= src[i];
        }
    }
    if(opt & CHG_OPT_RVT) {   /* 反转 */
        INT16S halflen;
        INT08S tempchar;
        halflen = length / 2;
        length--;
        for(i=0; i<halflen; i++) {/*lint --e{679}*/
            tempchar      = dst[i];
            dst[i]        = dst[length-i];
            dst[length-i] = tempchar;
        }
        length++;
    }
    if(opt & CHG_OPT_END) {
        dst[length]='\0';
    }
    return dst;
}

/*----------------------------------------------------------------------------------------------
 * 字符串转化为整数(8位)
 *
 * 支持下列选项:
 *  CHG_OPT_RAW      不转换，直接将字符(串)解释为数字
 *  CHG_OPT_DEC      指定字符串为十进制串(否则为十六进制串)
 *  CHG_OPT_RVT      将字符串反转后再解释(用于不同的字节序)
 *  CHG_OPT_FRC      字符串是一个实数(带小数点)
 *
 * 如果转化失败，则返回(0)
 * 转化结果有可能溢出(Overflow), 需特别注意!
**--------------------------------------------------------------------------------------------*/
INT08S STRtoINT08(INT08S * src, INT16U opt)
{
    INT08S retval;
    INT16S length, i, sign;
    INT16S frclen = 0, dot = 0;
    
    FamesAssert(src);

    if(!src){
        return (INT08S)0;
    }

    length = opt & 0xff;
    if(opt & CHG_OPT_FRC) {
        opt |= CHG_OPT_DEC;   /* 只有十进制数才有小数 */
        frclen = length & 0xf;
        length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
    }
    if(opt & CHG_OPT_RAW) {
        length=sizeof(retval);
    }
    if(length==0) {
        length=(INT16S)STRLEN(src);
    }
    if(opt & CHG_OPT_RVT) {   /* 反转 */
        INT16S halflen;
        INT08S tempchar;
        halflen = length / 2;
        length--;
        for(i=0; i<halflen; i++) {/*lint --e{679}*/
            tempchar      = src[i];
            src[i]        = src[length-i];
            src[length-i] = tempchar;
        }
        length++;
    }
    if(opt & CHG_OPT_RAW) {
        retval = *((INT08S *)src); /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
    } else {
        INT16S tempvalue;
        retval = (INT08S)0;
        for(i=0; i<length; i++){
            if((src[i]!=' ')&&(src[i]!='\t')){ /* 跳过空字符(空格/Tab) */
                break;
            }
        }
        if(src[i]=='-'){
            sign=1;
            i++;
        } else if(src[i]=='+'){
            sign=0;
            i++;
        } else {
            sign=0;
        }
        if(opt & CHG_OPT_DEC) {
            for(; i<length; i++) {
                tempvalue = CHARtoDEC(src[i]);
                if(tempvalue>=0) {
                    if(dot){ /* 有小数点...... */
                        if(frclen==0){
                            break;
                        }
                        frclen--;
                    }
                    retval*=(INT08S)10;
                    retval+=(INT08S)tempvalue;
                } else {
                    if(src[i]=='.'){
                        if((opt&CHG_OPT_FRC) && (dot==0)){
                            dot = 1;
                            continue;
                        }
                    }
                    break;
                }
            }
            for(; frclen>0; frclen--){ /* 补齐小数位数 */
                retval*=(INT08S)10;
            }
        } else {
            for(; i<length; i++) {
                tempvalue = CHARtoHEX(src[i]);
                if(tempvalue>=0) {
                    retval<<=(INT08S)4; /*lint !e701: Shift left of signed quantity (int) */
                    retval+=(INT08S)tempvalue;
                } else {
                    break;
                }
            }
        }
        if(sign){  /* 字符串中有一个负号 */
            retval=-retval;
        }
    }
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * 字符串转化为整数(16位)
 *
 * 支持下列选项:
 *  CHG_OPT_RAW      不转换，直接将字符(串)解释为数字
 *  CHG_OPT_DEC      指定字符串为十进制串(否则为十六进制串)
 *  CHG_OPT_RVT      将字符串反转后再解释(用于不同的字节序)
 *  CHG_OPT_FRC      字符串是一个实数(带小数点)
 *
 * 如果转化失败，则返回(0)
 * 转化结果有可能溢出(Overflow), 需特别注意!
**--------------------------------------------------------------------------------------------*/
INT16S STRtoINT16(INT08S * src, INT16U opt)
{
    INT16S retval;
    INT16S length, i, sign;
    INT16S frclen = 0, dot = 0;

    FamesAssert(src);

    if(!src){
        return (INT16S)0;
    }

    length = opt & 0xff;
    if(opt & CHG_OPT_FRC) {
        opt |= CHG_OPT_DEC;   /* 只有十进制数才有小数 */
        frclen = length & 0xf;
        length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
    }
    if(opt & CHG_OPT_RAW) {
        length=sizeof(retval);
    }
    if(length==0) {
        length=(INT16S)STRLEN(src);
    }
    if(opt & CHG_OPT_RVT) {   /* 反转 */
        INT16S halflen;
        INT08S tempchar;
        halflen = length / 2;
        length--;
        for(i=0; i<halflen; i++) {/*lint --e{679}*/
            tempchar      = src[i];
            src[i]        = src[length-i];
            src[length-i] = tempchar;
        }
        length++;
    }
    if(opt & CHG_OPT_RAW) {
        retval = *((INT16S *)src); /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
    } else {
        INT16S tempvalue;
        retval = (INT16S)0;
        for(i=0; i<length; i++){
            if((src[i]!=' ')&&(src[i]!='\t')){ /* 跳过空字符(空格/Tab) */
                break;
            }
        }
        if(src[i]=='-'){
            sign=1;
            i++;
        } else if(src[i]=='+'){
            sign=0;
            i++;
        } else {
            sign=0;
        }
        if(opt & CHG_OPT_DEC) {
            for(; i<length; i++) {
                tempvalue = CHARtoDEC(src[i]);
                if(tempvalue>=0) {
                    if(dot){ /* 有小数点...... */
                        if(frclen==0){
                            break;
                        }
                        frclen--;
                    }
                    retval*=10;
                    retval+=tempvalue;
                } else {
                    if(src[i]=='.'){
                        if((opt&CHG_OPT_FRC) && (dot==0)){
                            dot = 1;
                            continue;
                        }
                    }
                    break;
                }
            }
            for(; frclen>0; frclen--){ /* 补齐小数位数 */
                retval*=10;
            }
        } else {
            for(; i<length; i++) {
                tempvalue = CHARtoHEX(src[i]);
                if(tempvalue>=0) {
                    retval<<=4;/*lint !e701: Shift left of signed quantity (int) */
                    retval+=tempvalue;
                } else {
                    break;
                }
            }
        }
        if(sign){  /* 字符串中有一个负号 */
            retval=-retval;
        }
    }
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * 字符串转化为整数(32位)
 *
 * 支持下列选项:
 *  CHG_OPT_RAW      不转换，直接将字符(串)解释为数字
 *  CHG_OPT_DEC      指定字符串为十进制串(否则为十六进制串)
 *  CHG_OPT_RVT      将字符串反转后再解释(用于不同的字节序)
 *  CHG_OPT_FRC      字符串是一个实数(带小数点)
 *
 * 如果转化失败，则返回(0)
 * 转化结果有可能溢出(Overflow), 需特别注意!
**--------------------------------------------------------------------------------------------*/
INT32S STRtoINT32(INT08S * src, INT16U opt)
{
    INT32S retval;
    INT16S length, i, sign;
    INT16S frclen = 0, dot = 0;

    FamesAssert(src);
    
    if(!src){
        return (INT32S)0;
    }

    length = opt & 0xff;
    if(opt & CHG_OPT_FRC) {
        opt |= CHG_OPT_DEC;   /* 只有十进制数才有小数 */
        frclen = length & 0xf;
        length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
    }
    if(opt & CHG_OPT_RAW) {
        length=sizeof(retval);
    }
    if(length==0) {
        length=(INT16S)STRLEN(src);
    }
    if(opt & CHG_OPT_RVT) {   /* 反转 */
        INT16S halflen;
        INT08S tempchar;
        halflen = length / 2;
        length--;
        for(i=0; i<halflen; i++) {/*lint --e{679}*/
            tempchar      = src[i];
            src[i]        = src[length-i];
            src[length-i] = tempchar;
        }
        length++;
    }
    if(opt & CHG_OPT_RAW) {
        retval = *((INT32S *)src); /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
    } else {
        INT16S tempvalue;
        retval = (INT32S)0L;
        for(i=0; i<length; i++){
            if((src[i]!=' ')&&(src[i]!='\t')){ /* 跳过空字符(空格/Tab) */
                break;
            }
        }
        if(src[i]=='-'){
            sign=1;
            i++;
        } else if(src[i]=='+'){
            sign=0;
            i++;
        } else {
            sign=0;
        }
        if(opt & CHG_OPT_DEC) {
            for(; i<length; i++) {
                tempvalue = CHARtoDEC(src[i]);
                if(tempvalue>=0) {
                    if(dot){ /* 有小数点...... */
                        if(frclen==0){
                            break;
                        }
                        frclen--;
                    }
                    retval*=(INT32S)10L;
                    retval+=(INT32S)tempvalue;
                } else {
                    if(src[i]=='.'){
                        if((opt&CHG_OPT_FRC) && (dot==0)){
                            dot = 1;
                            continue;
                        }
                    }
                    break;
                }
            }
            for(; frclen>0; frclen--){ /* 补齐小数位数 */
                retval*=(INT32S)10L;
            }
        } else {
            for(; i<length; i++) {
                tempvalue = CHARtoHEX(src[i]);
                if(tempvalue>=0) {
                    retval*=(INT32S)16L;
                    retval+=(INT32S)tempvalue;
                } else {
                    break;
                }
            }
        }
        if(sign){  /* 字符串中有一个负号 */
            retval=-retval;
        }
    }
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * 将一个字节转为二进制字符串
 *
 * 支持下列选项:
 *  CHG_OPT_END      为字符串加上结束符
**--------------------------------------------------------------------------------------------*/
INT08S * INT08toBINSTR(INT08S * dst, INT08S src, INT16U opt)
{
    INT16S i;

    FamesAssert(dst);

    if(!dst){
        return NULL;
    }

    for(i=0; i<8; i++){
        if(src&0x80){
            dst[i]='1';
        } else {
            dst[i]='0';
        }
        src<<=1; /*lint !e701: Shift left of signed quantity (int) */
    }
    
    if(opt & CHG_OPT_END){
        dst[8]='\0';
    }
    
    return dst;
}

/*----------------------------------------------------------------------------------------------
 * 将一个字转为二进制字符串
 *
 * 支持下列选项:
 *  CHG_OPT_END      为字符串加上结束符
**--------------------------------------------------------------------------------------------*/
INT08S * INT16toBINSTR(INT08S * dst, INT16S src, INT16U opt)
{
    INT16S i;

    FamesAssert(dst);

    if(!dst){
        return NULL;
    }

    for(i=0; i<16; i++){
        if(src&0x8000){ /*lint !e737: Loss of sign in promotion from int to unsigned int */
            dst[i]='1';
        } else {
            dst[i]='0';
        }
        src<<=1; /*lint !e701: Shift left of signed quantity (int) */
    }
    if(opt & CHG_OPT_END){
        dst[16]='\0';
    }
    
    return dst;
}

/*----------------------------------------------------------------------------------------------
 * 将一个双字转为二进制字符串
 *
 * 支持下列选项:
 *  CHG_OPT_END      为字符串加上结束符
**--------------------------------------------------------------------------------------------*/
INT08S * INT32toBINSTR(INT08S * dst, INT32S src, INT16U opt)
{
    INT16S i;

    FamesAssert(dst);

    if(!dst){
        return NULL;
    }

    for(i=0; i<32; i++){
        if(src&0x80000000L){ /*lint !e737: Loss of sign in promotion from long to unsigned long */
            dst[i]='1';
        } else {
            dst[i]='0';
        }
        src<<=1; /*lint !e703: Shift left of signed quantity (long) */
    }
    
    if(opt & CHG_OPT_END){
        dst[32]='\0';
    }
    
    return dst;
}

/*----------------------------------------------------------------------------------------------
 * 将一个十进制字符转为数字
 * 
 * 注意: 转化失败将返回(-1)
**--------------------------------------------------------------------------------------------*/
INT16S CHARtoDEC(INT08S ch)
{
    if((ch>='0')&&(ch<='9')){
        return ((INT16S)ch-0x30);
    } else {
        return (-1);
    }
}

/*----------------------------------------------------------------------------------------------
 * 将一个十进制字符转为数字
 * 
 * 注意: 转化失败将返回(-1)
**--------------------------------------------------------------------------------------------*/
INT16S CHARtoHEX(INT08S ch)
{
    if((ch>='0')&&(ch<='9')){
        return ((INT16S)ch-0x30);
    } else if((ch>='a')&&(ch<='f')){
        return ((INT16S)ch-0x57);
    } else if((ch>='A')&&(ch<='F')){
        return ((INT16S)ch-0x37);
    } else {
        return (-1);
    }
}

/*----------------------------------------------------------------------------------------------
 * 将一个INT16字节逆序后输出
**--------------------------------------------------------------------------------------------*/
INT16U INT16XCHG(INT16U value)
{
    return (((value&0xff)<<8)+((value>>8)&0xff));
}

/*----------------------------------------------------------------------------------------------
 * 将一个INT32字节逆序后输出
**--------------------------------------------------------------------------------------------*/
INT32U INT32XCHG(INT32U value)
{
    return (((value&0xffL)<<24)+((value&0xff00L)<<8)+
        ((value&0xff0000L)>>8)+((value&0xff000000L)>>24));
}

/*----------------------------------------------------------------------------------------------
 * 将一个小写字母转为大写
**--------------------------------------------------------------------------------------------*/
INT08S UPPER(INT08S ch)
{
    if(ch >= 'a' && ch <= 'z')
        return (ch-0x20);
    return ch;
}

/*----------------------------------------------------------------------------------------------
 * 将一个大写字母转为小写
**--------------------------------------------------------------------------------------------*/
INT08S LOWER(INT08S ch)
{
    if(ch >= 'A' && ch <= 'Z')
        return (ch+0x20);
    return ch;
}

/*----------------------------------------------------------------------------------------------
 * 计算一个字符串的长度(不包括NUL)
**--------------------------------------------------------------------------------------------*/
INT16S STRLEN(INT08S * str)
{
    INT16S i;
    if(!str){
        return 0;
    }
    for(i=0; str[i]; i++){;}
    return i;
}

/*----------------------------------------------------------------------------------------------
 * 字符串拷贝
**--------------------------------------------------------------------------------------------*/
INT16S STRCPY(INT08S * dst, INT08S * src)
{
    INT16S i;
    if(!dst || !src){
        return 0;
    }
    for(i=0;;i++){
        dst[i]=src[i];
        if(dst[i]=='\0')break;
    }
    return i;
}

/*----------------------------------------------------------------------------------------------
 * 字符串比较
**--------------------------------------------------------------------------------------------*/
INT16S STRCMP(INT08S * s1, INT08S * s2)
{
    INT16S i;
    
    if(!s1 || !s2){
        return 0;
    }
    for(i=0;; i++){
        if(s1[i]>s2[i]){
            return 1;
        } else if(s1[i]<s2[i]){
            return -1;
        }
        if((!s1[i])||(!s2[i])){
            break;
        }
    }
    return 0;
    

}

/*----------------------------------------------------------------------------------------------
 * 字符串比较(没有大小写)
**--------------------------------------------------------------------------------------------*/
INT16S STRICMP(INT08S * s1, INT08S * s2)
{
    INT16S i;
    
    if(!s1 || !s2){
        return 0;
    }
    for(i=0;; i++){
        if(UPPER(s1[i])>UPPER(s2[i])){
            return 1;
        } else if(UPPER(s1[i])<UPPER(s2[i])){
            return -1;
        }
        if((!s1[i])||(!s2[i])){
            break;
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------------------------
 * 内存拷贝
**--------------------------------------------------------------------------------------------*/
INT16S MEMCPY(INT08S * dst, INT08S * src, INT16S len)
{
    INT16S i;
    
    if(!dst || !src || len<=0){
        return 0;
    }
    for(i=0; i<len; i++){
        dst[i]=src[i];
    }
    return i;
}

/*----------------------------------------------------------------------------------------------
 * 内存比较
**--------------------------------------------------------------------------------------------*/
INT16S MEMCMP(INT08S * s1, INT08S * s2, INT16S len)
{
    INT16S i;
    
    if(!s1 || !s2 || len<=0){
        return 0;
    }
    for(i=0; i<len; i++){
        if(s1[i]>s2[i]){
            return 1;
        } else if(s1[i]<s2[i]){
            return -1;
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------------------------
 * 内存比较(没有大小写)
**--------------------------------------------------------------------------------------------*/
INT16S MEMICMP(INT08S * s1, INT08S * s2, INT16S len)
{
    INT16S i;
    
    if(!s1 || !s2 || len<=0){
        return 0;
    }
    for(i=0; i<len; i++){
        if(UPPER(s1[i])>UPPER(s2[i])){
            return 1;
        } else if(UPPER(s1[i])<UPPER(s2[i])){
            return -1;
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------------------------
 * 内存填存
**--------------------------------------------------------------------------------------------*/
INT16S MEMSET(INT08S * dst, INT08S data, INT16S len)
{
    INT16S i;
    
    if(!dst || len<=0){
        return 0;
    }
    for(i=0; i<len; i++){
        dst[i]=data;
    }
    return i;
}

/*----------------------------------------------------------------------------------------------
 * 输出一个字节到I/O端口
**--------------------------------------------------------------------------------------------*/
void outportbyte(INT16S port, INT08U data)
{
    outportb(port, (INT08U)data);
}

/*----------------------------------------------------------------------------------------------
 * 从I/O端口读取一个字节
**--------------------------------------------------------------------------------------------*/
INT08U inportbyte(INT16S port)
{
    return (INT08U)inportb(port);
}

/*----------------------------------------------------------------------------------------------
 * 输出一个字到I/O端口
**--------------------------------------------------------------------------------------------*/
void outportword(INT16S port, INT16U data)
{
    outport(port, (INT16S)data);
}

/*----------------------------------------------------------------------------------------------
 * 从I/O端口读取一个字
**--------------------------------------------------------------------------------------------*/
INT16U inportword(INT16S port)
{
    return (INT16U)inport(port);
}

/*----------------------------------------------------------------------------------------------
 * 输出一个双字到I/O端口
**--------------------------------------------------------------------------------------------*/
void outportdword(INT16S port, INT32U data)
{   /*lint --e{550} : Symbol 'word1' not accessed*/ 
    INT16U word1, word2;
    
    word1=(INT16U)((data    )&0xFFFFL);
    word2=(INT16U)((data>>16)&0xFFFFL);
    asm pushf;
    asm cli;
    asm push dx;
    asm db   0x66, 0x50;
    asm mov  ax,   word2;
    asm db   0x66, 0xc1, 0xe0, 0x10;
    asm mov  ax,   word1;
    asm mov  dx,   port;
    asm db   0x66, 0xef;
    asm db   0x66, 0x58;
    asm pop  dx;
    asm popf;
    
    return;
}

/*----------------------------------------------------------------------------------------------
 * 从I/O端口读取一个双字
**--------------------------------------------------------------------------------------------*/
INT32U inportdword(INT16S port)
{   /*lint --e{530} : Symbol 'word2' not initialized*/
    INT16U word1, word2;
    INT32U data;
    
    asm pushf;
    asm cli;
    asm push dx;
    asm db   0x66, 0x50;
    asm mov  dx, port;
    asm db   0x66, 0xed;
    asm mov  word1, ax;
    asm db   0x66, 0xc1, 0xe8, 0x10;
    asm mov  word2, ax;
    asm db   0x66, 0x58;
    asm pop  dx;
    asm popf;
    
    data=(((INT32U)word2<<16)+(INT32U)word1);
    return data;
}

/*----------------------------------------------------------------------------------------------
 * 文本模式下的输出函数
**--------------------------------------------------------------------------------------------*/
void textprintstr(INT16S x, INT16S y, INT08S * str, INT08U color)
{
    INT16U i, disp_off, disp_seg;
    INT08S * disp;

    FamesAssert(str);

    if(!str){
        return;
    }

    x--,y--;
    disp_off=(INT16U)(160*y+x*2);
    disp_seg=0xB800;
    disp=MK_FP(disp_seg, disp_off);
    i=0;
    while(str[i]){
        *disp=str[i];
        disp++;
        *disp=(INT08S)((color==0)?(((INT08S)y%10+4)):color);
        disp++;
        i++;
    }
}
void textprintchar(INT16S x, INT16S y, INT08S ch, INT08U color)
{
    INT08S ts[2];
    ts[0]=ch;
    ts[1]=0;
    textprintstr(x, y, ts, color);
}
void textprintdec16(INT16S x, INT16S y, INT16S val, INT08U color, INT16U opt)
{
    INT08S ts[32];
    INT16toSTR(ts, val, opt|CHG_OPT_DEC|CHG_OPT_END);
    textprintstr(x, y, ts, color);
}
void textprintdec32(INT16S x, INT16S y, INT32S val, INT08U color, INT16U opt)
{
    INT08S ts[32];
    INT32toSTR(ts, val, opt|CHG_OPT_DEC|CHG_OPT_END);
    textprintstr(x, y, ts, color);
}
void textprinthex16(INT16S x, INT16S y, INT16S val, INT08U color)
{
    INT08S ts[9];
    INT16toSTR(ts, val, CHG_OPT_END);
    textprintstr(x, y, ts, color);
}
void textprinthex32(INT16S x, INT16S y, INT32S val, INT08U color)
{
    INT08S ts[9];
    INT32toSTR(ts, val, CHG_OPT_END);
    textprintstr(x, y, ts, color);
}


/*
*本文件结束: tool.c =============================================================================
*/


