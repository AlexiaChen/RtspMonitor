#ifndef MONITORWINDOWMANAGER_H
#define MONITORWINDOWMANAGER_H

#include <QObject>
#include <QGridLayout>
#include <QList>

class MonitorWindow;

//���Ӵ��ڹ����������ü��Ӵ��ڵ����в���
class MonitorWindowManager : public QGridLayout
{
	Q_OBJECT

public:
	//��ʼ�����Ӵ��ڲ��֣�������������
	MonitorWindowManager(int row, int column, QWidget *parent);
	~MonitorWindowManager();
	//���ؼ��Ӵ�������
	int numOfWindows() const;
	//���ô������в���
	void setMonitorLayout(int row, int column);
	//����λ�õõ����Ӵ��ڵ�ʵ��ָ��
	MonitorWindow* getItemAtPosition(int row, int column);

private:
	void remove(int row, int column, bool deleteWidgets);
	void deleteChildWidgets(QLayoutItem *item);

private:
	QWidget* m_parent;
	int m_numOfWindows;
	int m_row;
	int m_column;
	
};

#endif // MONITORWINDOWMANAGER_H
