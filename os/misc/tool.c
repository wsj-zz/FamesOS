/************************************************************************************************
** �ļ�: tool.c
** ˵��: ��������,������
** ����: Jun
** ʱ��: 2010-7-12
************************************************************************************************/
#define  FAMES_TOOL_C
#include "includes.h"


/*----------------------------------------------------------------------------------------------
 * ��һ��INT8ת��Ϊ�ַ���
 *
 * ֧������ѡ��:
 *  CHG_OPT_RAW      ��ת��, ֱ�ӽ��ֽڼӵ�Ŀ���ַ�����
 *  CHG_OPT_DEC      ������ת��Ϊʮ���ƴ�(����Ϊʮ�����ƴ�)
 *  CHG_OPT_SIG      ���ű�־
 *  CHG_OPT_FIL      �Ƿ�ָ�����
 *  CHG_OPT_ZER      ���ָ�����, ��λ�Ƿ����Ϊ0(��֮���Ϊ�ո�)
 *  CHG_OPT_LFT      ���ָ�����, �Ƿ������
 *  CHG_OPT_FRC      �Ƿ�תΪʵ����(��С����)
 *  CHG_OPT_END      �Ƿ�ҪΪ�ַ������Ͻ�����
 *
 * ��ָ��Ϊʵ��ʱ, ��8λ��Ϊ��������, ���4λ������������, ��4λ����С������(i.e. 3����0.001)
 *                 С���������Ϊ3
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
    #define HEX_STR_LEN  2    /* ʮ�������ַ������� */

    FamesAssert(dst);

    if(!dst){ /* dst must't be NULL */
        return NULL;
    }

    length = opt & 0xff;
    if(opt & CHG_OPT_RAW) {
        length = sizeof(src);
    }
    if(opt & CHG_OPT_RAW) {       /* ԭ������ */
        *((INT08S *)dst)=src; /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
        reallen=length;
    } else if(opt & CHG_OPT_DEC){ /* ʮ����ת�� */
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
        if(tempbuf[0]=='-'){       /* һ��ʮ���Ƹ��� */
            for(j=1; j<reallen; j++){
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* ��������Ϊȫ0����� */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=1;
            if(tempbuf[j]=='.'){   /* ������������Ϊ�յ����: -.334�� */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        } else {
            for(j=0; j<reallen; j++){ /* ���ҵ�һ����0���ֵ�λ�� */
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* ��������Ϊȫ0����� */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=0;
            if(tempbuf[j]=='.'){   /* ������������Ϊ�յ����: .334�� */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        }
        if(opt & CHG_OPT_FIL){    /* ָ���˿�� */
            if(length<=reallen){  /* �����Ҫ��ȡһ���� */
                reallen=length;
            }
            if(opt & CHG_OPT_LFT){/* �����, �ұ�ֻ�����ո� */
                for(i=0; i<reallen; i++){
                    dst[i]=tempbuf[i];
                }
                for(; i<length; i++){
                    dst[i]=' ';
                }
            } else {              /* �Ҷ���, ��߿����ո��'0' */
                if((tempbuf[0]!='-') && (opt&CHG_OPT_ZER)){ /* ��'0' */
                    for(i=0; i<length-reallen; i++){
                        dst[i]='0';
                    }
                } else {          /* ��ո� */
                    for(i=0; i<length-reallen; i++){
                        dst[i]=' ';
                    }
                }
                for(j=0; i<length; i++, j++){
                    dst[i]=tempbuf[j];
                }                            
            }
            reallen=length;
        } else {                   /* û��ָ����� */
            for(i=0; i<reallen; i++){
                dst[i]=tempbuf[i];
            }
        }
    } else {                      /* ʮ������ת��, ��ȹ̶�Ϊ8λ */
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
 * ��һ��INT16ת��Ϊ�ַ���
 *
 * ֧������ѡ��:
 *  CHG_OPT_RAW      ��ת��, ֱ�ӽ��ֽڼӵ�Ŀ���ַ�����
 *  CHG_OPT_DEC      ������ת��Ϊʮ���ƴ�(����Ϊʮ�����ƴ�)
 *  CHG_OPT_SIG      ���ű�־
 *  CHG_OPT_FIL      �Ƿ�ָ�����
 *  CHG_OPT_ZER      ���ָ�����, ��λ�Ƿ����Ϊ0(��֮���Ϊ�ո�)
 *  CHG_OPT_LFT      ���ָ�����, �Ƿ������
 *  CHG_OPT_FRC      �Ƿ�תΪʵ����(��С����)
 *  CHG_OPT_END      �Ƿ�ҪΪ�ַ������Ͻ�����
 *
 * ��ָ��Ϊʵ��ʱ, ��8λ��Ϊ��������, ���4λ������������, ��4λ����С������(i.e. 3����0.001)
 *                 С���������Ϊ5
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
    if(opt & CHG_OPT_RAW) {       /* ԭ������ */
        *((INT16S *)dst)=src; /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
        reallen=length;
    } else if(opt & CHG_OPT_DEC){ /* ʮ����ת�� */
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
        if(tempbuf[0]=='-'){       /* һ��ʮ���Ƹ��� */
            for(j=1; j<reallen; j++){
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* ��������Ϊȫ0����� */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=1;
            if(tempbuf[j]=='.'){   /* ������������Ϊ�յ����: -.334�� */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        } else {
            for(j=0; j<reallen; j++){ /* ���ҵ�һ����0���ֵ�λ�� */
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* ��������Ϊȫ0����� */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=0;
            if(tempbuf[j]=='.'){   /* ������������Ϊ�յ����: .334�� */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        }
        if(opt & CHG_OPT_FIL){    /* ָ���˿�� */
            if(length<=reallen){  /* �����Ҫ��ȡһ���� */
                reallen=length;
            }
            if(opt & CHG_OPT_LFT){/* �����, �ұ�ֻ�����ո� */
                for(i=0; i<reallen; i++){
                    dst[i]=tempbuf[i];
                }
                for(; i<length; i++){
                    dst[i]=' ';
                }
            } else {              /* �Ҷ���, ��߿����ո��'0' */
                if((tempbuf[0]!='-') && (opt&CHG_OPT_ZER)){ /* ��'0' */
                    for(i=0; i<length-reallen; i++){
                        dst[i]='0';
                    }
                } else {          /* ��ո� */
                    for(i=0; i<length-reallen; i++){
                        dst[i]=' ';
                    }
                }
                for(j=0; i<length; i++, j++){
                    dst[i]=tempbuf[j];
                }                            
            }
            reallen=length;
        } else {                  /* û��ָ����� */
            for(i=0; i<reallen; i++){
                dst[i]=tempbuf[i];
            }
        }
    } else {                      /* ʮ������ת��, ��ȹ̶�Ϊ8λ */
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
 * ��һ��INT32ת��Ϊ�ַ���
 *
 * ֧������ѡ��:
 *  CHG_OPT_RAW      ��ת��, ֱ�ӽ��ֽڼӵ�Ŀ���ַ�����
 *  CHG_OPT_DEC      ������ת��Ϊʮ���ƴ�(����Ϊʮ�����ƴ�)
 *  CHG_OPT_SIG      ���ű�־
 *  CHG_OPT_FIL      �Ƿ�ָ�����
 *  CHG_OPT_ZER      ���ָ�����, ��λ�Ƿ����Ϊ0(��֮���Ϊ�ո�)
 *  CHG_OPT_LFT      ���ָ�����, �Ƿ������
 *  CHG_OPT_FRC      �Ƿ�תΪʵ����(��С����)
 *  CHG_OPT_END      �Ƿ�ҪΪ�ַ������Ͻ�����
 *
 * ��ָ��Ϊʵ��ʱ, ��8λ��Ϊ��������, ���4λ������������, ��4λ����С������(i.e. 3����0.001)
 *                 С���������Ϊ9
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
    if(opt & CHG_OPT_RAW) {       /* ԭ������ */
        *((INT32S *)dst)=src; /*lint !e826: Suspicious pointer-to-pointer conversion (area too small) */
        reallen=length;
    } else if(opt & CHG_OPT_DEC){ /* ʮ����ת�� */
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
        if(tempbuf[0]=='-'){       /* һ��ʮ���Ƹ��� */
            for(j=1; j<reallen; j++){
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* ��������Ϊȫ0����� */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=1;
            if(tempbuf[j]=='.'){   /* ������������Ϊ�յ����: -.334�� */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        } else {
            for(j=0; j<reallen; j++){ /* ���ҵ�һ����0���ֵ�λ�� */
                if(tempbuf[j]!='0')break;
            }
            if(j>=reallen) {       /* ��������Ϊȫ0����� */
                j=reallen-1;
                tempbuf[j]='0';
            }
            i=0;
            if(tempbuf[j]=='.'){   /* ������������Ϊ�յ����: .334�� */
                tempbuf[i]='0';
                i++;
            }
            for(; j<reallen; j++, i++){
                tempbuf[i]=tempbuf[j];
            }
            tempbuf[i]='\0';
            reallen=i;
        }
        if(opt & CHG_OPT_FIL){    /* ָ���˿�� */
            if(length<=reallen){  /* �����Ҫ��ȡһ���� */
                reallen=length;
            }
            if(opt & CHG_OPT_LFT){/* �����, �ұ�ֻ�����ո� */
                for(i=0; i<reallen; i++){
                    dst[i]=tempbuf[i];
                }
                for(; i<length; i++){
                    dst[i]=' ';
                }
            } else {              /* �Ҷ���, ��߿����ո��'0' */
                if((tempbuf[0]!='-') && (opt&CHG_OPT_ZER)){ /* ��'0' */
                    for(i=0; i<length-reallen; i++){
                        dst[i]='0';
                    }
                } else {          /* ��ո� */
                    for(i=0; i<length-reallen; i++){
                        dst[i]=' ';
                    }
                }
                for(j=0; i<length; i++, j++){
                    dst[i]=tempbuf[j];
                }                            
            }
            reallen=length;
        } else {                   /* û��ָ����� */
            for(i=0; i<reallen; i++){
                dst[i]=tempbuf[i];
            }
        }
    } else {                      /* ʮ������ת��, ��ȹ̶�Ϊ8λ */
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
 * �ַ���ת��Ϊ�ַ���
 *
 * ֧������ѡ��:
 *  CHG_OPT_UPR      ���ַ���תΪ��д(�������ַ���ת��)
 *  CHG_OPT_LOW      ���ַ���תΪСд(�������ַ���ת��)
 *  CHG_OPT_RVT      �ַ�����ת
 *  CHG_OPT_END      Ϊ�ַ������Ͻ�����
 *
 * ע��: CHG_OPT_UPR|CHG_OPT_LOW �Ὣ��дתΪСд, ��СдתΪ��д
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
    for(i=0; i<length; i++) {  /* ������dst */
        if((opt&CHG_OPT_UPR)&&(src[i]>='a')&&(src[i]<='z')) {
            dst[i]=(src[i]-0x20);
        } else if((opt&CHG_OPT_LOW)&&(src[i]>='A')&&(src[i]<='Z')) {
            dst[i]=(src[i]+0x20);
        } else {
            dst[i]= src[i];
        }
    }
    if(opt & CHG_OPT_RVT) {   /* ��ת */
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
 * �ַ���ת��Ϊ����(8λ)
 *
 * ֧������ѡ��:
 *  CHG_OPT_RAW      ��ת����ֱ�ӽ��ַ�(��)����Ϊ����
 *  CHG_OPT_DEC      ָ���ַ���Ϊʮ���ƴ�(����Ϊʮ�����ƴ�)
 *  CHG_OPT_RVT      ���ַ�����ת���ٽ���(���ڲ�ͬ���ֽ���)
 *  CHG_OPT_FRC      �ַ�����һ��ʵ��(��С����)
 *
 * ���ת��ʧ�ܣ��򷵻�(0)
 * ת������п������(Overflow), ���ر�ע��!
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
        opt |= CHG_OPT_DEC;   /* ֻ��ʮ����������С�� */
        frclen = length & 0xf;
        length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
    }
    if(opt & CHG_OPT_RAW) {
        length=sizeof(retval);
    }
    if(length==0) {
        length=(INT16S)STRLEN(src);
    }
    if(opt & CHG_OPT_RVT) {   /* ��ת */
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
            if((src[i]!=' ')&&(src[i]!='\t')){ /* �������ַ�(�ո�/Tab) */
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
                    if(dot){ /* ��С����...... */
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
            for(; frclen>0; frclen--){ /* ����С��λ�� */
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
        if(sign){  /* �ַ�������һ������ */
            retval=-retval;
        }
    }
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * �ַ���ת��Ϊ����(16λ)
 *
 * ֧������ѡ��:
 *  CHG_OPT_RAW      ��ת����ֱ�ӽ��ַ�(��)����Ϊ����
 *  CHG_OPT_DEC      ָ���ַ���Ϊʮ���ƴ�(����Ϊʮ�����ƴ�)
 *  CHG_OPT_RVT      ���ַ�����ת���ٽ���(���ڲ�ͬ���ֽ���)
 *  CHG_OPT_FRC      �ַ�����һ��ʵ��(��С����)
 *
 * ���ת��ʧ�ܣ��򷵻�(0)
 * ת������п������(Overflow), ���ر�ע��!
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
        opt |= CHG_OPT_DEC;   /* ֻ��ʮ����������С�� */
        frclen = length & 0xf;
        length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
    }
    if(opt & CHG_OPT_RAW) {
        length=sizeof(retval);
    }
    if(length==0) {
        length=(INT16S)STRLEN(src);
    }
    if(opt & CHG_OPT_RVT) {   /* ��ת */
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
            if((src[i]!=' ')&&(src[i]!='\t')){ /* �������ַ�(�ո�/Tab) */
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
                    if(dot){ /* ��С����...... */
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
            for(; frclen>0; frclen--){ /* ����С��λ�� */
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
        if(sign){  /* �ַ�������һ������ */
            retval=-retval;
        }
    }
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * �ַ���ת��Ϊ����(32λ)
 *
 * ֧������ѡ��:
 *  CHG_OPT_RAW      ��ת����ֱ�ӽ��ַ�(��)����Ϊ����
 *  CHG_OPT_DEC      ָ���ַ���Ϊʮ���ƴ�(����Ϊʮ�����ƴ�)
 *  CHG_OPT_RVT      ���ַ�����ת���ٽ���(���ڲ�ͬ���ֽ���)
 *  CHG_OPT_FRC      �ַ�����һ��ʵ��(��С����)
 *
 * ���ת��ʧ�ܣ��򷵻�(0)
 * ת������п������(Overflow), ���ر�ע��!
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
        opt |= CHG_OPT_DEC;   /* ֻ��ʮ����������С�� */
        frclen = length & 0xf;
        length>>=4;  /*lint !e702: Shift right of signed quantity (int) */
    }
    if(opt & CHG_OPT_RAW) {
        length=sizeof(retval);
    }
    if(length==0) {
        length=(INT16S)STRLEN(src);
    }
    if(opt & CHG_OPT_RVT) {   /* ��ת */
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
            if((src[i]!=' ')&&(src[i]!='\t')){ /* �������ַ�(�ո�/Tab) */
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
                    if(dot){ /* ��С����...... */
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
            for(; frclen>0; frclen--){ /* ����С��λ�� */
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
        if(sign){  /* �ַ�������һ������ */
            retval=-retval;
        }
    }
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ��һ���ֽ�תΪ�������ַ���
 *
 * ֧������ѡ��:
 *  CHG_OPT_END      Ϊ�ַ������Ͻ�����
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
 * ��һ����תΪ�������ַ���
 *
 * ֧������ѡ��:
 *  CHG_OPT_END      Ϊ�ַ������Ͻ�����
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
 * ��һ��˫��תΪ�������ַ���
 *
 * ֧������ѡ��:
 *  CHG_OPT_END      Ϊ�ַ������Ͻ�����
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
 * ��һ��ʮ�����ַ�תΪ����
 * 
 * ע��: ת��ʧ�ܽ�����(-1)
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
 * ��һ��ʮ�����ַ�תΪ����
 * 
 * ע��: ת��ʧ�ܽ�����(-1)
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
 * ��һ��INT16�ֽ���������
**--------------------------------------------------------------------------------------------*/
INT16U INT16XCHG(INT16U value)
{
    return (((value&0xff)<<8)+((value>>8)&0xff));
}

/*----------------------------------------------------------------------------------------------
 * ��һ��INT32�ֽ���������
**--------------------------------------------------------------------------------------------*/
INT32U INT32XCHG(INT32U value)
{
    return (((value&0xffL)<<24)+((value&0xff00L)<<8)+
        ((value&0xff0000L)>>8)+((value&0xff000000L)>>24));
}

/*----------------------------------------------------------------------------------------------
 * ��һ��Сд��ĸתΪ��д
**--------------------------------------------------------------------------------------------*/
INT08S UPPER(INT08S ch)
{
    if(ch >= 'a' && ch <= 'z')
        return (ch-0x20);
    return ch;
}

/*----------------------------------------------------------------------------------------------
 * ��һ����д��ĸתΪСд
**--------------------------------------------------------------------------------------------*/
INT08S LOWER(INT08S ch)
{
    if(ch >= 'A' && ch <= 'Z')
        return (ch+0x20);
    return ch;
}

/*----------------------------------------------------------------------------------------------
 * ����һ���ַ����ĳ���(������NUL)
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
 * �ַ�������
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
 * �ַ����Ƚ�
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
 * �ַ����Ƚ�(û�д�Сд)
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
 * �ڴ濽��
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
 * �ڴ�Ƚ�
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
 * �ڴ�Ƚ�(û�д�Сд)
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
 * �ڴ����
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
 * ���һ���ֽڵ�I/O�˿�
**--------------------------------------------------------------------------------------------*/
void outportbyte(INT16S port, INT08U data)
{
    outportb(port, (INT08U)data);
}

/*----------------------------------------------------------------------------------------------
 * ��I/O�˿ڶ�ȡһ���ֽ�
**--------------------------------------------------------------------------------------------*/
INT08U inportbyte(INT16S port)
{
    return (INT08U)inportb(port);
}

/*----------------------------------------------------------------------------------------------
 * ���һ���ֵ�I/O�˿�
**--------------------------------------------------------------------------------------------*/
void outportword(INT16S port, INT16U data)
{
    outport(port, (INT16S)data);
}

/*----------------------------------------------------------------------------------------------
 * ��I/O�˿ڶ�ȡһ����
**--------------------------------------------------------------------------------------------*/
INT16U inportword(INT16S port)
{
    return (INT16U)inport(port);
}

/*----------------------------------------------------------------------------------------------
 * ���һ��˫�ֵ�I/O�˿�
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
 * ��I/O�˿ڶ�ȡһ��˫��
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
 * �ı�ģʽ�µ��������
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
*���ļ�����: tool.c =============================================================================
*/


