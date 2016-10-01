#include "MauBinhScreen.h"
#include "MauBinhPlayer.h"


MauBinhPlayer::MauBinhPlayer()
{
	ui = nullptr;
}

MauBinhPlayer::~MauBinhPlayer()
{
	if (ui)
	{
		ui->isUsing = false;
	}
}

MauBinhPlayer::MauBinhPlayer(Player* p)
{
	CopyValue(p);
	ui = nullptr;
}

void MauBinhPlayer::reset()
{
	if (ui) ui->isUsing = false;
}