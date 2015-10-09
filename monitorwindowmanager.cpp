#include "monitorwindowmanager.h"

#include "monitorwindow.h"

MonitorWindowManager::MonitorWindowManager(int row, int column, QWidget *parent)
	:QGridLayout(parent)
{
	m_numOfWindows = row*column;
	m_row = row;
	m_column = column;
	m_parent = parent;
	
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < column; ++j)
		{
			this->addWidget(new MonitorWindow(parent),i,j);
		}
	}
}

MonitorWindowManager::~MonitorWindowManager()
{
	
}

int MonitorWindowManager::numOfWindows() const
{
	return m_numOfWindows;
}

void MonitorWindowManager::setMonitorLayout(int row, int column)
{
	m_numOfWindows = row*column;
	
	// 删除并重新创建监视窗口

	for (int i = 0; i < m_row; ++i)
	{
		for (int j = 0; j < m_column; ++j)
		{
			remove(i, j, true);
		}
	}

	
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < column; ++j)
		{
			this->addWidget(new MonitorWindow(m_parent), i, j);
		}
	}


}

void MonitorWindowManager::remove(int row, int column, bool deleteWidgets)
{
	for (int i = this->count() - 1; i >= 0; i--) 
	{
		int r, c, rs, cs;
		this->getItemPosition(i, &r, &c, &rs, &cs);
		if ((r <= row && r + rs - 1 >= row) || (c <= column && c + cs - 1 >= column)) 
		{
			// This layout item is subject to deletion.
			QLayoutItem *item = this->takeAt(i);
			if (deleteWidgets) 
			{
				deleteChildWidgets(item);
			}
			delete item;
		}
	}
}

void MonitorWindowManager::deleteChildWidgets(QLayoutItem *item)
{
	if (item->layout()) 
	{
		// Process all child items recursively.
		for (int i = 0; i < item->layout()->count(); i++) 
		{
			deleteChildWidgets(item->layout()->itemAt(i));
		}
	}
	delete item->widget();
}

MonitorWindow* MonitorWindowManager::getItemAtPosition(int row, int column)
{
	QLayoutItem* item = this->itemAtPosition(row, column);
	if (dynamic_cast<QWidgetItem*>(item))
	{
		return (MonitorWindow*)(item->widget());
	}

	return nullptr;
}


