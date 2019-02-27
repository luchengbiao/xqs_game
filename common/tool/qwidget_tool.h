#pragma once
#include "QWidget"
#include "QLabel"
#include "QPixmap"

class QWidgetTool
{
public:
	QWidgetTool(){};
	~QWidgetTool(){};
	//Label的拓展效果
	void SetShadow(int posx,int posy,int blur_radios,QColor color,QWidget *qwidget);	//设置阴影
	void SetOpacity(QLabel *&label, double Opacity);	//设置透明度
	void SetDotInsteadText(QLabel *label, int max_width);	//设置用...代替超出文字部分
	void DrawRadius(QWidget* qwidget,int x_radius,int y_radius);
	void DrawRadiusAndLineGradient(QWidget* qwidget, int x_radius, int y_radius, int width, std::string start_color, std::string end_color,int start_pos,int height);	//绘制圆角背景渐变色
	void DrawRect(QWidget* qwidget,QRect rect,QColor color);
	
	/*九宫格
	将图片划分成九个部分，前两个参数表示为左上角水平分割和左上角的垂直分割。
	四个角的值分割值都与左上角相同。
	后两个参数为目标图片的宽度和高度。
	返回图片本身
	*/
	QPixmap NinePatch(QString& picName, int iHorzSplit, int iVertSplit, int DstWidth, int DstHeight);
	void SetToWhite(QPixmap qPixmap);	//设置为灰色图片
	void PaintText(QLabel *&label, QString qs);	//画字

	int CalculateBreakLine(QString &text, int padding, int font_size, int max_width);	//当字符超过max_width时换行,返回值是label的宽度
	int CalculateFirstBreakLine(QString &text, int padding, int font_size, int max_width);//当字符超过max_width时换行,返回值是label的宽度,只对第一行插入\n
	QString SetSingleLineRemarks(const std::wstring& remarks, int padding, int font_size, int max_width, bool b_bold = false);	//	用...替换显示过长的字符
	void CalculateScaleInfo(int &width, int &height, int max_width, int max_height);

};
