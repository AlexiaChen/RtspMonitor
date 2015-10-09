#ifndef MONITORWINDOWMANAGER_H
#define MONITORWINDOWMANAGER_H

#include <QObject>
#include <QGridLayout>
#include <QList>

class MonitorWindow;

//监视窗口管理器，设置监视窗口的行列布局
class MonitorWindowManager : public QGridLayout
{
	Q_OBJECT

public:
	//初始化监视窗口布局，并设置行列数
	MonitorWindowManager(int row, int column, QWidget *parent);
	~MonitorWindowManager();
	//返回监视窗口数量
	int numOfWindows() const;
	//设置窗口行列布局
	void setMonitorLayout(int row, int column);
	//根据位置得到监视窗口的实例指针
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
