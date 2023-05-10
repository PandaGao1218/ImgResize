#include "pch.h" 
#include "my_algo.h"

int getImageAreaScore(unsigned char* imgBuf, int width, int height) {
//35, 
#define X_BLOCK_1		(10)
#define Y_BLOCK_1		(10)
#define	OFFSET1_1		(20)	//for black, gray unit
#define	OFFSET2_1		(0)		//for white, gray unit
#define	THRES1_1		(30)//(10)	//exist finger?, percent of black
#define	THRES2_1		(95)	//wet, percent of black
#define	THRES3_1		(90)	//dry, gray value
#define	BAD_COFF_1		(0)		//weight
	int w, h;
	int hist[256];

	int i, m, x, y, mx, my, c0, c1;//, c2;
	int valid = 0, dry = 0, wet = 0, black, white, ret = 0;
	int WIDTH_SUB;
	int HEIGHT_SUB;

	w = 256;
	h = 360;

	WIDTH_SUB = w / X_BLOCK_1;
	HEIGHT_SUB = h / Y_BLOCK_1;


	for (i = 0; i < 256; i++) hist[i] = 0;

	for (i = 0; i < w * h; i++) {
		m = imgBuf[i];
		hist[m] ++;
	}

	mx = my = 0;
	for (i = 0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0)					//all black
	{
		ret = 0;
		goto go_end;
	}
	c0 = mx / my;						//center gray value

	mx = my = 0;
	for (i = 0; i < c0; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0) {					//all white
		ret = 0;
		goto go_end;
	}
	else c1 = mx / my;				//ridge gray value
	if (c1 == 0)					//almost black
	{
		ret = 0;
		goto go_end;
	}

	mx = my = 0;
	for (i = c0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0)					//almost black
	{
		ret = 0;
		goto go_end;
	}
	//else c2 = mx / my;			//valley gray value

	for (x = 0; x < X_BLOCK_1; x++)
	{
		for (y = 0; y < Y_BLOCK_1; y++)
		{
			black = 0; white = 0;
			for (mx = 0; mx < WIDTH_SUB; mx++)
			{
				for (my = 0; my < HEIGHT_SUB; my++)
				{
					m = imgBuf[(y * HEIGHT_SUB + my) * w + x * WIDTH_SUB + mx];
					if (m < (c0 - OFFSET1_1)) black++;						//ridge
					else if (m > (c0 + OFFSET2_1)) white++;					//valley or background
				}
			}
			if (black > (WIDTH_SUB * HEIGHT_SUB * THRES1_1 / 100))					//exist finger
			{
				if (black > WIDTH_SUB * HEIGHT_SUB * THRES2_1 / 100) wet++;			//wet
				else if (white > WIDTH_SUB * HEIGHT_SUB * THRES3_1 / 100) dry++;	//dry
				else valid++;
			}
		}
	}

	m = max(wet, dry);
	valid -= (BAD_COFF_1 * m);
	valid = max(0, valid);
	ret = valid * 100 / (X_BLOCK_1 * Y_BLOCK_1);
	/*
		for (i = 0; i < 256; i ++ ) hist[i] = 0;
		cex = ((width -2*0)/2) / PBK;
		cey = ((height-2*0)/2) / PBK;

		for ( k = 0; k < 3; k ++ ) {
			p = (k + 1) * (cey / 4);
			yy = 2 * p * PBK + 0;
			for ( q = 0; q < cex; q ++ ) {
				xx = 2 * q * PBK + 0;
				for ( i = 0; i < PBK; i ++ ) {
					y = yy + 2*i;
					for ( j = 0; j < PBK; j ++ ) {
						x = xx + 2*j;
						m = imgBuf [(y + (IM_CY-height)/2)*IM_CX+(x + (IM_CX-width)/2)];
						hist[m] ++;
					}
				}
			}
		}

		m1 = m2 = 0;
		for (i = 0; i < 256; i ++ ) {
			m1 += (i*hist[i]);
			m2 += (1*hist[i]);
		}
		if (m2==0) return 0;
		m = m1/m2;
		m1 = m2 = 0;
		for (i = 0; i < 256; i ++ ) {
			n = i - m; if (n<0) n = -n;
			m1 += (n*hist[i]);
			m2 += (1*hist[i]);
		}
		ret = m1/m2;
		ret = min(100,200*ret/128);
		n = 0;
		for (i = 0; i < m; i ++ ) n += hist[i];
		ret = min(100,ret*2*n/m2);
	*/
	/*
		#define PBK			4
		int i, j, k, p, q, x, y;
		int cex, cey, m, n;
		int ret, d, xx, yy;
		int cnt[3];
		int sum;
		cex = ((IM_CX -2*0)/2) / PBK;
		cey = ((IM_CY-2*0)/2) / PBK;
		 for ( k = 0; k < 3; k ++ ) {
			p = (k + 1) * (cey / 4);
			cnt[k] = 0;
			for ( q = 0; q < cex; q ++ ) {
				xx = 2 * q * PBK + 0;
				yy = 2 * p * PBK + 0;
				m = 0;
				for ( i = 0; i < PBK; i ++ ) {
					y = yy + 2*i;
					for ( j = 0; j < PBK; j ++ ) {
						x = xx + 2*j;
						m += imgBuf [y*IM_CX+x];
					}
				}
				m /= (PBK*PBK);
				n = 0;
				for ( i = 0; i < PBK; i ++ ) {
					y = yy + 2*i;
					for ( j = 0; j < PBK; j ++ ) {
						x =  xx + 2*j;
						d = m - imgBuf [y*IM_CX+x];
						if ( d < 0 ) d = -d;
						n += d;
					}
				}
				n /= (PBK*PBK);
				if (n>16) cnt[k] ++;
			}
		}
		n = cex;
		for ( k = 0; k < 3; k ++ ) {
			if ( n > cnt[k] ) n = cnt[k];
		}
		sum = 1 * cex;
		ret = 100 * n / sum;

		if ( ret > 100 ) ret = 100;
	*/

go_end:



	return ret;// 80;//45;
}

void MY_IsFinger(const unsigned char* image, int width, int height, int* gScore, int* aScore, int* wScore)
{
#define X_BLOCK		(10)
#define Y_BLOCK		(10)
#define	OFFSET1		(20)	//for black, gray unit
#define	OFFSET2		(0)		//for white, gray unit
#define	THRES1		(10)	//exist finger?, percent of black
#define	THRES2		(95)	//wet, percent of black
#define	BAD_COFF	(0)		//weight

	int WIDTH_SUB = width / X_BLOCK;
	int HEIGHT_SUB = height / Y_BLOCK;
	int hist[256];

	int i, m, x, y, mx, my, c0, c1;//, c2;
	int area = 0, valid = 0, dry = 0, wet = 0, black, white;

	for (i = 0; i < 256; i++) hist[i] = 0;

	for (i = 0; i < width * height; i++) {
		m = image[i];
		hist[m] ++;
	}

	mx = my = 0;
	for (i = 0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0)					//all black
	{
		*gScore = 0;
		*aScore = 100;
		*wScore = 100;				//0:normal, -100:dry, 100:wet 
		return;
	}
	c0 = mx / my;						//center gray value

	mx = my = 0;
	for (i = 0; i < c0; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0) {					//all white
		c1 = 0;
		*gScore = 0;
		*aScore = 0;
		*wScore = 0;				//0:normal, -100:dry, 100:wet 
		return;
	}
	else c1 = mx / my;				//ridge gray value
	if (c1 == 0)					//almost black
	{
		*gScore = 0;
		*aScore = 100;
		*wScore = 100;
		return;
	}

	/*
	mx = my = 0;
	for (i = c0; i < 256;i ++) {
		mx += (i*hist[i]);
		my += (1*hist[i]);
	}
	if (my == 0) c2 = 255;
	else c2 = mx / my;				//valley gray value
	*/

	for (x = 0; x < X_BLOCK; x++)
	{
		for (y = 0; y < Y_BLOCK; y++)
		{
			black = 0; white = 0;
			for (mx = 0; mx < WIDTH_SUB; mx++)
			{
				for (my = 0; my < HEIGHT_SUB; my++)
				{
					m = image[(y * HEIGHT_SUB + my) * width + x * WIDTH_SUB + mx];
					if (m < (c0 - OFFSET1)) black++;						//ridge
					else if (m > (c0 + OFFSET2)) white++;					//valley or background
				}
			}
			if (black > (WIDTH_SUB * HEIGHT_SUB * THRES1 / 100))					//exist finger	
			{
				area++;
				if (black > WIDTH_SUB * HEIGHT_SUB * THRES2 / 100) wet++;			//wet
				else valid++;
			}
		}
	}

	m = max(wet, dry);
	valid -= (BAD_COFF * m);
	valid = max(0, valid);

	*gScore = valid * 100 / (X_BLOCK * Y_BLOCK);
	*aScore = area * 100 / (X_BLOCK * Y_BLOCK);
	if (dry > wet) m = -m;
	*wScore = m * 100 / (X_BLOCK * Y_BLOCK);
}

int My_IsFinger(const unsigned char* image, int width, int height)
{
	int i, j, k, p, q, x, y;
	int cex, cey, m, n;
	int ret, d, xx, yy;
	int cnt[3];
	int sum;
#define PBK	(4)

	cex = ((width - 2 * 0) / 2) / PBK;
	cey = ((height - 2 * 0) / 2) / PBK;
	for (k = 0; k < 3; k++) {
		p = (k + 1) * (cey / 4);
		cnt[k] = 0;
		for (q = 0; q < cex; q++) {
			xx = 2 * q * PBK + 0;
			yy = 2 * p * PBK + 0;
			m = 0;
			for (i = 0; i < PBK; i++) {
				y = yy + 2 * i;
				for (j = 0; j < PBK; j++) {
					x = xx + 2 * j;
					m += image[y * width + x];
				}
			}
			m /= (PBK * PBK);
			n = 0;
			for (i = 0; i < PBK; i++) {
				y = yy + 2 * i;
				for (j = 0; j < PBK; j++) {
					x = xx + 2 * j;
					d = m - image[y * width + x];
					if (d < 0) d = -d;
					n += d;
				}
			}
			n /= (PBK * PBK);
			if (n > 8) cnt[k] ++;
		}
	}
	n = cex;
	for (k = 0; k < 3; k++) {
		if (n > cnt[k]) n = cnt[k];
	}
	sum = 1 * cex;
	ret = 100 * n / sum;

	if (ret > 100) ret = 100;
	return ret;
}

void LAPI_IsFinger(unsigned char* image, int width, int height, int* gScore, int* aScore, int* wScore)
{
#define X_BLOCK		(10)
#define Y_BLOCK		(10)
#define GAP			(8)		//diff between rigde and valley
#define	THRES1		(30)	//exist finger?, percent of black
#define	THRES2		(90)	//wet, percent of black
#define	THRES3		(90)	//dry, gray value
#define	BAD_COFF	(0)		//weight

	int WIDTH_SUB = width / X_BLOCK;
	int HEIGHT_SUB = height / Y_BLOCK;
	int hist[256], off1, off2;

	int i, m, x, y, mx, my, c0, c1, c2;
	int area = 0, valid = 0, dry = 0, wet = 0, black, white;

	for (i = 0; i < 256; i++) hist[i] = 0;

	for (i = 0; i < width * height; i++) {
		m = image[i];
		hist[m] ++;
	}

	mx = my = 0;
	for (i = 0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0)					//all black
	{
		*gScore = 0;
		*aScore = 100;
		*wScore = 100;				//0:normal, -100:dry, 100:wet 
		return;
	}
	c0 = mx / my;						//center gray value

	mx = my = 0;
	for (i = 0; i < c0; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0) {					//all white
		c1 = 0;
		*gScore = 0;
		*aScore = 0;
		*wScore = 0;				//0:normal, -100:dry, 100:wet 
		return;
	}
	else c1 = mx / my;				//ridge gray value
	if (c1 == 0)					//almost black
	{
		*gScore = 0;
		*aScore = 100;
		*wScore = 100;
		return;
	}

	mx = my = 0;
	for (i = c0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0) c2 = 255;
	else c2 = mx / my;				//valley gray value

	if ((c2 - c1) < GAP)
	{
		*gScore = 0;
		*aScore = 0;
		*wScore = 0;				//0:normal, -100:dry, 100:wet 
		return;
	}

	off1 = (c0 - c1) / 2;
	off2 = (c2 - c0) / 2;

	for (x = 0; x < X_BLOCK; x++)
	{
		for (y = 0; y < Y_BLOCK; y++)
		{
			black = 0; white = 0;
			for (mx = 0; mx < WIDTH_SUB; mx++)
			{
				for (my = 0; my < HEIGHT_SUB; my++)
				{
					m = image[(y * HEIGHT_SUB + my) * width + x * WIDTH_SUB + mx];
					if (m < (c0 - off1)) black++;						//ridge
					else if (m > (c0 + off2)) white++;					//valley or background
				}
			}
			if (black > (WIDTH_SUB * HEIGHT_SUB * THRES1 / 100))					//exist finger	
			{
				area++;
				if (black > WIDTH_SUB * HEIGHT_SUB * THRES2 / 100) wet++;			//wet
				else if (white > WIDTH_SUB * HEIGHT_SUB * THRES3 / 100) dry++;	//dry
				else valid++;
			}
		}
	}

	m = max(wet, dry);
	valid -= (BAD_COFF * m);
	valid = max(0, valid);

	*gScore = valid * 100 / (X_BLOCK * Y_BLOCK);
	*aScore = area * 100 / (X_BLOCK * Y_BLOCK);
	if (dry > wet) m = -m;
	*wScore = m * 100 / (X_BLOCK * Y_BLOCK);
}

