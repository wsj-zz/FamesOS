/***********************************************************************************************
** �ļ�:    bmp.c
**
** ˵��:    BMPͼ����غ���
**
** ����:    Jun
**
** ʱ��:    2010-04-15
**
** ע��:    �˰汾ֻ�ܴ���"δѹ��"��256ɫ��BMPͼƬ
***********************************************************************************************/
#define  FAMES_BMP_C
#include <includes.h>


#if     FAMES_BMP_EN == 1
/*----------------------------------------------------------------------------------------------
 * ����:    InitBMPINFO()
 * ˵��:    ��ʼ��BMPINFO�ṹ 
 * ����:    bmpinfo
**--------------------------------------------------------------------------------------------*/
BOOLEAN apical InitBMPINFO(BMPINFO * bmpinfo)
{
    FamesAssert(bmpinfo);

    if(!bmpinfo)
        return fail;

    bmpinfo->ready       = FAMES_BMP_READY_NO;
    bmpinfo->handle      = 0;
    bmpinfo->width       = 0;
    bmpinfo->height      = 0;
    bmpinfo->maxwidth    = 0;
    bmpinfo->maxheight   = 0;
    bmpinfo->colordepth  = 0;
    bmpinfo->bytesofline = 0;
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    LoadBmp()
 * ˵��:    ����BMP�ļ��е�ͼ��XMS, ����Ӧ����BMPINFO�ṹ
 * ����:    bmpinfo BMPINFO����ָ��
 *          bmpfile BMP�ļ���
**--------------------------------------------------------------------------------------------*/
BOOLEAN apical LoadBmp(BMPINFO * bmpinfo, FILENAME bmpfile)
{
    BmpCoreInfo   bmpcoreinfo;
    BmpInfoHeader bmpinfoheader;
    int           fd;
    XMSHANDLE     handle;
    INT16U        bytesofline;
    INT16U        i;
    
    if(!bmpinfo){
        return fail;
    }
    if(!bmpfile){
        return fail;
    }
    if(bmpinfo->ready!=FAMES_BMP_READY_NO){
        return fail;
    }
    DispatchLock();
    fd=open(bmpfile,O_RDONLY|O_BINARY);/*lint !e569*/
    DispatchUnlock();
    if(fd < 0){
        return fail;
    }
    DispatchLock();
    read(fd, (void *)&bmpcoreinfo, sizeof(BmpCoreInfo));
    DispatchUnlock();
    if((bmpcoreinfo.identifier[0]!='B')||(bmpcoreinfo.identifier[1]!='M')){
        DispatchLock();
        close(fd);
        DispatchUnlock();
        return fail;        
    }
    DispatchLock();
    read(fd, (void *)&bmpinfoheader, sizeof(BmpInfoHeader));
    DispatchUnlock();
    switch(bmpinfoheader.bits_per_pixel){
        case 8:
            handle=XMSalloc((INT16U)bmpinfoheader.height+1);
            if(!handle){
                DispatchLock();
                close(fd);
                DispatchUnlock();
                return fail;
            }
            if(bmpinfoheader.height!=0L){
                bytesofline=(INT16U)(bmpinfoheader.bitmap_data_size/bmpinfoheader.height);
            } else {
                DispatchLock();
                close(fd);
                DispatchUnlock();
                return fail;                
            }
            DispatchLock();
            lseek(fd, (INT32S)bmpcoreinfo.bitmap_data_offset, SEEK_SET);
            DispatchUnlock();
            for(i=(INT16U)bmpinfoheader.height-1; (INT16S)i>=0; i--){                                
                DispatchLock();
                if(eof(fd)){
                    close(fd); 
                    DispatchUnlock();
                    return fail; 
                }
                read(fd, (void *)BMPCACHE, bytesofline);
                DispatchUnlock();
                if(!XMSput(handle, (void *)((INT32U)i<<10L), (void *)BMPCACHE, 1024L)){
                    ;
                }
            }
            DispatchLock();
            close(fd); 
            DispatchUnlock();
            bmpinfo->colordepth  = FAMES_BMP_CD_8;
            bmpinfo->width       = (INT16S)bmpinfoheader.width;
            bmpinfo->height      = (INT16S)bmpinfoheader.height;
            bmpinfo->maxwidth    = 0;
            bmpinfo->maxheight   = 0;
            bmpinfo->bytesofline = (INT16S)bytesofline;
            bmpinfo->handle      = handle;
            bmpinfo->ready       = FAMES_BMP_READY_YES;
            break;
        default:
            DispatchLock();
            close(fd);
            DispatchUnlock();
            return fail;
    }
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    UnloadBmp()
 * ˵��:    �ͷ�BMP�ļ���ռ��XMS�ڴ��, ����Ӧ����BMPINFO�ṹ
 * ����:    bmpinfo BMPINFO����ָ��
**--------------------------------------------------------------------------------------------*/
BOOLEAN apical UnloadBmp(BMPINFO * bmpinfo)
{
    FamesAssert(bmpinfo);

    if(!bmpinfo){
        return fail;
    }
    if(bmpinfo->ready!=FAMES_BMP_READY_YES){
        return fail;
    }
    if(bmpinfo->handle==0){
        return fail;
    }
    lock_kernel();
    XMSfree(bmpinfo->handle);
    InitBMPINFO(bmpinfo);
    unlock_kernel();

    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    ShowBmp()
 * ˵��:    ����BMPINFO�ṹ��ʾһ��ͼƬ
 * ����:    x,y     ͼ������Ļ�ϵ�����(���Ͻ�)                 
 *          bmpinfo BMPINFO����ָ��
**--------------------------------------------------------------------------------------------*/
BOOLEAN apical ShowBmp(INT16S x, INT16S y, BMPINFO * bmpinfo)
{
    INT16S width, height, i, x2, /*y2,*/ bytesofline;
    
    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(!bmpinfo){
        return fail;
    }
    if(bmpinfo->ready!=FAMES_BMP_READY_YES){
        return fail;
    }
    if(bmpinfo->handle==0){
        return fail;
    }
    width =bmpinfo->width;
    height=bmpinfo->height;
    if(bmpinfo->maxwidth!=0){
        if(width>bmpinfo->maxwidth){
            width=bmpinfo->maxwidth;
        }
    }
    if(bmpinfo->maxheight!=0){
        if(height>bmpinfo->maxheight){
            height=bmpinfo->maxheight;
        }
    }
    x2=x+width-1;
    /*
    y2=y+height-1;
    */
    bytesofline=width;
    if(bytesofline&1){
        bytesofline++;
    }
    if(bytesofline>BMP_MAX_WIDTH){
        bytesofline=BMP_MAX_WIDTH;
    }
    if(1){ /*lint !e506 !e774*/
        for(i=0; i<height; i++,y++){
            XMSget(bmpinfo->handle, 
                  (void *)BMPCACHE, 
                  (void *)((INT32U)(INT32S)i<<10), 
                  (INT32S)bytesofline);
            gdi_draw_h_image(x, y, x2, BMPCACHE);
        }
    }
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    GetBmpLine()
 * ˵��:    ȡBMPͼ���е�һ��ͼ��
 * ����:    bmpinfo BMPINFO����ָ��
 *          lineno  �ڼ���,0=��1��
 * ���:    ͼ��Ŀ��(����)
**--------------------------------------------------------------------------------------------*/
INT16S apical GetBmpLine(BMPINFO * bmpinfo, INT16S lineno)
{
    if(!bmpinfo){
        return fail;
    }
    if(bmpinfo->ready!=FAMES_BMP_READY_YES){
        return fail;
    }
    if(bmpinfo->handle==0){
        return fail;
    }
    XMSget(bmpinfo->handle, (void *)BMPCACHE, 
          (void *)((INT32U)(INT32S)lineno<<10), (INT32S)BMP_MAX_WIDTH);
    return bmpinfo->width;
}

/*----------------------------------------------------------------------------------------------
 * ����:    SetBmpRect()
 * ˵��:    ����BMPINFO������������߶�
 * ����:    bmpinfo   BMPINFO����ָ��
 *          maxwidth  ������ʾ���(0~BMP_MAX_WIDTH)
 *          maxheight ������ʾ�߶�(0~BMP_MAX_HEIGHT)
**--------------------------------------------------------------------------------------------*/
BOOLEAN apical SetBmpRect(BMPINFO * bmpinfo, INT16S maxwidth, INT16S maxheight)
{
    if(!bmpinfo){
        return fail;
    }
    if(bmpinfo->ready!=FAMES_BMP_READY_YES){
        return fail;
    }
    if(maxwidth<0){
        return fail;
    }
    if(maxheight<0){
        return fail;
    }
    if(maxwidth>BMP_MAX_WIDTH){
        maxwidth=BMP_MAX_WIDTH;
    }
    if(maxheight>BMP_MAX_HEIGHT){
        maxheight=BMP_MAX_HEIGHT;
    }
    bmpinfo->maxwidth =maxwidth;
    bmpinfo->maxheight=maxheight;
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    GetBmpPalette()
 * ˵��:    ȡָ��BMP�ļ��еĵ�ɫ��
 * ����:    palette ��ɫ�建����
 *          bmpfile BMP�ļ��� 
 * ע��:    ������ֻ�ǽ�BMP�ļ��еĵ�ɫ��ԭ�ⲻ���ķŵ�palette��ȥ,
 *          ����Ҫ��palette����һ��Ҫ����1024�ֽ�,���򽫻�õ�����Ԥ�ϵĽ��.
**--------------------------------------------------------------------------------------------*/
BOOLEAN apical GetBmpPalette(INT08U palette[], FILENAME bmpfile)
{
    int  fd;

    if(!palette){
        return fail;
    }
    if(!bmpfile){
        return fail;
    }
    DispatchLock();
    fd=open(bmpfile,O_RDONLY|O_BINARY);/*lint !e569*/
    if(fd < 0){
        DispatchUnlock();
        return fail;
    }
    read(fd, palette, sizeof(BmpCoreInfo));
    read(fd, palette, sizeof(BmpInfoHeader));
    read(fd, palette, 1024);
    close(fd);
    DispatchUnlock();
    return ok;
}

#endif                                        /* #if FAMES_BMP_EN==1 */

/*==============================================================================================
 * 
 * ���ļ�����: bmp.c
 * 
**============================================================================================*/

