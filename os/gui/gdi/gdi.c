/***********************************************************************************************
 * �ļ�:    gdi.c
 *
 * ˵��:    ͼ���豸�ӿ�(GDI)
 *
 * ����:    Jun
 *
 * ʱ��:    2010-9-1
***********************************************************************************************/
#define  FAMES_GDI_C
#include <includes.h>

#if FAMES_GDI_EN == 1

/*------------------------------------------------------------------------------------
 * ����:    gdi_put_pixel()
 * ˵��:    ��ָ��λ�û�һ����
 * ����:    x       ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          color   �����ɫ
 * ���:    ok/fail
**----------------------------------------------------------------------------------*/
INT16S gdi_put_pixel(INT16S x, INT16S y, COLOR  color)
{
    if (gdc_is_point_visible(x, y))
        X_PUT_PIXEL(x, y, color);

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_get_pixel()
 * ˵��:    ��ָ��λ�ö�һ�������ɫ
 * ����:    x       ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 * ���:    color   �����ɫ
**----------------------------------------------------------------------------------*/
COLOR gdi_get_pixel(INT16S x, INT16S y)
{
    return X_GET_PIXEL(x, y);
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_h_line()
 * ˵��:    ��ˮƽ��
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          x2      ��β������(0~SCREEN.width-1)
 *          color   �ߵ���ɫ
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_h_line(INT16S x, INT16S y, INT16S x2, COLOR color)
{
    int vx1, vx2;
    GDC_value v;

    for (vx1 = x; vx1 <= x2;) {
        v = gdc_is_xline_visible(vx1, x2, y, &vx1, &vx2);
        if (v <= GDC_none)
            break;
        X_DRAW_H_LINE(vx1, y, vx2, color);
        vx1 = vx2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_v_line()
 * ˵��:    ����ֱ��
 * ����:    x       ������(0~SCREEN.width-1)
 *          y       ��ʼ������(0~SCREEN.height-1)
 *          y2      ��β������(0~SCREEN.height-1)
 *          color   �ߵ���ɫ
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_v_line(INT16S x, INT16S y, INT16S y2, COLOR color)
{
    int vy1, vy2;
    GDC_value v;

    for (vy1 = y; vy1 <= y2;) {
        v = gdc_is_yline_visible(vy1, y2, x, &vy1, &vy2);
        if (v <= GDC_none)
            break;
        X_DRAW_V_LINE(x, vy1, vy2, color);
        vy1 = vy2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_h_image()
 * ˵��:    ����ɫӳ��(IMAGE)ˮƽ����
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          x2      ��β������(0~SCREEN.width-1)
 *          colors  ��ɫӳ��(��ɫ����)
 * ע��:    ��������Ҫ������ʾͼ��(λͼ)
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_h_image(INT16S x, INT16S y, INT16S x2, COLOR colors[])
{
    int vx1, vx2;
    GDC_value v;

    for (vx1 = x; vx1 <= x2;) {
        v = gdc_is_xline_visible(vx1, x2, y, &vx1, &vx2);
        if (v <= GDC_none)
            break;
        X_DRAW_H_IMAGE(vx1, y, x2, colors + (vx1 - x)); /*lint !e679*/
        vx1 = vx2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_v_image()
 * ˵��:    ����ɫӳ��(IMAGE)��ֱ����
 * ����:    x       ������(0~SCREEN.width-1)
 *          y       ��ʼ������(0~SCREEN.height-1)
 *          y2      ��β������(0~SCREEN.height-1)
 *          colors  ��ɫӳ��(��ɫ����)
 * ע��:    ��������Ҫ������ʾͼ��(λͼ)
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_v_image(INT16S x, INT16S y, INT16S y2, COLOR colors[])
{
    int vy1, vy2;
    GDC_value v;

    for (vy1 = y; vy1 <= y2;) {
        v = gdc_is_yline_visible(vy1, y2, x, &vy1, &vy2);
        if (v <= GDC_none)
            break;
        X_DRAW_V_IMAGE(x, vy1, vy2, colors + (vy1 - y)); /*lint !e679*/
        vy1 = vy2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_h_bitmap()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ�û���,���򲻻�
 *          һ����໭8����
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          dots    ����ĸ���
 *          bitmap  һ���ֽڵ�λģʽ
 *          color   ��ɫ
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_h_bitmap(INT16S x, INT16S y, INT16S dots, INT08U bitmap, COLOR color)
{
    int vx1, vx2, x2, t, tdots;
    INT08U tbitmap;
    GDC_value v;

    if(dots > 8)
        dots=8;
    if(dots <= 0)
        return fail;

    x2 = x + dots - 1;

    for (vx1 = x; vx1 <= x2;) {
        v = gdc_is_xline_visible(vx1, x2, y, &vx1, &vx2);
        if (v <= GDC_none)
            break;
        t = vx1 - x;
        tdots = (vx2 - vx1) + 1;
        tbitmap = bitmap << t;
        tbitmap &= 0xff << (8 - tdots);
        X_DRAW_H_BITMAP(vx1, y, tdots, tbitmap, color);
        vx1 = vx2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����: gdi_draw_v_bitmap()
 * ˵��: ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ�û���,���򲻻�
 *       һ����໭8����
 * ����: x       ��ʼ������(0~SCREEN.width-1)
 *       y       ������(0~SCREEN.height-1)
 *       bitmap  һ���ֽڵ�λģʽ
 *       color   ��ɫ
 * ��;: �������ɴ��ӿ��������ʾ, �����ڻ�����
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_v_bitmap(INT16S x, INT16S y, INT08U bitmap, COLOR color)
{
    int vy1, vy2, y2, t, tdots;
    INT08U tbitmap;
    GDC_value v;

    y2 = y + 7;

    for (vy1 = y; vy1 <= y2;) {
        v = gdc_is_yline_visible(vy1, y2, x, &vy1, &vy2);
        if (v <= GDC_none)
            break;
        t = vy1 - y;
        tdots = (vy2 - vy1) + 1;
        tbitmap = bitmap << t;
        tbitmap &= 0xff << (8 - tdots);
        X_DRAW_V_BITMAP(x, vy1, tbitmap, color);
        vy1 = vy2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_h_bitmap_bg()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ����color����,
 *          ������bgcolor����, һ����໭8����.
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          dots    ����ĸ���
 *          bitmap  һ���ֽڵ�λģʽ
 *          color   ��ɫ
 *          bgcolor ������ɫ
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
 *          ����Ҫ������ɫ��ʱ��ǳ�����
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_h_bitmap_bg(INT16S x,
                           INT16S y,
                           INT16S dots,
                           INT08U bitmap,
                           COLOR  color,
                           COLOR  bgcolor
                          )
{
    int vx1, vx2, x2, t, tdots;
    INT08U tbitmap;
    GDC_value v;

    if(dots > 8)
        dots=8;
    if(dots <= 0)
        return fail;

    x2 = x + dots - 1;

    for (vx1 = x; vx1 <= x2;) {
        v = gdc_is_xline_visible(vx1, x2, y, &vx1, &vx2);
        if (v <= GDC_none)
            break;
        t = vx1 - x;
        tdots = (vx2 - vx1) + 1;
        tbitmap = bitmap << t;
        tbitmap &= 0xff << (8 - tdots);
        X_DRAW_H_BITMAP_BG(vx1, y, tdots, tbitmap, color, bgcolor);
        vx1 = vx2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_v_bitmap_bg()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ����color����,
 *          ������bgcolor����, һ����໭8����.
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          bitmap  һ���ֽڵ�λģʽ
 *          color   ��ɫ
 *          bgcolor ������ɫ
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
 *          ����Ҫ������ɫ��ʱ��ǳ�����
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_v_bitmap_bg(INT16S x, INT16S y, INT08U bitmap, COLOR color, COLOR bgcolor)
{
    int vy1, vy2, y2, t, tdots;
    INT08U tbitmap;
    GDC_value v;

    y2 = y + 7;

    for (vy1 = y; vy1 <= y2;) {
        v = gdc_is_yline_visible(vy1, y2, x, &vy1, &vy2);
        if (v <= GDC_none)
            break;
        t = vy1 - y;
        tdots = (vy2 - vy1) + 1;
        tbitmap = bitmap << t;
        tbitmap &= 0xff << (8 - tdots);
        X_DRAW_V_BITMAP_BG(x, vy1, tbitmap, color, bgcolor);
        vy1 = vy2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_h_bitmap_bgmap()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ����color����,
 *          ������bgcolors�е���ɫ����, һ����໭8����.
 * ����:    x        ��ʼ������(0~SCREEN.width-1)
 *          y        ������(0~SCREEN.height-1)
 *          bitmap   һ���ֽڵ�λģʽ
 *          color    ��ɫ
 *          bgcolors ������ɫ
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
 *          ����Ҫ������ͼ�ε�ʱ��ǳ�����
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_h_bitmap_bgmap(INT16S x, INT16S y, INT16S dots, INT08U bitmap, COLOR color, COLOR bgcolors[])
{
    int vx1, vx2, x2, t, tdots;
    INT08U tbitmap;
    GDC_value v;

    if(dots > 8)
        dots=8;
    if(dots <= 0)
        return fail;

    x2 = x + dots - 1;

    for (vx1 = x; vx1 <= x2;) {
        v = gdc_is_xline_visible(vx1, x2, y, &vx1, &vx2);
        if (v <= GDC_none)
            break;
        t = vx1 - x;
        tdots = (vx2 - vx1) + 1;
        tbitmap = bitmap << t;
        tbitmap &= 0xff << (8 - tdots);
        X_DRAW_H_BITMAP_BGMAP(vx1, y, tdots, tbitmap, color, bgcolors + (vx1 - x)); /*lint !e679*/
        vx1 = vx2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_v_bitmap_bgmap()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ����color����,
 *          ������bgcolors�е���ɫ����, һ����໭8����.
 * ����:    x        ��ʼ������(0~SCREEN.width-1)
 *          y        ������(0~SCREEN.height-1)
 *          bitmap   һ���ֽڵ�λģʽ
 *          color    ��ɫ
 *          bgcolors ������ɫ
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
 *          ����Ҫ������ͼ�ε�ʱ��ǳ�����
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_v_bitmap_bgmap(INT16S x, INT16S y, INT08U bitmap, COLOR color, COLOR bgcolors[])
{
    int vy1, vy2, y2, t, tdots;
    INT08U tbitmap;
    GDC_value v;

    y2 = y + 7;

    for (vy1 = y; vy1 <= y2;) {
        v = gdc_is_yline_visible(vy1, y2, x, &vy1, &vy2);
        if (v <= GDC_none)
            break;
        t = vy1 - y;
        tdots = (vy2 - vy1) + 1;
        tbitmap = bitmap << t;
        tbitmap &= 0xff << (8 - tdots);
        X_DRAW_V_BITMAP_BGMAP(x, vy1, tbitmap, color, bgcolors + (vy1 - y)); /*lint !e679*/
        vy1 = vy2 + 1;
        if (v == GDC_whole)
            break;
    };

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_rect()
 * ˵��:    ������
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ��ʼ������(0~SCREEN.height-1)
 *          x2      ��β������(0~SCREEN.width-1)
 *          y2      ��β������(0~SCREEN.height-1)
 *          color   �ߵ���ɫ
 * ���:    ok/fail
**----------------------------------------------------------------------------------*/
INT16S  gdi_draw_rect(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color)
{
    if (x > x2) {
        x += x2;
        x2 = x-x2;
        x -= x2;
    }
    if ( y > y2) {
        y += y2;
        y2 = y-y2;
        y -= y2;
    }
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    gdi_draw_v_line(x , y , y2, color);
    gdi_draw_v_line(x2, y , y2, color);
    gdi_draw_h_line(x , y , x2, color);
    gdi_draw_h_line(x , y2, x2, color);

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gdi_draw_box()
 * ˵��:    ��������
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ��ʼ������(0~SCREEN.height-1)
 *          x2      ��β������(0~SCREEN.width-1)
 *          y2      ��β������(0~SCREEN.height-1)
 *          color   �ߵ���ɫ
**----------------------------------------------------------------------------------*/
INT16S gdi_draw_box( INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color)
{
    if (x > x2) {
        x += x2;
        x2 = x-x2;
        x -= x2;
    }
    if (y > y2) {
        y += y2;
        y2 = y-y2;
        y -= y2;
    }
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    for(;y<=y2; y++){
        gdi_draw_h_line(x, y, x2, color);
    }

    return ok;
}

INT16S gdi_draw_line(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR color)
{
/* �˺���Ҫ�ŵ�v_lib��
int line(int _x, int _y, int _x2, int _y2, COLOR * color)
{
	int i, w, h, x, y, x2, y2;

	//printf("line(%d, %d, %d, %d) called\n", _x, _y, _x2, _y2);

	x = _x;  x2 = _x2;
	y = _y;  y2 = _y2;

	if(_x > _x2) { //ȷ�� x < x2
		x = _x2;
		x2 = _x;
	}
	if(_y > _y2) { //ȷ�� y < y2
		y = _y2;
		y2 = _y;
	}

	if(x == x2) { //����
		for(i = y; i <= y2; i++)
			putpixel(x, i, color);
		return 0;
	}
	if(y == y2) { //����
		for(i = x; i <= x2; i++)
			putpixel(i, y, color);
		return 0;
	}

	h = my_abs(y - y2) + 1;
	w = my_abs(x - x2) + 1;

	//printf("line(%d, %d, %d, %d) h=%d, w=%d\n", x, y, x2, y2, h, w);

	if(h <= w) { //б��<=1, ��б�Ƕ�<=45��
		int temp_y;
		if( (_x - _x2) / (_y - _y2) > 0) { //���Ͻǵ����½�
			temp_y = y;
			for(i = 0; i < w; i++) {
				putpixel(i + x, (temp_y + (i * h / w)), color);
			}
		} else { //���½ǵ����Ͻ�
			temp_y = y2;
			for(i = 0; i < w; i++) {
				putpixel(i + x, (temp_y - (i * h / w)), color);
			}
		}
	} else { //б��>1, ��б�Ƕ�>45��
		int temp_x;
		if( (_y - _y2) / (_x - _x2) > 0) { //���Ͻǵ����½�
			temp_x = x;
			for(i = 0; i < h; i++) {
				putpixel((temp_x + (i * w / h)), i + y, color);
			}
		} else { //���½ǵ����Ͻ�
			temp_x = x2;
			for(i = 0; i < h; i++) {
				putpixel((temp_x - (i * w / h)), i + y, color);
			}
		}
	}

	return 0;
}
*/
}

#endif /* FAMES_GDI_EN == 1 */


/*==============================================================================================
 * 
 * ���ļ�����: gdi.c
 * 
**============================================================================================*/

