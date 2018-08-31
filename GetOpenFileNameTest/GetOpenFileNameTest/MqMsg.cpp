#include "stdafx.h"
#include "MqMsg.h"

MqMsg::MqMsg()
{
}
MqMsg::MqMsg(operationEnum operation, string msg,int data)
{
	this->operation = operation;
	this->msg = msg;
	this->data = data;
}
MqMsg::~MqMsg()
{
}

 