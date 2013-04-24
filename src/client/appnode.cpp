/*
 *
 *  Portions Copyright (C) 2009 wind (lvlisong@redflag-linux.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <string>
#include <QtSql/QSqlQuery>
#include <QApplication>
#include "appnode.h"
#include "../localapps.h"
#include "../querythread.h"

#define DB_TABLE "softwares"

#define SQL_STATEMENT "SELECT name, version, brief, recommended, "\
                      "category, lastupdate, icon, uninstall, "\
                      "execname, privilege, os, updateinfo, "\
                      "comments, developer, source FROM softwares;"
/*TODO*/
NodeList* NodeList::S_NODELIST = NULL;

NodeList* NodeList::instance()
{
	if (NULL == S_NODELIST)
		S_NODELIST = new NodeList;
	return S_NODELIST;
}

NodeList::NodeList()
{
    initial();
}

NodeList::~NodeList()
{
    for (int i = 0; i < _nodes.count(); i++) {
        Node *n = _nodes.at(i);
        _nodes.remove(i);
        delete n;
    }
}

void NodeList::flush()
{
    /*
    delete S_NODELIST;
    S_NODELIST = NULL;
    */
    for (int i = 0; i < _nodes.count(); i++) {
        Node *n = _nodes.at(i);
        _nodes.remove(i);
        delete n;
    }
    _nodes.clear();
    initial();
}

void NodeList::initial()
{
    _finished = false;
    thread = new QueryThread("remote", SQL_STATEMENT);
    thread->start();
    connect(thread, SIGNAL(finished(int)), this, SLOT(onQueryFinished(int)));
    while (!_finished) {
        QApplication::processEvents();
    }
    QSqlQuery query = thread->result();
    while(query.next()) {
        Node *node = new Node;
        QString name = query.value(0).toString();
        node->setName(name);
        node->setVersion(query.value(1).toString());
        node->setBrief(query.value(2).toString());
        node->setRecommended(query.value(3).toBool());
        node->setCategory(static_cast<PackageCategory>(\
                            query.value(4).toInt()));
        node->setLastUpdate(query.value(5).toInt());
        node->setIcon(query.value(6).toString());
        node->setUninstName(query.value(7).toString());
        node->setExecName(query.value(8).toString());
        node->setPrivilege(query.value(9).toInt());
        node->setOS(query.value(10).toString());
        node->setUpdateInfo(query.value(11).toString());
        node->setComments(query.value(12).toString());
        node->setDeveloper(query.value(13).toString());
        node->setSource(query.value(14).toString());

        LocalApp *app = \
                LocalAppList::getList()->getAppByName(name);
        PackageStatus stat = UNINSTALLED;
        if (app) {
            if (node->lastUpdate() > app->date()) {
                stat = OLD;
            } else {
                stat = INSTALLED;
            }
        }
        node->setStatus(stat);
        _nodes.push_back(node);
    }
    delete thread;
    thread = NULL;
}

void NodeList::onQueryFinished(int error)
{
    _finished = true;
}
