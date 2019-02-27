#include "qwidget_tool.h"
#include "QGraphicsEffect"
#include "QFile"
#include "QLabel"
#include "QAbstractItemDelegate"
#include "QImage"
#include "QPixmap"
#include "QPainter"
#include "QWidget"
#include "QColor"

void QWidgetTool::SetShadow(int posx, int posy, int blur_radios, QColor color, QWidget *qwidget)
{
	QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(qwidget);
	shadow_effect->setOffset(posx, posy);
	shadow_effect->setBlurRadius(blur_radios);
	shadow_effect->setColor(color);
	qwidget->setGraphicsEffect(shadow_effect);
}

void QWidgetTool::SetOpacity(QLabel *&label, double Opacity)
{
	QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(label);
	//设定透明值
	effect->setOpacity(Opacity);
	label->setGraphicsEffect(effect);

}

void QWidgetTool::SetDotInsteadText(QLabel *label, int max_width)
{
	QString str = label->text();
	QString str_elided = label->fontMetrics().elidedText(str, Qt::ElideRight, max_width, Qt::TextShowMnemonic);
	label->setText(str_elided);
}

void QWidgetTool::DrawRadius(QWidget* qwidget, int x_radius, int y_radius)
{
	QPainter painter(qwidget);
	painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
	painter.setBrush(QBrush(Qt::white));
	painter.setPen(Qt::transparent);
	QRect rect = qwidget->rect();
	rect.setWidth(rect.width() - 1);
	rect.setHeight(rect.height() - 1);
	painter.drawRoundedRect(rect, x_radius, y_radius);
	//也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
	QStyleOption opt;
	opt.init(qwidget);
	QPainter p(qwidget);
	qwidget->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, qwidget);
}

void QWidgetTool::DrawRadiusAndLineGradient(QWidget* qwidget, int x_radius, int y_radius, int width, std::string start_color, std::string end_color, int start_pos, int height)
{
	QPainter painter(qwidget);
	painter.setRenderHint(QPainter::Antialiasing,true);  // 反锯齿;

	QLinearGradient linearGradient(-1, -1, width + 2, 40 + 2);
	QColor start(QString::fromStdString(start_color));
	QColor end(QString::fromStdString(end_color));
	linearGradient.setColorAt(0, start);
	linearGradient.setColorAt(1, end);
	//将直线开始点设为0，终点设为1，然后分段设置颜色
	painter.setBrush(linearGradient);
	painter.setPen(Qt::transparent);
	QRect rect = qwidget->rect();
	rect.setTop(start_pos);
	rect.setWidth(rect.width() - 1);
	rect.setHeight(height - 1);
	painter.drawRoundedRect(rect, x_radius, y_radius);
	//也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
	QStyleOption opt;
	opt.init(qwidget);
	QPainter p(qwidget);
	qwidget->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, qwidget);
}

void QWidgetTool::DrawRect(QWidget* qwidget, QRect rect,QColor color)
{
	QPainter painter(qwidget);
	painter.setRenderHint(QPainter::Antialiasing, true);  // 反锯齿;
	painter.setBrush(color);
	painter.setPen(Qt::transparent);
	painter.drawRect(rect);

	QStyleOption opt;
	opt.init(qwidget);
	QPainter p(qwidget);
	qwidget->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, qwidget);
}

QPixmap QWidgetTool::NinePatch(QString& picName, int iHorzSplit, int iVertSplit, int DstWidth, int DstHeight)
{
	//修复了无法使用透明色的错误
	QImage* pix = new QImage(picName);

	int pixWidth = pix->width();
	int pixHeight = pix->height();

	QImage pix_1 = pix->copy(0, 0, iHorzSplit, iVertSplit);
	QImage pix_2 = pix->copy(iHorzSplit, 0, pixWidth - iHorzSplit * 2, iVertSplit);
	QImage pix_3 = pix->copy(pixWidth - iHorzSplit, 0, iHorzSplit, iVertSplit);

	QImage pix_4 = pix->copy(0, iVertSplit, iHorzSplit, pixHeight - iVertSplit * 2);
	QImage pix_5 = pix->copy(iHorzSplit, iVertSplit, pixWidth - iHorzSplit * 2, pixHeight - iVertSplit * 2);
	QImage pix_6 = pix->copy(pixWidth - iHorzSplit, iVertSplit, iHorzSplit, pixHeight - iVertSplit * 2);

	QImage pix_7 = pix->copy(0, pixHeight - iVertSplit, iHorzSplit, iVertSplit);
	//网上此处代码pix_8出错,已修正
	QImage pix_8 = pix->copy(iHorzSplit, pixHeight - iVertSplit, pixWidth - iHorzSplit * 2, iVertSplit);
	QImage pix_9 = pix->copy(pixWidth - iHorzSplit, pixHeight - iVertSplit, iHorzSplit, iVertSplit);


	pix_2 = pix_2.scaled(DstWidth - iHorzSplit * 2, iVertSplit, Qt::IgnoreAspectRatio);//保持高度拉宽  
	pix_4 = pix_4.scaled(iHorzSplit, DstHeight - iVertSplit * 2, Qt::IgnoreAspectRatio);//保持宽度拉高  
	pix_5 = pix_5.scaled(DstWidth - iHorzSplit * 2, DstHeight - iVertSplit * 2, Qt::IgnoreAspectRatio);//宽高都缩放  
	pix_6 = pix_6.scaled(iHorzSplit, DstHeight - iVertSplit * 2, Qt::IgnoreAspectRatio);//保持宽度拉高  
	pix_8 = pix_8.scaled(DstWidth - iHorzSplit * 2, iVertSplit, Qt::IgnoreAspectRatio);//保持高度拉宽  
	QImage* resultImg = new QImage(DstWidth, DstHeight, QImage::Format_ARGB32);
	QPainter* painter = new QPainter(resultImg);

	painter->setCompositionMode(QPainter::CompositionMode_Source);
	if (!resultImg->isNull()) {
		painter->drawImage(0, 0, pix_1);
		painter->drawImage(iHorzSplit, 0, pix_2);
		painter->drawImage(DstWidth - iHorzSplit, 0, pix_3);

		painter->drawImage(0, iVertSplit, pix_4);
		painter->drawImage(iHorzSplit, iVertSplit, pix_5);
		painter->drawImage(DstWidth - iHorzSplit, iVertSplit, pix_6);

		painter->drawImage(0, DstHeight - iVertSplit, pix_7);
		painter->drawImage(iHorzSplit, DstHeight - iVertSplit, pix_8);
		painter->drawImage(DstWidth - iHorzSplit, DstHeight - iVertSplit, pix_9);
		painter->end();
	}
	QPixmap qp = QPixmap::fromImage(*resultImg);
	return qp;
}

void QWidgetTool::SetToWhite(QPixmap qPixmap)
{
	QImage image = qPixmap.toImage();
	image = image.convertToFormat(QImage::Format_ARGB32);
	union myrgb
	{
		uint rgba;
		uchar rgba_bits[4];
	};
	myrgb* mybits = (myrgb*)image.bits();
	int len = image.width()*image.height();
	while (len-- > 0)
	{
		mybits->rgba_bits[3] = (mybits->rgba == 0xFFFFFFFF) ? 0 : 255;
		mybits++;
	}
	
}

void QWidgetTool::PaintText(QLabel *&label, QString qs)
{
	QPainter painter(label);
	//设置画刷模式
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	//改变画笔和字体
	QFont font = painter.font();
	font.setBold(false);//加粗
	font.setPixelSize(10);//改变字体大小
	painter.setFont(font);
	//将文字绘制在图片中心位置
	int sum_x = 0;
	int sum_y = 0;
	QRectF ff(20, 20, 260, 2000);

	painter.drawRect(ff);
	QTextOption option(Qt::AlignLeft | Qt::AlignTop);
	option.setWrapMode(QTextOption::WordWrap);
	painter.drawText(ff, qs, option);
	label->adjustSize();
}

int QWidgetTool::CalculateBreakLine(QString &text, int padding, int font_size, int max_width)
{
	QFont font;
	font.setFamily("Microsoft YaHei");
	font.setPixelSize(font_size);
	QFontMetrics font_metrics(font);
	int sum = 0;
	int max_sum = 0;
	
	for (int i = 0; i < text.size(); i++)
	{
		if (text[i] == '\n')
		{
			max_sum = (max_sum > sum) ? max_sum : sum;
			sum = 0;
		}
		else
		{
			if (text[i] == 65532)
			{
				//65535为表情
				sum += 26;
			}
			else
			{
				sum += font_metrics.charWidth(text, i);
			}
			max_sum = (max_sum > sum) ? max_sum : sum;
			//应对英文字母不换行，且下一个字符不是最后一个字符。则强行给一个换行符
			if (sum >= max_width&&i + 1 != text.size() && text[i + 1] != '\n')
			{
				text.insert(i + 1, '\n');
			}
		}
	}
	if (padding == 0)
	{
		return max_sum;
	}
	else
	{
		return max_sum + padding + 8;
	}
}


int QWidgetTool::CalculateFirstBreakLine(QString &text, int padding, int font_size, int max_width)
{
	QFont font;
	font.setFamily("Microsoft YaHei");
	font.setPixelSize(font_size);
	QFontMetrics font_metrics(font);
	int sum = 0;
	int max_sum = 0;
	for (int i = 0; i < text.size(); i++)
	{
		if (text[i] == '\n')
		{
			max_sum = (max_sum > sum) ? max_sum : sum;
			break;
			sum = 0;
		}
		else
		{
			if (text[i] == 65532)
			{
				//65535为表情
				sum += 26;
			}
			else
			{
				sum += font_metrics.charWidth(text, i);
			}
			max_sum = (max_sum > sum) ? max_sum : sum;
			//应对英文字母不换行，且下一个字符不是最后一个字符。则强行给一个换行符
			if (sum >= max_width&&i + 1 != text.size() && text[i + 1] != '\n')
			{
				text.insert(i + 1, '\n');
				break;
			}
		}
	}
	if (padding == 0)
	{
		return max_sum;
	}
	else
	{
		return max_sum + padding + 8;
	}
}

QString QWidgetTool::SetSingleLineRemarks(const std::wstring& remarks, int padding, int font_size, int max_width, bool b_bold)
{
	QFont font;
	font.setFamily("Microsoft YaHei");
	font.setPixelSize(font_size);
	font.setBold(b_bold);
	QFontMetrics font_metrics(font);

	QString q_str = QString::fromStdWString(remarks);
	QString str_res = "";
	QString str_dot = "...";
	int sum = font_metrics.width(str_dot);
	int max_sum = max_width - 8 - padding;
	for(int i = 0; i < q_str.size(); ++i)
	{
		if(q_str[i] == '\n')
		{
			break;
		}

		sum += font_metrics.charWidth(q_str, i);
		if(sum >= max_sum)
		{
			str_res += str_dot;
			break;
		}
		str_res += q_str[i];
	}
	return str_res;
}

void QWidgetTool::CalculateScaleInfo(int &width, int &height, int max_width, int max_height)
{
	double ratio = (double)height / width;
	if (max_width >= max_height)
	{
		if (width >= max_width)
		{
			width = max_width;
			height = max_width*ratio;
		}
		if (height >= max_height)
		{
			height = max_height;
			width = height / ratio;
		}
	}
	else
	{
		if (height >= max_height)
		{
			height = max_height;
			width = height / ratio;
		}
		if (width >= max_width)
		{
			width = max_width;
			height = max_width* ratio;
		}
	}
}
