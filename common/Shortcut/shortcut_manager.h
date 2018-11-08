#pragma once
#include "base\memory\singleton.h"
#include <string>
#include <QObject>

//��ݼ�
class ShortCutManager : public QObject
{
	Q_OBJECT
public:
	SINGLETON_DEFINE(ShortCutManager);
	ShortCutManager();
	~ShortCutManager();

	bool ParseCommand(std::string command_str);

signals:
	void SignalMockHeartStart();   //ģ���Ͽ�������ʼ
	void SignalMockHeartStop();		//ģ���Ͽ���������
	void SignalMockAgoraLogout();	//ģ�������ǳ�
	void SignalMockServerHeartStop();//�Ͽι��̸������������������ӶϿ�
	void SignalMockServerHeartStart();//�Ͽι��̸�������������������
	void SignalMockOpenClassroom(std::string);

	void SignalO2OPlayback(unsigned long long course_id);
	void SignalO2MPlayback(unsigned long long course_id);
	void SignalReadingSetting(bool);

protected:
private:
};