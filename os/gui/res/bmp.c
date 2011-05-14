/***********************************************************************************************
** 文件:    bmp.c
**
** 说明:    BMP图象相关函数
**
** 作者:    Jun
**
** 时间:    2010-04-15
**
** 注意:    此版本只能处理"未压缩"的256色的BMP图片
***********************************************************************************************/
#define  FAMES_BMP_C
#include <includes.h>


#if     FAMES_BMP_EN == 1
/*----------------------------------------------------------------------------------------------
 * 函数:    InitBMPINFO()
 * 说明:    初始化BMPINFO结构 
 * 输入:    bmpinfo
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
 * 函数:    LoadBmp()
 * 说明:    加载BMP文件中的图象到XMS, 并对应设置BMPINFO结构
 * 输入:    bmpinfo BMPINFO对象指针
 *          bmpfile BMP文件名
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
 * 函数:    UnloadBmp()
 * 说明:    释放BMP文件所占的XMS内存块, 并对应设置BMPINFO结构
 * 输入:    bmpinfo BMPINFO对象指针
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
 * 函数:    ShowBmp()
 * 说明:    根据BMPINFO结构显示一幅图片
 * 输入:    x,y     图象在屏幕上的坐标(左上角)                 
 *          bmpinfo BMPINFO对象指针
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
 * 函数:    GetBmpLine()
 * 说明:    取BMP图象中的一行图象
 * 输入:    bmpinfo BMPINFO对象指针
 *          lineno  第几行,0=第1行
 * 输出:    图象的宽度(象素)
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
 * 函数:    SetBmpRect()
 * 说明:    设置BMPINFO的最大宽度与最大高度
 * 输入:    bmpinfo   BMPINFO对象指针
 *          maxwidth  最大可显示宽度(0~BMP_MAX_WIDTH)
 *          maxheight 最大可显示高度(0~BMP_MAX_HEIGHT)
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
 * 函数:    GetBmpPalette()
 * 说明:    取指定BMP文件中的调色板
 * 输入:    palette 调色板缓冲区
 *          bmpfile BMP文件名 
 * 注意:    本函数只是将BMP文件中的调色板原封不动的放到palette中去,
 *          所以要求palette缓冲一定要大于1024字节,否则将会得到不可预料的结果.
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
 * 本文件结束: bmp.c
 * 
**============================================================================================*/

