#pragma once
#include "QWidget"
#include "QLabel"
#include "QPixmap"

class QWidgetTool
{
public:
	QWidgetTool(){};
	~QWidgetTool(){};
	//Label����չЧ��
	void SetShadow(int posx,int posy,int blur_radios,QColor color,QWidget *qwidget);	//������Ӱ
	void SetOpacity(QLabel *&label, double Opacity);	//����͸����
	void SetDotInsteadText(QLabel *label, int max_width);	//������...���泬�����ֲ���
	void DrawRadius(QWidget* qwidget,int x_radius,int y_radius);
	void DrawRadiusAndLineGradient(QWidget* qwidget, int x_radius, int y_radius, int width, std::string start_color, std::string end_color,int start_pos,int height);	//����Բ�Ǳ�������ɫ
	void DrawRect(QWidget* qwidget,QRect rect,QColor color);
	
	/*�Ź���
	��ͼƬ���ֳɾŸ����֣�ǰ����������ʾΪ���Ͻ�ˮƽ�ָ�����ϽǵĴ�ֱ�ָ
	�ĸ��ǵ�ֵ�ָ�ֵ�������Ͻ���ͬ��
	����������ΪĿ��ͼƬ�Ŀ�Ⱥ͸߶ȡ�
	����ͼƬ����
	*/
	QPixmap NinePatch(QString& picName, int iHorzSplit, int iVertSplit, int DstWidth, int DstHeight);
	void SetToWhite(QPixmap qPixmap);	//����Ϊ��ɫͼƬ
	void PaintText(QLabel *&label, QString qs);	//����

	int CalculateBreakLine(QString &text, int padding, int font_size, int max_width);	//���ַ�����max_widthʱ����,����ֵ��label�Ŀ��
	int CalculateFirstBreakLine(QString &text, int padding, int font_size, int max_width);//���ַ�����max_widthʱ����,����ֵ��label�Ŀ��,ֻ�Ե�һ�в���\n
	QString SetSingleLineRemarks(const std::wstring& remarks, int padding, int font_size, int max_width, bool b_bold = false);	//	��...�滻��ʾ�������ַ�
	void CalculateScaleInfo(int &width, int &height, int max_width, int max_height);

};
