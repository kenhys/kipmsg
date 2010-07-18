/***************************************************************************
 *   Copyright (C) 2006-2010 by nikikuni                                   *
 *   nikikuni@yahoo.co.jp                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qtextcodec.h>
#include <qlistwidget.h>
#include <qlabel.h>

#include <kdebug.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kcombobox.h>

#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

#include "kipmsgsettings.h"
#include "encodingconfig.h"

#define COL_USER 0
#define COL_GROUP 1
#define COL_IPADDR 3
#define COL_LOGIN 4
#define COL_ENCODING 5

/**
 * コンストラクタ
 * ・設定をロードし画面に表示
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgEncodingConfigDialog::KIPMsgEncodingConfigDialog(QWidget* parent, const char* name, Qt::WindowFlags fl)
//        : KIPMsgEncodingConfigDialogBase(parent,name,fl)
        : KIPMsgEncodingConfigDialogBase()
{
	kDebug() << "START KIPMsgEncodingConfigDialog::KIPMsgEncodingConfigDialog" << endl;
	setupUi(this);
	setButtons( None );

	KIpMessengerEncodingConfigModel *model = new KIpMessengerEncodingConfigModel(m_HostListView);
/*
	model->setHorizontalHeaderItem( 0, new QStandardItem( tr2i18n( "User" ) ) );
	model->setHorizontalHeaderItem( 1, new QStandardItem( tr2i18n( "Group" ) ) );
	model->setHorizontalHeaderItem( 2, new QStandardItem( tr2i18n( "Host" ) ) );
	model->setHorizontalHeaderItem( 3, new QStandardItem( tr2i18n( "IP address" ) ) );
	model->setHorizontalHeaderItem( 4, new QStandardItem( tr2i18n( "Login" ) ) );
	model->setHorizontalHeaderItem( 5, new QStandardItem( tr2i18n( "Encoding" ) ) );
*/
	m_HostListView->setModel( model );
	refreshEncoding();
//FIXME QListViewItemIterator it( m_HostListView );
//FIXME いる？
/*
	QListViewItemIterator it( m_HostListView );
	if ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		slotHostSelected( item );
	}
*/
	kDebug() << "END   KIPMsgEncodingConfigDialog::KIPMsgEncodingConfigDialog" << endl;
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIPMsgEncodingConfigDialog::~KIPMsgEncodingConfigDialog()
{}

/**
 * エンコーディングを再表示。
 * ・ホストリストを再取得し、設定されているエンコーディングとマージし画面に表示しなおす。
 */
void
KIPMsgEncodingConfigDialog::refreshEncoding()
{
	kDebug() << "START KIPMsgEncodingConfigDialog::refreshEncoding" << endl;
	KIpMessengerEncodingConfigModel *dataModel = (KIpMessengerEncodingConfigModel*)m_HostListView->model();
	QListIterator<QModelIndex> it = m_HostListView->selectionModel()->selectedRows();
//TESTME
	dataModel->clear();
//FIXME	m_HostListView->setRootIsDecorated(FALSE);
//	m_HostListView->setRootIsDecorated(FALSE);

	//ホストリストを再取得
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	HostList hosts = agent->GetHostList();
	QStringList encodings = KIpMsgSettings::encodingSettings();
	for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
		QTextCodec *codec;
		for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
			QStringList fields = ite->split( "|" );
			if ( QString( ix->IpAddress().c_str() ) == fields[0] && 
				QString( ix->UserName().c_str() ) == fields[1] ) {
				ix->setEncodingName( (const char*)fields[2].toAscii().data() );
				break;
			}
		}
		if ( ix->EncodingName() != "" ) {
			codec = QTextCodec::codecForName( ix->EncodingName().c_str() );
		} else {
			codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding().toAscii() );
		}
//TESTME
		KIpMessengerEncodingConfigEntry entry = KIpMessengerEncodingConfigEntry(
													codec->toUnicode( ix->Nickname().c_str() ),
													codec->toUnicode( ix->GroupName().c_str() ),
													codec->toUnicode( ix->HostName().c_str() ),
													codec->toUnicode( ix->IpAddress().c_str() ),
													codec->toUnicode( ix->UserName().c_str() ),
													codec->toUnicode( ix->EncodingName().c_str() ) );
		dataModel->addEntry(entry);
	}
	
//FIXME QListViewItemIterator it( m_HostListView );
/* FIXME いる？
	QListViewItemIterator it( m_HostListView );
	QListViewItem *topItem = it.current();
	if ( topItem != NULL ) {
		m_HostListView->setSelected( topItem, TRUE );
	}
*/
	kDebug() << "END   KIPMsgEncodingConfigDialog::refreshEncoding" << endl;
}

/*$SPECIALIZATION$*/
/**
 * OKクリックイベント
 * ・設定を保存してダイアログを閉じる。
 */
void KIPMsgEncodingConfigDialog::slotOkClicked()
{
	kDebug() << "START KIPMsgEncodingConfigDialog::slotOkClicked" << endl;
	slotApplyClicked();
//	close();
	accept();
	kDebug() << "END   KIPMsgEncodingConfigDialog::slotOkClicked" << endl;
}

/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる。
 */
void KIPMsgEncodingConfigDialog::slotCancelClicked()
{
	kDebug() << "START KIPMsgEncodingConfigDialog::slotCancelClicked" << endl;
//	close();
	reject();
	kDebug() << "END   KIPMsgEncodingConfigDialog::slotCancelClicked" << endl;
}

/**
 * ホスト選択イベント
 * ・選択されたホストを設定済のエンコーディングで
 *   ホストの項目（ユーザ名／グループ名）を表示する。
 */
//void KIPMsgEncodingConfigDialog::slotHostSelected( QListViewItem *item )
void KIPMsgEncodingConfigDialog::slotCurrentHostChanged( QModelIndex &current, QModelIndex &previous )
{
	kDebug() << "START KIPMsgEncodingConfigDialog::slotCurrentHostChanged" << endl;
	KIpMessengerEncodingConfigModel *dataModel = (KIpMessengerEncodingConfigModel*)m_HostListView->model();
	KIpMessengerEncodingConfigEntry entry = dataModel->getEncodingConfigEntry(current);

	m_GroupNameLabel->setText( entry.groupName() );
	m_UserNameLabel->setText( entry.nickName() );
	SeletedIpAddress = entry.ipAddress();
	SeletedLoginUser = entry.userName();
	QString enc = entry.encodingName();
	if ( enc != "" ) {
		int selectedMessageEncoding = m_EncodingCombobox->findText( enc );
		m_EncodingCombobox->setCurrentIndex( selectedMessageEncoding );
	} else {
//	TESTME
//		item->setText( COL_ENCODING, KIpMsgSettings::messageEncoding() );
		entry.setEncodingName( KIpMsgSettings::messageEncoding() );
		dataModel->setEncodingConfigEntry( current, entry );
		int selectedMessageEncoding = m_EncodingCombobox->findText( KIpMsgSettings::messageEncoding() );
			m_EncodingCombobox->setCurrentIndex( selectedMessageEncoding );
	}
	kDebug() << "END   KIPMsgEncodingConfigDialog::slotCurrentHostChanged" << endl;
}

/**
 * エンコーディング変更イベント
 * ・指定されたエンコーディングをホストリストに設定する。
 * ・指定されたエンコーディングでホストの項目（ユーザ名／グループ名）を再表示する。
 */
void KIPMsgEncodingConfigDialog::slotEncodingComboChanged(int /*index*/)
{
	kDebug() << "START KIPMsgEncodingConfigDialog::slotEncodingComboChanged" << endl;
	string userName;
	string groupName;

	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	HostList hosts = agent->GetHostList();
	for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
		if ( QString( ix->IpAddress().c_str() ) == SeletedIpAddress && 
			QString( ix->UserName().c_str() ) == SeletedLoginUser ) {
			userName = ix->Nickname();
			groupName = ix->GroupName();
			break;
		}
		
	}
//TESTME
	KIpMessengerEncodingConfigModel *dataModel = (KIpMessengerEncodingConfigModel*)m_HostListView->model();
	int maxRows = m_HostListView->model()->rowCount();

	for ( int i = 0; i < maxRows; i++ ){
		QModelIndex rowIndexTop = dataModel->index( i, 0 );
		KIpMessengerEncodingConfigEntry entry = dataModel->getEncodingConfigEntry( rowIndexTop );
		if ( entry.ipAddress() == SeletedIpAddress &&
			 entry.userName() == SeletedLoginUser ) {
			QTextCodec *codec = QTextCodec::codecForName( m_EncodingCombobox->currentText().toAscii() );
			entry.setGroupName( codec->toUnicode( groupName.c_str() ) );
			entry.setNickName( codec->toUnicode( userName.c_str() ) );
			entry.setEncodingName( m_EncodingCombobox->currentText() );
			m_UserNameLabel->setText( entry.nickName() );
			m_GroupNameLabel->setText( entry.groupName() );
			dataModel->setEncodingConfigEntry( rowIndexTop, entry );
			break;
		}
	}
	kDebug() << "END   KIPMsgEncodingConfigDialog::slotEncodingComboChanged" << endl;
}

/**
 * 適用クリックイベント
 * ・設定を保存する。
 */
void KIPMsgEncodingConfigDialog::slotApplyClicked()
{
	kDebug() << "START KIPMsgEncodingConfigDialog::slotApplyClicked" << endl;
	vector<HostListItem> targets;
//TESTME
	KIpMessengerEncodingConfigModel *dataModel = (KIpMessengerEncodingConfigModel*)m_HostListView->model();
	int maxRows = m_HostListView->model()->rowCount();

	for ( int i = 0; i < maxRows; i++ ){
		QModelIndex rowIndexTop = dataModel->index( i, 0 );
		KIpMessengerEncodingConfigEntry entry = dataModel->getEncodingConfigEntry( rowIndexTop );
		if ( entry.encodingName() != "" ) {
			HostListItem host;
			host.setEncodingName( (const char*)entry.encodingName().toAscii().data() );
			if ( host.EncodingName() != "" ){
				QTextCodec *codec = QTextCodec::codecForName( host.EncodingName().c_str() );
				host.setIpAddress( codec->fromUnicode( entry.ipAddress() ).data() );
				host.setUserName( codec->fromUnicode( entry.userName() ).data() );
				targets.push_back( host );
			}
		}
	}
	QStringList encodings = KIpMsgSettings::encodingSettings();
	for( vector<HostListItem>::iterator host = targets.begin(); host != targets.end(); host++ ) {
		if ( host->EncodingName() != "" ){
			QTextCodec *codec = QTextCodec::codecForName( host->EncodingName().c_str() );
			QString ip = codec->toUnicode( host->IpAddress().c_str() );
			QString login = codec->toUnicode( host->UserName().c_str() );
			QString enc = codec->toUnicode( host->EncodingName().c_str() );
			for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
				QStringList fields = ite->split( "|" );
				if ( ip == fields[0] &&
					login == fields[1] ) {
					encodings.removeAll( *ite );
					break;
				}
			}
			encodings << ip + "|" + login + "|" + enc;
		}
	}
	KIpMsgSettings::setEncodingSettings( encodings );

	KIpMsgSettings::self()->writeConfig();
	kDebug() << "END   KIPMsgEncodingConfigDialog::slotApplyClicked" << endl;
}

KIpMessengerEncodingConfigEntry::KIpMessengerEncodingConfigEntry(QString nickName, QString groupName, QString hostName, QString ipAddress, QString userName, QString encodingName){
	kDebug() << "START KIpMessengerEncodingConfigEntry::KIpMessengerEncodingConfigEntry" << endl;
	this->m_NickName = nickName;
	this->m_GroupName = groupName;
	this->m_HostName = hostName;
	this->m_IpAddress = ipAddress;
	this->m_UserName = userName;
	this->m_EncodingName = encodingName;
	kDebug() << "END   KIpMessengerEncodingConfigEntry::KIpMessengerEncodingConfigEntry" << endl;
}

void KIpMessengerEncodingConfigModel::clear(){
	kDebug() << "START KIpMessengerEncodingConfigModel::clear" << endl;
	removeRows(0,rowCount()-1);
	kDebug() << "END   KIpMessengerEncodingConfigModel::clear" << endl;
}
void KIpMessengerEncodingConfigModel::setEncodingConfigEntry(const QModelIndex &index, const KIpMessengerEncodingConfigEntry entry) {
	kDebug() << "START KIpMessengerEncodingConfigModel::setEncodingConfigEntry" << endl;
	encodingConfigList.replace(index.row(), entry);
	kDebug() << "END   KIpMessengerEncodingConfigModel::setEncodingConfigEntry" << endl;
}
KIpMessengerEncodingConfigEntry KIpMessengerEncodingConfigModel::getEncodingConfigEntry(const QModelIndex &index) {
	kDebug() << "START KIpMessengerEncodingConfigModel::getEncodingConfigEntry" << endl;
	KIpMessengerEncodingConfigEntry entry = encodingConfigList.at(index.row());
	kDebug() << "END   KIpMessengerEncodingConfigModel::getEncodingConfigEntry" << endl;
	return entry;
}
int KIpMessengerEncodingConfigModel::rowCount(const QModelIndex &parent) const{
	kDebug() << "START KIpMessengerEncodingConfigModel::rowCount" << endl;
	kDebug() << "END   KIpMessengerEncodingConfigModel::rowCount" << endl;
	return encodingConfigList.count();
}
QVariant KIpMessengerEncodingConfigModel::data(const QModelIndex &index, int role) const{
	kDebug() << "START KIpMessengerEncodingConfigModel::data" << endl;
	if (!index.isValid()){
		kDebug() << "END   KIpMessengerEncodingConfigModel::data" << endl;
		return QVariant();
	}

	if (index.row() >= encodingConfigList.size()){
		kDebug() << "END   KIpMessengerEncodingConfigModel::data" << endl;
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		KIpMessengerEncodingConfigEntry entry = encodingConfigList.at(index.row());
		switch(index.column()){
			case 0:		return entry.nickName();
			case 1:		return entry.groupName();
			case 2:		return entry.hostName();
			case 3:		return entry.ipAddress();
			case 4:		return entry.userName();
			case 5:		return entry.encodingName();
			default:	return QVariant();
		}
	} else {
		kDebug() << "END   KIpMessengerEncodingConfigModel::data" << endl;
		return QVariant();
	}
	kDebug() << "END   KIpMessengerEncodingConfigModel::data" << endl;
}
QVariant KIpMessengerEncodingConfigModel::headerData(int selection, Qt::Orientation orientation, int role) const{
	kDebug() << "START KIpMessengerEncodingConfigModel::headerData" << endl;
	if (role != Qt::DisplayRole){
		kDebug() << "END   KIpMessengerEncodingConfigModel::headerData" << endl;
		return QVariant();
	}

	if (orientation == Qt::Horizontal){
		switch(selection){
			case 0:		return tr2i18n( "User" );
			case 1:		return tr2i18n( "Group" );
			case 2: 	return tr2i18n( "Host" );
			case 3: 	return tr2i18n( "IP address" );
			case 4: 	return tr2i18n( "Login" );
			case 5: 	return tr2i18n( "Encoding" );
			default:	return QVariant();
		}
	}
	kDebug() << "END   KIpMessengerEncodingConfigModel::headerData" << endl;
	return QVariant();
}

bool KIpMessengerEncodingConfigModel::addEntry(const KIpMessengerEncodingConfigEntry entry){
	kDebug() << "START KIpMessengerEncodingConfigModel::addEntry" << endl;
	encodingConfigList.append(entry);
	kDebug() << "END   KIpMessengerEncodingConfigModel::addEntry" << endl;
	return true;
}

#include "encodingconfig.moc"
