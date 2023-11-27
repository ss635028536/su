//
// SIO_JWServer.cpp : インプリメンテーション ファイル
//
#include <string.h>
#include <time.h>

#include "stdafx.h"
#include "LA.h"
#include "SIO_JWServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// シーケンサ・コマンドテーブル
static SEQ_COMMAND_DATA Seq_Command_Tbl[] = {
	{	RR_COMMAND, RR_MAX, 	"@%02dRR%04d0001",	"@%2dRR%2x%4hx",	5},
//@	{	RL_COMMAND, LR_MAX, 	"@%02dRL%04d0001",	"@%2dRL%2x%4hx",	5},
//@	{	RH_COMMAND, HR_MAX, 	"@%02dRH%04d0001",	"@%2dRH%2x%4hx",	5},
//@	{	RC_COMMAND, TIMER_MAX,	"@%02dRC%04d0001",	"@%2dRC%2x%4hd",	5},
//@	{	RG_COMMAND, TIMER_MAX,	"@%02dRG%04d0001",	"@%2dRG%2x%1hd",	5},
	{	RD_COMMAND, DM_MAX, 	"@%02dRD%04d0001",	"@%2dRD%2x%4hx",	5},
//@	{	RJ_COMMAND, AR_MAX, 	"@%02dRJ%04d0001",	"@%2dRJ%2x%4hx",	5},

	{	WR_COMMAND, RR_MAX, 	"@%02dWR%04d%04X",	"@%2dWR%2x",	5},
//@	{	WL_COMMAND, LR_MAX, 	"@%02dWL%04d%04X",	"@%2dWL%2x",	5},
//@	{	WH_COMMAND, HR_MAX, 	"@%02dWH%04d%04X",	"@%2dWH%2x",	5},
//@	{	WC_COMMAND, TIMER_MAX,	"@%02dWC%04d%04d",	"@%2dWC%2x",	5},
//@	{	WG_COMMAND, TIMER_MAX,	"@%02dWG%04d%1d",	"@%2dWG%2x",	5},
	{	WD_COMMAND, DM_MAX, 	"@%02dWD%04d%04X",	"@%2dWD%2x",	5},
//@	{	WJ_COMMAND, AR_MAX, 	"@%02dWJ%04d%04X",	"@%2dWJ%2x",	5},

//@	{	RTIM_COMMAND,	TIMER_MAX,	"@%02dR#TIM %04d",		"@%2dR#%2x%4hd",	200},
//@	{	RTIMH_COMMAND,	TIMER_MAX,	"@%02dR#TIMH%04d",		"@%2dR#%2x%4hd",	200},
//@	{	RCNT_COMMAND,	TIMER_MAX,	"@%02dR#CNT %04d",		"@%2dR#%2x%4hd",	200},
//@	{	RCNTR_COMMAND,	TIMER_MAX,	"@%02dR#CNTR%04d",		"@%2dR#%2x%4hd",	200},
//@	{	WTIM_COMMAND,	TIMER_MAX,	"@%02dW#TIM %04d%04d",	"@%2dW#%2x",	200},
//@	{	WTIMH_COMMAND,	TIMER_MAX,	"@%02dW#TIMH%04d%04d",	"@%2dW#%2x",	200},
//@	{	WCNT_COMMAND,	TIMER_MAX,	"@%02dW#CNT %04d%04d",	"@%2dW#%2x",	200},
//@	{	WCNTR_COMMAND,	TIMER_MAX,	"@%02dW#CNTR%04d%04d",	"@%2dW#%2x",	200},

//@	{	MS_COMMAND, USHRT_MAX,	"@%02dMS",		"@%2dMS%2x%4x%16c,  5"},
	{	SC_COMMAND, USHRT_MAX,	"@%02dSC%02X",	"@%2dSC%2x",	5},
//@	{	MF_COMMAND, USHRT_MAX,	"@%02dMF%02X",	"@%2dMF%2x",	5},
//@	{	TS_COMMAND, USHRT_MAX,	"@%02dTS%s",	"@%2dTS%s", 	5},

	{	RRXX_COMMAND,	RR_MAX, 	"@%02dRR%04d%04d",	"@%%2dRR%%2x%%%dc", 5},
	{	WRXX_COMMAND,	RR_MAX, 	"@%02dWR%04d%*.*s", "@%2dWR%2x",	5},
	{	RDXX_COMMAND,	DM_MAX, 	"@%02dRD%04d%04d",	"@%%2dRD%%2x%%%dc", 5},
	{	WDXX_COMMAND,	DM_MAX, 	"@%02dWD%04d%*.*s", "@%2dWD%2x",	5},
	{	WDXSPEED_COMMAND,	DM_MAX, 	"@%02dWD%04d%*.*s", "@%2dWD%2x",	5},
	{	WHXX_COMMAND,	DM_MAX, 	"@%02dWD%04d%*.*s", "@%2dWD%2x",	5},
	{	RHXX_COMMAND,	DM_MAX, 	"@%02dRD%04d%04d",	"@%%2dRD%%2x%%%dc", 5},
};

// 受信データ構造体
struct SRecvData
{
	// 受信文字列バッファ
	char	RcvBuf[SEQ_BUFF_SIZE];
	// 受信文字列データレングス
	int 	nRcvLength;
	// 受信データバッファ
	USHORT	DataBuf[SEQ_BUFF_SIZE];
	// 受信データ個数
	int 	nDataCnt;
};

// 送信コマンド構造体
struct SSendCmd
{
	// スレッドＩＤ
	DWORD	idThread;
	// データの受信を通知するメッセージコード
	UINT	nMsg;
	// 送信コマンド番号
	USHORT	nCmdNo;
	// CH番号
	USHORT	nChNo;
	// データ個数(送信/受信)
	USHORT	nDataCnt;
	// 送信データバッファ
	USHORT	DataBuf[SEQ_BUFF_SIZE];
};

//	シーケンサ通信スレッド
UINT SeqComThread( LPVOID pParam )
{
	CSIO_SeqCntl	*pSIO_JWServer = ( CSIO_SeqCntl* )pParam;
	SSendCmd	*pSendCmd;
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nExitFlag;			// 停止要求フラグ
	int 	nRet;
	int		nCounter = 0;
	CLAApp*	pApp;
	CString strError;
	// レーザアニールアプリケーションクラスの設定
	pApp = ( CLAApp* )AfxGetApp( );

	TRACE( "SeqComThread: nPort : %d : Thread start.\n", pSIO_JWServer->m_nPort );

	// 停止要求フラグOFF
	nExitFlag = OFF;
	// 受信データキュウーフラグON
	nRcvQueFlag = ON;

	while ( TRUE )
	{
		if ( pSIO_JWServer->ExitRequest( ) )
			break;
		else
		{
			if ( pSIO_JWServer->Remove_SendQueue( &pSendCmd ) )
			{
				// 受信タイムアウト時間を求める
				nRcvTimeout = Seq_Command_Tbl[pSendCmd->nCmdNo].nRcvTimeout * 100;
				// シーケンサとの通信
				nRet = pSIO_JWServer->SeqCommunication( pSendCmd->nCmdNo, pSendCmd->nChNo,
								pSendCmd->DataBuf, pSendCmd->nDataCnt, nRcvTimeout, nRcvQueFlag );
				if( !nRet )
				{
					PostThreadMessage( pSendCmd->idThread, pSendCmd->nMsg, RTN_NORMAL, NULL );
#ifdef	_SEQ_WATCH_
					TRACE( "@@@@@@@@@@@@@@PostMessage(RTN_NORMAL)\n" );
#endif	_SEQ_WATCH_
				}
				else if( nRet == RTN_RETRYOVER )
				{
					strError = "Joy Retry Over.";
					pApp->WriteJSWLog( strError );
					PostThreadMessage( pSendCmd->idThread, pSendCmd->nMsg, RTN_ERROR, NULL );
					TRACE( "@@@@@@@@@@@@@@PostMessage(RTN_ERROR)\n" );
				}
				else if( nRet == RTN_STOP_REQ )
				{
					strError = "Joy Stop Request.";
					pApp->WriteJSWLog( strError );
					nExitFlag = ON;
				}
				else
				{
					strError.Format( "Joy Error.Number %d.",nRet );
					pApp->WriteJSWLog( strError );
					PostThreadMessage( pSendCmd->idThread, pSendCmd->nMsg, RTN_ERROR, NULL );
					TRACE( "@@@@@@@@@@@@@@PostMessage(RTN_ERROR) 応答なし\n" );
				}
				delete pSendCmd;
			}
			if( nExitFlag == ON )
				break;

			if( pSIO_JWServer->m_nIdleFg )	// アイドル中の通信を行う?
			{
				// 入出力リレーの読み込み
				if( nRet = pSIO_JWServer->SeqSioRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqSioRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
				// XY軸位置情報の読み込み
				if( nRet = pSIO_JWServer->SeqXYPosRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqXYPosRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
				// 圧力ゲージ情報の読み込み
				if( nRet = pSIO_JWServer->SeqPressureRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqPressureRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
				// MFCﾀｲﾏ流量温度酸素濃度情報の読み込み
				if( nRet = pSIO_JWServer->SeqMfcFlowTempO2Read() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqMfcFlowTempO2Read.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
				// エアーベアリング情報の読み込み
				if( nRet = pSIO_JWServer->SeqStageAirBearingRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqStageAirBearingRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
				// フローメータ情報の読み込み
				if( nRet = pSIO_JWServer->SeqFlowMeterRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqFlowMeterRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
				// X Speed 情報の読み込み
				if( nRet = pSIO_JWServer->SeqXSpeedRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqXSpeedRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
				// THETA90度位置情報の読み込み
				if( nRet = pSIO_JWServer->SeqTheta90degRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqTheta90DegRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
				// エラーコード情報の読み込み
				if( nRet = pSIO_JWServer->SeqErrorCodeRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqErrorCodeRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
//> oku insert start 12-09-25
				//  Is Reading Glass Data Flag On or Off?
//@				if( pApp->m_nGlassRead == ON )
//@				{
				// ローダ情報の読み込み
				if( nRet = pSIO_JWServer->SeqLoaderInfoRead() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at SeqLoaderInfoRead.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
//@					// End of Reading
//@					pApp->m_nGlassRead = OFF;
//@				}
//> oku insert end 12-09-25
				// Read Ex. Trigger Reprate
				if( nRet = pSIO_JWServer->TrigOutReprateReadJ() )
				{
					if( nRet == RTN_STOP_REQ )
					{
						break;
					}
					else
					{
						strError.Format( "Joy Error at TrigOutReprateread.Number %d.",nRet );
						pApp->WriteJSWLog( strError );
					}
				}
			}
		}
		// 処理の一時停止
		Sleep( SEQCOM_THREAD_WAIT );
	}

	TRACE( "SeqComThread: nPort : %d : Thread exit.\n", pSIO_JWServer->m_nPort );

	AfxEndThread( 0 );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CSIO_Control

//
//	コンストラクタ
//
CSIO_SeqCntl::CSIO_SeqCntl( SSIO_Seq_Param& sParam )
{
	int		nStatus;

	TRACE( "CSIO_SeqCntl::CSIO_SeqCntl ==========> 開始\n" );

	InitializeCriticalSection( &m_csPlcSendLock );
	InitializeCriticalSection( &m_csPlcRcvDataLock );
	// レーザアニールアプリケーションクラスの設定
	m_pApp = ( CLAApp* )AfxGetApp( );

	// パラメータの保存
	m_nPort = sParam.nPort;					// ポート番号
	m_dwBaudrate = sParam.dwBaudrate;		// ボーレート
	m_dwDataBits = sParam.dwDataBits;		// データビット長
	m_dwStopBits = sParam.dwStopBits;		// ストップビット長
	m_dwParity = sParam.dwParity;			// パリティ
	m_dwXonXoffFlow = sParam.dwXonXoffFlow;	// フロー制御
	m_dwXonCode = sParam.dwXonCode;			// XONコード
	m_dwXoffCode = sParam.dwXoffCode;		// XOFFコード
	m_dwHardFlow = sParam.dwHardFlow;		// ハードウェアフロー制御
	m_dwDuplex = sParam.dwDuplex;			// 全二重/半二重
	m_dwRcvSize = sParam.dwRcvSize;			// 受信バッファサイズ

	m_bDelimiter = sParam.bDelimiter;		// デリミタ制御指定
	m_byDelimiter = sParam.byDelimiter;		// デリミタコード
	m_nReadTime = sParam.nReadTime;			// 受信監視時間
	m_nWriteTime = sParam.nWriteTime;		// 送信監視時間

	m_uMessage = sParam.uMessage;			// メッセージコード
	m_nIdleFg = sParam.nIdleFg;				// アイドル中通信フラグ


	m_pSeqComThread = NULL;

	// 通信ポート初期化処理(CIOタグ設定)
	if( nStatus = JW_Initialize() == 1 )
	{
		m_bThreadExit = FALSE;
		// 通信スレッドの起動
		if ( ( m_pSeqComThread = AfxBeginThread( SeqComThread, ( LPVOID )this ) ) != NULL )
		{
			// 自動削除の指示を偽に設定
			m_pSeqComThread->m_bAutoDelete = FALSE;
		}
		else
		{
			TRACE( "CSIO_SeqCntl::CSIO_SeqCntl: [AfxBeginThread] Failure.\n" );
		}
	}
	else
	{
	}

	TRACE( "CSIO_SeqCntl::CSIO_SeqCntl ==========> 終了\n" );
}

//
//	デストラクタ
//
CSIO_SeqCntl::~CSIO_SeqCntl( )
{
	DWORD	dwExitCode;
	int 	nCount = 0;

	TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl ==========> 開始\n" );

	if ( m_pSeqComThread != NULL )
	{
		// 受信スレッドの停止指示
		m_bThreadExit = TRUE;
		// 以下を繰り返す
		while ( TRUE )
		{
			// スレッド終了コードの取得
			if ( ::GetExitCodeThread( m_pSeqComThread->m_hThread, &dwExitCode ) != FALSE )
			{
				// スレッド終了の場合
				if ( dwExitCode != STILL_ACTIVE )
					// ループから抜ける
					break;
				else
				{
					// 監視カウンタが既定値に達した場合
					if ( nCount == ( THREAD_KILL_WAIT_TIME / THREAD_KILL_TICK_TIME ) )
						// ループから抜ける
						break;
					else
						// 監視カウンタを更新
						nCount++;
				}
			}
			else
			{
				TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl: [GetExitCodeThread] Failure.\n" );
				break;
			}
			// 処理の一時停止
			Sleep( THREAD_KILL_TICK_TIME );
		}
		// 監視カウンタが既定値に達していないない場合
		if ( nCount < ( THREAD_KILL_WAIT_TIME / THREAD_KILL_TICK_TIME ) )
		{
			TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl: Success.\n" );
		}
		else
		{
			TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl: Failure.\n" );
		}

		// 通信ポート終了処理
		Terminate();

		// 受信スレッドの廃棄
		delete m_pSeqComThread;
	}

	EnterCriticalSection( &m_csPlcRcvDataLock );
	SRecvData*	pRecvData;
	// 受信データキューの廃棄
	while( ! m_listRecvData.IsEmpty( ) )
	{
		// キューの先頭からデータを取得
		pRecvData = m_listRecvData.RemoveHead( );
		delete pRecvData;
	}
	LeaveCriticalSection( &m_csPlcRcvDataLock );

	EnterCriticalSection( &m_csPlcSendLock );
	SSendCmd*  pSendCmd;
	// 送信コマンドキューの廃棄
	while( ! m_listSendCmd.IsEmpty( ) )
	{
		// キューの先頭からデータを取得
		pSendCmd = m_listSendCmd.RemoveHead( );
		delete pSendCmd;
	}
	LeaveCriticalSection( &m_csPlcSendLock );

	DeleteCriticalSection( &m_csPlcSendLock );
	DeleteCriticalSection( &m_csPlcRcvDataLock );
#ifndef _DISP_ONLY_
	// JWサーバ通信切断
	DisconnectNet( );
#endif

	TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl ==========> 終了\n" );
}

/////////////////////////////////////////////////////////////////////////////
//	パブリック関数

//
//	受信データキュー内のデータ個数の取得
//
int CSIO_SeqCntl::GetRecvQ_Count( )
{
#ifndef _DISP_ONLY_
	EnterCriticalSection( &m_csPlcRcvDataLock );
	int		nRet = m_listRecvData.GetCount( );
	LeaveCriticalSection( &m_csPlcRcvDataLock );
	return nRet;
#else
	return( 1 );
#endif	_DISP_ONLY_
}

//
//	受信データキューへの追加
//
void CSIO_SeqCntl::Append_RecvQueue(
	char	*pRcvBuf,
	USHORT	*pDataBuf,
	int 	nDataCnt )
{
	SRecvData*	pRecvData = new SRecvData;
	int 	i;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::Append_RecvQueue ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信文字列の保存
	strcpy( pRecvData->RcvBuf, pRcvBuf );
	pRecvData->nRcvLength = strlen( pRcvBuf );
	// 受信データの保存
	for( i = 0; i < nDataCnt; i++ )
		pRecvData->DataBuf[i] = pDataBuf[i];
	pRecvData->nDataCnt = nDataCnt;
	// キューの最後に追加
	EnterCriticalSection( &m_csPlcRcvDataLock );
	m_listRecvData.AddTail( pRecvData );
	LeaveCriticalSection( &m_csPlcRcvDataLock );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::Append_RecvQueue ==========> 終了\n" );
#endif	_SEQ_WATCH_
}

//
//	送信コマンドキュー内のデータ個数の取得
//
int CSIO_SeqCntl::GetSendQ_Count( )
{
	EnterCriticalSection( &m_csPlcSendLock );
	int		nRet = m_listSendCmd.GetCount( );
	LeaveCriticalSection( &m_csPlcSendLock );

	return nRet;
}

//
//	送信コマンドキューへの追加
//
void CSIO_SeqCntl::Append_SendQueue(
	DWORD	idThread,
	UINT	nMsg,
	USHORT	nCmdNo,
	USHORT	nChNo,
	USHORT	nDataCnt,
	USHORT	*pDataBuf )
{
	SSendCmd*  pSendCmd = new SSendCmd;
	int 	i;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::Append_SendQueue ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 送信コマンドの保存
	EnterCriticalSection( &m_csPlcSendLock );
	pSendCmd->idThread = idThread;
	pSendCmd->nMsg = nMsg;
	pSendCmd->nCmdNo = nCmdNo;
	pSendCmd->nChNo = nChNo;
	pSendCmd->nDataCnt = nDataCnt;
	if( pDataBuf != NULL )
	{
		for( i = 0; i < nDataCnt; i++ )
			pSendCmd->DataBuf[i] = pDataBuf[i];
	}
	// キューの最後に追加
	m_listSendCmd.AddTail( pSendCmd );
	LeaveCriticalSection( &m_csPlcSendLock );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::Append_SendQueue ==========> 終了\n" );
#endif	_SEQ_WATCH_
}

//
// 送信コマンドキューから先頭データを取り出し
//
int	CSIO_SeqCntl::Remove_SendQueue( SSendCmd** pSendCmd )
{
	EnterCriticalSection( &m_csPlcSendLock );
	int	nRet = m_listSendCmd.GetCount( );

	// 送信コマンドキュー内にデータがある場合
	if( nRet )
	{
		// キューの先頭からデータを取得
		*pSendCmd = m_listSendCmd.RemoveHead( );
	}
	LeaveCriticalSection( &m_csPlcSendLock );

	return nRet;
}

//
//	データの受信
//
BOOL CSIO_SeqCntl::DataRead(
	char	*pRcvBuf,
	int 	*pnBufLength,
	USHORT	*pDataBuf,
	int 	*pnDataCnt )
{
#ifndef _DISP_ONLY_
	BOOL	Rcode = FALSE;
#else
	BOOL	Rcode = TRUE;
#endif	_DISP_ONLY_
	int 	i;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::DataRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	EnterCriticalSection( &m_csPlcRcvDataLock );
	if ( ! m_listRecvData.IsEmpty( ) )
	{
		SRecvData*	pRecvData;

		// キューの先頭からデータを取得
		pRecvData = m_listRecvData.RemoveHead( );
		// 受信文字列の設定
		*pnBufLength = pRecvData->nRcvLength;
		strcpy( pRcvBuf, pRecvData->RcvBuf );
		// 受信データの設定
		*pnDataCnt = pRecvData->nDataCnt;
		for( i = 0 ; i < pRecvData->nDataCnt ; i++ )
			pDataBuf[i] = pRecvData->DataBuf[i];
		delete pRecvData;

		Rcode = TRUE;
	}
	LeaveCriticalSection( &m_csPlcRcvDataLock );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::DataRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( Rcode );
}

//
//	スレッド停止指示
//
BOOL CSIO_SeqCntl::ExitRequest( )
{
	// 停止指示をもって終了
	return m_bThreadExit;
}

/*---------------------------------------------------------*
	名前： SeqCommunication
	概要： シーケンサとの通信
	引数： USHORT	nCmdidx (IN)
			コマンドのインデックス
		   USHORT	nChNo	(IN)
			チャンネル
		   USHORT	*pDataBuff	(OUT)
			書き込むデータ／読みだしたデータ
		   USHORT	nDataCnt	(IN)
			データ数
		   USHORT	nRcvTimeout (IN)
			受信タイムアウト時間
		   USHORT	nRcvQueFlag (IN)
			受信データキューフラグ
	戻値： int
			０：正常終了
			1000：通信エラーリトライ回数オーバ
			2000：停止要求
 *---------------------------------------------------------*/
int CSIO_SeqCntl::SeqCommunication(
	USHORT	nCmdidx,
	USHORT	nChNo,
	USHORT	*pDataBuff,
	USHORT	nDataCnt,
	USHORT	nRcvTimeout,
	USHORT	nRcvQueFlag )
{
	int 	nRetryCount;
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqCommunication ==========> 開始\n" );
#endif	_SEQ_WATCH_

#ifdef	_DISP_ONLY_
	return( 0 );/* シーケンサ使用してないときはすべて正常終了とする */
#endif	_DISP_ONLY_

	nRetryCount = RETRY_COUNT;

	USHORT	DataBuff[SEQ_BUFF_SIZE];	// 受信データバッファ
	int		nData;
	int		nTagNum;
	int		i,nValSize;
	char	*pBuf,*pD;
	CString strData;

	int nCom;

	switch( nCom = nCmdidx )
	{
		// 読み込みの場合
		case RRXX_COMMAND:
		case RDXX_COMMAND:
		case RHXX_COMMAND:
			TCOM_DATA1	*pDat;
			int nCnt;
			// 先頭アドレスの設定
			// CIOの場合
			if( (nCom == RRXX_COMMAND) && ( nChNo >= JW_CIO_FAST_CH  && nChNo < (JW_CIO_FAST_CH + JW_CIO_CH_CNT) ) )
			{
				// 先頭タグ番号の設定
				nTagNum = (nChNo - JW_CIO_FAST_CH) * 16;
				// 先頭アドレスの設定
				pDat = &tCIO_Data[nTagNum];
				// データ数の設定
				nCnt = nDataCnt * 16;

				// JWサーバデータの読み込み
				nRet = JWRead(
					0,				// UserID ::GUEST
					"",				// Passwd :: PWD無し
					nCnt,			// 読み込みタグ週
					pDat			// 読み込み先頭タグID
					);
				// 読み込み成功の場合
				if( nRet == 0 )
				{
					for ( i = 0; i < nDataCnt; i++)
					{
						nData = (pDat[ 0 + i*16 ].pbVal[0] & 0x01 ? 0x01 : 0x00);
						nData = nData | (pDat[  1 + i*16 ].pbVal[0] & 0x01 ? 0x02   : 0x00);
						nData = nData | (pDat[  2 + i*16 ].pbVal[0] & 0x01 ? 0x04   : 0x00);
						nData = nData | (pDat[  3 + i*16 ].pbVal[0] & 0x01 ? 0x08   : 0x00);
						nData = nData | (pDat[  4 + i*16 ].pbVal[0] & 0x01 ? 0x10   : 0x00);
						nData = nData | (pDat[  5 + i*16 ].pbVal[0] & 0x01 ? 0x20   : 0x00);
						nData = nData | (pDat[  6 + i*16 ].pbVal[0] & 0x01 ? 0x40   : 0x00);
						nData = nData | (pDat[  7 + i*16 ].pbVal[0] & 0x01 ? 0x80   : 0x00);
						nData = nData | (pDat[  8 + i*16 ].pbVal[0] & 0x01 ? 0x100  : 0x00);
						nData = nData | (pDat[  9 + i*16 ].pbVal[0] & 0x01 ? 0x200  : 0x00);
						nData = nData | (pDat[ 10 + i*16 ].pbVal[0] & 0x01 ? 0x400  : 0x00);
						nData = nData | (pDat[ 11 + i*16 ].pbVal[0] & 0x01 ? 0x800  : 0x00);
						nData = nData | (pDat[ 12 + i*16 ].pbVal[0] & 0x01 ? 0x1000 : 0x00);
						nData = nData | (pDat[ 13 + i*16 ].pbVal[0] & 0x01 ? 0x2000 : 0x00);
						nData = nData | (pDat[ 14 + i*16 ].pbVal[0] & 0x01 ? 0x4000 : 0x00);
						nData = nData | (pDat[ 15 + i*16 ].pbVal[0] & 0x01 ? 0x8000 : 0x00);

						DataBuff[i] = nData & 0xFFFF;
					}
				}
			}
			// MonitorData(W0100~017F)の場合
			else if( ( nCom == RDXX_COMMAND ||nCom == RHXX_COMMAND  ) && (nChNo >= JW_MONITOR_FAST_CH && nChNo < (JW_MONITOR_FAST_CH + JW_MONITOR_TAG_MAX) ) )
			{
				// 先頭タグ番号の設定
				nTagNum = nChNo - JW_MONITOR_FAST_CH;
				// 先頭アドレスの設定
				pDat = &tMonitor_Data[nTagNum];
				// データ数の設定
				nCnt = nDataCnt;

				// JWサーバデータの読み込み
				nRet = JWRead(
					0,				// UserID ::GUEST
					"",				// Passwd :: PWD無し
					nCnt,			// 読み込みタグ週
					pDat			// 読み込み先頭タグID
					);
				// 読み込み成功の場合
				if( nRet == 0 )
				{
					// BCDデータの場合
					if( nCom == RDXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)BCDtoINT( (int)tMonitor_Data[nTagNum+i].dblVal );
						}
					}
					// 16進データの場合
					else if( nCom == RHXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)tMonitor_Data[nTagNum+i].dblVal;
						}
					}
				}
			}
			// DataSet(W0030~007F)の場合
			else if( ( nCom == RDXX_COMMAND ||nCom == RHXX_COMMAND  ) && (nChNo >= JW_DATASET_FAST_CH && nChNo < (JW_DATASET_FAST_CH + JW_DATASET_TAG_MAX) ) )
			{
				// 先頭タグ番号の設定
				nTagNum = nChNo - JW_DATASET_FAST_CH;
				// 先頭アドレスの設定
				pDat = &tDataSet_Data[nTagNum];
				// データ数の設定
				nCnt = nDataCnt;

				// JWサーバデータの読み込み
				nRet = JWRead(
					0,				// UserID ::GUEST
					"",				// Passwd :: PWD無し
					nCnt,			// 読み込みタグ週
					pDat			// 読み込み先頭タグID
					);
				// 読み込み成功の場合
				if( nRet == 0 )
				{
					// BCDデータの場合
					if( nCom == RDXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)BCDtoINT( (int)tDataSet_Data[nTagNum+i].dblVal );
						}
					}
					// 16進データの場合
					else if( nCom == RHXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)tDataSet_Data[nTagNum+i].dblVal;
						}
					}
				}
			}
			// WorkSel(動作選択)(W0000~000F)の場合
			else if( ( nCom == RDXX_COMMAND ||nCom == RHXX_COMMAND  ) && (nChNo >= JW_WORK_FAST_CH && nChNo < (JW_WORK_FAST_CH + JW_WORK_TAG_MAX) ) )
			{
				// 先頭タグ番号の設定
				nTagNum = nChNo;
				// 先頭アドレスの設定
				pDat = &tWorkSel_Data[nTagNum];
				// データ数の設定
				nCnt = nDataCnt;

				// JWサーバデータの読み込み
				nRet = JWRead(
					0,				// UserID ::GUEST
					"",				// Passwd :: PWD無し
					nCnt,			// 読み込みタグ週
					pDat			// 読み込み先頭タグID
					);
				// 読み込み成功の場合
				if( nRet == 0 )
				{
					// BCDデータの場合
					if( nCom == RDXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)BCDtoINT( (int)tWorkSel_Data[nTagNum+i].dblVal );
						}
					}
					// 16進データの場合
					else if( nCom == RHXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)tWorkSel_Data[nTagNum+i].dblVal;
						}
					}
				}
			}

			break;
		// 書き込みの場合
		case WR_COMMAND:	// CIO書き込み
		case WDXX_COMMAND:	// BCDデータ書き込み
		case WHXX_COMMAND:
			long *pFasrTag;
			// データの設定
			nValSize = ( ( sizeof( double ) )* nDataCnt );
			pD = pBuf = new char[nValSize];
			for ( i = 0; i < nDataCnt; i++ ){
				if( nCom ==WDXX_COMMAND )
				{
					// １０進データを１６進データとして扱う
					nData = INTtoBCD( pDataBuff[i] );
				}
				else
					nData = pDataBuff[i];

				*(double *)pD = nData;
				pD += sizeof(double );
			}

			// 先頭アドレスの設定
			// CIOの場合
			if( ( nCom == WR_COMMAND ) && ( nChNo >= JW_CIO_FAST_CH && nChNo < (JW_CIO_FAST_CH + JW_CIO_CH_CNT) ) )
			{
				// 先頭タグ番号の設定
				nTagNum = (nChNo - JW_CIO_FAST_CH) * 16;
				// 先頭アドレスの設定
				pFasrTag = &m_pJwCIOIDs[nTagNum];
			}
			// MONITOR(W0100~017F)の場合
			else if( ( nCom == WDXX_COMMAND || nCom == WHXX_COMMAND ) && ( nChNo >= JW_MONITOR_FAST_CH && nChNo < (JW_MONITOR_FAST_CH + JW_MONITOR_TAG_MAX) ) )
			{
				// 先頭タグ番号の設定
				nTagNum = nChNo - JW_MONITOR_FAST_CH;
				// 先頭アドレスの設定
				pFasrTag = &m_pJwMonitorIDs[nTagNum];
			}
			// DataSet(W0030~008F)の場合
			else if( ( nCom == WDXX_COMMAND || nCom == WHXX_COMMAND ) && ( nChNo >= JW_DATASET_FAST_CH && nChNo < (JW_DATASET_FAST_CH + JW_DATASET_TAG_MAX) ) )
			{
				// 先頭タグ番号の設定
				nTagNum = nChNo - JW_DATASET_FAST_CH;
				// 先頭アドレスの設定
				pFasrTag = &m_pJwDataSetIDs[nTagNum];
			}
			// STEP(W0010~003F)の場合
			else if( ( nCom == WDXX_COMMAND || nCom == WHXX_COMMAND ) && ( nChNo >= JW_STEP_FAST_CH && nChNo < (JW_STEP_FAST_CH + JW_STEP_TAG_MAX) ) ) 
			{
				// 先頭タグ番号の設定
				nTagNum = nChNo - JW_STEP_FAST_CH;
				// 先頭アドレスの設定
				pFasrTag = &m_pJwStepIDs[nTagNum];
			}
			// WorkSel(W0000~000F)の場合
			else if( ( nCom == WDXX_COMMAND || nCom == WHXX_COMMAND ) && ( nChNo >= JW_WORK_FAST_CH && nChNo < (JW_WORK_FAST_CH + JW_WORK_TAG_MAX) ) )
			{
				// 先頭タグ番号の設定
				nTagNum = nChNo;
				// 先頭アドレスの設定
				pFasrTag = &m_pJwWorkSelIDs[nTagNum];
			}
			else
			{
				// エラー
				nRet = -999;
				delete pBuf;
				break;
			}

			// JWサーバにデータ書き込み
			nRet = JWWrite( 0,					// UserID ::GUEST
					"",							// Passwd :: PWD無し
					0,							//　常に０
					nDataCnt,					// 書き込みタグ数
					nValSize,					// PACK したデータバイト数 数値の場合には　8 * nn
												// 数値は常にdoubleとして扱う
					pFasrTag,		// 書き込みタグを並べた配列の先頭アドレス
					pBuf						// データをパックした先頭アドレス
												// 数値系の場合には　double の配列と同じ
					);
			delete pBuf;
			break;
		default:
			nRet = -999;
			TRACE( "CSIO_SeqCntl::SeqCommunication: nCmdidx Error.\n" );
	}

	if( nRet == 0)
	{
		if( nRcvQueFlag )
		{
			// 受信したデータをキューに追加
			Append_RecvQueue( "JWDATA", DataBuff, nDataCnt );
		}
		else
		{
			for( i = 0; i < nDataCnt; i++ )
				pDataBuff[i] = DataBuff[i];
		}
	}
	return( nRet );
}

/*---------------------------------------------------------*
	名前： SeqSend
	概要： シーケンサにデータを送信する
	引数： char 	*pBuf		(IN)
			送信データ
		   int		nLength 	(IN)
			送信データ数
	戻値： BOOL
			１：正常終了
			０：エラー
 *---------------------------------------------------------*/
BOOL CSIO_SeqCntl::SeqSend(
	char	*pBuf,
	int 	nLength )
{
	BOOL	rc;
	int		nStatus;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSend ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// データの送信
	nStatus = Send(pBuf, nLength);

	if( nStatus)
	{
		rc = FALSE;
		TRACE( "CSIO_SeqCntl::SeqSend: [Send] Failure. Error Code : %d\n", nStatus);
	}
	else
		rc = TRUE;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSend ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return rc;
}

/*---------------------------------------------------------*
	名前： SeqRecv
	概要： シーケンサからデータを受信する
	引数： char 	*pBuf		(OUT)
			受信データ
		   int		*pnLength	(OUT)
			受信データ数
		   int		nRTime		(IN)
			受信タイムアウト時間（100msec単位）
	戻値： BOOL
			１：正常終了
			０：エラー
 *---------------------------------------------------------*/
BOOL CSIO_SeqCntl::SeqRecv(
	char	*pBuf,
	int 	*pnLength,
	int 	nRTime )
{
	char	*pStr;
	BOOL	rc;
	int		nStatus;
#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqRecv ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// データの受信
	nStatus = Recv( pBuf, pnLength, nRTime);

	if( nStatus == 0)
	{
		rc = TRUE;
		pStr = strchr( pBuf, SEQ_DELIMITER );
		if( pStr != NULL )
		{
			*(pStr+1) = '\0';
			*pnLength = strlen( pBuf ); // 受信データ数のセット
		}
		else
		{
			ASSERT(0);	// エラー
		}
	}
	else
	{
		rc = FALSE;
		TRACE( "CSIO_SeqCntl::SeqRecv: [Recv] Failure. Error Code : %d\n", nStatus);
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqRecv ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return rc;
}

/*---------------------------------------------------------*
	名前： ResetComError
	概要： 通信エラーをリセットする
	引数： なし
	戻値： int
			1：正常終了
			0：エラー
 *---------------------------------------------------------*/
BOOL CSIO_SeqCntl::ResetComError( void )
{
	BOOL	rc;
	int		nStatus;

	TRACE( "CSIO_SeqCntl::ResetComError ==========> 開始\n" );

	nStatus = ErrorReset();

	if ( nStatus)
	{
		rc = FALSE;
		TRACE( "CSIO_SeqCntl::ResetComError: [ErrorReset] Failure. Error Code : %d\n", nStatus);
	}
	else
		rc = TRUE;

	TRACE( "CSIO_SeqCntl::ResetComError ==========> 終了\n" );

	return rc;
}

// シーケンサ通信変更関連(一部をSeq_BASEへ移動し古い部分は残さず)
/***********************************************************
	名前： SeqSioRead
	概要： 入出力リレーエリアの読み込み
	引数： なし
	戻値： int
			0   ：正常終了
			2000：停止要求
 ***********************************************************/
int CSIO_SeqCntl::SeqSioRead( void )
{
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	USHORT	DataBuff[SEQ_BUFF_SIZE];	// 入出力リレーデータバッファ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSioRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( nRet = SeqCommunication( RRXX_COMMAND, JW_CIO_FAST_CH, DataBuff,
									JW_CIO_CH_CNT, nRcvTimeout, nRcvQueFlag ) )
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}
	else
	{
		// 内部データへの設定
		CSeq_BASE::SeqSioRead( DataBuff );
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSioRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/***********************************************************
	名前： SeqSioOut
	概要： 入出力リレー書き込み
	引数： DWORD	idThread		(IN)
			スレッドＩＤ
		   UINT nMsg				(IN)
			通知メッセージ
		   USHORT	nCh 			(IN)
			チャンネル
		   USHORT	nBit			(IN)
			ビット
		   USHORT	nVal			(IN)
			データ
	戻値： なし
 ***********************************************************/
void CSIO_SeqCntl::SeqSioOut(
	DWORD	idThread,
	UINT	nMsg,
	USHORT	nCh,
	USHORT	nBit,
	USHORT	nVal )
{
	USHORT	nData;
	USHORT	nSendData[16];

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSioOut ==========> 開始\n" );
#endif	_SEQ_WATCH_

// シーケンサ通信変更関連(古い部分は残さず)
	CSeq_BASE::SeqSioOut( nCh, nBit, nVal );

	nData = m_SeqRROutBuff[nCh].dat;
	for( int i=0; i<16; i++)
	{
		nSendData[i] = (nData >> i) & 0x01;
	}
	Append_SendQueue( idThread, nMsg, WR_COMMAND, nCh, 16, nSendData );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSioOut ==========> 終了\n" );
#endif	_SEQ_WATCH_
}

/***********************************************************
	名前： SeqXYPosRead
	概要： ＸＹ軸位置情報の読み込み
	引数： なし
	戻値： int
			０：正常終了
			2000：停止要求
 ***********************************************************/
int CSIO_SeqCntl::SeqXYPosRead( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSeq_BASE::SeqXYPosRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;
//#	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x100, DataBuff, 16,
//#										nRcvTimeout, nRcvQueFlag ) ) )
	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0xF0, DataBuff, 32,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定
		CSeq_BASE::SeqXYPosRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXYPosRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/***********************************************************
	名前： PioOut
	概要： シーケンサ入出力リレーにデータを出力
	引数： DWORD	idThread		(IN)
			スレッドＩＤ
		   UINT nMsg				(IN)
			通知メッセージ
		   SEQ_RELAY	id			(IN)
			シーケンサ入出力リレーの ID 番号
		   USHORT	value			(IN)
			データ
	戻値： なし
 ***********************************************************/
void CSIO_SeqCntl::PioOut(
	DWORD		idThread,
	UINT		nMsg,
	SEQ_RELAY	id,
	USHORT		value )
{
	SeqSioOut( idThread, nMsg, GetSeqRelayAdrs1(id),
						GetSeqRelayAdrs2(id), value );
}

/***********************************************************
	名前： WriteDM
	概要： DMにデータを書き込む
	引数： DWORD	idThread		(IN)
			スレッドＩＤ
		   UINT nMsg				(IN)
			通知メッセージ
		   int	nMsg				(IN)
			DMアドレス
		   int		nDcnt			(IN)
			データ数
		   USHORT*	nData			(IN)
			データ
	戻値： なし
 ***********************************************************/
void CSIO_SeqCntl::WriteDM(
	DWORD		idThread,
	UINT		nMsg,
	int			nDMAdrs,
	int			nDcnt,
	USHORT*		pnData )
{
#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::WriteDM ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// DMにデータを書き込む
	Append_SendQueue( idThread, nMsg, WDXX_COMMAND,
								nDMAdrs, nDcnt, pnData );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::WriteDM ==========> 終了\n" );
#endif	_SEQ_WATCH_
}

/***********************************************************
	名前： SeqPressureRead
	概要： 圧力ゲージ情報の読み込み
	引数： なし
	戻値： int
			0   ：正常終了
			2000：停止要求
 ***********************************************************/
int CSIO_SeqCntl::SeqPressureRead( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqPressureRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x110, DataBuff, 16,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 圧力ゲージ内部データへの設定
		CSeq_BASE::SeqPressureRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqPressureRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/***********************************************************
	名前： SeqMfcFlowTempO2Read
	概要： MFCﾀｲﾏ流量温度酸素濃度情報の読み込み
	引数： なし
	戻値： int
			0   ：正常終了
			2000：停止要求
 ***********************************************************/
int CSIO_SeqCntl::SeqMfcFlowTempO2Read( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqMfcFlowTempO2Read ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x120, DataBuff, 24,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定
		CSeq_BASE::SeqMfcFlowTempO2Read( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqMfcFlowTempO2Read ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/***********************************************************
	名前： SeqErrorCodeRead
	概要： エラーコード情報の読み込み
	引数： なし
	戻値： int
			0   ：正常終了
			2000：停止要求
 ***********************************************************/
int CSIO_SeqCntl::SeqErrorCodeRead( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqErrorCodeRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x160, DataBuff, 15,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定
		CSeq_BASE::SeqErrorCodeRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqErrorCodeRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//> oku insert start 12-09-25
/***********************************************************
	名前： SeqLoadInfoRead
	概要： ローダー情報の読み込み
	引数： なし
	戻値： int
			0   ：正常終了
			2000：停止要求
 ***********************************************************/
int CSIO_SeqCntl::SeqLoaderInfoRead( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqLoaderInfoRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x170, DataBuff, 15,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定
		CSeq_BASE::SeqLoaderInfoRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqLoaderInfoRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}
//> oku insert end 12-09-25

//
// ステージベアリング圧情報の読み込み
//
int CSIO_SeqCntl::SeqStageAirBearingRead( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqStageAirBearingRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x140, DataBuff, ( AIR_BEARING_DATA_MAX + 1 ),
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定
		CSeq_BASE::SeqStageAirBearingRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqStageAirBearingRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//
// フローメータ情報の内部データへの設定
//
int CSIO_SeqCntl::SeqFlowMeterRead( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqFlowMeterRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x140, DataBuff, FLOW_METER_DATA_MAX,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定
		CSeq_BASE::SeqFlowMeterRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqFlowMeterRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//
// THETA90度位置情報の読み込み
//
int CSIO_SeqCntl::SeqTheta90degRead( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqTheta90degRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x14E, DataBuff, 2,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定
		CSeq_BASE::SeqTheta90degRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqTheta90degRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//
// X Speed 情報の内部データへの設定
//
int CSIO_SeqCntl::SeqXSpeedRead( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXSpeedRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x14C, DataBuff, 2,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定
		CSeq_BASE::SeqXSpeedRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXSpeedRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//
// Ex. Reprate 情報の内部データへの設定
//
int CSIO_SeqCntl::TrigOutReprateReadJ( void )
{
	USHORT	DataBuff[128];		// 受信データバッファ
	USHORT	nRcvTimeout;		// 受信タイムアウト時間
	USHORT	nRcvQueFlag;		// 受信データキュウーフラグ
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXSpeedRead ==========> 開始\n" );
#endif	_SEQ_WATCH_

	// 受信タイムアウト時間：500ミリ秒
	nRcvTimeout = 500;
	// 受信データキュウーフラグOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x150, DataBuff, 1,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// 内部データへの設定←送信したデータを内部データに設定
		CSeq_BASE::SeqTrigOutReprate( DataBuff );
		m_pApp->m_dJwLaserData[LASER_STATUS_EX_REPRATE] = DataBuff[0];
		int nData;
		nData = (int)DataBuff[0];
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// 停止要求有り?
		{
			return( nRet );
		}
		else
		{
			// エラー処理
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXSpeedRead ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	プライベート関数

// シーケンサ通信変更関連(古い部分は残さず)
/*---------------------------------------------------------*
	名前： MakeSeqCommand
	概要： シーケンサ通信コマンド文字列を作成する
	引数： USHORT	nCmdidx 	(IN)
			コマンドのインデックス
		   USHORT	nChNo		(IN)
			チャンネル
		   USHORT	*pDataBuff	(IN)
			書き込むデータ
		   USHORT	nDataCnt	(IN)
			データ数
		   char 	*pStr		(OUT)
			文字列格納用バッファ
	戻値： なし
 *---------------------------------------------------------*/
void CSIO_SeqCntl::MakeSeqCommand(
	USHORT	nCmdidx,
	USHORT	nChNo,
	USHORT	*pDataBuff,
	USHORT	nDataCnt,
	char	*pStr )
{
	UCHAR	fcs;
	int 	i;
	char	str1[SEQ_BUFF_SIZE];
	char	str2[SEQ_BUFF_SIZE];

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::MakeSeqCommand ==========> 開始\n" );
#endif	_SEQ_WATCH_

	switch( Seq_Command_Tbl[nCmdidx].nCom )
	{
		case RR_COMMAND:
//		case RL_COMMAND:
//		case RH_COMMAND:
//		case RC_COMMAND:
//		case RG_COMMAND:
//		case RD_COMMAND:
//		case RJ_COMMAND:
//		case RTIM_COMMAND:
//		case RTIMH_COMMAND:
//		case RCNT_COMMAND:
//		case RCNTR_COMMAND:
			sprintf( str2, Seq_Command_Tbl[nCmdidx].pSndFormat,
							SEQ_DEVICE_NO, nChNo );
			break;
		case RRXX_COMMAND:
		case RDXX_COMMAND:
		case RHXX_COMMAND:
			sprintf( str2, Seq_Command_Tbl[nCmdidx].pSndFormat,
							SEQ_DEVICE_NO, nChNo, nDataCnt );
			break;
		case WR_COMMAND:
//		case WL_COMMAND:
//		case WH_COMMAND:
//		case WC_COMMAND:
//		case WG_COMMAND:
		case WD_COMMAND:
//		case WJ_COMMAND:
//		case WTIM_COMMAND:
//		case WTIMH_COMMAND:
//		case WCNT_COMMAND:
//		case WCNTR_COMMAND:
			sprintf( str2, Seq_Command_Tbl[nCmdidx].pSndFormat,
							SEQ_DEVICE_NO, nChNo, *pDataBuff );
			break;
		case SC_COMMAND:
			sprintf( str2, Seq_Command_Tbl[nCmdidx].pSndFormat,
							SEQ_DEVICE_NO, *pDataBuff );
			break;
		case WRXX_COMMAND:
			for( i=0; i<nDataCnt; i++ )
			{
				sprintf( &str1[i*4], "%04X", pDataBuff[i]);
			}
			sprintf( str2, Seq_Command_Tbl[nCmdidx].pSndFormat,
							SEQ_DEVICE_NO, nChNo, nDataCnt*4, nDataCnt*4, str1 );
			break;
		case WDXX_COMMAND:
			for( i=0; i<nDataCnt; i++ )
			{
				sprintf( &str1[i*4], "%04d", pDataBuff[i]);
			}
			sprintf( str2, Seq_Command_Tbl[nCmdidx].pSndFormat,
							SEQ_DEVICE_NO, nChNo, nDataCnt*4, nDataCnt*4, str1 );
			break;
		case WDXSPEED_COMMAND:
			sprintf( &str1[0], "%01X", pDataBuff[1] );
			sprintf( &str1[1], "%03d", pDataBuff[0] );
			sprintf( str2, Seq_Command_Tbl[nCmdidx].pSndFormat,
							SEQ_DEVICE_NO, nChNo, 4, 4, str1 );
			break;
		case WHXX_COMMAND:
			for( i=0; i<nDataCnt; i++ )
			{
				sprintf( &str1[i*4], "%04X", pDataBuff[i]);
			}
			sprintf( str2, Seq_Command_Tbl[nCmdidx].pSndFormat,
							SEQ_DEVICE_NO, nChNo, nDataCnt*4, nDataCnt*4, str1 );
			break;
		default:
			TRACE( "CSIO_SeqCntl::MakeSeqCommand: nCmdidx Error.\n" );
	}

	fcs = CalcFCS(str2);
	sprintf( pStr, "%s%02X%c%c", str2, fcs, SEQ_TERMINATOR, SEQ_DELIMITER );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::MakeSeqCommand ==========> 終了\n" );
#endif	_SEQ_WATCH_
}

/*---------------------------------------------------------*
	名前： GetSeqAnswer
	概要： シーケンサ通信コマンドの応答データを得る
	引数： USHORT	nCmdidx 	(IN)
			コマンドのインデックス
		   USHORT	*pDataBuff	(OUT)
			読みだしたデータ
		   USHORT	nDataCnt	(IN)
			データ数
		   char 	*pStr		(IN)
			受信文字列格納バッファ
	戻値： int
			-1：受信データエラー
			０：正常終了
			１：運転モードのため実行不可
			２：モニタモードのため実行不可
			３：P-ROM のため実行不可
			４：アドレスオーバー
			11：プログラムモードのため実行不可
			12：デバッグモードのため実行不可
			13：スタンバイ／ローカルモードのため実行不可
			16：パリティエラー
			17：フレーミングエラー
			18：FCS エラー
			19：オーバーランエラー
			20：フォーマットエラー
			21：置数データエラー
			22：該当命令なし
			24：フレーム長 MAX エラー
			25：実行不可
			26：I/O テーブル作成不可
			27：PC 本体 CPU 異常
			28：該当メモリなし
			29：該当メモリプロテクト中
			160：パリティエラーのためアボート
			161：フレーミングエラーのためアボート
			162：オーバーランエラーのためアボート
			163：FCS エラーのためアボート
			164：フォーマットエラーのためアボート
			165：置数データエラーのためアボート
			168：フレーム長 MAX エラーのためアボート
			176：プログラムエリアが 16KB 以外のため実行不可
 *---------------------------------------------------------*/
int CSIO_SeqCntl::GetSeqAnswer(
	USHORT	nCmdidx,
	USHORT	*pDataBuff,
	USHORT	nDataCnt,
	char	*pStr)
{
	char	str_form[100];
	char	str2[SEQ_BUFF_SIZE];
	int 	u_no, ret_code, i;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::GetSeqAnswer ==========> 開始\n" );
#endif	_SEQ_WATCH_

	switch( Seq_Command_Tbl[nCmdidx].nCom )
	{
		case RR_COMMAND:
//		case RL_COMMAND:
//		case RH_COMMAND:
//		case RC_COMMAND:
//		case RG_COMMAND:
		case RD_COMMAND:
//		case RJ_COMMAND:
//		case RTIM_COMMAND:
//		case RTIMH_COMMAND:
//		case RCNT_COMMAND:
//		case RCNTR_COMMAND:
			if( sscanf( pStr, Seq_Command_Tbl[nCmdidx].pRcvFormat,
							&u_no, &ret_code, pDataBuff ) != 3 )
			{
				return -1;
			}
			break;
		case WR_COMMAND:
//		case WL_COMMAND:
//		case WH_COMMAND:
//		case WC_COMMAND:
//		case WG_COMMAND:
		case WD_COMMAND:
//		case WJ_COMMAND:
//		case WTIM_COMMAND:
//		case WTIMH_COMMAND:
//		case WCNT_COMMAND:
//		case WCNTR_COMMAND:
		case SC_COMMAND:
		case WRXX_COMMAND:
		case WDXX_COMMAND:
		case WDXSPEED_COMMAND:
		case WHXX_COMMAND:
			if( sscanf( pStr, Seq_Command_Tbl[nCmdidx].pRcvFormat,
							&u_no, &ret_code ) != 2 )
			{
				return -1;
			}
			break;
		case RRXX_COMMAND:
			sprintf( str_form, Seq_Command_Tbl[nCmdidx].pRcvFormat, nDataCnt*4 );
			if( sscanf( pStr, str_form, &u_no, &ret_code, str2 ) != 3 )
			{
				return -1;
			}
			for( i=0; i<nDataCnt; i++ )
			{
				if( sscanf( &str2[i*4], "%4hx", &pDataBuff[i] ) != 1 )
				{
					return 1;
				}
			}
			break;
		case RDXX_COMMAND:
			sprintf( str_form, Seq_Command_Tbl[nCmdidx].pRcvFormat, nDataCnt*4 );
			if( sscanf( pStr, str_form, &u_no, &ret_code, str2 ) != 3 )
			{
				return -1;
			}
			for( i=0; i<nDataCnt; i++ )
			{
				if( sscanf( &str2[i*4], "%4d", &pDataBuff[i] ) != 1 )
				{
					return 1;
				}
			}
			break;
		case RHXX_COMMAND:
			sprintf( str_form, Seq_Command_Tbl[nCmdidx].pRcvFormat, nDataCnt*4 );
			if( sscanf( pStr, str_form, &u_no, &ret_code, str2 ) != 3 )
			{
				return -1;
			}
			for( i=0; i<nDataCnt; i++ )
			{
				if( sscanf( &str2[i*4], "%04X", &pDataBuff[i] ) != 1 )
				{
					return 1;
				}
			}
			break;
		default:
			TRACE( "CSIO_SeqCntl::GetSeqAnswer: nCmdidx Error.\n" );
	}

	if( u_no != SEQ_DEVICE_NO )
	{
		return -1;
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::GetSeqAnswer ==========> 終了\n" );
#endif	_SEQ_WATCH_

	return ret_code;
}

/*---------------------------------------------------------*
	名前： CalcFCS
	概要： FCSを計算する
	引数： char *str		(IN)
			送信する文字列
	戻値： UCHAR
			FCS
 *---------------------------------------------------------*/
UCHAR	CSIO_SeqCntl::CalcFCS( char *str )
{
	UCHAR	fcs;

	for(fcs=0; *str; str ++)
	{
		fcs ^= *str;
	}

	return fcs;
}

/*---------------------------------------------------------*
	名前： CheckFCS
	概要： 受信したデータのFCSをチェックをする
	引数： char *str		(IN)
			受信したデータ
	戻値： int
			０：正常
			１：異常
 *---------------------------------------------------------*/
BOOL	CSIO_SeqCntl::CheckFCS( char *str )
{
	int 	fcs;
	UCHAR	fcs1, fcs2;
	char	fcs_data[5];		/* (FCS 2byte) + (terminator 2byte) + (NULL 1byte) */


	/* 受信文字列中の FCS 部分の取り出し */
	sprintf(fcs_data, "%4s", str + (strlen(str) - 4));

	fcs1 = CalcFCS(str);
	fcs2 = CalcFCS(fcs_data);
	fcs1 ^= fcs2;

	sscanf(fcs_data, "%2x", &fcs);

	return (BOOL)(fcs1!=fcs);
}

//
// 通信ポート初期化処理(CIOタグ設定)
//
int     CSIO_SeqCntl::JW_Initialize()
{
	int nRet;
	int i,j;
	int nCh;
	CString strTag;
	CString strCh;
	CString strBit;
	CString strLocal;
	CString strValue;

	// CIO設定
	strLocal = "LOCAL$IO.IO";
	strValue = "$VALUE";
	nCh = JW_CIO_FAST_CH;

	for(i=j=0; i<JW_CIO_TAG_MAX; i++ )
	{
		strCh.Format( "%d" , nCh );
		strBit.Format( "_%02d", j);
		strTag = strLocal + strCh + strBit + strValue;
		// CIOタグ名の設定
		strcpy( pJwCIOTags[i].name, strTag);
		if(j >= 15)
		{
			j=0;
			nCh++;
		}
		else
		{
			j++;
		}
	}

	nRet = JWGetTagIDS2( JW_CIO_TAG_MAX,	// 変換タグ数
					pJwCIOTags,			// タグ名へのポインタ
					256,				// タグ構造体のレコードサイズ　今回はタグ名だけなので　256単位の文字列
					0,					// タグ名構造体の　タグ名位置
					m_pJwCIOIDs,		// 変換後のタグID
					sizeof( long ),		// タグＩＤ構造体のサイズ
					0, 
					-1, 
					-1 );
	// PLCデータ読み込み時に必要
	for(i=0; i<JW_CIO_TAG_MAX; i++ )
	{
		tCIO_Data[i].col_id = m_pJwCIOIDs[i];
	}

	if( nRet == 1 )
	{
		// Monitor(W0100~017F)の設定
		strLocal = "LOCAL$MONITORDATA.T";
		strValue = "$VALUE";

		for(i=0; i<JW_MONITOR_TAG_MAX; i++ )
		{
			strCh.Format( "%03d" , i );
			strTag = strLocal + strCh + strValue;
			// Monitor(W0100~017F)タグ名の設定
			strcpy( pJwMonitorTags[i].name, strTag);
		}

		nRet = JWGetTagIDS2( JW_MONITOR_TAG_MAX,		// 変換タグ数
						pJwMonitorTags,			// タグ名へのポインタ
						256,				// タグ構造体のレコードサイズ　今回はタグ名だけなので　256単位の文字列
						0,					// タグ名構造体の　タグ名位置
						m_pJwMonitorIDs,		// 変換後のタグID
						sizeof( long ),		// タグＩＤ構造体のサイズ
						0, 
						-1, 
						-1 );
		// PLCデータ読み込み時に必要
		for(i=0; i<JW_MONITOR_TAG_MAX; i++ )
		{
			tMonitor_Data[i].col_id = m_pJwMonitorIDs[i];
		}
	}

	if( nRet == 1 )
	{
		// DataSet(W0030~007F))の設定
		strLocal = "LOCAL$DATASET.T";
		strValue = "$VALUE";

		for(i=0; i<JW_DATASET_TAG_MAX; i++ )
		{
			strCh.Format( "%02d" , i );
			strTag = strLocal + strCh + strValue;
			// DataSet(W0030~007F)タグ名の設定
			strcpy( pJwDataSetTags[i].name, strTag);
		}

		nRet = JWGetTagIDS2( JW_DATASET_TAG_MAX,		// 変換タグ数
						pJwDataSetTags,			// タグ名へのポインタ
						256,				// タグ構造体のレコードサイズ　今回はタグ名だけなので　256単位の文字列
						0,					// タグ名構造体の　タグ名位置
						m_pJwDataSetIDs,		// 変換後のタグID
						sizeof( long ),		// タグＩＤ構造体のサイズ
						0, 
						-1, 
						-1 );
		// PLCデータ読み込み時に必要
		for(i=0; i<JW_DATASET_TAG_MAX; i++ )
		{
			tDataSet_Data[i].col_id = m_pJwDataSetIDs[i];
		}
	}

	if( nRet == 1 )
	{
		// STEP(W0010~003F)の設定
		strLocal = "LOCAL$STEPDATA.T";
		strValue = "$VALUE";

		for(i=0; i<JW_STEP_TAG_MAX; i++ )
		{
			strCh.Format( "%02d" , i );
			strTag = strLocal + strCh + strValue;
			// STEP(W0010~003F)タグ名の設定
			strcpy( pJwStepTags[i].name, strTag);
		}


		nRet = JWGetTagIDS2( JW_STEP_TAG_MAX,		// 変換タグ数
						pJwStepTags,			// タグ名へのポインタ
						256,				// タグ構造体のレコードサイズ　今回はタグ名だけなので　256単位の文字列
						0,					// タグ名構造体の　タグ名位置
						m_pJwStepIDs,		// 変換後のタグID
						sizeof( long ),		// タグＩＤ構造体のサイズ
						0, 
						-1, 
						-1 );
//		// PLCデータ読み込み時に必要
//		for(i=0; i<JW_STEP_TAG_MAX; i++ )
//		{
//			tStep_Data[i].col_id = m_pJwStepIDs[i];
//		}
	}

	if( nRet == 1 )
	{
		// WorkSel(W0000~000F)の設定
		strLocal = "LOCAL$WORKSEL.T";
		strValue = "$VALUE";

		for(i=0; i<JW_WORK_TAG_MAX; i++ )
		{
			strCh.Format( "%02d" , i );
			strTag = strLocal + strCh + strValue;
			// DataSet(W0000~000F)タグ名の設定
			strcpy( pJwWorkSelTags[i].name, strTag);
		}

		nRet = JWGetTagIDS2( JW_WORK_TAG_MAX,		// 変換タグ数
						pJwWorkSelTags,			// タグ名へのポインタ
						256,				// タグ構造体のレコードサイズ　今回はタグ名だけなので　256単位の文字列
						0,					// タグ名構造体の　タグ名位置
						m_pJwWorkSelIDs,		// 変換後のタグID
						sizeof( long ),		// タグＩＤ構造体のサイズ
						0, 
						-1, 
						-1 );
		// PLCデータ読み込み時に必要
		for(i=0; i<JW_WORK_TAG_MAX; i++ )
		{
			tWorkSel_Data[i].col_id = m_pJwWorkSelIDs[i];
		}
	}

	return nRet;
}
