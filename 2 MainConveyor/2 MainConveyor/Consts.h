#pragma once
#include "stdafx.h"

const int N = 5;

const std::string StrType[] = { "0","1","2" };

const int SpinsInPlaquet = 4;

//in pair case 2 matrixes for Q and J interactions; in quatro case 2 matrixes for 2 terms
const int OrtsAmount = 3;

const bool DEBUG = true;

const int indexOfInsideJOperator = 2;

const int DiffStates = 16; //����������� ��������� ����������� ���������, ����� ������� ������
const int PairStates = DiffStates*DiffStates;

const int MaxIntElem = 4; // ���������� ��������� � ��������� ��������������
const int Namount = 576;   //���������� ��������� ground-��������� 6
const int ResAmount = 45; //���-�� ��������� J �������� � 8 �������


