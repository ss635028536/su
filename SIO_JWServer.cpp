//
// SIO_JWServer.cpp : �C���v�������e�[�V���� �t�@�C��
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

// �V�[�P���T�E�R�}���h�e�[�u��
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

// ��M�f�[�^�\����
struct SRecvData
{
	// ��M������o�b�t�@
	char	RcvBuf[SEQ_BUFF_SIZE];
	// ��M������f�[�^�����O�X
	int 	nRcvLength;
	// ��M�f�[�^�o�b�t�@
	USHORT	DataBuf[SEQ_BUFF_SIZE];
	// ��M�f�[�^��
	int 	nDataCnt;
};

// ���M�R�}���h�\����
struct SSendCmd
{
	// �X���b�h�h�c
	DWORD	idThread;
	// �f�[�^�̎�M��ʒm���郁�b�Z�[�W�R�[�h
	UINT	nMsg;
	// ���M�R�}���h�ԍ�
	USHORT	nCmdNo;
	// CH�ԍ�
	USHORT	nChNo;
	// �f�[�^��(���M/��M)
	USHORT	nDataCnt;
	// ���M�f�[�^�o�b�t�@
	USHORT	DataBuf[SEQ_BUFF_SIZE];
};

//	�V�[�P���T�ʐM�X���b�h
UINT SeqComThread( LPVOID pParam )
{
	CSIO_SeqCntl	*pSIO_JWServer = ( CSIO_SeqCntl* )pParam;
	SSendCmd	*pSendCmd;
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nExitFlag;			// ��~�v���t���O
	int 	nRet;
	int		nCounter = 0;
	CLAApp*	pApp;
	CString strError;
	// ���[�U�A�j�[���A�v���P�[�V�����N���X�̐ݒ�
	pApp = ( CLAApp* )AfxGetApp( );

	TRACE( "SeqComThread: nPort : %d : Thread start.\n", pSIO_JWServer->m_nPort );

	// ��~�v���t���OOFF
	nExitFlag = OFF;
	// ��M�f�[�^�L���E�[�t���OON
	nRcvQueFlag = ON;

	while ( TRUE )
	{
		if ( pSIO_JWServer->ExitRequest( ) )
			break;
		else
		{
			if ( pSIO_JWServer->Remove_SendQueue( &pSendCmd ) )
			{
				// ��M�^�C���A�E�g���Ԃ����߂�
				nRcvTimeout = Seq_Command_Tbl[pSendCmd->nCmdNo].nRcvTimeout * 100;
				// �V�[�P���T�Ƃ̒ʐM
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
					TRACE( "@@@@@@@@@@@@@@PostMessage(RTN_ERROR) �����Ȃ�\n" );
				}
				delete pSendCmd;
			}
			if( nExitFlag == ON )
				break;

			if( pSIO_JWServer->m_nIdleFg )	// �A�C�h�����̒ʐM���s��?
			{
				// ���o�̓����[�̓ǂݍ���
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
				// XY���ʒu���̓ǂݍ���
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
				// ���̓Q�[�W���̓ǂݍ���
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
				// MFC��ϗ��ʉ��x�_�f�Z�x���̓ǂݍ���
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
				// �G�A�[�x�A�����O���̓ǂݍ���
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
				// �t���[���[�^���̓ǂݍ���
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
				// X Speed ���̓ǂݍ���
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
				// THETA90�x�ʒu���̓ǂݍ���
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
				// �G���[�R�[�h���̓ǂݍ���
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
				// ���[�_���̓ǂݍ���
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
		// �����̈ꎞ��~
		Sleep( SEQCOM_THREAD_WAIT );
	}

	TRACE( "SeqComThread: nPort : %d : Thread exit.\n", pSIO_JWServer->m_nPort );

	AfxEndThread( 0 );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CSIO_Control

//
//	�R���X�g���N�^
//
CSIO_SeqCntl::CSIO_SeqCntl( SSIO_Seq_Param& sParam )
{
	int		nStatus;

	TRACE( "CSIO_SeqCntl::CSIO_SeqCntl ==========> �J�n\n" );

	InitializeCriticalSection( &m_csPlcSendLock );
	InitializeCriticalSection( &m_csPlcRcvDataLock );
	// ���[�U�A�j�[���A�v���P�[�V�����N���X�̐ݒ�
	m_pApp = ( CLAApp* )AfxGetApp( );

	// �p�����[�^�̕ۑ�
	m_nPort = sParam.nPort;					// �|�[�g�ԍ�
	m_dwBaudrate = sParam.dwBaudrate;		// �{�[���[�g
	m_dwDataBits = sParam.dwDataBits;		// �f�[�^�r�b�g��
	m_dwStopBits = sParam.dwStopBits;		// �X�g�b�v�r�b�g��
	m_dwParity = sParam.dwParity;			// �p���e�B
	m_dwXonXoffFlow = sParam.dwXonXoffFlow;	// �t���[����
	m_dwXonCode = sParam.dwXonCode;			// XON�R�[�h
	m_dwXoffCode = sParam.dwXoffCode;		// XOFF�R�[�h
	m_dwHardFlow = sParam.dwHardFlow;		// �n�[�h�E�F�A�t���[����
	m_dwDuplex = sParam.dwDuplex;			// �S��d/����d
	m_dwRcvSize = sParam.dwRcvSize;			// ��M�o�b�t�@�T�C�Y

	m_bDelimiter = sParam.bDelimiter;		// �f���~�^����w��
	m_byDelimiter = sParam.byDelimiter;		// �f���~�^�R�[�h
	m_nReadTime = sParam.nReadTime;			// ��M�Ď�����
	m_nWriteTime = sParam.nWriteTime;		// ���M�Ď�����

	m_uMessage = sParam.uMessage;			// ���b�Z�[�W�R�[�h
	m_nIdleFg = sParam.nIdleFg;				// �A�C�h�����ʐM�t���O


	m_pSeqComThread = NULL;

	// �ʐM�|�[�g����������(CIO�^�O�ݒ�)
	if( nStatus = JW_Initialize() == 1 )
	{
		m_bThreadExit = FALSE;
		// �ʐM�X���b�h�̋N��
		if ( ( m_pSeqComThread = AfxBeginThread( SeqComThread, ( LPVOID )this ) ) != NULL )
		{
			// �����폜�̎w�����U�ɐݒ�
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

	TRACE( "CSIO_SeqCntl::CSIO_SeqCntl ==========> �I��\n" );
}

//
//	�f�X�g���N�^
//
CSIO_SeqCntl::~CSIO_SeqCntl( )
{
	DWORD	dwExitCode;
	int 	nCount = 0;

	TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl ==========> �J�n\n" );

	if ( m_pSeqComThread != NULL )
	{
		// ��M�X���b�h�̒�~�w��
		m_bThreadExit = TRUE;
		// �ȉ����J��Ԃ�
		while ( TRUE )
		{
			// �X���b�h�I���R�[�h�̎擾
			if ( ::GetExitCodeThread( m_pSeqComThread->m_hThread, &dwExitCode ) != FALSE )
			{
				// �X���b�h�I���̏ꍇ
				if ( dwExitCode != STILL_ACTIVE )
					// ���[�v���甲����
					break;
				else
				{
					// �Ď��J�E���^������l�ɒB�����ꍇ
					if ( nCount == ( THREAD_KILL_WAIT_TIME / THREAD_KILL_TICK_TIME ) )
						// ���[�v���甲����
						break;
					else
						// �Ď��J�E���^���X�V
						nCount++;
				}
			}
			else
			{
				TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl: [GetExitCodeThread] Failure.\n" );
				break;
			}
			// �����̈ꎞ��~
			Sleep( THREAD_KILL_TICK_TIME );
		}
		// �Ď��J�E���^������l�ɒB���Ă��Ȃ��Ȃ��ꍇ
		if ( nCount < ( THREAD_KILL_WAIT_TIME / THREAD_KILL_TICK_TIME ) )
		{
			TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl: Success.\n" );
		}
		else
		{
			TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl: Failure.\n" );
		}

		// �ʐM�|�[�g�I������
		Terminate();

		// ��M�X���b�h�̔p��
		delete m_pSeqComThread;
	}

	EnterCriticalSection( &m_csPlcRcvDataLock );
	SRecvData*	pRecvData;
	// ��M�f�[�^�L���[�̔p��
	while( ! m_listRecvData.IsEmpty( ) )
	{
		// �L���[�̐擪����f�[�^���擾
		pRecvData = m_listRecvData.RemoveHead( );
		delete pRecvData;
	}
	LeaveCriticalSection( &m_csPlcRcvDataLock );

	EnterCriticalSection( &m_csPlcSendLock );
	SSendCmd*  pSendCmd;
	// ���M�R�}���h�L���[�̔p��
	while( ! m_listSendCmd.IsEmpty( ) )
	{
		// �L���[�̐擪����f�[�^���擾
		pSendCmd = m_listSendCmd.RemoveHead( );
		delete pSendCmd;
	}
	LeaveCriticalSection( &m_csPlcSendLock );

	DeleteCriticalSection( &m_csPlcSendLock );
	DeleteCriticalSection( &m_csPlcRcvDataLock );
#ifndef _DISP_ONLY_
	// JW�T�[�o�ʐM�ؒf
	DisconnectNet( );
#endif

	TRACE( "CSIO_SeqCntl::~CSIO_SeqCntl ==========> �I��\n" );
}

/////////////////////////////////////////////////////////////////////////////
//	�p�u���b�N�֐�

//
//	��M�f�[�^�L���[���̃f�[�^���̎擾
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
//	��M�f�[�^�L���[�ւ̒ǉ�
//
void CSIO_SeqCntl::Append_RecvQueue(
	char	*pRcvBuf,
	USHORT	*pDataBuf,
	int 	nDataCnt )
{
	SRecvData*	pRecvData = new SRecvData;
	int 	i;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::Append_RecvQueue ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M������̕ۑ�
	strcpy( pRecvData->RcvBuf, pRcvBuf );
	pRecvData->nRcvLength = strlen( pRcvBuf );
	// ��M�f�[�^�̕ۑ�
	for( i = 0; i < nDataCnt; i++ )
		pRecvData->DataBuf[i] = pDataBuf[i];
	pRecvData->nDataCnt = nDataCnt;
	// �L���[�̍Ō�ɒǉ�
	EnterCriticalSection( &m_csPlcRcvDataLock );
	m_listRecvData.AddTail( pRecvData );
	LeaveCriticalSection( &m_csPlcRcvDataLock );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::Append_RecvQueue ==========> �I��\n" );
#endif	_SEQ_WATCH_
}

//
//	���M�R�}���h�L���[���̃f�[�^���̎擾
//
int CSIO_SeqCntl::GetSendQ_Count( )
{
	EnterCriticalSection( &m_csPlcSendLock );
	int		nRet = m_listSendCmd.GetCount( );
	LeaveCriticalSection( &m_csPlcSendLock );

	return nRet;
}

//
//	���M�R�}���h�L���[�ւ̒ǉ�
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
	TRACE( "CSIO_SeqCntl::Append_SendQueue ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ���M�R�}���h�̕ۑ�
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
	// �L���[�̍Ō�ɒǉ�
	m_listSendCmd.AddTail( pSendCmd );
	LeaveCriticalSection( &m_csPlcSendLock );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::Append_SendQueue ==========> �I��\n" );
#endif	_SEQ_WATCH_
}

//
// ���M�R�}���h�L���[����擪�f�[�^�����o��
//
int	CSIO_SeqCntl::Remove_SendQueue( SSendCmd** pSendCmd )
{
	EnterCriticalSection( &m_csPlcSendLock );
	int	nRet = m_listSendCmd.GetCount( );

	// ���M�R�}���h�L���[���Ƀf�[�^������ꍇ
	if( nRet )
	{
		// �L���[�̐擪����f�[�^���擾
		*pSendCmd = m_listSendCmd.RemoveHead( );
	}
	LeaveCriticalSection( &m_csPlcSendLock );

	return nRet;
}

//
//	�f�[�^�̎�M
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
	TRACE( "CSIO_SeqCntl::DataRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	EnterCriticalSection( &m_csPlcRcvDataLock );
	if ( ! m_listRecvData.IsEmpty( ) )
	{
		SRecvData*	pRecvData;

		// �L���[�̐擪����f�[�^���擾
		pRecvData = m_listRecvData.RemoveHead( );
		// ��M������̐ݒ�
		*pnBufLength = pRecvData->nRcvLength;
		strcpy( pRcvBuf, pRecvData->RcvBuf );
		// ��M�f�[�^�̐ݒ�
		*pnDataCnt = pRecvData->nDataCnt;
		for( i = 0 ; i < pRecvData->nDataCnt ; i++ )
			pDataBuf[i] = pRecvData->DataBuf[i];
		delete pRecvData;

		Rcode = TRUE;
	}
	LeaveCriticalSection( &m_csPlcRcvDataLock );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::DataRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( Rcode );
}

//
//	�X���b�h��~�w��
//
BOOL CSIO_SeqCntl::ExitRequest( )
{
	// ��~�w���������ďI��
	return m_bThreadExit;
}

/*---------------------------------------------------------*
	���O�F SeqCommunication
	�T�v�F �V�[�P���T�Ƃ̒ʐM
	�����F USHORT	nCmdidx (IN)
			�R�}���h�̃C���f�b�N�X
		   USHORT	nChNo	(IN)
			�`�����l��
		   USHORT	*pDataBuff	(OUT)
			�������ރf�[�^�^�ǂ݂������f�[�^
		   USHORT	nDataCnt	(IN)
			�f�[�^��
		   USHORT	nRcvTimeout (IN)
			��M�^�C���A�E�g����
		   USHORT	nRcvQueFlag (IN)
			��M�f�[�^�L���[�t���O
	�ߒl�F int
			�O�F����I��
			1000�F�ʐM�G���[���g���C�񐔃I�[�o
			2000�F��~�v��
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
	TRACE( "CSIO_SeqCntl::SeqCommunication ==========> �J�n\n" );
#endif	_SEQ_WATCH_

#ifdef	_DISP_ONLY_
	return( 0 );/* �V�[�P���T�g�p���ĂȂ��Ƃ��͂��ׂĐ���I���Ƃ��� */
#endif	_DISP_ONLY_

	nRetryCount = RETRY_COUNT;

	USHORT	DataBuff[SEQ_BUFF_SIZE];	// ��M�f�[�^�o�b�t�@
	int		nData;
	int		nTagNum;
	int		i,nValSize;
	char	*pBuf,*pD;
	CString strData;

	int nCom;

	switch( nCom = nCmdidx )
	{
		// �ǂݍ��݂̏ꍇ
		case RRXX_COMMAND:
		case RDXX_COMMAND:
		case RHXX_COMMAND:
			TCOM_DATA1	*pDat;
			int nCnt;
			// �擪�A�h���X�̐ݒ�
			// CIO�̏ꍇ
			if( (nCom == RRXX_COMMAND) && ( nChNo >= JW_CIO_FAST_CH  && nChNo < (JW_CIO_FAST_CH + JW_CIO_CH_CNT) ) )
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = (nChNo - JW_CIO_FAST_CH) * 16;
				// �擪�A�h���X�̐ݒ�
				pDat = &tCIO_Data[nTagNum];
				// �f�[�^���̐ݒ�
				nCnt = nDataCnt * 16;

				// JW�T�[�o�f�[�^�̓ǂݍ���
				nRet = JWRead(
					0,				// UserID ::GUEST
					"",				// Passwd :: PWD����
					nCnt,			// �ǂݍ��݃^�O�T
					pDat			// �ǂݍ��ݐ擪�^�OID
					);
				// �ǂݍ��ݐ����̏ꍇ
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
			// MonitorData(W0100~017F)�̏ꍇ
			else if( ( nCom == RDXX_COMMAND ||nCom == RHXX_COMMAND  ) && (nChNo >= JW_MONITOR_FAST_CH && nChNo < (JW_MONITOR_FAST_CH + JW_MONITOR_TAG_MAX) ) )
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = nChNo - JW_MONITOR_FAST_CH;
				// �擪�A�h���X�̐ݒ�
				pDat = &tMonitor_Data[nTagNum];
				// �f�[�^���̐ݒ�
				nCnt = nDataCnt;

				// JW�T�[�o�f�[�^�̓ǂݍ���
				nRet = JWRead(
					0,				// UserID ::GUEST
					"",				// Passwd :: PWD����
					nCnt,			// �ǂݍ��݃^�O�T
					pDat			// �ǂݍ��ݐ擪�^�OID
					);
				// �ǂݍ��ݐ����̏ꍇ
				if( nRet == 0 )
				{
					// BCD�f�[�^�̏ꍇ
					if( nCom == RDXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)BCDtoINT( (int)tMonitor_Data[nTagNum+i].dblVal );
						}
					}
					// 16�i�f�[�^�̏ꍇ
					else if( nCom == RHXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)tMonitor_Data[nTagNum+i].dblVal;
						}
					}
				}
			}
			// DataSet(W0030~007F)�̏ꍇ
			else if( ( nCom == RDXX_COMMAND ||nCom == RHXX_COMMAND  ) && (nChNo >= JW_DATASET_FAST_CH && nChNo < (JW_DATASET_FAST_CH + JW_DATASET_TAG_MAX) ) )
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = nChNo - JW_DATASET_FAST_CH;
				// �擪�A�h���X�̐ݒ�
				pDat = &tDataSet_Data[nTagNum];
				// �f�[�^���̐ݒ�
				nCnt = nDataCnt;

				// JW�T�[�o�f�[�^�̓ǂݍ���
				nRet = JWRead(
					0,				// UserID ::GUEST
					"",				// Passwd :: PWD����
					nCnt,			// �ǂݍ��݃^�O�T
					pDat			// �ǂݍ��ݐ擪�^�OID
					);
				// �ǂݍ��ݐ����̏ꍇ
				if( nRet == 0 )
				{
					// BCD�f�[�^�̏ꍇ
					if( nCom == RDXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)BCDtoINT( (int)tDataSet_Data[nTagNum+i].dblVal );
						}
					}
					// 16�i�f�[�^�̏ꍇ
					else if( nCom == RHXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)tDataSet_Data[nTagNum+i].dblVal;
						}
					}
				}
			}
			// WorkSel(����I��)(W0000~000F)�̏ꍇ
			else if( ( nCom == RDXX_COMMAND ||nCom == RHXX_COMMAND  ) && (nChNo >= JW_WORK_FAST_CH && nChNo < (JW_WORK_FAST_CH + JW_WORK_TAG_MAX) ) )
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = nChNo;
				// �擪�A�h���X�̐ݒ�
				pDat = &tWorkSel_Data[nTagNum];
				// �f�[�^���̐ݒ�
				nCnt = nDataCnt;

				// JW�T�[�o�f�[�^�̓ǂݍ���
				nRet = JWRead(
					0,				// UserID ::GUEST
					"",				// Passwd :: PWD����
					nCnt,			// �ǂݍ��݃^�O�T
					pDat			// �ǂݍ��ݐ擪�^�OID
					);
				// �ǂݍ��ݐ����̏ꍇ
				if( nRet == 0 )
				{
					// BCD�f�[�^�̏ꍇ
					if( nCom == RDXX_COMMAND )
					{
						for( i = 0; i < nDataCnt; i++ )
						{
							DataBuff[i] = (USHORT)BCDtoINT( (int)tWorkSel_Data[nTagNum+i].dblVal );
						}
					}
					// 16�i�f�[�^�̏ꍇ
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
		// �������݂̏ꍇ
		case WR_COMMAND:	// CIO��������
		case WDXX_COMMAND:	// BCD�f�[�^��������
		case WHXX_COMMAND:
			long *pFasrTag;
			// �f�[�^�̐ݒ�
			nValSize = ( ( sizeof( double ) )* nDataCnt );
			pD = pBuf = new char[nValSize];
			for ( i = 0; i < nDataCnt; i++ ){
				if( nCom ==WDXX_COMMAND )
				{
					// �P�O�i�f�[�^���P�U�i�f�[�^�Ƃ��Ĉ���
					nData = INTtoBCD( pDataBuff[i] );
				}
				else
					nData = pDataBuff[i];

				*(double *)pD = nData;
				pD += sizeof(double );
			}

			// �擪�A�h���X�̐ݒ�
			// CIO�̏ꍇ
			if( ( nCom == WR_COMMAND ) && ( nChNo >= JW_CIO_FAST_CH && nChNo < (JW_CIO_FAST_CH + JW_CIO_CH_CNT) ) )
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = (nChNo - JW_CIO_FAST_CH) * 16;
				// �擪�A�h���X�̐ݒ�
				pFasrTag = &m_pJwCIOIDs[nTagNum];
			}
			// MONITOR(W0100~017F)�̏ꍇ
			else if( ( nCom == WDXX_COMMAND || nCom == WHXX_COMMAND ) && ( nChNo >= JW_MONITOR_FAST_CH && nChNo < (JW_MONITOR_FAST_CH + JW_MONITOR_TAG_MAX) ) )
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = nChNo - JW_MONITOR_FAST_CH;
				// �擪�A�h���X�̐ݒ�
				pFasrTag = &m_pJwMonitorIDs[nTagNum];
			}
			// DataSet(W0030~008F)�̏ꍇ
			else if( ( nCom == WDXX_COMMAND || nCom == WHXX_COMMAND ) && ( nChNo >= JW_DATASET_FAST_CH && nChNo < (JW_DATASET_FAST_CH + JW_DATASET_TAG_MAX) ) )
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = nChNo - JW_DATASET_FAST_CH;
				// �擪�A�h���X�̐ݒ�
				pFasrTag = &m_pJwDataSetIDs[nTagNum];
			}
			// STEP(W0010~003F)�̏ꍇ
			else if( ( nCom == WDXX_COMMAND || nCom == WHXX_COMMAND ) && ( nChNo >= JW_STEP_FAST_CH && nChNo < (JW_STEP_FAST_CH + JW_STEP_TAG_MAX) ) ) 
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = nChNo - JW_STEP_FAST_CH;
				// �擪�A�h���X�̐ݒ�
				pFasrTag = &m_pJwStepIDs[nTagNum];
			}
			// WorkSel(W0000~000F)�̏ꍇ
			else if( ( nCom == WDXX_COMMAND || nCom == WHXX_COMMAND ) && ( nChNo >= JW_WORK_FAST_CH && nChNo < (JW_WORK_FAST_CH + JW_WORK_TAG_MAX) ) )
			{
				// �擪�^�O�ԍ��̐ݒ�
				nTagNum = nChNo;
				// �擪�A�h���X�̐ݒ�
				pFasrTag = &m_pJwWorkSelIDs[nTagNum];
			}
			else
			{
				// �G���[
				nRet = -999;
				delete pBuf;
				break;
			}

			// JW�T�[�o�Ƀf�[�^��������
			nRet = JWWrite( 0,					// UserID ::GUEST
					"",							// Passwd :: PWD����
					0,							//�@��ɂO
					nDataCnt,					// �������݃^�O��
					nValSize,					// PACK �����f�[�^�o�C�g�� ���l�̏ꍇ�ɂ́@8 * nn
												// ���l�͏��double�Ƃ��Ĉ���
					pFasrTag,		// �������݃^�O����ׂ��z��̐擪�A�h���X
					pBuf						// �f�[�^���p�b�N�����擪�A�h���X
												// ���l�n�̏ꍇ�ɂ́@double �̔z��Ɠ���
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
			// ��M�����f�[�^���L���[�ɒǉ�
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
	���O�F SeqSend
	�T�v�F �V�[�P���T�Ƀf�[�^�𑗐M����
	�����F char 	*pBuf		(IN)
			���M�f�[�^
		   int		nLength 	(IN)
			���M�f�[�^��
	�ߒl�F BOOL
			�P�F����I��
			�O�F�G���[
 *---------------------------------------------------------*/
BOOL CSIO_SeqCntl::SeqSend(
	char	*pBuf,
	int 	nLength )
{
	BOOL	rc;
	int		nStatus;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSend ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// �f�[�^�̑��M
	nStatus = Send(pBuf, nLength);

	if( nStatus)
	{
		rc = FALSE;
		TRACE( "CSIO_SeqCntl::SeqSend: [Send] Failure. Error Code : %d\n", nStatus);
	}
	else
		rc = TRUE;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSend ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return rc;
}

/*---------------------------------------------------------*
	���O�F SeqRecv
	�T�v�F �V�[�P���T����f�[�^����M����
	�����F char 	*pBuf		(OUT)
			��M�f�[�^
		   int		*pnLength	(OUT)
			��M�f�[�^��
		   int		nRTime		(IN)
			��M�^�C���A�E�g���ԁi100msec�P�ʁj
	�ߒl�F BOOL
			�P�F����I��
			�O�F�G���[
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
	TRACE( "CSIO_SeqCntl::SeqRecv ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// �f�[�^�̎�M
	nStatus = Recv( pBuf, pnLength, nRTime);

	if( nStatus == 0)
	{
		rc = TRUE;
		pStr = strchr( pBuf, SEQ_DELIMITER );
		if( pStr != NULL )
		{
			*(pStr+1) = '\0';
			*pnLength = strlen( pBuf ); // ��M�f�[�^���̃Z�b�g
		}
		else
		{
			ASSERT(0);	// �G���[
		}
	}
	else
	{
		rc = FALSE;
		TRACE( "CSIO_SeqCntl::SeqRecv: [Recv] Failure. Error Code : %d\n", nStatus);
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqRecv ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return rc;
}

/*---------------------------------------------------------*
	���O�F ResetComError
	�T�v�F �ʐM�G���[�����Z�b�g����
	�����F �Ȃ�
	�ߒl�F int
			1�F����I��
			0�F�G���[
 *---------------------------------------------------------*/
BOOL CSIO_SeqCntl::ResetComError( void )
{
	BOOL	rc;
	int		nStatus;

	TRACE( "CSIO_SeqCntl::ResetComError ==========> �J�n\n" );

	nStatus = ErrorReset();

	if ( nStatus)
	{
		rc = FALSE;
		TRACE( "CSIO_SeqCntl::ResetComError: [ErrorReset] Failure. Error Code : %d\n", nStatus);
	}
	else
		rc = TRUE;

	TRACE( "CSIO_SeqCntl::ResetComError ==========> �I��\n" );

	return rc;
}

// �V�[�P���T�ʐM�ύX�֘A(�ꕔ��Seq_BASE�ֈړ����Â������͎c����)
/***********************************************************
	���O�F SeqSioRead
	�T�v�F ���o�̓����[�G���A�̓ǂݍ���
	�����F �Ȃ�
	�ߒl�F int
			0   �F����I��
			2000�F��~�v��
 ***********************************************************/
int CSIO_SeqCntl::SeqSioRead( void )
{
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	USHORT	DataBuff[SEQ_BUFF_SIZE];	// ���o�̓����[�f�[�^�o�b�t�@
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSioRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( nRet = SeqCommunication( RRXX_COMMAND, JW_CIO_FAST_CH, DataBuff,
									JW_CIO_CH_CNT, nRcvTimeout, nRcvQueFlag ) )
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}
	else
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqSioRead( DataBuff );
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSioRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/***********************************************************
	���O�F SeqSioOut
	�T�v�F ���o�̓����[��������
	�����F DWORD	idThread		(IN)
			�X���b�h�h�c
		   UINT nMsg				(IN)
			�ʒm���b�Z�[�W
		   USHORT	nCh 			(IN)
			�`�����l��
		   USHORT	nBit			(IN)
			�r�b�g
		   USHORT	nVal			(IN)
			�f�[�^
	�ߒl�F �Ȃ�
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
	TRACE( "CSIO_SeqCntl::SeqSioOut ==========> �J�n\n" );
#endif	_SEQ_WATCH_

// �V�[�P���T�ʐM�ύX�֘A(�Â������͎c����)
	CSeq_BASE::SeqSioOut( nCh, nBit, nVal );

	nData = m_SeqRROutBuff[nCh].dat;
	for( int i=0; i<16; i++)
	{
		nSendData[i] = (nData >> i) & 0x01;
	}
	Append_SendQueue( idThread, nMsg, WR_COMMAND, nCh, 16, nSendData );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqSioOut ==========> �I��\n" );
#endif	_SEQ_WATCH_
}

/***********************************************************
	���O�F SeqXYPosRead
	�T�v�F �w�x���ʒu���̓ǂݍ���
	�����F �Ȃ�
	�ߒl�F int
			�O�F����I��
			2000�F��~�v��
 ***********************************************************/
int CSIO_SeqCntl::SeqXYPosRead( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSeq_BASE::SeqXYPosRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;
//#	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x100, DataBuff, 16,
//#										nRcvTimeout, nRcvQueFlag ) ) )
	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0xF0, DataBuff, 32,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqXYPosRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXYPosRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/***********************************************************
	���O�F PioOut
	�T�v�F �V�[�P���T���o�̓����[�Ƀf�[�^���o��
	�����F DWORD	idThread		(IN)
			�X���b�h�h�c
		   UINT nMsg				(IN)
			�ʒm���b�Z�[�W
		   SEQ_RELAY	id			(IN)
			�V�[�P���T���o�̓����[�� ID �ԍ�
		   USHORT	value			(IN)
			�f�[�^
	�ߒl�F �Ȃ�
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
	���O�F WriteDM
	�T�v�F DM�Ƀf�[�^����������
	�����F DWORD	idThread		(IN)
			�X���b�h�h�c
		   UINT nMsg				(IN)
			�ʒm���b�Z�[�W
		   int	nMsg				(IN)
			DM�A�h���X
		   int		nDcnt			(IN)
			�f�[�^��
		   USHORT*	nData			(IN)
			�f�[�^
	�ߒl�F �Ȃ�
 ***********************************************************/
void CSIO_SeqCntl::WriteDM(
	DWORD		idThread,
	UINT		nMsg,
	int			nDMAdrs,
	int			nDcnt,
	USHORT*		pnData )
{
#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::WriteDM ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// DM�Ƀf�[�^����������
	Append_SendQueue( idThread, nMsg, WDXX_COMMAND,
								nDMAdrs, nDcnt, pnData );

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::WriteDM ==========> �I��\n" );
#endif	_SEQ_WATCH_
}

/***********************************************************
	���O�F SeqPressureRead
	�T�v�F ���̓Q�[�W���̓ǂݍ���
	�����F �Ȃ�
	�ߒl�F int
			0   �F����I��
			2000�F��~�v��
 ***********************************************************/
int CSIO_SeqCntl::SeqPressureRead( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqPressureRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x110, DataBuff, 16,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// ���̓Q�[�W�����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqPressureRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqPressureRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/***********************************************************
	���O�F SeqMfcFlowTempO2Read
	�T�v�F MFC��ϗ��ʉ��x�_�f�Z�x���̓ǂݍ���
	�����F �Ȃ�
	�ߒl�F int
			0   �F����I��
			2000�F��~�v��
 ***********************************************************/
int CSIO_SeqCntl::SeqMfcFlowTempO2Read( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqMfcFlowTempO2Read ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x120, DataBuff, 24,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqMfcFlowTempO2Read( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqMfcFlowTempO2Read ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/***********************************************************
	���O�F SeqErrorCodeRead
	�T�v�F �G���[�R�[�h���̓ǂݍ���
	�����F �Ȃ�
	�ߒl�F int
			0   �F����I��
			2000�F��~�v��
 ***********************************************************/
int CSIO_SeqCntl::SeqErrorCodeRead( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqErrorCodeRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x160, DataBuff, 15,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqErrorCodeRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqErrorCodeRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//> oku insert start 12-09-25
/***********************************************************
	���O�F SeqLoadInfoRead
	�T�v�F ���[�_�[���̓ǂݍ���
	�����F �Ȃ�
	�ߒl�F int
			0   �F����I��
			2000�F��~�v��
 ***********************************************************/
int CSIO_SeqCntl::SeqLoaderInfoRead( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqLoaderInfoRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x170, DataBuff, 15,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqLoaderInfoRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqLoaderInfoRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}
//> oku insert end 12-09-25

//
// �X�e�[�W�x�A�����O�����̓ǂݍ���
//
int CSIO_SeqCntl::SeqStageAirBearingRead( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqStageAirBearingRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x140, DataBuff, ( AIR_BEARING_DATA_MAX + 1 ),
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqStageAirBearingRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqStageAirBearingRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//
// �t���[���[�^���̓����f�[�^�ւ̐ݒ�
//
int CSIO_SeqCntl::SeqFlowMeterRead( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqFlowMeterRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x140, DataBuff, FLOW_METER_DATA_MAX,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqFlowMeterRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqFlowMeterRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//
// THETA90�x�ʒu���̓ǂݍ���
//
int CSIO_SeqCntl::SeqTheta90degRead( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqTheta90degRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x14E, DataBuff, 2,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqTheta90degRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqTheta90degRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//
// X Speed ���̓����f�[�^�ւ̐ݒ�
//
int CSIO_SeqCntl::SeqXSpeedRead( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXSpeedRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x14C, DataBuff, 2,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ�
		CSeq_BASE::SeqXSpeedRead( DataBuff );
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
			return( nRet );
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXSpeedRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

//
// Ex. Reprate ���̓����f�[�^�ւ̐ݒ�
//
int CSIO_SeqCntl::TrigOutReprateReadJ( void )
{
	USHORT	DataBuff[128];		// ��M�f�[�^�o�b�t�@
	USHORT	nRcvTimeout;		// ��M�^�C���A�E�g����
	USHORT	nRcvQueFlag;		// ��M�f�[�^�L���E�[�t���O
	int 	nRet;

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXSpeedRead ==========> �J�n\n" );
#endif	_SEQ_WATCH_

	// ��M�^�C���A�E�g���ԁF500�~���b
	nRcvTimeout = 500;
	// ��M�f�[�^�L���E�[�t���OOFF
	nRcvQueFlag = OFF;

	if( !( nRet = SeqCommunication( RHXX_COMMAND, 0x150, DataBuff, 1,
										nRcvTimeout, nRcvQueFlag ) ) )
	{
		// �����f�[�^�ւ̐ݒ聩���M�����f�[�^������f�[�^�ɐݒ�
		CSeq_BASE::SeqTrigOutReprate( DataBuff );
		m_pApp->m_dJwLaserData[LASER_STATUS_EX_REPRATE] = DataBuff[0];
		int nData;
		nData = (int)DataBuff[0];
	}
	else
	{
		if( nRet == RTN_STOP_REQ )	// ��~�v���L��?
		{
			return( nRet );
		}
		else
		{
			// �G���[����
		}
	}

#ifdef	_SEQ_WATCH_
	TRACE( "CSIO_SeqCntl::SeqXSpeedRead ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return( 0 );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	�v���C�x�[�g�֐�

// �V�[�P���T�ʐM�ύX�֘A(�Â������͎c����)
/*---------------------------------------------------------*
	���O�F MakeSeqCommand
	�T�v�F �V�[�P���T�ʐM�R�}���h��������쐬����
	�����F USHORT	nCmdidx 	(IN)
			�R�}���h�̃C���f�b�N�X
		   USHORT	nChNo		(IN)
			�`�����l��
		   USHORT	*pDataBuff	(IN)
			�������ރf�[�^
		   USHORT	nDataCnt	(IN)
			�f�[�^��
		   char 	*pStr		(OUT)
			������i�[�p�o�b�t�@
	�ߒl�F �Ȃ�
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
	TRACE( "CSIO_SeqCntl::MakeSeqCommand ==========> �J�n\n" );
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
	TRACE( "CSIO_SeqCntl::MakeSeqCommand ==========> �I��\n" );
#endif	_SEQ_WATCH_
}

/*---------------------------------------------------------*
	���O�F GetSeqAnswer
	�T�v�F �V�[�P���T�ʐM�R�}���h�̉����f�[�^�𓾂�
	�����F USHORT	nCmdidx 	(IN)
			�R�}���h�̃C���f�b�N�X
		   USHORT	*pDataBuff	(OUT)
			�ǂ݂������f�[�^
		   USHORT	nDataCnt	(IN)
			�f�[�^��
		   char 	*pStr		(IN)
			��M������i�[�o�b�t�@
	�ߒl�F int
			-1�F��M�f�[�^�G���[
			�O�F����I��
			�P�F�^�]���[�h�̂��ߎ��s�s��
			�Q�F���j�^���[�h�̂��ߎ��s�s��
			�R�FP-ROM �̂��ߎ��s�s��
			�S�F�A�h���X�I�[�o�[
			11�F�v���O�������[�h�̂��ߎ��s�s��
			12�F�f�o�b�O���[�h�̂��ߎ��s�s��
			13�F�X�^���o�C�^���[�J�����[�h�̂��ߎ��s�s��
			16�F�p���e�B�G���[
			17�F�t���[�~���O�G���[
			18�FFCS �G���[
			19�F�I�[�o�[�����G���[
			20�F�t�H�[�}�b�g�G���[
			21�F�u���f�[�^�G���[
			22�F�Y�����߂Ȃ�
			24�F�t���[���� MAX �G���[
			25�F���s�s��
			26�FI/O �e�[�u���쐬�s��
			27�FPC �{�� CPU �ُ�
			28�F�Y���������Ȃ�
			29�F�Y���������v���e�N�g��
			160�F�p���e�B�G���[�̂��߃A�{�[�g
			161�F�t���[�~���O�G���[�̂��߃A�{�[�g
			162�F�I�[�o�[�����G���[�̂��߃A�{�[�g
			163�FFCS �G���[�̂��߃A�{�[�g
			164�F�t�H�[�}�b�g�G���[�̂��߃A�{�[�g
			165�F�u���f�[�^�G���[�̂��߃A�{�[�g
			168�F�t���[���� MAX �G���[�̂��߃A�{�[�g
			176�F�v���O�����G���A�� 16KB �ȊO�̂��ߎ��s�s��
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
	TRACE( "CSIO_SeqCntl::GetSeqAnswer ==========> �J�n\n" );
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
	TRACE( "CSIO_SeqCntl::GetSeqAnswer ==========> �I��\n" );
#endif	_SEQ_WATCH_

	return ret_code;
}

/*---------------------------------------------------------*
	���O�F CalcFCS
	�T�v�F FCS���v�Z����
	�����F char *str		(IN)
			���M���镶����
	�ߒl�F UCHAR
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
	���O�F CheckFCS
	�T�v�F ��M�����f�[�^��FCS���`�F�b�N������
	�����F char *str		(IN)
			��M�����f�[�^
	�ߒl�F int
			�O�F����
			�P�F�ُ�
 *---------------------------------------------------------*/
BOOL	CSIO_SeqCntl::CheckFCS( char *str )
{
	int 	fcs;
	UCHAR	fcs1, fcs2;
	char	fcs_data[5];		/* (FCS 2byte) + (terminator 2byte) + (NULL 1byte) */


	/* ��M�����񒆂� FCS �����̎��o�� */
	sprintf(fcs_data, "%4s", str + (strlen(str) - 4));

	fcs1 = CalcFCS(str);
	fcs2 = CalcFCS(fcs_data);
	fcs1 ^= fcs2;

	sscanf(fcs_data, "%2x", &fcs);

	return (BOOL)(fcs1!=fcs);
}

//
// �ʐM�|�[�g����������(CIO�^�O�ݒ�)
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

	// CIO�ݒ�
	strLocal = "LOCAL$IO.IO";
	strValue = "$VALUE";
	nCh = JW_CIO_FAST_CH;

	for(i=j=0; i<JW_CIO_TAG_MAX; i++ )
	{
		strCh.Format( "%d" , nCh );
		strBit.Format( "_%02d", j);
		strTag = strLocal + strCh + strBit + strValue;
		// CIO�^�O���̐ݒ�
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

	nRet = JWGetTagIDS2( JW_CIO_TAG_MAX,	// �ϊ��^�O��
					pJwCIOTags,			// �^�O���ւ̃|�C���^
					256,				// �^�O�\���̂̃��R�[�h�T�C�Y�@����̓^�O�������Ȃ̂Ł@256�P�ʂ̕�����
					0,					// �^�O���\���̂́@�^�O���ʒu
					m_pJwCIOIDs,		// �ϊ���̃^�OID
					sizeof( long ),		// �^�O�h�c�\���̂̃T�C�Y
					0, 
					-1, 
					-1 );
	// PLC�f�[�^�ǂݍ��ݎ��ɕK�v
	for(i=0; i<JW_CIO_TAG_MAX; i++ )
	{
		tCIO_Data[i].col_id = m_pJwCIOIDs[i];
	}

	if( nRet == 1 )
	{
		// Monitor(W0100~017F)�̐ݒ�
		strLocal = "LOCAL$MONITORDATA.T";
		strValue = "$VALUE";

		for(i=0; i<JW_MONITOR_TAG_MAX; i++ )
		{
			strCh.Format( "%03d" , i );
			strTag = strLocal + strCh + strValue;
			// Monitor(W0100~017F)�^�O���̐ݒ�
			strcpy( pJwMonitorTags[i].name, strTag);
		}

		nRet = JWGetTagIDS2( JW_MONITOR_TAG_MAX,		// �ϊ��^�O��
						pJwMonitorTags,			// �^�O���ւ̃|�C���^
						256,				// �^�O�\���̂̃��R�[�h�T�C�Y�@����̓^�O�������Ȃ̂Ł@256�P�ʂ̕�����
						0,					// �^�O���\���̂́@�^�O���ʒu
						m_pJwMonitorIDs,		// �ϊ���̃^�OID
						sizeof( long ),		// �^�O�h�c�\���̂̃T�C�Y
						0, 
						-1, 
						-1 );
		// PLC�f�[�^�ǂݍ��ݎ��ɕK�v
		for(i=0; i<JW_MONITOR_TAG_MAX; i++ )
		{
			tMonitor_Data[i].col_id = m_pJwMonitorIDs[i];
		}
	}

	if( nRet == 1 )
	{
		// DataSet(W0030~007F))�̐ݒ�
		strLocal = "LOCAL$DATASET.T";
		strValue = "$VALUE";

		for(i=0; i<JW_DATASET_TAG_MAX; i++ )
		{
			strCh.Format( "%02d" , i );
			strTag = strLocal + strCh + strValue;
			// DataSet(W0030~007F)�^�O���̐ݒ�
			strcpy( pJwDataSetTags[i].name, strTag);
		}

		nRet = JWGetTagIDS2( JW_DATASET_TAG_MAX,		// �ϊ��^�O��
						pJwDataSetTags,			// �^�O���ւ̃|�C���^
						256,				// �^�O�\���̂̃��R�[�h�T�C�Y�@����̓^�O�������Ȃ̂Ł@256�P�ʂ̕�����
						0,					// �^�O���\���̂́@�^�O���ʒu
						m_pJwDataSetIDs,		// �ϊ���̃^�OID
						sizeof( long ),		// �^�O�h�c�\���̂̃T�C�Y
						0, 
						-1, 
						-1 );
		// PLC�f�[�^�ǂݍ��ݎ��ɕK�v
		for(i=0; i<JW_DATASET_TAG_MAX; i++ )
		{
			tDataSet_Data[i].col_id = m_pJwDataSetIDs[i];
		}
	}

	if( nRet == 1 )
	{
		// STEP(W0010~003F)�̐ݒ�
		strLocal = "LOCAL$STEPDATA.T";
		strValue = "$VALUE";

		for(i=0; i<JW_STEP_TAG_MAX; i++ )
		{
			strCh.Format( "%02d" , i );
			strTag = strLocal + strCh + strValue;
			// STEP(W0010~003F)�^�O���̐ݒ�
			strcpy( pJwStepTags[i].name, strTag);
		}


		nRet = JWGetTagIDS2( JW_STEP_TAG_MAX,		// �ϊ��^�O��
						pJwStepTags,			// �^�O���ւ̃|�C���^
						256,				// �^�O�\���̂̃��R�[�h�T�C�Y�@����̓^�O�������Ȃ̂Ł@256�P�ʂ̕�����
						0,					// �^�O���\���̂́@�^�O���ʒu
						m_pJwStepIDs,		// �ϊ���̃^�OID
						sizeof( long ),		// �^�O�h�c�\���̂̃T�C�Y
						0, 
						-1, 
						-1 );
//		// PLC�f�[�^�ǂݍ��ݎ��ɕK�v
//		for(i=0; i<JW_STEP_TAG_MAX; i++ )
//		{
//			tStep_Data[i].col_id = m_pJwStepIDs[i];
//		}
	}

	if( nRet == 1 )
	{
		// WorkSel(W0000~000F)�̐ݒ�
		strLocal = "LOCAL$WORKSEL.T";
		strValue = "$VALUE";

		for(i=0; i<JW_WORK_TAG_MAX; i++ )
		{
			strCh.Format( "%02d" , i );
			strTag = strLocal + strCh + strValue;
			// DataSet(W0000~000F)�^�O���̐ݒ�
			strcpy( pJwWorkSelTags[i].name, strTag);
		}

		nRet = JWGetTagIDS2( JW_WORK_TAG_MAX,		// �ϊ��^�O��
						pJwWorkSelTags,			// �^�O���ւ̃|�C���^
						256,				// �^�O�\���̂̃��R�[�h�T�C�Y�@����̓^�O�������Ȃ̂Ł@256�P�ʂ̕�����
						0,					// �^�O���\���̂́@�^�O���ʒu
						m_pJwWorkSelIDs,		// �ϊ���̃^�OID
						sizeof( long ),		// �^�O�h�c�\���̂̃T�C�Y
						0, 
						-1, 
						-1 );
		// PLC�f�[�^�ǂݍ��ݎ��ɕK�v
		for(i=0; i<JW_WORK_TAG_MAX; i++ )
		{
			tWorkSel_Data[i].col_id = m_pJwWorkSelIDs[i];
		}
	}

	return nRet;
}
